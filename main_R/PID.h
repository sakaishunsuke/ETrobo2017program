/***************************************************************************//**
	@file		PID.h
	@brief		PID 制御モジュールヘッダ
	@author		$Author: toyo_chun $
	@version	$Revision: 386 $
	@date		$Date:: 2012-02-19 01:17:54 +0900 #$
*//****************************************************************************/

#if !defined(_PID_h_)
#define _PID_h_

/*******************************************************************************
	include
*******************************************************************************/
//#include "ecrobot_types.h"
//#include "Direction.h"

/*******************************************************************************
macro
*******************************************************************************/
#define PID_TURN_LIMIT		97

/* PID 係数 */
#define PID_TURN_P_FACTOR	( 0.67)
#define PID_TURN_I_FACTOR	( 0.00007)
#define PID_TURN_D_FACTOR	( 9.2)

/*******************************************************************************
	typedef
*******************************************************************************/
/** Pid 構造体 */
typedef struct
{
	int	brightness_P;	/**< P 制御用の明るさの偏差 */
	int	brightness_I;	/**< I 制御用の明るさの偏差の積算値 */
} PID;



/*******************************************************************************
	prototype
*******************************************************************************/
extern void	PID_initialize(PID *this, int theBrightnessDeviation);
extern int	PID_getTurn(PID *this, int theBrightnessDeviation, int theEdge);

#endif	/* !_PID_h_ */

