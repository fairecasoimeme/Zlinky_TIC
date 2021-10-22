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


/*****************************************************************************
 *
 * MODULE:             Groups Cluster
 *
 * COMPONENT:          Groups_internal.h
 *
 * DESCRIPTION:        The internal API for the Groups Cluster
 *
 *****************************************************************************/

#ifndef  APPLIANCE_EVENTS_AND_ALERTS_INTERNAL_H_INCLUDED
#define  APPLIANCE_EVENTS_AND_ALERTS_INTERNAL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"

#include "zcl.h"
#include "ApplianceEventsAndAlerts.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_ApplianceEventsAndAlertsCommandHandler(
                ZPS_tsAfEvent               *pZPSevent,
                tsZCL_EndPointDefinition    *psEndPointDefinition,
                tsZCL_ClusterInstance       *psClusterInstance);

#ifdef APPLIANCE_EVENTS_AND_ALERTS_SERVER
PUBLIC teZCL_Status eCLD_AEAAGetAlertsReceive(
                ZPS_tsAfEvent                                   *pZPSevent,
                uint8                                           *pu8TransactionSequenceNumber);
#endif

#ifdef APPLIANCE_EVENTS_AND_ALERTS_CLIENT
PUBLIC teZCL_Status eCLD_AEAAGetAlertsResponseORAlertsNotificationReceive(
                ZPS_tsAfEvent                                                               *pZPSevent,
                uint8                                                                       *pu8TransactionSequenceNumber,
                tsCLD_AEAA_GetAlertsResponseORAlertsNotificationPayload                     *psPayload);

PUBLIC teZCL_Status eCLD_AEAAEventNotificationReceive(
                ZPS_tsAfEvent                                                               *pZPSevent,
                uint8                                                                       *pu8TransactionSequenceNumber,
                tsCLD_AEAA_EventNotificationPayload                                         *psPayload);
#endif

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* APPLIANCE_EVENTS_AND_ALERTS_INTERNAL_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
