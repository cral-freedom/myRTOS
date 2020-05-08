
任务调度策略：时间片调度

任务切换：
	通过SysTick异常周期性地切入系统，从而能任务调度；
	任务切换的工作是由PendSV来做。
	
	kernel.c
		
		初始化SysTick定时器，提供系统“心跳”；
		SysTick->CTRL: bit0-定时器使能 bit1-中断使能 bit2-时钟源选择(=1系统主频，=0系统主频/8)
		
		SysTick_Handler：检测是否需要任务切换，如果需要则挂起PendSV中断
	
	
	//重装载值计算原理：系统时钟频率为72M，1秒钟计数72M次，现SysyTick计数72M/1000次（计数范围：72M/1000-1 ~ 0），即1ms(1/1000秒)重装载一次	

	kernel.asm（源自Angle_145大神）	
		
		PendSV_Handler：把寄存器值保存到任务A堆栈，再把任务B堆栈中的内容恢复（上下文切换）；调整PC指针，切换任务。


状态变换：Ready → Running → Blocked → Ready
	
	
	