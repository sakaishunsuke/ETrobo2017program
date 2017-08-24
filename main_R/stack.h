


/* �X�^�b�N�̃f�[�^�\���Ɗ֐��v���g�^�C�v */
#ifndef _stack
#define _stack

/*--- �X�^�b�N����������\���� ---*/
typedef struct {
	int  max;       /* �X�^�b�N�̃T�C�Y */
	int  ptr;       /* �X�^�b�N�|�C���^ */
	int  *stk;      /* �X�^�b�N�i�̐擪�v�f�ւ̃|�C���^�j */
} Stack;


int StackAlloc(Stack *s, int max);     /*--- �X�^�b�N�̏����� ---*/
void StackFree(Stack *s);              /*--- �X�^�b�N�̌�n�� ---*/
int StackPush(Stack *s, int x);        /*--- �X�^�b�N�Ƀf�[�^���v�b�V�� ---*/
int StackPop(Stack *s, int *x);        /*--- �X�^�b�N����f�[�^���|�b�v ---*/
int StackPeek(const Stack *s, int *x); /*--- �X�^�b�N����f�[�^���s�[�N ---*/
int StackSize(const Stack *s);         /*--- �X�^�b�N�̑傫�� ---*/
int StackNo(const Stack *s);           /*--- �X�^�b�N�ɐς܂�Ă���f�[�^�� ---*/
int StackIsEmpty(const Stack *s);      /*--- �X�^�b�N�͋� ---*/
int StackIsFull(const Stack *s);       /*--- �X�^�b�N�͖��t�� ---*/
void StackClear(Stack *s);             /*--- �X�^�b�N����ɂ��� ---*/

#endif //_stack