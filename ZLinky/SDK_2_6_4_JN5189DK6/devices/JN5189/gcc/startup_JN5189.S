/* --------------------------------------------------------------------------*/
/* @file:    startup_JN5189.S                                                */
/* @purpose: CMSIS Cortex-M4 Core Device Startup File                        */
/*           JN5189                                                          */
/* @version: 1.0                                                      */
/* @date:    2018-7-31                                                         */
/* --------------------------------------------------------------------------*/
/*                                                                           */
/* Copyright 1997-2016 Freescale Semiconductor, Inc.                         */
/* Copyright 2016-2018 NXP                                                   */
/* All rights reserved.                                                      */
/*                                                                           */
/* SPDX-License-Identifier: BSD-3-Clause                                     */
/*****************************************************************************/
/* Version: GCC for ARM Embedded Processors                                  */
/*****************************************************************************/


    .syntax unified
    .arch armv7-m

    .section .isr_vector, "a"
    .align 2
    .globl __Vectors
__Vectors:
    .long   __StackTop                                      /* Top of Stack */
    .long   Reset_Handler                                   /* Reset Handler */
    .long   NMI_Handler                                     /* NMI Handler */
    .long   HardFault_Handler                               /* Hard Fault Handler */
    .long   MemManage_Handler                               /* MPU Fault Handler */
    .long   BusFault_Handler                                /* Bus Fault Handler */
    .long   UsageFault_Handler                              /* Usage Fault Handler */
    .long   0                                               /* Reserved */
    .long   0                                               /* Reserved */
    .long   0                                               /* Reserved */
    .long   0                                               /* Reserved */
    .long   SVC_Handler                                     /* SVCall Handler */
    .long   DebugMon_Handler                                /* Debug Monitor Handler */
    .long   0                                               /* Reserved */
    .long   PendSV_Handler                                  /* PendSV Handler */
    .long   SysTick_Handler                                 /* SysTick Handler */

    /* External Interrupts */
    .long   WDT_BOD_IRQHandler                     /* System (BOD, Watchdog Timer, Flash controller) interrupt */
    .long   DMA0_IRQHandler                     /* DMA interrupt */
    .long   GINT0_IRQHandler                     /* GPIO global interrupt */
    .long   CIC_IRB_IRQHandler                     /* Infra Red Blaster interrupt */
    .long   PIN_INT0_IRQHandler                     /* Pin Interrupt and Pattern matching 0 */
    .long   PIN_INT1_IRQHandler                     /* Pin Interrupt and Pattern matching 1 */
    .long   PIN_INT2_IRQHandler                     /* Pin Interrupt and Pattern matching 2 */
    .long   PIN_INT3_IRQHandler                     /* Pin Interrupt and Pattern matching 3 */
    .long   SPIFI0_IRQHandler                     /* Quad-SPI flash interface interrupt */
    .long   CTIMER0_IRQHandler                     /* Counter/Timer 0 interrupt */
    .long   CTIMER1_IRQHandler                     /* Counter/Timer 1 interrupt */
    .long   FLEXCOMM0_IRQHandler                     /* Flexcomm Interface 0 (USART0, FLEXCOMM0) */
    .long   FLEXCOMM1_IRQHandler                     /* Flexcomm Interface 1 (USART1, FLEXCOMM1) */
    .long   FLEXCOMM2_IRQHandler                     /* Flexcomm Interface 2 (I2C0, FLEXCOMM2) */
    .long   FLEXCOMM3_IRQHandler                     /* Flexcomm Interface 3 (I2C1, FLEXCOMM3) */
    .long   FLEXCOMM4_IRQHandler                     /* Flexcomm Interface 4 (SPI0, FLEXCOMM4) */
    .long   FLEXCOMM5_IRQHandler                     /* Flexcomm Interface 5 (SPI5, FLEXCOMM) */
    .long   PWM0_IRQHandler                     /* PWM channel 0 interrupt */
    .long   PWM1_IRQHandler                     /* PWM channel 1 interrupt */
    .long   PWM2_IRQHandler                     /* PWM channel 2 interrupt */
    .long   PWM3_IRQHandler                     /* PWM channel 3 interrupt */
    .long   PWM4_IRQHandler                     /* PWM channel 4 interrupt */
    .long   PWM5_IRQHandler                     /* PWM channel 5 interrupt */
    .long   PWM6_IRQHandler                     /* PWM channel 6  interrupt */
    .long   PWM7_IRQHandler                     /* PWM channel 7 interrupt */
    .long   PWM8_IRQHandler                     /* PWM channel 8 interrupt */
    .long   PWM9_IRQHandler                     /* PWM channel 9 interrupt */
    .long   PWM10_IRQHandler                     /* PWM channel 10 interrupt */
    .long   FLEXCOMM6_IRQHandler                     /* Flexcomm Interface6 (I2C2, FLEXCOMM6) */
    .long   RTC_IRQHandler                     /* Real Time Clock interrupt */
    .long   NFCTag_IRQHandler                     /* NFC Tag interrupt */
    .long   MAILBOX_IRQHandler                     /* Mailbox interrupts, Wake-up from Deep Sleep interrupt */
    .long   ADC0_SEQA_IRQHandler                     /* ADC Sequence A interrupt */
    .long   ADC0_SEQB_IRQHandler                     /* ADC Sequence B interrupt */
    .long   ADC0_THCMP_IRQHandler                     /* ADC Threshold compare and overrun interrupt */
    .long   DMIC0_IRQHandler                     /* DMIC interrupt */
    .long   HWVAD0_IRQHandler                     /* Hardware Voice activity detection interrupt */
    .long   BLE_DP_IRQHandler                     /* BLE Data Path interrupt */
    .long   BLE_DP0_IRQHandler                     /* BLE Data Path interrupt 0 */
    .long   BLE_DP1_IRQHandler                     /* BLE Data Path interrupt 1 */
    .long   BLE_DP2_IRQHandler                     /* BLE Data Path interrupt 2 */
    .long   BLE_LL_ALL_IRQHandler                     /* All BLE link layer interrupts */
    .long   ZIGBEE_MAC_IRQHandler                     /* Zigbee MAC interrupt */
    .long   ZIGBEE_MODEM_IRQHandler                     /* Zigbee MoDem interrupt */
    .long   RFP_TMU_IRQHandler                     /* RFP Timing Managemnt Unit (TMU) interrupt */
    .long   RFP_AGC_IRQHandler                     /* RFP AGC interrupt */
    .long   ISO7816_IRQHandler                     /* ISO7816 controller interrupt */
    .long   ANA_COMP_IRQHandler                     /* Analog Comparator interrupt */
    .long   WAKE_UP_TIMER0_IRQHandler                     /* Wake up Timer 0 interrupt */
    .long   WAKE_UP_TIMER1_IRQHandler                     /* Wake up Timer 1 interrupt */
    .long   PVTVF0_AMBER_IRQHandler                     /* PVT Monitor interrupt */
    .long   PVTVF0_RED_IRQHandler                     /* PVT Monitor interrupt */
    .long   PVTVF1_AMBER_IRQHandler                     /* PVT Monitor interrupt */
    .long   PVTVF1_RED_IRQHandler                     /* PVT Monitor interrupt */
    .long   BLE_WAKE_UP_TIMER_IRQHandler                     /* BLE Wake up Timer interrupt */
    .long   SHA_IRQHandler                     /* SHA interrupt */


    .size    __Vectors, . - __Vectors



    .text
    .thumb

