/*****************************************************************************
 *
 * MODULE:             JN-AN-1243
 *
 * COMPONENT:          app.h
 *
 * DESCRIPTION:        Hardware defines
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5169,
 * JN5179, JN5189].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2017-2019. All rights reserved
 *
 ***************************************************************************/
#ifndef _APP_H_
#define _APP_H_
#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*${macro:start}*/
/* default to uart 0 */
#ifndef UART
#define UART            (USART0)
#endif

#ifndef UARTLINKY
#define UARTLINKY            (USART1)
#endif


/* default BAUD rate 9600 */
#ifndef UART_BAUD_RATE
#define UART_BAUD_RATE        115200U
#endif

#define APP_isrUart         FLEXCOMM0_IRQHandler
#define UART0_IRQ           USART0_IRQn

#define APP_isrUartLinky    FLEXCOMM1_IRQHandler
#define UARTLINKY_IRQ       USART1_IRQn


/* there is one IO port on Jn518x, 0  */
#define APP_BOARD_GPIO_PORT        0

#define DEMO_GINT0_PORT kGINT_Port0

#if (defined OM15082)

/* OM15076 Carrier Board */
#define APP_BOARD_SW0_PIN   (1)   /* USERINTERFACE */
#define APP_BOARD_SWISP_PIN (5)   /* ISP */
/* OM15082 Expansion Board */
#define APP_BOARD_SW1_PIN   (19)  /* SW1 */
#define APP_BOARD_SW2_PIN   (15)  /* SW2 */
#define APP_BOARD_SW3_PIN   (7)   /* SW3 */
#define APP_BOARD_SW4_PIN   (4)   /* SW4 */

/* expansion board leds Pin High == ON */
#define APP_BOARD_LED1_PIN  (16) /* expansion bord led 1 */
#define APP_BOARD_LED2_PIN  (6)  /* expansion bord led 2 */
#define APP_BOARD_LED3_PIN  (3)  /* expansion bord led 3 */

/* base board leds Pin High == OFF */
#define APP_BASE_BOARD_LED1_PIN (0)  /* base bord led 1 */
#define APP_BASE_BOARD_LED2_PIN (3)  /* base bord led 2 */

#define ON (1)
#define OFF (0)
#define LED1  0
#define LED2  1
#define LED3  2
#define LED4  3
#define LED5  4


#endif

#if ((defined OM15081) || (defined OM5578))
#define APP_BOARD_SW0_PIN   (0)   /* USERINTERFACE */
#define APP_BOARD_SW1_PIN   (2)   /* USERINTERFACE */
#define APP_BOARD_SWISP_PIN (5)   /* ISP */

#define ON (1)
#define OFF (0)
#define LED1  0
#define LED2  1
#define LED3  2
//#define LED4  3
//#define LED5  4

/* the white led on om15081 driven via pwm */

/* base board leds Pin High == OFF */
#define APP_BASE_BOARD_LED1_PIN (10)  /* base bord led 1 */
#define APP_BASE_BOARD_LED2_PIN (3)  /* base bord led 2 */

#endif

#if (defined DONGLE)
// TODO IO for dongle here
#define ON (1)
#define OFF (0)
#define LED1  0
#define LED2  1


/* base board leds Pin High == OFF */
#define APP_BASE_BOARD_LED1_PIN (0)  /* base bord led 1 */
#define APP_BASE_BOARD_LED2_PIN (3)  /* base bord led 2 */

#endif

/*${macro:end}*/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*${prototype:start}*/
void BOARD_InitHardware(void);
void BOARD_SetClockForPowerMode(void);
/*${prototype:end}*/


#endif /* _APP_H_ */
