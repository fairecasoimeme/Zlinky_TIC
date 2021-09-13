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
 * COMPONENT:          ApplianceEventsAndAlerts.c
 *
 * DESCRIPTION:        Appliance Statistics Cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "ApplianceEventsAndAlerts.h"
#include "ApplianceEventsAndAlerts_internal.h"
#include "zcl_options.h"

#include "zcl.h"
#include "string.h"

#ifdef CLD_APPLIANCE_EVENTS_AND_ALERTS

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
    const tsZCL_CommandDefinition asCLD_ApplianceEventsAndAlertsClusterCommandDefinitions[] = {
        {E_CLD_APPLIANCE_EVENTS_AND_ALERTS_CMD_GET_ALERTS,                E_ZCL_CF_RX|E_ZCL_CF_TX},     /* Mandatory */
        {E_CLD_APPLIANCE_EVENTS_AND_ALERTS_CMD_ALERTS_NOTIFICATION,       E_ZCL_CF_TX},     /* Mandatory */
        {E_CLD_APPLIANCE_EVENTS_AND_ALERTS_CMD_EVENT_NOTIFICATION,        E_ZCL_CF_TX}
        
    };
#endif

const tsZCL_AttributeDefinition asCLD_ApplianceEventsAndAlertsClusterAttributeDefinitions[] = {
    {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                  (E_ZCL_AF_RD|E_ZCL_AF_GA),               E_ZCL_BMAP32,   (uint32)(&((tsCLD_ApplianceEventsAndAlerts*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

    {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,            (E_ZCL_AF_RD|E_ZCL_AF_GA),                E_ZCL_UINT16,     (uint32)(&((tsCLD_ApplianceEventsAndAlerts*)(0))->u16ClusterRevision),      0},   /* Mandatory  */    
};

tsZCL_ClusterDefinition sCLD_ApplianceEventsAndAlerts = {
        APPLIANCE_MANAGEMENT_CLUSTER_ID_APPLIANCE_EVENTS_AND_ALERTS,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_ApplianceEventsAndAlertsClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_ApplianceEventsAndAlertsClusterAttributeDefinitions,
        NULL
        #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
            ,
            (sizeof(asCLD_ApplianceEventsAndAlertsClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
            (tsZCL_CommandDefinition*)asCLD_ApplianceEventsAndAlertsClusterCommandDefinitions
        #endif
};
uint8 au8ApplianceEventsAndAlertsAttributeControlBits[(sizeof(asCLD_ApplianceEventsAndAlertsClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME:       eCLD_ApplianceEventsAndAlertsCreateApplianceEventsAndAlerts
 *
 * DESCRIPTION:
 * Creates a basic cluster
 *
 * PARAMETERS:  Name                         Usage
 *              psClusterInstance            Cluster structure
 *              bIsServer                    Server/Client Flag
 *              psClusterDefinition          Cluster Definitation
 *              pvEndPointSharedStructPtr    EndPoint Shared Structure Pointer
 *              pu8AttributeEventsAndAlertsBits      Attribute EventsAndAlerts Bits
 * RETURN:
 * teZCL_Status
 *
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ApplianceEventsAndAlertsCreateApplianceEventsAndAlerts(
                tsZCL_ClusterInstance                                  *psClusterInstance,
                bool_t                                                  bIsServer,
                tsZCL_ClusterDefinition                                *psClusterDefinition,
                void                                                   *pvEndPointSharedStructPtr,
                uint8                                                  *pu8AttributeEventsAndAlertsBits,
                tsCLD_ApplianceEventsAndAlertsCustomDataStructure      *psCustomDataStructure)
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
    
    /* Create an instance of an appliance events and alerts cluster */
    vZCL_InitializeClusterInstance(
                               psClusterInstance, 
                               bIsServer,
                               psClusterDefinition,
                               pvEndPointSharedStructPtr,
                               pu8AttributeEventsAndAlertsBits,
                               psCustomDataStructure,
                               eCLD_ApplianceEventsAndAlertsCommandHandler);
                     
    psCustomDataStructure->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterDefinition->u16ClusterEnum;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psCustomDataStructure->sCallBackMessage;
    psCustomDataStructure->sCustomCallBackEvent.psClusterInstance = psClusterInstance; 
    
    /* Initialise attributes defaults */
    if(pvEndPointSharedStructPtr != NULL)
    {
        ((tsCLD_ApplianceEventsAndAlerts*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_APPLIANCE_EVENTS_AND_ALERTS_FEATURE_MAP;
        
        ((tsCLD_ApplianceEventsAndAlerts*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_APPLIANCE_EVENTS_AND_ALERTS_CLUSTER_REVISION;
    }    
    
    return E_ZCL_SUCCESS;
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

