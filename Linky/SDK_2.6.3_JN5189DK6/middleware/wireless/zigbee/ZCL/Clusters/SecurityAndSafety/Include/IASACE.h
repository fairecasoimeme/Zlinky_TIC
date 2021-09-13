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
 * MODULE:             IAS ACE Cluster
 *
 * COMPONENT:          IASACE.h
 *
 * DESCRIPTION:        Header for IAS ACE Cluster
 *
 *****************************************************************************/

#ifndef IASACE_H
#define IASACE_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */

#define SECURITY_AND_SAFETY_CLUSTER_ID_IASACE                                  0x0501

#ifndef CLD_IASACE_ZONE_TABLE_SIZE
    #define CLD_IASACE_ZONE_TABLE_SIZE                                         8
#endif    

#define CLD_IASACE_MAX_BYTES_FOR_NUM_OF_ZONES                                  16

#ifndef CLD_IASACE_MAX_LENGTH_ARM_DISARM_CODE
    #define CLD_IASACE_MAX_LENGTH_ARM_DISARM_CODE                                     8 
#endif

#ifndef CLD_IASACE_MAX_LENGTH_ZONE_LABEL
    #define CLD_IASACE_MAX_LENGTH_ZONE_LABEL                                          16
#endif

#ifndef CLD_IASACE_CLUSTER_REVISION
    #define CLD_IASACE_CLUSTER_REVISION                                                1
#endif 


#ifndef CLD_IASACE_FEATURE_MAP
    #define CLD_IASACE_FEATURE_MAP                                                     0
#endif 
/* Bitmap to set the configuration of a particular zone */

#define CLD_IASACE_ZONE_CONFIG_FLAG_BYPASS                                     (1<<0)
#define CLD_IASACE_ZONE_CONFIG_FLAG_DAY_HOME                                   (1<<1)
#define CLD_IASACE_ZONE_CONFIG_FLAG_NIGHT_SLEEP                                (1<<2)
#define CLD_IASACE_ZONE_CONFIG_FLAG_NOT_BYPASSED                               (1<<3)

/* Bitmap for the status of a particular zone */

#define CLD_IASACE_ZONE_STATUS_FLAG_BYPASS                                     (1<<0)
#define CLD_IASACE_ZONE_STATUS_FLAG_ARM                                        (1<<1)

/****************************************************************************/
/*             IASACE Cluster - Optional Attributes                     */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the cluster.                                               */
/****************************************************************************/

/* End of optional attributes */


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* Server Command codes */
typedef enum 
{
    E_CLD_IASACE_CMD_ARM_RESP                   = 0x00,     /* Mandatory */
    E_CLD_IASACE_CMD_GET_ZONE_ID_MAP_RESP,                  /* Mandatory */
    E_CLD_IASACE_CMD_GET_ZONE_INFO_RESP,                    /* Mandatory */
    E_CLD_IASACE_CMD_ZONE_STATUS_CHANGED,                   /* Mandatory */
    E_CLD_IASACE_CMD_PANEL_STATUS_CHANGED,                  /* Mandatory */
    E_CLD_IASACE_CMD_GET_PANEL_STATUS_RESP,                 /* Mandatory */
    E_CLD_IASACE_CMD_SET_BYPASSED_ZONE_LIST,                /* Mandatory */
    E_CLD_IASACE_CMD_BYPASS_RESP,                           /* Mandatory */
    E_CLD_IASACE_CMD_GET_ZONE_STATUS_RESP                   /* Mandatory */
} teCLD_IASACE_ServerCmdId;

/* Client Command codes */
typedef enum 
{
    E_CLD_IASACE_CMD_ARM                        = 0x00,     /* Mandatory */
    E_CLD_IASACE_CMD_BYPASS,                                /* Mandatory */
    E_CLD_IASACE_CMD_EMERGENCY,                             /* Mandatory */
    E_CLD_IASACE_CMD_FIRE,                                  /* Mandatory */
    E_CLD_IASACE_CMD_PANIC,                                 /* Mandatory */
    E_CLD_IASACE_CMD_GET_ZONE_ID_MAP,                       /* Mandatory */
    E_CLD_IASACE_CMD_GET_ZONE_INFO,                         /* Mandatory */
    E_CLD_IASACE_CMD_GET_PANEL_STATUS,                      /* Mandatory */
    E_CLD_IASACE_CMD_GET_BYPASSED_ZONE_LIST,                /* Mandatory */
    E_CLD_IASACE_CMD_GET_ZONE_STATUS
} teCLD_IASACE_ClientCmdId;


