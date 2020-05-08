#include "task.h"
#include "stdlib.h"
#include "string.h"
#include "usart.h"
pTCB TASK_LIST[OS_MAX_TASK];
int created_task_num = 0;//全局变量
//任务堆栈初使化
//只要产生PendSV中断,跳转中断处理函数前 xPSR,PC,LR,R12,R3-R0被自动保存到系统或任务栈中(此步聚是PendSV中断硬件机制)，
//保存在哪个栈取决于当前的SP类型，如是MSP则保存到系统栈，如是PSP则保存到任务栈。本OS是保存于任务栈。而R11-R4需要手动保存到任务栈中
//入栈顺序：栈顶->栈尾 xPSR,PC,LR,R12,R3-R0,R4-R11共16个(SP(R13)保存在TCB首个成员变量中)。
stk32* task_stk_init(void* func, stk32 *TopOfStack)
{
    stk32 *stk;
    stk = TopOfStack;
    *(stk)    = (u32)0x01000000L;//xPSR 程序状态寄存器，xPSR T位(第24位)必须置1,否则第一次执行任务时进入Fault中断                                                     
    *(--stk)  = (u32)func;   //PC   初使化时指向任务函数首地址（任务切换时，可能指向任务函数中间某地址）            
	*(--stk)  = (u32)0xFFFFFFFEL;//LR   保存着各种跳转的返回连接地址,初使化为最低4位为E，是一个非法值，主要目的是不让使用R14，即任务是不能返回的              
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
