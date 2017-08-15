#include "devices.h"
/*
#include <string.h>
#include <stdlib.h>
*/

#define CALIB_FONT_HEIGHT (8/*TODO: magic number*/)


typedef struct {
	int white;
	int black;
	int gyro;
	int gray;
}Calibrate;

Calibrate calibrateValue;
int isCalibrate = 0;

/**
* センサー、モーターの接続を定義します
*/
static const sensor_port_t
touch_sensor = EV3_PORT_1,
sonar_sensor = EV3_PORT_2,
color_sensor = EV3_PORT_3,
gyro_sensor = EV3_PORT_4;

static const motor_port_t
left_motor = EV3_PORT_C,
right_motor = EV3_PORT_B,
tail_motor = EV3_PORT_A;




void Devices_calibrate() {
	isCalibrate = 1;

	/* センサー入力ポートの設定 */
	ev3_sensor_config(sonar_sensor, ULTRASONIC_SENSOR);
	ev3_sensor_config(color_sensor, COLOR_SENSOR);
	ev3_color_sensor_get_reflect(color_sensor); /* 反射率モード */
	ev3_sensor_config(touch_sensor, TOUCH_SENSOR);
	ev3_sensor_config(gyro_sensor, GYRO_SENSOR);
	/* モーター出力ポートの設定 */
	ev3_motor_config(left_motor, LARGE_MOTOR);
	ev3_motor_config(right_motor, LARGE_MOTOR);
	ev3_motor_config(tail_motor, LARGE_MOTOR);
	/* 尻尾センサリセット (尻尾が一番上まで戻ってない可能性がある)*/
	ev3_motor_reset_counts(tail_motor);
	//尻尾が一番上まで来るようにする
	Devices_controlDevice(MOTOR_TAIL_,TAIL_ANGLE_MINIMUM);
	/* 尻尾センサリセット (尻尾が一番上の状態)*/
	ev3_motor_reset_counts(tail_motor);

	/* ジャイロセンサーリセット */
	ev3_gyro_sensor_reset(gyro_sensor);
	balance_init(); /* 倒立振子API初期化 */


	ev3_lcd_draw_string("EV3way-ET balance", 0, CALIB_FONT_HEIGHT * 1);
	ev3_lcd_draw_string("         GYRO Calibrate", 0, CALIB_FONT_HEIGHT * 2);
	ev3_lcd_draw_string("         WHITE Calibrate", 0, CALIB_FONT_HEIGHT * 3);
	ev3_lcd_draw_string("         BLACK Calibrate", 0, CALIB_FONT_HEIGHT * 4);
	ev3_lcd_draw_string("         GRAY Calibrate", 0, CALIB_FONT_HEIGHT * 5);
	tslp_tsk(100);

	// ジャイロ
	while (1) {
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP); // 完全停止用角度に制御
		if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1){
			break; // タッチセンサが押された
		}
	}
	calibrateValue.gyro = Devices_getDeviceValue(SENSOR_GYRO_);
	ev3_lcd_draw_string("Complete GYRO Calibrate", 0, CALIB_FONT_HEIGHT * 2);
	while (1) {
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP); // 完全停止用角度に制御
		if (Devices_getDeviceValue(SENSOR_TOUCH_) == 0){
			break; // タッチセンサから指が離れた
		}
	}

	// 白色
	while (1) {
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP);
		if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1) {
			break;
		}
		//tslp_tsk(40);
	}
	calibrateValue.white = Devices_getDeviceValue(SENSOR_COLOR_);
	ev3_lcd_draw_string("Complete WHITE Calibrate", 0, CALIB_FONT_HEIGHT * 3);
	while (1) {
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP); // 完全停止用角度に制御
		if (Devices_getDeviceValue(SENSOR_TOUCH_) == 0){
			break; // タッチセンサから指が離れた
		}
	}

	// 黒色
	while (1) {
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP);
		if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1) {
			break;
		}
	}
	calibrateValue.black = Devices_getDeviceValue(SENSOR_COLOR_);
	ev3_lcd_draw_string("Complete BLACK Calibrate", 0, CALIB_FONT_HEIGHT * 4);
	while (1) {
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP); // 完全停止用角度に制御
		if (Devices_getDeviceValue(SENSOR_TOUCH_) == 0){
			break; // タッチセンサから指が離れた
		}
	}

	// 灰色
	while (1) {
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP);
		if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1) {
			break;
		}
	}
	calibrateValue.gray = Devices_getDeviceValue(SENSOR_COLOR_);
	ev3_lcd_draw_string("Complete GRAY Calibrate", 0, CALIB_FONT_HEIGHT * 5);
	while (1) {
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP); // 完全停止用角度に制御
		if (Devices_getDeviceValue(SENSOR_TOUCH_) == 0){
			break; // タッチセンサから指が離れた
		}
	}

	char mozi[50] = {};
	sprintf(mozi, "         WHITE = %4d", Devices_getCalibrateValue(WHITE_CALIBRATE));
	ev3_lcd_draw_string(mozi, 0, CALIB_FONT_HEIGHT * 8);

	sprintf(mozi, "         BLACK = %4d", Devices_getCalibrateValue(BLACK_CALIBRATE));
	ev3_lcd_draw_string(mozi, 0, CALIB_FONT_HEIGHT * 9);

	sprintf(mozi, "    threshiold = %4d", (Devices_getCalibrateValue(BLACK_CALIBRATE)+
										Devices_getCalibrateValue(WHITE_CALIBRATE)) /2 );
	ev3_lcd_draw_string(mozi, 0, CALIB_FONT_HEIGHT * 10);

	sprintf(mozi, "          GRAY = %4d", Devices_getCalibrateValue(GRAY_CALIBRATE));
	ev3_lcd_draw_string(mozi, 0, CALIB_FONT_HEIGHT * 11);
}