/* Arm Mode Field */
typedef enum 
{
    E_CLD_IASACE_ARM_MODE_DISARM                                      = 0x00,
    E_CLD_IASACE_ARM_MODE_ARM_DAY_HOME_ZONES_ONLY,
    E_CLD_IASACE_ARM_MODE_ARM_NIGHT_SLEEP_ZONES_ONLY,
    E_CLD_IASACE_ARM_MODE_ARM_ALL_ZONES,
} teCLD_IASACE_ArmMode;

/* Arm Notification Field */
typedef enum 
{
    E_CLD_IASACE_ARM_NOTIF_ALL_ZONES_DISARMED                         = 0x00,
    E_CLD_IASACE_ARM_NOTIF_ONLY_DAY_HOME_ZONES_ARMED,
    E_CLD_IASACE_ARM_NOTIF_ONLY_NIGHT_SLEEP_ZONES_ARMED,
    E_CLD_IASACE_ARM_NOTIF_ALL_ZONES_ARMED,
    E_CLD_IASACE_ARM_NOTIF_INVALID_ARM_DISARM_CODE,
    E_CLD_IASACE_ARM_NOTIF_NOT_READY_TO_ARM,
    E_CLD_IASACE_ARM_NOTIF_ALREADY_DISARMED
} teCLD_IASACE_ArmNotification;


/* Audible Notification Field */
typedef enum 
{
    E_CLD_IASACE_AUDIBLE_NOTIF_MUTE                           = 0x00,
    E_CLD_IASACE_AUDIBLE_NOTIF_DEFAULT_SOUND
} teCLD_IASACE_AudibleNotification;

/* Panel Status */
typedef enum 
{
    E_CLD_IASACE_PANEL_STATUS_PANEL_DISARMED          = 0x00,
    E_CLD_IASACE_PANEL_STATUS_PANEL_ARMED_DAY,
    E_CLD_IASACE_PANEL_STATUS_PANEL_ARMED_NIGHT,
    E_CLD_IASACE_PANEL_STATUS_PANEL_ARMED_AWAY,
    E_CLD_IASACE_PANEL_STATUS_PANEL_EXIT_DELAY,
    E_CLD_IASACE_PANEL_STATUS_PANEL_ENTRY_DELAY,
    E_CLD_IASACE_PANEL_STATUS_PANEL_NOT_READY_TO_ARM,
    E_CLD_IASACE_PANEL_STATUS_PANEL_IN_ALARM,
    E_CLD_IASACE_PANEL_STATUS_PANEL_ARMING_STAY,
    E_CLD_IASACE_PANEL_STATUS_PANEL_ARMING_NIGHT,
    E_CLD_IASACE_PANEL_STATUS_PANEL_ARMING_AWAY
} teCLD_IASACE_PanelStatus;


/* Alarm Status */
typedef enum 
{
    E_CLD_IASACE_ALARM_STATUS_NO_ALARM        = 0x00,
    E_CLD_IASACE_ALARM_STATUS_BURGLAR,
    E_CLD_IASACE_ALARM_STATUS_FIRE,
    E_CLD_IASACE_ALARM_STATUS_EMERGENCY,
    E_CLD_IASACE_ALARM_STATUS_POLICE_PANIC,
    E_CLD_IASACE_ALARM_STATUS_FIRE_PANIC,
    E_CLD_IASACE_ALARM_STATUS_EMERGENCY_PANIC
} teCLD_IASACE_AlarmStatus;


/* Bypass Result */
typedef enum 
{
    E_CLD_IASACE_BYPASS_RESULT_ZONE_BYPASSED          = 0x00,
    E_CLD_IASACE_BYPASS_RESULT_ZONE_NOT_BYPASSED,
    E_CLD_IASACE_BYPASS_RESULT_NOT_ALLOWED,
    E_CLD_IASACE_BYPASS_RESULT_INVALID_ZONE_ID,
    E_CLD_IASACE_BYPASS_RESULT_UNKNOWN_ZONE_ID,
    E_CLD_IASACE_BYPASS_RESULT_INVALID_ARM_DISARM_CODE
} teCLD_IASACE_BypassResult;


