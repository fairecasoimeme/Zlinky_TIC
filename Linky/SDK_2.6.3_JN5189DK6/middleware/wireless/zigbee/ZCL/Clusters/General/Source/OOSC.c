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
 * MODULE:             On/Off Switch Configuration Cluster
 *
 * COMPONENT:          OOSC.c
 *
 * DESCRIPTION:        On/Off switch configuration cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "OOSC.h"
#include "zcl_options.h"
#include "OOSC.h"


#ifdef CLD_OOSC

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
const tsZCL_AttributeDefinition asCLD_OOSCClusterAttributeDefinitions[] = {
#ifdef OOSC_SERVER
        {E_CLD_OOSC_ATTR_ID_SWITCH_TYPE,    E_ZCL_AF_RD,                E_ZCL_ENUM8,    (uint32)(&((tsCLD_OOSC*)(0))->eSwitchType),0},    /* Mandatory */

        {E_CLD_OOSC_ATTR_ID_SWITCH_ACTIONS, (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_ENUM8,    (uint32)(&((tsCLD_OOSC*)(0))->eSwitchActions),0}, /* Mandatory */
#endif        
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,      (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_BMAP32,   (uint32)(&((tsCLD_OOSC*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION, (E_ZCL_AF_RD|E_ZCL_AF_GA),E_ZCL_UINT16,   (uint32)(&((tsCLD_OOSC*)(0))->u16ClusterRevision),0},   /* Mandatory  */

    };
tsZCL_ClusterDefinition sCLD_OOSC = {
        GENERAL_CLUSTER_ID_ONOFF_SWITCH_CONFIGURATION,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_OOSCClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_OOSCClusterAttributeDefinitions,
        NULL
};
uint8 au8OOSCAttributeControlBits[(sizeof(asCLD_OOSCClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_OOSCCreateOnOffSwitchConfig
 **
 ** DESCRIPTION:
 ** Creates an on/off cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eCLD_OOSCCreateOnOffSwitchConfig(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits)
{

    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if(psClusterInstance==NULL)
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
                                   NULL,
                                   NULL);    

        if(pvEndPointSharedStructPtr != NULL)
        {
#ifdef OOSC_SERVER            
            /* Set attribute defaults */
            ((tsCLD_OOSC*)psClusterInstance->pvEndPointSharedStructPtr)->eSwitchActions = 0x00;
#endif            
            ((tsCLD_OOSC*)psClusterInstance->pvEndPointSharedStructPtr)->u32FeatureMap = CLD_OOSC_FEATURE_MAP;
            
            ((tsCLD_OOSC*)psClusterInstance->pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_OOSC_CLUSTER_REVISION;
        }

    //  register timer
    return E_ZCL_SUCCESS;

}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

