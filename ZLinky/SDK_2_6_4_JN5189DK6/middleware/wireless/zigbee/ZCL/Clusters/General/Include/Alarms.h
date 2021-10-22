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
 * MODULE:             Alarms Cluster
 *
 * COMPONENT:          Alarms.h
 *
 * DESCRIPTION:        Header for Alarms Cluster
 *
 *****************************************************************************/

#ifndef ALARMS_H
#define ALARMS_H

#include <jendefs.h>
#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */
#define GENERAL_CLUSTER_ID_ALARMS                 0x0009

/****************************************************************************/
/*             Alarms Cluster - Optional Attributes                         */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the cluster.                                               */
/****************************************************************************/

/* Enable the optional Alarm Count attribute */
//#define   CLD_ALARMS_ATTR_ALARM_COUNT

/* Configure the maximum number of alarms */
#ifndef CLD_ALARMS_MAX_NUMBER_OF_ALARMS
#define CLD_ALARMS_MAX_NUMBER_OF_ALARMS         (10)
#endif

#ifndef CLD_ALARMS_CLUSTER_REVISION
    #define CLD_ALARMS_CLUSTER_REVISION                     1
#endif    

#ifndef CLD_ALARMS_FEATURE_MAP
    #define CLD_ALARMS_FEATURE_MAP                          0
#endif
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Command codes - Client */
typedef enum 
{
    E_CLD_ALARMS_CMD_RESET_ALARM        = 0x00,         /* Mandatory */
    E_CLD_ALARMS_CMD_RESET_ALL_ALARMS,                  /* Mandatory */
    E_CLD_ALARMS_CMD_GET_ALARM,
    E_CLD_ALARMS_CMD_RESET_ALARM_LOG,
} teCLD_Alarms_ClientCommands;


/* Command codes - Server */
typedef enum 
{
    E_CLD_ALARMS_CMD_ALARM              = 0x00,         /* Mandatory */
    E_CLD_ALARMS_CMD_GET_ALARM_RESPONSE,
} teCLD_Alarms_ServerCommands;


/* Attribute ID's */
typedef enum 
{
    /* Alarms attribute set attribute ID's (3.11.2.1) */
    E_CLD_ALARMS_ATTR_ID_ALARM_COUNT            = 0x0000,  /* Optional */
} teCLD_Alarms_AttributeID;


/* Alarms Cluster */
typedef struct
{
#ifdef ALARMS_SERVER
    #ifdef CLD_ALARMS_ATTR_ALARM_COUNT
        zuint16                 u16AlarmCount;
    #endif
#endif
    zbmap32                 u32FeatureMap;
    
    zuint16                 u16ClusterRevision;

} tsCLD_Alarms;


/* Attribute Status */
typedef struct
{
    uint8                       u8AlarmCode;
    uint16                      u16ClusterId;
} tsCLD_AlarmsResetAlarmCommandPayload;


/* Payload for alarm command */
typedef struct
{
    uint8                       u8AlarmCode;
    uint16                      u16ClusterId;
} tsCLD_AlarmsAlarmCommandPayload;


/* Response for Get Alarm command */
typedef struct
{
    uint8                       u8Status;
    uint8                       u8AlarmCode;
    uint16                      u16ClusterId;
    uint32                      u32TimeStamp;
} tsCLD_AlarmsGetAlarmResponsePayload;


/* Alarms table entry */
typedef struct
{
    DNODE                       dllAlarmsNode;

    uint8                       u8AlarmCode;
    uint16                      u16ClusterId;
    uint32                      u32TimeStamp;
} tsCLD_AlarmsTableEntry;


/* Definition of Call back Event Structure */
typedef struct
{
    uint8                                               u8CommandId;
    union
    {
        tsCLD_AlarmsResetAlarmCommandPayload            *psResetAlarmCommandPayload;
        tsCLD_AlarmsAlarmCommandPayload                 *psAlarmCommandPayload;
        tsCLD_AlarmsGetAlarmResponsePayload             *psGetAlarmResponse;
    } uMessage;
} tsCLD_AlarmsCallBackMessage;


/* Custom data structure */
typedef struct
{
#ifdef ALARMS_SERVER    
    DLIST   lAlarmsAllocList;
    DLIST   lAlarmsDeAllocList;
#endif
    tsZCL_ReceiveEventAddress       sReceiveEventAddress;
    tsZCL_CallBackEvent             sCustomCallBackEvent;
    tsCLD_AlarmsCallBackMessage     sCallBackMessage;
#ifdef ALARMS_SERVER    
    tsCLD_AlarmsTableEntry          asAlarmsTableEntry[CLD_ALARMS_MAX_NUMBER_OF_ALARMS];
#endif    
} tsCLD_AlarmsCustomDataStructure;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_AlarmsCreateAlarms(
                    tsZCL_ClusterInstance       *psClusterInstance,
                    bool_t                      bIsServer,
                    tsZCL_ClusterDefinition     *psClusterDefinition,
                    void                        *pvEndPointSharedStructPtr,
                    uint8                       *pu8AttributeControlBits,
                    tsCLD_AlarmsCustomDataStructure *psCustomDataStructure);

#ifdef ALARMS_CLIENT                    
PUBLIC teZCL_Status eCLD_AlarmsCommandResetAlarmCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_AlarmsResetAlarmCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_AlarmsCommandResetAllAlarmsCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber);

PUBLIC teZCL_Status eCLD_AlarmsCommandGetAlarmCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber);

PUBLIC teZCL_Status eCLD_AlarmsCommandResetAlarmLogCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber);
#endif

#ifdef ALARMS_SERVER
PUBLIC teZCL_Status eCLD_AlarmsResetAlarmLog(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC teZCL_Status eCLD_AlarmsAddAlarmToLog(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8AlarmCode,
                    uint16                      u16ClusterId);

PUBLIC teZCL_Status eCLD_AlarmsGetAlarmFromLog(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       *pu8AlarmCode,
                    uint16                      *pu16ClusterId,
                    uint32                      *pu32TimeStamp);

PUBLIC teZCL_Status eCLD_AlarmsSignalAlarm(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    uint8                       u8AlarmCode,
                    uint16                      u16ClusterId);
#endif                    
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern tsZCL_ClusterDefinition sCLD_Alarms;
extern const tsZCL_AttributeDefinition asCLD_AlarmsClusterAttributeDefinitions[];
extern uint8 au8AlarmsAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* ALARMS_H */
