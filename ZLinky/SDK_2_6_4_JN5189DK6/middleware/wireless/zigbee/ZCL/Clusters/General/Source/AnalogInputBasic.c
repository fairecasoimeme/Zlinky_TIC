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
 * MODULE:             Analog Input Basic Cluster
 *
 * COMPONENT:          AnalogInputBasic.c
 *
 * DESCRIPTION:        Analog Inpunt Basic cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "zcl_customcommand.h"
#include "AnalogInputBasic.h"
#include "zcl_options.h"


#ifdef CLD_ANALOG_INPUT_BASIC

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
#ifdef ANALOG_INPUT_BASIC_SERVER
    tsZCL_AttributeDefinition asCLD_AnalogInputBasicClusterAttributeDefinitions [] = {
            /* ZigBee Cluster Library Version */
        #ifdef CLD_ANALOG_INPUT_BASIC_ATTR_DESCRIPTION
            {E_CLD_ANALOG_INPUT_BASIC_ATTR_ID_DESCRIPTION,            (E_ZCL_AF_RD|E_ZCL_AF_WR),                   E_ZCL_CSTRING,           (uint32)(&((tsCLD_AnalogInputBasic*)(0))->sDescription),              0},  /* Optional */
        #endif

        #ifdef CLD_ANALOG_INPUT_BASIC_ATTR_MAX_PRESENT_VALUE
            {E_CLD_ANALOG_INPUT_BASIC_ATTR_ID_MAX_PRESENT_VALUE,      (E_ZCL_AF_RD|E_ZCL_AF_WR),                   E_ZCL_FLOAT_SINGLE,      (uint32)(&((tsCLD_AnalogInputBasic*)(0))->fMaxPresentValue),          0},  /* Optional */
        #endif

        #ifdef CLD_ANALOG_INPUT_BASIC_ATTR_MIN_PRESENT_VALUE
            {E_CLD_ANALOG_INPUT_BASIC_ATTR_ID_MIN_PRESENT_VALUE,      (E_ZCL_AF_RD|E_ZCL_AF_WR),                   E_ZCL_FLOAT_SINGLE,      (uint32)(&((tsCLD_AnalogInputBasic*)(0))->fMinPresentValue),          0},  /* Optional */
        #endif

            {E_CLD_ANALOG_INPUT_BASIC_ATTR_ID_OUT_OF_SERVICE,         (E_ZCL_AF_RD|E_ZCL_AF_WR),                   E_ZCL_BOOL,              (uint32)(&((tsCLD_AnalogInputBasic*)(0))->bOutOfService),             0},  /* Mandatory */
            
            {E_CLD_ANALOG_INPUT_BASIC_ATTR_ID_PRESENT_VALUE,          (E_ZCL_AF_RD|E_ZCL_AF_WR|E_ZCL_AF_RP),       E_ZCL_FLOAT_SINGLE,      (uint32)(&((tsCLD_AnalogInputBasic*)(0))->fPresentValue),             0},  /* Mandatory */

        #ifdef CLD_ANALOG_INPUT_BASIC_ATTR_RELIABILITY
            {E_CLD_ANALOG_INPUT_BASIC_ATTR_ID_RELIABILITY,            (E_ZCL_AF_RD|E_ZCL_AF_WR),                   E_ZCL_ENUM8,             (uint32)(&((tsCLD_AnalogInputBasic*)(0))->u8Reliability),             0},  /* Optional */
        #endif

        #ifdef CLD_ANALOG_INPUT_BASIC_ATTR_RESOLUTION
            {E_CLD_ANALOG_INPUT_BASIC_ATTR_ID_RESOLUTION,             (E_ZCL_AF_RD|E_ZCL_AF_WR),                   E_ZCL_FLOAT_SINGLE,      (uint32)(&((tsCLD_AnalogInputBasic*)(0))->fResolution),               0},  /* Optional */
        #endif

            {E_CLD_ANALOG_INPUT_BASIC_ATTR_ID_STATUS_FLAGS,           (E_ZCL_AF_RD|E_ZCL_AF_RP),                   E_ZCL_BMAP8,             (uint32)(&((tsCLD_AnalogInputBasic*)(0))->u8StatusFlags),             0},  /* Mandatory */

        #ifdef CLD_ANALOG_INPUT_BASIC_ATTR_ENGINEERING_UNITS
            {E_CLD_ANALOG_INPUT_BASIC_ATTR_ID_ENGINEERING_UNITS,      (E_ZCL_AF_RD|E_ZCL_AF_WR),                   E_ZCL_ENUM16,            (uint32)(&((tsCLD_AnalogInputBasic*)(0))->u16EngineeringUnits),       0}, /* Optional */
        #endif

        #ifdef CLD_ANALOG_INPUT_BASIC_ATTR_APPLICATION_TYPE
            {E_CLD_ANALOG_INPUT_BASIC_ATTR_ID_APPLICATION_TYPE,        E_ZCL_AF_RD,                                E_ZCL_UINT32,            (uint32)(&((tsCLD_AnalogInputBasic*)(0))->u32ApplicationType),         0}, /* Optional */
        #endif    
          {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                        (E_ZCL_AF_RD|E_ZCL_AF_GA),                  E_ZCL_BMAP32,            (uint32)(&((tsCLD_AnalogInputBasic*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                   (E_ZCL_AF_RD|E_ZCL_AF_GA),                  E_ZCL_UINT16,            (uint32)(&((tsCLD_AnalogInputBasic*)(0))->u16ClusterRevision),         0},   /* Mandatory  */
            
        #ifdef CLD_ANALOG_INPUT_BASIC_ATTR_ATTRIBUTE_REPORTING_STATUS
            {E_CLD_GLOBAL_ATTR_ID_ATTRIBUTE_REPORTING_STATUS,         (E_ZCL_AF_RD|E_ZCL_AF_GA),                E_ZCL_ENUM8,             (uint32)(&((tsCLD_AnalogInputBasic*)(0))->u8AttributeReportingStatus), 0}  /* Optional */
        #endif
    };

    tsZCL_ClusterDefinition sCLD_AnalogInputBasic = {
            GENERAL_CLUSTER_ID_ANALOG_INPUT_BASIC,
            FALSE,
            E_ZCL_SECURITY_NETWORK,
            (sizeof(asCLD_AnalogInputBasicClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
            (tsZCL_AttributeDefinition*)asCLD_AnalogInputBasicClusterAttributeDefinitions,
            NULL
    };
    uint8 au8AnalogInputBasicAttributeControlBits[(sizeof(asCLD_AnalogInputBasicClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
#endif

#ifdef ANALOG_INPUT_BASIC_CLIENT
    tsZCL_AttributeDefinition asCLD_AnalogInputBasicClientClusterAttributeDefinitions [] = {
            {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                        (E_ZCL_AF_RD|E_ZCL_AF_GA),                  E_ZCL_BMAP32,            (uint32)(&((tsCLD_AnalogInputBasicClient*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

            {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                   (E_ZCL_AF_RD|E_ZCL_AF_GA),                  E_ZCL_UINT16,            (uint32)(&((tsCLD_AnalogInputBasicClient*)(0))->u16ClusterRevision),         0},   /* Mandatory  */
    };

    tsZCL_ClusterDefinition sCLD_AnalogInputBasicClient = {
            GENERAL_CLUSTER_ID_ANALOG_INPUT_BASIC,
            FALSE,
            E_ZCL_SECURITY_NETWORK,
            (sizeof(asCLD_AnalogInputBasicClientClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
            (tsZCL_AttributeDefinition*)asCLD_AnalogInputBasicClientClusterAttributeDefinitions,
            NULL
    };
    uint8 au8AnalogInputBasicClientAttributeControlBits[(sizeof(asCLD_AnalogInputBasicClientClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
#endif


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME:       eCLD_AnalogInputBasicCreateAnalogInputBasic
 *
 * DESCRIPTION:
 * Creates a Analog input basic cluster
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
PUBLIC  teZCL_Status eCLD_AnalogInputBasicCreateAnalogInputBasic(
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
    

    /* Create an instance of a analog input basic cluster */
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
            #ifdef ANALOG_INPUT_BASIC_SERVER
                #ifdef CLD_ANALOG_INPUT_BASIC_ATTR_DESCRIPTION
                    ((tsCLD_AnalogInputBasic*)pvEndPointSharedStructPtr)->sDescription.u8MaxLength = sizeof(((tsCLD_AnalogInputBasic*)pvEndPointSharedStructPtr)->au8Description);
                    ((tsCLD_AnalogInputBasic*)pvEndPointSharedStructPtr)->sDescription.u8Length = 0;
                    ((tsCLD_AnalogInputBasic*)pvEndPointSharedStructPtr)->sDescription.pu8Data = ((tsCLD_AnalogInputBasic*)pvEndPointSharedStructPtr)->au8Description;
                #endif

                ((tsCLD_AnalogInputBasic*)pvEndPointSharedStructPtr)->bOutOfService = FALSE;

            #ifdef CLD_ANALOG_INPUT_BASIC_ATTR_RELIABILITY
                    ((tsCLD_AnalogInputBasic*)pvEndPointSharedStructPtr)->u8Reliability = 0; //E_CLD_ANALOG_INPUT_BASIC_RELIABILITY_NO_FAULT_DETECTED
            #endif

            ((tsCLD_AnalogInputBasic*)pvEndPointSharedStructPtr)->u8StatusFlags = 0;

            ((tsCLD_AnalogInputBasic*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_ANALOG_INPUT_BASIC_FEATURE_MAP;
            
            ((tsCLD_AnalogInputBasic*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_ANALOG_INPUT_BASIC_CLUSTER_REVISION;
            #endif
            }
            else
            {
            #ifdef ANALOG_INPUT_BASIC_CLIENT
              ((tsCLD_AnalogInputBasicClient*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_ANALOG_INPUT_BASIC_FEATURE_MAP; 
              ((tsCLD_AnalogInputBasicClient*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_ANALOG_INPUT_BASIC_CLUSTER_REVISION; 
            #endif
            }
        }

    #ifdef ANALOG_INPUT_BASIC_SERVER
    if(bIsServer)
    {
        /* As this cluster has reportable attributes enable default reporting */
        vZCL_SetDefaultReporting(psClusterInstance);
    }
    #endif
    return E_ZCL_SUCCESS;

}

#ifdef ANALOG_INPUT_BASIC_SERVER

/****************************************************************************
 **
 **  NAME:       eCLD_AnalogInputBasicHandler
 **
 ** DESCRIPTION:
 ** Handles the dependency between Basic Analog Input attributes
 **
 ** PARAMETERS          
 ** Type                    Name                         Usage
 ** uint8                   u8SourceEndPointId           Source Endpoint ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
 
PUBLIC  teZCL_Status eCLD_AnalogInputBasicHandler(
                                    uint8                       u8SourceEndPointId)
{

    teZCL_Status eStatus;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_AnalogInputBasic *psSharedStruct;
    void *psCustomDataStructure;
    
    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(GENERAL_CLUSTER_ID_ANALOG_INPUT_BASIC, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCustomDataStructure);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }

    /* Point to shared struct */
    psSharedStruct = (tsCLD_AnalogInputBasic *)psClusterInstance->pvEndPointSharedStructPtr;
    
    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    if(psSharedStruct->bOutOfService)
    {
        psSharedStruct->u8StatusFlags |= ANALOG_INPUT_BASIC_STATUS_FLAGS_OUT_OF_SERVICE_MASK;
    }else{
        psSharedStruct->u8StatusFlags  &=  ~(ANALOG_INPUT_BASIC_STATUS_FLAGS_OUT_OF_SERVICE_MASK);
    }
#ifdef CLD_ANALOG_INPUT_BASIC_ATTR_RELIABILITY
    if(psSharedStruct->u8Reliability != E_CLD_ANALOG_INPUT_BASIC_RELIABILITY_NO_FAULT_DETECTED)
        psSharedStruct->u8StatusFlags  |= ANALOG_INPUT_BASIC_STATUS_FLAGS_FAULT_MASK;
    else
        psSharedStruct->u8StatusFlags  &= ~(ANALOG_INPUT_BASIC_STATUS_FLAGS_FAULT_MASK);
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

