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
 * COMPONENT:          ApplianceStatistics.c
 *
 * DESCRIPTION:        Appliance Statistics Cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "ApplianceStatistics.h"
#include "ApplianceStatistics_internal.h"
#include "zcl_options.h"

#include "zcl.h"
#include "string.h"
#include "dbg.h"

#ifdef DEBUG_CLD_APPLIANCE_STATISTICS
#define TRACE_APPLIANCE_STATISTICS    TRUE
#else
#define TRACE_APPLIANCE_STATISTICS    FALSE
#endif

#ifdef CLD_APPLIANCE_STATISTICS

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
    const tsZCL_CommandDefinition asCLD_ApplianceStatisticsClusterCommandDefinitions[] = {
        {E_CLD_APPLIANCE_STATISTICS_CMD_LOG_NOTIFICATION,               E_ZCL_CF_TX},     /* Mandatory */
        {E_CLD_APPLIANCE_STATISTICS_CMD_LOG_RESPONSE,                   E_ZCL_CF_TX},     /* Mandatory */
        {E_CLD_APPLIANCE_STATISTICS_CMD_LOG_QUEUE_RESPONSE,             E_ZCL_CF_TX},     /* Mandatory */    
        {E_CLD_APPLIANCE_STATISTICS_CMD_STATISTICS_AVAILABLE,           E_ZCL_CF_TX},     /* Mandatory */ 
        {E_CLD_APPLIANCE_STATISTICS_CMD_LOG_REQUEST,                    E_ZCL_CF_RX},
        {E_CLD_APPLIANCE_STATISTICS_CMD_LOG_QUEUE_REQUEST,              E_ZCL_CF_RX}
    };
