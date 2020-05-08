#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

#define TaskSum 3 
//������ʱ��ʵ�ֶ�Ӧ�ù���
typedef struct _Soft_timer
{
	char name[10];//����
	u8 timID;//�����ʱ��ID
	u8 activated;//����״̬
	u8 nT;//Ӳ����ʱ������T�ı������ɱ�ʾ��ʱʱ��
	u8 count;//��ʱ
	void (*task)(int);
	
//��չ�����ζ�ʱ����ζ�ʱ��ѭ����ʱ��ĿǰĬ��Ϊѭ����ʱ
//	u8 cir_flag;//ѭ����ʶ
//	u8 exce_count;//���д���
//	u8 time_out;//��ʱ����
	
}S_TIM;

void TIM3_Int_Init(u16 arr,u16 psc); 
void led_task(int t);
void print_task(int t);
void timer_task(int t);
void SoftTimerManager(int t);

void TIM2_Int_Init(u16 arr,u16 psc);
void Delay_100ms(u32 nTime);

#endif
