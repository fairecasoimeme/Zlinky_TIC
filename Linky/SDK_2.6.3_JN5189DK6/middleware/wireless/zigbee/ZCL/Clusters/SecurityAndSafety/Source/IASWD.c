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
 * COMPONENT:          IASWD.c
 *
 * DESCRIPTION:        IAS WD cluster definition
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
#include "dbg.h"
#include "IASWD.h"
#include "IASWD_internal.h"

#ifdef DEBUG_CLD_IASWD
#define TRACE_IASWD    TRUE
#else
#define TRACE_IASWD    FALSE
#endif

#ifdef CLD_IASWD

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if !defined(IASWD_SERVER) && !defined(IASWD_CLIENT)
#error You Must Have either IASWD_SERVER and/or IASWD_CLIENT defined in zcl_options.h
#endif

#ifndef CLD_IASWD_ATTR_ID_MAX_DURATION
#define CLD_IASWD_ATTR_ID_MAX_DURATION 240
#endif

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
    const tsZCL_CommandDefinition asCLD_IASWDClusterCommandDefinitions[] = {

        {E_CLD_IASWD_CMD_START_WARNING,                   E_ZCL_CF_RX},/* Mandatory */
        {E_CLD_IASWD_CMD_SQUAWK,                          E_ZCL_CF_RX} /* Mandatory */

    };
#endif

const tsZCL_AttributeDefinition asCLD_IASWDClusterAttributeDefinitions[] = {
#ifdef IASWD_SERVER    
        {E_CLD_IASWD_ATTR_ID_MAX_DURATION,     (E_ZCL_AF_RD|E_ZCL_AF_WR), E_ZCL_UINT16,    (uint32)(&((tsCLD_IASWD*)(0))->u16MaxDuration),0},       /* Mandatory */
#endif        
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,      (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_BMAP32,   (uint32)(&((tsCLD_IASWD*)(0))->u32FeatureMap),0},   /* Mandatory  */ 
        
        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,(E_ZCL_AF_RD|E_ZCL_AF_GA), E_ZCL_UINT16,    (uint32)(&((tsCLD_IASWD*)(0))->u16ClusterRevision),0},   /* Mandatory  */
    };

