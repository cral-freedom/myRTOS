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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 	
	TIM_ARRPreloadConfig(TIM2, ENABLE);
    
    /* 设置更新请求源只在计数器上溢或下溢时产生中断 */
    TIM_UpdateRequestConfig(TIM2,TIM_UpdateSource_Global); 
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);	
}

void Delay_100ms(u32 nTime)
{     
    /* 清零计数器并使能定时器 */  
    TIM2->CNT   = 0;  
	
    TIM_Cmd(TIM2, ENABLE);     

    for( ; nTime > 0 ; nTime--)
    {
     /* 等待一个延时单位的结束 */
     while(TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) != SET);
     TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    }

    TIM_Cmd(TIM2, DISABLE);
}


//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM3, //TIM2
		TIM_IT_Update ,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设						 
}



S_TIM task[TaskSum] = {
	//	名字 		id 激活状态 间隔/100ms 计时	回调函数
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
		printf("time:%d s\r\n",t/10);//10次100ms = 1s
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
		if(check_activate(task[i].activated))//检测task[i]的激活状态
		{
			task[i].task(t);//调用回调函数，参数为硬件定时器中断次数
		}
	}
}

void TIM3_IRQHandler(void)   //TIM3中断
{
	static int times = 0;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
		times++;
		if(times == 0x7fffffff)//防溢出
			times = 0;
		SoftTimerManager(times);
	}
}












