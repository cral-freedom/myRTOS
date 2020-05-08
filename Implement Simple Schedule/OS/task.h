#ifndef __TASK_H
#define __TASK_H
#include "sys.h"
//����Խ����������ȨԽ��

typedef unsigned int u32;
typedef unsigned int stk32;
typedef unsigned char u8;
//typedef void (*TaskFunc)( void );
#define OS_MAX_TASK 8	//�����������

typedef enum
{
	eTask_Running = 0,
	eTask_Ready,
	eTask_Suspended,
	eTask_Blocked,
	eTask_Deleted,
}eTaskSta;


typedef struct _TaskControlBlock
{
	stk32 *StkPtr;
	char name[16];
	int state;	//����״̬
	int prio;
	int DlyTim; //��������ʱ��
}TCB,*pTCB;
extern pTCB TASK_LIST[OS_MAX_TASK];
extern int created_task_num;


stk32* task_stk_init(void* func, stk32 *TopOfStack);
void create_new_task( void *func, char name[], int prio, stk32 *TopOfStack, pTCB *tcb);
int GetTaskNum(eTaskSta state);
pTCB GetHighRdyTask(void);	
//void Task_Delete(Tsk_Handle *pTCB);//�����ھ����б��е�����ɾ������ô���
#endif
