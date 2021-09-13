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
 * COMPONENT:          ACE.c
 *
 * DESCRIPTION:        IAS ACE cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>

#include "zps_apl.h"
#include "zps_apl_aib.h"

#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"
#include "string.h"
#include "IASACE.h"
#include "IASACE_internal.h"

#include "dbg.h"

#ifdef DEBUG_CLD_IASACE
#define TRACE_IASACE    TRUE
#else
#define TRACE_IASACE    FALSE
#endif



/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if defined(CLD_IASACE) && !defined(IASACE_SERVER) && !defined(IASACE_CLIENT)
#error You Must Have either IASACE_SERVER and/or IASACE_CLIENT defined in zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#ifdef CLD_IASACE
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
    const tsZCL_CommandDefinition asCLD_IASACEClusterCommandDefinitions[] = {

        /* Server Receives and client sends */
        {E_CLD_IASACE_CMD_ARM,                    E_ZCL_CF_RX}, /* Mandatory */
        {E_CLD_IASACE_CMD_BYPASS,                 E_ZCL_CF_RX}, /* Mandatory */
        {E_CLD_IASACE_CMD_EMERGENCY,              E_ZCL_CF_RX}, /* Mandatory */
        {E_CLD_IASACE_CMD_FIRE,                   E_ZCL_CF_RX}, /* Mandatory */
        {E_CLD_IASACE_CMD_PANIC,                  E_ZCL_CF_RX}, /* Mandatory */
        {E_CLD_IASACE_CMD_GET_ZONE_ID_MAP,        E_ZCL_CF_RX}, /* Mandatory */
        {E_CLD_IASACE_CMD_GET_ZONE_INFO,          E_ZCL_CF_RX}, /* Mandatory */
        {E_CLD_IASACE_CMD_GET_PANEL_STATUS,       E_ZCL_CF_RX}, /* Mandatory */
        {E_CLD_IASACE_CMD_GET_BYPASSED_ZONE_LIST, E_ZCL_CF_RX}, /* Mandatory */    
        {E_CLD_IASACE_CMD_GET_ZONE_STATUS,        E_ZCL_CF_RX}, /* Mandatory */     
       
       
        /* Server Sends and client receives */
        {E_CLD_IASACE_CMD_ARM_RESP,               E_ZCL_CF_TX}, /* Mandatory */
        {E_CLD_IASACE_CMD_GET_ZONE_ID_MAP_RESP,   E_ZCL_CF_TX}, /* Mandatory */
        {E_CLD_IASACE_CMD_GET_ZONE_INFO_RESP,     E_ZCL_CF_TX}, /* Mandatory */
        {E_CLD_IASACE_CMD_ZONE_STATUS_CHANGED,    E_ZCL_CF_TX}, /* Mandatory */    
        {E_CLD_IASACE_CMD_PANEL_STATUS_CHANGED,   E_ZCL_CF_TX}, /* Mandatory */
        {E_CLD_IASACE_CMD_GET_PANEL_STATUS_RESP,  E_ZCL_CF_TX}, /* Mandatory */     
        {E_CLD_IASACE_CMD_SET_BYPASSED_ZONE_LIST, E_ZCL_CF_TX}, /* Mandatory */
        {E_CLD_IASACE_CMD_BYPASS_RESP,            E_ZCL_CF_TX}, /* Mandatory */
        {E_CLD_IASACE_CMD_GET_ZONE_STATUS_RESP,   E_ZCL_CF_TX}, /* Mandatory */ 
       
    };
#endif