/* Reset Handler */

    .thumb_func
    .align 2
    .globl   Reset_Handler
    .weak    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
#ifndef __NO_SYSTEM_INIT
    movs  r0,#56
    ldr   r1, =0x40000220
    str   r0, [r1]           /* Enable SRAM clock used by Stack */
    ldr   r0,=ResetISR
    blx   r0
#endif

    /*      Loop to copy data from read only memory to RAM. The ranges
     *      of copy from/to are specified by following symbols evaluated in
     *      linker script.
     *      __etext: End of code section, i.e., begin of data sections to copy from.
     *      __data_start__/__data_end__: RAM address range that data should be
     *      copied to. Both must be aligned to 4 bytes boundary.  */

    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

#if 1
/* Here are two copies of loop implemenations. First one favors code size
 * and the second one favors performance. Default uses the first one.
 * Change to "#if 0" to use the second one */
.LC0:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC0
#else
    subs    r3, r2
    ble    .LC1
.LC0:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .LC0
.LC1:
#endif

#ifdef __STARTUP_CLEAR_BSS
/*     This part of work usually is done in C library startup code. Otherwise,
 *     define this macro to enable it in this startup.
 *
 *     Loop to zero out BSS section, which uses following symbols
 *     in linker script:
 *      __bss_start__: start of BSS section. Must align to 4
 *      __bss_end__: end of BSS section. Must align to 4
 */
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__

    movs    r0, 0
.LC2:
    cmp     r1, r2
    itt    lt
    strlt   r0, [r1], #4
    blt    .LC2
#endif /* __STARTUP_CLEAR_BSS */

#ifndef __START
#define __START _start
#endif
#ifndef __ATOLLIC__
    ldr   r0,=__START
    blx   r0
#else
    ldr   r0,=__libc_init_array
    blx   r0
    ldr   r0,=main
    bx    r0
