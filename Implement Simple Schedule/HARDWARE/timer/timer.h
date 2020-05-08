#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

#define TaskSum 3 
//单个定时器实现多应用管理
typedef struct _Soft_timer
{
	char name[10];//名字
	u8 timID;//软件定时器ID
	u8 activated;//激活状态
	u8 nT;//硬件定时器周期T的倍数，可表示定时时间
	u8 count;//计时
	void (*task)(int);
	
//扩展：单次定时、多次定时和循环定时，目前默认为循环定时
//	u8 cir_flag;//循环标识
//	u8 exce_count;//运行次数
//	u8 time_out;//超时次数
	
}S_TIM;

void TIM3_Int_Init(u16 arr,u16 psc); 
void led_task(int t);
void print_task(int t);
void timer_task(int t);
void SoftTimerManager(int t);

void TIM2_Int_Init(u16 arr,u16 psc);
void Delay_100ms(u32 nTime);

#endif