/* Panel Parameter ID */
typedef enum 
{
    E_CLD_IASACE_PANEL_PARAMETER_PANEL_STATUS          = 0x00,
    E_CLD_IASACE_PANEL_PARAMETER_SECONDS_REMAINING,
    E_CLD_IASACE_PANEL_PARAMETER_AUDIBLE_NOTIFICATION,
    E_CLD_IASACE_PANEL_PARAMETER_ALARM_STATUS
} teCLD_IASACE_PanelParameterID;


/* Zone Parameter ID */
typedef enum 
{
    E_CLD_IASACE_ZONE_PARAMETER_ZONE_CONFIG_FLAG          = 0x00,
    E_CLD_IASACE_ZONE_PARAMETER_ZONE_STATUS_FLAG,
    E_CLD_IASACE_ZONE_PARAMETER_ZONE_STATUS,
    E_CLD_IASACE_ZONE_PARAMETER_AUDIBLE_NOTIFICATION,
    E_CLD_IASACE_ZONE_PARAMETER_ZONE_LABEL,
    E_CLD_IASACE_ZONE_PARAMETER_ARM_DISARM_CODE
} teCLD_IASACE_ZoneParameterID;

/*IAS ACE Cluster */
typedef struct
{
    zbmap32     u32FeatureMap;
  
    zuint16     u16ClusterRevision;    
} tsCLD_IASACE; 

/* Definition of Command Payload Structure */

/* ARM Payload */
typedef struct
{
    zenum8                              eArmMode;   
    tsZCL_CharacterString               sArmDisarmCode;
    zuint8                              u8ZoneID;
} tsCLD_IASACE_ArmPayload;


/* Bypass Payload */
typedef struct
{   
    zuint8                              u8NumOfZones;
    zuint8                              *pu8ZoneID;
    tsZCL_CharacterString               sArmDisarmCode;
} tsCLD_IASACE_BypassPayload;


/* Get Zone Information Payload */
typedef struct
{
    zuint8                              u8ZoneID;
} tsCLD_IASACE_GetZoneInfoPayload;


/* Get Zone Status Payload */
typedef struct
{
    zuint8                              u8StartingZoneID;
    zuint8                              u8MaxNumOfZoneID;
    zbool                               bZoneStatusMaskFlag;
    zbmap16                             u16ZoneStatusMask;
} tsCLD_IASACE_GetZoneStatusPayload;

/* ARM Response Payload */
typedef struct
{
    zenum8                              eArmNotification;   
} tsCLD_IASACE_ArmRespPayload;

/* Get Zone ID Map Response Payload */
typedef struct
{
    zbmap16                             au16ZoneIDMap[CLD_IASACE_MAX_BYTES_FOR_NUM_OF_ZONES]; 
} tsCLD_IASACE_GetZoneIDMapRespPayload;

/* Get Zone Information Response Payload */
typedef struct
{
    zuint8                              u8ZoneID;
    zbmap16                             u16ZoneType;
    zieeeaddress                        u64IeeeAddress;
    tsZCL_CharacterString               sZoneLabel; 
} tsCLD_IASACE_GetZoneInfoRespPayload;

/* Zone Status Changed Payload */
typedef struct
{
    zuint8                              u8ZoneID;
    zenum16                             eZoneStatus;
    zenum8                              eAudibleNotification;
    tsZCL_CharacterString               sZoneLabel;
} tsCLD_IASACE_ZoneStatusChangedPayload;

/* Panel Status Changed Payload */
typedef struct
{
    zenum8                              ePanelStatus;
    zuint8                              u8SecondsRemaining;
    zenum8                              eAudibleNotification;
    zenum8                              eAlarmStatus;
} tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload;


/* Set Bypassed Zone List Payload */
typedef struct
{
    zuint8                              u8NumofZones;
    zuint8                              *pu8ZoneID;         
} tsCLD_IASACE_SetBypassedZoneListPayload;

/* Bypass Response Payload */
typedef struct
{
    zuint8                              u8NumofZones;
    zuint8                              *pu8BypassResult;           
} tsCLD_IASACE_BypassRespPayload;


/* Get Zone Status Response Payload */
typedef struct
{
    zbool                               bZoneStatusComplete;
    zuint8                              u8NumofZones;
    zuint8                              *pu8ZoneStatus;         
} tsCLD_IASACE_GetZoneStatusRespPayload;


