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


/****************************************************************************
 *
 * MODULE:             Zigbee Demand Response And Load Control Cluster
 *
 * DESCRIPTION:
 * The Internal API for the Jennic DRLC Cluster
 *
 ****************************************************************************/

#ifndef  DRLC_INTERNAL_H_INCLUDED
#define  DRLC_INTERNAL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"

#include "zcl.h"
#include "DRLC.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teSE_DRLCStatus eSE_DRLCLoadControlEventSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address              *psDestinationAddress,
                    tsSE_DRLCLoadControlEvent  *psLoadControlEvent,
                    bool_t                      bSendWithTimeNow,
                    uint8                      *pu8TransactionSequenceNumber);

PUBLIC teSE_DRLCStatus eSE_DRLCLoadControlEventReceive(
                    ZPS_tsAfEvent               *pZPSevent, 
                    tsZCL_EndPointDefinition    *psEndPointDefinition, 
                    tsSE_DRLCLoadControlEvent   *psLoadControlEvent,
                    uint8                       *pu8TransactionSequenceNumber);

PUBLIC teSE_DRLCStatus eSE_DRLCCancelLoadControlEventSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address              *psDestinationAddress,
                    tsSE_DRLCCancelLoadControlEvent *psCancelLoadControlEvent,
                    uint8                      *pu8TransactionSequenceNumber);

PUBLIC teSE_DRLCStatus eSE_DRLCCancelLoadControlEventReceive(
                    ZPS_tsAfEvent               *pZPSevent, 
                    tsZCL_EndPointDefinition    *psEndPointDefinition, 
                    tsSE_DRLCCancelLoadControlEvent *psCancelLoadControlEvent,
                    uint8                       *pu8TransactionSequenceNumber);

PUBLIC teSE_DRLCStatus eSE_DRLCCancelAllLoadControlEventsSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address              *psDestinationAddress,
                    teSE_DRLCCancelControl     eSE_CancelEventsControl,
                    uint8                      *pu8TransactionSequenceNumber);

PUBLIC teSE_DRLCStatus eSE_DRLCCancelAllLoadControlEventsReceive(
                    ZPS_tsAfEvent               *pZPSevent, 
                    tsZCL_EndPointDefinition    *psEndPointDefinition, 
                    tsSE_DRLCCancelLoadControlAllEvent *pEvent,
                    uint8                       *pu8TransactionSequenceNumber);

PUBLIC teSE_DRLCStatus eSE_DRLCGetScheduledEventsReceive(
                    ZPS_tsAfEvent               *pZPSevent, 
                    tsZCL_EndPointDefinition    *psEndPointDefinition, 
                    tsSE_DRLCGetScheduledEvents *psGetScheduledEvents,
                    uint8                       *pu8TransactionSequenceNumber);

PUBLIC teSE_DRLCStatus eSE_DRLCReportEventStatusReceive(
                    ZPS_tsAfEvent               *pZPSevent, 
                    tsZCL_EndPointDefinition    *psEndPointDefinition, 
                    tsSE_DRLCReportEvent        *psReportEvent,
                    uint8                       *pu8TransactionSequenceNumber);

