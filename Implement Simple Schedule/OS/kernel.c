#include "kernel.h"
#include "led.h"
#include "usart.h"
#include "task.h"
//#define myHeap_Size 1024
//unsigned int CPU_ExceptStack[myHeap_Size]; /* 给MSR分配的主堆栈，不是任务堆栈 */
//unsigned int* OS_CPU_ExceptStackBase = &CPU_ExceptStack[myHeap_Size-1] ; /* 主堆栈的指针 */
int uxCriticalNesting = 0;//暂时没怎么用到
extern pTCB pTCB_IDLE;
extern pTCB pTCB_Cur;	//引用声明
extern pTCB pTCB_Rdy;	


//打开cpu全局中断
void os_cpu_interrupt_enable(void)
{
  if(uxCriticalNesting>0) uxCriticalNesting--;
  else if(uxCriticalNesting==0) EXIT_CRITICAL();
}
//关闭cpu全局中断
void os_cpu_interrupt_disable(void)
{
	ENTER_CRITICAL();
    if(uxCriticalNesting<0xFFFFFFFF) uxCriticalNesting++;
}

int* NVIC_INT_CTRL= (int *)0xE000ED04;  //中断控制寄存器地址
void SetPendSV(void)//挂起PendSV
{
	*NVIC_INT_CTRL=0x10000000;
}

void OS_Schedule(void)
{	
	pTCB pT = GetHighRdyTask();
	//检测是否需要任务切换，如果需要则挂起PendSV中断
	if(pT != pTCB_Cur)
	{
		if(pTCB_Cur->state == eTask_Running)
			pTCB_Cur->state = eTask_Ready;
		
		pTCB_Rdy = pT;
		pTCB_Rdy->state = eTask_Running;
		//DeList(&pRList,&pTCB_Rdy);//从就绪列表中取出新的队首TCB
		//pTCB_Cur = pTCB_Rdy;//pTCB_Cur在汇编中完成了，不能随便调整pTCB_Cur
		
		SetPendSV();
	}
}
void PendSVPriority_Init(void)
{
	char* NVIC_SYSPRI14= (char *)0xE000ED22;  		//PendSV优先级寄存器地址
	*NVIC_SYSPRI14=0xFF;							//设置PendSV中断优先级最低 
}


//systick中断服务函数
void SysTick_Handler(void)
{	
//	static int t = TIME_SECTION;
	int i = 0;
//	t--;
//	if(t == 0) 
//	{		
//		t = TIME_SECTION;
		//Print();
		
		os_cpu_interrupt_disable();
		if(GetTaskNum(eTask_Blocked) != 0)//延时任务列表中是否有阻塞任务
		{
			for(i = 0; i < created_task_num; i++)
			{
				if(TASK_LIST[i]->state == eTask_Blocked)
				{
					if(TASK_LIST[i]->DlyTim == 0)//延时时间到了，解除阻塞
					{
						TASK_LIST[i]->state = eTask_Ready;
					}
					else
						TASK_LIST[i]->DlyTim--;
				}
			}
		}

		if(GetTaskNum(eTask_Ready) != 0)
			OS_Schedule();
		
		os_cpu_interrupt_enable();
//	}
}

void OSDelayTicks(int ticks)
{
	pTCB_Cur->state = eTask_Blocked;
	pTCB_Cur->DlyTim = ticks/TIME_SECTION-1;
	OS_Schedule();
	while(pTCB_Cur->DlyTim != 0);//不能是while(1)，否则下次任务解阻塞后，继续运行while(1);
}



void SysTick_Init(void)
{
	char *Systick_priority = (char *)0xe000ed23;	//Systick中断优先级寄存器
	*Systick_priority = 0x00;           			//设置SysTick中断优先级最高
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//选择外部时钟  HCLK
	SysTick->LOAD  = ( SystemCoreClock / configTICK_RATE_HZ) - 1UL;	//定时周期1ms
	SysTick->VAL   = 0;                            	//Systick定时器计数器清0
	//SysTick->CTRL: bit0-定时器使能 bit1-中断使能 bit2-时钟源选择(=1系统主频，=0系统主频/8)
	SysTick->CTRL = 0x7;	//选择外部时钟，允许中断，开启定时器
}



void OS_Start(void)
{
	PendSVPriority_Init();
	SysTick_Init();

	pTCB_Rdy = GetHighRdyTask();
	pTCB_Rdy->state = eTask_Running;
	SP_INIT();
	SetPendSV();
	while(1);//等待调度
	
}

void print_task_list(eTaskSta state)
{
	int i,List_len = GetTaskNum(state);
	
	if(state == eTask_Blocked)
		printf("DList_len:%d\tpDList:",List_len);
	else if(state == eTask_Ready)
		printf("RList_len:%d\tpRList:",List_len);
	
	for(i = 0; i < created_task_num; i++)
	{
		if(TASK_LIST[i]->state == state)
			printf("\t%s",TASK_LIST[i]->name);
	}
	printf("\r\n");
	
}
void Print(void)//系统信息的打印函数是一定要先写出来的！！！没有进程工作的全貌，很难诊断问题
{

	printf("Current Task:%s\r\n",pTCB_Cur->name);
//	print_task_list(eTask_Blocked);
//	print_task_list(eTask_Ready);	
}



