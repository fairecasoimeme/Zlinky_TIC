/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef ZIGBEE_CONFIG_H
#define ZIGBEE_CONFIG_H


/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "EmbeddedTypes.h"
#include "ZQueue.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define ZIGBEE_TIMER_STORAGE         (6)
	
#define BDB_QUEUE_SIZE               (2)
#define TIMER_QUEUE_SIZE             (8)
#define MLME_QUEQUE_SIZE             (9)
#define MCPS_QUEUE_SIZE              (16)
#define MCPS_DCFM_QUEUE_SIZE         (8)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
void APP_vRunZigbee(void);
void APP_vStopZigbeeTimers(void);
void APP_vInitZigbeeResources(void);
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern uint8_t u8TimerZCL;

extern tszQueue APP_msgBdbEvents;
extern tszQueue zps_msgMlmeDcfmInd;
extern tszQueue zps_msgMcpsDcfmInd;
extern tszQueue zps_msgMcpsDcfm;
extern tszQueue zps_TimeEvents;
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
#endif /* ZIGBEE_CONFIG_H */