/* Definition of IASACE Call back Event Structure */
typedef struct
{
    uint8                                                                               u8CommandId;
    union
    {
        tsCLD_IASACE_ArmPayload                                                         *psArmPayload;
        tsCLD_IASACE_BypassPayload                                                      *psBypassPayload;
        tsCLD_IASACE_GetZoneInfoPayload                                                 *psGetZoneInfoPayload;
        tsCLD_IASACE_GetZoneStatusPayload                                               *psGetZoneStatusPayload;
        tsCLD_IASACE_ArmRespPayload                                                     *psArmRespPayload;
        tsCLD_IASACE_GetZoneIDMapRespPayload                                            *psGetZoneIDMapRespPayload;
        tsCLD_IASACE_GetZoneInfoRespPayload                                             *psGetZoneInfoRespPayload;
        tsCLD_IASACE_ZoneStatusChangedPayload                                           *psZoneStatusChangedPayload;
        tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload                      *psPanelStatusChangedOrGetPanelStatusRespPayload;
        tsCLD_IASACE_SetBypassedZoneListPayload                                         *psSetBypassedZoneListPayload;
        tsCLD_IASACE_BypassRespPayload                                                  *psBypassRespPayload;
        tsCLD_IASACE_GetZoneStatusRespPayload                                           *psGetZoneStatusRespPayload;
    } uMessage;
} tsCLD_IASACECallBackMessage;


typedef struct
{
    zuint8              u8ZoneID;
    zbmap16             u16ZoneType;
    zieeeaddress        u64IeeeAddress;
} tsCLD_IASACE_ZoneTable;

typedef struct
{
    zenum8                                  ePanelStatus;
    zuint8                                  u8SecondsRemaining;
    zenum8                                  eAudibleNotification;
    zenum8                                  eAlarmStatus;
}tsCLD_IASACE_PanelParameter;

typedef struct
{
    zbmap8                                  u8ZoneConfigFlag;
    zbmap8                                  u8ZoneStatusFlag;
    zbmap16                                 eZoneStatus;
    zenum8                                  eAudibleNotification;
    tsZCL_CharacterString                   sZoneLabel;
    uint8                                   au8ZoneLabel[CLD_IASACE_MAX_LENGTH_ZONE_LABEL];
    tsZCL_CharacterString                   sArmDisarmCode;
    uint8                                   au8ArmDisarmCode[CLD_IASACE_MAX_LENGTH_ARM_DISARM_CODE];
}tsCLD_IASACE_ZoneParameter;

/* Custom data structure */
typedef struct
{
    tsZCL_ReceiveEventAddress               sReceiveEventAddress;
    tsZCL_CallBackEvent                     sCustomCallBackEvent;
    tsCLD_IASACECallBackMessage             sCallBackMessage;
#if (defined CLD_IASACE) && (defined IASACE_SERVER)
    tsCLD_IASACE_PanelParameter             sCLD_IASACE_PanelParameter;
    tsCLD_IASACE_ZoneParameter              asCLD_IASACE_ZoneParameter[CLD_IASACE_ZONE_TABLE_SIZE];
    tsCLD_IASACE_ZoneTable                  asCLD_IASACE_ZoneTable[CLD_IASACE_ZONE_TABLE_SIZE];
#endif
} tsCLD_IASACECustomDataStructure;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_IASACECreateIASACE(
                tsZCL_ClusterInstance                       *psClusterInstance,
                bool_t                                      bIsServer,
                tsZCL_ClusterDefinition                     *psClusterDefinition,
                void                                        *pvEndPointSharedStructPtr,
                uint8                                       *pu8AttributeControlBits,
                tsCLD_IASACECustomDataStructure             *psCustomDataStructure);

#ifdef IASACE_SERVER
PUBLIC teZCL_CommandStatus eCLD_IASACEAddZoneEntry (
                uint8                                       u8SourceEndPointId,
                uint16                                      u16ZoneType,
                uint64                                      u64IeeeAddress,
                uint8                                       *pu8ZoneID);
 
PUBLIC teZCL_CommandStatus eCLD_IASACERemoveZoneEntry (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8ZoneID,
                uint64                                      *pu64IeeeAddress);

PUBLIC teZCL_CommandStatus eCLD_IASACEGetZoneTableEntry (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8ZoneID,
                tsCLD_IASACE_ZoneTable                      **ppsZoneTable);

PUBLIC teZCL_CommandStatus eCLD_IASACEGetEnrolledZones (
                uint8                                       u8SourceEndPointId,
                uint8                                       *pu8ZoneID,
                uint8                                       *pu8NumOfEnrolledZones);
                
