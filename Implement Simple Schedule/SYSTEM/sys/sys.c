#include "sys.h"


//********************************************************************************  
//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//�ر������ж�
void irq_disable(void)	{	__ASM volatile("cpsid i");	}
//���������ж�
void irq_enable(void)	{	__ASM volatile("cpsie i");	}


//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

