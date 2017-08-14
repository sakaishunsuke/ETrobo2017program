/* �X�^�b�N�̑���֐���` */

#include  <stdio.h>
#include  <stdlib.h>
#include  "stack.h"

/*--- �X�^�b�N�̏����� ---*/
int StackAlloc(Stack *s, int max) {
	s->ptr = 0;
	if ((s->stk = calloc(max, sizeof(int))) == NULL) {
		s->max = 0;                     /* �z��̊m�ۂɎ��s */
		return (-1);
	}
	s->max = max;
	return (0);
}

/*--- �X�^�b�N�̌�n�� ---*/
void StackFree(Stack *s) {
	if (s->stk != NULL) {
		free(s->stk);
		s->max = s->ptr = 0;
	}
}

/*--- �X�^�b�N�Ƀf�[�^���v�b�V�� ---*/
int StackPush(Stack *s, int x) {
	if (s->ptr >= s->max)
		return (-1);
	s->stk[s->ptr++] = x;
	return (0);
}

/*--- �X�^�b�N����f�[�^���|�b�v ---*/
int StackPop(Stack *s, int *x) {
	if (s->ptr <= 0)                        /* �X�^�b�N�͋� */
		return (-1);
	*x = s->stk[--s->ptr];
	return (0);
}

/*--- �X�^�b�N����f�[�^���s�[�N ---*/
int StackPeek(const Stack *s, int *x) {
	if (s->ptr <= 0)                        /* �X�^�b�N�͋� */
		return (-1);
	*x = s->stk[s->ptr - 1];
	return (0);
}

/*--- �X�^�b�N�̑傫�� ---*/
int StackSize(const Stack *s) {
	return (s->max);
}

/*--- �X�^�b�N�ɐς܂�Ă���f�[�^�� ---*/
int StackNo(const Stack *s) {
	return (s->ptr);
}

/*--- �X�^�b�N�͋� ---*/
int StackIsEmpty(const Stack *s) {
	return (s->ptr <= 0);
}

/*--- �X�^�b�N�͖��t�� ---*/
int StackIsFull(const Stack *s) {
	return (s->ptr >= s->max);
}

/*--- �X�^�b�N����ɂ��� ---*/
void StackClear(Stack *s) {
	s->ptr = 0;
}