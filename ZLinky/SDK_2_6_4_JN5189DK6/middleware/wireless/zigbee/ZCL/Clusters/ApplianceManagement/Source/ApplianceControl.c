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
 * MODULE:             Appliance Control Cluster
 *
 * COMPONENT:          ApplianceControl.c
 *
 * DESCRIPTION:        Appliance Control Cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "ApplianceControl.h"
#include "ApplianceControl_internal.h"
#include "zcl_options.h"

#include "zcl.h"
#include "string.h"

#ifdef CLD_APPLIANCE_CONTROL

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
const tsZCL_CommandDefinition asCLD_ApplianceControlClusterCommandDefinitions[] = {
    {E_CLD_APPLIANCE_CONTROL_CMD_EXECUTION_OF_COMMAND,                E_ZCL_CF_RX},     /* Mandatory */
    {E_CLD_APPLIANCE_CONTROL_CMD_SIGNAL_STATE,                        E_ZCL_CF_RX},     /* Mandatory */
    {E_CLD_APPLIANCE_CONTROL_CMD_SIGNAL_STATE_RESPONSE,               E_ZCL_CF_TX},     /* Mandatory */
    {E_CLD_APPLIANCE_CONTROL_CMD_SIGNAL_STATE_NOTIFICATION,           E_ZCL_CF_TX}      /* Mandatory */

};
#endif

