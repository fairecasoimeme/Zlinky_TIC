/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_pwm.h"
#include "fsl_inputmux.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_USART USART0
#define DEMO_USART_CLK_SRC kCLOCK_Fro32M
#define EXAMPLE_PWM_CLK_SRC kCLOCK_Fro32M
#define EXAMPLE_PWM PWM
#define EXAMPLE_PWM_IRQ_ID PWM0_IRQn
#define EXAMPLE_PWM0_IRQ_HANDLER_FUNC PWM0_IRQHandler
#define EXAMPLE_PWM1_IRQ_HANDLER_FUNC PWM1_IRQHandler
#define EXAMPLE_PWM2_IRQ_HANDLER_FUNC PWM2_IRQHandler
#define EXAMPLE_PWM3_IRQ_HANDLER_FUNC PWM3_IRQHandler
#define EXAMPLE_PWM4_IRQ_HANDLER_FUNC PWM4_IRQHandler
#define EXAMPLE_PWM5_IRQ_HANDLER_FUNC PWM5_IRQHandler
#define EXAMPLE_PWM6_IRQ_HANDLER_FUNC PWM6_IRQHandler
#define EXAMPLE_PWM7_IRQ_HANDLER_FUNC PWM7_IRQHandler
#define EXAMPLE_PWM8_IRQ_HANDLER_FUNC PWM8_IRQHandler
#define EXAMPLE_PWM9_IRQ_HANDLER_FUNC PWM9_IRQHandler
#define EXAMPLE_PWM10_IRQ_HANDLER_FUNC PWM10_IRQHandler


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static volatile pwm_setup_t pwmChan;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void)
{
    pwm_config_t pwmConfig;
    uint32_t pwmSourceClockFreq;
    uint32_t pwmChannelClockFreq;

    /* Board pin, clock, debug console initialization */
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* attach clock for USART(debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* reset FLEXCOMM for USART */
    RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);

    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

    PRINTF("PWM driver example. Connect scope to Port 0 Pin 3 to see output\r\n");
    PRINTF("or look at LED DS3/D3 on carrier/sensor boards.\r\n");

    /* Get default configuration */
    PWM_GetDefaultConfig(&pwmConfig);

    /* PWM Init with default clock selected */
    PWM_Init(EXAMPLE_PWM, &pwmConfig);

    /* Get the default source clock frequency */
    pwmSourceClockFreq = CLOCK_GetFreq(kCLOCK_Pwm);

    /* Set up PWM channel to generate PWM pulse of 10ms with a starting 100% duty cycle */
    pwmChan.pol_ctrl      = kPWM_SetHighOnMatchLowOnPeriod;
    pwmChan.dis_out_level = kPWM_SetLow;
    pwmChan.prescaler_val = 0;
    pwmChannelClockFreq   = pwmSourceClockFreq / (1 + pwmChan.prescaler_val);
    pwmChan.period_val    = USEC_TO_COUNT(10000, pwmChannelClockFreq);
    /* Compare value starts same as period value to give a 100% starting duty cycle */
    pwmChan.comp_val = pwmChan.period_val;
    PWM_SetupPwm(EXAMPLE_PWM, kPWM_Pwm3, (pwm_setup_t *)&pwmChan);

    /* Clear interrupt status for PWM channel */
    PWM_ClearStatusFlags(EXAMPLE_PWM, kPWM_Pwm3);

    /* Enable IRQ in NVIC for PWM channel 3 */
    EnableIRQ(PWM3_IRQn);

    /* Enable PWM channel interrupt */
    PWM_EnableInterrupts(EXAMPLE_PWM, kPWM_Pwm3);

    /* Start the PWM generation channel */
    PWM_StartTimer(EXAMPLE_PWM, kPWM_Pwm3);

    while (1)
    {
        __WFI();
    }
}

/*!
 * @brief PWM3_IRQHandler function
 * ISR Handler function for PWM3 channel. This is called at the end of each period.
 * In this example at the end of each period the compare value is modified
 * to change the duty cycle.
 */
void PWM3_IRQHandler(void)
{
    static uint32_t count = 2000;

    count--;
    if (count == 0)
    {
        count = 2000;
    }

    /* Change the compare value in the channel setup structure to change the duty cycle, ramping up over 2000 steps */
    pwmChan.comp_val = (count * pwmChan.period_val) / 2000;

    /* Re-apply the channel setup to update the compare value */
    PWM_SetupPwm(EXAMPLE_PWM, kPWM_Pwm3, (pwm_setup_t *)&pwmChan);

    /* Handle PWM channel interrupt, clear interrupt status */
    PWM_ClearStatusFlags(EXAMPLE_PWM, kPWM_Pwm3);
}
