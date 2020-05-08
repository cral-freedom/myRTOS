
	IMPORT pTCB_Cur
	IMPORT pTCB_Rdy
	;IMPORT OS_CPU_ExceptStackBase	
	EXPORT PendSV_Handler
	EXPORT SP_INIT
	EXPORT ENTER_CRITICAL
	EXPORT EXIT_CRITICAL
		
	PRESERVE8                  ;//字节对齐关键词,指定当前文件八字节对齐。
	AREA |.text|, CODE, READONLY ;//定义一个代码段或数据段。
	THUMB                      ;//指定以下指令都是THUMB指令集(ARM汇编有多种指令集)

EXIT_CRITICAL                  ;//进入临界代码区标号，等价C语言函数void os_core_enter(void){}
    CPSID    I                 ;//关闭全局中断(Cortex-M内核的全局中断开关指令)
    BX    LR                   ;//等价C语言：return
ENTER_CRITICAL                   ;//void os_core_exit(void){}
    CPSIE    I                 ;//打开全局中断
    BX    LR                   ;//return   
	
SP_INIT  					   ;初始化PSP指针                      
    CPSID    I                 ;//关闭全局中断 
	
    LDR R4,=0x0                ;//R4装载立即数0(不直接给PSP赋值0而是经进R寄存器作为媒介是因为PSP只能和R寄存器打交道)            
    MSR     PSP, R4            ;//PSP(process stack pointer)程序堆栈指针赋值0。PSP属用户级(特级权下为MSP)，双堆栈结构。 

	;LDR     R0, = OS_CPU_ExceptStackBase   ; Initialize the MSP to the OS_CPU_ExceptStkBase
	;LDR     R1, [R0]
	;MSR     MSP, R1
	
    CPSIE    I                 ;//打开全局中断(此时若没有其他中断在响应，则立即进入PendSV中断函数)  
    BX    LR  
	
;/******************PendSV_Handler************/
PendSV_Handler
    CPSID    I                            ; OS_ENTER_CRITICAL();
	
    MRS     R0, PSP                            ; R0 = PSP;
    CBZ     R0, PendSV_Handler_NoSave          ; if(R0 == 0) goto PendSV_Handler_NoSave;
    
    SUB     R0, R0, #0x20            ; R0 = R0 - 0x20;
    
    ; easy method
    STM     R0, {R4-R11}
    
    LDR     R1, =pTCB_Cur            ; R1 = OSTCBCur;
    LDR     R1, [R1]                 ; R1 = *R1;(R1 = OSTCBCur->OSTCBStkPtr)
    STR     R0, [R1]                 ; *R1 = R0;(*(OSTCBCur->OSTCBStkPrt) = R0)
 
PendSV_Handler_NoSave				;每次都会进去（因为PendSV_Handler_NoSave不是函数，而是中间的一个标签，用于跳转）

	;实质就是pTCB_Cur = pTCB_Rdy
	;每次运行PendSV_Handler，都会使pTCB_Cur指向pTCB_Rdy，所以调度时只需从任务数组中获取pTCB_Rdy
    LDR     R0, =pTCB_Cur           ; R0 = OSTCBCur;
    LDR     R1, =pTCB_Rdy           ; R1 = OSTCBNext;
    LDR     R2, [R1]                ; R2 = OSTCBNext->OSTCBStkPtr;
    STR     R2, [R0]                ; *R0 = R2;(OSTCBCur->OSTCBStkPtr = OSTCBNext->OSTCBStkPtr)
    
    LDR     R0, [R2]                 ; R0 = *R2;(R0 = OSTCBNext->OSTCBStkPtr)
    LDM     R0, {R4-R11}
    ADD    R0, R0, #0x20
 
    MSR     PSP, R0                 ; PSP = R0;(PSP = OSTCBNext->OSTCBStkPtr)
    ORR     LR, LR, #0x04           ; LR = LR | 0x04;
	
    CPSIE     I                     ; OS_EXIT_CRITICAL();
    BX    LR                        ; return;                                       ; Enable interrupts at processor level
	
	align 4                    ;//内存对齐指令(编译器提供的)，以4个字节(32位)对齐
    end                        ;//伪指令,放在程序行的最后,告诉编译器编译程序到此结束
		
		
		
