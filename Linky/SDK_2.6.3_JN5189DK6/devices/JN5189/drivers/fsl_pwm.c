/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h>
#include "fsl_common.h"
#include "fsl_pwm.h"
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.jn_pwm"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define PWM_CHANS_EN_MASK                                                                                    \
    (PWM_CTRL0_PWM_EN_0_MASK | PWM_CTRL0_PWM_EN_1_MASK | PWM_CTRL0_PWM_EN_2_MASK | PWM_CTRL0_PWM_EN_3_MASK | \
     PWM_CTRL0_PWM_EN_4_MASK | PWM_CTRL0_PWM_EN_5_MASK | PWM_CTRL0_PWM_EN_6_MASK | PWM_CTRL0_PWM_EN_7_MASK | \
     PWM_CTRL0_PWM_EN_8_MASK | PWM_CTRL0_PWM_EN_9_MASK | PWM_CTRL0_PWM_EN_10_MASK)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void PWM_GetDefaultConfig(pwm_config_t *userConfig)
{
    /* Check if valid parameters */
    assert(userConfig);

    /* Initialise configuration structure */
    userConfig->clk_sel = kCLOCK_PWMOsc32Mclk;
    return;
}

status_t PWM_Init(PWM_Type *base, const pwm_config_t *userConfig)
{
    clock_attach_id_t clock_sel;

    /* Check if valid buffers are passed */
    if ((base == NULL) || (userConfig == NULL))
    {
        return kStatus_InvalidArgument;
    }

    /* Check if valid parameters are passed */
    if (userConfig->clk_sel > kCLOCK_PWMTestClk)
    {
        return kStatus_InvalidArgument;
    }

    /* Enable PWM peripheral clock for register access */
    /* Make sure that the APB bus clock (kCLOCK_Iocon) is enabled before this */
    CLOCK_EnableClock(kCLOCK_Pwm);

    /* Select clock source for PWM */
    /* Make sure that the selected clock source is enabled before this */
    if (userConfig->clk_sel == kCLOCK_PWMOsc32Mclk)
    {
        clock_sel = kOSC32M_to_PWM_CLK;
    }
    else if (userConfig->clk_sel == kCLOCK_PWMFro48Mclk)
    {
        clock_sel = kFRO48M_to_PWM_CLK;
    }
    else
    {
        clock_sel = kNONE_to_PWM_CLK;
    }
    CLOCK_AttachClk(clock_sel);

    return kStatus_Success;
}

void PWM_Deinit(PWM_Type *base)
{
    /* Stop PWM channels */
    base->CTRL0 &= ~(PWM_CHANS_EN_MASK);

    /* Disable PWM clock */
    CLOCK_DisableClock(kCLOCK_Pwm);

    return;
}

status_t PWM_SetupPwm(PWM_Type *base, pwm_channels_t pwm_chan, pwm_setup_t *pwmSetup)
{
    uint32_t prescale_reg_off;
    uint32_t prescale_shift;
    volatile uint32_t *prescale_reg_ptr;
    volatile uint32_t *pcp_reg_ptr;

    /* Check if valid buffers are passed */
    if ((base == NULL) || (pwmSetup == NULL))
    {
        return kStatus_InvalidArgument;
    }

    /* Check if valid parameters are passed */
    if ((pwm_chan > kPWM_PwmAll) || (pwmSetup->pol_ctrl > kPWM_SetLowOnMatchHighOnPeriod) ||
        (pwmSetup->dis_out_level > kPWM_SetHigh) || (pwmSetup->prescaler_val > PWM_PSCL01_PSCL_0_MASK) ||
        (pwmSetup->comp_val > pwmSetup->period_val))
    {
        return kStatus_InvalidArgument;
    }

    /* Set Polarity & Disable output values for channel */
    base->CTRL1 &= ~((PWM_CTRL1_POL_0_MASK << pwm_chan) | (PWM_CTRL1_DIS_LEVEL_0_MASK << pwm_chan));
    base->CTRL1 |=
        ((pwmSetup->pol_ctrl << pwm_chan) | (pwmSetup->dis_out_level << (pwm_chan + PWM_CTRL1_DIS_LEVEL_0_SHIFT)));

    /* Set Prescaler value for channel */
    prescale_reg_off = pwm_chan >> 1;
    prescale_shift   = pwm_chan % 2;
    prescale_reg_ptr = ((volatile uint32_t *)&base->PSCL01) + prescale_reg_off;
    *prescale_reg_ptr &= ~(PWM_PSCL01_PSCL_0_MASK << (PWM_PSCL01_PSCL_1_SHIFT * prescale_shift));
    *prescale_reg_ptr |= (pwmSetup->prescaler_val << (PWM_PSCL01_PSCL_1_SHIFT * prescale_shift));

    /* Set period & compare values for channel */
    pcp_reg_ptr  = ((volatile uint32_t *)&base->PCP0) + pwm_chan;
    *pcp_reg_ptr = (pwmSetup->comp_val << PWM_PCP0_COMPARE_SHIFT) | pwmSetup->period_val;

    return kStatus_Success;
}

uint32_t PWM_GetStatusFlags(PWM_Type *base, pwm_channels_t pwm_chan)
{
    uint32_t intstat_reg_off;
    uint32_t intstat_shift;
    uint32_t intstat_reg_val;
    uint32_t intstat_reg_byte;

    /* Read Interrupt register value for channel */
    intstat_reg_off  = pwm_chan >> 2;
    intstat_shift    = pwm_chan % 4;
    intstat_reg_val  = *(((volatile uint32_t *)&base->PST0) + intstat_reg_off);
    intstat_reg_byte = (intstat_reg_val >> (PWM_PST0_INT_FLG_1_SHIFT * intstat_shift));
    return (intstat_reg_byte & 1);
}

void PWM_ClearStatusFlags(PWM_Type *base, pwm_channels_t pwm_chan)
{
    uint32_t intstat_reg_off;
    volatile uint32_t *intstat_reg_ptr;
    uint32_t intstat_shift;

    /* Read Interrupt register value for channel */
    intstat_reg_off = pwm_chan >> 2;
    intstat_shift   = pwm_chan % 4;
    intstat_reg_ptr = (volatile uint32_t *)&base->PST0 + intstat_reg_off;
    *intstat_reg_ptr |= (1 << (PWM_PST0_INT_FLG_1_SHIFT * intstat_shift));
    return;
}

uint16_t PWM_ReadPeriodValue(PWM_Type *base, pwm_channels_t pwm_chan)
{
    volatile uint32_t *pcp_reg_ptr;
    uint16_t period_val;

    /* Read PCP register value for channel */
    pcp_reg_ptr = ((volatile uint32_t *)&base->PCP0) + pwm_chan;
    period_val  = (uint16_t)((*pcp_reg_ptr) & PWM_PCP0_PERIOD_MASK);
    return period_val;
}

uint16_t PWM_ReadCompareValue(PWM_Type *base, pwm_channels_t pwm_chan)
{
    volatile uint32_t *pcp_reg_ptr;
    uint16_t cmp_val;

    /* Read PCP register value for channel */
    pcp_reg_ptr = ((volatile uint32_t *)&base->PCP0) + pwm_chan;
    cmp_val     = (uint16_t)(((*pcp_reg_ptr) >> PWM_PCP0_COMPARE_SHIFT) & PWM_PCP0_PERIOD_MASK);
    return cmp_val;
}
