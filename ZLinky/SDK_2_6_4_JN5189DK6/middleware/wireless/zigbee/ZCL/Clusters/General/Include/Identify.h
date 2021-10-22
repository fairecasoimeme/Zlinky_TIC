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
 * MODULE:             Identify Cluster
 *
 * COMPONENT:          Identify.h
 *
 * DESCRIPTION:        Header for Identify Cluster
 *
 *****************************************************************************/

#ifndef IDENTIFY_H
#define IDENTIFY_H

#include <jendefs.h>
#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */
#define GENERAL_CLUSTER_ID_IDENTIFY                 0x0003


#define CLD_IDENTIFY_NUMBER_OF_MANDATORY_ATTRIBUTE  1 //u16IdentifyTime


#define CLD_IDENTIFY_NUMBER_OF_OPTIONAL_ATTRIBUTE   1

#define CLD_IDENTIFY_MAX_NUMBER_OF_ATTRIBUTE       \
    CLD_IDENTIFY_NUMBER_OF_OPTIONAL_ATTRIBUTE +    \
    CLD_IDENTIFY_NUMBER_OF_MANDATORY_ATTRIBUTE

/****************************************************************************/
/*             Identify Cluster - Optional Attributes                */
/*                                                                           */
/* Add the following #define's to your zcl_options.h file to add optional    */
/* attributes to the identify cluster.                                       */
/****************************************************************************/

/* Commission State attribute */
//#define CLD_IDENTIFY_ATTR_COMMISSION_STATE

/* EZ Mode Invoke Command */
//#define CLD_IDENTIFY_CMD_EZ_MODE_INVOKE

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Commission State - bit definitions and bit masks */
#define IDENTIFY_COMMISSION_STATE_NETWORK       (1 << 0)
#define IDENTIFY_COMMISSION_STATE_OPERATIONAL   (1 << 1)

/* EZMODE INVOKE  - bit definitions and bit masks */
#define IDENTIFY_CMD_EZMODE_INVOKE_ACTION_FACTOTY_FRESH             (1 << 0)
#define IDENTIFY_CMD_EZMODE_INVOKE_ACTION_NETWORK_STEERING          (1 << 1)
#define IDENTIFY_CMD_EZMODE_INVOKE_ACTION_FINDING_AND_BINDING       (1 << 2)

#ifndef CLD_IDENTIFY_CLUSTER_REVISION
    #define CLD_IDENTIFY_CLUSTER_REVISION                         2
#endif 

#ifndef CLD_IDENTIFY_FEATURE_MAP
    #define CLD_IDENTIFY_FEATURE_MAP                              0
#endif       
      

/* Command codes */
typedef enum 
{
    E_CLD_IDENTIFY_CMD_IDENTIFY                 = 0x00,    /* Mandatory */
    E_CLD_IDENTIFY_CMD_IDENTIFY_QUERY,                     /* Mandatory */
    E_CLD_IDENTIFY_CMD_EZ_MODE_INVOKE,
    E_CLD_IDENTIFY_CMD_UPDATE_COMMISSION_STATE,
    E_CLD_IDENTIFY_CMD_TRIGGER_EFFECT =             0x40   /* Optional*/
} teCLD_Identify_Command;

typedef enum 
{
    E_CLD_IDENTIFY_EFFECT_BLINK             = 0x00,
    E_CLD_IDENTIFY_EFFECT_BREATHE,
    E_CLD_IDENTIFY_EFFECT_OKAY,
    E_CLD_IDENTIFY_EFFECT_CHANNEL_CHANGE    = 0x0b,
    E_CLD_IDENTIFY_EFFECT_FINISH_EFFECT     = 0xfe,
    E_CLD_IDENTIFY_EFFECT_STOP_EFFECT       = 0xff
} teCLD_Identify_EffectId;

/* Attribute ID's */
typedef enum 
{
    /* Identify attribute set attribute ID's (3.5.2.2) */
    E_CLD_IDENTIFY_ATTR_ID_IDENTIFY_TIME     = 0x0000,   /* Mandatory */
    E_CLD_IDENTIFY_ATTR_ID_COMMISSION_STATE,             /* Optional */
} teCLD_Identify_ClusterID;


