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
 * MODULE:             Appliance Control Cluster
 *
 * COMPONENT:          ApplianceControl.h
 *
 * DESCRIPTION:        Header for Appliance Control Cluster
 *
 *****************************************************************************/

#ifndef APPLIANCE_CONTROL_H
#define APPLIANCE_CONTROL_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "zcl_customcommand.h"
#include "dlist.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/*             Appliance Control Cluster - Optional Attributes                          */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the Appliance Control  cluster.                                         */
/****************************************************************************/

/* Remaining Time attribute */
//#ifdef CLD_APPLIANCE_CONTROL_REMAINING_TIME

/* Optional Global Attribute Reporting Status */
//#ifdef CLD_APPLIANCE_CONTROL_ATTRIBUTE_REPORTING_STATUS

/* Cluster ID's */
#define APPLIANCE_MANAGEMENT_CLUSTER_ID_APPLIANCE_CONTROL                                 0x001b

#ifndef CLD_APPLIANCE_CONTROL_CLUSTER_REVISION
    #define CLD_APPLIANCE_CONTROL_CLUSTER_REVISION                     1
#endif   

#ifndef CLD_APPLIANCE_CONTROL_FEATURE_MAP
    #define CLD_APPLIANCE_CONTROL_FEATURE_MAP                          0
#endif  
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum 
{
    E_CLD_APPLIANCE_CONTROL_ATTR_ID_START_TIME                           = 0x0000, /* Mandatory */
    E_CLD_APPLIANCE_CONTROL_ATTR_ID_FINISH_TIME,                                   /* Mandatory */
    E_CLD_APPLIANCE_CONTROL_ATTR_ID_REMAINING_TIME,                                /* Optional */
} teCLD_ApplianceControl_Cluster_AttrID;

/* Client Command codes */
typedef enum 
{
    E_CLD_APPLIANCE_CONTROL_CMD_EXECUTION_OF_COMMAND                = 0x00,     /* Mandatory */
    E_CLD_APPLIANCE_CONTROL_CMD_SIGNAL_STATE,                                   /* Mandatory */
} teCLD_ApplianceControl_ClientCommandId;

/* Server Command codes */
typedef enum 
{
    E_CLD_APPLIANCE_CONTROL_CMD_SIGNAL_STATE_RESPONSE                = 0x00,     /* Mandatory */
    E_CLD_APPLIANCE_CONTROL_CMD_SIGNAL_STATE_NOTIFICATION                        /* Mandatory */
} teCLD_ApplianceControl_ServerCommandId;

/* Appliance Control Cluster */
typedef struct
{
#ifdef APPLIANCE_CONTROL_SERVER    
    zuint16                         u16StartTime;
    zuint16                         u16FinishTime;
#ifdef CLD_APPLIANCE_CONTROL_REMAINING_TIME
    zuint16                         u16RemainingTime;
#endif
#ifdef CLD_APPLIANCE_CONTROL_ATTRIBUTE_REPORTING_STATUS
    zenum8                          u8AttributeReportingStatus;
#endif
#endif
    zbmap32                         u32FeatureMap;
    zuint16                         u16ClusterRevision; 
} tsCLD_ApplianceControl;

/* Execution of Command Payload */
typedef struct
{
    zenum8                         eExecutionCommandId;
} tsCLD_AC_ExecutionOfCommandPayload;

/* Signal State Response  & Signal State Notifivcation Payload */
typedef struct
{
    zenum8                         eApplianceStatus;
    zuint8                         u8RemoteEnableFlagAndDeviceStatusTwo;
    zuint24                        u24ApplianceStatusTwo;
} tsCLD_AC_SignalStateResponseORSignalStateNotificationPayload;

