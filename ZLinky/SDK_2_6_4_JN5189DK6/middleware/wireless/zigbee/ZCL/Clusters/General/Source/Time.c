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
 * MODULE:             Time Cluster
 *
 * COMPONENT:          Time.c
 *
 * DESCRIPTION:        Time cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "Time.h"
#include "zcl_options.h"

#ifdef CLD_TIME

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
const tsZCL_AttributeDefinition asCLD_TimeClusterAttributeDefinitions[] = {
#ifdef TIME_SERVER
        {E_CLD_TIME_ATTR_ID_TIME,           (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_UTCT,     (uint32)(&((tsCLD_Time*)(0))->utctTime),0},     /* Mandatory */

        {E_CLD_TIME_ATTR_ID_TIME_STATUS,    (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_BMAP8,    (uint32)(&((tsCLD_Time*)(0))->u8TimeStatus),0}, /* Mandatory */

    #ifdef CLD_TIME_ATTR_TIME_ZONE
        {E_CLD_TIME_ATTR_ID_TIME_ZONE,      (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_INT32,    (uint32)(&((tsCLD_Time*)(0))->i32TimeZone),0},
    #endif

    #ifdef CLD_TIME_ATTR_DST_START
        {E_CLD_TIME_ATTR_ID_DST_START,      (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_UINT32,   (uint32)(&((tsCLD_Time*)(0))->u32DstStart),0},
    #endif

    #ifdef CLD_TIME_ATTR_DST_END
        {E_CLD_TIME_ATTR_ID_DST_END,        (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_UINT32,   (uint32)(&((tsCLD_Time*)(0))->u32DstEnd),0},
    #endif

    #ifdef CLD_TIME_ATTR_DST_SHIFT
        {E_CLD_TIME_ATTR_ID_DST_SHIFT,      (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_INT32,    (uint32)(&((tsCLD_Time*)(0))->i32DstShift),0},
    #endif

    #ifdef CLD_TIME_ATTR_STANDARD_TIME
        {E_CLD_TIME_ATTR_ID_STANDARD_TIME,  (E_ZCL_AF_RD),  E_ZCL_UINT32,   (uint32)(&((tsCLD_Time*)(0))->u32StandardTime),0},
    #endif

    #ifdef CLD_TIME_ATTR_LOCAL_TIME
        {E_CLD_TIME_ATTR_ID_LOCAL_TIME,     (E_ZCL_AF_RD),  E_ZCL_UINT32,   (uint32)(&((tsCLD_Time*)(0))->u32LocalTime),0},
    #endif

    #ifdef CLD_TIME_ATTR_LAST_SET_TIME
        {E_CLD_TIME_ATTR_ID_LAST_SET_TIME,     (E_ZCL_AF_RD),  E_ZCL_UTCT,   (uint32)(&((tsCLD_Time*)(0))->u32LastSetTime),0},
    #endif

    #ifdef CLD_TIME_ATTR_VALID_UNTIL_TIME
        {E_CLD_TIME_ATTR_ID_VALID_UNTIL_TIME,     (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_UTCT,   (uint32)(&((tsCLD_Time*)(0))->u32ValidUntilTime),0},
    #endif
#endif    
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,        (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_BMAP32,   (uint32)(&((tsCLD_Time*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,   (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_UINT16,     (uint32)(&((tsCLD_Time*)(0))->u16ClusterRevision),0},   /* Mandatory  */

    };

tsZCL_ClusterDefinition sCLD_Time = {
        GENERAL_CLUSTER_ID_TIME,
        FALSE,
#ifndef TIME_USE_NWK_SECURITY
        E_ZCL_SECURITY_APPLINK,
#else
        E_ZCL_SECURITY_NETWORK,
#endif
        (sizeof(asCLD_TimeClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_TimeClusterAttributeDefinitions,
        NULL
};

uint8 au8TimeClusterAttributeControlBits[(sizeof(asCLD_TimeClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eCLD_TimeCreateTime
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
PUBLIC  teZCL_Status eCLD_TimeCreateTime(
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

    /* Create an instance of a time cluster */
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
#ifdef TIME_SERVER        
        #ifdef CLD_TIME_ATTR_LAST_SET_TIME
            ((tsCLD_Time*)pvEndPointSharedStructPtr)->u32LastSetTime = 0xFFFFFFFFUL;
        #endif

        #ifdef CLD_TIME_ATTR_VALID_UNTIL_TIME
            ((tsCLD_Time*)pvEndPointSharedStructPtr)->u32ValidUntilTime = 0xFFFFFFFFUL;
        #endif
#endif        
        ((tsCLD_Time*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_TIME_FEATURE_MAP;
            
        ((tsCLD_Time*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_TIME_CLUSTER_REVISION;
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

