/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/***********************************************************************/
/* INCLUDES                                                            */
/***********************************************************************/
#include "EmbeddedTypes.h"
#include "HAL_timer_driver.h"
#include "fsl_os_abstraction.h"


/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/
/***********************************************************************/
/* GLOBAL VARIABLES                                                    */
/***********************************************************************/


/***********************************************************************/
/* LOCAL FUNCTION PROTOTYPES                                           */
/***********************************************************************/
/***********************************************************************/
/* GLOBAL FUNCTIONS                                                    */
/***********************************************************************/
void HAL_Timer_Init()
{

}

void HAL_Timer_delay_ms(uint32_t ms)
{
    OSA_TimeDelay(ms);
}


