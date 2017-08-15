#include "lineTraceRun.h"
#include "grayJudgment.h"
#include "PID.h"

PID pid;

int threshiold;
int gyroOffset;

int LineTraceRun_run() {
	static signed char forward = 10;      /* �O��i���� */
	signed int turn;         /* ���񖽗� */
	signed char pwm_L, pwm_R; /* ���E���[�^PWM�o�� */

	int32_t motor_ang_l, motor_ang_r;
	int gyro, volt;

	//if (ev3_button_is_pressed(BACK_BUTTON)) break;

	Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_DRIVE); /* �o�����X���s�p�p�x�ɐ��� */

	turn = PID_getTurn(&pid, Devices_getDeviceValue(SENSOR_COLOR_) - threshiold, -1);//�Ō�̈����̓G�b�W�A�E�F-1�A���F1

	/* �O�i���� */
	//forward = 100 - (turn >= 0 ? turn : -turn);
	if (forward < 75)
		forward++;
	
	if (Devices_getDeviceValue(SENSOR_COLOR_) >= threshiold) {
		turn = -15; /* �����񖽗� */
	}else {
		turn = 15; /* �E���񖽗� */
	}
	

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

	char mozi[50] = {};//���E�̉�]���\��
	sprintf(mozi, "motor_L = %4d motor_R = %4d",motor_ang_l, motor_ang_r);
	ev3_lcd_draw_string(mozi, 0, 8 * 13);
	
	//if (GrayJudgment(Devices_getDeviceValue(SENSOR_COLOR_), motor_ang_l, motor_ang_r) == true)
	//	ev3_speaker_play_tone(NOTE_FS5, 10);

	if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1
		|| (pwm_L > 90 && pwm_R >90)
		|| (pwm_L <- 90 && pwm_R <-90) )
		return STOP;
	//return STOP;
	return LINE_TRACE_RUN;

}


void LineTraceRun_makeThreshiold() {
	threshiold = (Devices_getCalibrateValue(BLACK_CALIBRATE) + Devices_getCalibrateValue(WHITE_CALIBRATE))/2;
	gyroOffset = Devices_getCalibrateValue(SENSOR_GYRO_);
	GrayJudgment_makeThreshiold(threshiold,0, Devices_getCalibrateValue(GRAY_CALIBRATE));
	PID_initialize(&pid, 0);
}