#endif

    .pool
    .size Reset_Handler, . - Reset_Handler

    .align  1
    .thumb_func
    .weak DefaultISR
    .type DefaultISR, %function
DefaultISR:
    b DefaultISR
    .size DefaultISR, . - DefaultISR

    .align 1
    .thumb_func
    .weak NMI_Handler
    .type NMI_Handler, %function
NMI_Handler:
    ldr   r0,=NMI_Handler
    bx    r0
    .size NMI_Handler, . - NMI_Handler

    .align 1
    .thumb_func
    .weak HardFault_Handler
    .type HardFault_Handler, %function
HardFault_Handler:
    ldr   r0,=HardFault_Handler
    bx    r0
    .size HardFault_Handler, . - HardFault_Handler

    .align 1
    .thumb_func
    .weak MemManage_Handler
    .type MemManage_Handler, %function
MemManage_Handler:
    ldr   r0,=MemManage_Handler
    bx    r0
    .size MemManage_Handler, . - MemManage_Handler

    .align 1
    .thumb_func
    .weak BusFault_Handler
    .type BusFault_Handler, %function
BusFault_Handler:
    ldr   r0,=BusFault_Handler
    bx    r0
    .size BusFault_Handler, . - BusFault_Handler

    .align 1
    .thumb_func
    .weak UsageFault_Handler
    .type UsageFault_Handler, %function
UsageFault_Handler:
    ldr   r0,=UsageFault_Handler
    bx    r0
    .size UsageFault_Handler, . - UsageFault_Handler

    .align 1
    .thumb_func
    .weak SVC_Handler
    .type SVC_Handler, %function

SVC_Handler:
    ldr   r0,=SVC_Handler
    bx    r0
    .size SVC_Handler, . - SVC_Handler
    .align 1
    .thumb_func
    .weak DebugMon_Handler
    .type DebugMon_Handler, %function
DebugMon_Handler:
    ldr   r0,=DebugMon_Handler
    bx    r0
    .size DebugMon_Handler, . - DebugMon_Handler

    .align 1
    .thumb_func
    .weak PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:
    ldr   r0,=PendSV_Handler
    bx    r0
    .size PendSV_Handler, . - PendSV_Handler

    .align 1
    .thumb_func
    .weak SysTick_Handler
    .type SysTick_Handler, %function
SysTick_Handler:
    ldr   r0,=SysTick_Handler
    bx    r0
    .size SysTick_Handler, . - SysTick_Handler

    .align 1
    .thumb_func
    .weak WDT_BOD_IRQHandler
    .type WDT_BOD_IRQHandler, %function
WDT_BOD_IRQHandler:
    ldr   r0,=WDT_BOD_DriverIRQHandler
    bx    r0
    .size WDT_BOD_IRQHandler, . - WDT_BOD_IRQHandler

    .align 1
    .thumb_func
    .weak DMA0_IRQHandler
    .type DMA0_IRQHandler, %function
DMA0_IRQHandler:
    ldr   r0,=DMA0_DriverIRQHandler
    bx    r0
    .size DMA0_IRQHandler, . - DMA0_IRQHandler

    .align 1
    .thumb_func
    .weak GINT0_IRQHandler
    .type GINT0_IRQHandler, %function
GINT0_IRQHandler:
    ldr   r0,=GINT0_DriverIRQHandler
    bx    r0
    .size GINT0_IRQHandler, . - GINT0_IRQHandler

    .align 1
    .thumb_func
    .weak CIC_IRB_IRQHandler
    .type CIC_IRB_IRQHandler, %function
CIC_IRB_IRQHandler:
    ldr   r0,=CIC_IRB_DriverIRQHandler
    bx    r0
    .size CIC_IRB_IRQHandler, . - CIC_IRB_IRQHandler

    .align 1
    .thumb_func
    .weak PIN_INT0_IRQHandler
    .type PIN_INT0_IRQHandler, %function
PIN_INT0_IRQHandler:
    ldr   r0,=PIN_INT0_DriverIRQHandler
    bx    r0
    .size PIN_INT0_IRQHandler, . - PIN_INT0_IRQHandler

    .align 1
    .thumb_func
    .weak PIN_INT1_IRQHandler
    .type PIN_INT1_IRQHandler, %function
PIN_INT1_IRQHandler:
    ldr   r0,=PIN_INT1_DriverIRQHandler
    bx    r0
    .size PIN_INT1_IRQHandler, . - PIN_INT1_IRQHandler

    .align 1
    .thumb_func
    .weak PIN_INT2_IRQHandler
    .type PIN_INT2_IRQHandler, %function
PIN_INT2_IRQHandler:
    ldr   r0,=PIN_INT2_DriverIRQHandler
    bx    r0
    .size PIN_INT2_IRQHandler, . - PIN_INT2_IRQHandler

    .align 1
    .thumb_func
    .weak PIN_INT3_IRQHandler
    .type PIN_INT3_IRQHandler, %function
