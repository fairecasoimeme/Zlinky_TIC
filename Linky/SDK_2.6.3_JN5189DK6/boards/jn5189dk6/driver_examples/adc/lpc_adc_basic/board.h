/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#include "fsl_device_registers.h"
#include "fsl_common.h"
#include "clock_config.h"
#include "fsl_clock.h"
#include "fsl_power.h"
#include "fsl_gpio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME "DK6"

/* The UART to use for debug messages. */
#define BOARD_DEBUG_UART_TYPE kSerialPort_Uart
#define BOARD_DEBUG_UART_BAUDRATE 115200U
#define BOARD_DEBUG_UART_BASEADDR (uint32_t) USART0
#define BOARD_DEBUG_UART_INSTANCE 0U
#define BOARD_DEBUG_UART_CLK_FREQ CLOCK_GetFreq(kCLOCK_Fro32M)
#define BOARD_UART_IRQ LPUART0_IRQn
#define BOARD_UART_IRQ_HANDLER LPUART0_IRQHandler
#define BOARD_DEBUG_UART_CLK_ATTACH kOSC32M_to_USART_CLK

/* There are 2 red LEDs on DK6 board: PIO0 and PIO3 */
#define BOARD_LED_RED1_GPIO GPIO
#define BOARD_LED_RED1_GPIO_PORT 0U
#define BOARD_LED_RED1_GPIO_PIN 3U
#define BOARD_LED_RED2_GPIO GPIO
#define BOARD_LED_RED2_GPIO_PORT 0U
#define BOARD_LED_RED2_GPIO_PIN 0U

/* There are 2 red LEDs on USB Dongle: PIO4 and PIO10 */
#define BOARD_LED_USB_DONGLE1_GPIO GPIO
#define BOARD_LED_USB_DONGLE1_GPIO_PORT 0U
#define BOARD_LED_USB_DONGLE1_GPIO_PIN 4U
#define BOARD_LED_USB_DONGLE2_GPIO GPIO
#define BOARD_LED_USB_DONGLE2_GPIO_PORT 0U
#define BOARD_LED_USB_DONGLE2_GPIO_PIN 10U

/* SPIFI clock rate */
#define BOARD_SPIFI_CLK_RATE 16000000U

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

/* Board led color mapping */
#define LOGIC_LED_ON 0U
#define LOGIC_LED_OFF 1U

#define LED_RED1_INIT(output)                                                            \
    GPIO_PinInit(BOARD_LED_RED1_GPIO, BOARD_LED_RED1_GPIO_PORT, BOARD_LED_RED1_GPIO_PIN, \
                 &(gpio_pin_config_t){kGPIO_DigitalOutput, (output)}) /*!< Enable target LED_RED1 */
#define LED_RED1_ON()                                             \
    GPIO_PortClear(BOARD_LED_RED1_GPIO, BOARD_LED_RED1_GPIO_PORT, \
                   1U << BOARD_LED_RED1_GPIO_PIN) /*!< Turn on target LED_RED1 */
#define LED_RED1_OFF()                                          \
    GPIO_PortSet(BOARD_LED_RED1_GPIO, BOARD_LED_RED1_GPIO_PORT, \
                 1U << BOARD_LED_RED1_GPIO_PIN) /*!< Turn off target LED_RED1 */
#define LED_RED1_TOGGLE()                                          \
    GPIO_PortToggle(BOARD_LED_RED1_GPIO, BOARD_LED_RED1_GPIO_PORT, \
                    1U << BOARD_LED_RED1_GPIO_PIN) /*!< Toggle on target LED_RED1 */

#define LED_RED2_INIT(output)                                                            \
    GPIO_PinInit(BOARD_LED_RED2_GPIO, BOARD_LED_RED2_GPIO_PORT, BOARD_LED_RED2_GPIO_PIN, \
                 &(gpio_pin_config_t){kGPIO_DigitalOutput, (output)}) /*!< Enable target LED_RED2 */
#define LED_RED2_ON()                                             \
    GPIO_PortClear(BOARD_LED_RED2_GPIO, BOARD_LED_RED2_GPIO_PORT, \
                   1U << BOARD_LED_RED2_GPIO_PIN) /*!< Turn on target LED_RED2 */
#define LED_RED2_OFF()                                          \
    GPIO_PortSet(BOARD_LED_RED2_GPIO, BOARD_LED_RED2_GPIO_PORT, \
                 1U << BOARD_LED_RED2_GPIO_PIN) /*!< Turn off target LED_RED2 */
#define LED_RED2_TOGGLE()                                          \
    GPIO_PortToggle(BOARD_LED_RED2_GPIO, BOARD_LED_RED2_GPIO_PORT, \
                    1U << BOARD_LED_RED2_GPIO_PIN) /*!< Toggle on target LED_RED2 */

#define BOARD_SW1_GPIO GPIO
#define BOARD_SW1_GPIO_PORT 0U
#define BOARD_SW1_GPIO_PIN 1U
#define BOARD_SW1_NAME "SW1"
#define BOARD_SW2_GPIO GPIO
#define BOARD_SW2_GPIO_PORT 0U
#define BOARD_SW2_GPIO_PIN 5U
#define BOARD_SW2_NAME "SW2"
#define BOARD_SW3_IRQ PIN_INT0_IRQn
#define BOARD_SW3_IRQ_HANDLER PIN_INT0_IRQHandler
#define BOARD_SW4_NAME "SW4"
#define BOARD_SW4_IRQ PIN_INT0_IRQn

/* Capacitance values for 32MHz and 32kHz crystals; board-specific. Value is
   pF x 100. For example, 6pF becomes 600, 1.2pF becomes 120 */
