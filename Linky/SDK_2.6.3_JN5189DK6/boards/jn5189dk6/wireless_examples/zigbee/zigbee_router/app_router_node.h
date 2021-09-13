/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef APP_ROUTER_NODE_H_
#define APP_ROUTER_NODE_H_

#include "app_common.h"
#include "OTA.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

void APP_vInitialiseRouter(void);
void APP_vFactoryResetRecords(void);
void APP_taskRouter(void);
teNodeState eGetNodeState(void);
#ifdef CLD_OTA
tsOTA_PersistedData sGetOTACallBackPersistdata(void);
#endif

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern tsDeviceDesc sDeviceDesc;

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#endif /*APP_ROUTER_NODE_H_*/
