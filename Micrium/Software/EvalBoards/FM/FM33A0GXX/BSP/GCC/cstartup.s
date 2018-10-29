@/*
@*********************************************************************************************************
@*                                              EXAMPLE CODE
@*
@*                          (c) Copyright 2011-2012; Micrium, Inc.; Weston, FL
@*
@*                   All rights reserved.  Protected by international copyright laws.
@*                   Knowledge of the source code may not be used to write a similar
@*                   product.  This file may only be used in accordance with a license
@*                   and should not be redistributed in any way.
@*********************************************************************************************************
@*/


@/*
@*********************************************************************************************************
@*
@*                                          STARTUP CODE
@*
@*                                     ST Microelectronics STM32F
@*                                              on the
@*
@*                                          STM320518-EVAL
@*                                         Evaluation Board
@*
@* Filename      : cstartup.c
@* Version       : V1.00.00
@* Programmer(s) : MD
@*
@*********************************************************************************************************
@*/


@/*
@********************************************************************************************************
@*                                              STACK DEFINITIONS
@*
@* Configuration Wizard Menu:
@* // <h> Stack Configuration
@* //   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
@* // </h>;
@*********************************************************************************************************
@*/

  .syntax unified
  .cpu cortex-m0
  .fpu softvfp
  .thumb

.global g_pfnVectors
.global Default_Handler

@ Start address for the initialization values of the .data section.
.word _sidata
@ Start address for the .data section.
.word _sdata
@ End address for the .data section.
.word _edata
@ Start address for the .bss section.
.word _sbss
@ End address for the .bss section.
.word _ebss

@ NOTE: All previous words are defined in the Linker Script.
.equ  BootRAM, 0xF108F85F


@/*
@**********************************************************************************************************
@ Description:      This is the code that gets called when the processor first
@                   starts execution following a reset event. Only the absolutely
@                   necessary set is performed, after which the application
@                   supplied main() routine is called.
@ Parameters:  None
@
@ Return Value: None
@**********************************************************************************************************
@*/

@ Reset Handler
  .section .text.Reset_Handler
  .weak Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:

  ldr   r0, =_estack
  mov   sp, r0          /* set stack pointer */

@ Copy the data segment initializers from flash to SRAM.
  movs r1, #0
  b LoopCopyDataInit

CopyDataInit:
  ldr r3, =_sidata
  ldr r3, [r3, r1]
  str r3, [r0, r1]
  adds r1, r1, #4

LoopCopyDataInit:
  ldr r0, =_sdata
  ldr r3, =_edata
  adds r2, r0, r1
  cmp r2, r3
  bcc CopyDataInit
  ldr r2, =_sbss
  b LoopFillZerobss

/* Zero fill the bss segment. */
FillZerobss:                                                    @ Zero fill the bss segment.
  movs r3, #0
  str  r3, [r2]
  adds r2, r2, #4


LoopFillZerobss:
  ldr r3, = _ebss
  cmp r2, r3
  bcc FillZerobss

@ Call the clock system intitialization function.
@    bl  SystemInit

@ Call static constructors
    bl __libc_init_array

@ Call the application's entry point.
    bl main
    bx lr
  
LoopForever:
    b LoopForever

.size Reset_Handler, .-Reset_Handler


@/*
@*********************************************************************************************************
@*                                  EXCEPTION / INTERRUPT VECTOR TABLE
@*
@* Note(s) : (1) The Cortex-M3 may have up to 256 external interrupts, which are the final entries in the
@*               vector table.  The STM32L1xxx has 81 external interrupt vectors.
@*
@*           (2) Interrupts vector 2-13 are implemented in this file as infinite loop for debuging
@*               purposes only. The application might implement a recover procedure if it is needed.
@*
@*           (3) OS_CPU_PendSVHandler() and OS_CPU_SysTickHandler() are implemented in the generic OS
@*               port.
@*********************************************************************************************************
@*/