PUBLIC void vSE_DRLCHandleReportEventStatus(
                    ZPS_tsAfEvent               *pZPSevent, 
                    tsZCL_EndPointDefinition    *psEndPointDefinition, 
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC void vSE_DRLCHandleGetScheduledEvents(
                    ZPS_tsAfEvent               *pZPSevent, 
                    tsZCL_EndPointDefinition    *psEndPointDefinition, 
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC void vSE_DRLCHandleLoadControlEvent(
                    ZPS_tsAfEvent               *pZPSevent, 
                    tsZCL_EndPointDefinition    *psEndPointDefinition, 
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC void vSE_DRLCHandleLoadControlEventCancel(
                    ZPS_tsAfEvent               *pZPSevent, 
                    tsZCL_EndPointDefinition    *psEndPointDefinition, 
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC void vSE_DRLCHandleLoadControlEventCancelAll(
                    ZPS_tsAfEvent               *pZPSevent, 
                    tsZCL_EndPointDefinition    *psEndPointDefinition, 
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC teSE_DRLCStatus eSE_DRLCGetScheduledEventsReceive(
                    ZPS_tsAfEvent               *pZPSevent, 
                    tsZCL_EndPointDefinition    *psEndPointDefinition, 
                    tsSE_DRLCGetScheduledEvents *psGetScheduledEvents,
                    uint8                       *pu8TransactionSequenceNumber);

PUBLIC teSE_DRLCStatus eSE_DRLCAddLoadControlEventNoMutex(
                    tsZCL_EndPointDefinition     *psEndPointDefinition,
                    tsZCL_ClusterInstance        *psClusterInstance,
                    tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure,
                    tsSE_DRLCLoadControlEvent    *psLoadControlEvent);

PUBLIC void vSetRandomizationValues(
                    tsZCL_EndPointDefinition     *psEndPointDefinition, 
                    tsZCL_ClusterInstance        *psClusterInstance,
                    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord);

PUBLIC bool_t boInEffectiveTimeExtendedEventOperation(
                    tsSE_DRLCLoadControlEvent   *psLoadControlEvent, 
                    uint32                       u32UTCtime);

PUBLIC bool_t boEffectiveExpiredTimeCheck(
                    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord, 
                    tsZCL_EndPointDefinition    *psEndPointDefinition, 
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint32                       u32UTCtime);

PUBLIC bool_t boInEffectiveActiveTimeCheck(
                    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord, 
                    tsZCL_EndPointDefinition    *psEndPointDefinition, 
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint32                       u32UTCtime);

PUBLIC uint32 u32EffectiveDuration(
                    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord, 
                    tsZCL_EndPointDefinition    *psEndPointDefinition, 
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC uint32 boCancelTimeCheck(
                    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord, 
                    tsZCL_EndPointDefinition   *psEndPointDefinition, 
                    tsZCL_ClusterInstance      *psClusterInstance,
                    uint32                      u32UTCtime);

PUBLIC teSE_DRLCStatus eSE_DRLCFindDRLCCluster(
                    uint8                        u8SourceEndPointId,
                    bool_t                       bIsServer,
                    tsZCL_EndPointDefinition     **ppsEndPointDefinition,
                    tsZCL_ClusterInstance        **ppsClusterInstance,
                    tsSE_DRLCCustomDataStructure **ppsDRLCCustomDataStructure);

PUBLIC teSE_DRLCStatus eSE_DRLCAddToSchedulerList(
                    tsZCL_EndPointDefinition           *psEndPointDefinition, 
                    tsZCL_ClusterInstance              *psClusterInstance,
                    tsSE_DRLCCustomDataStructure       *psDRLCCustomDataStructure,
                    tsSE_DRLCLoadControlEventRecord    *psLoadControlEventRecord, 
                    teSE_DRLCEventList                  eFromEventList,
                    teSE_DRLCEventList                  eToEventList);

PUBLIC teSE_DRLCStatus eSE_DRLCOverlapAddEventToScheduler(
                    tsZCL_EndPointDefinition           *psEndPointDefinition, 
                    tsZCL_ClusterInstance              *psClusterInstance,
                    tsSE_DRLCLoadControlEventRecord    *psLoadControlEventAddRecord, 
                    tsSE_DRLCCustomDataStructure       *psDRLCCustomDataStructure,
                    teSE_DRLCEventList                  eFromEventList,
                    teSE_DRLCEventList                  eToEventList);

PUBLIC teSE_DRLCStatus eSE_DRLCCancelLoadControlEventNoMutex(
                    tsZCL_EndPointDefinition           *psZCL_EndPointDefinition, 
                    tsZCL_ClusterInstance              *psClusterInstance,
                    tsSE_DRLCCustomDataStructure       *psDRLCCustomDataStructure,
                    tsSE_DRLCCancelLoadControlEvent    *psCancelLoadControlEvent);

PUBLIC tsSE_DRLCLoadControlEventRecord * psGetListHead(
                    tsSE_DRLCCustomDataStructure       *psDRLCCustomDataStructure, 
                    teSE_DRLCEventList                  eEventList,
                    DLIST                             **pplEventList);

PUBLIC teSE_DRLCStatus eSE_DRLCGenerateReportEventStatusMessage(
                    tsZCL_ClusterInstance              *psClusterInstance,
                    tsSE_DRLCReportEvent               *psReportEvent);

PUBLIC teSE_DRLCStatus eSE_DRLCFindDRLCCluster(
                    uint8                               u8SourceEndPointId,
                    bool_t                              bIsServer,
                    tsZCL_EndPointDefinition         **ppsEndPointDefinition,
                    tsZCL_ClusterInstance            **ppsClusterInstance,
                    tsSE_DRLCCustomDataStructure     **ppsDRLCCustomDataStructure);

PUBLIC teSE_DRLCStatus eSE_CancelAllLoadControlEventsReceive(
                    ZPS_tsAfEvent                      *pZPSevent, 
                    tsZCL_EndPointDefinition           *psEndPointDefinition, 
                    teSE_DRLCCancelControl             *peCancelEventControl,
                    uint8                              *pu8TransactionSequenceNumber);

PUBLIC void vRemoveFromSchedulerList(
                    tsSE_DRLCCustomDataStructure       *psDRLCCustomDataStructure,
                    tsSE_DRLCLoadControlEventRecord    *psLoadControlEventRecord,
                    teSE_DRLCEventList                  eEventList);

PUBLIC void vSimpleAddEventToScheduler(
                    tsSE_DRLCCustomDataStructure       *psDRLCCustomDataStructure,
                    tsSE_DRLCLoadControlEventRecord    *psLoadControlEventRecord,
                    teSE_DRLCEventList                  eFromEventList,
                    teSE_DRLCEventList                  eToEventList);

PUBLIC tsSE_DRLCLoadControlEventRecord * psCheckToSeeIfEventExists(
                   tsSE_DRLCCustomDataStructure        *psDRLCCustomDataStructure, 
                   uint32                               u32IssuerId,
                   teSE_DRLCEventList                  *peEventList);

PUBLIC teSE_DRLCStatus eSE_DRLCSchedulerUpdate(
                   uint8                                u8SourceEndPointId, 
                   bool_t                               bIsServer, 
                   uint32                               u32UTCtime);

PUBLIC teSE_DRLCStatus eDRLC_SetCallBackMessage(
                   tsDRLC_Common                       *psDRLC_Common,
                   teSE_DRLCCallBackEventType           eEventType,
                   uint8                                u8CommandId,
                   teSE_DRLCStatus                      eDRLCStatus);

PUBLIC teZCL_Status eSE_DRLCCommandHandler(
                    ZPS_tsAfEvent                      *pZPSevent, 
                    tsZCL_EndPointDefinition           *psEndPointDefinition, 
                    tsZCL_ClusterInstance              *psClusterInstance);

PUBLIC teSE_DRLCStatus eSE_DRLCInitialiseReportStructure(
                    tsSE_DRLCReportEvent               *psReportEvent, 
                    uint32                              u32IssuerId,
                    uint8                               u8CriticalityLevel,
                    uint8                               u8EventControl);


/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* DRLC_INTERNAL_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
