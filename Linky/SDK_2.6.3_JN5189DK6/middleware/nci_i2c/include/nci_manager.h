/*! *********************************************************************************
* Copyright 2020 NXP
* All rights reserved.
*
* \file
*
* This is the header file for the NCI_Manager.h file
*
** SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef _NCI_MANAGER_H_
#define _NCI_MANAGER_H_

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "global_types.h"

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
    E_NCI_MANAGER_EVENT_PRESENT, /* Should be handled by the app in a synchrous processing */
    E_NCI_MANAGER_EVENT_ABSENT, /* Should be handled by the app in a synchrous processing */
    E_NCI_MANAGER_INIT_FAIL,    /* Can be processed asynchronously by posting an event */
    E_NCI_MANAGER_INIT_OK,      /* Can be processed asynchronously by posting an event */
    E_NCI_MANAGER_READ_FAIL,    /* Can be processed asynchronously by posting an event */
    E_NCI_MANAGER_READ_OK,      /* Can be processed asynchronously by posting an event */
    E_NCI_MANAGER_WRITE_FAIL,   /* Can be processed asynchronously by posting an event */
    E_NCI_MANAGER_WRITE_OK,     /* Can be processed asynchronously by posting an event */
    E_NCI_MANAGER_NO_EVENT      /* Should never be forwarded to the app */
} eNciManagerAppEventType;

typedef struct
{
    eNciManagerAppEventType eventType;
    uint8_t *pData;
    uint32_t dataLen;
} sNciManagerEvent_t;


typedef void (*NCI_ManagerAppProcessEventCallback)(void *pEvent);

typedef void (*NCI_ManagerAppPostEventCallback)(NCI_ManagerAppProcessEventCallback appCallback, sNciManagerEvent_t *pEvent);

/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */


void NCI_ManagerInit(NCI_ManagerAppPostEventCallback appPostEventCallback, NCI_ManagerAppProcessEventCallback appProcessEventCallback);

bool_t NCI_ManagerStop(void);

void NCI_ManagerRunStateMachine(uint32_t tickMs);

bool_t NCI_ManagerWriteToSram(uint8_t *pData, uint32_t dataLen);

bool_t NCI_ManagerReadFromSram(uint8_t *pData, uint32_t dataLen);

#endif /* _NCI_MANAGER_H_ */