tsZCL_ClusterDefinition sCLD_IASWD = {
        SECURITY_AND_SAFETY_CLUSTER_ID_IASWD,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_IASWDClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_IASWDClusterAttributeDefinitions,
        NULL
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED        
        ,
        (sizeof(asCLD_IASWDClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
        (tsZCL_CommandDefinition*)asCLD_IASWDClusterCommandDefinitions 
#endif        
};
uint8 au8IASWDAttributeControlBits[(sizeof(asCLD_IASWDClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_IASWDCreateIASWD
 **
 ** DESCRIPTION:
 ** Creates a IAS Warning Device Cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASWDCreateIASWD(
        tsZCL_ClusterInstance                   *psClusterInstance,
        bool_t                                  bIsServer,
        tsZCL_ClusterDefinition                 *psClusterDefinition,
        void                                    *pvEndPointSharedStructPtr,
        uint8                                   *pu8AttributeControlBits,
        tsCLD_IASWD_CustomDataStructure         *psCustomDataStructure)
{

    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if((psClusterInstance==NULL) || (psCustomDataStructure==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif
    
    /* Zero everything */
    memset(pvEndPointSharedStructPtr, 0, sizeof(tsCLD_IASWD));
    memset(psCustomDataStructure, 0, sizeof(tsCLD_IASWD_CustomDataStructure));

    // cluster data
    vZCL_InitializeClusterInstance(
                                   psClusterInstance, 
                                   bIsServer,
                                   psClusterDefinition,
                                   pvEndPointSharedStructPtr,
                                   pu8AttributeControlBits,
                                   psCustomDataStructure,
                                   eCLD_IASWDCommandHandler);   
                                   
    psCustomDataStructure->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterDefinition->u16ClusterEnum;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psCustomDataStructure->sCallBackMessage;
    psCustomDataStructure->sCustomCallBackEvent.psClusterInstance = psClusterInstance; 
    if(pvEndPointSharedStructPtr != NULL)
    {
        #ifdef IASWD_SERVER
            /* Set Default Values */
            ((tsCLD_IASWD*)pvEndPointSharedStructPtr)->u16MaxDuration=CLD_IASWD_ATTR_ID_MAX_DURATION;
        #endif
            ((tsCLD_IASWD*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_IASWD_FEATURE_MAP;
            
            ((tsCLD_IASWD*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_IASWD_CLUSTER_REVISION;
    }
    return E_ZCL_SUCCESS;
}

#ifdef IASWD_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_IASWDUpdate
 **
 ** DESCRIPTION:
 ** Update function to be called at 100msec
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8               u8EndPointId             EndPoint Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASWDUpdate(uint8 u8SourceEndPointId)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;

    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_IASWD_CustomDataStructure *pCustomDataStructure;
    tsZCL_CallBackEvent sZCL_CallBackEvent;

    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(SECURITY_AND_SAFETY_CLUSTER_ID_IASWD, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }
    
    if( NULL == pCustomDataStructure)
    {
        return E_ZCL_ERR_CUSTOM_DATA_NULL;
    }
    
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    
    /*Subtract the time Loaded for Warning or Stobe at 50msec */
    uint8 u8WarningMode         =  pCustomDataStructure->sWarning.u8WarningModeStrobeAndSirenLevel >> 4;
    uint8 u8StrobeWithWarning   =  (pCustomDataStructure->sWarning.u8WarningModeStrobeAndSirenLevel & 0x0F) >> 2;
    uint8 u8SirenLevel          =  pCustomDataStructure->sWarning.u8WarningModeStrobeAndSirenLevel & 0x03;
    uint8 u8DutyCycle           =  pCustomDataStructure->sWarning.u8StrobeDutyCycle/10;

    
    /* Warning Mode is Set*/
    if(  u8WarningMode )
    {
        /*Warning is still on */
        if(pCustomDataStructure->u32WarningDurationRemainingIn100MS > 0)
        {
            tsCLD_IASWD_WarningUpdate sWarningUpdate;
            
            pCustomDataStructure->u32WarningDurationRemainingIn100MS -= 1;
            
            sWarningUpdate.u8WarningMode= u8WarningMode;
            sWarningUpdate.u8SirenLevel = u8SirenLevel;
            sWarningUpdate.u16WarningDurationRemaining = pCustomDataStructure->u32WarningDurationRemainingIn100MS/10;
            
            sWarningUpdate.u8StrobeWithWarning = u8StrobeWithWarning;
            
            if(u8StrobeWithWarning)
            {
                sWarningUpdate.eStrobeLevel =pCustomDataStructure->sWarning.eStrobeLevel;
                
                uint32 u32CurrentCycleRemainingTime = pCustomDataStructure->u32WarningDurationRemainingIn100MS - ((uint32)sWarningUpdate.u16WarningDurationRemaining*10) ;
                /*Reverse Logic*/
                if(u32CurrentCycleRemainingTime >= (10-u8DutyCycle))
                {
                    sWarningUpdate.bStrobe=TRUE;
                }
                else
                {
                    sWarningUpdate.bStrobe=FALSE;
                }
            }

            pCustomDataStructure->sCallBackMessage.uMessage.psWarningUpdate=&sWarningUpdate; 

            /* Generate a callback to let the user know that an update event occurred */        
    
            sZCL_CallBackEvent.u8EndPoint           = psEndPointDefinition->u8EndPointNumber;
            sZCL_CallBackEvent.psClusterInstance    = psClusterInstance;
            sZCL_CallBackEvent.pZPSevent            = NULL;
            sZCL_CallBackEvent.eEventType           = E_ZCL_CBET_CLUSTER_UPDATE;
            psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);
 
        }
        
    }

    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif
 
    return eStatus;
}
/****************************************************************************
 **
 ** NAME:       eCLD_IASWDUpdateMaxDuration
 **
 ** DESCRIPTION:
 ** Update the Max Duration attribute
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8               u8EndPointId             EndPoint Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASWDUpdateMaxDuration (uint8 u8SourceEndPointId, uint16 u16MaxDuration)
{
    teZCL_Status eStatus;
    
               
    eStatus = eZCL_WriteLocalAttributeValue(
                     u8SourceEndPointId,                  //uint8                      u8SrcEndpoint,
                     SECURITY_AND_SAFETY_CLUSTER_ID_IASWD, //uint16                     u16ClusterId,
                     TRUE,                                //bool_t                       bIsServerClusterInstance,
                     FALSE,                               //bool_t                       bManufacturerSpecific,
                     FALSE,                               //bool_t                     bIsClientAttribute,
                     E_CLD_IASWD_ATTR_ID_MAX_DURATION,    //uint16                     u16AttributeId,
                     &u16MaxDuration                      //void                      *pvAttributeValue
                     );

    return eStatus;
}
#endif
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

#endif          // ifdef CLD_IASWD

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

