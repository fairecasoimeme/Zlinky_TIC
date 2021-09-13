//*****************************************************************************
// JN518x Microcontroller Startup code for use with LPCXpresso IDE
//
// Version : 141204
//*****************************************************************************
//
// Copyright 1997-2016 Freescale Semiconductor, Inc.
// Copyright 2016-2017 NXP
// SPDX-License-Identifier: BSD-3-Clause
// --------------------------------------------------------------------------------------
// Version: GCC for ARM Embedded Processors
// ----------------------------------------------------------------------------------- */

#if defined(__cplusplus)
#ifdef __REDLIB__
#error Redlib does not support C++
#else
//*****************************************************************************
//
// The entry point for the C++ library startup
//
//*****************************************************************************
extern "C" {
extern void __libc_init_array(void);
}
#endif
#endif

#define WEAK __attribute__((weak))
#define ALIAS(f) __attribute__((weak, alias(#f)))

//*****************************************************************************
#if defined(__cplusplus)
extern "C" {
#endif

//*****************************************************************************
// Declaration of external SystemInit function
extern WEAK void SystemInit(void);

//*****************************************************************************
//
// Forward declaration of the default handlers. These are aliased.
// When the application defines a handler (with the same name), this will
// automatically take precedence over these weak definitions
//
//*****************************************************************************
void ResetISR(void);
WEAK void NMI_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void MemManage_Handler(void);
WEAK void BusFault_Handler(void);
WEAK void UsageFault_Handler(void);
WEAK void SVC_Handler(void);
WEAK void DebugMon_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void SysTick_Handler(void);
WEAK void IntDefaultHandler(void);

//*****************************************************************************
//
// Forward declaration of the specific IRQ handlers. These are aliased
// to the IntDefaultHandler, which is a 'forever' loop. When the application
// defines a handler (with the same name), this will automatically take
// precedence over these weak definitions
//
//*****************************************************************************
// Example...
// void WDT_IRQHandler(void) ALIAS(IntDefaultHandler);
void System_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMA0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void GINT_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void IRBlaster_IRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SPIFI_IRQHandler(void) ALIAS(IntDefaultHandler);
void CTIMER0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CTIMER1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM4_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM5_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PWM0_IRQHandler(void) ALIAS(IntDefaultHandler);
void PWM1_IRQHandler(void) ALIAS(IntDefaultHandler);
void PWM2_IRQHandler(void) ALIAS(IntDefaultHandler);
void PWM3_IRQHandler(void) ALIAS(IntDefaultHandler);
void PWM4_IRQHandler(void) ALIAS(IntDefaultHandler);
void PWM5_IRQHandler(void) ALIAS(IntDefaultHandler);
void PWM6_IRQHandler(void) ALIAS(IntDefaultHandler);
void PWM7_IRQHandler(void) ALIAS(IntDefaultHandler);
void PWM8_IRQHandler(void) ALIAS(IntDefaultHandler);
void PWM9_IRQHandler(void) ALIAS(IntDefaultHandler);
void PWM10_IRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM6_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void RTC_IRQHandler(void) ALIAS(IntDefaultHandler);
void NFCTag_IRQHandler(void) ALIAS(IntDefaultHandler);
void MAILBOX_IRQHandler(void) ALIAS(IntDefaultHandler);
void ADC_SEQA_IRQHandler(void) ALIAS(IntDefaultHandler);
void ADC_SEQB_IRQHandler(void) ALIAS(IntDefaultHandler);
void ADC_THCMP_OVR_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMIC0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void HWVAD0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void BLE_DP_IRQHandler(void) ALIAS(IntDefaultHandler);
void BLE_DP0_IRQHandler(void) ALIAS(IntDefaultHandler);
void BLE_DP1_IRQHandler(void) ALIAS(IntDefaultHandler);
void BLE_DP2_IRQHandler(void) ALIAS(IntDefaultHandler);
void BLE_LL_ALL_IRQHandler(void) ALIAS(IntDefaultHandler);
void ZIGBEE_MAC_IRQHandler(void) ALIAS(IntDefaultHandler);
void ZIGBEE_MODEM_IRQHandler(void) ALIAS(IntDefaultHandler);
void RFP_TMU_IRQHandler(void) ALIAS(IntDefaultHandler);
void RFP_AGC_IRQHandler(void) ALIAS(IntDefaultHandler);
void ISO7816_IRQHandler(void) ALIAS(IntDefaultHandler);
void ANA_COMP_IRQHandler(void) ALIAS(IntDefaultHandler);
void WAKE_UP_TIMER0_IRQHandler(void) ALIAS(IntDefaultHandler);
void WAKE_UP_TIMER1_IRQHandler(void) ALIAS(IntDefaultHandler);
void PVT_AMBER0_IRQHandler(void) ALIAS(IntDefaultHandler);
void PVT_RED0_IRQHandler(void) ALIAS(IntDefaultHandler);
void PVT_AMBER1_IRQHandler(void) ALIAS(IntDefaultHandler);
void PVT_RED1_IRQHandler(void) ALIAS(IntDefaultHandler);
void BLE_WakeupTimer_IRQHandler(void) ALIAS(IntDefaultHandler);  
#if !defined(CPU_JN518X_REV) || (CPU_JN518X_REV > 1)  
void SHA_IRQHandler(void) ALIAS(IntDefaultHandler);
#endif

/*
 * Forward declaration of the application IRQ handlers. When the application
 * defines a handler (with the same name), this will automatically take
 * precedence over weak definitions below
 */
void DMA0_IRQHandler(void) __attribute__((weak));
void GINT_IRQHandler(void) __attribute__((weak));
void PINT0_IRQHandler(void) __attribute__((weak));
void PINT1_IRQHandler(void) __attribute__((weak));
void PINT2_IRQHandler(void) __attribute__((weak));
void PINT3_IRQHandler(void) __attribute__((weak));
void CTIMER0_IRQHandler(void) __attribute__((weak));
void CTIMER1_IRQHandler(void) __attribute__((weak));
void FLEXCOMM0_IRQHandler(void) __attribute__((weak));
void FLEXCOMM1_IRQHandler(void) __attribute__((weak));
void FLEXCOMM2_IRQHandler(void) __attribute__((weak));
void FLEXCOMM3_IRQHandler(void) __attribute__((weak));
void FLEXCOMM4_IRQHandler(void) __attribute__((weak));
void FLEXCOMM5_IRQHandler(void) __attribute__((weak));
void FLEXCOMM6_IRQHandler(void) __attribute__((weak));
void DMIC0_IRQHandler(void) __attribute__((weak));
void HWVAD0_IRQHandler(void) __attribute__((weak));
#if !defined(CPU_JN518X_REV) || (CPU_JN518X_REV > 1)    
void SHA_IRQHandler(void) __attribute__((weak));
#endif
  
//*****************************************************************************
//
// The entry point for the application.
// __main() is the entry point for Redlib based applications
// main() is the entry point for Newlib based applications
//
//*****************************************************************************
#if defined(__REDLIB__)
extern void __main(void);
#endif
extern int main(void);
//*****************************************************************************
//
// External declaration for the pointer to the stack top from the Linker Script
//
//*****************************************************************************
extern void _vStackTop(void);

//*****************************************************************************
#if defined(__cplusplus)
} // extern "C"
#endif
//*****************************************************************************
//
// The vector table.
// This relies on the linker script to place at correct location in memory.
//
//*****************************************************************************
extern void (*const g_pfnVectors[])(void);
__attribute__((used, section(".isr_vector"))) void (*const g_pfnVectors[])(void) = {
    // Core Level - CM4
    &_vStackTop,        // The initial stack pointer
    ResetISR,           // The reset handler
    NMI_Handler,        // The NMI handler
    HardFault_Handler,  // The hard fault handler
    MemManage_Handler,  // The MPU fault handler
    BusFault_Handler,   // The bus fault handler
    UsageFault_Handler, // The usage fault handler
    0,                  // Reserved
    0,                  // Reserved
    0,                  // Reserved
    0,                  // Reserved
    SVC_Handler,        // SVCall handler
    DebugMon_Handler,   // Debug monitor handler
    0,                  // Reserved
    PendSV_Handler,     // The PendSV handler
    SysTick_Handler,    // The SysTick handler

    // JN518x specific handlers
    System_IRQHandler,     /*!< System (BOD, Watchdog Timer, Flash controller) interrupt */
    DMA0_DriverIRQHandler, /*!< DMA interrupt */
    GINT_IRQHandler,       /*!< GPIO group interrupt */
    IRBlaster_IRQHandler,  /*!< Infra Red Blaster */
    PINT0_IRQHandler,      /*!< Pin Interrupt (and Pattern matching) 0 */
    PINT1_IRQHandler,      /*!< Pin Interrupt (and Pattern matching) 1 */
    PINT2_IRQHandler,      /*!< Pin Interrupt (and Pattern matching) 2 */
    PINT3_IRQHandler,      /*!< Pin Interrupt (and Pattern matching) 3 */
    SPIFI_IRQHandler,      /*!< Quad-SPI flash interface interrupt */
    CTIMER0_IRQHandler,    /*!< Counter/Timer 0 interrupt */
    CTIMER1_IRQHandler,    /*!< Counter/Timer 1 interrupt */
    FLEXCOMM0_IRQHandler,
    FLEXCOMM1_IRQHandler,
    FLEXCOMM2_IRQHandler,
    FLEXCOMM3_IRQHandler,
    FLEXCOMM4_IRQHandler,
    FLEXCOMM5_IRQHandler,
    PWM0_IRQHandler,        /*!< PWM channel 0  interrupt */
    PWM1_IRQHandler,        /*!< PWM channel 1  interrupt */
    PWM2_IRQHandler,        /*!< PWM channel 2  interrupt */
    PWM3_IRQHandler,        /*!< PWM channel 3  interrupt */
    PWM4_IRQHandler,        /*!< PWM channel 4  interrupt */
    PWM5_IRQHandler,        /*!< PWM channel 5  interrupt */
    PWM6_IRQHandler,        /*!< PWM channel 6  interrupt */
    PWM7_IRQHandler,        /*!< PWM channel 7  interrupt */
    PWM8_IRQHandler,        /*!< PWM channel 8  interrupt */
    PWM9_IRQHandler,        /*!< PWM channel 9  interrupt */
    PWM10_IRQHandler,       /*!< PWM channel 10 interrupt */
    FLEXCOMM6_IRQHandler,
    RTC_IRQHandler,
    NFCTag_IRQHandler,
    MAILBOX_IRQHandler,
    ADC_SEQA_IRQHandler,
    ADC_SEQB_IRQHandler,
    ADC_THCMP_OVR_IRQHandler,
    DMIC0_IRQHandler,
    HWVAD0_IRQHandler,
    BLE_DP_IRQHandler,
    BLE_DP0_IRQHandler,
    BLE_DP1_IRQHandler,
    BLE_DP2_IRQHandler,
    BLE_LL_ALL_IRQHandler,
    ZIGBEE_MAC_IRQHandler,
    ZIGBEE_MODEM_IRQHandler,
    RFP_TMU_IRQHandler,
    RFP_AGC_IRQHandler,
    ISO7816_IRQHandler,
    ANA_COMP_IRQHandler,
    WAKE_UP_TIMER0_IRQHandler,
    WAKE_UP_TIMER1_IRQHandler,
    PVT_AMBER0_IRQHandler,
    PVT_RED0_IRQHandler,
    PVT_AMBER1_IRQHandler,
    PVT_RED1_IRQHandler,
    BLE_WakeupTimer_IRQHandler,  /*!<BLE Wakeup timer interrupt */
#if !defined(CPU_JN518X_REV) || (CPU_JN518X_REV > 1)  
    SHA_IRQHandler,              /*!< HASH/SHA interrupt */
#endif
}; /* End of g_pfnVectors */