PIN_INT3_IRQHandler:
    ldr   r0,=PIN_INT3_DriverIRQHandler
    bx    r0
    .size PIN_INT3_IRQHandler, . - PIN_INT3_IRQHandler

    .align 1
    .thumb_func
    .weak SPIFI0_IRQHandler
    .type SPIFI0_IRQHandler, %function
SPIFI0_IRQHandler:
    ldr   r0,=SPIFI0_DriverIRQHandler
    bx    r0
    .size SPIFI0_IRQHandler, . - SPIFI0_IRQHandler

    .align 1
    .thumb_func
    .weak CTIMER0_IRQHandler
    .type CTIMER0_IRQHandler, %function
CTIMER0_IRQHandler:
    ldr   r0,=CTIMER0_DriverIRQHandler
    bx    r0
    .size CTIMER0_IRQHandler, . - CTIMER0_IRQHandler

    .align 1
    .thumb_func
    .weak CTIMER1_IRQHandler
    .type CTIMER1_IRQHandler, %function
CTIMER1_IRQHandler:
    ldr   r0,=CTIMER1_DriverIRQHandler
    bx    r0
    .size CTIMER1_IRQHandler, . - CTIMER1_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXCOMM0_IRQHandler
    .type FLEXCOMM0_IRQHandler, %function
FLEXCOMM0_IRQHandler:
    ldr   r0,=FLEXCOMM0_DriverIRQHandler
    bx    r0
    .size FLEXCOMM0_IRQHandler, . - FLEXCOMM0_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXCOMM1_IRQHandler
    .type FLEXCOMM1_IRQHandler, %function
FLEXCOMM1_IRQHandler:
    ldr   r0,=FLEXCOMM1_DriverIRQHandler
    bx    r0
    .size FLEXCOMM1_IRQHandler, . - FLEXCOMM1_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXCOMM2_IRQHandler
    .type FLEXCOMM2_IRQHandler, %function
FLEXCOMM2_IRQHandler:
    ldr   r0,=FLEXCOMM2_DriverIRQHandler
    bx    r0
    .size FLEXCOMM2_IRQHandler, . - FLEXCOMM2_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXCOMM3_IRQHandler
    .type FLEXCOMM3_IRQHandler, %function
FLEXCOMM3_IRQHandler:
    ldr   r0,=FLEXCOMM3_DriverIRQHandler
    bx    r0
    .size FLEXCOMM3_IRQHandler, . - FLEXCOMM3_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXCOMM4_IRQHandler
    .type FLEXCOMM4_IRQHandler, %function
FLEXCOMM4_IRQHandler:
    ldr   r0,=FLEXCOMM4_DriverIRQHandler
    bx    r0
    .size FLEXCOMM4_IRQHandler, . - FLEXCOMM4_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXCOMM5_IRQHandler
    .type FLEXCOMM5_IRQHandler, %function
FLEXCOMM5_IRQHandler:
    ldr   r0,=FLEXCOMM5_DriverIRQHandler
    bx    r0
    .size FLEXCOMM5_IRQHandler, . - FLEXCOMM5_IRQHandler

    .align 1
    .thumb_func
    .weak PWM0_IRQHandler
    .type PWM0_IRQHandler, %function
PWM0_IRQHandler:
    ldr   r0,=PWM0_DriverIRQHandler
    bx    r0
    .size PWM0_IRQHandler, . - PWM0_IRQHandler

    .align 1
    .thumb_func
    .weak PWM1_IRQHandler
    .type PWM1_IRQHandler, %function
PWM1_IRQHandler:
    ldr   r0,=PWM1_DriverIRQHandler
    bx    r0
    .size PWM1_IRQHandler, . - PWM1_IRQHandler

    .align 1
    .thumb_func
    .weak PWM2_IRQHandler
    .type PWM2_IRQHandler, %function
PWM2_IRQHandler:
    ldr   r0,=PWM2_DriverIRQHandler
    bx    r0
    .size PWM2_IRQHandler, . - PWM2_IRQHandler

    .align 1
    .thumb_func
    .weak PWM3_IRQHandler
    .type PWM3_IRQHandler, %function
PWM3_IRQHandler:
    ldr   r0,=PWM3_DriverIRQHandler
    bx    r0
    .size PWM3_IRQHandler, . - PWM3_IRQHandler

    .align 1
    .thumb_func
    .weak PWM4_IRQHandler
    .type PWM4_IRQHandler, %function
PWM4_IRQHandler:
    ldr   r0,=PWM4_DriverIRQHandler
    bx    r0
    .size PWM4_IRQHandler, . - PWM4_IRQHandler

    .align 1
    .thumb_func
    .weak PWM5_IRQHandler
    .type PWM5_IRQHandler, %function