@    .section .text.Default_Handler,"ax",%progbits
@Default_Handler:
@Infinite_Loop:
@  b Infinite_Loop
@  .size Default_Handler, .-Default_Handler

    .section    .isr_vector,    "a",    %progbits
    .type       g_pfnVectors,   %object
    .size       g_pfnVectors,   .-g_pfnVectors

                                                                @ Vector Table Mapped to Address 0 at Reset
                    .extern  BSP_IntHandlerWWDG
                    .extern  BSP_IntHandlerPVD
                    .extern  BSP_IntHandlerTAMP_STAMP
                    .extern  BSP_IntHandlerRTC_WKUP
                    .extern  BSP_IntHandlerFLASH
                    .extern  BSP_IntHandlerRCC
                    .extern  BSP_IntHandlerEXTI0
                    .extern  BSP_IntHandlerEXTI1
                    .extern  BSP_IntHandlerEXTI2
                    .extern  BSP_IntHandlerEXTI3
                    .extern  BSP_IntHandlerEXTI4
                    .extern  BSP_IntHandlerDMA1_CH1
                    .extern  BSP_IntHandlerDMA1_CH2
                    .extern  BSP_IntHandlerDMA1_CH3
                    .extern  BSP_IntHandlerDMA1_CH4
                    .extern  BSP_IntHandlerDMA1_CH5
                    .extern  BSP_IntHandlerDMA1_CH6
                    .extern  BSP_IntHandlerDMA1_CH7
                    .extern  BSP_IntHandlerADC1
                    .extern  BSP_IntHandlerUSB_HP
                    .extern  BSP_IntHandlerUSB_LP
                    .extern  BSP_IntHandlerDAC
                    .extern  BSP_IntHandlerCOMP

                    .extern  BSP_IntHandlerEXTI9_5
                    .extern  BSP_IntHandlerLCD
                    .extern  BSP_IntHandlerTIM9
                    .extern  BSP_IntHandlerTIM10
                    .extern  BSP_IntHandlerTIM11
                    .extern  BSP_IntHandlerTIM2
                    .extern  BSP_IntHandlerTIM3
                    .extern  BSP_IntHandlerTIM4
                    .extern  BSP_IntHandlerI2C1_EV
                    .extern  BSP_IntHandlerI2C1_ER
                    .extern  BSP_IntHandlerI2C2_EV
                    .extern  BSP_IntHandlerI2C2_ER
                    .extern  BSP_IntHandlerSPI1
                    .extern  BSP_IntHandlerSPI2
                    .extern  BSP_IntHandlerUSART1
                    .extern  BSP_IntHandlerUSART2
                    .extern  BSP_IntHandlerUSART3
                    .extern  BSP_IntHandlerEXTI15_10
                    .extern  BSP_IntHandlerRTCAlarm
                    .extern  BSP_IntHandlerUSB_FS_WKUP
                    .extern  BSP_IntHandlerTIM6
                    .extern  BSP_IntHandlerTIM7

                    .extern  BSP_IntHandlerSDIO
                    .extern  BSP_IntHandlerTIM5
                    .extern  BSP_IntHandlerSPI3
                    .extern  BSP_IntHandlerUART4
                    .extern  BSP_IntHandlerUART5
                    .extern  BSP_IntHandlerDMA2_CH1
                    .extern  BSP_IntHandlerDMA2_CH2
                    .extern  BSP_IntHandlerDMA2_CH3
                    .extern  BSP_IntHandlerDMA2_CH4
                    .extern  BSP_IntHandlerDMA2_CH5
                    .extern  BSP_IntHandlerAES
                    .extern  BSP_IntHandlerCOMP_ACQ

                    .extern  OS_CPU_PendSVHandler
                    .extern  OS_CPU_SysTickHandler


g_pfnVectors:                                                   @ External Interrupts
              .word _estack                                     @ Top of Stack
              .word Reset_Handler                               @ Reset Handle
              .word App_NMI_ISR                                 @ 2, NMI
              .word App_Fault_ISR                           @ 3, Hard Fault
              .word 0
              .word 0
              .word 0
              .word 0
              .word 0
              .word 0
              .word 0
              .word App_Fault_ISR                                 @ 11, SVCall
              .word 0
              .word 0
              .word OS_CPU_PendSVHandler                        @ 14, PendSV Handler
              .word OS_CPU_SysTickHandler                       @ 15, uC/OS-III Tick ISR Handler

              .word BSP_IntHandlerWWDG
              .word BSP_IntHandlerPVD
              .word BSP_IntHandlerRTC
              .word BSP_IntHandlerFLASH
              .word BSP_IntHandlerRCC
              .word BSP_IntHandlerEXTI0_1
              .word BSP_IntHandlerEXTI2_3
              .word BSP_IntHandlerEXTI4_15
              .word BSP_IntHandlerTSC
              .word BSP_IntHandlerDMA_CH1
              .word BSP_IntHandlerDMA_CH2_3
              .word BSP_IntHandlerDMA_CH4_5
              .word BSP_IntHandlerADC_COMP
              .word BSP_IntHandlerTIM1_BRK_UP_TRG_COM
              .word BSP_IntHandlerTIM1_CC
              .word BSP_IntHandlerTIM2
              .word BSP_IntHandlerTIM3
              .word BSP_IntHandlerTIM6_DAC
              .word 0
              .word BSP_IntHandlerTIM14
              .word BSP_IntHandlerTIM15
              .word BSP_IntHandlerTIM16
              .word BSP_IntHandlerTIM17
              .word BSP_IntHandlerI2C1
              .word BSP_IntHandlerI2C2
              .word BSP_IntHandlerSPI1
              .word BSP_IntHandlerSPI2
              .word BSP_IntHandlerUSART1
              .word BSP_IntHandlerUSART2
              .word 0
              .word BSP_IntHandlerCEC
              .word 0
              .word BootRAM                                     @  0x108. This is for boot in RAM mode for
                                                                @  STM32L15x ULtra Low Power Medium-density devices.


@*/
@/*
@*********************************************************************************************************
@ Provide weak aliases for each Exception handler to the Default_Handler.
@ As they are weak aliases, any function with the same name will override
@ this definition.
@
@*********************************************************************************************************
@*/

    .section    .text.App_NMI_ISR
App_NMI_ISR:
    b   App_NMI_ISR
    .size   App_NMI_ISR, .-App_NMI_ISR


    .section    .text.App_Fault_ISR
App_Fault_ISR:
    b   App_Fault_ISR
    .size   App_Fault_ISR, .-App_Fault_ISR


    .section    .text.App_MemFault_ISR
App_MemFault_ISR:
    b   App_MemFault_ISR
    .size   App_MemFault_ISR, .-App_MemFault_ISR


    .section    .text.App_BusFault_ISR
App_BusFault_ISR:
    b   App_BusFault_ISR
    .size   App_BusFault_ISR, .-App_BusFault_ISR


    .section    .text.App_UsageFault_ISR
App_UsageFault_ISR:
    b   App_UsageFault_ISR
    .size   App_UsageFault_ISR, .-App_UsageFault_ISR

    .section    .text.App_Spurious_ISR
App_Spurious_ISR:
    b   App_Spurious_ISR
    .size   App_Spurious_ISR, .-App_Spurious_ISR


    .section    .text.App_Reserved_ISR
App_Reserved_ISR:
    b   App_Reserved_ISR
    .size   App_Reserved_ISR, .-App_Reserved_ISR



