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
 * MODULE:             Binary Output Basic Cluster
 *
 * COMPONENT:          BinaryOutputBasic.c
 *
 * DESCRIPTION:        Binary Inpunt Basic cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "zcl_customcommand.h"
#include "BinaryOutputBasic.h"
#include "zcl_options.h"


#ifdef CLD_BINARY_OUTPUT_BASIC

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#if defined(CLD_BINARY_OUTPUT_BASIC) && !defined(BINARY_OUTPUT_BASIC_SERVER) && !defined(BINARY_OUTPUT_BASIC_CLIENT)
#error You Must Have either BINARY_OUTPUT_BASIC_SERVER and/or BINARY_OUTPUT_BASIC_CLIENT defined in zcl_options.h
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
#ifdef BINARY_OUTPUT_BASIC_SERVER
    tsZCL_AttributeDefinition asCLD_BinaryOutputBasicClusterAttributeDefinitions [] = {
            /* ZigBee Cluster Library Version */
        #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_ACTIVE_TEXT
            {E_CLD_BINARY_OUTPUT_BASIC_ATTR_ID_ACTIVE_TEXT,            (E_ZCL_AF_RD|E_ZCL_AF_WR),               E_ZCL_CSTRING,        (uint32)(&((tsCLD_BinaryOutputBasic*)(0))->sActiveText),          0},  /* Optional */
        #endif

        #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_DESCRIPTION
            {E_CLD_BINARY_OUTPUT_BASIC_ATTR_ID_DESCRIPTION,            (E_ZCL_AF_RD|E_ZCL_AF_WR),               E_ZCL_CSTRING,        (uint32)(&((tsCLD_BinaryOutputBasic*)(0))->sDescription),       0},  /* Optional */
        #endif

        #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_INACTIVE_TEXT
            {E_CLD_BINARY_OUTPUT_BASIC_ATTR_ID_INACTIVE_TEXT,          (E_ZCL_AF_RD|E_ZCL_AF_WR),               E_ZCL_CSTRING,        (uint32)(&((tsCLD_BinaryOutputBasic*)(0))->sInactiveText),       0},  /* Optional */
        #endif

        #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_MINIMUM_OFF_TIME
            {E_CLD_BINARY_OUTPUT_BASIC_ATTR_ID_MINIMUM_OFF_TIME,       (E_ZCL_AF_RD|E_ZCL_AF_WR),               E_ZCL_UINT32,        (uint32)(&((tsCLD_BinaryOutputBasic*)(0))->u32MinimumOffTime),     0}, /* Optional */
        #endif  

        #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_MINIMUM_ON_TIME
            {E_CLD_BINARY_OUTPUT_BASIC_ATTR_ID_MINIMUM_ON_TIME,        (E_ZCL_AF_RD|E_ZCL_AF_WR),               E_ZCL_UINT32,        (uint32)(&((tsCLD_BinaryOutputBasic*)(0))->u32MinimumOnTime),      0}, /* Optional */
        #endif    
        
            {E_CLD_BINARY_OUTPUT_BASIC_ATTR_ID_OUT_OF_SERVICE,         (E_ZCL_AF_RD|E_ZCL_AF_WR),               E_ZCL_BOOL,            (uint32)(&((tsCLD_BinaryOutputBasic*)(0))->bOutOfService),        0},  /* Mandatory */

        #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_POLARITY
            {E_CLD_BINARY_OUTPUT_BASIC_ATTR_ID_POLARITY,               E_ZCL_AF_RD,                             E_ZCL_ENUM8,        (uint32)(&((tsCLD_BinaryOutputBasic*)(0))->u8Polarity),            0},  /* Optional */
        #endif        

            {E_CLD_BINARY_OUTPUT_BASIC_ATTR_ID_PRESENT_VALUE,          (E_ZCL_AF_RD|E_ZCL_AF_WR|E_ZCL_AF_RP),   E_ZCL_BOOL,         (uint32)(&((tsCLD_BinaryOutputBasic*)(0))->bPresentValue),         0},  /* Mandatory */

        #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_RELIABILITY
            {E_CLD_BINARY_OUTPUT_BASIC_ATTR_ID_RELIABILITY,            (E_ZCL_AF_RD|E_ZCL_AF_WR),               E_ZCL_ENUM8,        (uint32)(&((tsCLD_BinaryOutputBasic*)(0))->u8Reliability),        0},  /* Optional */
        #endif        

        #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_RELINQUISH_DEFAULT
            {E_CLD_BINARY_OUTPUT_BASIC_ATTR_ID_RELINQUISH_DEFAULT,     (E_ZCL_AF_RD|E_ZCL_AF_WR),               E_ZCL_BOOL,         (uint32)(&((tsCLD_BinaryOutputBasic*)(0))->bRelinquishDefault),        0},  /* Optional */
        #endif
        
            {E_CLD_BINARY_OUTPUT_BASIC_ATTR_ID_STATUS_FLAGS,           (E_ZCL_AF_RD|E_ZCL_AF_RP),               E_ZCL_BMAP8,        (uint32)(&((tsCLD_BinaryOutputBasic*)(0))->u8StatusFlags),        0},  /* Mandatory */

        #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_APPLICATION_TYPE
            {E_CLD_BINARY_OUTPUT_BASIC_ATTR_ID_APPLICATION_TYPE,        E_ZCL_AF_RD,                            E_ZCL_UINT32,        (uint32)(&((tsCLD_BinaryOutputBasic*)(0))->u32ApplicationType),    0}, /* Optional */
        #endif   
            {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                          (E_ZCL_AF_RD|E_ZCL_AF_GA),              E_ZCL_BMAP32,        (uint32)(&((tsCLD_BinaryOutputBasic*)(0))->u32FeatureMap),0},   /* Mandatory  */ 
            
            {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                     (E_ZCL_AF_RD|E_ZCL_AF_GA),              E_ZCL_UINT16,        (uint32)(&((tsCLD_BinaryOutputBasic*)(0))->u16ClusterRevision),         0},   /* Mandatory  */
                
        #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_ATTRIBUTE_REPORTING_STATUS
            {E_CLD_GLOBAL_ATTR_ID_ATTRIBUTE_REPORTING_STATUS,           (E_ZCL_AF_RD|E_ZCL_AF_GA),            E_ZCL_ENUM8,         (uint32)(&((tsCLD_BinaryOutputBasic*)(0))->u8AttributeReportingStatus), 0},  /* Optional */
        #endif
    };

    tsZCL_ClusterDefinition sCLD_BinaryOutputBasic = {
            GENERAL_CLUSTER_ID_BINARY_OUTPUT_BASIC,
            FALSE,
            E_ZCL_SECURITY_NETWORK,
            (sizeof(asCLD_BinaryOutputBasicClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
            (tsZCL_AttributeDefinition*)asCLD_BinaryOutputBasicClusterAttributeDefinitions,
            NULL
    };
    uint8 au8BinaryOutputBasicAttributeControlBits[(sizeof(asCLD_BinaryOutputBasicClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
#endif

#ifdef BINARY_OUTPUT_BASIC_CLIENT
    tsZCL_AttributeDefinition asCLD_BinaryOutputBasicClientClusterAttributeDefinitions [] = {
         /* ZigBee Cluster Library Version */
      {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                          (E_ZCL_AF_RD|E_ZCL_AF_GA),              E_ZCL_BMAP32,        (uint32)(&((tsCLD_BinaryOutputBasicClient*)(0))->u32FeatureMap),0},   /* Mandatory  */ 
      
      {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                     (E_ZCL_AF_RD|E_ZCL_AF_GA),              E_ZCL_UINT16,        (uint32)(&((tsCLD_BinaryOutputBasicClient*)(0))->u16ClusterRevision),         0},   /* Mandatory  */
    };

    tsZCL_ClusterDefinition sCLD_BinaryOutputBasicClient = {
            GENERAL_CLUSTER_ID_BINARY_OUTPUT_BASIC,
            FALSE,
            E_ZCL_SECURITY_NETWORK,
            (sizeof(asCLD_BinaryOutputBasicClientClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
            (tsZCL_AttributeDefinition*)asCLD_BinaryOutputBasicClientClusterAttributeDefinitions,
            NULL
    };
    uint8 au8BinaryOutputBasicClientAttributeControlBits[(sizeof(asCLD_BinaryOutputBasicClientClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
#endif
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME:       eCLD_BinaryOutputBasicCreateBinaryOutputBasic
 *
 * DESCRIPTION:
 * Creates a binary output basic cluster
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
PUBLIC  teZCL_Status eCLD_BinaryOutputBasicCreateBinaryOutputBasic(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits)
{


    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if((psClusterInstance==NULL) ||
           (psClusterDefinition==NULL)  )
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif
    

    /* Create an instance of a bianry input basic cluster */
    vZCL_InitializeClusterInstance(
       psClusterInstance, 
       bIsServer,
       psClusterDefinition,
       pvEndPointSharedStructPtr,
       pu8AttributeControlBits,
       NULL,
       NULL);
       
        /* Initialise attributes defaults */
    if(psClusterInstance->pvEndPointSharedStructPtr != NULL)
    {
        if(bIsServer)
        {
        #ifdef BINARY_OUTPUT_BASIC_SERVER
            #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_ACTIVE_TEXT
                ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->sActiveText.u8MaxLength = sizeof(((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->au8ActiveText);
                ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->sActiveText.u8Length = 0;
                ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->sActiveText.pu8Data = ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->au8ActiveText;
            #endif

            #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_DESCRIPTION
                ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->sDescription.u8MaxLength = sizeof(((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->au8Description);
                ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->sDescription.u8Length = 0;
                ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->sDescription.pu8Data = ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->au8Description;
            #endif

            #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_INACTIVE_TEXT
                ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->sInactiveText.u8MaxLength = sizeof(((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->au8InactiveText);
                ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->sInactiveText.u8Length = 0;
                ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->sInactiveText.pu8Data = ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->au8InactiveText;
            #endif
    
            #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_MINIMUM_OFF_TIME
                ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->u32MinimumOffTime = 0xFFFFFFFF;
            #endif
    
            #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_MINIMUM_ON_TIME
                ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->u32MinimumOnTime = 0xFFFFFFFF;
            #endif
        
            ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->bOutOfService = FALSE;

            #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_POLARITY
                    ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->u8Polarity = 0; //E_CLD_BINARY_OUTPUT_BASIC_POLARITY_NORMAL
            #endif
                
            #ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_RELIABILITY
                    ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->u8Reliability = 0; //E_CLD_BINARY_OUTPUT_BASIC_RELIABILITY_NO_FAULT_DETECTED
            #endif

            ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->u8StatusFlags = 0;
            ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_BINARY_OUTPUT_BASIC_FEATURE_MAP;      
            ((tsCLD_BinaryOutputBasic*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_BINARY_OUTPUT_BASIC_CLUSTER_REVISION;
        #endif
        }
        else
        {
        #ifdef BINARY_OUTPUT_BASIC_CLIENT
            ((tsCLD_BinaryOutputBasicClient*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_BINARY_OUTPUT_BASIC_FEATURE_MAP;
            ((tsCLD_BinaryOutputBasicClient*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_BINARY_OUTPUT_BASIC_CLUSTER_REVISION;
        #endif
        }
    }

    #ifdef BINARY_OUTPUT_BASIC_SERVER
    if(bIsServer)
    {
        /* As this cluster has reportable attributes enable default reporting */
        vZCL_SetDefaultReporting(psClusterInstance);
    }
    #endif
    
    return E_ZCL_SUCCESS;

}

#ifdef BINARY_OUTPUT_BASIC_SERVER

/****************************************************************************
 **
 **  NAME:       eCLD_BinaryOutputBasicHandler
 **
 ** DESCRIPTION:
 ** Handles the dependency between Basic Binary Output attributes
 **
 ** PARAMETERS          
 ** Type                    Name                         Usage
 ** uint8                   u8SourceEndPointId           Source Endpoint ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
 
PUBLIC  teZCL_Status eCLD_BinaryOutputBasicHandler(
                                    uint8                       u8SourceEndPointId)
{

    teZCL_Status eStatus;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_BinaryOutputBasic *psSharedStruct;
    void *psCustomDataStructure;
    
    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(GENERAL_CLUSTER_ID_BINARY_OUTPUT_BASIC, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCustomDataStructure);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }

    /* Point to shared struct */
    psSharedStruct = (tsCLD_BinaryOutputBasic *)psClusterInstance->pvEndPointSharedStructPtr;
    
    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    if(psSharedStruct->bOutOfService)
    {
        psSharedStruct->u8StatusFlags |= BINARY_OUTPUT_BASIC_STATUS_FLAGS_OUT_OF_SERVICE_MASK;
    }else{
        psSharedStruct->u8StatusFlags  &=  ~(BINARY_OUTPUT_BASIC_STATUS_FLAGS_OUT_OF_SERVICE_MASK);
    }
#ifdef CLD_BINARY_OUTPUT_BASIC_ATTR_RELIABILITY
    if(psSharedStruct->u8Reliability != E_CLD_BINARY_OUTPUT_BASIC_RELIABILITY_NO_FAULT_DETECTED)
        psSharedStruct->u8StatusFlags  |= BINARY_OUTPUT_BASIC_STATUS_FLAGS_FAULT_MASK;
    else
        psSharedStruct->u8StatusFlags  &= ~(BINARY_OUTPUT_BASIC_STATUS_FLAGS_FAULT_MASK);
#endif    

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    
    return E_ZCL_SUCCESS;
}
#endif

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

