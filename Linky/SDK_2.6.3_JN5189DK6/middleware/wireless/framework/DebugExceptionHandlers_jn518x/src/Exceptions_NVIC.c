/*****************************************************************************
 *
 * MODULE:             JN51xx Exceptions NVIC
 *
 * COMPONENT:          Exception
 *
 * AUTHOR:             Wayne Ellis
 *
 * DESCRIPTION:        JN517x NVIC Exception routines
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

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <time.h>

#include <string.h>
#include <stdlib.h>
#include <jendefs.h>

#include <stdio.h>

#include "Debug.h"

#include "DebugExceptionHandlers_jn518x.h"
#include "ARMcortexMxRegisters_JN51xx.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

extern void vPrintSystemHandlerAndControlStateRegisterBitFields(uint32 u32RegisterData);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern void ( *g_pfnVectors[36])(void);

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

/****************************************************************************
 * NAME:        vDebugExceptionHandlersInitialise
 *
 * DESCRIPTION:
 * Initialise ARM NVIC for exceptions.
 *
 * RETURNS:
 * Nothing.
 *
 * NOTES:
 *
 ****************************************************************************/

PUBLIC void vDebugExceptionHandlersInitialise(void)
{
    uint32 u32RegisterValue=0;

    // initialise the print mechanism
    vDebugInit();

    // enable exception features - DIV0 and align errors
    U32_SET_BITS(
       &u32RegisterValue,
    /*    REG_CONFIGURATION_CONTROL_UNALIGN_TRP_MASK | */REG_CONFIGURATION_CONTROL_DIV_0_TRP_MASK);

    vREG_Write(REG_CONFIGURATION_CONTROL, u32RegisterValue);

    // set the exception priorities here - benign but listed so we know where they are
    // MemManage
    vREG_Write8(REG_SYSTEM_HANDLER_PRIORITY_4, 0);
    // BusFault
    vREG_Write8(REG_SYSTEM_HANDLER_PRIORITY_5, 0);
    // UsageFault
    vREG_Write8(REG_SYSTEM_HANDLER_PRIORITY_6, 0);
#if 0
    // jn518x uses these, except DM
    // SVC
    vREG_Write8(REG_SYSTEM_HANDLER_PRIORITY_11, 0);
    // debug monitor
    vREG_Write8(REG_SYSTEM_HANDLER_PRIORITY_12, 0);
    // pend SV
    vREG_Write8(REG_SYSTEM_HANDLER_PRIORITY_14, 0);
    // SYSTICK
    vREG_Write8(REG_SYSTEM_HANDLER_PRIORITY_15, 0);
#endif

    u32RegisterValue = u32REG_Read(REG_SYSTEM_HANDLER_CNTRL_STATE);

    U32_SET_BITS(
      &u32RegisterValue,
       REG_SYSTEM_HANDLER_CNTRL_STATE_USGFAULTEN_MASK |
	   REG_SYSTEM_HANDLER_CNTRL_STATE_BUSFAULTEN_MASK |
	   REG_SYSTEM_HANDLER_CNTRL_STATE_MEMFAULTEN_MASK);

    vREG_Write(REG_SYSTEM_HANDLER_CNTRL_STATE, u32RegisterValue);

}

/****************************************************************************
 **
 ** NAME:      vPrintNVICconfiguration
 **
 ** DESCRIPTION:
 ** prints NVIC register contents
 **
 ** PARAMETERS:        Name                            Usage
 ** None
 **
 ** RETURN:
 ** Nothing
 **
 ****************************************************************************/