#endif
const tsZCL_AttributeDefinition asCLD_ApplianceStatisticsClusterAttributeDefinitions[] = {
#ifdef APPLIANCE_STATISTICS_SERVER
        /* ZigBee Cluster Library Version */
        {E_CLD_APPLIANCE_STATISTICS_ATTR_ID_LOG_MAX_SIZE ,                          E_ZCL_AF_RD,                E_ZCL_UINT32,    (uint32)(&((tsCLD_ApplianceStatistics*)(0))->u32LogMaxSize),            0},  /* Mandatory */

        {E_CLD_APPLIANCE_STATISTICS_ATTR_ID_LOG_QUEUE_MAX_SIZE ,                    E_ZCL_AF_RD,                E_ZCL_UINT8,     (uint32)(&((tsCLD_ApplianceStatistics*)(0))->u8LogQueueMaxSize),        0},    /* Mandatory */
#endif        
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                                          (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_BMAP32,   (uint32)(&((tsCLD_ApplianceStatistics*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                                     (E_ZCL_AF_RD|E_ZCL_AF_GA),   E_ZCL_UINT16,     (uint32)(&((tsCLD_ApplianceStatistics*)(0))->u16ClusterRevision),      0},   /* Mandatory  */    
};

tsZCL_ClusterDefinition sCLD_ApplianceStatistics = {
        APPLIANCE_MANAGEMENT_CLUSTER_ID_APPLIANCE_STATISTICS,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_ApplianceStatisticsClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_ApplianceStatisticsClusterAttributeDefinitions,
        NULL
        #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED        
            ,
            (sizeof(asCLD_ApplianceStatisticsClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
            (tsZCL_CommandDefinition*)asCLD_ApplianceStatisticsClusterCommandDefinitions          
        #endif        
};
uint8 au8ApplianceStatisticsAttributeControlBits[(sizeof(asCLD_ApplianceStatisticsClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME:       eCLD_ApplianceStatisticsCreateApplianceStatistics
 *
 * DESCRIPTION:
 * Creates a basic cluster
 *
 * PARAMETERS:  Name                         Usage
 *              psClusterInstance            Cluster structure
 *              bIsServer                    Server/Client Flag
 *              psClusterDefinition          Cluster Definitation
 *              pvEndPointSharedStructPtr    EndPoint Shared Structure Pointer
 *              pu8AttributeControlBits      Attribute Control Bits
 * RETURN:
 * teZCL_Status
 *
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ApplianceStatisticsCreateApplianceStatistics(
                tsZCL_ClusterInstance                              *psClusterInstance,
                bool_t                                             bIsServer,
                tsZCL_ClusterDefinition                            *psClusterDefinition,
                void                                               *pvEndPointSharedStructPtr,
                uint8                                              *pu8AttributeControlBits,
                tsCLD_ApplianceStatisticsCustomDataStructure       *psCustomDataStructure)
{


    #ifdef    APPLIANCE_STATISTICS_SERVER    
        uint8 u8LogCount = 0; 
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


    /* Create an instance of Appliance Identification cluster */
    vZCL_InitializeClusterInstance(
                       psClusterInstance, 
                       bIsServer,
                       psClusterDefinition,
                       pvEndPointSharedStructPtr,
                       pu8AttributeControlBits,
                       psCustomDataStructure,
                       eCLD_ApplianceStatisticsCommandHandler);
                       
    psCustomDataStructure->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterDefinition->u16ClusterEnum;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psCustomDataStructure->sCallBackMessage;
    psCustomDataStructure->sCustomCallBackEvent.psClusterInstance = psClusterInstance; 
    
#ifdef  APPLIANCE_STATISTICS_SERVER    
    for(u8LogCount = 0 ; u8LogCount < CLD_APPLIANCE_STATISTICS_ATTR_LOG_QUEUE_MAX_SIZE; u8LogCount++)
    {
        psCustomDataStructure->asLogTable[u8LogCount].u8LogLength = (CLD_APPLIANCE_STATISTICS_ATTR_LOG_MAX_SIZE + 1);
    }
#endif   
    /* Initialise attributes defaults */
    if(pvEndPointSharedStructPtr != NULL)
    {
#ifdef  APPLIANCE_STATISTICS_SERVER        
        ((tsCLD_ApplianceStatistics*)pvEndPointSharedStructPtr)->u32LogMaxSize = CLD_APPLIANCE_STATISTICS_ATTR_LOG_MAX_SIZE;
        ((tsCLD_ApplianceStatistics*)pvEndPointSharedStructPtr)->u8LogQueueMaxSize = CLD_APPLIANCE_STATISTICS_ATTR_LOG_QUEUE_MAX_SIZE;
#endif         
        ((tsCLD_ApplianceStatistics*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_APPLIANCE_STATISTICS_FEATURE_MAP;
        
        ((tsCLD_ApplianceStatistics*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_APPLIANCE_STATISTICS_CLUSTER_REVISION;
    }
   
   return E_ZCL_SUCCESS;
}

#ifdef    APPLIANCE_STATISTICS_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_ASCAddLog
 **
 ** DESCRIPTION:
 ** Adds a Log
 **
 ** PARAMETERS:                 Name                    Usage
 ** uint8                       u8SourceEndPointId      Endpoint id
 ** uint32                      u32LogId                  Log Id
 ** uint8                       u8LogLength               Log Length
 ** uint8 *                     pu8LogData                Log Data
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_CommandStatus eCLD_ASCAddLog(uint8 u8SourceEndPointId,
                                                   uint32                     u32LogId,
                                                   uint8                      u8LogLength,
                                                   uint32                     u32Time,
                                                   uint8                      *pu8LogData)
{
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_ApplianceStatisticsCustomDataStructure *pCustomDataStructure;
    uint8 u8LogCount;
    tsCLD_LogTable *psLogTable;
    tsZCL_Address sZCL_Address = {0};
    uint8 u8TransactionSeqNumber = 0;
    tsCLD_ASC_LogNotificationORLogResponsePayload     sPayload;

    if(pu8LogData == NULL)
    {
        return E_ZCL_CMDS_INVALID_FIELD;
    }
    
    /* Find pointers to cluster */
    if(eZCL_FindCluster(APPLIANCE_MANAGEMENT_CLUSTER_ID_APPLIANCE_STATISTICS, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_CMDS_FAILURE;
    }
    
    if(u8LogLength    > CLD_APPLIANCE_STATISTICS_ATTR_LOG_MAX_SIZE)
    {
        return E_ZCL_CMDS_INVALID_VALUE;
    }

    if(eCLD_ASCGetLogEntry(u8SourceEndPointId,u32LogId,&psLogTable) == E_ZCL_CMDS_SUCCESS)
    {   
        psLogTable->utctTime = u32Time;
        psLogTable->u8LogLength = u8LogLength;
        psLogTable->pu8LogData = pu8LogData;
        return E_ZCL_CMDS_SUCCESS;
    }

    for( u8LogCount = 0 ; u8LogCount < CLD_APPLIANCE_STATISTICS_ATTR_LOG_QUEUE_MAX_SIZE; u8LogCount++)
    {
        if( pCustomDataStructure->asLogTable[u8LogCount].u8LogLength > CLD_APPLIANCE_STATISTICS_ATTR_LOG_MAX_SIZE )
            {
                pCustomDataStructure->asLogTable[u8LogCount].u32LogID = u32LogId;
                pCustomDataStructure->asLogTable[u8LogCount].u8LogLength = u8LogLength;
                pCustomDataStructure->asLogTable[u8LogCount].utctTime = u32Time;
                pCustomDataStructure->asLogTable[u8LogCount].pu8LogData = pu8LogData;
                memcpy(&sPayload,(tsCLD_LogTable *)&pCustomDataStructure->asLogTable[u8LogCount],sizeof(tsCLD_LogTable));
                sZCL_Address.eAddressMode = E_ZCL_AM_BOUND;
                eCLD_ASCLogNotificationSend(                
                                            u8SourceEndPointId,
                                            0,
                                            &sZCL_Address,
                                            &u8TransactionSeqNumber,
                                            &sPayload);
                return E_ZCL_CMDS_SUCCESS;
            }
    }

    return E_ZCL_CMDS_INSUFFICIENT_SPACE;

}

/****************************************************************************
 **
 ** NAME:       eCLD_ASCRemoveLog
 **
 ** DESCRIPTION:
 ** removes a Log
 **
 ** PARAMETERS:                 Name                    Usage
 ** uint8                       u8SourceEndPointId      Endpoint id
 ** uint32                      u32LogId                  Log Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_CommandStatus eCLD_ASCRemoveLog(uint8 u8SourceEndPointId,
                                                   uint32 u32LogId)
{
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_ApplianceStatisticsCustomDataStructure *pCustomDataStructure;
    uint8 u8LogCount;

    /* Find pointers to cluster */
    if(eZCL_FindCluster(APPLIANCE_MANAGEMENT_CLUSTER_ID_APPLIANCE_STATISTICS, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_CMDS_FAILURE;
    }

    for( u8LogCount = 0 ; u8LogCount < CLD_APPLIANCE_STATISTICS_ATTR_LOG_QUEUE_MAX_SIZE; u8LogCount++)
    {
        if( (pCustomDataStructure->asLogTable[u8LogCount].u32LogID == u32LogId) && (pCustomDataStructure->asLogTable[u8LogCount].u8LogLength <= CLD_APPLIANCE_STATISTICS_ATTR_LOG_MAX_SIZE))
            {
                pCustomDataStructure->asLogTable[u8LogCount].u8LogLength = (CLD_APPLIANCE_STATISTICS_ATTR_LOG_MAX_SIZE + 1);
                return E_ZCL_CMDS_SUCCESS;
            }
    }

    return E_ZCL_CMDS_NOT_FOUND;

}

/****************************************************************************
 **
 ** NAME:       eCLD_ASCGetLogEntry
 **
 ** DESCRIPTION:
 ** Gives the log entry
 **
 ** PARAMETERS:                 Name                    Usage
 ** uint8                       u8SourceEndPointId      Endpoint id
 ** uint32                      u32LogId                Log Id
 ** tsCLD_LogTable**            ppsLogTable             Pointer to Pointer to log entry
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_CommandStatus eCLD_ASCGetLogEntry(
                uint8                                             u8SourceEndPointId,
                uint32                                            u32LogId,
                tsCLD_LogTable                                    **ppsLogTable)
{
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_ApplianceStatisticsCustomDataStructure *pCustomDataStructure;
    uint8 u8LogCount;

    /* Find pointers to cluster */
    if(eZCL_FindCluster(APPLIANCE_MANAGEMENT_CLUSTER_ID_APPLIANCE_STATISTICS, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_CMDS_FAILURE;
    }

    for( u8LogCount = 0 ; u8LogCount < CLD_APPLIANCE_STATISTICS_ATTR_LOG_QUEUE_MAX_SIZE; u8LogCount++)
    {
        if( pCustomDataStructure->asLogTable[u8LogCount].u32LogID == u32LogId  && (pCustomDataStructure->asLogTable[u8LogCount].u8LogLength <= CLD_APPLIANCE_STATISTICS_ATTR_LOG_MAX_SIZE))
            {
                 *ppsLogTable = (tsCLD_LogTable *)(&(pCustomDataStructure->asLogTable[u8LogCount]));
                 DBG_vPrintf(TRACE_APPLIANCE_STATISTICS, "ADDR = %016lx = %016lx\n",ppsLogTable,(*ppsLogTable));
                 return E_ZCL_CMDS_SUCCESS;
            }
    }

    return E_ZCL_CMDS_NOT_FOUND;
}
/****************************************************************************
 **
 ** NAME:       eCLD_ASCGetLogsAvailable
 **
 ** DESCRIPTION:
 ** Check all the logs available
 **
 ** PARAMETERS:                 Name                    Usage
 ** uint8                       u8SourceEndPointId      Endpoint id
 ** uint32*                     pu32LogId                  Pointer to Log Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_CommandStatus eCLD_ASCGetLogsAvailable(uint8 u8SourceEndPointId,
                                                   uint32*     pu32LogId,
                                                   uint8*      pu8LogIdCount)
{
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_ApplianceStatisticsCustomDataStructure *pCustomDataStructure;
    uint8 u8LogCount,u8LogAvailbleCount = 0;

    /* Find pointers to cluster */
    if(eZCL_FindCluster(APPLIANCE_MANAGEMENT_CLUSTER_ID_APPLIANCE_STATISTICS, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_CMDS_FAILURE;
    }

    if(pu32LogId == NULL || pu8LogIdCount == NULL)
    {
        return E_ZCL_CMDS_INVALID_FIELD;
    }
    
    for( u8LogCount = 0 ; u8LogCount < CLD_APPLIANCE_STATISTICS_ATTR_LOG_QUEUE_MAX_SIZE; u8LogCount++)
    {
        if( pCustomDataStructure->asLogTable[u8LogCount].u8LogLength <= CLD_APPLIANCE_STATISTICS_ATTR_LOG_MAX_SIZE )
            {
                pu32LogId[u8LogAvailbleCount] = pCustomDataStructure->asLogTable[u8LogCount].u32LogID;
                (u8LogAvailbleCount)++;
                
                if(u8LogAvailbleCount >= (*pu8LogIdCount))
                {
                    *pu8LogIdCount = u8LogAvailbleCount;
                    return E_ZCL_CMDS_INSUFFICIENT_SPACE;
                }
            }
    }
    
    *pu8LogIdCount = u8LogAvailbleCount;
    return E_ZCL_CMDS_SUCCESS;

}
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
#endif                    
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

