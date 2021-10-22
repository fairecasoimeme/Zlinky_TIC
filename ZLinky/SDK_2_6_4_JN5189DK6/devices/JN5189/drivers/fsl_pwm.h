/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __FSL_PWM_H_
#define __FSL_PWM_H_

#include <stddef.h>
#include "fsl_common.h"

/*!
 * @addtogroup pwm_driver
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief PWM driver version 2.0.0. */
#define FSL_PWM_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*! @brief PWM channel selection values */
typedef enum _pwm_channels
{
    kPWM_Pwm0 = 0x0, /*!< Channel 0 */
    kPWM_Pwm1,       /*!< Channel 1 */
    kPWM_Pwm2,       /*!< Channel 2 */
    kPWM_Pwm3,       /*!< Channel 3 */
    kPWM_Pwm4,       /*!< Channel 4 */
    kPWM_Pwm5,       /*!< Channel 5 */
    kPWM_Pwm6,       /*!< Channel 6 */
    kPWM_Pwm7,       /*!< Channel 7 */
    kPWM_Pwm8,       /*!< Channel 8 */
    kPWM_Pwm9,       /*!< Channel 9 */
    kPWM_PwmAll,     /*!< Channel 10 - All the channels will output
                       same output programmed in this channel */
} pwm_channels_t;

/*! @brief PWM channel polarity control values */
typedef enum _pwm_polarity_control
{
    kPWM_SetHighOnMatchLowOnPeriod = 0x0, /*!< Set high on compare match,
                                            set low at end of PWM period */
    kPWM_SetLowOnMatchHighOnPeriod,       /*!< Set low on compare match,
                                            set high at end of PWM period */
} pwm_polarity_control_t;

/*! @brief PWM channel disable output level values */
typedef enum _pwm_dis_output_level
{
    kPWM_SetLow = 0x0, /*!< Set to Low level */
    kPWM_SetHigh,      /*!< Set to High level */
} pwm_dis_output_level_t;

/*! @brief PWM channel interrupt enable flags */
typedef enum _pwm_interrupt_enable
{
    kPWM_InterruptDisabled = 0x0, /*!< PWM channel interrupt disabled */
    kPWM_InterruptEnabled,        /*!< PWM channel interrupt enabled */
} pwm_interrupt_enable_t;

/*! @brief PWM channel interrupt status flags */
typedef enum _pwm_interrupt_status
{
    kPWM_NoInterrupt = 0x0, /*!< PWM channel interrupt not occurred */
    kPWM_InterruptPendig,   /*!< PWM channel interrupt pending */
} pwm_interrupt_status_t;

/*! @brief PWM configuration structure */
typedef struct _pwm_config
{
    pwm_clock_source_t clk_sel; /*!< PWM clock select value */
} pwm_config_t;

/*! @brief PWM channel setup structure */
typedef struct _pwm_setup
{
    pwm_polarity_control_t pol_ctrl;      /*!< Channel polarity control */
    pwm_dis_output_level_t dis_out_level; /*!< Channel disable output level */
    uint16_t prescaler_val;               /*!< Channel Prescaler value
                                            - 10 bit value */
    uint16_t period_val;                  /*!< Channel PWM period value */
    uint16_t comp_val;                    /*!< Channel compare match value */
} pwm_setup_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus. */

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief  Fill in the PWM config struct with the default settings
 *
 * The default values are:
 * @code
 *   userConfig->clk_sel = kPWM_Osc32Mclk;
 * @endcode
 * @param userConfig Pointer to user's PWM config structure.
 */
void PWM_GetDefaultConfig(pwm_config_t *userConfig);

/*!
 * @brief Initializes the PWM module. Call this API to ungate the PWM clock
 * and configure the PWM HW.
 *
 * @note This API should be called at the beginning of the application to use
 * the PWM driver, or any operation to the PWM module could cause hard fault
 * because PWM module clock is not enabled. The configuration structure can be
 * filled by user from scratch, or be set with default values by
 * PWM_GetDefaultConfig(). After calling this API, the application can
 * configure PWM channels to generate PWM outputs.
 * Example:
 * @code
 * pwm_config_t userConfig = {
 * .clk_sel = kPWM_Fro48Mclk,
 * };
 * PWM_Init(PWM, &userConfig);
 * @endcode
 *
 * @param base PWM base address
 * @param userConfig pointer to user configuration structure
 *
 * @return kStatus_Success - Success
 * @return kStatus_InvalidArgument - Invalid input parameter
 */
status_t PWM_Init(PWM_Type *base, const pwm_config_t *userConfig);

/*!
 * @brief Gate the PWM module clock
 *
 * @param base PWM base address
 */
