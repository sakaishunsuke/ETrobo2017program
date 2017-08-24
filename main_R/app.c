/**
 ******************************************************************************
 ** ファイル名 : app.c
 **
 ** 概要 : 2輪倒立振子ライントレースロボットのTOPPERS/HRP2用Cサンプルプログラム
 **
 ** 注記 : sample_c4 (sample_c3にBluetooth通信リモートスタート機能を追加)
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

//#include "grayJudgment.h"//PCへlogを送信するため

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




static int      bt_cmd = 0;     /* Bluetoothコマンド 1:リモートスタート */
static FILE     *bt = NULL;     /* Bluetoothファイルハンドル */

//暫定でここに書いてる
#define SCENARIO_SIZE 10 /*シナリオの数*/
#define SCENARIO_KIND_SIZE 5 /*シナリオの中の種類　PID、ターン後の時間、maxスピード*/
/*--- シナリオのデータ類を実現する構造体 ---*/
typedef struct {
	int  max;       /* シナリオの数 */
	int  Number;       /* 現在のシナリオ番号 */
	int  scenario[SCENARIO_SIZE][SCENARIO_KIND_SIZE];      /* シナリオ本体() */
} SCENARIO;
SCENARIO scenario;	/* 構造体の実体化 */
void scenarioInit(SCENARIO *s) {	/* シナリオの初期化関数 */
	s->max = 1;
	s->Number = 0;
	s->scenario[0][0] = 1;
	s->scenario[0][1] = 0;
	s->scenario[0][2] = 0;
	s->scenario[0][3] = 500;
	s->scenario[0][4] = 50;
}

/*--- BTのデータ類を実現する構造体 ---*/
typedef struct {
	bool_t	send_flag;			/* BTでデータを遅らせるflag*/
	int		left_motor_value;	/* 左モータの回転量 */
	int		right_motor_value;	/* 右モータの回転量 */
	int		running_state;		/* 走行の状態、直進0、右旋回1、左旋回-1 */
	bool_t	start_flag;			/* BTからのスタートのflag */
	bool_t	stop_flag;			/* BTからのストップのflag */
} BT_DATA;
void btDataInit(BT_DATA *bt_data) {
	bt_data->send_flag = false;
	bt_data->left_motor_value = 0;
	bt_data->right_motor_value = 0;
	bt_data->running_state = 0;
	bt_data->start_flag = false;
	bt_data->stop_flag = false;
}
BT_DATA bt_data;	/* 構造体の実体化 */

/*BTの設定のマクロ(暫定)*/
#define BTLEN 10 /*BTで受け取る文字の長さ*/


/* 下記のマクロは個体/環境に合わせて変更する必要があります */

/* sample_c1マクロ */
//#define GYRO_OFFSET  0          /* ジャイロセンサオフセット値(角速度0[deg/sec]時) */
//#define LIGHT_WHITE  40         /* 白色の光センサ値 */
//#define LIGHT_BLACK  0          /* 黒色の光センサ値 */


/* sample_c2マクロ */
#define SONAR_ALERT_DISTANCE 30 /* 超音波センサによる障害物検知距離[cm] */


/* sample_c3マクロ */
//#define TAIL_ANGLE_STAND_UP  93 /* 完全停止時の角度[度] */
//#define TAIL_ANGLE_DRIVE      3 /* バランス走行時の角度[度] */
//#define P_GAIN             2.5F /* 完全停止用モータ制御比例係数 */
//#define PWM_ABS_MAX          60 /* 完全停止用モータ制御PWM絶対最大値 */


/* sample_c4マクロ */
//#define DEVICE_NAME     "ET0"  /* Bluetooth名 hrp2/target/ev3.h BLUETOOTH_LOCAL_NAMEで設定 */
//#define PASS_KEY        "1234" /* パスキー    hrp2/target/ev3.h BLUETOOTH_PIN_CODEで設定 */
#define CMD_START         '1'    /* リモートスタートコマンド */

/* LCDフォントサイズ */
#define CALIB_FONT (EV3_FONT_SMALL)
#define CALIB_FONT_WIDTH (6/*TODO: magic number*/)
#define CALIB_FONT_HEIGHT (8/*TODO: magic number*/)

