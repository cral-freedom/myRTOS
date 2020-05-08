#ifndef __KERNEL_H
#define __KERNEL_H 			   
#include "sys.h"

#define configTICK_RATE_HZ	(1000)              //ʱ�ӽ���Ƶ�ʣ���������Ϊ1000�����ھ���1ms
#define TIME_SECTION 1 //100*1ms	

extern int uxCriticalNesting;
void SetPendSV(void);//����PendSV
void PendSVPriority_Init(void);
void PendSV_Handler(void);
void SysTick_Init(void);
void OSDelayTicks(int ticks);
void OS_Start(void);
void SP_INIT(void);
void EXIT_CRITICAL(void);
void ENTER_CRITICAL(void);
void os_cpu_interrupt_disable(void);
void os_cpu_interrupt_enable(void);
void Print(void);
#endif
