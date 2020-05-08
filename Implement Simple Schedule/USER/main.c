#include "sys.h"
#include "led.h"
#include "usart.h"
#include "delay.h"
#include "timer.h"
#include "task.h"
#include "kernel.h"

#define STK_SIZE 128
stk32 STK_LED_TASK[STK_SIZE]; 
stk32 STK_PRINT_TASK[STK_SIZE]; 
stk32 STK_IDLE_TASK[64];

void OS_IDLE_Task(void)
{
	//printf("idle task\r\n");
	while(1);
}

void Led_Task(void)
{
	while(1)
	{	
		LED = !LED;
//		printf("led task\r\n");
		OSDelayTicks(500);
	}
}
void Print_Task(void)
{
	while(1)
	{
		printf("print task\r\n");
		OSDelayTicks(1000);
	}
}
pTCB pTCB_IDLE;
pTCB pTCB_Cur;	//指向当前任务TCB
pTCB pTCB_Rdy;	

int main(void)
{

	pTCB pT1,pT2;
	
	LED_Init();
	uart_init(115200);
//	TIM2_Int_Init(1000-1,7200-1);//10Khz的计数频率，计数到1000为100ms 
//	Delay_100ms(10);

	create_new_task(OS_IDLE_Task,"OS_IDLE_Task",0,&STK_IDLE_TASK[64-1],&pTCB_IDLE);
	create_new_task(Led_Task,"Led_Task",2,&STK_LED_TASK[STK_SIZE-1],&pT1);
	create_new_task(Print_Task,"Print_Task",1,&STK_PRINT_TASK[STK_SIZE-1],&pT2);
	
	OS_Start();
	while(1);
}