PWM5_IRQHandler:
    ldr   r0,=PWM5_DriverIRQHandler
    bx    r0
    .size PWM5_IRQHandler, . - PWM5_IRQHandler

    .align 1
    .thumb_func
    .weak PWM6_IRQHandler
    .type PWM6_IRQHandler, %function
PWM6_IRQHandler:
    ldr   r0,=PWM6_DriverIRQHandler
    bx    r0
    .size PWM6_IRQHandler, . - PWM6_IRQHandler

    .align 1
    .thumb_func
    .weak PWM7_IRQHandler
    .type PWM7_IRQHandler, %function
PWM7_IRQHandler:
    ldr   r0,=PWM7_DriverIRQHandler
    bx    r0
    .size PWM7_IRQHandler, . - PWM7_IRQHandler

    .align 1
    .thumb_func
    .weak PWM8_IRQHandler
    .type PWM8_IRQHandler, %function
PWM8_IRQHandler:
    ldr   r0,=PWM8_DriverIRQHandler
    bx    r0
    .size PWM8_IRQHandler, . - PWM8_IRQHandler

    .align 1
    .thumb_func
    .weak PWM9_IRQHandler
    .type PWM9_IRQHandler, %function
PWM9_IRQHandler:
    ldr   r0,=PWM9_DriverIRQHandler
    bx    r0
    .size PWM9_IRQHandler, . - PWM9_IRQHandler

    .align 1
    .thumb_func
    .weak PWM10_IRQHandler
    .type PWM10_IRQHandler, %function
PWM10_IRQHandler:
    ldr   r0,=PWM10_DriverIRQHandler
    bx    r0
    .size PWM10_IRQHandler, . - PWM10_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXCOMM6_IRQHandler
    .type FLEXCOMM6_IRQHandler, %function
FLEXCOMM6_IRQHandler:
    ldr   r0,=FLEXCOMM6_DriverIRQHandler
    bx    r0
    .size FLEXCOMM6_IRQHandler, . - FLEXCOMM6_IRQHandler

    .align 1
    .thumb_func
    .weak RTC_IRQHandler
    .type RTC_IRQHandler, %function
RTC_IRQHandler:
    ldr   r0,=RTC_DriverIRQHandler
    bx    r0
    .size RTC_IRQHandler, . - RTC_IRQHandler

    .align 1
    .thumb_func
    .weak NFCTag_IRQHandler
    .type NFCTag_IRQHandler, %function
NFCTag_IRQHandler:
    ldr   r0,=NFCTag_DriverIRQHandler
    bx    r0
    .size NFCTag_IRQHandler, . - NFCTag_IRQHandler

    .align 1
    .thumb_func
    .weak MAILBOX_IRQHandler
    .type MAILBOX_IRQHandler, %function
MAILBOX_IRQHandler:
    ldr   r0,=MAILBOX_DriverIRQHandler
    bx    r0
    .size MAILBOX_IRQHandler, . - MAILBOX_IRQHandler

    .align 1
    .thumb_func
    .weak ADC0_SEQA_IRQHandler
    .type ADC0_SEQA_IRQHandler, %function
ADC0_SEQA_IRQHandler:
    ldr   r0,=ADC0_SEQA_DriverIRQHandler
    bx    r0
    .size ADC0_SEQA_IRQHandler, . - ADC0_SEQA_IRQHandler

    .align 1
    .thumb_func
    .weak ADC0_SEQB_IRQHandler
    .type ADC0_SEQB_IRQHandler, %function
ADC0_SEQB_IRQHandler:
    ldr   r0,=ADC0_SEQB_DriverIRQHandler
    bx    r0
    .size ADC0_SEQB_IRQHandler, . - ADC0_SEQB_IRQHandler

    .align 1
    .thumb_func
    .weak ADC0_THCMP_IRQHandler
    .type ADC0_THCMP_IRQHandler, %function
ADC0_THCMP_IRQHandler:
    ldr   r0,=ADC0_THCMP_DriverIRQHandler
    bx    r0
    .size ADC0_THCMP_IRQHandler, . - ADC0_THCMP_IRQHandler

    .align 1
    .thumb_func
    .weak DMIC0_IRQHandler
    .type DMIC0_IRQHandler, %function
DMIC0_IRQHandler:
    ldr   r0,=DMIC0_DriverIRQHandler
    bx    r0
    .size DMIC0_IRQHandler, . - DMIC0_IRQHandler

    .align 1
    .thumb_func
    .weak HWVAD0_IRQHandler
    .type HWVAD0_IRQHandler, %function
