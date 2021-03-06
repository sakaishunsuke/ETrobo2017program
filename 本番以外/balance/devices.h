#ifndef _devices
#define _devices


#include "ev3api.h"
#include "app.h"
#include "balancer.h"

#define TAIL_ANGLE_MINIMUM  360  /*KöðêÔãÉ·éÁêl*/
#define TAIL_ANGLE_STAND_UP  98  /* ®Sâ~Ìpx[x] */
#define TAIL_ANGLE_DRIVE      3  /* oXsÌpx[x] */
#define P_GAIN             0.5F  /* ®Sâ~p[^§äÝæW */
#define PWM_ABS_MAX          60  /* ®Sâ~p[^§äPWMâÎÅål */

/* eIuWFNgÌÊµÔ */

#define SENSOR_COLOR_ 0
#define SENSOR_SONER_ 1
#define SENSOR_TOUCH_ 2
#define SENSOR_GYRO_ 3
#define MOTOR_RIGHT_ 4
#define MOTOR_LEFT_ 5
#define MOTOR_TAIL_ 6
#define BATTERY_ 7
#define MONITOR_ 8

#define WHITE_CALIBRATE 10
#define BLACK_CALIBRATE 11
#define GYRO_CALIBRATE 12
#define GRAY_CALIBRATE 13


void Devices_calibrate();
void Devices_controlDevice(int deviceNo, int value);
int  Devices_getCalibrateValue(int deviceNo);
int  Devices_getDeviceValue(int deviceNo);



#endif 