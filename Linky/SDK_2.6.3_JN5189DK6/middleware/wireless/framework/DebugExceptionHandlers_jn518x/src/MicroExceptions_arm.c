/*****************************************************************************
 *
 * MODULE:             Microspecific
 *
 * COMPONENT:          Microspecific
 *
 * AUTHOR:             Wayne Ellis
 *
 * DESCRIPTION:        Sample JN517x Exception Handler Code
 *
 * $HeadURL:  $
 *
 * $Revision:  $
 *
 * $LastChangedBy: we1 $
 *
 * $LastChangedDate:  $
 *
 * $Id:  $
 *
 *****************************************************************************
 *
 * This software is owned by Jennic and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on Jennic products. You, and any third parties must reproduce
 * the copyright and warranty notice and any other legend of ownership on each
 * copy or partial copy of the software.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS". JENNIC MAKES NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
 * ACCURACY OR LACK OF NEGLIGENCE. JENNIC SHALL NOT, IN ANY CIRCUMSTANCES,
 * BE LIABLE FOR ANY DAMAGES, INCLUDING, BUT NOT LIMITED TO, SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON WHATSOEVER.
 *
 * Copyright Jennic Ltd. 2014 All rights reserved
 *
 ****************************************************************************/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include "jendefs.h"
#include "ARMcortexMxRegisters_JN51xx.h"
#include "DebugExceptionHandlers_jn518x.h"
#include "Debug.h"
#include "fsl_wwdt.h"
#include "fsl_debug_console.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
enum { R0, R1, R2, R3, R12, LR, PC, PSR, SIZE_OF_EXCEPTION_STACK};

// using AAPCS the parameter (the stack frame) will map to r0
#define MICRO_GET_EXCEPTION_STACK_FRAME()                                   \
{                                                                            \
    asm volatile("MRS R0, MSP");                                            \
}

/* TRAP instruction */
#define MICRO_TRAP()                                                        \
{                                                                           \
    asm volatile("BKPT 0;");                                                \
}

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
void vPrintSystemHandlerAndControlStateRegisterBitFields(uint32_t u32RegisterData);
void vPrintConfigurableFaultStatusRegisterBitFields(uint32_t u32RegisterData);
void vPrintHardFaultStatusRegisterBitFields(uint32_t u32RegisterData);
void vPrintRegisterBit(uint32_t u32BitSet);

void vDebugHardFaultHandler(uint32_t *pu32ExceptionArgs);
void vDebugBusFaultHandler(uint32_t *pu32ExceptionArgs);
void vDebugUsageFaultHandler(uint32_t *pu32ExceptionArgs);
void vDebugMemoryFaultHandler(uint32_t *pu32ExceptionArgs);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC void ExceptionUnwindStack(uint32_t * pt)
{
    extern void *_vStackTop;
    PRINTF("\r\n stack dump:\r\n");
    while ((uint32)pt < (uint32)&_vStackTop )
    {
        PRINTF("0x%x  0x%x \r\n",pt, *pt);
        pt++;
        WWDT_Refresh(WWDT);
    }
    PRINTF("BBC_SM_STATE 0x400b107c = 0x%x\r\n",(*(volatile uint32 *)0x400b107c));
    PRINTF("BBC_TXCTL    0x400b11c0 = 0x%x\r\n",(*(volatile uint32 *)0x400b11c0));
    PRINTF("BBC_RXCTL    0x400b12c0 = 0x%x\r\n",(*(volatile uint32 *)0x400b12c0));
    PRINTF("PHY_MCTRL    0x400b0018 = 0x%x\r\n",(*(volatile uint32 *)0x400b0018));
    PRINTF("APP: Watchdog halted device !\n");
    RESET_PeripheralReset(kWWDT_RST_SHIFT_RSTn);
    WWDT_Deinit(WWDT);
    while (1);
}
#if (defined(__MCUXPRESSO) && defined(__SEMIHOST_HARDFAULT_DISABLE)) || (!defined(__MCUXPRESSO))
void HardFault_Handler(void)
{
    MICRO_GET_EXCEPTION_STACK_FRAME();
    asm volatile("B vDebugHardFaultHandler");
}
#endif