void Devices_controlDevice( int deviceNo, int value) {
	float pwm;
	switch (deviceNo) {
		case MOTOR_RIGHT_:

			if (value == 0) {
				ev3_motor_stop(right_motor, true);
			}
			else{
				ev3_motor_set_power(right_motor, (int)value);
			}

			break;


		case MOTOR_LEFT_:

			if (value == 0)	{
				ev3_motor_stop(left_motor, true);
			}
			else{
				ev3_motor_set_power(left_motor, (int)value);
			}

			break;


		case MOTOR_TAIL_:
			if(value == TAIL_ANGLE_MINIMUM){//尻尾を一番上まで持ってくる(一度しか使われない)
				int tail_angle = 0 , tail_angle_old = 0;
				ev3_motor_set_power(tail_motor, -10);
				do{
					tail_angle_old = tail_angle;
					tslp_tsk(500);//モーターが動く時間を作る
					tail_angle = Devices_getDeviceValue(MOTOR_TAIL_);
				}while((tail_angle_old - tail_angle) != 0);//尻尾が動いている場合はまだ動かす
				ev3_motor_stop(tail_motor, true);
				break;
			}

			pwm = (float)(value - Devices_getDeviceValue(MOTOR_TAIL_));
			if(pwm > 3 || pwm <-3){
				if(pwm > 3){
				pwm = pwm*pwm;
				}else{
				pwm = -pwm*pwm;
				}
			}
			if (pwm > PWM_ABS_MAX){
				pwm = PWM_ABS_MAX;
			}
			else if (pwm < -PWM_ABS_MAX){
				pwm = -PWM_ABS_MAX;
			}

			if (pwm ==0){
				ev3_motor_stop(tail_motor, true);
			}
			else{
				ev3_motor_set_power(tail_motor, (signed char)pwm);
			}
			break;

		case MONITOR_:
			switch (value){
				case -1:
					ev3_lcd_draw_string("Waiting Start AND press touch sensor", 0, CALIB_FONT_HEIGHT * 6);
					break;

				case 0:
					ev3_lcd_draw_string("Running STOP mode                   ", 0, CALIB_FONT_HEIGHT * 6);
					break;

				case 1:
					ev3_lcd_draw_string("Running BALANCE mode                ", 0, CALIB_FONT_HEIGHT * 6);
					break;

				case 2:
					ev3_lcd_draw_string("Running LINE_TRACE mode             ", 0, CALIB_FONT_HEIGHT * 6);
					break;
			}
			
	}
	return;
}


int  Devices_getCalibrateValue(int deviceNo) {
	if (isCalibrate == 0)Devices_calibrate();
	
	switch (deviceNo) {
		case GYRO_SENSOR:
			return calibrateValue.gyro;

		case BLACK_CALIBRATE:
			return calibrateValue.black;

		case WHITE_CALIBRATE:
			return calibrateValue.white;
		
		case GRAY_CALIBRATE:
			return calibrateValue.gray;
	}

	return -1;
}


int  Devices_getDeviceValue(int deviceNo) {
	switch (deviceNo) {
		case SENSOR_COLOR_:
			return ev3_color_sensor_get_reflect(color_sensor);

		case SENSOR_SONER_:
			return ev3_ultrasonic_sensor_get_distance(sonar_sensor);

		case SENSOR_TOUCH_:
			if (ev3_touch_sensor_is_pressed(touch_sensor)) {
				while (ev3_touch_sensor_is_pressed(touch_sensor));
				return  1;
			}
			return 0;
			
		case SENSOR_GYRO_:
			return ev3_gyro_sensor_get_rate(gyro_sensor);

		case MOTOR_RIGHT_:
			return ev3_motor_get_counts(right_motor);

		case MOTOR_LEFT_:
			return ev3_motor_get_counts(left_motor);

		case MOTOR_TAIL_:
			return ev3_motor_get_counts(tail_motor);

		case BATTERY_:
			return ev3_battery_voltage_mV();
	}

	return -1;
}

