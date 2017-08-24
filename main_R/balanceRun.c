#include "lineTraceRun.h"

int threshiold;
int gyroOffset;

int BalanceRun_run() {
	static signed char forward = -5;      /* 前後進命令 */
	signed char turn = 0;     /* 旋回命令 */

	signed char pwm_L, pwm_R; /* 左右モータPWM出力 */

	int32_t motor_ang_l, motor_ang_r;
	int gyro, volt;

	//if (ev3_button_is_pressed(BACK_BUTTON)) break;

	Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_DRIVE); /* バランス走行用角度に制御 */

	//forward = 20; /* 前進命令 */
	if (forward > 50)
		forward = 50;  /* 停止命令 */
	else
		forward += 3;

	/* 倒立振子制御API に渡すパラメータを取得する */
	motor_ang_l = Devices_getDeviceValue(MOTOR_LEFT_);
	motor_ang_r = Devices_getDeviceValue(MOTOR_RIGHT_);
	gyro = Devices_getDeviceValue(SENSOR_GYRO_);
	volt = Devices_getDeviceValue(BATTERY_);

	/* 倒立振子制御APIを呼び出し、倒立走行するための */
	/* 左右モータ出力値を得る */
	balance_control(
		(float)forward,
		(float)turn,
		(float)gyro,
		(float)gyroOffset,
		(float)motor_ang_l,
		(float)motor_ang_r,
		(float)volt,
		(signed char*)&pwm_L,
		(signed char*)&pwm_R);

	/* EV3ではモーター停止時のブレーキ設定が事前にできないため */
	/* 出力0時に、その都度設定する */
	Devices_controlDevice(MOTOR_LEFT_, pwm_L);
	Devices_controlDevice(MOTOR_RIGHT_, pwm_R);

	if (Devices_getDeviceValue(SENSOR_COLOR_) < threshiold ) {
		return (int)LINE_TRACE_RUN;

	}
	if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1)
		return (int)STOP;

	return (int)BALANCE_RUN;

}


void BalanceRun_makeThreshiold() {
	threshiold = (Devices_getCalibrateValue(BLACK_CALIBRATE) + Devices_getCalibrateValue(WHITE_CALIBRATE)) / 2;
	gyroOffset = Devices_getCalibrateValue(SENSOR_GYRO_);
}