HWVAD0_IRQHandler:
    ldr   r0,=HWVAD0_DriverIRQHandler
    bx    r0
    .size HWVAD0_IRQHandler, . - HWVAD0_IRQHandler

    .align 1
    .thumb_func
    .weak BLE_DP_IRQHandler
    .type BLE_DP_IRQHandler, %function
BLE_DP_IRQHandler:
    ldr   r0,=BLE_DP_DriverIRQHandler
    bx    r0
    .size BLE_DP_IRQHandler, . - BLE_DP_IRQHandler

    .align 1
    .thumb_func
    .weak BLE_DP0_IRQHandler
    .type BLE_DP0_IRQHandler, %function
BLE_DP0_IRQHandler:
    ldr   r0,=BLE_DP0_DriverIRQHandler
    bx    r0
    .size BLE_DP0_IRQHandler, . - BLE_DP0_IRQHandler

    .align 1
    .thumb_func
    .weak BLE_DP1_IRQHandler
    .type BLE_DP1_IRQHandler, %function
BLE_DP1_IRQHandler:
    ldr   r0,=BLE_DP1_DriverIRQHandler
    bx    r0
    .size BLE_DP1_IRQHandler, . - BLE_DP1_IRQHandler

    .align 1
    .thumb_func
    .weak BLE_DP2_IRQHandler
    .type BLE_DP2_IRQHandler, %function
BLE_DP2_IRQHandler:
    ldr   r0,=BLE_DP2_DriverIRQHandler
    bx    r0
    .size BLE_DP2_IRQHandler, . - BLE_DP2_IRQHandler

    .align 1
    .thumb_func
    .weak BLE_LL_ALL_IRQHandler
    .type BLE_LL_ALL_IRQHandler, %function
BLE_LL_ALL_IRQHandler:
    ldr   r0,=BLE_LL_ALL_DriverIRQHandler
    bx    r0
    .size BLE_LL_ALL_IRQHandler, . - BLE_LL_ALL_IRQHandler

    .align 1
    .thumb_func
    .weak ZIGBEE_MAC_IRQHandler
    .type ZIGBEE_MAC_IRQHandler, %function
ZIGBEE_MAC_IRQHandler:
    ldr   r0,=ZIGBEE_MAC_DriverIRQHandler
    bx    r0
    .size ZIGBEE_MAC_IRQHandler, . - ZIGBEE_MAC_IRQHandler

    .align 1
    .thumb_func
    .weak ZIGBEE_MODEM_IRQHandler
    .type ZIGBEE_MODEM_IRQHandler, %function
ZIGBEE_MODEM_IRQHandler:
    ldr   r0,=ZIGBEE_MODEM_DriverIRQHandler
    bx    r0
    .size ZIGBEE_MODEM_IRQHandler, . - ZIGBEE_MODEM_IRQHandler

    .align 1
    .thumb_func
    .weak RFP_TMU_IRQHandler
    .type RFP_TMU_IRQHandler, %function
RFP_TMU_IRQHandler:
    ldr   r0,=RFP_TMU_DriverIRQHandler
    bx    r0
    .size RFP_TMU_IRQHandler, . - RFP_TMU_IRQHandler

    .align 1
    .thumb_func
    .weak RFP_AGC_IRQHandler
    .type RFP_AGC_IRQHandler, %function
RFP_AGC_IRQHandler:
    ldr   r0,=RFP_AGC_DriverIRQHandler
    bx    r0
    .size RFP_AGC_IRQHandler, . - RFP_AGC_IRQHandler

    .align 1
    .thumb_func
    .weak ISO7816_IRQHandler
    .type ISO7816_IRQHandler, %function
ISO7816_IRQHandler:
    ldr   r0,=ISO7816_DriverIRQHandler
    bx    r0
    .size ISO7816_IRQHandler, . - ISO7816_IRQHandler

    .align 1
    .thumb_func
    .weak ANA_COMP_IRQHandler
    .type ANA_COMP_IRQHandler, %function
ANA_COMP_IRQHandler:
    ldr   r0,=ANA_COMP_DriverIRQHandler
    bx    r0
    .size ANA_COMP_IRQHandler, . - ANA_COMP_IRQHandler

    .align 1
    .thumb_func
    .weak WAKE_UP_TIMER0_IRQHandler
    .type WAKE_UP_TIMER0_IRQHandler, %function
WAKE_UP_TIMER0_IRQHandler:
    ldr   r0,=WAKE_UP_TIMER0_DriverIRQHandler
    bx    r0
    .size WAKE_UP_TIMER0_IRQHandler, . - WAKE_UP_TIMER0_IRQHandler

    .align 1
    .thumb_func
    .weak WAKE_UP_TIMER1_IRQHandler
    .type WAKE_UP_TIMER1_IRQHandler, %function
