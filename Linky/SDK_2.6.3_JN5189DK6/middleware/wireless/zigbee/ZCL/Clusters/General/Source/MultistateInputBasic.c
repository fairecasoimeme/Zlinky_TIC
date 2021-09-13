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
 * MODULE:             Multistate Input Basic Cluster
 *
 * COMPONENT:          MultistateInputBasic.c
 *
 * DESCRIPTION:        Multistate Input Basic cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "zcl_customcommand.h"
#include "MultistateInputBasic.h"
#include "zcl_options.h"


#ifdef CLD_MULTISTATE_INPUT_BASIC

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#if defined(CLD_MULTISTATE_INPUT_BASIC) && !defined(MULTISTATE_INPUT_BASIC_SERVER) && !defined(MULTISTATE_INPUT_BASIC_CLIENT)
#error You Must Have either MULTISTATE_INPUT_BASIC_SERVER and/or MULTISTATE_INPUT_BASIC_CLIENT defined in zcl_options.h
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
#ifdef MULTISTATE_INPUT_BASIC_SERVER
    tsZCL_AttributeDefinition asCLD_MultistateInputBasicClusterAttributeDefinitions [] = {
            /* ZigBee Cluster Library Version */
        #ifdef CLD_MULTISTATE_INPUT_BASIC_ATTR_DESCRIPTION
            {E_CLD_MULTISTATE_INPUT_BASIC_ATTR_ID_DESCRIPTION,            (E_ZCL_AF_RD|E_ZCL_AF_WR),        E_ZCL_CSTRING,      (uint32)(&((tsCLD_MultistateInputBasic*)(0))->sDescription),          0},  /* Optional */
        #endif

            {E_CLD_MULTISTATE_INPUT_BASIC_ATTR_ID_NUMBER_OF_STATES,       (E_ZCL_AF_RD|E_ZCL_AF_WR),        E_ZCL_UINT16,       (uint32)(&((tsCLD_MultistateInputBasic*)(0))->u16NumberOfStates),    0}, /* Mandatory */

            {E_CLD_MULTISTATE_INPUT_BASIC_ATTR_ID_OUT_OF_SERVICE,         (E_ZCL_AF_RD|E_ZCL_AF_WR),        E_ZCL_BOOL,         (uint32)(&((tsCLD_MultistateInputBasic*)(0))->bOutOfService),         0},  /* Mandatory */

            {E_CLD_MULTISTATE_INPUT_BASIC_ATTR_ID_PRESENT_VALUE,          (E_ZCL_AF_RD|E_ZCL_AF_RP),        E_ZCL_UINT16,       (uint32)(&((tsCLD_MultistateInputBasic*)(0))->u16PresentValue),       0},  /* Mandatory */

        #ifdef CLD_MULTISTATE_INPUT_BASIC_ATTR_RELIABILITY
            {E_CLD_MULTISTATE_INPUT_BASIC_ATTR_ID_RELIABILITY,            (E_ZCL_AF_RD|E_ZCL_AF_WR),        E_ZCL_ENUM8,        (uint32)(&((tsCLD_MultistateInputBasic*)(0))->u8Reliability),         0},  /* Optional */
        #endif        

            {E_CLD_MULTISTATE_INPUT_BASIC_ATTR_ID_STATUS_FLAGS,           (E_ZCL_AF_RD|E_ZCL_AF_RP),        E_ZCL_BMAP8,        (uint32)(&((tsCLD_MultistateInputBasic*)(0))->u8StatusFlags),         0},  /* Mandatory */

        #ifdef CLD_MULTISTATE_INPUT_BASIC_ATTR_APPLICATION_TYPE
            {E_CLD_MULTISTATE_INPUT_BASIC_ATTR_ID_APPLICATION_TYPE,        E_ZCL_AF_RD,                     E_ZCL_UINT32,       (uint32)(&((tsCLD_MultistateInputBasic*)(0))->u32ApplicationType),   0}, /* Optional */
        #endif    

        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                                (E_ZCL_AF_RD|E_ZCL_AF_GA),        E_ZCL_BMAP32,       (uint32)(&((tsCLD_MultistateInputBasic*)(0))->u32FeatureMap),0},   /* Mandatory  */ 
            
        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                           (E_ZCL_AF_RD|E_ZCL_AF_GA),        E_ZCL_UINT16,       (uint32)(&((tsCLD_MultistateInputBasic*)(0))->u16ClusterRevision),     0},   /* Mandatory  */
        
        #ifdef CLD_MULTISTATE_INPUT_BASIC_ATTR_ATTRIBUTE_REPORTING_STATUS
            {E_CLD_GLOBAL_ATTR_ID_ATTRIBUTE_REPORTING_STATUS,             (E_ZCL_AF_RD|E_ZCL_AF_GA),        E_ZCL_ENUM8,        (uint32)(&((tsCLD_MultistateInputBasic*)(0))->u8AttributeReportingStatus),  0},  /* Optional */
        #endif 
    };

    tsZCL_ClusterDefinition sCLD_MultistateInputBasic = {
            GENERAL_CLUSTER_ID_MULTISTATE_INPUT_BASIC,
            FALSE,
            E_ZCL_SECURITY_NETWORK,
            (sizeof(asCLD_MultistateInputBasicClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
            (tsZCL_AttributeDefinition*)asCLD_MultistateInputBasicClusterAttributeDefinitions,
            NULL
    };
    uint8 au8MultistateInputBasicAttributeControlBits[(sizeof(asCLD_MultistateInputBasicClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
#endif
#ifdef MULTISTATE_INPUT_BASIC_CLIENT
    tsZCL_AttributeDefinition asCLD_MultistateInputBasicClientClusterAttributeDefinitions [] = {
            /* ZigBee Cluster Library Version */
      {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                              (E_ZCL_AF_RD|E_ZCL_AF_GA),     E_ZCL_BMAP32,       (uint32)(&((tsCLD_MultistateInputBasicClient*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

      {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                         (E_ZCL_AF_RD|E_ZCL_AF_GA),     E_ZCL_UINT16,       (uint32)(&((tsCLD_MultistateInputBasicClient*)(0))->u16ClusterRevision),     0},   /* Mandatory  */
    };

    tsZCL_ClusterDefinition sCLD_MultistateInputBasicClient = {
            GENERAL_CLUSTER_ID_MULTISTATE_INPUT_BASIC,
            FALSE,
            E_ZCL_SECURITY_NETWORK,
            (sizeof(asCLD_MultistateInputBasicClientClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
            (tsZCL_AttributeDefinition*)asCLD_MultistateInputBasicClientClusterAttributeDefinitions,
            NULL
    };
    uint8 au8MultistateInputBasicClientAttributeControlBits[(sizeof(asCLD_MultistateInputBasicClientClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
#endif
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME:       eCLD_MultistateInputBasicCreateMultistateInputBasic
 *
 * DESCRIPTION:
 * Creates a multistate input basic cluster
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
PUBLIC  teZCL_Status eCLD_MultistateInputBasicCreateMultistateInputBasic(
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
    

    /* Create an instance of a multistate input basic cluster */
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
        #ifdef MULTISTATE_INPUT_BASIC_SERVER
            #ifdef CLD_MULTISTATE_INPUT_BASIC_ATTR_DESCRIPTION
                ((tsCLD_MultistateInputBasic*)pvEndPointSharedStructPtr)->sDescription.u8MaxLength = sizeof(((tsCLD_MultistateInputBasic*)pvEndPointSharedStructPtr)->au8Description);
                ((tsCLD_MultistateInputBasic*)pvEndPointSharedStructPtr)->sDescription.u8Length = 0;
                ((tsCLD_MultistateInputBasic*)pvEndPointSharedStructPtr)->sDescription.pu8Data = ((tsCLD_MultistateInputBasic*)pvEndPointSharedStructPtr)->au8Description;
            #endif

            ((tsCLD_MultistateInputBasic*)pvEndPointSharedStructPtr)->u16NumberOfStates = CLD_MULTISTATE_INPUT_BASIC_ATTR_NUMBER_OF_STATES;
        
            ((tsCLD_MultistateInputBasic*)pvEndPointSharedStructPtr)->bOutOfService = FALSE;
    
            #ifdef CLD_MULTISTATE_INPUT_BASIC_ATTR_RELIABILITY
                ((tsCLD_MultistateInputBasic*)pvEndPointSharedStructPtr)->u8Reliability = 0; //E_CLD_MULTISTATE_INPUT_BASIC_RELIABILITY_NO_FAULT_DETECTED
            #endif

            ((tsCLD_MultistateInputBasic*)pvEndPointSharedStructPtr)->u8StatusFlags = 0;
            
            ((tsCLD_MultistateInputBasic*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_MULTISTATE_INPUT_BASIC_FEATURE_MAP;
            
            ((tsCLD_MultistateInputBasic*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_MULTISTATE_INPUT_BASIC_CLUSTER_REVISION;
        #endif
        }
        else
        {
        #ifdef MULTISTATE_INPUT_BASIC_CLIENT
          ((tsCLD_MultistateInputBasicClient*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_MULTISTATE_INPUT_BASIC_FEATURE_MAP;
          
          ((tsCLD_MultistateInputBasicClient*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_MULTISTATE_INPUT_BASIC_CLUSTER_REVISION;
        #endif
        }                
    }
    
    #ifdef MULTISTATE_INPUT_BASIC_SERVER
        if(bIsServer)
        {
            /* As this cluster has reportable attributes enable default reporting */
            vZCL_SetDefaultReporting(psClusterInstance);
        }
    #endif
    
    return E_ZCL_SUCCESS;

}

#ifdef MULTISTATE_INPUT_BASIC_SERVER

/****************************************************************************
 **
 **  NAME:       eCLD_MultistateInputBasicHandler
 **
 ** DESCRIPTION:
 ** Handles the dependency between Basic Multistate Input attributes
 **
 ** PARAMETERS          
 ** Type                    Name                         Usage
 ** uint8                   u8SourceEndPointId           Source Endpoint ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
 
PUBLIC  teZCL_Status eCLD_MultistateInputBasicHandler(
                                    uint8                       u8SourceEndPointId)
{

    teZCL_Status eStatus;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_MultistateInputBasic *psSharedStruct;
    tsZCL_AttributeDefinition *psAttributeDefinition;
    void *psCustomDataStructure;
    uint16  u16attributeIndex;
    
    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(GENERAL_CLUSTER_ID_MULTISTATE_INPUT_BASIC, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCustomDataStructure);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }

    /* Point to shared struct */
    psSharedStruct = (tsCLD_MultistateInputBasic *)psClusterInstance->pvEndPointSharedStructPtr;
    
    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    
    eStatus = eZCL_SearchForAttributeEntry(u8SourceEndPointId,
                        E_CLD_MULTISTATE_INPUT_BASIC_ATTR_ID_PRESENT_VALUE,
                        FALSE,
                        FALSE,
                        psClusterInstance,
                        &psAttributeDefinition,
                        &u16attributeIndex);

    if(psSharedStruct->bOutOfService)
    {
        psAttributeDefinition->u8AttributeFlags = (E_ZCL_AF_RD|E_ZCL_AF_WR);
        psSharedStruct->u8StatusFlags |= MULTISTATE_INPUT_BASIC_STATUS_FLAGS_OUT_OF_SERVICE_MASK;
    }else{
        psAttributeDefinition->u8AttributeFlags = E_ZCL_AF_RD;
        psSharedStruct->u8StatusFlags  &=  ~(MULTISTATE_INPUT_BASIC_STATUS_FLAGS_OUT_OF_SERVICE_MASK);
    }
#ifdef CLD_MULTISTATE_INPUT_BASIC_ATTR_RELIABILITY
    if(psSharedStruct->u8Reliability != E_CLD_MULTISTATE_INPUT_BASIC_RELIABILITY_NO_FAULT_DETECTED)
        psSharedStruct->u8StatusFlags  |= MULTISTATE_INPUT_BASIC_STATUS_FLAGS_FAULT_MASK;
    else
        psSharedStruct->u8StatusFlags  &= ~(MULTISTATE_INPUT_BASIC_STATUS_FLAGS_FAULT_MASK);
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

