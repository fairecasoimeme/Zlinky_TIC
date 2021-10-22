/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef APP_MAIN_H
#define APP_MAIN_H

#include "ZQueue.h"
#include "tsv_pub.h"
#include "bdb_api.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

void APP_vInitResources(void);
void APP_vMainLoop(void);
void APP_cbTimerLed(void *pvParam);
void APP_cbTimerPoll( void *pvParam);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern uint8_t u8TimerButtonScan;
extern uint8_t  u8LedTimer;
extern uint8_t  u8TimerPoll;
extern tszQueue APP_msgBdbEvents;
extern tszQueue APP_msgAppEvents;
extern uint32_t u32Togglems;

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/



#endif /* APP_MAIN_H */