#define CLOCK_32MfXtalIecLoadpF_x100 (600)   /* 6.0pF */
#define CLOCK_32MfXtalPPcbParCappF_x100 (20) /* 0.2pF */
#define CLOCK_32MfXtalNPcbParCappF_x100 (40) /* 0.4pF */
#define CLOCK_32kfXtalIecLoadpF_x100 (600)   /* 6.0pF */
#define CLOCK_32kfXtalPPcbParCappF_x100 (40) /* 0.4pF */
#define CLOCK_32kfXtalNPcbParCappF_x100 (40) /* 0.4pF */

/* Capacitance variation for 32MHz crystal across temperature
   ----------------------------------------------------------

   TCXO_32M_MODE_EN should be 1 to indicate that temperature-compensated 32MHz
   XO is supported and required. If so, HW_32M_LOAD_VS_TEMP_MIN,
   _MAX, _STEP must be defined here and CLOCK_ai32MXtalIecLoadPfVsTemp_x1000
   must be defined in board.c.

   Values are used as follows:
   CLOCK_ai32MXtalIecLoadPfVsTemp_x1000 is an array of crystal load capacitance
   values across temp, with each value being at a specific temp. First value is
   for temp given by HW_32M_LOAD_VS_TEMP_MIN, next value is for
   temp given by HW_32M_LOAD_VS_TEMP_MIN + _STEP, next value is
   for temp given by HW_32M_LOAD_VS_TEMP_MIN + _ STEP x 2, etc.
   Final value is for temp given by HW_32M_LOAD_VS_TEMP_MAX. It is
   important for HW_32M_LOAD_VS_TEMP_x defines and the table to be
   matched to one another */
#define TCXO_32M_MODE_EN (1)

/* Values below are for NDK NX2016SA 32MHz EXS00A-CS11213-6(IEC) */

/* Temperature related to element 0 of CLOCK_ai32MXtalIecLoadPfVsTemp_x1000 */
#define HW_32M_LOAD_VS_TEMP_MIN (-40)

/* Temperature related to final element of CLOCK_ai32MXtalIecLoadPfVsTemp_x1000 */
#define HW_32M_LOAD_VS_TEMP_MAX (130)

/* Temperature step between elements of CLOCK_ai32MXtalIecLoadPfVsTemp_x1000 */
#define HW_32M_LOAD_VS_TEMP_STEP (5)

#define HW_32M_LOAD_VS_TEMP_SIZE ((HW_32M_LOAD_VS_TEMP_MAX - HW_32M_LOAD_VS_TEMP_MIN) / HW_32M_LOAD_VS_TEMP_STEP + 1U)

/* Table of load capacitance versus temperature for 32MHz crystal. Values are
   for temperatures from -40 to +130 in steps of 5 */
extern int32_t CLOCK_ai32MXtalIecLoadPfVsTemp_x1000[HW_32M_LOAD_VS_TEMP_SIZE];

/* Capacitance variation for 32kHz crystal across temperature
   ----------------------------------------------------------

   TCXO_32k_MODE_EN should be 1 to indicate that temperature-compensated 32kHz
   XO is supported and required. If so, HW_32k_LOAD_VS_TEMP_MIN,
   _MAX, _STEP must be defined here and CLOCK_ai32kXtalIecLoadPfVsTemp_x1000
   must be defined in board.c.

   Values are used as follows:
   CLOCK_ai32kXtalIecLoadPfVsTemp_x1000 is an array of crystal load capacitance
   values across temp, with each value being at a specific temp. First value is
   for temp given by HW_32k_LOAD_VS_TEMP_MIN, next value is for
   temp given by HW_32k_LOAD_VS_TEMP_MIN + _STEP, next value is
   for temp given by HW_32k_LOAD_VS_TEMP_MIN + _ STEP x 2, etc.
   Final value is for temp given by HW_32k_LOAD_VS_TEMP_MAX. It is
   important for HW_32k_LOAD_VS_TEMP_x defines and the table to be
   matched to one another */
#define TCXO_32k_MODE_EN               \
    (0) /* Disabled because table is   \
           *not* correct: values below \
           are just for example */

/* Temperature related to element 0 of CLOCK_ai32kXtalIecLoadPfVsTemp_x1000 */
#define HW_32k_LOAD_VS_TEMP_MIN (-40)

/* Temperature related to final element of CLOCK_ai32kXtalIecLoadPfVsTemp_x1000 */
#define HW_32k_LOAD_VS_TEMP_MAX (130)

/* Temperature step between elements of CLOCK_ai32kXtalIecLoadPfVsTemp_x1000 */
#define HW_32k_LOAD_VS_TEMP_STEP (5)

#define HW_32k_LOAD_VS_TEMP_SIZE ((HW_32k_LOAD_VS_TEMP_MAX - HW_32k_LOAD_VS_TEMP_MIN) / HW_32k_LOAD_VS_TEMP_STEP + 1U)

/* Table of load capacitance versus temperature for 32kHz crystal. Values are
   for temperatures from -20 to +100 in steps of 20 */
extern int32_t CLOCK_ai32kXtalIecLoadPfVsTemp_x1000[HW_32k_LOAD_VS_TEMP_SIZE];

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/
status_t BOARD_InitDebugConsole(void);

/* Dummy functions, added to enable LowPower module to link */
void BOARD_DbgDiagEnable(void);
void BOARD_DbgLpIoSet(int pinid, int val);

extern uint32_t BOARD_GetSpiClock(uint32_t instance);
extern void BOARD_InitSPI(void);
extern void BOARD_InitSPIFI(void);
extern void BOARD_SetSpiFi_LowPowerEnter(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
