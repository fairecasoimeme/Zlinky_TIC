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
 * MODULE:             Commissioning Cluster
 *
 * COMPONENT:          Commissioning.h
 *
 * DESCRIPTION:        Header for Commissioning Cluster
 *
 *****************************************************************************/

#ifndef COMMISSIONING_H
#define COMMISSIONING_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/


/* Cluster ID's */
#define COMMISSIONING_CLUSTER_ID_COMMISSIONING                         0x0015

#ifndef CLD_COMMISSIONING_NUM_OF_ATTR_SETS
    #define CLD_COMMISSIONING_NUM_OF_ATTR_SETS                         5
#endif

#ifndef CLD_COMMISSIONING_CLUSTER_REVISION
    #define CLD_COMMISSIONING_CLUSTER_REVISION                         1
#endif 

#ifndef CLD_COMMISSIONING_FEATURE_MAP
    #define CLD_COMMISSIONING_FEATURE_MAP                              0
#endif 

#if ((defined(CLD_COMMISSIONING_CMD_SAVE_STARTUP_PARAMS) && !defined(CLD_COMMISSIONING_CMD_RESTORE_STARTUP_PARAMS)) ||  (!defined(CLD_COMMISSIONING_CMD_SAVE_STARTUP_PARAMS) && defined(CLD_COMMISSIONING_CMD_RESTORE_STARTUP_PARAMS)))
#error A device that implements the optional Save Startup Parameters command shall also implement the Restore Startup Parameters Request command (and vice-versa).
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* Command codes */
typedef enum 
{
    E_CLD_COMMISSIONING_CMD_RESTART_DEVICE                 = 0x00,    /* Mandatory */
    E_CLD_COMMISSIONING_CMD_SAVE_STARTUP_PARAMS,                      /* Optional */
    E_CLD_COMMISSIONING_CMD_RESTORE_STARTUP_PARAMS,                   /* Optional */
    E_CLD_COMMISSIONING_CMD_RESET_STARTUP_PARAMS                      /* Mandatory */
} teCLD_Commissioning_Command;

typedef enum 
{
    /* Commissioning attribute set attribute ID's (3.15.2.2) */
    E_CLD_COMMISSIONING_ATTR_ID_SHORT_ADDRESS   = 0x0000,
    E_CLD_COMMISSIONING_ATTR_ID_EXT_PANID,
    E_CLD_COMMISSIONING_ATTR_ID_PANID,
    E_CLD_COMMISSIONING_ATTR_ID_CHANNEL_MASK,
    E_CLD_COMMISSIONING_ATTR_ID_PROTOCOL_VERSION,
    E_CLD_COMMISSIONING_ATTR_ID_STACK_PROFILE,
    E_CLD_COMMISSIONING_ATTR_ID_STARTUP_CONTROl,
    E_CLD_COMMISSIONING_ATTR_ID_TC_ADDR = 0x0010,
    E_CLD_COMMISSIONING_ATTR_ID_TC_MASTER_KEY,
    E_CLD_COMMISSIONING_ATTR_ID_NETWORK_KEY,
    E_CLD_COMMISSIONING_ATTR_ID_USE_INSECURE_JOIN,
    E_CLD_COMMISSIONING_ATTR_ID_PRECONFIG_LINK_KEY,
    E_CLD_COMMISSIONING_ATTR_ID_NWK_KEY_SEQ_NO,
    E_CLD_COMMISSIONING_ATTR_ID_NWK_KEY_TYPE,
    E_CLD_COMMISSIONING_ATTR_ID_NWK_MANAGER_ADDR,
    /* Join Parameters attribute set attribute ID's (3.15.2.2.2)*/
    E_CLD_COMMISSIONING_ATTR_ID_SCAN_ATTEMPTS = 0x0020,
    E_CLD_COMMISSIONING_ATTR_ID_TIME_BW_SCANS,
    E_CLD_COMMISSIONING_ATTR_ID_REJOIN_INTERVAL,
    E_CLD_COMMISSIONING_ATTR_ID_MAX_REJOIN_INTERVAL,
    /* End Device Parameters attribute set attribute ID's (3.15.2.2.3)*/
    E_CLD_COMMISSIONING_ATTR_ID_INDIRECT_POLL_RATE = 0x0030,
    E_CLD_COMMISSIONING_ATTR_ID_PARENT_RETRY_THRSHOLD,
    /* Concentrator Parameters attribute set attribute ID's (3.15.2.2.4)*/
    E_CLD_COMMISSIONING_ATTR_ID_CONCENTRATOR_FLAG = 0x0040,
    E_CLD_COMMISSIONING_ATTR_ID_CONCENTRATOR_RADIUS,
    E_CLD_COMMISSIONING_ATTR_ID_CONCENTRATOR_DISCVRY_TIME,
} teCLD_Commissioning_AttributeID;