typedef struct
{
    uint8                                       u8CommandId;
    bool_t                                      *pbApplianceStatusTwoPresent;
    union
    {
        tsCLD_AC_ExecutionOfCommandPayload                               *psExecutionOfCommandPayload;
        tsCLD_AC_SignalStateResponseORSignalStateNotificationPayload     *psSignalStateResponseORSignalStateNotificationPayload;
    } uMessage;
} tsCLD_ApplianceControlCallBackMessage;

/* Custom data structure */
typedef struct
{
    tsZCL_ReceiveEventAddress                 sReceiveEventAddress;
    tsZCL_CallBackEvent                       sCustomCallBackEvent;
    tsCLD_ApplianceControlCallBackMessage     sCallBackMessage;
} tsCLD_ApplianceControlCustomDataStructure;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_ApplianceControlCreateApplianceControl(
                tsZCL_ClusterInstance                              *psClusterInstance,
                bool_t                                             bIsServer,
                tsZCL_ClusterDefinition                            *psClusterDefinition,
                void                                               *pvEndPointSharedStructPtr,
                uint8                                              *pu8AttributeControlBits,
                tsCLD_ApplianceControlCustomDataStructure          *psCustomDataStructure);
                
#ifdef APPLIANCE_CONTROL_SERVER
PUBLIC INLINE teZCL_Status eCLD_ACSignalStateNotificationSend(                
                uint8                                                         u8SourceEndPointId,
                uint8                                                         u8DestinationEndPointId,
                tsZCL_Address                                                 *psDestinationAddress,
                uint8                                                         *pu8TransactionSequenceNumber,
                bool_t                                                         bApplianceStatusTwoPresent,
                tsCLD_AC_SignalStateResponseORSignalStateNotificationPayload  *psPayload) ALWAYS_INLINE;
                
PUBLIC teZCL_Status eCLD_ACSignalStateResponseORSignalStateNotificationSend(
                uint8                                                               u8SourceEndPointId,
                uint8                                                               u8DestinationEndPointId,
                tsZCL_Address                                                       *psDestinationAddress,
                uint8                                                               *pu8TransactionSequenceNumber,
                teCLD_ApplianceControl_ServerCommandId                              eCommandId,
                bool_t                                                                bApplianceStatusTwoPresent,
                tsCLD_AC_SignalStateResponseORSignalStateNotificationPayload       *psPayload);

PUBLIC INLINE teZCL_Status eCLD_ACSignalStateNotificationSend(                
                uint8                                                         u8SourceEndPointId,
                uint8                                                         u8DestinationEndPointId,
                tsZCL_Address                                                 *psDestinationAddress,
                uint8                                                         *pu8TransactionSequenceNumber,
                bool_t                                                         bApplianceStatusTwoPresent,
                tsCLD_AC_SignalStateResponseORSignalStateNotificationPayload  *psPayload)
{
    // handle sequence number get a new sequence number
    if(pu8TransactionSequenceNumber != NULL)
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
    return eCLD_ACSignalStateResponseORSignalStateNotificationSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                  E_CLD_APPLIANCE_CONTROL_CMD_SIGNAL_STATE_NOTIFICATION,
                                             bApplianceStatusTwoPresent,
                                                              psPayload);
}
                
PUBLIC teZCL_Status eCLD_ACChangeAttributeTime(
                uint8                                             u8SourceEndPointId,
                teCLD_ApplianceControl_Cluster_AttrID             eAttributeTimeId,
                uint16                                            u16TimeValue);        
#endif

#ifdef APPLIANCE_CONTROL_CLIENT                    
PUBLIC teZCL_Status eCLD_ACExecutionOfCommandSend(
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_AC_ExecutionOfCommandPayload                  *psPayload);
                    
PUBLIC teZCL_Status eCLD_ACSignalStateSend(
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber);
#endif                    
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern tsZCL_ClusterDefinition sCLD_ApplianceControl;
extern const tsZCL_AttributeDefinition asCLD_ApplianceControlClusterAttributeDefinitions[];
extern uint8 au8ApplianceControlAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* APPLIANCE_CONTROL_H */
