/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017, 2019-2020 NXP
* All rights reserved.
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef __PWR_LIB_H__
#define __PWR_LIB_H__

/*****************************************************************************
 *                               INCLUDED HEADERS                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the headers that this module needs to include.    *
 * Note that it is not a good practice to include header files into header   *
 * files, so use this section only if there is no other better solution.     *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
 #include "PWR_Interface.h"

/*****************************************************************************
 *                             PUBLIC MACROS                                *
 *---------------------------------------------------------------------------*
 * Add to this section all the access macros, registers mappings, bit access *
 * macros, masks, flags etc ...
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*****************************************************************************
 *                             PRIVATE MACROS                                *
 *---------------------------------------------------------------------------*
 * Add to this section all the access macros, registers mappings, bit access *
 * macros, masks, flags etc ...
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*
 *  define gSwdWakeupReconnect_c as 1 to ensure that SWD can wake device up and
 *  attach debugger when powered down
 */
#ifndef gSwdWakeupReconnect_c
#define  gSwdWakeupReconnect_c 0
#endif

#ifndef SWDIO_GPIO_PIN
#define SWDIO_GPIO_PIN 13U
#endif

/*****************************************************************************
 *                        PRIVATE TYPE DEFINITIONS                           *
 *---------------------------------------------------------------------------*
 * Add to this section all the data types definitions: stuctures, unions,    *
 * enumerations, typedefs ...                                                *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

typedef struct
{
	uint64_t wakeup_src;
	uint32_t wakeup_io;
	uint32_t sleep_cfg;
} pwrlib_pd_cfg_t;

/*****************************************************************************
 *                               PUBLIC VARIABLES(External)                  *
 *---------------------------------------------------------------------------*
 * Add to this section all the variables and constants that have global      *
 * (project) scope.                                                          *
 * These variables / constants can be accessed outside this module.          *
 * These variables / constants shall be preceded by the 'extern' keyword in  *
 * the interface header.                                                     *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
extern volatile PWR_WakeupReason_t PWRLib_MCU_WakeupReason;


/*****************************************************************************
 *                            PUBLIC FUNCTIONS                               *
 *---------------------------------------------------------------------------*
 * Add to this section all the global functions prototype preceded (as a     *
 * good practice) by the keyword 'extern'                                    *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*---------------------------------------------------------------------------
 * Name: PWRLib_Init
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_Init(void);

/*---------------------------------------------------------------------------
 * Name: PWRLib_MCUEnter_Sleep
 * Description: Puts the processor into Sleep .

                Mode of operation details:
                 - ARM core enters Sleep Mode
                 - ARM core is clock gated (HCLK = OFF)
                 - peripherals are functional
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_MCU_Enter_Sleep(void);

/*---------------------------------------------------------------------------
 * Name: PWRLib_SetLowPowerMode
 * Description:
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_SetDeepSleepMode(uint8_t lpMode);

/*---------------------------------------------------------------------------
 * Name: PWRLib_GetDeepSleepMode
 * Description:
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PWRLib_GetDeepSleepMode(void);

/*---------------------------------------------------------------------------
 * Name: PWRLib_ClearWakeupReason
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_ClearWakeupReason(void);

/*---------------------------------------------------------------------------
 * Name: PWRLib_EnterPowerDownModeRamOn
 * Description: - request to switch in power down mode
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_EnterPowerDownModeRamOn(uint32_t mode);

/*---------------------------------------------------------------------------
 * Name: PWRLib_EnterPowerDownModeRamOff
 * Description: - request to switch in power down mode
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_EnterPowerDownModeRamOff(pwrlib_pd_cfg_t *pd_cfg);

/*---------------------------------------------------------------------------
 * Name: PWRLib_EnterDeepDownMode
 * Description: - request to switch in power down mode
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_EnterDeepDownMode(pwrlib_pd_cfg_t *pd_cfg);

/*---------------------------------------------------------------------------
 * Name: PWR_UpdateWakeupReason
 * Description: - request to switch in power down mode
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_UpdateWakeupReason(void);

/*---------------------------------------------------------------------------
 * Name: PWRLib_u32RamBanksSpanned
 * Description: - returns bitmap of RAM banks spanned by given address range
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint32_t PWRLib_u32RamBanksSpanned(uint32_t u32Start, uint32_t u32Length);

void PWR_SetWakeUpConfig(uint32_t set_msk, uint32_t clr_msk);
uint32_t PWR_GetWakeUpConfig(void);
void vSetWakeUpIoConfig(void);
uint64_t u64GetWakeupSourceConfig(uint32_t u32Mode);

#endif /* __PWR_LIB_H__ */
