/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __FSL_CMP_H_
#define __FSL_CMP_H_

#include "fsl_common.h"

/*!
 * @addtogroup jn_cmp
 * @{
 */

/******************************************************************************
 * Definitions.
 *****************************************************************************/
/*! @name Driver version */
/*@{*/
/*! @brief Driver version 2.0.1. */
#define FSL_CMP_DRIVER_VERSION (MAKE_VERSION(2U, 0U, 1U))
/*@}*/

/*! @brief cmp level shift value definition */
#define CMP_INT_POL_SHIFT_VALUE (1U)

/*! @brief cmp status */
enum _cmp_status
{
    kCMP_In0BiggerThanIn1 = 1U, /*!< comparator input 0 is bigger than input 1 */
    kCMP_In1BiggerThanIn0 = 0U, /*!< comparator input 1 is bigger than input 0 */
};

/*! @brief cmp interrupt */
typedef enum _cmp_interrupt_mask
{
    kCMP_EdgeRising        = 0U << CMP_INT_POL_SHIFT_VALUE, /*!< Edge sensitive, falling edge */
    kCMP_EdgeFalling       = 1U << CMP_INT_POL_SHIFT_VALUE, /*!< Edge sensitive, rising edge */
    kCMP_EdgeRisingFalling = 3U << CMP_INT_POL_SHIFT_VALUE, /*!< Edge sensitive, rising and falling edge */

    kCMP_LevelLow  = (0U << CMP_INT_POL_SHIFT_VALUE) | 1U, /*!< Level sensitive, low level */
    kCMP_LevelHigh = (2U << CMP_INT_POL_SHIFT_VALUE) | 1U, /*!< Level sensitive, high level */
} cmp_interrupt_mask_t;

/*! @brief cmp work mode */
typedef enum _cmp_mode
{
    kCMP_FastMode     = 0U, /*!< Used in an active or deep sleep mode, this mode requires PMU bias enabled */
    kCMP_LowpowerMode = 1U, /*!< Used for all power mode, doesn't require PMU bias enabled */
} cmp_mode_t;

/*! @brief cmp input source */
typedef enum _cmp_input
{
    kCMP_InputAllExternal = 0U, /*!< Cmp input from two external source */
    kCMP_InputOneExternalOneInternal =
        1U, /*!< Cmp input from one external input and one internal voltage reference 0.8V */
} cmp_input_t;

/*! @brief cmp configurataions */
typedef struct _cmp_config
{
    bool enLowHysteris; /*!< low hysteresis */
    bool enInputSwap;   /*!< input swap */
    cmp_input_t src;    /*!< input source select */
    cmp_mode_t mode;    /*!< cmp work mode */
} cmp_config_t;
/*************************************************************************************************
 * API
 ************************************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Cmp Initialization and deinitialization
 * @{
 */

/*!
 * @brief CMP intialization.
 * Note: The cmp initial function not responsible for cmp power, application shall handle it.
 *
 * @param config init configurations.
 */
void CMP_Init(cmp_config_t *config);

/*!
 * @brief CMP deintialization.
 * Note: The cmp deinit function not responsible for cmp power, application shall handle it.
 *
 */
void CMP_Deinit(void);

/* @} */

/*!
 * @name cmp functionality
 * @{
 */

/*!
 * @brief Swap the external input channel.
 *
 * @param base CMP base address.
 */
static inline void CMP_SwapExtInput(void)
{
    PMC->ANAMUXCOMP |= PMC_ANAMUXCOMP_COMP_INPUTSWAP_MASK;
}

/*!
 * @brief switch cmp work mode.
 *
 * @param enable true is enter low power mode, false is enter fast mode
 */
static inline void CMP_EnableLowePowerMode(bool enable)
{
    if (enable)
    {
        PMC->ANAMUXCOMP |= PMC_ANAMUXCOMP_COMP_LOWPOWER_MASK;
    }
    else
    {
        PMC->ANAMUXCOMP &= ~PMC_ANAMUXCOMP_COMP_LOWPOWER_MASK;
    }
}

/*!
 * @brief switch input source.
 *
 * @param enable true is one external and one internal, false is all external.
 */
static inline void CMP_EnableInnerInput(bool enable)
{
    if (enable)
    {
        PMC->ANAMUXCOMP |= PMC_ANAMUXCOMP_COMP_INNINT_MASK;
    }
    else
    {
        PMC->ANAMUXCOMP &= ~PMC_ANAMUXCOMP_COMP_INNINT_MASK;
    }
}

/*!
 * @brief cmp enable low hysteresis.
 *
 */
static inline void CMP_EnableLowHysteresis(bool enable)
{
    if (enable)
    {
        PMC->ANAMUXCOMP |= PMC_ANAMUXCOMP_COMP_HYST_MASK;
    }
    else
    {
        PMC->ANAMUXCOMP &= ~PMC_ANAMUXCOMP_COMP_HYST_MASK;
    }
}

/*!
 * @brief cmp output status.
 *
 * @return 0 is kCMP_In1BiggerThanIn0, 1 is kCMP_In0BiggerThanIn1.
 */
static inline uint32_t CMP_GetOutput(void)
{
    return (SYSCON->ANACTRL_VAL & SYSCON_ANACTRL_VAL_ANACOMP_MASK) >> SYSCON_ANACTRL_VAL_ANACOMP_SHIFT;
}

/* @} */

/*!
 * @name cmp interrupt
 * @{
 */

/*!
 * @brief cmp set interrupt configurations.
 *
 * @param mask interrupt mask.
 */
void CMP_SetInterruptConfig(cmp_interrupt_mask_t mask);

/*!
 * @brief cmp enable interrupt.
 *
 */
static inline void CMP_EnableInterrupt(void)
{
    SYSCON->ANACTRL_INTENSET |= SYSCON_ANACTRL_INTENSET_ANACOMP_MASK;
}

/*!
 * @brief cmp disable interrupt.
 *
 */
static inline void CMP_DisableInterrupt(void)
{
    SYSCON->ANACTRL_INTENCLR |= SYSCON_ANACTRL_INTENCLR_ANACOMP_MASK;
}

/*!
 * @brief cmp get status.
 *
 * @return true is interrupt pending, false is no interrupt pending.
 */
static inline bool CMP_GetStatus(void)
{
    return (SYSCON->ANACTRL_STAT & SYSCON_ANACTRL_STAT_ANACOMP_MASK) ? true : false;
}

/*!
 * @brief cmp clear interrupt status.
 *
 * @return true is interrupt pending, false is no interrupt pending.
 */
static inline void CMP_ClearStatus(void)
{
    SYSCON->ANACTRL_STAT |= SYSCON_ANACTRL_STAT_ANACOMP_MASK;
}

/*!
 * @brief cmp get interrupt status.
 *
 * @return true is interrupt pending, false is no interrupt pending.
 */
static inline bool CMP_GetInterruptStatus(void)
{
    return (SYSCON->ANACTRL_INTSTAT & SYSCON_ANACTRL_INTSTAT_ANACOMP_MASK) ? true : false;
}
/* @} */

#if defined(__cplusplus)
}
#endif

/*! @} */
#endif /* __FSL_CMP_H_ */
