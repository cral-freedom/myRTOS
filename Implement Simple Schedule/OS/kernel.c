#include "kernel.h"
#include "led.h"
#include "usart.h"
#include "task.h"
//#define myHeap_Size 1024
//unsigned int CPU_ExceptStack[myHeap_Size]; /* ��MSR���������ջ�����������ջ */
//unsigned int* OS_CPU_ExceptStackBase = &CPU_ExceptStack[myHeap_Size-1] ; /* ����ջ��ָ�� */
int uxCriticalNesting = 0;//��ʱû��ô�õ�
extern pTCB pTCB_IDLE;
extern pTCB pTCB_Cur;	//��������
extern pTCB pTCB_Rdy;	


//��cpuȫ���ж�
void os_cpu_interrupt_enable(void)
{
  if(uxCriticalNesting>0) uxCriticalNesting--;
  else if(uxCriticalNesting==0) EXIT_CRITICAL();
}
//�ر�cpuȫ���ж�
void os_cpu_interrupt_disable(void)
{
	ENTER_CRITICAL();
    if(uxCriticalNesting<0xFFFFFFFF) uxCriticalNesting++;
}

int* NVIC_INT_CTRL= (int *)0xE000ED04;  //�жϿ��ƼĴ�����ַ
void SetPendSV(void)//����PendSV
{
	*NVIC_INT_CTRL=0x10000000;
}

void OS_Schedule(void)
{	
	pTCB pT = GetHighRdyTask();
	//����Ƿ���Ҫ�����л��������Ҫ�����PendSV�ж�
	if(pT != pTCB_Cur)
	{
		if(pTCB_Cur->state == eTask_Running)
			pTCB_Cur->state = eTask_Ready;
		
		pTCB_Rdy = pT;
		pTCB_Rdy->state = eTask_Running;
		//DeList(&pRList,&pTCB_Rdy);//�Ӿ����б���ȡ���µĶ���TCB
		//pTCB_Cur = pTCB_Rdy;//pTCB_Cur�ڻ��������ˣ�����������pTCB_Cur
		
		SetPendSV();
	}
}
void PendSVPriority_Init(void)
{
	char* NVIC_SYSPRI14= (char *)0xE000ED22;  		//PendSV���ȼ��Ĵ�����ַ
	*NVIC_SYSPRI14=0xFF;							//����PendSV�ж����ȼ���� 
}


//systick�жϷ�����
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
		if(GetTaskNum(eTask_Blocked) != 0)//��ʱ�����б����Ƿ�����������
		{
			for(i = 0; i < created_task_num; i++)
			{
				if(TASK_LIST[i]->state == eTask_Blocked)
				{
					if(TASK_LIST[i]->DlyTim == 0)//��ʱʱ�䵽�ˣ��������
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
	while(pTCB_Cur->DlyTim != 0);//������while(1)�������´�����������󣬼�������while(1);
}



void SysTick_Init(void)
{
	char *Systick_priority = (char *)0xe000ed23;	//Systick�ж����ȼ��Ĵ���
	*Systick_priority = 0x00;           			//����SysTick�ж����ȼ����
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//ѡ���ⲿʱ��  HCLK
	SysTick->LOAD  = ( SystemCoreClock / configTICK_RATE_HZ) - 1UL;	//��ʱ����1ms
	SysTick->VAL   = 0;                            	//Systick��ʱ����������0
	//SysTick->CTRL: bit0-��ʱ��ʹ�� bit1-�ж�ʹ�� bit2-ʱ��Դѡ��(=1ϵͳ��Ƶ��=0ϵͳ��Ƶ/8)
	SysTick->CTRL = 0x7;	//ѡ���ⲿʱ�ӣ������жϣ�������ʱ��
}



void OS_Start(void)
{
	PendSVPriority_Init();
	SysTick_Init();

	pTCB_Rdy = GetHighRdyTask();
	pTCB_Rdy->state = eTask_Running;
	SP_INIT();
	SetPendSV();
	while(1);//�ȴ�����
	
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
void Print(void)//ϵͳ��Ϣ�Ĵ�ӡ������һ��Ҫ��д�����ģ�����û�н��̹�����ȫò�������������
{

	printf("Current Task:%s\r\n",pTCB_Cur->name);
//	print_task_list(eTask_Blocked);
//	print_task_list(eTask_Ready);	
}



