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
 * COMPONENT:          Commissioning.c
 *
 * DESCRIPTION:        Commissioning cluster definition
 *
 ******************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "Commissioning.h"
#include "Commissioning_internal.h"
#include "zcl_options.h"
#include <string.h>

#ifdef CLD_COMMISSIONING

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
    const tsZCL_CommandDefinition asCLD_CommissioningClusterCommandDefinitions[] = {
        {E_CLD_COMMISSIONING_CMD_RESTART_DEVICE,             E_ZCL_CF_RX|E_ZCL_CF_TX},     /* Mandatory */
    #ifdef CLD_COMMISSIONING_CMD_SAVE_STARTUP_PARAMS    
        {E_CLD_COMMISSIONING_CMD_SAVE_STARTUP_PARAMS,        E_ZCL_CF_RX|E_ZCL_CF_TX},     /* Optional */ 
    #endif
    #ifdef CLD_COMMISSIONING_CMD_RESTORE_STARTUP_PARAMS    
        {E_CLD_COMMISSIONING_CMD_RESTORE_STARTUP_PARAMS,     E_ZCL_CF_RX|E_ZCL_CF_TX},     /* Optional */
    #endif
        {E_CLD_COMMISSIONING_CMD_RESET_STARTUP_PARAMS,       E_ZCL_CF_RX|E_ZCL_CF_TX},     /* Mandatory */
    };
#endif

