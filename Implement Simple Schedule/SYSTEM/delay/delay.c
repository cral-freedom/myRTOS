#include "delay.h"

//uS—” ±
void delay_us(u32 us)
{       
    u32 ticks;
    u32 told,tnow,tcnt=0;
    u32 reload=SysTick->LOAD;       
    ticks=SystemCoreClock/1000000*us;      
    tcnt=0;
    told=SysTick->VAL;
    while(1)
    {
			tnow=SysTick->VAL;  
			if(tnow!=told)
			{       
				if(tnow<told) tcnt += told-tnow;
				else tcnt += reload-tnow+told;      
				told = tnow;
				if(tcnt>=ticks) break;
			} 					
    }
}
//mS—” ±
void delay_ms(u32 ms)
{   
  delay_us(ms*1000);			
}




















































