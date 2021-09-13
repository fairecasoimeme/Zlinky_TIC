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
 * MODULE:             IAS WD Cluster
 *
 * COMPONENT:          IASWD.h
 *
 * DESCRIPTION:        Header for IAS WD Cluster
 *
 *****************************************************************************/

#ifndef IAS_WD_H
#define IAS_WD_H

#include <jendefs.h>
#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */
#define SECURITY_AND_SAFETY_CLUSTER_ID_IASWD          0x0502

#ifndef CLD_IASWD_CLUSTER_REVISION
    #define CLD_IASWD_CLUSTER_REVISION               1
#endif

#ifndef CLD_IASWD_FEATURE_MAP
    #define CLD_IASWD_FEATURE_MAP                    0
#endif
/****************************************************************************/
/*             IAS WD Cluster - Optional Attributes                */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the IAS WD cluster.                               */
/****************************************************************************/


/* End of Optional attribute dependencies */

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Attribute ID's */
typedef enum 
{
    E_CLD_IASWD_ATTR_ID_MAX_DURATION        = 0x0000,   /* Mandatory*/
} teCLD_IASWD_AttributeID;

typedef enum 
{
    E_CLD_IASWD_WARNING_STOP=0,
    E_CLD_IASWD_WARNING_BURGLAR,
    E_CLD_IASWD_WARNING_FIRE,
    E_CLD_IASWD_WARNING_EMERGENCY
}teCLD_IASWD_WarningMode;

typedef enum 
{
    E_CLD_IASWD_NO_STROBE_WITH_WARNING=0,
    E_CLD_IASWD_STROBE_WITH_WARNING
}teCLD_IASWD_WarningStrobe;

typedef enum 
{
    E_CLD_IASWD_SIREN_LOW_SOUND=0,
    E_CLD_IASWD_SIREN_MEDIUM_SOUND,
    E_CLD_IASWD_SIREN_HIGH_SOUND,
    E_CLD_IASWD_SIREN_VERY_HIGH_SOUND
}teCLD_IASWD_SirenLevel;

typedef enum 
{
    E_CLD_IASWD_STROBE_LOW=0,
    E_CLD_IASWD_STROBE_MEDIUM,
    E_CLD_IASWD_STROBE_HIGH,
    E_CLD_IASWD_STROBE_VERY_HIGH
}teCLD_IASWD_StrobeLevel;

typedef enum 
{
    E_CLD_IASWD_SQUAWK_SYSTEM_ARMED=0,
    E_CLD_IASWD_SQUAWK_SYSTEM_DISARMED
}teCLD_IASWD_SquawkMode;

typedef enum 
{
    E_CLD_IASWD_NO_STROBE_WITH_SQUAWK=0,
    E_CLD_IASWD_STROBE_WITH_SQUAWK
}teCLD_IASWD_SquawkStrobe;

typedef enum 
{
    E_CLD_IASWD_SQUAWK_LOW_SOUND=0,
    E_CLD_IASWD_SQUAWK_MEDIUM_SOUND,
    E_CLD_IASWD_SQUAWK_HIGH_SOUND,
    E_CLD_IASWD_SQUAWK_VERY_HIGH_SOUND
}teCLD_IASWD_SquawkLevel;

typedef enum 
{
    E_CLD_IASWD_CLUSTER_UPDATE_STROBE =0,
    E_CLD_IASWD_CLUSTER_UPDATE_WARNING
}teCLD_IASWD_ClusterUpdate;

/* Server Received command Id*/
typedef enum 
{
    E_CLD_IASWD_CMD_START_WARNING = 0x00,              /* Mandatory */
    E_CLD_IASWD_CMD_SQUAWK
} teCLD_IASWD_ServerReceivedCommandID;

/*IAS WD Cluster */
typedef struct
{
#ifdef IASWD_SERVER   
    zuint16     u16MaxDuration;  
#endif    
    zbmap32     u32FeatureMap;
    
    zuint16     u16ClusterRevision;    
} tsCLD_IASWD; 

typedef struct
{
    uint8       u8WarningModeStrobeAndSirenLevel;
    uint16      u16WarningDuration;
    uint8       u8StrobeDutyCycle;
    zenum8      eStrobeLevel;
}tsCLD_IASWD_StartWarningReqPayload;

typedef struct
{
    uint8                         u8SquawkModeStrobeAndLevel ;  
}tsCLD_IASWD_SquawkReqPayload;

typedef struct
{
    bool_t      bStrobe;
    uint8       u8StrobeDutyCycle;
    zenum8      eStrobeLevel;
}tsCLD_IASWD_StrobeUpdate;

typedef struct
{
    uint8       u8WarningMode;
    uint8       u8SirenLevel;
    uint16      u16WarningDurationRemaining;

    uint8       u8StrobeWithWarning;
    bool_t      bStrobe;
    zenum8      eStrobeLevel;
}tsCLD_IASWD_WarningUpdate;

/* Definition of Call back Event Structure */
typedef struct
{
    uint8                           u8CommandId;
    union
    {
        tsCLD_IASWD_StartWarningReqPayload    *psWDStartWarningReqPayload;
        tsCLD_IASWD_SquawkReqPayload          *psWDSquawkReqPayload;
        tsCLD_IASWD_StrobeUpdate              *psStrobeUpdate;
        tsCLD_IASWD_WarningUpdate             *psWarningUpdate;

    } uMessage;
} tsCLD_IASWDCallBackMessage;

/* Custom data structure */
typedef struct
{
    tsCLD_IASWD_SquawkReqPayload          sSquawk;
    tsCLD_IASWD_StartWarningReqPayload    sWarning;
    uint32                                u32WarningDurationRemainingIn100MS;
    
    tsZCL_ReceiveEventAddress     sReceiveEventAddress;
    tsZCL_CallBackEvent           sCustomCallBackEvent;
    tsCLD_IASWDCallBackMessage    sCallBackMessage;   
} tsCLD_IASWD_CustomDataStructure;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_IASWDCreateIASWD (
                                tsZCL_ClusterInstance           *psClusterInstance,
                                bool_t                          bIsServer,
                                tsZCL_ClusterDefinition         *psClusterDefinition,
                                void                            *pvEndPointSharedStructPtr,
                                uint8                           *pu8AttributeControlBits,
                                tsCLD_IASWD_CustomDataStructure *psCustomDataStructure);
#ifdef IASWD_SERVER
PUBLIC teZCL_Status eCLD_IASWDUpdate (
                                uint8                             u8SourceEndPoint);
#endif

#ifdef IASWD_CLIENT
PUBLIC teZCL_Status eCLD_IASWDStartWarningReqSend (
                                uint8                              u8SourceEndPointId,
                                uint8                              u8DestinationEndPointId,
                                tsZCL_Address                      *psDestinationAddress,
                                uint8                              *pu8TransactionSequenceNumber,
                                tsCLD_IASWD_StartWarningReqPayload *psPayload);
PUBLIC teZCL_Status eCLD_IASWDSquawkReqSend (
                                uint8                              u8SourceEndPointId,
                                uint8                              u8DestinationEndPointId,
                                tsZCL_Address                      *psDestinationAddress,
                                uint8                              *pu8TransactionSequenceNumber,
                                tsCLD_IASWD_SquawkReqPayload       *psPayload); 
#endif                                
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern tsZCL_ClusterDefinition sCLD_IASWD;
extern const tsZCL_AttributeDefinition asCLD_IASWDClusterAttributeDefinitions[];
extern uint8 au8IASWDAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* IAS_WD_H */
