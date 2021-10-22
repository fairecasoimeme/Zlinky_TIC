/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_ntag.h"
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.jn_ntag"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

typedef struct _ntag_handle
{
    ntag_field_detect_callback_t callback; /*!< A callback function called when Field Detect state changes */
    void *userData;                        /*!< A callback parameter passed to the callback. */
} ntag_handle_t;

ntag_handle_t s_ntagHandle;

/*******************************************************************************
 * Codes
 ******************************************************************************/

#if !(defined(FSL_FEATURE_NTAG_APP_NOT_SUPPORTED) && FSL_FEATURE_NTAG_APP_NOT_SUPPORTED)
void NFCTag_IRQHandler(void)
{
    ntag_handle_t *h = &s_ntagHandle;
    ntag_field_detect_t fd;

    /* Toggle INT_INVERT bit to catch opposite FD transition and avoid immediate ISR reentry */
    ASYNC_SYSCON->NFCTAGPADSCTRL ^= ASYNC_SYSCON_NFCTAGPADSCTRL_INT_INVERT_MASK;

    if (h->callback)
    {
        fd = (!(ASYNC_SYSCON->NFCTAGPADSCTRL & ASYNC_SYSCON_NFCTAGPADSCTRL_INT_INVERT_MASK) ? kNTAG_FieldDetectIn :
                                                                                              kNTAG_FieldDetectOut);
        h->callback(fd, h->userData);
    }
}
#endif

void NTAG_GetDefaultConfig(ntag_config_t *config)
{
    config->userData = NULL;
    config->callback = NULL;
}

void NTAG_Init(const ntag_config_t *config)
{
    ntag_handle_t *h = &s_ntagHandle;
    h->callback      = config->callback;
    h->userData      = config->userData;

    NVIC_EnableIRQ(NTAG_IRQ);
}

ntag_field_detect_t NTAG_PollFieldDetect(void)
{
    ntag_field_detect_t fd;

#ifdef RFT1503
    fd = (!(ASYNC_SYSCON->NFCTAGPADSCTRL & ASYNC_SYSCON_NFCTAGPADSCTRL_INT_INVERT_MASK) ? kNTAG_FieldDetectIn :
                                                                                          kNTAG_FieldDetectOut);
#endif

#ifdef RFT1750
    int primask, invert;

    primask = __get_PRIMASK();
    __disable_irq();
    invert = (ASYNC_SYSCON->NFCTAGPADSCTRL & ASYNC_SYSCON_NFCTAGPADSCTRL_INT_INVERT_MASK);

    /* Detect pending interrupt if currently in field */
    ASYNC_SYSCON->NFCTAGPADSCTRL |= ASYNC_SYSCON_NFCTAGPADSCTRL_INT_INVERT_MASK;
    NVIC_ClearPendingIRQ(NTAG_IRQ);
    fd = (NVIC_GetPendingIRQ(NTAG_IRQ) ? kNTAG_FieldDetectIn : kNTAG_FieldDetectOut);

    /* Revert INVERT_MASK to its original value */
    if (invert == 0)
    {
        ASYNC_SYSCON->NFCTAGPADSCTRL &= ~ASYNC_SYSCON_NFCTAGPADSCTRL_INT_INVERT_MASK;
    }
    NVIC_ClearPendingIRQ(NTAG_IRQ);
    __set_PRIMASK(primask);
#endif

    return fd;
}

void NTAG_SetState(ntag_state_t state)
{
    switch (state)
    {
        case kNTAG_StateActive:
            /*
             * NTAG pads configuration: I2C SDA/SCL, FD and VDD lines
             * This can be done only after APB bridge has been enabled
             */
#if defined(CPU_JN518X_REV) && (CPU_JN518X_REV == 1)
            ASYNC_SYSCON->NFCTAGPADSCTRL =
                (ASYNC_SYSCON_NFCTAGPADSCTRL_INT_INVERT_MASK | ASYNC_SYSCON_NFCTAGPADSCTRL_INT_ENZI_MASK |
                 ASYNC_SYSCON_NFCTAGPADSCTRL_INT_FILTEROFF_MASK | ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SCL_OD_MASK |
                 ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SCL_FILTEROFF_MASK | ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SCL_ENZI_MASK |
                 ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SDA_OD_MASK | ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SDA_FILTEROFF_MASK |
                 ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SDA_ENZI_MASK);
#else
            /* NFCTAG_VDD=1 */
            ASYNC_SYSCON->NFCTAG_VDD =
                (ASYNC_SYSCON_NFCTAG_VDD_NFCTAG_VDD_OE_MASK | ASYNC_SYSCON_NFCTAG_VDD_NFCTAG_VDD_OUT_MASK);

            /* VDD=1 (PU), FD_INT=PU, SDA/SCL=PU */
            ASYNC_SYSCON->NFCTAGPADSCTRL =
                (ASYNC_SYSCON_NFCTAGPADSCTRL_INT_INVERT_MASK | ASYNC_SYSCON_NFCTAGPADSCTRL_INT_ENZI_MASK |
                 ASYNC_SYSCON_NFCTAGPADSCTRL_INT_FILTEROFF_MASK | ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SCL_OD_MASK |
                 ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SCL_FILTEROFF_MASK | ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SCL_ENZI_MASK |
                 ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SDA_OD_MASK | ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SDA_FILTEROFF_MASK |
                 ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SDA_ENZI_MASK);
#endif
            break;
        case kNTAG_StateInactive:
#if defined(CPU_JN518X_REV) && (CPU_JN518X_REV == 1)
#else
            /* NFCTAG_VDD=0 */
            ASYNC_SYSCON->NFCTAG_VDD = ASYNC_SYSCON_NFCTAG_VDD_NFCTAG_VDD_OE_MASK;

            /* VDD=PD, FD_INT=PU, SDA/SCL=PD */
            ASYNC_SYSCON->NFCTAGPADSCTRL =
                (ASYNC_SYSCON_NFCTAGPADSCTRL_VDD_EPUN_MASK | ASYNC_SYSCON_NFCTAGPADSCTRL_VDD_EPD_MASK |
                 ASYNC_SYSCON_NFCTAGPADSCTRL_INT_INVERT_MASK | ASYNC_SYSCON_NFCTAGPADSCTRL_INT_ENZI_MASK |
                 ASYNC_SYSCON_NFCTAGPADSCTRL_INT_FILTEROFF_MASK | ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SCL_EPUN_MASK |
                 ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SCL_EPD_MASK | ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SCL_OD_MASK |
                 ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SDA_EPUN_MASK | ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SDA_EPD_MASK |
                 ASYNC_SYSCON_NFCTAGPADSCTRL_I2C_SDA_OD_MASK);
#endif
            break;
        default:
            /* Error: ignore */
            break;
    }
}
