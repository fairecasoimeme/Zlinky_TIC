/*! *********************************************************************************
* Copyright 2020 NXP
* All rights reserved.
*
* \file
*
* This is the header file for the NTAG_Manager.h file
*
** SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef _APP_NTAG_CORE_STATE_H_
#define _APP_NTAG_CORE_STATE_H_

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "EmbeddedTypes.h"
#include "ntag_driver.h"

/*! *********************************************************************************
*************************************************************************************
* Public MACRO
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
********************************************************************************** */

typedef enum
{
    E_APP_NTAG_CORE_STATE_EVENT_FD,
    E_APP_NTAG_CORE_STATE_EVENT_READ_SRAM_FAIL,
    E_APP_NTAG_CORE_STATE_EVENT_READ_SRAM_OK,
    E_APP_NTAG_CORE_STATE_EVENT_WRITE_SRAM_FAIL,
    E_APP_NTAG_CORE_STATE_EVENT_WRITE_SRAM_OK,
    E_APP_NTAG_CORE_STATE_EVENT_NO_EVENT /* Should never be forwarded to the app */
} eAppNtagCoreStateEventType;

typedef struct
{
    eAppNtagCoreStateEventType eventType;
    uint8_t *pData;
    uint32_t dataLen;
} sAppNtagCoreStateEvent_t;


typedef void (*AppProcessEventCallback)(void *pEvent);

typedef void (*AppPostEventCallback)(AppProcessEventCallback appCallback, sAppNtagCoreStateEvent_t *pEvent);

/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */


void app_ntag_core_state_init(AppPostEventCallback appPostEventCallback, 
                        AppProcessEventCallback appProcessEventCallback,
                        NTAG_HANDLE_T ntagDriverHandle);

void app_ntag_core_state_runStateMachine();

bool_t app_ntag_core_state_isInField();

bool_t app_ntag_core_state_writeToSram(uint8_t *pData, uint32_t dataLen);

bool_t app_ntag_core_state_readFromSram(uint8_t *pData, uint32_t dataLen);

#endif /* _APP_NTAG_CORE_STATE_H_ */