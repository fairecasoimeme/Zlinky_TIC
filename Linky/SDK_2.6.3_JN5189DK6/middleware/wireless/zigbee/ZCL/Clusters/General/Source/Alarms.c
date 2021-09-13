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
 * COMPONENT:          Alarms.c
 *
 * DESCRIPTION:        Alarms cluster definition
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
#include "Alarms.h"
#include "Alarms_internal.h"

#include "dbg.h"

#ifdef DEBUG_CLD_ALARMS
#define TRACE_ALARMS    TRUE
#else
#define TRACE_ALARMS    FALSE
#endif


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
    const tsZCL_CommandDefinition asCLD_AlarmClusterCommandDefinitions[] = {
        {E_CLD_ALARMS_CMD_RESET_ALARM,           E_ZCL_CF_RX},     /* Mandatory */
        {E_CLD_ALARMS_CMD_RESET_ALL_ALARMS,      E_ZCL_CF_RX},     /* Mandatory */
        {E_CLD_ALARMS_CMD_GET_ALARM,             E_ZCL_CF_RX},     /* Mandatory */    
        {E_CLD_ALARMS_CMD_RESET_ALARM_LOG,       E_ZCL_CF_RX}, 
        
        {E_CLD_ALARMS_CMD_ALARM,                 E_ZCL_CF_TX},     /* Mandatory */
        {E_CLD_ALARMS_CMD_GET_ALARM_RESPONSE,    E_ZCL_CF_TX},     /* Mandatory */    
    };
#endif
const tsZCL_AttributeDefinition asCLD_AlarmsClusterAttributeDefinitions[] = {
#ifdef ALARMS_SERVER
    #ifdef CLD_ALARMS_ATTR_ALARM_COUNT
        {E_CLD_ALARMS_ATTR_ID_ALARM_COUNT,          E_ZCL_AF_RD,                E_ZCL_UINT16,  (uint32)(&((tsCLD_Alarms*)(0))->u16AlarmCount),0},   /* Optional  */
    #endif
#endif
    {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,              (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_BMAP32,   (uint32)(&((tsCLD_Alarms*)(0))->u32FeatureMap),0},   /* Mandatory  */        

    {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,         (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_UINT16,  (uint32)(&((tsCLD_Alarms*)(0))->u16ClusterRevision),0},   /* Mandatory  */

};

tsZCL_ClusterDefinition sCLD_Alarms = {
        GENERAL_CLUSTER_ID_ALARMS,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_AlarmsClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_AlarmsClusterAttributeDefinitions,
        NULL
        #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED        
            ,
            (sizeof(asCLD_AlarmClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
            (tsZCL_CommandDefinition*)asCLD_AlarmClusterCommandDefinitions         
        #endif        
};

uint8 au8AlarmsAttributeControlBits[(sizeof(asCLD_AlarmsClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_IdentifyCreateIdentify
 **
 ** DESCRIPTION:
 ** Creates an identify cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teCLD_IdentifyStatus
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_AlarmsCreateAlarms(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits,
                tsCLD_AlarmsCustomDataStructure    *psCustomDataStructure)
{

    #ifdef ALARMS_SERVER
        int n;
    #endif  
    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if((psClusterInstance==NULL) || (psCustomDataStructure==NULL))
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
               eCLD_AlarmsCommandHandler);

    psCustomDataStructure->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterDefinition->u16ClusterEnum;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psCustomDataStructure->sCallBackMessage;
    psCustomDataStructure->sCustomCallBackEvent.psClusterInstance = psClusterInstance; 
    
    /* Initialise attributes */
    #ifdef ALARMS_SERVER
        #ifdef CLD_ALARMS_ATTR_ALARM_COUNT
            ((tsCLD_Alarms*)pvEndPointSharedStructPtr)->u16AlarmCount = 0;
        #endif

         /* initialise lists */
        vDLISTinitialise(&((tsCLD_AlarmsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr)->lAlarmsAllocList);
        vDLISTinitialise(&((tsCLD_AlarmsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr)->lAlarmsDeAllocList);

        for(n=0; n < CLD_ALARMS_MAX_NUMBER_OF_ALARMS; n++)
        {
            /* add all header slots to the to free list */
            vDLISTaddToTail(&psCustomDataStructure->lAlarmsDeAllocList, (DNODE *)&psCustomDataStructure->asAlarmsTableEntry[n]);
        }
    #endif
        
    ((tsCLD_Alarms*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_ALARMS_CLUSTER_REVISION;
    ((tsCLD_Alarms*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_ALARMS_FEATURE_MAP;
    
    //  register timer
    return E_ZCL_SUCCESS;

}

#ifdef ALARMS_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_AlarmsSignalAlarm
 **
 ** DESCRIPTION:
 ** Should be called by clusters wishing to signal an alarm condition
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** uint8                       u8AlarmCode                    Alarm Code
 ** uint16                      u16ClusterId                   Cluster Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_AlarmsSignalAlarm(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    uint8                       u8AlarmCode,
                    uint16                      u16ClusterId)
{
    tsZCL_ClusterInstance       *psClusterInstance;
    tsZCL_EndPointDefinition    *psEndPointDefinition;

    teZCL_Status eStatus;
    tsCLD_AlarmsAlarmCommandPayload sPayload;

    sPayload.u8AlarmCode = u8AlarmCode;
    sPayload.u16ClusterId = u16ClusterId;

    if((psDestinationAddress == NULL) || (pu8TransactionSequenceNumber == NULL))
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    /* Check end point is registered and cluster is present in the send device */
    eStatus = eZCL_SearchForEPentry(u8SourceEndPointId, &psEndPointDefinition);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return(eStatus);
    }

    eStatus = eZCL_SearchForClusterEntry(u8SourceEndPointId, GENERAL_CLUSTER_ID_ALARMS, TRUE, &psClusterInstance);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return(eStatus);
    }

    /* Add alarm to log */
    eStatus = eCLD_AlarmsAddAlarmToLog(psEndPointDefinition,
                                       psClusterInstance,
                                       u8AlarmCode,
                                       u16ClusterId);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ALARMS, "eCLD_AlarmsAddAlarmToLog Error: %d\r\n", eStatus);
        return eStatus;
    }

    /* Send alarm message */
    eStatus = eCLD_AlarmsCommandAlarmCommandSend(u8SourceEndPointId,
                                                 u8DestinationEndPointId,
                                                 psDestinationAddress,
                                                 pu8TransactionSequenceNumber,
                                                 &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ALARMS, "eCLD_AlarmsCommandAlarmCommandSend Error: %d\r\n", eStatus);
        return eStatus;
    }

    return E_ZCL_SUCCESS;

}
#endif
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

