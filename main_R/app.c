/**
 ******************************************************************************
 ** �t�@�C���� : app.c
 **
 ** �T�v : 2�֓|���U�q���C���g���[�X���{�b�g��TOPPERS/HRP2�pC�T���v���v���O����
 **
 ** ���L : sample_c4 (sample_c3��Bluetooth�ʐM�����[�g�X�^�[�g�@�\��ǉ�)
 ******************************************************************************
 **/

#include "ev3api.h"
#include <syssvc/serial.h>
#include <string.h>
#include "app.h"
#include "balancer.h"

#include "devices.h"

#include "lineTraceRun.h"
#include "runControl.h"

//#include "grayJudgment.h"//PC��log�𑗐M���邽��

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif




static int      bt_cmd = 0;     /* Bluetooth�R�}���h 1:�����[�g�X�^�[�g */
static FILE     *bt = NULL;     /* Bluetooth�t�@�C���n���h�� */

//�b��ł����ɏ����Ă�
#define SCENARIO_SIZE 10 /*�V�i���I�̐�*/
#define SCENARIO_KIND_SIZE 5 /*�V�i���I�̒��̎�ށ@PID�A�^�[����̎��ԁAmax�X�s�[�h*/
/*--- �V�i���I�̃f�[�^�ނ���������\���� ---*/
typedef struct {
	int  max;       /* �V�i���I�̐� */
	int  Number;       /* ���݂̃V�i���I�ԍ� */
	int  scenario[SCENARIO_SIZE][SCENARIO_KIND_SIZE];      /* �V�i���I�{��() */
} SCENARIO;
SCENARIO scenario;	/* �\���̂̎��̉� */
void scenarioInit(SCENARIO *s) {	/* �V�i���I�̏������֐� */
	s->max = 1;
	s->Number = 0;
	s->scenario[0][0] = 1;
	s->scenario[0][1] = 0;
	s->scenario[0][2] = 0;
	s->scenario[0][3] = 500;
	s->scenario[0][4] = 50;
}

/*--- BT�̃f�[�^�ނ���������\���� ---*/
typedef struct {
	bool_t	send_flag;			/* BT�Ńf�[�^��x�点��flag*/
	int		left_motor_value;	/* �����[�^�̉�]�� */
	int		right_motor_value;	/* �E���[�^�̉�]�� */
	int		running_state;		/* ���s�̏�ԁA���i0�A�E����1�A������-1 */
	bool_t	start_flag;			/* BT����̃X�^�[�g��flag */
	bool_t	stop_flag;			/* BT����̃X�g�b�v��flag */
} BT_DATA;
void btDataInit(BT_DATA *bt_data) {
	bt_data->send_flag = false;
	bt_data->left_motor_value = 0;
	bt_data->right_motor_value = 0;
	bt_data->running_state = 0;
	bt_data->start_flag = false;
	bt_data->stop_flag = false;
}
BT_DATA bt_data;	/* �\���̂̎��̉� */

/*BT�̐ݒ�̃}�N��(�b��)*/
#define BTLEN 10 /*BT�Ŏ󂯎�镶���̒���*/


/* ���L�̃}�N���͌�/���ɍ��킹�ĕύX����K�v������܂� */

/* sample_c1�}�N�� */
//#define GYRO_OFFSET  0          /* �W���C���Z���T�I�t�Z�b�g�l(�p���x0[deg/sec]��) */
//#define LIGHT_WHITE  40         /* ���F�̌��Z���T�l */
//#define LIGHT_BLACK  0          /* ���F�̌��Z���T�l */


/* sample_c2�}�N�� */
#define SONAR_ALERT_DISTANCE 30 /* �����g�Z���T�ɂ���Q�����m����[cm] */


/* sample_c3�}�N�� */
//#define TAIL_ANGLE_STAND_UP  93 /* ���S��~���̊p�x[�x] */
//#define TAIL_ANGLE_DRIVE      3 /* �o�����X���s���̊p�x[�x] */
//#define P_GAIN             2.5F /* ���S��~�p���[�^������W�� */
//#define PWM_ABS_MAX          60 /* ���S��~�p���[�^����PWM��΍ő�l */


/* sample_c4�}�N�� */
//#define DEVICE_NAME     "ET0"  /* Bluetooth�� hrp2/target/ev3.h BLUETOOTH_LOCAL_NAME�Őݒ� */
//#define PASS_KEY        "1234" /* �p�X�L�[    hrp2/target/ev3.h BLUETOOTH_PIN_CODE�Őݒ� */
#define CMD_START         '1'    /* �����[�g�X�^�[�g�R�}���h */

/* LCD�t�H���g�T�C�Y */
#define CALIB_FONT (EV3_FONT_SMALL)
#define CALIB_FONT_WIDTH (6/*TODO: magic number*/)
#define CALIB_FONT_HEIGHT (8/*TODO: magic number*/)

/* �֐��v���g�^�C�v�錾 */
static int sonar_alert(void);


