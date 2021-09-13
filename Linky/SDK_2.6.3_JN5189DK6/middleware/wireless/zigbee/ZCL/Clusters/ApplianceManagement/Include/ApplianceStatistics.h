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
 * MODULE:             Appliance Statistics Cluster
 *
 * COMPONENT:          ApplianceStatistics.h
 *
 * DESCRIPTION:        Header for Appliance Statistics Cluster
 *
 *****************************************************************************/

#ifndef APPLIANCE_STATISTICS_H
#define APPLIANCE_STATISTICS_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "zcl_customcommand.h"
#include "dlist.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/*             Appliance Statistics Cluster - Optional Attributes                          */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the Appliance Statistics cluster.                                         */
/****************************************************************************/

/* Cluster ID's */
#define APPLIANCE_MANAGEMENT_CLUSTER_ID_APPLIANCE_STATISTICS                                 0x0b03

#ifndef CLD_APPLIANCE_STATISTICS_ATTR_LOG_MAX_SIZE
    #define CLD_APPLIANCE_STATISTICS_ATTR_LOG_MAX_SIZE                      0x000003C
#else
    #if(CLD_APPLIANCE_STATISTICS_ATTR_LOG_MAX_SIZE > 0x46)
        #error The maximum log size supported is 0x46 as partition cluster is not supported 
    #endif
#endif

#ifndef CLD_APPLIANCE_STATISTICS_ATTR_LOG_QUEUE_MAX_SIZE
    #define CLD_APPLIANCE_STATISTICS_ATTR_LOG_QUEUE_MAX_SIZE                1
#else
    #if(CLD_APPLIANCE_STATISTICS_ATTR_LOG_QUEUE_MAX_SIZE > 15)
        #error The maximum log queue size supported is 15 as fragmentation is not supported 
    #endif
#endif

#ifndef CLD_APPLIANCE_STATISTICS_CLUSTER_REVISION
    #define CLD_APPLIANCE_STATISTICS_CLUSTER_REVISION                     1
#endif

#ifndef CLD_APPLIANCE_STATISTICS_FEATURE_MAP
    #define CLD_APPLIANCE_STATISTICS_FEATURE_MAP                          0
#endif
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum 
{
    E_CLD_APPLIANCE_STATISTICS_ATTR_ID_LOG_MAX_SIZE                        = 0x0000, /* Mandatory */
    E_CLD_APPLIANCE_STATISTICS_ATTR_ID_LOG_QUEUE_MAX_SIZE,                           /* Mandatory */
} teCLD_ApplianceStatistics_Cluster_AttrID;

/* Server Command codes */
typedef enum 
{
    E_CLD_APPLIANCE_STATISTICS_CMD_LOG_NOTIFICATION               = 0x00,    /* Mandatory */
    E_CLD_APPLIANCE_STATISTICS_CMD_LOG_RESPONSE,                             /* Mandatory */
    E_CLD_APPLIANCE_STATISTICS_CMD_LOG_QUEUE_RESPONSE,                          /* Mandatory */
    E_CLD_APPLIANCE_STATISTICS_CMD_STATISTICS_AVAILABLE                      /* Mandatory */
} teCLD_ApplianceStatistics_ServerCommandId;

/* Client Command codes */
typedef enum 
{
    E_CLD_APPLIANCE_STATISTICS_CMD_LOG_REQUEST                    = 0x00,     /* Mandatory */
    E_CLD_APPLIANCE_STATISTICS_CMD_LOG_QUEUE_REQUEST                          /* Mandatory */
} teCLD_ApplianceStatistics_ClientCommandId;

/* Appliance Statistics Cluster */
typedef struct
{
#ifdef APPLIANCE_STATISTICS_SERVER    
    zuint32                         u32LogMaxSize;
    zuint8                          u8LogQueueMaxSize;
#endif    
    zbmap32                         u32FeatureMap;
    zuint16                         u16ClusterRevision;
} tsCLD_ApplianceStatistics;

typedef struct
{
    zutctime                     utctTime;
    uint32                       u32LogID;
    uint8                        u8LogLength;
    uint8                        *pu8LogData;
} tsCLD_LogTable;

/* Log Notification & Log Response Payload */
typedef struct
{
    zutctime                utctTime;
    zuint32                 u32LogId;
    zuint32                 u32LogLength;
    uint8                   *pu8LogData;
} tsCLD_ASC_LogNotificationORLogResponsePayload;

/* Log Queue Response  & Statistics Available Payload */
typedef struct
{
    zuint8                  u8LogQueueSize;
    zuint32                 *pu32LogId;
} tsCLD_ASC_LogQueueResponseORStatisticsAvailablePayload;

/* Log Request Payload */
typedef struct
{
    zuint32                 u32LogId;
} tsCLD_ASC_LogRequestPayload;

typedef struct
{
    uint8                                       u8CommandId;
    union
    {
        tsCLD_ASC_LogNotificationORLogResponsePayload                       *psLogNotificationORLogResponsePayload;
        tsCLD_ASC_LogQueueResponseORStatisticsAvailablePayload               *psLogQueueResponseORStatisticsAvailabePayload;
        tsCLD_ASC_LogRequestPayload                                         *psLogRequestPayload;
    } uMessage;
} tsCLD_ApplianceStatisticsCallBackMessage;