void PWM_Deinit(PWM_Type *base);

/*! @}*/

/*!
 * @name PWM module output
 * @{
 */

/*!
 * @brief Sets up the PWM channel.
 *
 * The function initializes the PWM channel according to the parameters
 * passed in by the user. The function sets up the PWM compare match register
 * & period registers.
 *
 * @param base       PWM base address
 * @param pwm_chan   PWM channel select value
 * @param pwmSetup   Pointer to PWM user setup structure
 *
 * @return kStatus_Success - Success
 * @return kStatus_InvalidArgument - Invalid input parameter
 */
status_t PWM_SetupPwm(PWM_Type *base, pwm_channels_t pwm_chan, pwm_setup_t *pwmSetup);

/*! @}*/

/*!
 * @name PWM Interrupts Interface
 * @{
 */

/*!
 * @brief Enable PWM channel interrupt.
 *
 * This function enables the interrupt for the specified PWM channel.
 *
 * @param base PWM base address
 * @param pwm_chan PWM channel select value
 */
static inline void PWM_EnableInterrupts(PWM_Type *base, pwm_channels_t pwm_chan)
{
    base->CTRL0 |= (PWM_CTRL0_INT_EN_0_MASK << pwm_chan);
}

/*!
 * @brief Disable PWM channel interrupt.
 *
 * This function disables the interrupt for the specified PWM channel.
 *
 * @param base PWM base address
 * @param pwm_chan PWM channel select value
 */
static inline void PWM_DisableInterrupts(PWM_Type *base, pwm_channels_t pwm_chan)
{
    base->CTRL0 &= ~(PWM_CTRL0_INT_EN_0_MASK << pwm_chan);
}

/*!
 * @brief Gets the enabled PWM interrupts
 *
 * @param base     PWM base address
 * @param pwm_chan PWM channel select value
 *
 * @return PWM interrupt enabled status. This is the one of the values
 *         specified in enumeration ::pwm_interrupt_enable_t
 */
static inline uint32_t PWM_GetEnabledInterrupts(PWM_Type *base, pwm_channels_t pwm_chan)
{
    return ((base->CTRL0 >> (PWM_CTRL0_INT_EN_0_SHIFT + pwm_chan)) & 1);
}

/*! @}*/

/*!
 * @name Status Interface
 * @{
 */

/*!
 * @brief Gets the PWM status flags
 *
 * @param base      PWM base address
 * @param pwm_chan PWM channel select value
 *
 * @return The status flags. This is the one of the value of members of the
 *         enumeration ::pwm_interrupt_status_t
 */
uint32_t PWM_GetStatusFlags(PWM_Type *base, pwm_channels_t pwm_chan);

/*!
 * @brief Clears the PWM status flags
 *
 * @param base      PWM base address
 * @param pwm_chan  PWM channel select value
 */
void PWM_ClearStatusFlags(PWM_Type *base, pwm_channels_t pwm_chan);

/*! @}*/

/*!
 * @name Timer Start and Stop
 * @{
 */

/*!
 * @brief Start PWM channel
 *
 * The API will start PWM channel output on the pin. Before calling
 * this API, make sure that the PWM channel is set up using
 * PWM_SetupPwm() API.
 *
 * @param base      PWM base address
 * @param pwm_chan  PWM channel select value
 */
static inline void PWM_StartTimer(PWM_Type *base, pwm_channels_t pwm_chan)
{
    base->CTRL0 |= (1 << pwm_chan);
}

/*!
 * @brief Stop PWM channel
 *
 * The API will stop PWM channel output on the pin.
 *
 * @param base      PWM base address
 * @param pwm_chan  PWM channel select value
 */
static inline void PWM_StopTimer(PWM_Type *base, pwm_channels_t pwm_chan)
{
    base->CTRL0 &= ~(1 << pwm_chan);
}

/*!
 * @brief Read current period value for PWM channel
 *
 * The API will read the current period value set for the PWM channel.
 *
 * @param base      PWM base address
 * @param pwm_chan  PWM channel select value
 *
 * @return 16-bit period value
 */
uint16_t PWM_ReadPeriodValue(PWM_Type *base, pwm_channels_t pwm_chan);

/*!
 * @brief Read compare match value for PWM channel
 *
 * The API will read the compare match value set for the PWM channel.
 *
 * @param base      PWM base address
 * @param pwm_chan  PWM channel select value
 *
 * @return 16-bit period value
 */
uint16_t PWM_ReadCompareValue(PWM_Type *base, pwm_channels_t pwm_chan);

/*! @}*/

/* @} */
#if defined(__cplusplus)
}
#endif /*_cplusplus. */
/*@}*/

#endif /* __FSL_PWM_H_*/
