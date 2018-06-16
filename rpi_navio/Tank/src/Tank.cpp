//============================================================================
// Name        : Tank.cpp
// Author      : Helios
// Version     :
// Copyright   : No Copyright
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <cstdlib>
#include <string>
#include <cstring>
#include <cctype>
#include <thread>
#include <chrono>
#include <unistd.h>
#include "Navio/Navio+/PCA9685_2.h"
#include "Navio/Common/gpio.h"
#include "Navio/Common/Util.h"
#include "Navio/Common/Ublox.h"
#include "Navio/Common/MPU9250.h"
#include "json/json.hpp"
#include <unistd.h>
#include <memory>
#include <algorithm>
#include <iostream>
#include <mqtt/async_client.h>

namespace {

const std::string TOPIC_PWM = "/tank/pwm";

}
using namespace Navio;
using namespace std;
using json = nlohmann::json;

#define SERVO_MIN 0 /* pwm / 100 hz*/
#define SERVO_MAX 4095 /*pwm / 100 hz*/
#define SERVO_DEFAULT 614 /*default pwm / 100 hz https://hk.saowen.com/a/619eb39463b8518b8984b4da9dafb646554fe53cd512b68750a5ea80919fc585*/
#define SERVO_FREQUENCY 100

class user_callback: public virtual mqtt::callback {
	void connection_lost(const std::string& cause) override {
		std::cout << "\nConnection lost" << std::endl;
		if (!cause.empty())
			std::cout << "\tcause: " << cause << std::endl;
	}

	void delivery_complete(mqtt::delivery_token_ptr tok) override {
		std::cout << "\n\t[Delivery complete for token: " << (tok ? tok->get_message_id() : -1) << "]" << std::endl;
	}

public:
};

int main(int argc, char *argv[]) {
	if (check_apm()) {
		cout << "check_apm failed " << endl;
		return 1;
	}
	mqtt::async_client client("tcp://192.168.1.14:1883", "TankPi");
	cout << "Client created " << endl;

	user_callback cb;
	client.set_callback(cb);

	mqtt::connect_options connOpts;
	connOpts.set_keep_alive_interval(20);
	connOpts.set_clean_session(true);
	client.connect(connOpts)->wait();
	cout << "Client connected " << endl;

	client.start_consuming();
	cout << "Client started consuming " << endl;

	client.subscribe(TOPIC_PWM, 0)->wait();
	cout << "Client subscribed " << endl;

//	static const uint8_t outputEnablePin = RPI_GPIO_27;
//	Pin pin(outputEnablePin);
//	if (pin.init()) {
//		pin.setMode(Pin::GpioModeOutput);
//		pin.write(0); /* drive Output Enable low */
//	}

	PCA9685_2 pwm; //externes 0x40 0x41
	if (!pwm.testConnection()) {
		cout << "PWM connection unsucessful " << endl;
		return 1;
	}
	pwm.setFrequency(SERVO_FREQUENCY);
	pwm.setAllPWM(SERVO_DEFAULT);
	pwm.initialize();
	usleep(100000);
	pwm.setFrequency(SERVO_FREQUENCY);
	usleep(100000);
	pwm.setAllPWM(SERVO_DEFAULT);

	cout << "PWM initialized " << endl;

	// This vector is used to store location data, decoded from ubx messages.
	// After you decode at least one message successfully, the information is stored in vector
	// in a way described in function decodeMessage(vector<double>& data) of class UBXParser(see ublox.h)

	std::vector<double> pos_data;

	// create ublox class instance
	Ublox gps;
	if (!gps.testConnection()) {
		cout << "gps.testConnection()" << endl;
		return 1;
	}
	cout << "GPS connected " << endl;

	MPU9250 mpu;
	cout << "mpu created " << endl;
	mpu.initialize();
	cout << "mpu initialized " << endl;
	float ax, ay, az;
	float gx, gy, gz;
	float mx, my, mz;

	json gpsJson;
	json mpuJson;
	cout << "Start loop " << endl;
	while (true) {
		mpu.update();
		mpu.read_accelerometer(&ax, &ay, &az);
		mpu.read_gyroscope(&gx, &gy, &gz);
		mpu.read_magnetometer(&mx, &my, &mz);
		mpuJson["ax"] = ax;
		mpuJson["ay"] = ay;
		mpuJson["az"] = az;
		mpuJson["gx"] = gx;
		mpuJson["gy"] = gy;
		mpuJson["gz"] = gz;
		mpuJson["mx"] = mx;
		mpuJson["my"] = my;
		mpuJson["mz"] = mz;
		client.publish(mqtt::make_message("/tank/mpu", mpuJson.dump()));

		if (gps.decodeSingleMessage(Ublox::NAV_POSLLH, pos_data) == 1) {
			gpsJson["time"] = pos_data[0] / 1000;	//"GPS Millisecond Time of Week: %.0lf s\n
			gpsJson["longitude"] = pos_data[1] / 10000000;
			gpsJson["latitude"] = pos_data[2] / 10000000;
			gpsJson["height_ellipsoid"] = pos_data[3] / 1000;
			gpsJson["height_sealevel"] = pos_data[4] / 1000;
			gpsJson["horizontal_accuracy"] = pos_data[5] / 1000;
			gpsJson["vertival_accuracy"] = pos_data[6] / 1000;
			client.publish(mqtt::make_message("/tank/gps", gpsJson.dump()));
		}
		mqtt::const_message_ptr msg;
		if (client.try_consume_message(&msg)) {
			if (TOPIC_PWM.compare(msg->get_topic()) == 0) {
				cout << "got on topic: " << msg->get_topic() << " message: " << msg->to_string() << endl;
				auto pwmJson = json::parse(msg->to_string());
				for (json::iterator it = pwmJson.begin(); it != pwmJson.end(); ++it) {
					auto value = it.value();
					int channel = stoi(it.key());
					int pwmValue = max(min(SERVO_MAX, (int) value), SERVO_MIN);
					pwm.setPWM(channel, pwmValue);
					cout << "pwm channel " << channel << " ms: " << pwmValue << endl;
				}
			} else {
				cout << "got on topic: " << msg->get_topic() << " message: " << msg->to_string() << endl;
			}
		}
		//todo different loop-times
		usleep(500000);
	}

	return 0;
}
