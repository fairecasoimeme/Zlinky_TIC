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
 * MODULE:             Fan Control Cluster
 *
 * COMPONENT:          FanControl.c
 *
 * DESCRIPTION:        Fan Control Cluster Definition
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "FanControl.h"
#include "zcl_options.h"

#ifdef CLD_FAN_CONTROL

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
const tsZCL_AttributeDefinition asCLD_FanControlClusterAttributeDefinitions[] = {
#ifdef FAN_CONTROL_SERVER
    {E_CLD_FAN_CONTROL_ATTR_ID_FAN_MODE,          (E_ZCL_AF_RD|E_ZCL_AF_WR), E_ZCL_ENUM8,  (uint32)(&((tsCLD_FanControl*)(0))->e8FanMode), 0},          /* Mandatory */

    {E_CLD_FAN_CONTROL_ATTR_ID_FAN_MODE_SEQUENCE, (E_ZCL_AF_RD|E_ZCL_AF_WR), E_ZCL_ENUM8,  (uint32)(&((tsCLD_FanControl*)(0))->e8FanModeSequence), 0},  /* Mandatory */
#endif
    {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,       (E_ZCL_AF_RD|E_ZCL_AF_GA), E_ZCL_UINT16, (uint32)(&((tsCLD_FanControl*)(0))->u16ClusterRevision),0},   /* Mandatory  */
};

tsZCL_ClusterDefinition sCLD_FanControl = {
        HVAC_CLUSTER_ID_FAN_CONTROL,        // ClusterId
        FALSE,                              // bIsManufacturerSpecificCluster
        E_ZCL_SECURITY_NETWORK,             // Lower Nibble - SecurityInfo; Upper Nibble - ClusterType
        (sizeof(asCLD_FanControlClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),  // Number of Attributes
        (tsZCL_AttributeDefinition*)asCLD_FanControlClusterAttributeDefinitions,
        NULL                                // Pointer to SceneExtensionTable, if any
};

uint8 au8FanControlClusterAttributeControlBits[(sizeof(asCLD_FanControlClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eCLD_CreateFanControl
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
PUBLIC  teZCL_Status eCLD_CreateFanControl(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits)
{

    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if((psClusterInstance==NULL) ||
           (psClusterDefinition==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    /* Create an instance of a Fan Control cluster */
    vZCL_InitializeClusterInstance(
                                   psClusterInstance, 
                                   bIsServer,
                                   psClusterDefinition,
                                   pvEndPointSharedStructPtr,
                                   pu8AttributeControlBits,
                                   NULL,
                                   NULL);    

    /* Initialise attributes defaults */
    if(pvEndPointSharedStructPtr != NULL)
    {
        #ifdef FAN_CONTROL_SERVER  
            ((tsCLD_FanControl*)pvEndPointSharedStructPtr)->e8FanMode = 0x05;           // Auto as per 6.4.2.2
            ((tsCLD_FanControl*)pvEndPointSharedStructPtr)->e8FanModeSequence = 0x02;   // as per 6.4.2.2
        #endif

        ((tsCLD_FanControl*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_FAN_CONTROL_CLUSTER_REVISION;
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

