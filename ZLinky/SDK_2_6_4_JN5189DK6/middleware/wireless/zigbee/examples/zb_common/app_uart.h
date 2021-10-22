/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef  UART_H_INCLUDED
#define  UART_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "EmbeddedTypes.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define RX_BUFFER_SIZE  256
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

void UART_vInit(void);
void UART_vTxChar(uint8_t u8TxChar);
bool_t UART_bTxReady(void);
void UART_vRtsStartFlow(void);
void UART_vRtsStopFlow(void);
void UART_vSetTxInterrupt(bool_t bState);
void UART_vSetBaudRate(uint32_t u32BaudRate);
bool_t UART_bBufferReceive ( uint8_t* u8Data );
void UART_vSendString(char* sMessage);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* UART_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/