#ifdef COMMISSIONING_SERVER
/* Types of attribute set */
typedef enum 
{
    E_CLD_COMMISSIONING_ATTR_SET_STARTUP_PARAMS                 = 0x00,    
    E_CLD_COMMISSIONING_ATTR_SET_JOIN_PARAMS,                      
    E_CLD_COMMISSIONING_ATTR_SET_ENDDEVICE_PARAMS,                   
    E_CLD_COMMISSIONING_ATTR_SET_CONCENTRATOR_PARAMS                     
} teCLD_Commissioning_AttributeSet;

/* Commissioning StartUp Parameters attribute set (13.2.2.2.1) */ 
typedef struct
{
#ifdef     CLD_COMMISSIONING_ATTR_SHORT_ADDRESS
    uint16              u16ShortAddress;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_EXTENED_PAN_ID
    zieeeaddress        u64ExtPanId;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_PAN_ID
    uint16              u16PANId;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_CHANNEL_MASK
    zbmap32               u32ChannelMask;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_PROTOCOL_VERSION
    uint8               u8ProtocolVersion;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_STACK_PROFILE
    uint8               u8StackProfile;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_START_UP_CONTROL
    zenum8               e8StartUpControl;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_TC_ADDR
    zieeeaddress         u64TcAddr;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_TC_MASTER_KEY
    tsZCL_Key         sTcMasterKey;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_NWK_KEY
    tsZCL_Key         sNwkKey;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_USE_INSECURE_JOIN
    bool_t         bUseInsecureJoin;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_PRE_CONFIG_LINK_KEY
    tsZCL_Key             sPreConfigLinkKey;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_NWK_KEY_SEQ_NO
    uint8         u8NwkKeySeqNo;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_NWK_KEY_TYPE
    zenum8         e8NwkKeyType;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_NWK_MANAGER_ADDR
    uint16         u16NwkManagerAddr;
#endif
} tsCLD_StartupParameters;

/* Join Parameters attribute set (13.2.2.2.2)*/
typedef struct
{
#ifdef     CLD_COMMISSIONING_ATTR_SCAN_ATTEMPTS
    uint8         u8ScanAttempts;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_TIME_BW_SCANS
    uint16         u16TimeBwScans;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_REJOIN_INTERVAL
    uint16         u16RejoinInterval;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_MAX_REJOIN_INTERVAL
    uint16         u16MaxRejoinInterval;
#endif
} tsCLD_JoinParameters;


/* End Device Parameters attribute set (13.2.2.2.3)*/
typedef struct
{
#ifdef     CLD_COMMISSIONING_ATTR_INDIRECT_POLL_RATE
    uint16         u16IndirectPollRate;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_PARENT_RETRY_THRSHLD
    uint8         u8ParentRetryThreshold;
#endif
} tsCLD_EndDeviceParameters;

/* Concentrator Parameters attribute set (13.2.2.2.4)*/
typedef struct
{
#ifdef     CLD_COMMISSIONING_ATTR_CONCENTRATOR_FLAG
    bool_t         bConcentratorFlag;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_CONCENTRATOR_RADIUS
    uint8         u8ConcentratorRadius;
#endif

#ifdef     CLD_COMMISSIONING_ATTR_CONCENTRATOR_DISCVRY_TIME
    uint8         u8ConcentratorDiscoveryTime;
#endif
} tsCLD_ConcentratorParameters;
#endif /* COMMISSIONING_SERVER */

/* Commissioning Cluster */
typedef struct
{
#ifdef COMMISSIONING_SERVER    
    /* Commissioning attribute set (13.2.2.2.1) */
    tsCLD_StartupParameters         sStartupParameters;
    /* Join Parameters attribute set (13.2.2.2.2)*/
    tsCLD_JoinParameters            sJoinParameters;
    /* End Device Parameters attribute set (13.2.2.2.3)*/
    tsCLD_EndDeviceParameters       sEndDeviceParameters;   
    /* Concentrator Parameters attribute set (13.2.2.2.4)*/
    tsCLD_ConcentratorParameters    sConcentratorParameters;
#endif
    zbmap32       u32FeatureMap;
    zuint16       u16ClusterRevision;
} tsCLD_Commissioning;

/* Restart Device command payload */
typedef struct
{
    zbmap8                  u8Options;
    uint8                   u8Delay;
    uint8                   u8Jitter;
} tsCLD_Commissioning_RestartDevicePayload;

/* Save Startup Parameters command payload */
/* Restore Startup Parameters command payload */
/* Reset Startup Parameters command payload */
typedef struct
{
    zbmap8                  u8Options;
    uint8                   u8Index;
} tsCLD_Commissioning_ModifyStartupParametersPayload;

/* Restart Device Response payload */
/* Save Startup Parameters Response payload */
/* Restore Startup Parameters Response payload */
/* Reset Startup Parameters Response payload */
typedef struct
{
    zenum8                  u8Status;
} tsCLD_Commissioning_ResponsePayload;