/* Custom data structure */
typedef struct
{
    tsZCL_ReceiveEventAddress sReceiveEventAddress;
    tsZCL_CallBackEvent sCustomCallBackEvent;
    tsCLD_ApplianceStatisticsCallBackMessage sCallBackMessage;
#if (defined CLD_APPLIANCE_STATISTICS) && (defined APPLIANCE_STATISTICS_SERVER)
    tsCLD_LogTable asLogTable[CLD_APPLIANCE_STATISTICS_ATTR_LOG_QUEUE_MAX_SIZE];
#endif
} tsCLD_ApplianceStatisticsCustomDataStructure;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_ApplianceStatisticsCreateApplianceStatistics(
                tsZCL_ClusterInstance                              *psClusterInstance,
                bool_t                                             bIsServer,
                tsZCL_ClusterDefinition                            *psClusterDefinition,
                void                                               *pvEndPointSharedStructPtr,
                uint8                                              *pu8AttributeControlBits,
                tsCLD_ApplianceStatisticsCustomDataStructure       *psCustomDataStructure);
                
#ifdef APPLIANCE_STATISTICS_SERVER 
PUBLIC INLINE teZCL_Status eCLD_ASCLogNotificationSend(                
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_ASC_LogNotificationORLogResponsePayload       *psPayload) ALWAYS_INLINE;
                
PUBLIC teZCL_Status eCLD_ASCLogNotificationORLogResponseSend(
                uint8                                                       u8SourceEndPointId,
                uint8                                                       u8DestinationEndPointId,
                tsZCL_Address                                               *psDestinationAddress,
                uint8                                                       *pu8TransactionSequenceNumber,
                teCLD_ApplianceStatistics_ServerCommandId                   eCommandId,
                tsCLD_ASC_LogNotificationORLogResponsePayload               *psPayload);

PUBLIC INLINE teZCL_Status eCLD_ASCLogNotificationSend(                
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_ASC_LogNotificationORLogResponsePayload       *psPayload)
{
    // handle sequence number get a new sequence number
    if(pu8TransactionSequenceNumber != NULL)
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
    
    return eCLD_ASCLogNotificationORLogResponseSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                        E_CLD_APPLIANCE_STATISTICS_CMD_LOG_NOTIFICATION,
                                                              psPayload);
}
                   
PUBLIC INLINE teZCL_Status eCLD_ASCStatisticsAvailableSend(
                uint8                                                       u8SourceEndPointId,
                uint8                                                       u8DestinationEndPointId,
                tsZCL_Address                                               *psDestinationAddress,
                uint8                                                       *pu8TransactionSequenceNumber) ALWAYS_INLINE;
                
PUBLIC teZCL_Status eCLD_ASCLogQueueResponseORStatisticsAvailableSend(
                uint8                                                       u8SourceEndPointId,
                uint8                                                       u8DestinationEndPointId,
                tsZCL_Address                                               *psDestinationAddress,
                uint8                                                       *pu8TransactionSequenceNumber,
                teCLD_ApplianceStatistics_ServerCommandId                   eCommandId);
                
PUBLIC INLINE teZCL_Status eCLD_ASCStatisticsAvailableSend(
                uint8                                                       u8SourceEndPointId,
                uint8                                                       u8DestinationEndPointId,
                tsZCL_Address                                               *psDestinationAddress,
                uint8                                                       *pu8TransactionSequenceNumber)
{
    // handle sequence number get a new sequence number
    if(pu8TransactionSequenceNumber != NULL)
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
    return eCLD_ASCLogQueueResponseORStatisticsAvailableSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                    E_CLD_APPLIANCE_STATISTICS_CMD_STATISTICS_AVAILABLE);
}                
                                        
PUBLIC teZCL_CommandStatus eCLD_ASCAddLog(
                uint8                                              u8SourceEndPointId,
                uint32                                             u32LogId,
                uint8                                              u8LogLength,
                uint32                                             u32Time,
                uint8                                              *pu8LogData);
PUBLIC teZCL_CommandStatus eCLD_ASCRemoveLog(
                uint8                                             u8SourceEndPointId,
                uint32                                            u32LogId); 
PUBLIC teZCL_CommandStatus eCLD_ASCGetLogsAvailable(
                uint8                                             u8SourceEndPointId,
                uint32                                            *pu32LogId,
                uint8                                             *pu8LogIdCount);        
PUBLIC teZCL_CommandStatus eCLD_ASCGetLogEntry(
                uint8                                             u8SourceEndPointId,
                uint32                                            u32LogId,
                tsCLD_LogTable                                  **ppsLogTable);
#endif

#ifdef APPLIANCE_STATISTICS_CLIENT    
PUBLIC teZCL_Status eCLD_ASCLogRequestSend(
                uint8                                                       u8SourceEndPointId,
                uint8                                                       u8DestinationEndPointId,
                tsZCL_Address                                               *psDestinationAddress,
                uint8                                                       *pu8TransactionSequenceNumber,
                tsCLD_ASC_LogRequestPayload                                 *psPayload);

PUBLIC teZCL_Status eCLD_ASCLogQueueRequestSend(
                uint8                                                       u8SourceEndPointId,
                uint8                                                       u8DestinationEndPointId,
                tsZCL_Address                                               *psDestinationAddress,
                uint8                                                       *pu8TransactionSequenceNumber);

#endif
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern tsZCL_ClusterDefinition sCLD_ApplianceStatistics;
extern const tsZCL_AttributeDefinition asCLD_ApplianceStatisticsClusterAttributeDefinitions[];
extern uint8 au8ApplianceStatisticsAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* APPLIANCE_STATISTICS_H */