/* Identify Cluster */
typedef struct
{
#ifdef IDENTIFY_SERVER    
    zuint16                 u16IdentifyTime;
#ifdef CLD_IDENTIFY_ATTR_COMMISSION_STATE
    zbmap8                  u8CommissionState;
#endif  
#endif
    zbmap32                 u32FeatureMap;

    zuint16                 u16ClusterRevision;
} tsCLD_Identify;


/* Identify request command payload */
typedef struct
{
    zuint16                 u16IdentifyTime;
} tsCLD_Identify_IdentifyRequestPayload;


/* Identify query response command payload */
typedef struct
{
    zuint16                 u16Timeout;
} tsCLD_Identify_IdentifyQueryResponsePayload;

/* trigger effect command payload */
typedef struct
{
    teCLD_Identify_EffectId     eEffectId;
    zuint8                      u8EffectVarient;
} tsCLD_Identify_TriggerEffectRequestPayload;

#ifdef CLD_IDENTIFY_CMD_EZ_MODE_INVOKE
/* Identify EZ mode Invoke command payload */
typedef struct
{
    zbmap8                u8Action;
} tsCLD_Identify_EZModeInvokePayload;
#endif

#ifdef CLD_IDENTIFY_ATTR_COMMISSION_STATE
/* Identify Update Commission State command payload */
typedef struct
{
    zenum8                u8Action;
    zbmap8                u8CommissionStateMask;
} tsCLD_Identify_UpdateCommissionStatePayload;
#endif

/* Definition of Identify Call back Event Structure */
typedef struct
{
    uint8                                       u8CommandId;
    union
    {
        tsCLD_Identify_IdentifyRequestPayload           *psIdentifyRequestPayload;
        tsCLD_Identify_IdentifyQueryResponsePayload     *psIdentifyQueryResponsePayload;
        tsCLD_Identify_TriggerEffectRequestPayload      *psTriggerEffectRequestPayload;
#ifdef CLD_IDENTIFY_CMD_EZ_MODE_INVOKE      
        tsCLD_Identify_EZModeInvokePayload              *psEZModeInvokePayload;
#endif
#ifdef CLD_IDENTIFY_ATTR_COMMISSION_STATE
        tsCLD_Identify_UpdateCommissionStatePayload     *psUpdateCommissionStatePayload;
#endif      
    } uMessage;
} tsCLD_IdentifyCallBackMessage;


/* Custom data structure */
typedef struct
{
    tsZCL_ReceiveEventAddress         sReceiveEventAddress;
    tsZCL_CallBackEvent               sCustomCallBackEvent;
    tsCLD_IdentifyCallBackMessage     sCallBackMessage;
} tsCLD_IdentifyCustomDataStructure;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_IdentifyCreateIdentify(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits,
                tsCLD_IdentifyCustomDataStructure  *psCustomDataStructure);

PUBLIC teZCL_Status eCLD_IdentifyRegisterTimeServer(void);

#ifdef IDENTIFY_CLIENT
PUBLIC teZCL_Status eCLD_IdentifyCommandIdentifyRequestSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                tsCLD_Identify_IdentifyRequestPayload *psPayload);

PUBLIC teZCL_Status eCLD_IdentifyCommandIdentifyQueryRequestSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber);
#ifdef CLD_IDENTIFY_ATTR_COMMISSION_STATE
PUBLIC teZCL_Status eCLD_IdentifyUpdateCommissionStateCommandSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                tsCLD_Identify_UpdateCommissionStatePayload *psPayload);
                
#endif              
                
#ifdef  CLD_IDENTIFY_CMD_TRIGGER_EFFECT
PUBLIC teZCL_Status eCLD_IdentifyCommandTriggerEffectSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                teCLD_Identify_EffectId     eEffectId,
                uint8   u8EffectVarient);
#endif
#endif /*IDENTIFY_CLIENT*/      
          
#ifdef CLD_IDENTIFY_CMD_EZ_MODE_INVOKE
PUBLIC teZCL_Status eCLD_IdentifyEZModeInvokeCommandSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                bool_t            bDirection,
                tsCLD_Identify_EZModeInvokePayload *psPayload);
                
#endif              

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern tsZCL_ClusterDefinition sCLD_Identify;
extern const tsZCL_AttributeDefinition asCLD_IdentifyClusterAttributeDefinitions[];
extern uint8 au8IdentifyAttributeControlBits[];
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* IDENTIFY_H */
