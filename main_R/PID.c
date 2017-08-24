/***************************************************************************//**
	@file		PID.c
	@brief		PID ���䃂�W���[��
	@author		$Author: toyo_chun $
	@version	$Revision: 386 $
	@date		$Date:: 2012-02-19 01:17:54 +0900 #$
*//****************************************************************************/

/*******************************************************************************
	include
*******************************************************************************/
#include "PID.h"







/*******************************************************************************
	prototype
*******************************************************************************/
static int	PID_calculateTurn(
			PID *this,
			float theP_Factor, float theI_Factor, float theD_Factor,
			int theBrightnessDeviation, int theEdgeFactor
		);



/***************************************************************************//**
	@brief		����������
	@author		takigawa
	@date		2012/05/17

	@param		*this			PID �\���̂ւ̃|�C���^
	@param		theBrightnessDeviation	���邳�̕΍�
	@return		�Ȃ�
*//****************************************************************************/
void	PID_initialize(PID *this, int theBrightnessDeviation)
{
	this->brightness_P	= theBrightnessDeviation;
	this->brightness_I	= 0;
}



/***************************************************************************//**
	@brief		turn �l���擾����
	@author		takigawa
	@date		2012/05/17

	@param		*this			PID �\���̂ւ̃|�C���^
	@param		theBrightnessDeviation	���邳�̕΍�
	@param		theEdge			���s�G�b�W�W�� (-1�F�E�C1�F��)
	@return		turn �l : -100 (������) �` 100 (�E����)
*//****************************************************************************/
int	PID_getTurn(PID *this, int theBrightnessDeviation, int theEdge)
{
	int	turn;

	turn =	PID_calculateTurn(
			this,
			PID_TURN_P_FACTOR,
			PID_TURN_I_FACTOR,
			PID_TURN_D_FACTOR,
			theBrightnessDeviation,
			theEdge
			//�G�b�W�͉E��-1�ō���1
		);

	return turn;
}



/***************************************************************************//**
	@brief		PID ����p�� turn �l���擾����
	@author		takigawa
	@date		2012/05/17

	@param		*this			PID �\���̂ւ̃|�C���^
	@param		theP_Factor		P �����̌W��
	@param		theI_Factor		I �����̌W��
	@param		theD_Factor		D �����̌W��
	@param		theBrightnessDeviation	���邳�̕΍�
	@param		theEdgeFactor		���s�G�b�W�W�� (-1�F�E�C1�F��)
	@return		turn �l : -100 (������) �` 100 (�E����)
*//****************************************************************************/
int	PID_calculateTurn(
		PID *this,
		float theP_Factor, float theI_Factor, float theD_Factor,
		int theBrightnessDeviation, int theEdgeFactor
	)
{
	int	turn;			/* ���񖽗�: -100 (������) �` 100 (�E����) */
	int	turn_P;			/* ���񖽗߂̔�ᐬ�� */
	int	turn_I;			/* ���񖽗߂̐ϕ����� */
	int	turn_D;			/* ���񖽗߂̔������� */
	int	brightness_P;		/* ���邳�� P ���� */
	int	brightness_I;		/* ���邳�� I ���� */
	int	brightness_D;		/* ���邳�� D ���� */

	brightness_P		= theBrightnessDeviation;
	brightness_I		= this->brightness_I + theBrightnessDeviation;
	brightness_D		= theBrightnessDeviation - this->brightness_P;

	this->brightness_P	= theBrightnessDeviation;
	this->brightness_I	= brightness_I;

	/*
		P �������� turn �����߂�
	*/
	turn_P = (int)(theP_Factor * (float)brightness_P);
	turn_P *= theEdgeFactor;

	/*
		I �������� turn �����߂�
	*/
	turn_I = (int)(theI_Factor * (float)brightness_I);
	turn_I *= theEdgeFactor;

	/*
		D �������� turn �����߂�
	*/
	turn_D = (int)(theD_Factor * (float)brightness_D);
	turn_D *= theEdgeFactor;

	/*
		turn �����肷��
	*/
	turn = turn_P + turn_I + turn_D;
	if(turn >= PID_TURN_LIMIT) {
		turn = PID_TURN_LIMIT;
	} else if (turn <= (-1 * PID_TURN_LIMIT)) {
		turn = -1 * PID_TURN_LIMIT;
	}

	return turn;
}

