/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef _APP_H_
#define _APP_H_


#include "fsl_common.h"
#include "fsl_debug_console.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* default to uart 0 */
#ifndef UART
#define UART                              (USART0)
#endif

/* default BAUD rate 115200 */
#ifndef UART_BAUD_RATE
#define UART_BAUD_RATE                     (115200U)
#endif

/* USART interrupt handler */
#define APP_isrUart                        FLEXCOMM0_IRQHandler

/* there is one IO port on Jn518x, 0  */
#define APP_BOARD_GPIO_PORT                (0)

#define DEMO_GINT0_PORT                    (kGINT_Port0)

#define UART0_IRQ                          (USART0_IRQn)
#define APP_BOARD_TEST_GPIO_PORT           (0)

#define DMA                                 DMA0
#define DMA_BUFFER_LENGTH                   (256)
#define USART_TX_DMA_CHANNEL	            (1)
#define USART_RX_DMA_CHANNEL	            (0)
#define UART_CLK_FREQ                       (CLOCK_GetFreq(kCLOCK_Fro32M))


/* OM15076 Carrier Board */
#define APP_BOARD_SW0_PIN       (1)   /* USERINTERFACE */
#define APP_BOARD_SWISP_PIN     (5)   /* ISP */

/* base board leds Pin High == OFF */
#define APP_BASE_BOARD_LED1_PIN (0)  /* base bord led 1 */
#define APP_BASE_BOARD_LED2_PIN (3)  /* base bord led 2 */

#define ON                     (1)
#define OFF                    (0)
#define LED1                    0
#define LED2                    1
#define LED3                    2

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void BOARD_InitHardware(void);
void BOARD_SetClockForPowerMode(void);

#endif /* _APP_H_ */
