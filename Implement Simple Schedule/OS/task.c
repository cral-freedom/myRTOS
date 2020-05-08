#include "task.h"
#include "stdlib.h"
#include "string.h"
#include "usart.h"
pTCB TASK_LIST[OS_MAX_TASK];
int created_task_num = 0;//ȫ�ֱ���
//�����ջ��ʹ��
//ֻҪ����PendSV�ж�,��ת�жϴ�����ǰ xPSR,PC,LR,R12,R3-R0���Զ����浽ϵͳ������ջ��(�˲�����PendSV�ж�Ӳ������)��
//�������ĸ�ջȡ���ڵ�ǰ��SP���ͣ�����MSP�򱣴浽ϵͳջ������PSP�򱣴浽����ջ����OS�Ǳ���������ջ����R11-R4��Ҫ�ֶ����浽����ջ��
//��ջ˳��ջ��->ջβ xPSR,PC,LR,R12,R3-R0,R4-R11��16��(SP(R13)������TCB�׸���Ա������)��
stk32* task_stk_init(void* func, stk32 *TopOfStack)
{
    stk32 *stk;
    stk = TopOfStack;
    *(stk)    = (u32)0x01000000L;//xPSR ����״̬�Ĵ�����xPSR Tλ(��24λ)������1,�����һ��ִ������ʱ����Fault�ж�                                                     
    *(--stk)  = (u32)func;   //PC   ��ʹ��ʱָ���������׵�ַ�������л�ʱ������ָ���������м�ĳ��ַ��            
	*(--stk)  = (u32)0xFFFFFFFEL;//LR   �����Ÿ�����ת�ķ������ӵ�ַ,��ʹ��Ϊ���4λΪE����һ���Ƿ�ֵ����ҪĿ���ǲ���ʹ��R14���������ǲ��ܷ��ص�              
	stk-=13;	
    return stk;
}

void create_new_task( void *func, char name[], int prio, stk32 *TopOfStack, pTCB *tcb)
{
	int name_len = strlen(name);
	irq_disable();
	
	if(created_task_num == OS_MAX_TASK)
	{
		printf("Create Task Fail\r\n");
	}
	
	if(tcb)
	{
		*tcb = (pTCB)malloc(sizeof(TCB));
		if(*tcb)
		{
			(*tcb)->StkPtr = task_stk_init(func,TopOfStack);
			memcpy((*tcb)->name,name,sizeof(char)*name_len);
			(*tcb)->state = eTask_Ready;
			(*tcb)->prio = prio;
			(*tcb)->DlyTim = 0;	
			TASK_LIST[created_task_num] = *tcb;
			created_task_num++;
		}
	}
	irq_enable();
}

int GetTaskNum(eTaskSta state)
{
	int i, num = 0;
	for(i = 0; i < created_task_num; i++)
	{	
		if(TASK_LIST[i]->state == state)
			num++;
	}
	return num;
}


extern pTCB pTCB_IDLE;
pTCB GetHighRdyTask(void)
{
	int i = 0;
	pTCB pTtmp ,pTrdy = pTCB_IDLE;
	for(i = 0; i < created_task_num; i++)
	{
		if(TASK_LIST[i]->state == eTask_Ready)
		{
			pTtmp = TASK_LIST[i];
			if(pTtmp->prio > pTrdy->prio)
				pTrdy = pTtmp;
		}
	}
	return pTrdy;		
}