/* 関数プロトタイプ宣言 */
static int sonar_alert(void);


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




void init() {
	/* LCD画面表示 */
	ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);

	/*シナリオの初期化*/
	scenarioInit(&scenario);
	/* BT_DATAの初期化 */
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


/* メインタスク */
void main_task(intptr_t unused)
{
    /* 初期化 */
	init();
	/* Open Bluetooth file */
    bt = ev3_serial_open_file(EV3_SERIAL_BT);
    //assert(bt != NULL);
	/* Bluetooth通信タスクの起動 */
    act_tsk(BT_TASK);

    ev3_led_set_color(LED_ORANGE); /* 初期化完了通知 */

    while(1)
    {
		/* スタート待機 */
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP); /* 完全停止用角度に制御 */

        if (bt_cmd == 1){
            break; /* リモートスタート */
        }

		if(Devices_getDeviceValue(SENSOR_TOUCH_)==1){
            break; /* タッチセンサが押された */
        }

        tslp_tsk(1); /* 10msecウェイト */
    }


	/* 走行モーターエンコーダーリセット */
	ev3_motor_reset_counts(left_motor);
	ev3_motor_reset_counts(right_motor);

    ev3_led_set_color(LED_GREEN); /* スタート通知 */

    /**
    * Main loop for the self-balance control algorithm
    */
    while(1){
		RunControl_run();
        tslp_tsk(4); /* 4msec周期起動 */
    }
    ev3_motor_stop(left_motor, false);
    ev3_motor_stop(right_motor, false);

    ter_tsk(BT_TASK);
    fclose(bt);

    ext_tsk();
}

//*****************************************************************************
// 関数名 : sonar_alert
// 引数 : 無し
// 返り値 : 1(障害物あり)/0(障害物無し)
// 概要 : 超音波センサによる障害物検知
//*****************************************************************************
static int sonar_alert(void)
{
    static unsigned int counter = 0;
    static int alert = 0;

    signed int distance;

    if (++counter == 40/4) /* 約40msec周期毎に障害物検知  */
    {
        /*
         * 超音波センサによる距離測定周期は、超音波の減衰特性に依存します。
         * NXTの場合は、40msec周期程度が経験上の最短測定周期です。
         * EV3の場合は、要確認
         */
        distance = ev3_ultrasonic_sensor_get_distance(sonar_sensor);
        if ((distance <= SONAR_ALERT_DISTANCE) && (distance >= 0))
        {
            alert = 1; /* 障害物を検知 */
        }
        else
        {
            alert = 0; /* 障害物無し */
        }
        counter = 0;
    }

    return alert;
}



//*****************************************************************************
// 関数名 : bt_task
// 引数 : unused
// 返り値 : なし
// 概要 : Bluetooth通信によるリモートスタート。 Tera Termなどのターミナルソフトから、
//       ASCIIコードで1を送信すると、リモートスタートする。
//*****************************************************************************
void bt_task(intptr_t unused)
{
    while(1)
    {
		/*PCに送信*/
		//fprintf(bt, "N_%1d", scenario.Number); /* 現在のシナリオ番号を転送 */
		/*
		char buf[BTLEN];
		//serial_rea_dat(EV3_SERIAL_BT, buf, BTLEN);//BTLEN分の文字を取得

		if (strstr(buf,"start") != NULL) {
			bt_cmd = 1;
		}
		else if (strstr(buf, "stop") != NULL) {
			bt_cmd = 0;
		}
		else if (strstr(buf,"M_") != NULL) {
			char *tok = strtok(buf,"M_");
			if (tok != NULL) {
				scenario.Number = tok[0] - '0';//数値を取得
				scenario.Number++;
			}
		}
		*/
		if (bt_cmd != 1) {
			uint8_t c = fgetc(bt); // 受信 
			switch (c)
			{
			case '1':
				bt_cmd = 0;
				break;
			default:
				break;
			}
			fputc(c, bt); // エコーバック 
		}
		
		//結果をPCに送信
		/*
		fprintf(bt, "S%03d,%03d,%03d,null,",送る変数１, 送る変数２, 送る変数３);
		*/
	}
}