PUBLIC void vPrintNVICconfiguration(void)
{
    int i;
    uint32 u32RegisterValue;

    vDebug("**************************************************************************\r\n");
	vDebug("**************   NVIC Exception CONFIGURATION INFORMATION  ***************\r\n");
	vDebug("**************************************************************************\r\n");

    u32RegisterValue = u32REG_Read(REG_INT_ICTR);
    vDebug("REG_INT_ICTR = 0x");
    vDebugHex(u32RegisterValue, 8);
    vDebug("\r\n");

    vDebug("Address Of g_pfnVectors = 0x");
    vDebugHex((uint32)g_pfnVectors, 8);
    vDebug("\r\n");

    // Test for alignment on an 64 byte boundary - TRUE if aligned
    if(IS_ALIGNED(g_pfnVectors, 0x3F) == FALSE)
    {
        vDebug("g_pfnVectors is MISaligned \r\n");
    }
    else
    {
        vDebug("g_pfnVectors is Aligned OK \r\n");
    }

    // print Exception table
    vDebug("\r\n *** Exception Table ***\r\n");
    for(i=0; i<16; i++)
    {
        vDebug("g_pfnVectors[0x");
        vDebugHex(i, 2);
        vDebug("] = 0x");
        vDebugHex((uint32)g_pfnVectors[i], 8);
        vDebug("\r\n");
    }

    // check the installers are present
    vDebug("\r\nChecking Debug Exception Handlers Installed :\r\n");

    if(g_pfnVectors[3] == (void*)HardFault_Handler)
    {
    	vDebug("Debug HardFault_Handler Installed OK\r\n");
    }
    else
    {
    	vDebug("Debug HardFault_Handler NOT Installed\r\n");
    	vDebug(" HardFault_Handler @ 0x");
		vDebugHex((uint32)HardFault_Handler, 8);
		vDebug("\r\n");
    }

    if(g_pfnVectors[4] == (void*)MemManage_Handler)
	{
		vDebug("Debug MemManage_Handler Installed OK\r\n");
	}
    else
	{
		vDebug("Debug MemManage_Handler NOT Installed\r\n");
		vDebug(" MemManage_Handler @ 0x");
		vDebugHex((uint32)MemManage_Handler, 8);
		vDebug("\r\n");
	}

    if(g_pfnVectors[5] == (void*)BusFault_Handler)
	{
		vDebug("Debug BusFault_Handler Installed OK\r\n");
	}
    else
	{
		vDebug("Debug BusFault_Handler NOT Installed\r\n");
		vDebug(" BusFault_Handler @ 0x");
		vDebugHex((uint32)BusFault_Handler, 8);
		vDebug("\r\n");
	}

    if(g_pfnVectors[6] == (void*)UsageFault_Handler)
	{
		vDebug("Debug UsageFault_Handler Installed OK\r\n");
	}
    else
	{
		vDebug("Debug UsageFault_Handler NOT Installed\r\n");
		vDebug(" UsageFault_Handler @ 0x");
		vDebugHex((uint32)UsageFault_Handler, 8);
		vDebug("\r\n");
	}

    vDebug("\r\n");

    vDebug("\r\nNVIC Configuration :\r\n");
    // read control
    u32RegisterValue = u32REG_Read(REG_APP_INT_RESET_CTRL);
    vDebug("REG_APP_INT_RESET_CTRL = 0x");
    vDebugHex(u32RegisterValue, 8);
    vDebug("\r\n");

    // read vector table location
    u32RegisterValue = u32REG_Read(REG_INT_VECTOR_TABLE_OFFSET);
    vDebug("REG_INT_VECTOR_TABLE_OFFSET = 0x");
    vDebugHex(u32RegisterValue, 8);
    vDebug("\r\n");

    // read enable exception features - DIV0 and align errors
    u32RegisterValue = u32REG_Read(REG_CONFIGURATION_CONTROL);
    vDebug("REG_CONFIGURATION_CONTROL = 0x");
    vDebugHex(u32RegisterValue, 8);
    vDebug("\r\n\r\n");

    vDebug("\r\nNVIC Exception Priorities :\r\n");
    // read the exception priorities here
    // MemManage
    u32RegisterValue = u8REG_Read(REG_SYSTEM_HANDLER_PRIORITY_4);
    vDebug("REG_SYSTEM_HANDLER_PRIORITY_4 = 0x");
    vDebugHex(u32RegisterValue, 8);
    vDebug("\r\n");
    // BusFault
    u32RegisterValue = u8REG_Read(REG_SYSTEM_HANDLER_PRIORITY_5);
    vDebug("REG_SYSTEM_HANDLER_PRIORITY_5 = 0x");
    vDebugHex(u32RegisterValue, 8);
    vDebug("\r\n");
    // UsageFault
    u32RegisterValue = u8REG_Read(REG_SYSTEM_HANDLER_PRIORITY_6);
    vDebug("REG_SYSTEM_HANDLER_PRIORITY_6 = 0x");
    vDebugHex(u32RegisterValue, 8);
    vDebug("\r\n");
#if 0
    // SVC
    u32RegisterValue = u8REG_Read(REG_SYSTEM_HANDLER_PRIORITY_11);
    vDebug("REG_SYSTEM_HANDLER_PRIORITY_11 = 0x");
    vDebugHex(u32RegisterValue, 8);
    vDebug("\r\n");
    // debug monitor
    u32RegisterValue = u8REG_Read(REG_SYSTEM_HANDLER_PRIORITY_12);
    vDebug("REG_SYSTEM_HANDLER_PRIORITY_12 = 0x");
    vDebugHex(u32RegisterValue, 8);
    vDebug("\r\n");
    // pend SV
    u32RegisterValue = u8REG_Read(REG_SYSTEM_HANDLER_PRIORITY_14);
    vDebug("REG_SYSTEM_HANDLER_PRIORITY_14 = 0x");
    vDebugHex(u32RegisterValue, 8);
    vDebug("\r\n");
    // SYSTICK
    u32RegisterValue = u8REG_Read(REG_SYSTEM_HANDLER_PRIORITY_15);
    vDebug("REG_SYSTEM_HANDLER_PRIORITY_15 = 0x");
    vDebugHex(u32RegisterValue, 8);
    vDebug("\r\n\r\n");
#endif
    // handler status register
    u32RegisterValue = u32REG_Read(REG_SYSTEM_HANDLER_CNTRL_STATE);
    vDebug("REG_SYSTEM_HANDLER_CNTRL_STATE = 0x");
    vDebugHex(u32RegisterValue, 8);
    vDebug("\r\n");

    vDebug("\r\nNVIC Exception Status :\r\n");
    vPrintSystemHandlerAndControlStateRegisterBitFields(u32RegisterValue);

    vDebug("*******************************************************************\r\n");
	vDebug("************   END OF NVIC CONFIGURATION INFORMATION     **********\r\n");
	vDebug("*******************************************************************\r\n");

	vDebug("\r\n");
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
