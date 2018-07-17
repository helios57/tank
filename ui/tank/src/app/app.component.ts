import {Component, OnDestroy} from '@angular/core';
import {Subject, Subscription} from 'rxjs';
import {IMqttMessage, MqttService} from 'ngx-mqtt';


@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent implements OnDestroy {
  private $mpu: Subscription;
  private $gps: Subscription;
  public mpu: string;
  public gps: string;

  constructor(private _mqttService: MqttService) {
    this.$mpu = this._mqttService.observe('/tank/mpu').subscribe((message: IMqttMessage) => {
      this.mpu = message.payload.toString();
      console.log(this.mpu);
    });
    this.$gps = this._mqttService.observe('/tank/gps').subscribe((message: IMqttMessage) => {
      this.gps = message.payload.toString();
      console.log(this.mpu);
    });
  }


  private _destroy$ = new Subject<void>();
  title = 'Tank';
  s10 = {
    min: 320,
    max: 620,
    value: 520
  };
  s9 = {
    min: 120,
    max: 900,
    value: 520
  };
  s8 = {
    min: 120,
    max: 900,
    value: 490
  };
  s6 = {
    min: 120,
    max: 900,
    value: 510
  };
  s5 = {
    min: 120,
    max: 900,
    value: 520
  };
  s4 = {
    min: 120,
    max: 900,
    value: 500
  };

  private unsafePublish(topic: string, message: string): void {
    this._mqttService.unsafePublish(topic, message, {qos: 0, retain: true});
  }

  onInputChange(value: number, slider: string) {
    console.log('s' + slider, value, this['s' + slider].value);
    this['s' + slider].value = value;
    this.unsafePublish('/tank/pwm', '{"' + slider + '": ' + value + '}');
  }

  ngOnDestroy(): void {
    this.$mpu.unsubscribe();
    this.$gps.unsubscribe();
    this._destroy$.next();
  }
}