const tsZCL_AttributeDefinition asCLD_IASACEClusterAttributeDefinitions[] = {
    {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,       (E_ZCL_AF_RD|E_ZCL_AF_GA),        E_ZCL_BMAP32,   (uint32)(&((tsCLD_IASACE*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

    {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,  (E_ZCL_AF_RD|E_ZCL_AF_GA),        E_ZCL_UINT16,    (uint32)(&((tsCLD_IASACE*)(0))->u16ClusterRevision),0},   /* Mandatory  */
};

tsZCL_ClusterDefinition sCLD_IASACE = {
        SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_IASACEClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_IASACEClusterAttributeDefinitions,
        NULL
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED        
        ,
        (sizeof(asCLD_IASACEClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
        (tsZCL_CommandDefinition*)asCLD_IASACEClusterCommandDefinitions 
#endif        
};
uint8 au8IASACEAttributeControlBits[(sizeof(asCLD_IASACEClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eCLD_IASACECreateIASACE
 **
 ** DESCRIPTION:
 ** Creates a thermostat cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_IASACECreateIASACE(
                tsZCL_ClusterInstance                   *psClusterInstance,
                bool_t                                  bIsServer,
                tsZCL_ClusterDefinition                 *psClusterDefinition,
                void                                    *pvEndPointSharedStructPtr,
                uint8                                   *pu8AttributeControlBits,                
                tsCLD_IASACECustomDataStructure         *psCustomDataStructure)
{
#ifdef IASACE_SERVER    
    uint8 u8ZoneCount;
#endif

    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if((psClusterInstance==NULL) ||
           (psClusterDefinition==NULL) ||
           (psCustomDataStructure==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif
    
    // cluster data
    vZCL_InitializeClusterInstance(
                                   psClusterInstance, 
                                   bIsServer,
                                   psClusterDefinition,
                                   pvEndPointSharedStructPtr,
                                   pu8AttributeControlBits,
                                   psCustomDataStructure,
                                   eCLD_IASACECommandHandler);  
                                   
    psCustomDataStructure->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterDefinition->u16ClusterEnum;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psCustomDataStructure->sCallBackMessage;
    psCustomDataStructure->sCustomCallBackEvent.psClusterInstance = psClusterInstance; 
    
                                   
    #ifdef IASACE_SERVER
    for(u8ZoneCount = 0 ; u8ZoneCount <CLD_IASACE_ZONE_TABLE_SIZE ; u8ZoneCount++)
    {
        psCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneCount].sArmDisarmCode.u8MaxLength = CLD_IASACE_MAX_LENGTH_ARM_DISARM_CODE;
        psCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneCount].sZoneLabel.u8MaxLength = CLD_IASACE_MAX_LENGTH_ZONE_LABEL;
        psCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneCount].sArmDisarmCode.pu8Data = psCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneCount].au8ArmDisarmCode;
        psCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneCount].sZoneLabel.pu8Data = psCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneCount].au8ZoneLabel;
    }    
    #endif
    
    if(pvEndPointSharedStructPtr != NULL)
    {
        ((tsCLD_IASACE*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_IASACE_FEATURE_MAP;
      
        ((tsCLD_IASACE*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_IASACE_CLUSTER_REVISION;
    }

    
    return E_ZCL_SUCCESS;

}

#ifdef IASACE_SERVER

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEAddZoneEntry
 **
 ** DESCRIPTION:
 ** Used by cluster to add a zone table entry
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          EndPoint Id
 ** uint16                      u16ZoneType                 Zone Type
 ** uint64                      u64IeeeAddress              IEEE address of the zone
 ** uint8                       *pu8ZoneID                  Pointer to the Zone ID allocated
 **
 ** RETURN:
 ** teZCL_Status
 ** 
 ****************************************************************************/

PUBLIC teZCL_CommandStatus eCLD_IASACEAddZoneEntry (
                uint8                                       u8SourceEndPointId,
                uint16                                      u16ZoneType,
                uint64                                      u64IeeeAddress,
                uint8                                       *pu8ZoneID)  
{
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_IASACECustomDataStructure *pCustomDataStructure;
    uint8 u8ZoneCount;
  
    if(pu8ZoneID == NULL)
    {
        return E_ZCL_CMDS_INVALID_FIELD;
    }
    
    /* Find pointers to cluster */
    if(eZCL_FindCluster(SECURITY_AND_SAFETY_CLUSTER_ID_IASACE, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_CMDS_FAILURE;
    }

    if((u64IeeeAddress  == 0) || (u64IeeeAddress  == 0xffffffffffffffffULL))
    {
        return E_ZCL_CMDS_INVALID_VALUE;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    
    for( u8ZoneCount = 0 ; u8ZoneCount < CLD_IASACE_ZONE_TABLE_SIZE; u8ZoneCount++)
    {
        if( pCustomDataStructure->asCLD_IASACE_ZoneTable[u8ZoneCount].u64IeeeAddress == 0)
            {
                pCustomDataStructure->asCLD_IASACE_ZoneTable[u8ZoneCount].u8ZoneID = u8ZoneCount;
                pCustomDataStructure->asCLD_IASACE_ZoneTable[u8ZoneCount].u16ZoneType = u16ZoneType;
                pCustomDataStructure->asCLD_IASACE_ZoneTable[u8ZoneCount].u64IeeeAddress = u64IeeeAddress;
                *pu8ZoneID = u8ZoneCount;
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif

                return E_ZCL_CMDS_SUCCESS;
            }
    }

    //Release Mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    
    return E_ZCL_CMDS_INSUFFICIENT_SPACE;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACERemoveZoneEntry
 **
 ** DESCRIPTION:
 ** removes a zone table entry
 **
 ** PARAMETERS:                 Name                    Usage
 ** uint8                       u8SourceEndPointId      Endpoint id
 ** uint8                       u8ZoneID                Zone Id
 ** uint64                      pu64IeeeAddress         Pointer to IEEE address 
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_CommandStatus eCLD_IASACERemoveZoneEntry (
                                uint8                                   u8SourceEndPointId,
                                uint8                                   u8ZoneID,
                                uint64                                  *pu64IeeeAddress)

{
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_IASACECustomDataStructure *pCustomDataStructure;
    uint8 u8ZoneCount;
    
    /* Find pointers to cluster */
    if(eZCL_FindCluster(SECURITY_AND_SAFETY_CLUSTER_ID_IASACE, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_CMDS_FAILURE;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    
    for( u8ZoneCount = 0 ; u8ZoneCount < CLD_IASACE_ZONE_TABLE_SIZE; u8ZoneCount++)
    {
        if((pCustomDataStructure->asCLD_IASACE_ZoneTable[u8ZoneCount].u8ZoneID == u8ZoneID) && (pCustomDataStructure->asCLD_IASACE_ZoneTable[u8ZoneCount].u64IeeeAddress != 0))
            {
                *pu64IeeeAddress = pCustomDataStructure->asCLD_IASACE_ZoneTable[u8ZoneCount].u64IeeeAddress;
                pCustomDataStructure->asCLD_IASACE_ZoneTable[u8ZoneCount].u64IeeeAddress = 0;
                //Release Mutex
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif

                return E_ZCL_CMDS_SUCCESS;
            }
    }

    //Release Mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return E_ZCL_CMDS_NOT_FOUND;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEGetZoneTableEntry
 **
 ** DESCRIPTION:
 ** Gives the zone table entry
 **
 ** PARAMETERS:                 Name                    Usage
 ** uint8                       u8SourceEndPointId      Endpoint id
 ** uint32                      u32LogId                Log Id
 ** tsCLD_LogTable**            ppsZoneTable             Pointer to Pointer to Zone entry
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_CommandStatus eCLD_IASACEGetZoneTableEntry (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8ZoneID,
                tsCLD_IASACE_ZoneTable                      **ppsZoneTable)
{
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_IASACECustomDataStructure *pCustomDataStructure;
    uint8 u8ZoneCount;

    /* Find pointers to cluster */
    if(eZCL_FindCluster(SECURITY_AND_SAFETY_CLUSTER_ID_IASACE, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_CMDS_FAILURE;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    for( u8ZoneCount = 0 ; u8ZoneCount < CLD_IASACE_ZONE_TABLE_SIZE; u8ZoneCount++)
    {
        if((pCustomDataStructure->asCLD_IASACE_ZoneTable[u8ZoneCount].u8ZoneID == u8ZoneID) && (pCustomDataStructure->asCLD_IASACE_ZoneTable[u8ZoneCount].u64IeeeAddress != 0))
            {
                 *ppsZoneTable = (tsCLD_IASACE_ZoneTable *)(&(pCustomDataStructure->asCLD_IASACE_ZoneTable[u8ZoneCount]));
                 DBG_vPrintf(TRACE_IASACE, "ADDR = %016lx = %016lx\n",ppsZoneTable,(*ppsZoneTable));
                 //Release Mutex
                 #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                 #endif

                 return E_ZCL_CMDS_SUCCESS;
            }
    }

    //Release Mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return E_ZCL_CMDS_NOT_FOUND;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEGetEnrolledZones
 **
 ** DESCRIPTION:
 ** Gives the list of all the enrolled zones back
 **
 ** PARAMETERS:                 Name                    Usage
 ** uint8                       u8SourceEndPointId      Endpoint id
 ** uint8*                      pu8ZoneID               Pointer to Enrolled Zone ID list
 ** uint8*                      pu8NumOfEnrolledZones   Pointer to Number of Enrolled devices
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_CommandStatus eCLD_IASACEGetEnrolledZones (
                uint8                                       u8SourceEndPointId,
                uint8                                       *pu8ZoneID,
                uint8                                       *pu8NumOfEnrolledZones)
{
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_IASACECustomDataStructure *pCustomDataStructure;
    uint8 u8ZoneCount = 0,u8ZoneAvailableCount = 0;

    
    if(pu8ZoneID == NULL || pu8NumOfEnrolledZones == NULL)
    {
        return E_ZCL_CMDS_INVALID_FIELD;
    }
    
    /* Find pointers to cluster */
    if(eZCL_FindCluster(SECURITY_AND_SAFETY_CLUSTER_ID_IASACE, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_CMDS_FAILURE;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    
    for( u8ZoneCount = 0 ; u8ZoneCount < CLD_IASACE_ZONE_TABLE_SIZE; u8ZoneCount++)
    {
        if(pCustomDataStructure->asCLD_IASACE_ZoneTable[u8ZoneCount].u64IeeeAddress != 0)
            {
                 pu8ZoneID[u8ZoneAvailableCount] = pCustomDataStructure->asCLD_IASACE_ZoneTable[u8ZoneCount].u8ZoneID;
                 u8ZoneAvailableCount++;
                 
                 if(u8ZoneAvailableCount > *pu8NumOfEnrolledZones)
                 {
                    *pu8NumOfEnrolledZones = u8ZoneAvailableCount;
                    //Release Mutex
                    #ifndef COOPERATIVE
                        eZCL_ReleaseMutex(psEndPointDefinition);
                    #endif

                    return E_ZCL_CMDS_INSUFFICIENT_SPACE;
                 }
            }
    }

    *pu8NumOfEnrolledZones = u8ZoneAvailableCount;
    
    //Release Mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif
    
    return E_ZCL_CMDS_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_IASACESetPanelParameter
 **
 ** DESCRIPTION:
 ** Allows to set Panel Parameter
 **
 ** PARAMETERS:                     Name                                    Usage
 ** uint8                           u8SourceEndPointId                      Endpoint id
 ** teCLD_IASACE_PanelParameterID   eParameterId                            ID of the panel parameter to be written
 ** uint8                           u8ParameterValue                        Parameter Value 
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACESetPanelParameter (
                uint8                                       u8SourceEndPointId,
                teCLD_IASACE_PanelParameterID               eParameterId,
                uint8                                       u8ParameterValue)
{
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_IASACECustomDataStructure *pCustomDataStructure;
    tsZCL_Address sZCL_Address = {0};
    uint8 u8TransactionSeqNumber = 0;
    tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload sPayload;

    /* Find pointers to cluster */
    if(eZCL_FindCluster(SECURITY_AND_SAFETY_CLUSTER_ID_IASACE, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_ERR_CLUSTER_NOT_FOUND;
    }
    
    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    
    switch(eParameterId)
    {
        case E_CLD_IASACE_PANEL_PARAMETER_PANEL_STATUS:
        
            pCustomDataStructure->sCLD_IASACE_PanelParameter.ePanelStatus = u8ParameterValue;
            sZCL_Address.eAddressMode = E_ZCL_AM_BOUND;
            memcpy(&sPayload,(tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload *)&pCustomDataStructure->sCLD_IASACE_PanelParameter,sizeof(tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload));
            eCLD_IASACE_PanelStatusChangedSend (
                                                u8SourceEndPointId,
                                                0,
                                                &sZCL_Address,
                                                &u8TransactionSeqNumber,
                                                &sPayload);
        break;
        
        case E_CLD_IASACE_PANEL_PARAMETER_SECONDS_REMAINING:
            pCustomDataStructure->sCLD_IASACE_PanelParameter.u8SecondsRemaining = u8ParameterValue;
        break;
        
        case E_CLD_IASACE_PANEL_PARAMETER_AUDIBLE_NOTIFICATION:
            pCustomDataStructure->sCLD_IASACE_PanelParameter.eAudibleNotification = u8ParameterValue;
        break;
        
        case E_CLD_IASACE_PANEL_PARAMETER_ALARM_STATUS:
            pCustomDataStructure->sCLD_IASACE_PanelParameter.eAlarmStatus = u8ParameterValue;
        break;
    
        default:
            //Release Mutex
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif
          
            return E_ZCL_ERR_ATTRIBUTE_NOT_FOUND;
        break;           
    }
    
    //Release Mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif
        
    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEGetPanelParameter
 **
 ** DESCRIPTION:
 ** Allows to get Panel Parameter
 **
 ** PARAMETERS:                     Name                                    Usage
 ** uint8                           u8SourceEndPointId                      Endpoint id
 ** teCLD_IASACE_PanelParameterID   eParameterId                            ID of the panel parameter to be written
 ** uint8*                          *pu8ParameterValue                      Parameter Value 
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACEGetPanelParameter (
                uint8                                       u8SourceEndPointId,
                teCLD_IASACE_PanelParameterID               eParameterId,
                uint8                                       *pu8ParameterValue)
{
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_IASACECustomDataStructure *pCustomDataStructure;

        
    if(pu8ParameterValue == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }
 
    /* Find pointers to cluster */
    if(eZCL_FindCluster(SECURITY_AND_SAFETY_CLUSTER_ID_IASACE, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_ERR_CLUSTER_NOT_FOUND;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    
    switch(eParameterId)
    {
        case E_CLD_IASACE_PANEL_PARAMETER_PANEL_STATUS:
            *pu8ParameterValue = pCustomDataStructure->sCLD_IASACE_PanelParameter.ePanelStatus;
        break;
        
        case E_CLD_IASACE_PANEL_PARAMETER_SECONDS_REMAINING:
            *pu8ParameterValue = pCustomDataStructure->sCLD_IASACE_PanelParameter.u8SecondsRemaining;
        break;
        
        case E_CLD_IASACE_PANEL_PARAMETER_AUDIBLE_NOTIFICATION:
            *pu8ParameterValue = pCustomDataStructure->sCLD_IASACE_PanelParameter.eAudibleNotification;
        break;
        
        case E_CLD_IASACE_PANEL_PARAMETER_ALARM_STATUS:
            *pu8ParameterValue = pCustomDataStructure->sCLD_IASACE_PanelParameter.eAlarmStatus;
        break;
    
        default:
            //Release Mutex
            #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif
  
            return E_ZCL_ERR_ATTRIBUTE_NOT_FOUND;
        break;           
    }
    
    //Release Mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif
        
    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACESetZoneParameter
 **
 ** DESCRIPTION:
 ** Allows to set zone Parameter
 **
 ** PARAMETERS:                     Name                                    Usage
 ** uint8                           u8SourceEndPointId                      Endpoint id
 ** teCLD_IASACE_ZoneParameterID    eParameterId                            ID of the zone parameter to be written
 ** uint8                           u8ZoneID                                Zone ID
 ** uint8                           u8ParameterLength                       Parameter Length
 ** uint8*                          pu8ParameterValue                       Pointer to Parameter Value 
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACESetZoneParameter (
                uint8                                       u8SourceEndPointId,
                teCLD_IASACE_ZoneParameterID                eParameterId,
                uint8                                       u8ZoneID,
                uint8                                       u8ParameterLength,
                uint8                                       *pu8ParameterValue)
{
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_IASACECustomDataStructure *pCustomDataStructure;
    tsCLD_IASACE_ZoneTable *psZoneTable;
    tsZCL_Address sZCL_Address = {0};
    uint8 u8TransactionSeqNumber = 0;
    tsCLD_IASACE_ZoneStatusChangedPayload sPayload;

    if(pu8ParameterValue == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    if( eCLD_IASACEGetZoneTableEntry(u8SourceEndPointId,u8ZoneID,&psZoneTable) != E_ZCL_CMDS_SUCCESS)
    {
        return E_ZCL_ERR_NO_REPORT_ENTRIES;
    }
    
    /* Find pointers to cluster */
    if(eZCL_FindCluster(SECURITY_AND_SAFETY_CLUSTER_ID_IASACE, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_ERR_CLUSTER_NOT_FOUND;
    }
    
    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    
    switch(eParameterId)
    {
        case E_CLD_IASACE_ZONE_PARAMETER_ZONE_CONFIG_FLAG:
            pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].u8ZoneConfigFlag = pu8ParameterValue[0];
        break;
        
        case E_CLD_IASACE_ZONE_PARAMETER_ZONE_STATUS_FLAG:
            pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].u8ZoneStatusFlag = pu8ParameterValue[0];
        break;
        
        case E_CLD_IASACE_ZONE_PARAMETER_ZONE_STATUS:
            pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].eZoneStatus = (uint16)((pu8ParameterValue[1] << 8) | pu8ParameterValue[0]);
            sZCL_Address.eAddressMode = E_ZCL_AM_BOUND;
            sPayload.u8ZoneID = u8ZoneID;
            sPayload.eZoneStatus = pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].eZoneStatus;
            sPayload.eAudibleNotification = pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].eAudibleNotification;
            sPayload.sZoneLabel = pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].sZoneLabel;
            eCLD_IASACE_ZoneStatusChangedSend (
                                        u8SourceEndPointId,
                                        0,
                                        &sZCL_Address,
                                        &u8TransactionSeqNumber,
                                        &sPayload);
        break;
        
        case E_CLD_IASACE_ZONE_PARAMETER_AUDIBLE_NOTIFICATION:
            pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].eAudibleNotification = pu8ParameterValue[0];
        break;
    
        case E_CLD_IASACE_ZONE_PARAMETER_ZONE_LABEL:
            if(u8ParameterLength > CLD_IASACE_MAX_LENGTH_ZONE_LABEL)
            {
                //Release Mutex
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif
  
                return E_ZCL_ERR_PARAMETER_RANGE;
            }
            pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].sZoneLabel.u8Length = u8ParameterLength;
            memcpy(pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].au8ZoneLabel,pu8ParameterValue,u8ParameterLength);
        break;
        
        case E_CLD_IASACE_ZONE_PARAMETER_ARM_DISARM_CODE:
            if(u8ParameterLength > CLD_IASACE_MAX_LENGTH_ARM_DISARM_CODE)
            {
                //Release Mutex
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif
                  
                return E_ZCL_ERR_PARAMETER_RANGE;
            }
            pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].sArmDisarmCode.u8Length = u8ParameterLength;
            memcpy(pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].au8ArmDisarmCode,pu8ParameterValue,u8ParameterLength);
        break;
        
        default:
            //Release Mutex
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif
          
            return E_ZCL_ERR_ATTRIBUTE_NOT_FOUND;
        break;           
    }
    
    //Release Mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif
        
    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEGetZoneParameter
 **
 ** DESCRIPTION:
 ** Allows to get zone Parameter
 **
 ** PARAMETERS:                     Name                                    Usage
 ** uint8                           u8SourceEndPointId                      Endpoint id
 ** teCLD_IASACE_ZoneParameterID    eParameterId                            ID of the zone parameter to be written
 ** uint8                           u8ZoneID                                Zone ID of the zone to be changed
 ** uint8                           *pu8ParameterLength                     Pointer to Parameter Length   
 ** uint8                           *pu8ParameterValue                      Pointer to Paramater value 
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACEGetZoneParameter (
                uint8                                       u8SourceEndPointId,
                teCLD_IASACE_ZoneParameterID                eParameterId,
                uint8                                       u8ZoneID,
                uint8                                       *pu8ParameterLength,
                uint8                                       *pu8ParameterValue)
{
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_IASACECustomDataStructure *pCustomDataStructure;
    tsCLD_IASACE_ZoneTable *psZoneTable;

    if(pu8ParameterValue == NULL || pu8ParameterLength == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }
    
    if( eCLD_IASACEGetZoneTableEntry(u8SourceEndPointId,u8ZoneID,&psZoneTable) != E_ZCL_CMDS_SUCCESS)
    {
        return E_ZCL_ERR_NO_REPORT_ENTRIES;
    }
    
    /* Find pointers to cluster */
    if(eZCL_FindCluster(SECURITY_AND_SAFETY_CLUSTER_ID_IASACE, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_ERR_CLUSTER_NOT_FOUND;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    
    switch(eParameterId)
    {
        case E_CLD_IASACE_ZONE_PARAMETER_ZONE_CONFIG_FLAG:
            *pu8ParameterLength = 1; 
            pu8ParameterValue[0] = pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].u8ZoneConfigFlag;
        break;
        
        case E_CLD_IASACE_ZONE_PARAMETER_ZONE_STATUS_FLAG:
            *pu8ParameterLength = 1; 
            pu8ParameterValue[0] = pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].u8ZoneStatusFlag;
        break;
        
        case E_CLD_IASACE_ZONE_PARAMETER_ZONE_STATUS:
            *pu8ParameterLength = 2; 
            pu8ParameterValue[0] = (uint8)(pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].eZoneStatus >> 8);
            pu8ParameterValue[1] = (uint8)pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].eZoneStatus;
        break;
        
        case E_CLD_IASACE_ZONE_PARAMETER_AUDIBLE_NOTIFICATION:
            *pu8ParameterLength = 1; 
            pu8ParameterValue[0] = pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].eAudibleNotification;
        break;
    
        case E_CLD_IASACE_ZONE_PARAMETER_ZONE_LABEL:
            if(*pu8ParameterLength < pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].sZoneLabel.u8Length)
            {
                //Release Mutex
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif
    
                return E_ZCL_ERR_PARAMETER_RANGE;
            }
            *pu8ParameterLength = pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].sZoneLabel.u8Length;
            memcpy(pu8ParameterValue,pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].au8ZoneLabel,*pu8ParameterLength);
        break;
        
        case E_CLD_IASACE_ZONE_PARAMETER_ARM_DISARM_CODE:
            if(*pu8ParameterLength < pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].sArmDisarmCode.u8Length)
            {
                //Release Mutex
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif
    
                return E_ZCL_ERR_PARAMETER_RANGE;
            }
            *pu8ParameterLength = pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].sArmDisarmCode.u8Length;
            memcpy(pu8ParameterValue,pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].au8ArmDisarmCode,*pu8ParameterLength);
        break;
        
        default:
            //Release Mutex
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif
    
            return E_ZCL_ERR_ATTRIBUTE_NOT_FOUND;
        break;           
    }
    
    //Release Mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif
    
    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACESetZoneParameterValue
 **
 ** DESCRIPTION:
 ** Allows to set zone Parameter for Config flag, status flag ,zone status & audible notification
 **
 ** PARAMETERS:                     Name                                    Usage
 ** uint8                           u8SourceEndPointId                      Endpoint id
 ** teCLD_IASACE_ZoneParameterID    eParameterId                            ID of the zone parameter to be written
 ** uint8                           u8ZoneID                                Zone ID
 ** uint16                          u16ParameterValue                       Parameter Value 
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACESetZoneParameterValue (
            uint8                                       u8SourceEndPointId,
            teCLD_IASACE_ZoneParameterID                eParameterId,
            uint8                                       u8ZoneID,
            uint16                                      u16ParameterValue)
{
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_IASACECustomDataStructure *pCustomDataStructure;
    tsCLD_IASACE_ZoneTable *psZoneTable;
    tsZCL_Address sZCL_Address = {0};
    uint8 u8TransactionSeqNumber = 0;
    tsCLD_IASACE_ZoneStatusChangedPayload sPayload;

    if( eCLD_IASACEGetZoneTableEntry(u8SourceEndPointId,u8ZoneID,&psZoneTable) != E_ZCL_CMDS_SUCCESS)
    {
        return E_ZCL_ERR_NO_REPORT_ENTRIES;
    }
    
    /* Find pointers to cluster */
    if(eZCL_FindCluster(SECURITY_AND_SAFETY_CLUSTER_ID_IASACE, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_ERR_CLUSTER_NOT_FOUND;
    }
    
    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    
    switch(eParameterId)
    {
        case E_CLD_IASACE_ZONE_PARAMETER_ZONE_CONFIG_FLAG:
            pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].u8ZoneConfigFlag = u16ParameterValue;
        break;
        
        case E_CLD_IASACE_ZONE_PARAMETER_ZONE_STATUS_FLAG:
            pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].u8ZoneStatusFlag = u16ParameterValue;
        break;
        
        case E_CLD_IASACE_ZONE_PARAMETER_ZONE_STATUS:
            pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].eZoneStatus = u16ParameterValue;
            sZCL_Address.eAddressMode = E_ZCL_AM_BOUND;
            sPayload.u8ZoneID = u8ZoneID;
            sPayload.eZoneStatus = pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].eZoneStatus;
            sPayload.eAudibleNotification = pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].eAudibleNotification;
            sPayload.sZoneLabel = pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].sZoneLabel;
            eCLD_IASACE_ZoneStatusChangedSend (
                                        u8SourceEndPointId,
                                        0,
                                        &sZCL_Address,
                                        &u8TransactionSeqNumber,
                                        &sPayload);
        break;
        
        case E_CLD_IASACE_ZONE_PARAMETER_AUDIBLE_NOTIFICATION:
            pCustomDataStructure->asCLD_IASACE_ZoneParameter[u8ZoneID].eAudibleNotification = u16ParameterValue;
        break;
            
        default:
            //Release Mutex
            #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif
          
            return E_ZCL_ERR_ATTRIBUTE_NOT_FOUND;
        break;           
    }
    
    //Release Mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif
        
    return E_ZCL_SUCCESS;
}
#endif /*#define IASACE_SERVER */
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/


#endif /*#define  CLD_IASACE */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