WAKE_UP_TIMER1_IRQHandler:
    ldr   r0,=WAKE_UP_TIMER1_DriverIRQHandler
    bx    r0
    .size WAKE_UP_TIMER1_IRQHandler, . - WAKE_UP_TIMER1_IRQHandler

    .align 1
    .thumb_func
    .weak PVTVF0_AMBER_IRQHandler
    .type PVTVF0_AMBER_IRQHandler, %function
PVTVF0_AMBER_IRQHandler:
    ldr   r0,=PVTVF0_AMBER_DriverIRQHandler
    bx    r0
    .size PVTVF0_AMBER_IRQHandler, . - PVTVF0_AMBER_IRQHandler

    .align 1
    .thumb_func
    .weak PVTVF0_RED_IRQHandler
    .type PVTVF0_RED_IRQHandler, %function
PVTVF0_RED_IRQHandler:
    ldr   r0,=PVTVF0_RED_DriverIRQHandler
    bx    r0
    .size PVTVF0_RED_IRQHandler, . - PVTVF0_RED_IRQHandler

    .align 1
    .thumb_func
    .weak PVTVF1_AMBER_IRQHandler
    .type PVTVF1_AMBER_IRQHandler, %function
PVTVF1_AMBER_IRQHandler:
    ldr   r0,=PVTVF1_AMBER_DriverIRQHandler
    bx    r0
    .size PVTVF1_AMBER_IRQHandler, . - PVTVF1_AMBER_IRQHandler

    .align 1
    .thumb_func
    .weak PVTVF1_RED_IRQHandler
    .type PVTVF1_RED_IRQHandler, %function
PVTVF1_RED_IRQHandler:
    ldr   r0,=PVTVF1_RED_DriverIRQHandler
    bx    r0
    .size PVTVF1_RED_IRQHandler, . - PVTVF1_RED_IRQHandler

    .align 1
    .thumb_func
    .weak BLE_WAKE_UP_TIMER_IRQHandler
    .type BLE_WAKE_UP_TIMER_IRQHandler, %function
BLE_WAKE_UP_TIMER_IRQHandler:
    ldr   r0,=BLE_WAKE_UP_TIMER_DriverIRQHandler
    bx    r0
    .size BLE_WAKE_UP_TIMER_IRQHandler, . - BLE_WAKE_UP_TIMER_IRQHandler

    .align 1
    .thumb_func
    .weak SHA_IRQHandler
    .type SHA_IRQHandler, %function
SHA_IRQHandler:
    ldr   r0,=SHA_DriverIRQHandler
    bx    r0
    .size SHA_IRQHandler, . - SHA_IRQHandler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro def_irq_handler  handler_name
    .weak \handler_name
    .set  \handler_name, DefaultISR
    .endm
