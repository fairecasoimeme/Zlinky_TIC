/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_cmp.h"
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.jn_cmp"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
void CMP_Init(cmp_config_t *config)
{
    assert(config);

    uint32_t anaComp = 0U;

    anaComp = PMC->ANAMUXCOMP & (~(PMC_ANAMUXCOMP_COMP_HYST_MASK | PMC_ANAMUXCOMP_COMP_INNINT_MASK |
                                   PMC_ANAMUXCOMP_COMP_LOWPOWER_MASK | PMC_ANAMUXCOMP_COMP_INPUTSWAP_MASK));

    anaComp |= (config->src << PMC_ANAMUXCOMP_COMP_INNINT_SHIFT) |
               (config->mode << PMC_ANAMUXCOMP_COMP_LOWPOWER_SHIFT) |
               (((uint32_t)(config->enLowHysteris)) << PMC_ANAMUXCOMP_COMP_HYST_SHIFT) |
               (((uint32_t)(config->enInputSwap)) << PMC_ANAMUXCOMP_COMP_INPUTSWAP_SHIFT);

    PMC->ANAMUXCOMP = anaComp;
}

void CMP_Deinit(void)
{
    /* nothing to do */
}

void CMP_SetInterruptConfig(cmp_interrupt_mask_t mask)
{
    uint32_t anaCtrl = SYSCON->ANACTRL_CTRL;

    if (mask & SYSCON_ANACTRL_CTRL_COMPINTRLVL_MASK)
    {
        /* level sensitive interrupt */
        anaCtrl |= SYSCON_ANACTRL_CTRL_COMPINTRLVL_MASK;
    }
    else
    {
        anaCtrl &= ~SYSCON_ANACTRL_CTRL_COMPINTRLVL_MASK;
    }
    /* interrupt polarity */
    anaCtrl = (anaCtrl & (~SYSCON_ANACTRL_CTRL_COMPINTRPOL_MASK)) |
              SYSCON_ANACTRL_CTRL_COMPINTRPOL(mask >> CMP_INT_POL_SHIFT_VALUE);

    SYSCON->ANACTRL_CTRL = anaCtrl;
}
