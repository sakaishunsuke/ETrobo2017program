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
	/* 尻尾センサリセット */
	ev3_motor_reset_counts(tail_motor);

	/* ジャイロセンサーリセット */
	ev3_gyro_sensor_reset(gyro_sensor);
	balance_init(); /* 倒立振子API初期化 */


	ev3_lcd_draw_string("EV3way-ET prototype_c", 0, CALIB_FONT_HEIGHT * 1);
	ev3_lcd_draw_string("         GYRO Calibrate", 0, CALIB_FONT_HEIGHT * 2);
	ev3_lcd_draw_string("         WHITE Calibrate", 0, CALIB_FONT_HEIGHT * 3);
	ev3_lcd_draw_string("         BLACK Calibrate", 0, CALIB_FONT_HEIGHT * 4);
	ev3_lcd_draw_string("         GRAY Calibrate", 0, CALIB_FONT_HEIGHT * 5);
	tslp_tsk(1000);

	/*
	char mozi[50] = {};
	int a;
	strcpy(mozi, "wwwww");
	while (1) {
		if (Devices_getDeviceValue(SENSOR_TOUCH_)==false) {
			ev3_lcd_draw_string("         SENSOR_TOUCH is TRUE", 0, CALIB_FONT_HEIGHT * 6);
		}
		else {
			ev3_lcd_draw_string("         SENSOR_TOUCH is FALSE", 0, CALIB_FONT_HEIGHT * 6);
		}

		if (ev3_ultrasonic_sensor_get_distance(sonar_sensor) < 15) {
			ev3_lcd_draw_string("         SONAR is TRUE", 0, CALIB_FONT_HEIGHT * 8);
		}
		else {
			ev3_lcd_draw_string("         SONAR is FALSE", 0, CALIB_FONT_HEIGHT * 8);
		}if (ev3_touch_sensor_is_pressed(touch_sensor) == true) {
			a = 1;
		}
		else a = 2;
		sprintf(mozi, "sensor_touch is %d", a );
		ev3_lcd_draw_string(mozi, 0, CALIB_FONT_HEIGHT * 10);
	}
	*/

	// ジャイロ
	while (1) {
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP); // 完全停止用角度に制御
		if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1){
			break; // タッチセンサが押された
		}
		//tslp_tsk(40);
	}
	calibrateValue.gyro = Devices_getDeviceValue(SENSOR_GYRO_);
	ev3_lcd_draw_string("Complete GYRO Calibrate", 0, CALIB_FONT_HEIGHT * 2);
	tslp_tsk(1000); // 1s停止

/*
	// 白色
	while (1) {
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP);
		if (Devices_getDeviceValue(SENSOR_TOUCH_) == 0) {
			break;
		}
		//tslp_tsk(40);
	}
	calibrateValue.white = Devices_getDeviceValue(SENSOR_COLOR_);
	ev3_lcd_draw_string("Complete WHITE Calibrate", 0, CALIB_FONT_HEIGHT * 3);
	tslp_tsk(1000);

	// 黒色
	while (1) {
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP);
		if (Devices_getDeviceValue(SENSOR_TOUCH_) == 0) {
			break;
		}
		//tslp_tsk(40);
	}
	calibrateValue.black = Devices_getDeviceValue(SENSOR_COLOR_);
	ev3_lcd_draw_string("Complete BLACK Calibrate", 0, CALIB_FONT_HEIGHT * 4);
	tslp_tsk(1000);
	// 灰色
	while (1) {
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP);
		if (Devices_getDeviceValue(SENSOR_TOUCH_) == 0) {
			break;
		}
		//tslp_tsk(40);
	}
*/
	calibrateValue.gray = Devices_getDeviceValue(SENSOR_COLOR_);
	ev3_lcd_draw_string("Complete GRAY Calibrate", 0, CALIB_FONT_HEIGHT * 5);
	tslp_tsk(1000);

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

			pwm = (float)(value - Devices_getDeviceValue(MOTOR_TAIL_))*P_GAIN;
			if (pwm > PWM_ABS_MAX){
				pwm = PWM_ABS_MAX;
			}
			else if (pwm < -PWM_ABS_MAX){
				pwm = -PWM_ABS_MAX;
			}

			if (pwm == 0){
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
			/*
			if (ev3_button_is_pressed(UP_BUTTON)) {
				while (ev3_button_is_pressed(UP_BUTTON));
				return  1;
			}
			*/
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