PUBLIC teZCL_Status eCLD_IASACESetPanelParameter (
                uint8                                       u8SourceEndPointId,
                teCLD_IASACE_PanelParameterID               eParameterId,
                uint8                                       u8ParameterValue);
                
PUBLIC teZCL_Status eCLD_IASACEGetPanelParameter (
                uint8                                       u8SourceEndPointId,
                teCLD_IASACE_PanelParameterID               eParameterId,
                uint8                                       *pu8ParameterValue);                
                
PUBLIC teZCL_Status eCLD_IASACESetZoneParameter (
                uint8                                       u8SourceEndPointId,
                teCLD_IASACE_ZoneParameterID                eParameterId,
                uint8                                       u8ZoneID,
                uint8                                       u8ParameterLength,
                uint8                                       *pu8ParameterValue);
   
PUBLIC teZCL_Status eCLD_IASACESetZoneParameterValue (
            uint8                                       u8SourceEndPointId,
            teCLD_IASACE_ZoneParameterID                eParameterId,
            uint8                                       u8ZoneID,
            uint16                                      u16ParameterValue);
            
PUBLIC teZCL_Status eCLD_IASACEGetZoneParameter (
                uint8                                       u8SourceEndPointId,
                teCLD_IASACE_ZoneParameterID                eParameterId,
                uint8                                       u8ZoneID,
                uint8                                       *pu8ParameterLength,
                uint8                                       *pu8ParameterValue);                

PUBLIC teZCL_Status eCLD_IASACE_ZoneStatusChangedSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_ZoneStatusChangedPayload       *psPayload);

PUBLIC INLINE teZCL_Status eCLD_IASACE_PanelStatusChangedSend (
                uint8                                                       u8SourceEndPointId,
                uint8                                                       u8DestinationEndPointId,
                tsZCL_Address                                               *psDestinationAddress,
                uint8                                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload  *psPayload) ALWAYS_INLINE;
                
PUBLIC teZCL_Status eCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespSend (
                uint8                                                       u8SourceEndPointId,
                uint8                                                       u8DestinationEndPointId,
                tsZCL_Address                                               *psDestinationAddress,
                uint8                                                       *pu8TransactionSequenceNumber,
                teCLD_IASACE_ServerCmdId                                    eCommandId,
                tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload  *psPayload);
                
PUBLIC INLINE teZCL_Status eCLD_IASACE_PanelStatusChangedSend (
                uint8                                                       u8SourceEndPointId,
                uint8                                                       u8DestinationEndPointId,
                tsZCL_Address                                               *psDestinationAddress,
                uint8                                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload  *psPayload)
{
    // handle sequence number get a new sequence number
    if(pu8TransactionSequenceNumber != NULL)
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
    
    return eCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                                  E_CLD_IASACE_CMD_PANEL_STATUS_CHANGED,
                                                              psPayload);
}
PUBLIC teZCL_Status eCLD_IASACE_SetBypassedZoneListSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_SetBypassedZoneListPayload     *psPayload);                
#endif       

#ifdef IASACE_CLIENT
PUBLIC teZCL_Status eCLD_IASACE_ArmSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_ArmPayload                     *psPayload);
    
PUBLIC teZCL_Status eCLD_IASACE_BypassSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_BypassPayload                  *psPayload);

PUBLIC teZCL_Status eCLD_IASACE_EmergencySend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber);

PUBLIC teZCL_Status eCLD_IASACE_FireSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber);

PUBLIC teZCL_Status eCLD_IASACE_PanicSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber);

PUBLIC teZCL_Status eCLD_IASACE_GetZoneIDMapSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber);

PUBLIC teZCL_Status eCLD_IASACE_GetZoneInfoSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_GetZoneInfoPayload             *psPayload);

PUBLIC teZCL_Status eCLD_IASACE_GetPanelStatusSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber);

PUBLIC teZCL_Status eCLD_IASACE_GetBypassedZoneListSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber);

PUBLIC teZCL_Status eCLD_IASACE_GetZoneStatusSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_GetZoneStatusPayload           *psPayload);
                            
#endif       
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern const tsZCL_AttributeDefinition asCLD_IASACEClusterAttributeDefinitions[];
extern tsZCL_ClusterDefinition sCLD_IASACE;
extern uint8 au8IASACEAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* IASACE_H */
