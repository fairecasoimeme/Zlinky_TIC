/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include "EmbeddedTypes.h"
#include "board.h"
#include "app.h"
#include "fsl_usart.h"
#include "usart_dma_rxbuffer.h"
#include "app_uart.h"

#ifdef FSL_RTOS_FREE_RTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#endif
#include "portmacro.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifndef TRACE_UART
#define TRACE_UART	FALSE
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME: UART_bBufferReceive
 *
 * DESCRIPTION:
 *
 * PARAMETERS: Name        RW  Usage
 *
 * RETURNS:
 *
 ****************************************************************************/
bool_t UART_bBufferReceive ( uint8_t* pu8Data )
{
	  bool_t bReturn = FALSE;

      if (USART_DMA_ReadBytes(pu8Data,1) == 0)
      {
    	  bReturn = FALSE;
      }
      else
      {
    	  bReturn = TRUE;
      }
	  return bReturn;
}

/****************************************************************************
 *
 * NAME: vUART_Init
 *
 * DESCRIPTION:
 *
 * PARAMETERS:      Name            RW  Usage
 * None.
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * None.
 ****************************************************************************/
void UART_vInit(void)
{
	USART_DMA_Init();
}

/****************************************************************************
 *
 * NAME: vUART_SetBuadRate
 *
 * DESCRIPTION:
 *
 * PARAMETERS: Name        RW  Usage
 *
 * RETURNS:
 *
 ****************************************************************************/

void UART_vSetBaudRate(uint32_t u32BaudRate)
{
	int result;

    /* setup baudrate */
    result = USART_SetBaudRate(UART, u32BaudRate, CLOCK_GetFreq(kCLOCK_Fro32M));
    if (kStatus_Success != result)
    {
        //DBG_vPrintf(TRACE_UART,"\r\nFailed to set UART speed ");
    }
}

/****************************************************************************
 *
 * NAME: UART_vRtsStopFlow
 *
 * DESCRIPTION:
 * Set UART RS-232 RTS line high to stop any further data coming in
 *
 ****************************************************************************/

void UART_vRtsStopFlow(void)
{
    //vAHI_UartSetControl(UART, FALSE, FALSE, E_AHI_UART_WORD_LEN_8, TRUE, E_AHI_UART_RTS_HIGH);
}

/****************************************************************************
 *
 * NAME: UART_vRtsStartFlow
 *
 * DESCRIPTION:
 * Set UART RS-232 RTS line low to allow further data
 *
 ****************************************************************************/

void UART_vRtsStartFlow(void)
{
    //vAHI_UartSetControl(UART, FALSE, FALSE, E_AHI_UART_WORD_LEN_8, TRUE, E_AHI_UART_RTS_LOW);
}
/* [I SP001222_P1 283] end */

/****************************************************************************
 *
 * NAME: vUART_TxChar
 *
 * DESCRIPTION:
 * Set UART RS-232 RTS line low to allow further data
 *
 ****************************************************************************/
void UART_vTxChar(uint8_t u8Char)
{
	USART_WriteByte(UART, u8Char);
    /* Wait to finish transfer */
    while (!(UART->FIFOSTAT & USART_FIFOSTAT_TXEMPTY_MASK))
    {
    }
}

/****************************************************************************
 *
 * NAME: vUART_TxReady
 *
 * DESCRIPTION:
 * Set UART RS-232 RTS line low to allow further data
 *
 ****************************************************************************/
bool_t UART_bTxReady()
{
    return ( (kUSART_TxFifoEmptyFlag|kUSART_TxFifoNotFullFlag) & USART_GetStatusFlags(UART) );
}

/****************************************************************************
 *
 * NAME: vUART_SetTxInterrupt
 *
 * DESCRIPTION:
 * Enable / disable the tx interrupt
 *
 ****************************************************************************/
void UART_vSetTxInterrupt(bool_t bState)
{
	USART_DisableInterrupts(UART, (kUSART_TxErrorInterruptEnable | kUSART_RxErrorInterruptEnable | kUSART_TxLevelInterruptEnable| kUSART_RxLevelInterruptEnable) );
	if (bState == FALSE)
	{
	    USART_EnableInterrupts(UART, (kUSART_RxLevelInterruptEnable | kUSART_RxErrorInterruptEnable) );
	}
	else
	{
		USART_EnableInterrupts(UART, (kUSART_RxLevelInterruptEnable | kUSART_RxErrorInterruptEnable | kUSART_TxLevelInterruptEnable|kUSART_TxErrorInterruptEnable ));
	}
}

/****************************************************************************
 *
 * NAME: UART_vSendString
 *
 * DESCRIPTION:
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void UART_vSendString(char* sMessage)
{
    uint32_t u32Counter = 0 ;
    for(u32Counter = 0; u32Counter < strlen(sMessage); u32Counter++ )
    {
    	UART_vTxChar(sMessage[u32Counter]);
    }
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
