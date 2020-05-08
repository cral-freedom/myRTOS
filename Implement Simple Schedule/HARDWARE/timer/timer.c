#include "timer.h"
#include "stm32f10x_tim.h"
#include "led.h"
#include "usart.h"


void led_task(int t);
void print_task(int t);
void timer_task(int t);



void TIM2_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 	
	TIM_ARRPreloadConfig(TIM2, ENABLE);
    
    /* ���ø�������Դֻ�ڼ��������������ʱ�����ж� */
    TIM_UpdateRequestConfig(TIM2,TIM_UpdateSource_Global); 
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);	
}

void Delay_100ms(u32 nTime)
{     
    /* �����������ʹ�ܶ�ʱ�� */  
    TIM2->CNT   = 0;  
	
    TIM_Cmd(TIM2, ENABLE);     

    for( ; nTime > 0 ; nTime--)
    {
     /* �ȴ�һ����ʱ��λ�Ľ��� */
     while(TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) != SET);
     TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    }

    TIM_Cmd(TIM2, DISABLE);
}


//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, //TIM2
		TIM_IT_Update ,
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����						 
}



S_TIM task[TaskSum] = {
	//	���� 		id ����״̬ ���/100ms ��ʱ	�ص�����
	{"led_200", 	0,	1, 		2, 			0,	led_task},
	{"print_500", 	1, 	1, 		5, 			0,	print_task},
	{"count", 		2, 	1, 		10, 		0,	timer_task}	
}; //TaskSum = 3

void led_task(int t)
{
	task[0].count++;
	if(task[0].count == task[0].nT)
	{
		LED =! LED;
		task[0].count = 0;
	}
}

void print_task(int t)
{
	task[1].count++;
	if(task[1].count == task[1].nT)
	{
		printf("print_task\r\n");
		task[1].count = 0;
	}
}
void timer_task(int t)
{
	task[2].count++;
	if(task[2].count == task[2].nT)
	{
		printf("time:%d s\r\n",t/10);//10��100ms = 1s
		task[2].count = 0;
	}
}

int check_activate(u8 activate)
{
	return activate==1;
}

void SoftTimerManager(int t)
{
	int i;
	for(i=0; i<TaskSum; i++)
	{
		if(check_activate(task[i].activated))//���task[i]�ļ���״̬
		{
			task[i].task(t);//���ûص�����������ΪӲ����ʱ���жϴ���
		}
	}
}

void TIM3_IRQHandler(void)   //TIM3�ж�
{
	static int times = 0;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ 
		times++;
		if(times == 0x7fffffff)//�����
			times = 0;
		SoftTimerManager(times);
	}
}