/* Exception Handlers */
    def_irq_handler    WDT_BOD_DriverIRQHandler                 /* System (BOD, Watchdog Timer, Flash controller) interrupt */
    def_irq_handler    DMA0_DriverIRQHandler                 /* DMA interrupt */
    def_irq_handler    GINT0_DriverIRQHandler                 /* GPIO global interrupt */
    def_irq_handler    CIC_IRB_DriverIRQHandler                 /* Infra Red Blaster interrupt */
    def_irq_handler    PIN_INT0_DriverIRQHandler                 /* Pin Interrupt and Pattern matching 0 */
    def_irq_handler    PIN_INT1_DriverIRQHandler                 /* Pin Interrupt and Pattern matching 1 */
    def_irq_handler    PIN_INT2_DriverIRQHandler                 /* Pin Interrupt and Pattern matching 2 */
    def_irq_handler    PIN_INT3_DriverIRQHandler                 /* Pin Interrupt and Pattern matching 3 */
    def_irq_handler    SPIFI0_DriverIRQHandler                 /* Quad-SPI flash interface interrupt */
    def_irq_handler    CTIMER0_DriverIRQHandler                 /* Counter/Timer 0 interrupt */
    def_irq_handler    CTIMER1_DriverIRQHandler                 /* Counter/Timer 1 interrupt */
    def_irq_handler    FLEXCOMM0_DriverIRQHandler                 /* Flexcomm Interface 0 (USART0, FLEXCOMM0) */
    def_irq_handler    FLEXCOMM1_DriverIRQHandler                 /* Flexcomm Interface 1 (USART1, FLEXCOMM1) */
    def_irq_handler    FLEXCOMM2_DriverIRQHandler                 /* Flexcomm Interface 2 (I2C0, FLEXCOMM2) */
    def_irq_handler    FLEXCOMM3_DriverIRQHandler                 /* Flexcomm Interface 3 (I2C1, FLEXCOMM3) */
    def_irq_handler    FLEXCOMM4_DriverIRQHandler                 /* Flexcomm Interface 4 (SPI0, FLEXCOMM4) */
    def_irq_handler    FLEXCOMM5_DriverIRQHandler                 /* Flexcomm Interface 5 (SPI5, FLEXCOMM) */
    def_irq_handler    PWM0_DriverIRQHandler                 /* PWM channel 0 interrupt */
    def_irq_handler    PWM1_DriverIRQHandler                 /* PWM channel 1 interrupt */
    def_irq_handler    PWM2_DriverIRQHandler                 /* PWM channel 2 interrupt */
    def_irq_handler    PWM3_DriverIRQHandler                 /* PWM channel 3 interrupt */
    def_irq_handler    PWM4_DriverIRQHandler                 /* PWM channel 4 interrupt */
    def_irq_handler    PWM5_DriverIRQHandler                 /* PWM channel 5 interrupt */
    def_irq_handler    PWM6_DriverIRQHandler                 /* PWM channel 6  interrupt */
    def_irq_handler    PWM7_DriverIRQHandler                 /* PWM channel 7 interrupt */
    def_irq_handler    PWM8_DriverIRQHandler                 /* PWM channel 8 interrupt */
    def_irq_handler    PWM9_DriverIRQHandler                 /* PWM channel 9 interrupt */
    def_irq_handler    PWM10_DriverIRQHandler                 /* PWM channel 10 interrupt */
    def_irq_handler    FLEXCOMM6_DriverIRQHandler                 /* Flexcomm Interface6 (I2C2, FLEXCOMM6) */
    def_irq_handler    RTC_DriverIRQHandler                 /* Real Time Clock interrupt */
    def_irq_handler    NFCTag_DriverIRQHandler                 /* NFC Tag interrupt */
    def_irq_handler    MAILBOX_DriverIRQHandler                 /* Mailbox interrupts, Wake-up from Deep Sleep interrupt */
    def_irq_handler    ADC0_SEQA_DriverIRQHandler                 /* ADC Sequence A interrupt */
    def_irq_handler    ADC0_SEQB_DriverIRQHandler                 /* ADC Sequence B interrupt */
    def_irq_handler    ADC0_THCMP_DriverIRQHandler                 /* ADC Threshold compare and overrun interrupt */
    def_irq_handler    DMIC0_DriverIRQHandler                 /* DMIC interrupt */
    def_irq_handler    HWVAD0_DriverIRQHandler                 /* Hardware Voice activity detection interrupt */
    def_irq_handler    BLE_DP_DriverIRQHandler                 /* BLE Data Path interrupt */
    def_irq_handler    BLE_DP0_DriverIRQHandler                 /* BLE Data Path interrupt 0 */
    def_irq_handler    BLE_DP1_DriverIRQHandler                 /* BLE Data Path interrupt 1 */
    def_irq_handler    BLE_DP2_DriverIRQHandler                 /* BLE Data Path interrupt 2 */
    def_irq_handler    BLE_LL_ALL_DriverIRQHandler                 /* All BLE link layer interrupts */
    def_irq_handler    ZIGBEE_MAC_DriverIRQHandler                 /* Zigbee MAC interrupt */
    def_irq_handler    ZIGBEE_MODEM_DriverIRQHandler                 /* Zigbee MoDem interrupt */
    def_irq_handler    RFP_TMU_DriverIRQHandler                 /* RFP Timing Managemnt Unit (TMU) interrupt */
    def_irq_handler    RFP_AGC_DriverIRQHandler                 /* RFP AGC interrupt */
    def_irq_handler    ISO7816_DriverIRQHandler                 /* ISO7816 controller interrupt */
    def_irq_handler    ANA_COMP_DriverIRQHandler                 /* Analog Comparator interrupt */
    def_irq_handler    WAKE_UP_TIMER0_DriverIRQHandler                 /* Wake up Timer 0 interrupt */
    def_irq_handler    WAKE_UP_TIMER1_DriverIRQHandler                 /* Wake up Timer 1 interrupt */
    def_irq_handler    PVTVF0_AMBER_DriverIRQHandler                 /* PVT Monitor interrupt */
    def_irq_handler    PVTVF0_RED_DriverIRQHandler                 /* PVT Monitor interrupt */
    def_irq_handler    PVTVF1_AMBER_DriverIRQHandler                 /* PVT Monitor interrupt */
    def_irq_handler    PVTVF1_RED_DriverIRQHandler                 /* PVT Monitor interrupt */
    def_irq_handler    BLE_WAKE_UP_TIMER_DriverIRQHandler                 /* BLE Wake up Timer interrupt */
    def_irq_handler    SHA_DriverIRQHandler                 /* SHA interrupt */

    .end
