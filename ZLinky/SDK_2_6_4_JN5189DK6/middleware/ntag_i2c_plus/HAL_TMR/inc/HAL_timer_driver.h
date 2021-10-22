/*
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/** @file HAL_timer_driver.h
 * \brief Public interface to access the timing hardware.
 */
#ifndef _HAL_TIMER_DRIVER_H_
#define _HAL_TIMER_DRIVER_H_

/***********************************************************************/
/* INCLUDES                                                            */
/***********************************************************************/
#include "global_types.h"

/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/
/***********************************************************************/
/* TYPES                                                               */
/***********************************************************************/
/***********************************************************************/
/* GLOBAL VARIABLES                                                    */
/***********************************************************************/
/***********************************************************************/
/* GLOBAL FUNCTION PROTOTYPES                                          */
/***********************************************************************/

/**
 * \brief initialize the timing hardware
 *
 * This function prepares the timing hardware to points where calling
 * any of the following delay functions guarantees at least the desired delay
 * time while not blocking indefinitely.
 *
 * \return		none
 */
void HAL_Timer_Init(void);

/**
 * \brief wait for the specified number of milliseconds
 *
 * This function delays the current thread of execution for at least the
 * number of specified milliseconds. In tasked or threaded environments
 * this function may additionally yield the CPU.
 *
 * \param	ms	wait time in milliseconds
 * \return		none
 */
void HAL_Timer_delay_ms(uint32_t ms);

/**
 * \brief returns the current time in ms
 *
 * This function returns the current value of the ms tick timer.
 *
 * \return		current time in ms
 */
uint32_t HAL_Timer_getTime_ms(void);

#endif /* _HAL_TIMER_DRIVER_H_ */
