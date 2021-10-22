/****************************************************************************
 *
 * Copyright 2020 NXP.
 *
 * NXP Confidential. 
 * 
 * This software is owned or controlled by NXP and may only be used strictly 
 * in accordance with the applicable license terms.  
 * By expressly accepting such terms or by downloading, installing, activating 
 * and/or otherwise using the software, you are agreeing that you have read, 
 * and that you agree to comply with and are bound by, such license terms.  
 * If you do not agree to be bound by the applicable license terms, 
 * then you may not retain, install, activate or otherwise use the software. 
 * 
 *
 ****************************************************************************/


/*###############################################################################
#
# MODULE:      BDB
#
# COMPONENT:   bdb_fb_api.c
#
# DESCRIPTION: BDB Find & Bind interface 
#              
#
###############################################################################*/

#ifndef BDB_FIND_AND_BIND_H
#define BDB_FIND_AND_BIND_H

#if defined __cplusplus
extern "C" {
#endif


#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "zps_apl_aps.h"
#include "appZdpExtraction.h"
#include "bdb_api.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* The time in seconds that a device will wait before 
 * re broadcasting identify query request */
#ifndef BDB_FB_RESEND_IDENTIFY_QUERY_TIME
#define BDB_FB_RESEND_IDENTIFY_QUERY_TIME                       10
#endif

/* maximum number of target devices that the device is going to bind.*/
#ifndef BDB_FB_MAX_TARGET_DEVICES
#define BDB_FB_MAX_TARGET_DEVICES                               5
#endif

#ifndef BDB_FB_NUMBER_OF_ENDPOINTS
#define BDB_FB_NUMBER_OF_ENDPOINTS                              ZCL_NUMBER_OF_ENDPOINTS
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/*Find and Bind states */
typedef enum 
{
    E_FB_IDLE_STATE,
    E_FB_IN_PROGRESS_STATE,
    E_FB_WAIT_FOR_QUERY_RESPONSE_STATE,
    E_FB_WAIT_FOR_IEEE_RESPONSE_STATE,
    E_FB_WAIT_FOR_SIMPLE_DESCRIPTOR_RESPONSE_STATE,
    E_FB_WAIT_FOR_STOP_IDENTICIATION_STATE
}teFB_State;

/* Target Address Info */
typedef struct
{
    uint8  u8DstEndpoint;
    uint16 u16NwkAddr;
    uint64 u64IeeeAddr;
}tsFB_TargetInfo;

/* Find and Bind Structure */
typedef struct
{
    uint8                           u8Endpoint;
    teFB_State                      eFBState;
    uint8                           u8ReSendQueryTime;
    uint16                          u8CommTime;
    tsFB_TargetInfo                 asTargetInfo[BDB_FB_MAX_TARGET_DEVICES];
}tsFindAndBind;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC BDB_teStatus BDB_eFbTriggerAsInitiator(uint8 u8SourceEndPointId);
PUBLIC void BDB_vFbHandleQueryResponse(tsZCL_CallBackEvent *pCallBackEvent);
PUBLIC void BDB_vFbZclHandler(uint8 u8EventType,tsZCL_CallBackEvent *psCallBackEvent);
PUBLIC void vFbZdpHandler(ZPS_tsAfEvent *pZPSevent);
PUBLIC BDB_teStatus BDB_eFbTriggerAsTarget(uint8 u8EndPointId);
PUBLIC void BDB_vFbExitAsInitiator();
PUBLIC void BDB_vFbExitAsTarget(uint8 u8SourceEndpoint);

PUBLIC void BDB_vFbTimerCb(void *pvParam);
PUBLIC void BDB_vFbHandleStopIdentification(tsZCL_CallBackEvent *pCallBackEvent);
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern PUBLIC uint8 u8TimerBdbFb;

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif /* BDB_FIND_AND_BIND_H */
