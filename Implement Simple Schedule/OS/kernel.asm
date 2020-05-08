
	IMPORT pTCB_Cur
	IMPORT pTCB_Rdy
	;IMPORT OS_CPU_ExceptStackBase	
	EXPORT PendSV_Handler
	EXPORT SP_INIT
	EXPORT ENTER_CRITICAL
	EXPORT EXIT_CRITICAL
		
	PRESERVE8                  ;//�ֽڶ���ؼ���,ָ����ǰ�ļ����ֽڶ��롣
	AREA |.text|, CODE, READONLY ;//����һ������λ����ݶΡ�
	THUMB                      ;//ָ������ָ���THUMBָ�(ARM����ж���ָ�)

EXIT_CRITICAL                  ;//�����ٽ��������ţ��ȼ�C���Ժ���void os_core_enter(void){}
    CPSID    I                 ;//�ر�ȫ���ж�(Cortex-M�ں˵�ȫ���жϿ���ָ��)
    BX    LR                   ;//�ȼ�C���ԣ�return
ENTER_CRITICAL                   ;//void os_core_exit(void){}
    CPSIE    I                 ;//��ȫ���ж�
    BX    LR                   ;//return   
	
SP_INIT  					   ;��ʼ��PSPָ��                      
    CPSID    I                 ;//�ر�ȫ���ж� 
	
    LDR R4,=0x0                ;//R4װ��������0(��ֱ�Ӹ�PSP��ֵ0���Ǿ���R�Ĵ�����Ϊý������ΪPSPֻ�ܺ�R�Ĵ����򽻵�)            
    MSR     PSP, R4            ;//PSP(process stack pointer)�����ջָ�븳ֵ0��PSP���û���(�ؼ�Ȩ��ΪMSP)��˫��ջ�ṹ�� 

	;LDR     R0, = OS_CPU_ExceptStackBase   ; Initialize the MSP to the OS_CPU_ExceptStkBase
	;LDR     R1, [R0]
	;MSR     MSP, R1
	
    CPSIE    I                 ;//��ȫ���ж�(��ʱ��û�������ж�����Ӧ������������PendSV�жϺ���)  
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
 
PendSV_Handler_NoSave				;ÿ�ζ����ȥ����ΪPendSV_Handler_NoSave���Ǻ����������м��һ����ǩ��������ת��

	;ʵ�ʾ���pTCB_Cur = pTCB_Rdy
	;ÿ������PendSV_Handler������ʹpTCB_Curָ��pTCB_Rdy�����Ե���ʱֻ������������л�ȡpTCB_Rdy
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
	
	align 4                    ;//�ڴ����ָ��(�������ṩ��)����4���ֽ�(32λ)����
    end                        ;//αָ��,���ڳ����е����,���߱�����������򵽴˽���
		
		
		