/**
* �Z���T�[�A���[�^�[�̐ڑ����`���܂�
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




void init() {
	/* LCD��ʕ\�� */
	ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);

	/*�V�i���I�̏�����*/
	scenarioInit(&scenario);
	/* BT_DATA�̏����� */
	btDataInit(&bt_data);
	
	/* 
	ev3_sensor_config(sonar_sensor, ULTRASONIC_SENSOR);
	ev3_sensor_config(color_sensor, COLOR_SENSOR);
	ev3_color_sensor_get_reflect(color_sensor);
	ev3_sensor_config(touch_sensor, TOUCH_SENSOR);
	ev3_sensor_config(gyro_sensor, GYRO_SENSOR);
	ev3_motor_config(left_motor, LARGE_MOTOR);
	ev3_motor_config(right_motor, LARGE_MOTOR);
	ev3_motor_config(tail_motor, LARGE_MOTOR);
	ev3_motor_reset_counts(tail_motor);
	*/
	/*
	ev3_sensor_config(EV3_PORT_1, TOUCH_SENSOR);

	while (1) {
		if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1) {
			ev3_lcd_draw_string("         SENSOR_TOUCH is TRUE", 0, CALIB_FONT_HEIGHT * 6);
		}
		else {
			ev3_lcd_draw_string("         SENSOR_TOUCH is FALSE", 0, CALIB_FONT_HEIGHT * 6);
		}

		if (ev3_touch_sensor_is_pressed(EV3_PORT_1)) {
			ev3_lcd_draw_string("         SENSOR_TOUCH2 is TRUE", 0, CALIB_FONT_HEIGHT * 8);
		}
		else {
			ev3_lcd_draw_string("         SENSOR_TOUCH2 is FALSE", 0, CALIB_FONT_HEIGHT * 8);
		}
	}*/

	Devices_calibrate();
	Devices_controlDevice(MONITOR_, -1);
	RunControl_makeThreshiold();
}


/* ���C���^�X�N */
void main_task(intptr_t unused)
{
    /* ������ */
	init();
	/* Open Bluetooth file */
    bt = ev3_serial_open_file(EV3_SERIAL_BT);
    //assert(bt != NULL);
	/* Bluetooth�ʐM�^�X�N�̋N�� */
    act_tsk(BT_TASK);

    ev3_led_set_color(LED_ORANGE); /* �����������ʒm */

    while(1)
    {
		/* �X�^�[�g�ҋ@ */
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP); /* ���S��~�p�p�x�ɐ��� */

        if (bt_cmd == 1){
            break; /* �����[�g�X�^�[�g */
        }

		if(Devices_getDeviceValue(SENSOR_TOUCH_)==1){
            break; /* �^�b�`�Z���T�������ꂽ */
        }

        tslp_tsk(1); /* 10msec�E�F�C�g */
    }


	/* ���s���[�^�[�G���R�[�_�[���Z�b�g */
	ev3_motor_reset_counts(left_motor);
	ev3_motor_reset_counts(right_motor);

    ev3_led_set_color(LED_GREEN); /* �X�^�[�g�ʒm */

    /**
    * Main loop for the self-balance control algorithm
    */
    while(1){
		RunControl_run();
        tslp_tsk(4); /* 4msec�����N�� */
    }
    ev3_motor_stop(left_motor, false);
    ev3_motor_stop(right_motor, false);

    ter_tsk(BT_TASK);
    fclose(bt);

    ext_tsk();
}

//*****************************************************************************
// �֐��� : sonar_alert
// ���� : ����
// �Ԃ�l : 1(��Q������)/0(��Q������)
// �T�v : �����g�Z���T�ɂ���Q�����m
//*****************************************************************************
static int sonar_alert(void)
{
    static unsigned int counter = 0;
    static int alert = 0;

    signed int distance;

    if (++counter == 40/4) /* ��40msec�������ɏ�Q�����m  */
    {
        /*
         * �����g�Z���T�ɂ�鋗����������́A�����g�̌��������Ɉˑ����܂��B
         * NXT�̏ꍇ�́A40msec�������x���o����̍ŒZ��������ł��B
         * EV3�̏ꍇ�́A�v�m�F
         */
        distance = ev3_ultrasonic_sensor_get_distance(sonar_sensor);
        if ((distance <= SONAR_ALERT_DISTANCE) && (distance >= 0))
        {
            alert = 1; /* ��Q�������m */
        }
        else
        {
            alert = 0; /* ��Q������ */
        }
        counter = 0;
    }

    return alert;
}



//*****************************************************************************
// �֐��� : bt_task
// ���� : unused
// �Ԃ�l : �Ȃ�
// �T�v : Bluetooth�ʐM�ɂ�郊���[�g�X�^�[�g�B Tera Term�Ȃǂ̃^�[�~�i���\�t�g����A
//       ASCII�R�[�h��1�𑗐M����ƁA�����[�g�X�^�[�g����B
//*****************************************************************************
void bt_task(intptr_t unused)
{
    while(1)
    {
		/*PC�ɑ��M*/
		//fprintf(bt, "N_%1d", scenario.Number); /* ���݂̃V�i���I�ԍ���]�� */
		/*
		char buf[BTLEN];
		//serial_rea_dat(EV3_SERIAL_BT, buf, BTLEN);//BTLEN���̕������擾

		if (strstr(buf,"start") != NULL) {
			bt_cmd = 1;
		}
		else if (strstr(buf, "stop") != NULL) {
			bt_cmd = 0;
		}
		else if (strstr(buf,"M_") != NULL) {
			char *tok = strtok(buf,"M_");
			if (tok != NULL) {
				scenario.Number = tok[0] - '0';//���l���擾
				scenario.Number++;
			}
		}
		*/
		if (bt_cmd != 1) {
			uint8_t c = fgetc(bt); // ��M 
			switch (c)
			{
			case '1':
				bt_cmd = 0;
				break;
			default:
				break;
			}
			fputc(c, bt); // �G�R�[�o�b�N 
		}
		
		//���ʂ�PC�ɑ��M
		/*
		fprintf(bt, "S%03d,%03d,%03d,null,",����ϐ��P, ����ϐ��Q, ����ϐ��R);
		*/
	}
}
