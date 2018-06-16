/*
 PCA9685 driver code is placed under the BSD license.
 Written by Mikhail Avkhimenia (mikhail.avkhimenia@emlid.com)
 Copyright (c) 2014, Emlid Limited
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of the Emlid Limited nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL EMLID LIMITED BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PCA9685_2_HPP
#define PCA9685_2_HPP

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include "../Common/I2Cdev.h"
#include "PCA9685.h"

#define PCA9685_2_DEFAULT_ADDRESS     0x41 // All address pins low, Navio default

class PCA9685_2 {
public:
	PCA9685_2(uint8_t address = PCA9685_2_DEFAULT_ADDRESS);

	void initialize();
	bool testConnection();

	float getFrequency();
	void setFrequency(float frequency);

	void sleep();
	void restart();

	void setPWM(uint8_t channel, uint16_t offset, uint16_t length);
	void setPWM(uint8_t channel, uint16_t length);
//	void setPWMmS(uint8_t channel, float length_mS);
//	void setPWMuS(uint8_t channel, float length_uS);

	void setAllPWM(uint16_t offset, uint16_t length);
	void setAllPWM(uint16_t length);
//	void setAllPWMmS(float length_mS);
//	void setAllPWMuS(float length_uS);

private:
	uint8_t devAddr;
	float frequency;
};

#endif // PCA9685_2_HPP