//*****************************************************************************
// Functions to carry out the initialization of RW and BSS data sections. These
// are written as separate functions rather than being inlined within the
// ResetISR() function in order to cope with MCUs with multiple banks of
// memory.
//*****************************************************************************
__attribute__((section(".after_vectors"))) void data_init(unsigned int romstart, unsigned int start, unsigned int len)
{
    unsigned int *pulDest = (unsigned int *)start;
    unsigned int *pulSrc = (unsigned int *)romstart;
    unsigned int loop;
    for (loop = 0; loop < len; loop = loop + 4)
        *pulDest++ = *pulSrc++;
}

__attribute__((section(".after_vectors"))) void bss_init(unsigned int start, unsigned int len)
{
    unsigned int *pulDest = (unsigned int *)start;
    unsigned int loop;
    for (loop = 0; loop < len; loop = loop + 4)
        *pulDest++ = 0;
}

//*****************************************************************************
// The following symbols are constructs generated by the linker, indicating
// the location of various points in the "Global Section Table". This table is
// created by the linker via the Code Red managed linker script mechanism. It
// contains the load address, execution address and length of each RW data
// section and the execution and length of each BSS (zero initialized) section.
//*****************************************************************************
extern unsigned int __data_section_table;
extern unsigned int __data_section_table_end;
extern unsigned int __bss_section_table;
extern unsigned int __bss_section_table_end;

