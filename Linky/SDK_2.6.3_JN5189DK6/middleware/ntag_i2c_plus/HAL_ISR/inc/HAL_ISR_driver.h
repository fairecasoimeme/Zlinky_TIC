/*
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _HAL_ISR_DRIVER_H_
#define _HAL_ISR_DRIVER_H_
/** @file HAL_ISR_driver.h
 * \brief Public interface for registering callbacks and waiting for interrupts.
 */

/***********************************************************************/
/* INCLUDES                                                            */
/***********************************************************************/
#include "global_types.h"
/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/
#ifndef API_DESCRIPTION
#define ISR_CALLBACK_LIST_BEGIN \
    typedef enum                \
    {
#define ISR_CALLBACK_ENTRY(label, channel, pin_func) label
#define ISR_CALLBACK_LIST_END \
    , ISR_MAX_CALLBACKS       \
    }                         \
    ISR_SOURCE_T;
#endif

#define ISR_DEFAULT_ARGUMENT NULL
#define ISR_INVALID_HANDLER NULL
#define ISR_EMPTY_HANDLER ISR_INVALID_HANDLER
/***********************************************************************/
/* TYPES                                                               */
/***********************************************************************/
#ifndef API_DESCRIPTION
ISR_CALLBACK_LIST_BEGIN
#include "HAL_ISR_callback_list.h" /* allowed here - generator header */
ISR_CALLBACK_LIST_END
#endif

typedef void *ISR_ARGUMENT_T;

typedef void (*ISR_HANDLER_T)(ISR_ARGUMENT_T);

typedef enum
{
    ISR_EDGE_RISE,
    ISR_EDGE_FALL,
    ISR_LEVEL_HI,
    ISR_LEVEL_LO
} ISR_MODE_T;

/***********************************************************************/
/* GLOBAL VARIABLES                                                    */
/***********************************************************************/
/***********************************************************************/
/* GLOBAL FUNCTION PROTOTYPES                                          */
/***********************************************************************/

/**
 * \brief initialize the interrupt hardware
 *
 * This function initializes the interrupt hardware and activates the
 * specified device pins as interrupt source.
 *
 * Interrupts will NOT be enabled.
 *
 * \return		none
 */
void HAL_ISR_Init(void);

/**
 * \brief register a callback with the specified interrupt source
 *
 * This function registers a callback for the specified interrupt source and
 * switches the source to the selected mode.
 *
 * This function enables the corresponding interrupt.
 *
 * \param	source 	callback will be called when this source activates
 * \param	mode   	function or mode of the source e.g level sensitive, active high
 * \param	handler	function to be called when the source triggers, may be NULL
 * \param	arg    	argument passed to callback
 *
 * \return	       	none
 */
#if defined(__LPC8XX__) || defined(__LPC11UXX__) || defined(__LPC11U37H__)
#include "fsl_pint.h"
#endif
void HAL_ISR_RegisterCallback(ISR_SOURCE_T source, ISR_MODE_T mode, ISR_HANDLER_T handler, ISR_ARGUMENT_T arg);

/**
 * \brief wait for the specified event source to trigger
 *
 * This function waits until the specified event source triggers or the timeout value
 * is reached.
 *
 * This function enables the corresponding interrupt on entry and disables it
 * before exit.
 *
 * \param	source 	  	callback will be called when this source activates
 * \param	timeout_ms	timeout value in ms
 *
 * \return	          	TRUE on timeout, FALSE if event triggered
 */
BOOL HAL_ISR_SleepWithTimeout(ISR_SOURCE_T source, uint32_t timeout_ms);

#endif /* _HAL_ISR_DRIVER_H_ */
