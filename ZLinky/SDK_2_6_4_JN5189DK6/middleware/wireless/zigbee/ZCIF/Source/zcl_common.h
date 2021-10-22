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
 * MODULE:             Smart Energy
 *
 * COMPONENT:          zcl_common.h
 *
 * DESCRIPTION:       Common structure include file
 *
 ****************************************************************************/

#ifndef ZCL_COMMON_H_INCLUDED
#define ZCL_COMMON_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "dlist.h"

#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_internal.h"
#include "zcl_heap.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#ifdef CLD_BIND_SERVER

typedef struct
{
    uint8                 u8SourceEndPointId;
    uint8                 u8DestinationEndPointId;
    uint16                u16ClusterId;
    teZCL_AddressMode     eAddressMode;
    uint16                u16PDUSize;
    uint8                 au8PDU[MAX_PDU_BIND_QUEUE_PAYLOAD_SIZE];
}tsBindServerBufferedPDUInfo;

typedef struct
{
    DLIST                 lBindServerAllocList;
    DLIST                 lBindServerDeAllocList;
}tsBindServerCustomData;

typedef struct
{
    DNODE                       dllBufferNode;
    tsBindServerBufferedPDUInfo sBindServerBufferedPDUInfo;
}tsBindServerBufferedPDURecord;


PUBLIC void vZCL_BindServerTransmissionTimerCallback(void);
#endif /* CLD_BIND_SERVER */
typedef void (* tfpZCL_ZCLZPSEventHandler)(ZPS_tsAfEvent *pZPSevent);
typedef struct
{
    uint8                       u8NumberOfEndpoints;
    uint8                       u8TransactionSequenceNumber;
    uint8                       u8ApsSequenceNumberOfLastTransmit;
    uint8                       u8NumberOfTimers;
    uint8                       u8NumberOfReports;
    bool_t                      bDisableAPSACK;
#ifdef CLD_GREENPOWER
    uint8                       u8GreenPowerMappedEpId;
#endif
    bool_t                      bTimeHasBeenSynchronised;
    bool_t                      bAvoidDirectionSearch;
    ZPS_teStatus                eLastZpsError;
    teZCL_ZCLSendSecurity       eSecuritySupported;
    uint16                      u16SystemMinimumReportingInterval;
    uint16                      u16SystemMaximumReportingInterval;
    tsZCL_EndPointRecord       *psZCL_EndPointRecord;
    tfpZCL_ZCLCallBackFunction  pfZCLinternalCallBackFunction;

    PDUM_thAPdu                 hZCL_APdu;
    uint32                      u32HeapStart;
    uint32                      u32HeapEnd;
    uint32                      u32UTCTime;
    tsZCL_TimerRecord          *psTimerRecord;
    tsZCL_ReportRecord         *psReportRecord;
    // timer
    DLIST                       lTimerAllocList;
    DLIST                       lTimerDeAllocList;
    // time
    // mutexes
    tsZCL_CallBackEvent         sZCL_InternalCallBackEvent;
    tsZCL_CallBackEvent         sZCL_CallBackEvent;
    // reports
    DLIST                       lReportAllocList;
    DLIST                       lReportDeAllocList;
    // timer
    tsZCL_CallBackEvent         sTimerCallBackEvent;
    //Message signing
    tfpZCL_ZCLMessageSignatureCreationFunction      pfZCLSignatureCreationFunction;
    tfpZCL_ZCLMessageSignatureVerificationFunction  pfZCLSignatureVerificationFunction;
    #ifdef PC_PLATFORM_BUILD
        bool_t                      bPCtestDisableAttributeChecking;
    #endif

    #ifdef CLD_BIND_SERVER
        tsBindServerCustomData     sBindServerCustomData;
    #endif

} tsZCL_Common;

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC teZCL_Status eZCL_CopyStringAttributeToPreviousValue(
                     void                      *pvStringToStore,
                     teZCL_ZCLAttributeType     eAttributeDataType,
                     tsZCL_ReportRecord        *psAttributeReportingRecord);

PUBLIC  void vZCL_SetPointerToStringStorage(
                     teZCL_ZCLAttributeType       eAttributeDataType,
                     tsZCL_ReportRecord           *psAttributeReportingRecord,
                     uint8                        *pu8PointerToStringStorage);
                     
PUBLIC  uint8* pu8ZCL_GetPointerToStringStorage(
                     teZCL_ZCLAttributeType       eAttributeDataType,
                     tsZCL_ReportRecord           *psAttributeReportingRecord);
                     
PUBLIC  void vZCL_AllocateSpaceForStringReports(
                     teZCL_ZCLAttributeType       eAttributeDataType,
                     tsZCL_ReportRecord           *psAttributeReportingRecord);
                     
PUBLIC teZCL_Status eZCL_CheckForStringAttributeChange(
                     void                      *pvStringToStore,
                     teZCL_ZCLAttributeType     eAttributeDataType,
                     tsZCL_ReportRecord        *psAttributeReportingRecord);

PUBLIC teZCL_Status eZCL_IndicateReportableChange(
                     tsZCL_ReportRecord        *psAttributeReportingRecord);

PUBLIC teZCL_Status eZCL_StoreAttributeValue(
                     void                      *pvAttributeValueToStore,
                     tsZCL_ReportRecord        *psAttributeReportingRecord);

PUBLIC teZCL_Status eZCL_StoreChangeAttributeValue(
                     tsZCL_ClusterInstance     *psClusterInstance,
                     tsZCL_AttributeDefinition *psAttributeDefinition,
                     tsZCL_ReportRecord        *psAttributeReportingRecord);

PUBLIC void vZCL_PassEventToUser(tsZCL_CallBackEvent *psZCL_CallBackEvent);

PUBLIC void vZCL_RegisterHandleGeneralCmdCallBack(void* fnPtr);

PUBLIC void vZCL_RegisterCheckForManufCodeCallBack(void* fnPtr);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern PUBLIC tsZCL_Common *psZCL_Common;

#if defined __cplusplus
}
#endif

#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/





