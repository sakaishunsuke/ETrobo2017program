#include "lineTraceRun.h"

int threshiold;
int gyroOffset;

int BalanceRun_run() {
	static signed char forward = -5;      /* �O��i���� */
	signed char turn = 0;     /* ���񖽗� */

	signed char pwm_L, pwm_R; /* ���E���[�^PWM�o�� */

	int32_t motor_ang_l, motor_ang_r;
	int gyro, volt;

	//if (ev3_button_is_pressed(BACK_BUTTON)) break;

	Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_DRIVE); /* �o�����X���s�p�p�x�ɐ��� */

	//forward = 20; /* �O�i���� */
	if (forward > 50)
		forward = 50;  /* ��~���� */
	else
		forward += 3;

	/* �|���U�q����API �ɓn���p�����[�^���擾���� */
	motor_ang_l = Devices_getDeviceValue(MOTOR_LEFT_);
	motor_ang_r = Devices_getDeviceValue(MOTOR_RIGHT_);
	gyro = Devices_getDeviceValue(SENSOR_GYRO_);
	volt = Devices_getDeviceValue(BATTERY_);

	/* �|���U�q����API���Ăяo���A�|�����s���邽�߂� */
	/* ���E���[�^�o�͒l�𓾂� */
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

	/* EV3�ł̓��[�^�[��~���̃u���[�L�ݒ肪���O�ɂł��Ȃ����� */
	/* �o��0���ɁA���̓s�x�ݒ肷�� */
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