//*****************************************************************************
// Reset entry point for your code.
// Sets up a simple runtime environment and initializes the C/C++
// library.
//*****************************************************************************

#if 0
__attribute__ ((naked, section(".after_vectors.reset")))
void ResetISR(void) {
    asm volatile(
        ".set   cpu_ctrl,       0x40000800\t\n"
        ".set   coproc_boot,    0x40000804\t\n"
        ".set   coproc_stack,   0x40000808\t\n"
        "LDR    R0,=coproc_boot\t\n"    // load co-processor boot address (from CPBOOT)
        "LDR    R0,[R0]\t\n"            // get address to branch to
        "MOVS   R0,R0\t\n"              // Check if 0
        "BEQ.N  masterboot\t\n"         // if zero in boot reg, we just branch to  real reset
		"LDR    R1,=coproc_stack\t\n"   // load co-processor stack pointer (from CPSTACK)
		"LDR    R1,[R1]\t\n"
		"MOV    SP,R1\t\n"
        "BX     R0\t\n"                 // branch to boot address
    "masterboot:\t\n"
        "LDR    R0, =ResetISR2\t\n"     // jump to 'real' reset handler
        "BX     R0\t\n"
    );
}

__attribute__ ((section(".after_vectors")))
void
ResetISR2(void) {
#endif

__attribute__((section(".after_vectors.reset"))) void ResetISR(void)
{
    //
    // Copy the data sections from flash to SRAM.
    //
    unsigned int LoadAddr, ExeAddr, SectionLen;
    unsigned int *SectionTableAddr;

    // Load base address of Global Section Table
    SectionTableAddr = &__data_section_table;

    // Copy the data sections from flash to SRAM.
    while (SectionTableAddr < &__data_section_table_end)
    {
        LoadAddr = *SectionTableAddr++;
        ExeAddr = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        data_init(LoadAddr, ExeAddr, SectionLen);
    }
    // At this point, SectionTableAddr = &__bss_section_table;
    // Zero fill the bss segment
    while (SectionTableAddr < &__bss_section_table_end)
    {
        ExeAddr = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        bss_init(ExeAddr, SectionLen);
    }

    // Check to see if we are running the code from a non-zero
    // address (eg RAM, external flash), in which case we need
    // to modify the VTOR register to tell the CPU that the
    // vector table is located at a non-0x0 address.
    unsigned int *pSCB_VTOR = (unsigned int *)0xE000ED08;
    if ((unsigned int *)g_pfnVectors != (unsigned int *)0x00000000)
    {
        // CMSIS : SCB->VTOR = <address of vector table>
        *pSCB_VTOR = (unsigned int)g_pfnVectors;
    }

    if (SystemInit != 0)
    {
        SystemInit();
    }

#if defined(__cplusplus)
    //
    // Call C++ library initialisation
    //
    __libc_init_array();
#endif

#if defined(__REDLIB__)
    // Call the Redlib library, which in turn calls main()
    __main();
#else
    main();
#endif

    //
    // main() shouldn't return, but if it does, we'll just enter an infinite loop
    //
    while (1)
    {
        ;
    }
}

//*****************************************************************************
// Default exception handlers. Override the ones here by defining your own
// handler routines in your application code.
//*****************************************************************************
//*****************************************************************************
//
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
//
//*****************************************************************************
__attribute__((weak, section(".after_vectors"))) void IntDefaultHandler(void)
{
    while (1)
    {
    }
}
/*
 * Default core exception handlers. Override the ones here by defining your own
 * handler routines in your application code.
 */

__attribute__((weak, section(".after_vectors"))) void NMI_Handler(void)
{
    while (1)
    {
    }
}

__attribute__((weak, section(".after_vectors"))) void HardFault_Handler(void)
{
    while (1)
    {
    }
}

__attribute__((weak, section(".after_vectors"))) void MemManage_Handler(void)
{
    while (1)
    {
    }
}

__attribute__((weak, section(".after_vectors"))) void BusFault_Handler(void)
{
    while (1)
    {
    }
}

__attribute__((weak, section(".after_vectors"))) void UsageFault_Handler(void)
{
    while (1)
    {
    }
}

__attribute__((weak, section(".after_vectors"))) void SVC_Handler(void)
{
    while (1)
    {
    }
}

__attribute__((weak, section(".after_vectors"))) void DebugMon_Handler(void)
{
    while (1)
    {
    }
}

__attribute__((weak, section(".after_vectors"))) void PendSV_Handler(void)
{
    while (1)
    {
    }
}

__attribute__((weak, section(".after_vectors"))) void SysTick_Handler(void)
{
    while (1)
    {
    }
}

/*
 * Default application exception handlers. Override the ones here by defining your own
 * handler routines in your application code. These routines call driver exception handlers
 * or IntDefaultHandler() if no driver exception handler is included.
 */

__attribute__((weak)) void DMIC0_IRQHandler(void)
{
    DMIC0_DriverIRQHandler();
}

__attribute__((weak)) void HWVAD0_IRQHandler(void)
{
    HWVAD0_DriverIRQHandler();
}

__attribute__((weak)) void PINT0_IRQHandler(void)
{
    PIN_INT0_DriverIRQHandler();
}

__attribute__((weak)) void PINT1_IRQHandler(void)
{
    PIN_INT1_DriverIRQHandler();
}

__attribute__((weak)) void PINT2_IRQHandler(void)
{
    PIN_INT2_DriverIRQHandler();
}

__attribute__((weak)) void PINT3_IRQHandler(void)
{
    PIN_INT3_DriverIRQHandler();
}

__attribute__((weak)) void GINT_IRQHandler(void)
{
    GINT_DriverIRQHandler();
}

__attribute__((weak)) void DMA0_IRQHandler(void)
{
    DMA0_DriverIRQHandler();
}

__attribute__((weak)) void CTIMER0_IRQHandler(void)
{
    CTIMER0_DriverIRQHandler();
}

__attribute__((weak)) void CTIMER1_IRQHandler(void)
{
    CTIMER1_DriverIRQHandler();
}

__attribute__((weak)) void USART0_IRQHandler(void)
{
    FLEXCOMM0_DriverIRQHandler();
}

__attribute__((weak)) void FLEXCOMM0_IRQHandler(void)
{
    FLEXCOMM0_DriverIRQHandler();
}

__attribute__((weak)) void USART1_IRQHandler(void)
{
    FLEXCOMM1_DriverIRQHandler();
}

__attribute__((weak)) void FLEXCOMM1_IRQHandler(void)
{
    FLEXCOMM1_DriverIRQHandler();
}

__attribute__((weak)) void I2C0_IRQHandler(void)
{
    FLEXCOMM2_DriverIRQHandler();
}

__attribute__((weak)) void FLEXCOMM2_IRQHandler(void)
{
    FLEXCOMM2_DriverIRQHandler();
}

__attribute__((weak)) void I2C1_IRQHandler(void)
{
    FLEXCOMM3_DriverIRQHandler();
}

__attribute__((weak)) void FLEXCOMM3_IRQHandler(void)
{
    FLEXCOMM3_DriverIRQHandler();
}

#define SPI0_IRQHandler FLEXCOMM4_DriverIRQHandler

__attribute__((weak)) void FLEXCOMM4_IRQHandler(void)
{
    FLEXCOMM4_DriverIRQHandler();
}

#define SPI1_IRQHandler FLEXCOMM5_DriverIRQHandler

__attribute__((weak)) void FLEXCOMM5_IRQHandler(void)
{
    FLEXCOMM5_DriverIRQHandler();
}

__attribute__((weak)) void FLEXCOMM6_IRQHandler(void)
{
    FLEXCOMM6_DriverIRQHandler();
}