/* Definition of Commissioning Call back Event Structure */
typedef struct
{
    uint8                                       u8CommandId;
    union
    {
        tsCLD_Commissioning_RestartDevicePayload                    *psRestartDevicePayload;
        tsCLD_Commissioning_ModifyStartupParametersPayload          *psModifyStartupParamsPayload; 
    } uReqMessage;
    union
    {
        tsCLD_Commissioning_ResponsePayload                         *psCommissioningResponsePayload; 
    } uRespMessage;
} tsCLD_CommissioningCallBackMessage;


/* Custom data structure */
typedef struct
{
    tsZCL_ReceiveEventAddress              sReceiveEventAddress;
    tsZCL_CallBackEvent                    sCustomCallBackEvent;
    tsCLD_CommissioningCallBackMessage     sCallBackMessage;
} tsCLD_CommissioningCustomDataStructure;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_CommissioningClusterCreateCommissioning(
                tsZCL_ClusterInstance                   *psClusterInstance,
                bool_t                                   bIsServer,
                tsZCL_ClusterDefinition                 *psClusterDefinition,
                uint8                                    u8ClusterSecurity,
                void                                    *pvEndPointSharedStructPtr,
                uint8                                   *pu8AttributeControlBits,
                tsCLD_CommissioningCustomDataStructure  *psCustomDataStructure);

#ifdef COMMISSIONING_CLIENT
PUBLIC  teZCL_Status eCLD_CommissioningCommandRestartDeviceSend(
                                        uint8                                       u8SourceEndPointId,
                                        uint8                                       u8DestinationEndPointId,
                                        tsZCL_Address                               *psDestinationAddress,
                                        uint8                                       *pu8TransactionSequenceNumber,
                                        tsCLD_Commissioning_RestartDevicePayload    *psPayload);

PUBLIC  teZCL_Status eCLD_CommissioningCommandModifyStartupParamsSend(
                                        uint8                                               u8SourceEndPointId,
                                        uint8                                               u8DestinationEndPointId,
                                        tsZCL_Address                                       *psDestinationAddress,
                                        uint8                                               *pu8TransactionSequenceNumber,
                                        tsCLD_Commissioning_ModifyStartupParametersPayload  *psPayload,
                                        teCLD_Commissioning_Command                         eCLD_Commissioning_Command);
                                        
#ifdef  CLD_COMMISSIONING_CMD_SAVE_STARTUP_PARAMS
PUBLIC INLINE teZCL_Status eCLD_CommissioningCommandSaveStartupParamsSend    (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_Commissioning_ModifyStartupParametersPayload          *psPayload) ALWAYS_INLINE;         
PUBLIC INLINE teZCL_Status eCLD_CommissioningCommandSaveStartupParamsSend   (                
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_Commissioning_ModifyStartupParametersPayload  *psPayload)
{
    return eCLD_CommissioningCommandModifyStartupParamsSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                                                              psPayload,
                            E_CLD_COMMISSIONING_CMD_SAVE_STARTUP_PARAMS);
}
#endif

#ifdef  CLD_COMMISSIONING_CMD_RESTORE_STARTUP_PARAMS
PUBLIC INLINE teZCL_Status eCLD_CommissioningCommandRestoreStartupParamsSend    (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_Commissioning_ModifyStartupParametersPayload          *psPayload) ALWAYS_INLINE;         
PUBLIC INLINE teZCL_Status eCLD_CommissioningCommandRestoreStartupParamsSend   (                
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_Commissioning_ModifyStartupParametersPayload  *psPayload)
{
    return eCLD_CommissioningCommandModifyStartupParamsSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                                                              psPayload,
                         E_CLD_COMMISSIONING_CMD_RESTORE_STARTUP_PARAMS);
}
#endif

PUBLIC INLINE teZCL_Status eCLD_CommissioningCommandResetStartupParamsSend    (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_Commissioning_ModifyStartupParametersPayload          *psPayload) ALWAYS_INLINE;         
PUBLIC INLINE teZCL_Status eCLD_CommissioningCommandResetStartupParamsSend   (                
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_Commissioning_ModifyStartupParametersPayload  *psPayload)
{
    return eCLD_CommissioningCommandModifyStartupParamsSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                                                              psPayload,
                            E_CLD_COMMISSIONING_CMD_RESET_STARTUP_PARAMS);
}
#endif /* COMMISSIONING_CLIENT */

#ifdef COMMISSIONING_SERVER
PUBLIC  teZCL_Status eCLD_CommissioningSetAttribute(
                                            uint8                                   u8SourceEndPointId,
                                            teCLD_Commissioning_AttributeSet        eAttributeSet,
                                            void                                    *vptrAttributeSetStructure);
#endif /*COMMISSIONING_SERVER*/
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern tsZCL_ClusterDefinition sCLD_Commissioning;
extern const tsZCL_AttributeDefinition asCLD_CommissioningClusterAttributeDefinitions[];
extern uint8 au8CommissioningClusterAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* COMMISSIONING_H */