const tsZCL_AttributeDefinition asCLD_ApplianceControlClusterAttributeDefinitions[] = {
#ifdef APPLIANCE_CONTROL_SERVER
        /* ZigBee Cluster Library Version */
        {E_CLD_APPLIANCE_CONTROL_ATTR_ID_START_TIME ,                          (E_ZCL_AF_RD|E_ZCL_AF_RP),  E_ZCL_UINT16,    (uint32)(&((tsCLD_ApplianceControl*)(0))->u16StartTime),            0},  /* Mandatory */

        {E_CLD_APPLIANCE_CONTROL_ATTR_ID_FINISH_TIME ,                         (E_ZCL_AF_RD|E_ZCL_AF_RP),  E_ZCL_UINT16,     (uint32)(&((tsCLD_ApplianceControl*)(0))->u16FinishTime),          0},     /* Mandatory */
        
    #ifdef CLD_APPLIANCE_CONTROL_REMAINING_TIME
        {E_CLD_APPLIANCE_CONTROL_ATTR_ID_REMAINING_TIME ,                      (E_ZCL_AF_RD|E_ZCL_AF_RP),  E_ZCL_UINT16,     (uint32)(&((tsCLD_ApplianceControl*)(0))->u16RemainingTime),        0},     /* Mandatory */
    #endif

#endif
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                                      (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_BMAP32,   (uint32)(&((tsCLD_ApplianceControl*)(0))->u32FeatureMap),0},   /* Mandatory  */ 
    
        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                                 (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_UINT16,     (uint32)(&((tsCLD_ApplianceControl*)(0))->u16ClusterRevision),      0},   /* Mandatory  */    
    
    #if (defined APPLIANCE_CONTROL_SERVER) && (defined CLD_APPLIANCE_CONTROL_ATTRIBUTE_REPORTING_STATUS)
        {E_CLD_GLOBAL_ATTR_ID_ATTRIBUTE_REPORTING_STATUS,                       (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_ENUM8,     (uint32)(&((tsCLD_ApplianceControl*)(0))->u8AttributeReportingStatus), 0},  /* Optional */
    #endif
    };

tsZCL_ClusterDefinition sCLD_ApplianceControl = {
        APPLIANCE_MANAGEMENT_CLUSTER_ID_APPLIANCE_CONTROL,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_ApplianceControlClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_ApplianceControlClusterAttributeDefinitions,
        NULL
        #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
            ,
            (sizeof(asCLD_ApplianceControlClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
            (tsZCL_CommandDefinition*)asCLD_ApplianceControlClusterCommandDefinitions
        #endif
};
uint8 au8ApplianceControlAttributeControlBits[(sizeof(asCLD_ApplianceControlClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME:       eCLD_ApplianceControlCreateApplianceControl
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
PUBLIC  teZCL_Status eCLD_ApplianceControlCreateApplianceControl(
                tsZCL_ClusterInstance                              *psClusterInstance,
                bool_t                                             bIsServer,
                tsZCL_ClusterDefinition                            *psClusterDefinition,
                void                                               *pvEndPointSharedStructPtr,
                uint8                                              *pu8AttributeControlBits,
                tsCLD_ApplianceControlCustomDataStructure          *psCustomDataStructure)
{


    #ifdef STRICT_PARAM_CHECK    
        /* Parameter check */
        if((psClusterInstance==NULL) ||
           (psClusterDefinition==NULL) ||
           (psCustomDataStructure==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif
    


    /* Create an instance of a Appliance Control cluster */
    vZCL_InitializeClusterInstance(
                                   psClusterInstance, 
                                   bIsServer,
                                   psClusterDefinition,
                                   pvEndPointSharedStructPtr,
                                   pu8AttributeControlBits,
                                   psCustomDataStructure,
                                   eCLD_ApplianceControlCommandHandler);

    psCustomDataStructure->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterDefinition->u16ClusterEnum;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psCustomDataStructure->sCallBackMessage;
    psCustomDataStructure->sCustomCallBackEvent.psClusterInstance = psClusterInstance;   

    /* Initialise attributes defaults */
        if(pvEndPointSharedStructPtr != NULL)
        {
    #ifdef APPLIANCE_CONTROL_SERVER             
            ((tsCLD_ApplianceControl*)pvEndPointSharedStructPtr)->u16StartTime = 0;
            ((tsCLD_ApplianceControl*)pvEndPointSharedStructPtr)->u16FinishTime = 0;
            #ifdef CLD_APPLIANCE_CONTROL_REMAINING_TIME
                ((tsCLD_ApplianceControl*)pvEndPointSharedStructPtr)->u16RemainingTime = 0;
            #endif
    #endif            
            ((tsCLD_ApplianceControl*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_APPLIANCE_CONTROL_FEATURE_MAP;
                
            ((tsCLD_ApplianceControl*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_APPLIANCE_CONTROL_CLUSTER_REVISION;
        }    

    /* As this cluster has reportable attributes enable default reporting */
    vZCL_SetDefaultReporting(psClusterInstance);
    
    return E_ZCL_SUCCESS;

}

#ifdef APPLIANCE_CONTROL_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_ACChangeAttributeTime
 **
 ** DESCRIPTION:
 ** Adds a Log
 **
 ** PARAMETERS:                             Name                    Usage
 ** uint8                                   u8SourceEndPointId      Endpoint id
 ** teCLD_ApplianceControl_Cluster_AttrID   eAttributeTimeId         Time Attribute ID: start, finish and remaining
 ** uint16                                    u16TimeValue            Time value to be set
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ACChangeAttributeTime(
                                                    uint8                                         u8SourceEndPointId,
                                                    teCLD_ApplianceControl_Cluster_AttrID         eAttributeTimeId,
                                                    uint16                                         u16TimeValue)    
{
    teZCL_Status eStatus;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_ApplianceControlCustomDataStructure *pCustomDataStructure;
    tsCLD_ApplianceControl *psSharedStruct;
    tsZCL_CallBackEvent sZCL_CallBackEvent;

    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(APPLIANCE_MANAGEMENT_CLUSTER_ID_APPLIANCE_CONTROL, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }
    
    /* Point to shared struct */
    psSharedStruct = (tsCLD_ApplianceControl *)psClusterInstance->pvEndPointSharedStructPtr;
    
    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif
    
    switch(eAttributeTimeId)
    {
        case E_CLD_APPLIANCE_CONTROL_ATTR_ID_START_TIME:
            psSharedStruct->u16StartTime = u16TimeValue;
        break;
        case E_CLD_APPLIANCE_CONTROL_ATTR_ID_FINISH_TIME:
            psSharedStruct->u16StartTime = u16TimeValue;
        break;
#ifdef CLD_APPLIANCE_CONTROL_REMAINING_TIME
        case E_CLD_APPLIANCE_CONTROL_ATTR_ID_REMAINING_TIME:
            psSharedStruct->u16RemainingTime = u16TimeValue;
        break;
#endif
        default:
            // release EP
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif

            return E_ZCL_ERR_ATTRIBUTE_NOT_FOUND;
        break;
    }

        /* Generate a callback to let the user know that an update event occurred */
    sZCL_CallBackEvent.u8EndPoint           = psEndPointDefinition->u8EndPointNumber;
    sZCL_CallBackEvent.psClusterInstance    = psClusterInstance;
    sZCL_CallBackEvent.pZPSevent            = NULL;
    sZCL_CallBackEvent.eEventType           = E_ZCL_CBET_CLUSTER_UPDATE;
    psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return E_ZCL_SUCCESS;
}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
#endif                    
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

