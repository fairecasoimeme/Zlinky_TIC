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
 * MODULE:             Flow Measurement Cluster
 *
 * COMPONENT:          FlowMeasurement.c
 *
 * DESCRIPTION:        Flow Measurement cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "FlowMeasurement.h"
#include "zcl_customcommand.h"
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

    const tsZCL_AttributeDefinition asCLD_FlowMeasurementClusterAttributeDefinitions[] = {
#ifdef FLOW_MEASUREMENT_SERVER
        {E_CLD_FLOWMEAS_ATTR_ID_MEASURED_VALUE,         (E_ZCL_AF_RD|E_ZCL_AF_RP),  E_ZCL_INT16,    (uint32)(&((tsCLD_FlowMeasurement*)(0))->i16MeasuredValue), 0},   /* Mandatory */

        {E_CLD_FLOWMEAS_ATTR_ID_MIN_MEASURED_VALUE,     E_ZCL_AF_RD,                E_ZCL_INT16,    (uint32)(&((tsCLD_FlowMeasurement*)(0))->i16MinMeasuredValue), 0},/* Mandatory */

        {E_CLD_FLOWMEAS_ATTR_ID_MAX_MEASURED_VALUE,     E_ZCL_AF_RD,                E_ZCL_INT16,    (uint32)(&((tsCLD_FlowMeasurement*)(0))->i16MaxMeasuredValue), 0},/* Mandatory */

        #ifdef CLD_FLOWMEAS_ATTR_TOLERANCE
            {E_CLD_FLOWMEAS_ATTR_ID_TOLERANCE,          E_ZCL_AF_RD,                E_ZCL_UINT16,   (uint32)(&((tsCLD_FlowMeasurement*)(0))->u16Tolerance), 0},
        #endif

#endif        
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,             (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_BMAP32,   (uint32)(&((tsCLD_FlowMeasurement*)(0))->u32FeatureMap),0},   /* Mandatory  */ 
            
        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,        (E_ZCL_AF_RD|E_ZCL_AF_GA), E_ZCL_UINT16,   (uint32)(&((tsCLD_FlowMeasurement*)(0))->u16ClusterRevision), 0},   /* Mandatory  */
            
        #if (defined FLOW_MEASUREMENT_SERVER) && (defined CLD_FLOWMEAS_ATTR_ATTRIBUTE_REPORTING_STATUS)
            {E_CLD_GLOBAL_ATTR_ID_ATTRIBUTE_REPORTING_STATUS,(E_ZCL_AF_RD|E_ZCL_AF_GA),E_ZCL_ENUM8,    (uint32)(&((tsCLD_FlowMeasurement*)(0))->u8AttributeReportingStatus), 0},  /* Optional */
        #endif   
        
};

tsZCL_ClusterDefinition sCLD_FlowMeasurement = {
        MEASUREMENT_AND_SENSING_CLUSTER_ID_FLOW_MEASUREMENT,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_FlowMeasurementClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_FlowMeasurementClusterAttributeDefinitions,
        NULL
};

uint8 au8FlowMeasurementAttributeControlBits[(sizeof(asCLD_FlowMeasurementClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eCLD_FlowMeasurementCreateFlowMeasurement
 **
 ** DESCRIPTION:
 ** Creates a flow measurement cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_FlowMeasurementCreateFlowMeasurement(
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

    /* Create an instance of a flow measurement cluster */
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
        ((tsCLD_FlowMeasurement*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_FLOWMEAS_FEATURE_MAP;
      
        ((tsCLD_FlowMeasurement*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_FLOWMEAS_CLUSTER_REVISION;
    }
 
    /* As this cluster has reportable attributes enable default reporting */
    vZCL_SetDefaultReporting(psClusterInstance);
    
    return E_ZCL_SUCCESS;

}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