const tsZCL_AttributeDefinition asCLD_CommissioningClusterAttributeDefinitions[] = {
#ifdef COMMISSIONING_SERVER
            /* Commissioning Startup attribute set (13.2.2.2.1) */
    #ifdef     CLD_COMMISSIONING_ATTR_SHORT_ADDRESS
        {E_CLD_COMMISSIONING_ATTR_ID_SHORT_ADDRESS,        (E_ZCL_AF_RD),               E_ZCL_UINT16,        (uint32)(&((tsCLD_Commissioning*)(0))->sStartupParameters.u16ShortAddress),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_EXTENED_PAN_ID
        {E_CLD_COMMISSIONING_ATTR_ID_EXT_PANID,            (E_ZCL_AF_RD),                E_ZCL_IEEE_ADDR,     (uint32)(&((tsCLD_Commissioning*)(0))->sStartupParameters.u64ExtPanId),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_PAN_ID
        {E_CLD_COMMISSIONING_ATTR_ID_PANID,                (E_ZCL_AF_RD),               E_ZCL_UINT16,        (uint32)(&((tsCLD_Commissioning*)(0))->sStartupParameters.u16PANId),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_CHANNEL_MASK
        {E_CLD_COMMISSIONING_ATTR_ID_CHANNEL_MASK,         (E_ZCL_AF_RD),               E_ZCL_BMAP32,        (uint32)(&((tsCLD_Commissioning*)(0))->sStartupParameters.u32ChannelMask),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_PROTOCOL_VERSION
        {E_CLD_COMMISSIONING_ATTR_ID_PROTOCOL_VERSION,     (E_ZCL_AF_RD),               E_ZCL_UINT8,        (uint32)(&((tsCLD_Commissioning*)(0))->sStartupParameters.u8ProtocolVersion),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_STACK_PROFILE
        {E_CLD_COMMISSIONING_ATTR_ID_STACK_PROFILE,        (E_ZCL_AF_RD),               E_ZCL_UINT8,        (uint32)(&((tsCLD_Commissioning*)(0))->sStartupParameters.u8StackProfile),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_START_UP_CONTROL
        {E_CLD_COMMISSIONING_ATTR_ID_STARTUP_CONTROl,      (E_ZCL_AF_RD),               E_ZCL_ENUM8,         (uint32)(&((tsCLD_Commissioning*)(0))->sStartupParameters.e8StartUpControl),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_TC_ADDR
        {E_CLD_COMMISSIONING_ATTR_ID_TC_ADDR,              (E_ZCL_AF_RD),               E_ZCL_IEEE_ADDR,     (uint32)(&((tsCLD_Commissioning*)(0))->sStartupParameters.u64TcAddr),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_TC_MASTER_KEY
        {E_CLD_COMMISSIONING_ATTR_ID_TC_MASTER_KEY,        (E_ZCL_AF_RD),               E_ZCL_KEY_128,       (uint32)(&((tsCLD_Commissioning*)(0))->sStartupParameters.sTcMasterKey),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_NWK_KEY
        {E_CLD_COMMISSIONING_ATTR_ID_NETWORK_KEY,          (E_ZCL_AF_RD),               E_ZCL_KEY_128,       (uint32)(&((tsCLD_Commissioning*)(0))->sStartupParameters.sNwkKey),0},
    #endif


    #ifdef     CLD_COMMISSIONING_ATTR_USE_INSECURE_JOIN
        {E_CLD_COMMISSIONING_ATTR_ID_USE_INSECURE_JOIN,    (E_ZCL_AF_RD),               E_ZCL_BOOL,          (uint32)(&((tsCLD_Commissioning*)(0))->sStartupParameters.bUseInsecureJoin),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_PRE_CONFIG_LINK_KEY
        {E_CLD_COMMISSIONING_ATTR_ID_PRECONFIG_LINK_KEY,   (E_ZCL_AF_RD),               E_ZCL_KEY_128,       (uint32)(&((tsCLD_Commissioning*)(0))->sStartupParameters.sPreConfigLinkKey),0},
    #endif


    #ifdef     CLD_COMMISSIONING_ATTR_NWK_KEY_SEQ_NO
        {E_CLD_COMMISSIONING_ATTR_ID_NWK_KEY_SEQ_NO,       (E_ZCL_AF_RD),               E_ZCL_UINT8,         (uint32)(&((tsCLD_Commissioning*)(0))->sStartupParameters.u8NwkKeySeqNo),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_NWK_KEY_TYPE
        {E_CLD_COMMISSIONING_ATTR_ID_NWK_KEY_TYPE,         (E_ZCL_AF_RD),               E_ZCL_ENUM8,         (uint32)(&((tsCLD_Commissioning*)(0))->sStartupParameters.e8NwkKeyType),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_NWK_MANAGER_ADDR
        {E_CLD_COMMISSIONING_ATTR_ID_NWK_MANAGER_ADDR,     (E_ZCL_AF_RD),               E_ZCL_UINT16,        (uint32)(&((tsCLD_Commissioning*)(0))->sStartupParameters.u16NwkManagerAddr),0},
    #endif
        /* Join Parameters attribute set (13.2.2.2.2)*/
    #ifdef     CLD_COMMISSIONING_ATTR_SCAN_ATTEMPTS
        {E_CLD_COMMISSIONING_ATTR_ID_SCAN_ATTEMPTS,        (E_ZCL_AF_RD),               E_ZCL_UINT8,         (uint32)(&((tsCLD_Commissioning*)(0))->sJoinParameters.u8ScanAttempts),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_TIME_BW_SCANS
        {E_CLD_COMMISSIONING_ATTR_ID_TIME_BW_SCANS,        (E_ZCL_AF_RD),               E_ZCL_UINT16,        (uint32)(&((tsCLD_Commissioning*)(0))->sJoinParameters.u16TimeBwScans),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_REJOIN_INTERVAL
        {E_CLD_COMMISSIONING_ATTR_ID_REJOIN_INTERVAL,      (E_ZCL_AF_RD),               E_ZCL_UINT16,        (uint32)(&((tsCLD_Commissioning*)(0))->sJoinParameters.u16RejoinInterval),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_MAX_REJOIN_INTERVAL
        {E_CLD_COMMISSIONING_ATTR_ID_MAX_REJOIN_INTERVAL,   (E_ZCL_AF_RD),              E_ZCL_UINT16,       (uint32)(&((tsCLD_Commissioning*)(0))->sJoinParameters.u16MaxRejoinInterval),0},
    #endif
        /* End Device Parameters attribute set (13.2.2.2.3)*/
    #ifdef     CLD_COMMISSIONING_ATTR_INDIRECT_POLL_RATE
        {E_CLD_COMMISSIONING_ATTR_ID_INDIRECT_POLL_RATE,    (E_ZCL_AF_RD),              E_ZCL_UINT16,       (uint32)(&((tsCLD_Commissioning*)(0))->sEndDeviceParameters.u16IndirectPollRate),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_PARENT_RETRY_THRSHLD
        {E_CLD_COMMISSIONING_ATTR_ID_PARENT_RETRY_THRSHOLD,  (E_ZCL_AF_RD),             E_ZCL_UINT8,        (uint32)(&((tsCLD_Commissioning*)(0))->sEndDeviceParameters.u8ParentRetryThreshold),0},
    #endif
        /* Concentrator Parameters attribute set (3.15.2.2.4)*/
    #ifdef     CLD_COMMISSIONING_ATTR_CONCENTRATOR_FLAG
        {E_CLD_COMMISSIONING_ATTR_ID_CONCENTRATOR_FLAG,     (E_ZCL_AF_RD),              E_ZCL_BOOL,         (uint32)(&((tsCLD_Commissioning*)(0))->sConcentratorParameters.bConcentratorFlag),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_CONCENTRATOR_RADIUS
        {E_CLD_COMMISSIONING_ATTR_ID_CONCENTRATOR_RADIUS,   (E_ZCL_AF_RD),              E_ZCL_UINT8,        (uint32)(&((tsCLD_Commissioning*)(0))->sConcentratorParameters.u8ConcentratorRadius),0},
    #endif

    #ifdef     CLD_COMMISSIONING_ATTR_CONCENTRATOR_DISCVRY_TIME
        {E_CLD_COMMISSIONING_ATTR_ID_CONCENTRATOR_DISCVRY_TIME,(E_ZCL_AF_RD),           E_ZCL_UINT8,        (uint32)(&((tsCLD_Commissioning*)(0))->sConcentratorParameters.u8ConcentratorDiscoveryTime),0},
    #endif
#endif
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                  (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_BMAP32,   (uint32)(&((tsCLD_Commissioning*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,             (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_UINT16,       (uint32)(&((tsCLD_Commissioning*)(0))->u16ClusterRevision),0},   /* Mandatory  */
    };

tsZCL_ClusterDefinition sCLD_Commissioning = {
        COMMISSIONING_CLUSTER_ID_COMMISSIONING,
        FALSE,
        E_ZCL_SECURITY_APPLINK,
        (sizeof(asCLD_CommissioningClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_CommissioningClusterAttributeDefinitions,
        NULL
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED        
        ,
        (sizeof(asCLD_CommissioningClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
        (tsZCL_CommandDefinition*)asCLD_CommissioningClusterCommandDefinitions          
#endif  
};
uint8 au8CommissioningClusterAttributeControlBits[(sizeof(asCLD_CommissioningClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eCLD_CommissioningClusterCreateCommissioning
 **
 ** DESCRIPTION:
 ** Creates a time cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_CommissioningClusterCreateCommissioning(
                tsZCL_ClusterInstance                   *psClusterInstance,
                bool_t                                  bIsServer,
                tsZCL_ClusterDefinition                 *psClusterDefinition,
                uint8                                   u8ClusterSecurity,
                void                                    *pvEndPointSharedStructPtr,
                uint8                                   *pu8AttributeControlBits,
                tsCLD_CommissioningCustomDataStructure  *psCustomDataStructure)
{

    tsCLD_Commissioning *psCluster;

    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if((psClusterInstance==NULL) ||
           (psClusterDefinition==NULL) ||
            (psCustomDataStructure==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    /* Create an instance of a commissioning cluster */
    vZCL_InitializeClusterInstance(
                                   psClusterInstance, 
                                   bIsServer,
                                   psClusterDefinition,
                                   pvEndPointSharedStructPtr,
                                   pu8AttributeControlBits,
                                   psCustomDataStructure,
                                   eCLD_CommissioningCommandHandler);

    psCustomDataStructure->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterDefinition->u16ClusterEnum;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psCustomDataStructure->sCallBackMessage;
    psCustomDataStructure->sCustomCallBackEvent.psClusterInstance = psClusterInstance; 
    
    /* Update cluster security */
    psClusterInstance->psClusterDefinition->u8ClusterControlFlags |= u8ClusterSecurity;

    //  attributes set default values
    psCluster = (tsCLD_Commissioning*)pvEndPointSharedStructPtr;
    
    psCluster->u32FeatureMap = CLD_COMMISSIONING_FEATURE_MAP;
    
    psCluster->u16ClusterRevision = CLD_COMMISSIONING_CLUSTER_REVISION;
    
    return E_ZCL_SUCCESS;

}

#ifdef COMMISSIONING_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_CommissioningSetAttribute
 **
 ** DESCRIPTION:
 ** Used by cluster to set its attributes
 **
 ** PARAMETERS:                         Name                        Usage
 ** uint8                               u8SourceEndPointId          EndPoint Id
 ** teCLD_Commissioning_AttributeSet    eAttributeSet               Attrbute Set
 ** void*                               pvAttributeSetStructure    Pointer to attribute set structure
 **
 ** RETURN:
 ** teZCL_Status
 ** 
 ****************************************************************************/

PUBLIC  teZCL_Status eCLD_CommissioningSetAttribute(
                                            uint8                                   u8SourceEndPointId,
                                            teCLD_Commissioning_AttributeSet        eAttributeSet,
                                            void                                    *pvAttributeSetStructure)  
{
    teZCL_Status eStatus;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_CommissioningCustomDataStructure *psCustomDataStructure;

    if(pvAttributeSetStructure == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }    
    
    eStatus = eZCL_FindCluster(COMMISSIONING_CLUSTER_ID_COMMISSIONING, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCustomDataStructure);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }
        
    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif
    
    switch(eAttributeSet)
    {
        case E_CLD_COMMISSIONING_ATTR_SET_STARTUP_PARAMS:
        {
            tsCLD_StartupParameters *psStartupParameters = (tsCLD_StartupParameters *)&(((tsCLD_Commissioning *)(psClusterInstance->pvEndPointSharedStructPtr))->sStartupParameters);
            memcpy(psStartupParameters,(tsCLD_StartupParameters *)pvAttributeSetStructure,sizeof(tsCLD_StartupParameters));
            break;
        }
        case E_CLD_COMMISSIONING_ATTR_SET_JOIN_PARAMS:
        {
            tsCLD_JoinParameters *psJoinParameters = (tsCLD_JoinParameters *)&(((tsCLD_Commissioning *)(psClusterInstance->pvEndPointSharedStructPtr))->sJoinParameters);
            memcpy(psJoinParameters,(tsCLD_JoinParameters *)pvAttributeSetStructure,sizeof(tsCLD_JoinParameters));
            break;
        }
        case E_CLD_COMMISSIONING_ATTR_SET_ENDDEVICE_PARAMS:
        {
            tsCLD_EndDeviceParameters *psEndDeviceParameters = (tsCLD_EndDeviceParameters *)&(((tsCLD_Commissioning *)(psClusterInstance->pvEndPointSharedStructPtr))->sEndDeviceParameters);
            memcpy(psEndDeviceParameters,(tsCLD_EndDeviceParameters *)pvAttributeSetStructure,sizeof(tsCLD_EndDeviceParameters));
            break;
        }
        case E_CLD_COMMISSIONING_ATTR_SET_CONCENTRATOR_PARAMS:
        {
            tsCLD_ConcentratorParameters *psConcentratorParameters = (tsCLD_ConcentratorParameters *)&(((tsCLD_Commissioning *)(psClusterInstance->pvEndPointSharedStructPtr))->sConcentratorParameters);
            memcpy(psConcentratorParameters,(tsCLD_ConcentratorParameters *)pvAttributeSetStructure,sizeof(tsCLD_ConcentratorParameters));
            break;
        }
        
        default:
            return E_ZCL_ERR_ATTRIBUTE_TYPE_UNSUPPORTED;
            break;
    }
    
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif
    
    return eStatus;
}

#endif
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