void BusFault_Handler(void)
{
    MICRO_GET_EXCEPTION_STACK_FRAME();
    asm volatile("B vDebugBusFaultHandler");
}

void UsageFault_Handler(void)
{
    MICRO_GET_EXCEPTION_STACK_FRAME();
    asm volatile("B vDebugUsageFaultHandler");
}

void MemManage_Handler(void)
{
    MICRO_GET_EXCEPTION_STACK_FRAME();
    asm volatile("B vDebugMemoryFaultHandler");
}

void vDebugHardFaultHandler(uint32_t *pu32ExceptionArgs)
{
    volatile uint32_t u32RegisterData;
    
#ifdef DEBUG_ENABLE
    uint32_t u32ExceptionArgs[SIZE_OF_EXCEPTION_STACK];

    // make local copies
    u32ExceptionArgs[R0] = pu32ExceptionArgs[R0];
    u32ExceptionArgs[R1] = pu32ExceptionArgs[R1];
    u32ExceptionArgs[R2] = pu32ExceptionArgs[R2];
    u32ExceptionArgs[R3] = pu32ExceptionArgs[R3];
    u32ExceptionArgs[R12] = pu32ExceptionArgs[R12];
    u32ExceptionArgs[LR] = pu32ExceptionArgs[LR];
    u32ExceptionArgs[PC] = pu32ExceptionArgs[PC];
    u32ExceptionArgs[PSR] = pu32ExceptionArgs[PSR];

    vDebugNonInt ("\r\n **** Hard Fault Handler **** \r\n");
    vDebugNonInt ("\r\n **** Stack Register Dump **** \r\n");

    vDebugNonInt ("R0 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R0], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt ("R1 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R1], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("R2 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R2], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("R3 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R3], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("R12 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R12], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("LR [R14] = 0x");
    vDebugHexNonInt(u32ExceptionArgs[LR], 8);
    vDebugNonInt(" [Subroutine Call Return Address]\r\n");
    vDebugNonInt("PC [R15] = 0x");
    vDebugHexNonInt(u32ExceptionArgs[PC], 8);
    vDebugNonInt(" [Program counter]\r\n");
    vDebugNonInt("PSR = 0x");
    vDebugHexNonInt(u32ExceptionArgs[PSR], 8);
    vDebugNonInt("\r\n");
#endif

    // report REG_SYSTEM_HANDLER_CNTRL_STATE
    u32RegisterData = u32REG_Read(REG_SYSTEM_HANDLER_CNTRL_STATE);
    vDebugNonInt("REG_SYSTEM_HANDLER_CNTRL_STATE = 0x");
    vDebugHexNonInt(u32RegisterData, 8);
    vDebugNonInt("\r\n");

    vDebugNonInt("REG_SYSTEM_HANDLER_CNTRL_STATE Register Details\r\n");
    vPrintSystemHandlerAndControlStateRegisterBitFields(u32RegisterData);

    // read addresses before valid bits
    // BusFault Address Register contains the address of the location that generated a BusFault.
    // VALID bit of the BFSR is set to 1
    u32RegisterData = u32REG_Read(REG_BUSFAULT_ADDRESS);
    vDebugNonInt("REG_BUSFAULT_ADDRESS = 0x");
    vDebugHexNonInt(u32RegisterData, 8);
    vDebugNonInt("\r\n");

    // The MMFAR contains the address of the location that generated a MemManage fault
    u32RegisterData = u32REG_Read(REG_MEMMANAGE_FAULT_ADDRESS);
    vDebugNonInt("REG_MEMMANAGE_FAULT_ADDRESS = 0x");
    vDebugHexNonInt(u32RegisterData, 8);
    vDebugNonInt("\r\n");

    // Configurable Fault Status Register contains the reasons for the specific fault.
    u32RegisterData = u32REG_Read(REG_CONFIGURABLE_FAULT_STATUS);
    vDebugNonInt("REG_CONFIGURABLE_FAULT_STATUS = 0x");
    vDebugHexNonInt(u32RegisterData, 8);
    vDebugNonInt("\r\n");

    vDebugNonInt("REG_CONFIGURABLE_FAULT_STATUS Register Details\r\n");
    vPrintConfigurableFaultStatusRegisterBitFields(u32RegisterData);

    // hard fault.
    u32RegisterData = u32REG_Read(REG_HARDFAULT_STATUS);
    vDebugNonInt("REG_HARDFAULT_STATUS = 0x");
    vDebugHexNonInt(u32RegisterData, 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("REG_HARDFAULT_STATUS Register Details\r\n");
    vPrintHardFaultStatusRegisterBitFields(u32RegisterData);

    // Debug Fault Status Register
    u32RegisterData = u32REG_Read(REG_DEBUG_FAULT_STATUS);
    vDebugNonInt("REG_DEBUG_FAULT_STATUS = 0x");
    vDebugHexNonInt(u32RegisterData, 8);
    vDebugNonInt("\r\n");

    // Auxiliary Fault Status Register
    u32RegisterData = u32REG_Read(REG_AUXILIARY_FAULT_STATUS);
    vDebugNonInt("REG_AUXILIARY_FAULT_STATUS = 0x");
    vDebugHexNonInt(u32RegisterData, 8);
    vDebugNonInt("\r\n");

    if(u32REG_Read(REG_DEBUG_HALTING_STATUS_AND_CONTROL) & 0x1)
    {
        // if C_DEBUGEN == 1, debugger connected halt program execution
        MICRO_TRAP();
    }

    // enter endless loop or reset
#if (defined MICRO_EXCEPTION_RESET)
    vREG_SysWrite(REG_SYS_RSTCTRL, 0x2);
#else
    ExceptionUnwindStack((uint32_t *) __get_MSP());
#endif

}

void vDebugBusFaultHandler(uint32_t *pu32ExceptionArgs)
{

    volatile uint32_t u32RegisterData;

#ifdef DEBUG_ENABLE
    uint32_t u32ExceptionArgs[SIZE_OF_EXCEPTION_STACK];

    // make local copies
    u32ExceptionArgs[R0] = pu32ExceptionArgs[R0];
    u32ExceptionArgs[R1] = pu32ExceptionArgs[R1];
    u32ExceptionArgs[R2] = pu32ExceptionArgs[R2];
    u32ExceptionArgs[R3] = pu32ExceptionArgs[R3];
    u32ExceptionArgs[R12] = pu32ExceptionArgs[R12];
    u32ExceptionArgs[LR] = pu32ExceptionArgs[LR];
    u32ExceptionArgs[PC] = pu32ExceptionArgs[PC];
    u32ExceptionArgs[PSR] = pu32ExceptionArgs[PSR];

    vDebugNonInt ("\r\n **** Bus Fault Handler **** \r\n");
    vDebugNonInt ("\r\n **** Stack Register Dump **** \r\n");

    vDebugNonInt ("R0 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R0], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt ("R1 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R1], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("R2 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R2], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("R3 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R3], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("R12 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R12], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("LR [R14] = 0x");
    vDebugHexNonInt(u32ExceptionArgs[LR], 8);
    vDebugNonInt(" [Subroutine Call Return Address]\r\n");
    vDebugNonInt("PC [R15] = 0x");
    vDebugHexNonInt(u32ExceptionArgs[PC], 8);
    vDebugNonInt(" [Program counter]\r\n");
    vDebugNonInt("PSR = 0x");
    vDebugHexNonInt(u32ExceptionArgs[PSR], 8);
    vDebugNonInt("\r\n");
#endif

    // report REG_SYSTEM_HANDLER_CNTRL_STATE
    u32RegisterData = u32REG_Read(REG_SYSTEM_HANDLER_CNTRL_STATE);
    vDebugNonInt("REG_SYSTEM_HANDLER_CNTRL_STATE = 0x");
    vDebugHexNonInt(u32RegisterData, 8);

    vDebugNonInt("REG_SYSTEM_HANDLER_CNTRL_STATE Register Details\r\n");
    vPrintSystemHandlerAndControlStateRegisterBitFields(u32RegisterData);

    // read these before valid bits
    // BusFault Address Register contains the address of the location that generated a BusFault.
    // VALID bit of the BFSR is set to 1
    u32RegisterData = u32REG_Read(REG_BUSFAULT_ADDRESS);
    vDebugNonInt("REG_BUSFAULT_ADDRESS = 0x");
    vDebugHexNonInt(u32RegisterData, 8);
    vDebugNonInt("\r\n");

    // Configurable Fault Status Register contains the reasons for the specific fault.
    u32RegisterData = u32REG_Read(REG_CONFIGURABLE_FAULT_STATUS);
    vDebugNonInt("REG_CONFIGURABLE_FAULT_STATUS = 0x");
    vDebugHexNonInt(u32RegisterData, 8);
    vDebugNonInt("\r\n");

    vDebugNonInt("REG_CONFIGURABLE_FAULT_STATUS Register Details\r\n");
    vPrintConfigurableFaultStatusRegisterBitFields(u32RegisterData);

    if(u32REG_Read(REG_DEBUG_HALTING_STATUS_AND_CONTROL) & 0x1)
    {
        // if C_DEBUGEN == 1, debugger connected halt program execution
        MICRO_TRAP();
    }

    // enter endless loop or reset
#if (defined MICRO_EXCEPTION_RESET)
    vREG_SysWrite(REG_SYS_RSTCTRL, 0x2);
#else
    ExceptionUnwindStack((uint32_t *) __get_MSP());
#endif

}

void vDebugUsageFaultHandler(uint32_t *pu32ExceptionArgs)
{
    volatile uint32_t u32RegisterData;
    
#ifdef DEBUG_ENABLE
    uint32_t u32ExceptionArgs[SIZE_OF_EXCEPTION_STACK];

    // make local copies
    u32ExceptionArgs[R0] = pu32ExceptionArgs[R0];
    u32ExceptionArgs[R1] = pu32ExceptionArgs[R1];
    u32ExceptionArgs[R2] = pu32ExceptionArgs[R2];
    u32ExceptionArgs[R3] = pu32ExceptionArgs[R3];
    u32ExceptionArgs[R12] = pu32ExceptionArgs[R12];
    u32ExceptionArgs[LR] = pu32ExceptionArgs[LR];
    u32ExceptionArgs[PC] = pu32ExceptionArgs[PC];
    u32ExceptionArgs[PSR] = pu32ExceptionArgs[PSR];

    vDebugNonInt ("\r\n **** Usage Fault Handler **** \r\n");
    vDebugNonInt ("\r\n **** Stack Register Dump **** \r\n");

    vDebugNonInt ("R0 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R0], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt ("R1 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R1], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("R2 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R2], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("R3 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R3], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("R12 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R12], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("LR [R14] = 0x");
    vDebugHexNonInt(u32ExceptionArgs[LR], 8);
    vDebugNonInt(" [Subroutine Call Return Address]\r\n");
    vDebugNonInt("PC [R15] = 0x");
    vDebugHexNonInt(u32ExceptionArgs[PC], 8);
    vDebugNonInt(" [Program counter]\r\n");
    vDebugNonInt("PSR = 0x");
    vDebugHexNonInt(u32ExceptionArgs[PSR], 8);
    vDebugNonInt("\r\n");
#endif

    // report REG_SYSTEM_HANDLER_CNTRL_STATE
    u32RegisterData = u32REG_Read(REG_SYSTEM_HANDLER_CNTRL_STATE);
    vDebugNonInt("REG_SYSTEM_HANDLER_CNTRL_STATE = 0x");
    vDebugHexNonInt(u32RegisterData, 8);

    vDebugNonInt("REG_SYSTEM_HANDLER_CNTRL_STATE Register Details\r\n");
    vPrintSystemHandlerAndControlStateRegisterBitFields(u32RegisterData);

    // Configurable Fault Status Register contains the reasons for the specific fault.
    u32RegisterData = u32REG_Read(REG_CONFIGURABLE_FAULT_STATUS);
    vDebugNonInt("REG_CONFIGURABLE_FAULT_STATUS = 0x");
    vDebugHexNonInt(u32RegisterData, 8);
    vDebugNonInt("\r\n");

    vDebugNonInt("REG_CONFIGURABLE_FAULT_STATUS Register Details\r\n");
    vPrintConfigurableFaultStatusRegisterBitFields(u32RegisterData);

    if(u32REG_Read(REG_DEBUG_HALTING_STATUS_AND_CONTROL) & 0x1)
    {
        // if C_DEBUGEN == 1, debugger connected halt program execution
        MICRO_TRAP();
    }

    // enter endless loop or reset
#if (defined MICRO_EXCEPTION_RESET)
    vREG_SysWrite(REG_SYS_RSTCTRL, 0x2);
#else
    ExceptionUnwindStack((uint32_t *) __get_MSP());
#endif

}

void vDebugMemoryFaultHandler(uint32_t *pu32ExceptionArgs)
{
    volatile uint32_t u32RegisterData;
    
#ifdef DEBUG_ENABLE
    uint32_t u32ExceptionArgs[SIZE_OF_EXCEPTION_STACK];

    // make local copies
    u32ExceptionArgs[R0] = pu32ExceptionArgs[R0];
    u32ExceptionArgs[R1] = pu32ExceptionArgs[R1];
    u32ExceptionArgs[R2] = pu32ExceptionArgs[R2];
    u32ExceptionArgs[R3] = pu32ExceptionArgs[R3];
    u32ExceptionArgs[R12] = pu32ExceptionArgs[R12];
    u32ExceptionArgs[LR] = pu32ExceptionArgs[LR];
    u32ExceptionArgs[PC] = pu32ExceptionArgs[PC];
    u32ExceptionArgs[PSR] = pu32ExceptionArgs[PSR];

    vDebugNonInt ("\r\n **** Memory Fault Handler **** \r\n");
    vDebugNonInt ("\r\n **** Stack Register Dump **** \r\n");

    vDebugNonInt ("R0 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R0], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt ("R1 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R1], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("R2 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R2], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("R3 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R3], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("R12 = 0x");
    vDebugHexNonInt(u32ExceptionArgs[R12], 8);
    vDebugNonInt("\r\n");
    vDebugNonInt("LR [R14] = 0x");
    vDebugHexNonInt(u32ExceptionArgs[LR], 8);
    vDebugNonInt(" [Subroutine Call Return Address]\r\n");
    vDebugNonInt("PC [R15] = 0x");
    vDebugHexNonInt(u32ExceptionArgs[PC], 8);
    vDebugNonInt(" [Program counter]\r\n");
    vDebugNonInt("PSR = 0x");
    vDebugHexNonInt(u32ExceptionArgs[PSR], 8);
    vDebugNonInt("\r\n");
#endif

    // report REG_SYSTEM_HANDLER_CNTRL_STATE
    u32RegisterData = u32REG_Read(REG_SYSTEM_HANDLER_CNTRL_STATE);
    vDebugNonInt("REG_SYSTEM_HANDLER_CNTRL_STATE = 0x");
    vDebugHexNonInt(u32RegisterData, 8);

    vDebugNonInt("REG_SYSTEM_HANDLER_CNTRL_STATE Register Details\r\n");
    vPrintSystemHandlerAndControlStateRegisterBitFields(u32RegisterData);

    // The MMFAR contains the address of the location that generated a MemManage fault
    u32RegisterData = u32REG_Read(REG_MEMMANAGE_FAULT_ADDRESS);
    vDebugNonInt("REG_MEMMANAGE_FAULT_ADDRESS = 0x");
    vDebugHexNonInt(u32RegisterData, 8);
    vDebugNonInt("\r\n");

    // Configurable Fault Status Register contains the reasons for the specific fault.
    u32RegisterData = u32REG_Read(REG_CONFIGURABLE_FAULT_STATUS);
    vDebugNonInt("REG_CONFIGURABLE_FAULT_STATUS = 0x");
    vDebugHexNonInt(u32RegisterData, 8);
    vDebugNonInt("\r\n");

    vDebugNonInt("REG_CONFIGURABLE_FAULT_STATUS Register Details\r\n");
    vPrintConfigurableFaultStatusRegisterBitFields(u32RegisterData);

    if(u32REG_Read(REG_DEBUG_HALTING_STATUS_AND_CONTROL) & 0x1)
    {
        // if C_DEBUGEN == 1, debugger connected halt program execution
        MICRO_TRAP();
    }

    // enter endless loop or reset
#if (defined MICRO_EXCEPTION_RESET)
    vREG_SysWrite(REG_SYS_RSTCTRL, 0x2);
#else
    ExceptionUnwindStack((uint32_t *) __get_MSP());
#endif

}

void vDebugExceptionConvertImpreciseToPreciseError(void)
{
    uint32_t u32RegisterData;

    // imp errors can be made p errors by  setting DISDEFWBUF bit(write to 1)
    u32RegisterData = u32REG_Read(REG_AUXILIARY_CONTROL);
    U32_SET_BITS(&u32RegisterData, REG_AUXILIARY_CONTROL_DISDEFWBUF_MASK);
    vREG_Write(REG_AUXILIARY_CONTROL, u32RegisterData);

}

void vPrintSystemHandlerAndControlStateRegisterBitFields(uint32_t u32RegisterData)
{
    vDebugNonInt("REG_SYSTEM_HANDLER_CNTRL_STATE Fault Enable Indicators\r\n");

    vDebugNonInt("MEMFAULTEN : ");
    vPrintRegisterBit(u32RegisterData & REG_SYSTEM_HANDLER_CNTRL_STATE_MEMFAULTEN_MASK);
    vDebugNonInt("BUSFAULTEN : ");
    vPrintRegisterBit(u32RegisterData & REG_SYSTEM_HANDLER_CNTRL_STATE_BUSFAULTEN_MASK);
    vDebugNonInt("USGFAULTEN : ");
    vPrintRegisterBit(u32RegisterData & REG_SYSTEM_HANDLER_CNTRL_STATE_USGFAULTEN_MASK);

    vDebugNonInt("REG_SYSTEM_HANDLER_CNTRL_STATE Fault Active Indicators\r\n");

    vDebugNonInt("MEMFAULTACT : ");
    vPrintRegisterBit(u32RegisterData & REG_SYSTEM_HANDLER_CNTRL_STATE_MEMFAULTACT_MASK);
    vDebugNonInt("BUSFAULTACT : ");
    vPrintRegisterBit(u32RegisterData & REG_SYSTEM_HANDLER_CNTRL_STATE_BUSFAULTACT_MASK);
    vDebugNonInt("USGFAULTACT : ");
    vPrintRegisterBit(u32RegisterData & REG_SYSTEM_HANDLER_CNTRL_STATE_USGFAULTACT_MASK);

    vDebugNonInt("REG_SYSTEM_HANDLER_CNTRL_STATE Fault Pended Indicators\r\n");

    vDebugNonInt("MEMFAULTPENDED : ");
    vPrintRegisterBit(u32RegisterData & REG_SYSTEM_HANDLER_CNTRL_STATE_MEMFAULTPENDED_MASK);
    vDebugNonInt("BUSFAULTPENDED : ");
    vPrintRegisterBit(u32RegisterData & REG_SYSTEM_HANDLER_CNTRL_STATE_BUSFAULTPENDED_MASK);
    vDebugNonInt("USGFAULTPENDED : ");
    vPrintRegisterBit(u32RegisterData & REG_SYSTEM_HANDLER_CNTRL_STATE_USGFAULTPENDED_MASK);

}

void vPrintConfigurableFaultStatusRegisterBitFields(uint32_t u32RegisterData)
{
    vDebugNonInt("MemManage Fault Status Register\r\n");

    vDebugNonInt("IACCVIOL : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_MMFSR_IACCVIOL_MASK);
    vDebugNonInt("DACCVIOL : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_MMFSR_DACCVIOL_MASK);
    vDebugNonInt("MUNSTKERR : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_MMFSR_MUNSTKERR_MASK);
    vDebugNonInt("MSTKERR : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_MMFSR_MSTKERR_MASK);
    vDebugNonInt("MMARVALID : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_MMFSR_MMARVALID_MASK);

    vDebugNonInt("BusFault Status Register\r\n");

    vDebugNonInt("IBUSERR : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_BFSR_IBUSERR_MASK);
    vDebugNonInt("PRECISERR : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_BFSR_PRECISERR_MASK);
    // imp errors can be made p errors by  setting DISDEFWBUF bit(write to 1)
    vDebugNonInt("IMPRECISERR : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_BFSR_IMPRECISERR_MASK);
    vDebugNonInt("UNSTKERR : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_BFSR_UNSTKERR_MASK);
    vDebugNonInt("STKERR : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_BFSR_STKERR_MASK);
    vDebugNonInt("BFARVALID : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_BFSR_BFARVALID_MASK);

    vDebugNonInt("UsageFault Status Register\r\n");

    vDebugNonInt("UNDEFINSTR : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_UFSR_UNDEFINSTR_MASK);
    vDebugNonInt("INVSTATE : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_UFSR_INVSTATE_MASK);
    vDebugNonInt("INVPC : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_UFSR_INVPC_MASK);
    vDebugNonInt("NOCP : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_UFSR_NOCP_MASK);
    vDebugNonInt("UNALIGNED : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_UFSR_UNALIGNED_MASK);
    vDebugNonInt("DIVBYZERO : ");
    vPrintRegisterBit(u32RegisterData & REG_CONFIGURABLE_FAULT_STATUS_UFSR_DIVBYZERO_MASK);

}

void vPrintHardFaultStatusRegisterBitFields(uint32_t u32RegisterData)
{
    vDebugNonInt("HardFault Status Register\r\n");

    vDebugNonInt("VECTTBL : ");
    vPrintRegisterBit(u32RegisterData & REG_HARDFAULT_STATUS_VECTTBL_MASK);
    vDebugNonInt("FORCED : ");
    vPrintRegisterBit(u32RegisterData & REG_HARDFAULT_STATUS_FORCED_MASK);
    vDebugNonInt("DEBUGEVT : ");
    vPrintRegisterBit(u32RegisterData & REG_HARDFAULT_STATUS_DEBUGEVT_MASK);

}

void vPrintRegisterBit(uint32_t u32BitSet)
{
    if(u32BitSet != 0)
    {
      vDebugNonInt("1\r\n");
    }
    else
    {
      vDebugNonInt("0\r\n");
    }

}

/****************************************************************************/
/***        End of file                                                   ***/
/****************************************************************************/
