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
 * MODULE:             Smart Energy
 *
 * COMPONENT:          zcl_search.c
 *
 * DESCRIPTION:       ZCL search functions
 *
 ****************************************************************************/


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>

#include "zcl.h"
#include "zcl_common.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE bool_t bZCL_IsManufacturerCodeSupported(uint16 u16ManufacturerCode);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE bool_t (*prfnbZCLIsManufacturerCodeSupported)(uint16)  = NULL;
/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eZCL_SearchForEPentry
 **
 ** DESCRIPTION:
 ** Searches For An Endpoint In The ZCL
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8endpointId                EP Id
 ** tsZCL_EndPointDefinition  **ppsZCL_EndPointDefinition   EP definition structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_SearchForEPentry(
                        uint8                       u8endpointId,
                        tsZCL_EndPointDefinition  **ppsZCL_EndPointDefinition)
{

    uint8 u8EndPointIndex = 0;
    // noddy checks
    if(u8endpointId == 0)
    {
        return(E_ZCL_ERR_EP_RANGE);
    }

	#ifndef COOPERATIVE
    // get ZCL mutex
    vZCL_GetInternalMutex();
	#endif

    if((eZCL_SearchForEPIndex(u8endpointId,&u8EndPointIndex) != E_ZCL_SUCCESS) || 
        (psZCL_Common->psZCL_EndPointRecord[u8EndPointIndex].bRegistered == FALSE))
    {
	#ifndef COOPERATIVE
        vZCL_ReleaseInternalMutex();
	#endif
        return(E_ZCL_ERR_EP_UNKNOWN);
    }
    *ppsZCL_EndPointDefinition = psZCL_Common->psZCL_EndPointRecord[u8EndPointIndex].psEndPointDefinition;
    #ifndef COOPERATIVE
    vZCL_ReleaseInternalMutex();
	#endif

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eZCL_SearchForEPentryAndCheckManufacturerId
 **
 ** DESCRIPTION:
 ** Searches For An Endpoint In The ZCL
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8endpointId                EP Id
 ** bool_t                      bIsManufacturerSpecific     Attribute manufacturer specific
 ** uint16                      u16ManufacturerCode         Manufacturer code
 ** tsZCL_EndPointDefinition  **ppsZCL_EndPointDefinition   EP definition structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_SearchForEPentryAndCheckManufacturerId(
                        uint8                       u8endpointId,
                        bool_t                      bIsManufacturerSpecific,
                        uint16                      u16ManufacturerCode,
                        tsZCL_EndPointDefinition  **ppsZCL_EndPointDefinition)
{

    teZCL_Status eSearchForEPentry;

    eSearchForEPentry = eZCL_SearchForEPentry(u8endpointId, ppsZCL_EndPointDefinition);

    if((eSearchForEPentry == E_ZCL_SUCCESS) && (bIsManufacturerSpecific == TRUE))
    {
        if(((*ppsZCL_EndPointDefinition)->u16ManufacturerCode != u16ManufacturerCode) &&
            (bZCL_IsManufacturerCodeSupported(u16ManufacturerCode) == FALSE))
        {
            return(E_ZCL_FAIL);
        }
    }
    return(eSearchForEPentry);
}

/****************************************************************************
 **
 ** NAME:       eZCL_SearchForClusterEntry
 **
 ** DESCRIPTION:
 ** Searches For A Cluster Definition In The Endpoint.
 ** assumes EP has already been validiated
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8endpointId                EP Id
 ** uint16                      u16ClusterEnum              Cluster Id
 ** bool_t                      bDirection                  Client/Server
 ** tsZCL_ClusterInstance     **ppsClusterInstance      Cluster definition structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_SearchForClusterEntry(
                        uint8                       u8EndpointId,
                        uint16                      u16ClusterEnum,
                        bool_t                      bDirection,
                        tsZCL_ClusterInstance     **ppsClusterInstance)
{

    int i;
    uint8 u8EndPointIndex = 0;

    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    // noddy checks
    if((eZCL_SearchForEPIndex(u8EndpointId,&u8EndPointIndex) != E_ZCL_SUCCESS) || (u8EndpointId == 0))
    {
        return(E_ZCL_ERR_EP_RANGE);
    }

    psEndPointDefinition = psZCL_Common->psZCL_EndPointRecord[u8EndPointIndex].psEndPointDefinition;

    // EP has been listed in ZCL_Create but memory not allocated because register hasn't been called registered
    if (psEndPointDefinition == NULL)
    {
        return(E_ZCL_ERR_EP_RANGE);
    }

	#ifndef COOPERATIVE
    // get EP mutex
    vZCL_GetInternalMutex();
	#endif

    // first cluster definition
    psClusterInstance = psEndPointDefinition->psClusterInstance;

    for(i=0; i<psZCL_Common->psZCL_EndPointRecord[u8EndPointIndex].psEndPointDefinition->u16NumberOfClusters; i++)
    {
        if(psClusterInstance->psClusterDefinition->u16ClusterEnum == u16ClusterEnum)
        {
            //CLUSTER_MIRROR_BIT required for Mirrored clusters, as both client and server are handled as a single cluster
            //So no direction bit is checked, for eg. it is required for Mirroring in SE.
            if ((psClusterInstance->psClusterDefinition->u8ClusterControlFlags & CLUSTER_MIRROR_BIT) ||
                (psClusterInstance->bIsServer == bDirection))
            {
                *ppsClusterInstance = psClusterInstance;
				#ifndef COOPERATIVE
                vZCL_ReleaseInternalMutex();
				#endif
                return(E_ZCL_SUCCESS);
            }
        }
        psClusterInstance++;
    }

    // not found
    *ppsClusterInstance = NULL;
	#ifndef COOPERATIVE
    vZCL_ReleaseInternalMutex();
#endif
    return(E_ZCL_ERR_CLUSTER_NOT_FOUND);
}

/****************************************************************************
 **
 ** NAME:       eZCL_SearchForAttributeEntry
 **
 ** DESCRIPTION:
 ** Searches For An Attribute Definition In The Endpoint
 ** assumes EP has already been validiated
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8EndpointId                EP Id
 ** uint16                      u16AttributeEnum            Attribute Id
 ** bool_t                      bIsManufacturerSpecific     Attribute manufacturer specific
 ** tsZCL_ClusterInstance      *psClusterInstance           Cluster Structure
 ** tsZCL_AttributeDefinition **ppsAttributeDefinition      Attribute Structure
 ** uint16                      *pu16attributeIndex         Index of attribute definition
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC    teZCL_Status eZCL_SearchForAttributeEntry(
                        uint8                       u8EndpointId,
                        uint16                      u16AttributeEnum,
                        bool_t                      bIsManufacturerSpecific,
                        bool_t                      bIsClientAttribute,
                        tsZCL_ClusterInstance      *psClusterInstance,
                        tsZCL_AttributeDefinition **ppsAttributeDefinition,
                        uint16                     *pu16attributeIndex)
{

    int i;
    uint16 u16TempAttrId;
    tsZCL_AttributeDefinition  *psAttributeDefinition;

    // noddy checks
    if((u8EndpointId == 0) || (pu16attributeIndex == NULL))
    {
        return(E_ZCL_ERR_EP_RANGE);
    }

    if (psClusterInstance == NULL)
    {
        *ppsAttributeDefinition = NULL;
        *pu16attributeIndex = 0;
        return (E_ZCL_ERR_CLUSTER_NULL);
    }

	#ifndef COOPERATIVE
        vZCL_GetInternalMutex();
    #endif
    psAttributeDefinition = psClusterInstance->psClusterDefinition->psAttributeDefinition;
    u16TempAttrId = psAttributeDefinition->u16AttributeEnum;
    for(i=0; i<psClusterInstance->psClusterDefinition->u16NumberOfAttributes; i++)
    {
        if(((u16TempAttrId == u16AttributeEnum) &&
                    bZCL_CheckManufacturerSpecificAttributeFlagMatch(psAttributeDefinition, bIsManufacturerSpecific))&&
                    bZCL_CheckAttributeDirectionFlagMatch(psAttributeDefinition, !bIsClientAttribute))

        {
            *ppsAttributeDefinition = psAttributeDefinition;
			#ifndef COOPERATIVE
                vZCL_ReleaseInternalMutex();
			#endif
            *pu16attributeIndex = i;
            return(E_ZCL_SUCCESS);
        }
        if((psAttributeDefinition->u16AttributeArrayLength != 0)&&((u16TempAttrId  - psAttributeDefinition->u16AttributeEnum) < psAttributeDefinition->u16AttributeArrayLength))
        {
            u16TempAttrId++;
        }
        else
        {
            psAttributeDefinition++;
            u16TempAttrId = psAttributeDefinition->u16AttributeEnum;
        }
    }

    // not found
    *ppsAttributeDefinition = NULL;
    *pu16attributeIndex = 0;
	#ifndef COOPERATIVE
        vZCL_ReleaseInternalMutex();
    #endif
    return(E_ZCL_ERR_ATTRIBUTE_NOT_FOUND);
}

/****************************************************************************
 **
 ** NAME:       eZCL_GetLocalAttributeValue
 **
 ** DESCRIPTION:
 ** Returns a Local Attribute value
 **
 ** PARAMETERS:                 Name                    Usage
 ** uint16                      u16AttributeId          Attribute Id
 ** bool_t                      bManufacturerSpecific
 ** bool_t                        bIsClientAttribute      whether client or server attribute
 ** tsZCL_EndPointDefinition   *psEndPointDefinition    EP definition
 ** tsZCL_ClusterInstance      *psClusterInstance       Cluster Definition
 ** void                       *pvAttributeValue        Attribute Data Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC    teZCL_Status eZCL_GetLocalAttributeValue(
                    uint16                      u16AttributeId,
                    bool_t                      bManufacturerSpecific,
                    bool_t                      bIsClientAttribute,
                    tsZCL_EndPointDefinition   *psEndPointDefinition,
                    tsZCL_ClusterInstance      *psClusterInstance,
                    void                       *pvAttributeValue)
{

    //uint32 u32attributeOffset;
    uint16 u16attributeIndex;
    tsZCL_AttributeDefinition *psAttributeDefinition;

    teZCL_Status eZCL_SearchForAttributeEntryReturn;
    uint8 u8attributeSize;

    tsZCL_OctetString *psZCL_OctetString;
    tsZCL_LongOctetString *psZCL_LongOctetString;

    #ifdef STRICT_PARAM_CHECK
        if((psEndPointDefinition==NULL) || (psClusterInstance==NULL) || (pvAttributeValue==NULL))
        {
            return(E_ZCL_ERR_PARAMETER_NULL);
        }
    #endif
    
    #ifndef COOPERATIVE
        // get ZCL mutex
        vZCL_GetInternalMutex();
    #endif

    // check if attribute is present
    eZCL_SearchForAttributeEntryReturn = eZCL_SearchForAttributeEntry(
            psEndPointDefinition->u8EndPointNumber,
            u16AttributeId,
            bManufacturerSpecific,
            bIsClientAttribute,
            psClusterInstance,
            &psAttributeDefinition,
            &u16attributeIndex);
    if(eZCL_SearchForAttributeEntryReturn != E_ZCL_SUCCESS)
    {
	    #ifndef COOPERATIVE
        vZCL_ReleaseInternalMutex();
		#endif
        return(eZCL_SearchForAttributeEntryReturn);
    }
    // value - structure base address in cluster struct, offset in attribute structure
    //u32attributeOffset = psAttributeDefinition->u32OffsetFromStructBase;
    eZCL_GetAttributeTypeSize(psAttributeDefinition->eAttributeDataType, &u8attributeSize);

    /* check attribute type */
    /* character & octet strings should be handled separately */
    if(psAttributeDefinition->eAttributeDataType == E_ZCL_OSTRING ||
            psAttributeDefinition->eAttributeDataType == E_ZCL_CSTRING)
    {
        psZCL_OctetString = (tsZCL_OctetString*)pvZCL_GetAttributePointer(psAttributeDefinition, psClusterInstance, u16AttributeId);

        /* validate data pointer, correctly initialized or not */
        if(psZCL_OctetString->pu8Data == NULL || ((tsZCL_OctetString *)pvAttributeValue)->pu8Data == NULL)
        {
		#ifndef COOPERATIVE
            vZCL_ReleaseInternalMutex();
			#endif
            return(E_ZCL_ERR_PARAMETER_NULL);
        }

        ((tsZCL_OctetString *)pvAttributeValue)->u8Length = psZCL_OctetString->u8Length;
        memcpy(((tsZCL_OctetString *)pvAttributeValue)->pu8Data, psZCL_OctetString->pu8Data, psZCL_OctetString->u8Length);
    }
    else if(psAttributeDefinition->eAttributeDataType == E_ZCL_LOSTRING ||
            psAttributeDefinition->eAttributeDataType == E_ZCL_LCSTRING)
    {
        psZCL_LongOctetString = (tsZCL_LongOctetString*)pvZCL_GetAttributePointer(psAttributeDefinition, psClusterInstance, u16AttributeId);

        /* validate data pointer, correctly initialized or not */
        if(psZCL_LongOctetString->pu8Data == NULL || ((tsZCL_LongOctetString *)pvAttributeValue)->pu8Data == NULL)
        {
            #ifndef COOPERATIVE
                vZCL_ReleaseInternalMutex();
			#endif
            return(E_ZCL_ERR_PARAMETER_NULL);
        }

        ((tsZCL_LongOctetString *)pvAttributeValue)->u16Length = psZCL_LongOctetString->u16Length;
        memcpy(((tsZCL_LongOctetString *)pvAttributeValue)->pu8Data, psZCL_LongOctetString->pu8Data, psZCL_LongOctetString->u16Length);
    }
    else
    {
        memcpy((uint8 *)pvAttributeValue, (uint8 *)pvZCL_GetAttributePointer(psAttributeDefinition, psClusterInstance, u16AttributeId), u8ZCL_GetAttributeAllignToFourBytesBoundary(u8attributeSize));
    }
    #ifndef COOPERATIVE
        vZCL_ReleaseInternalMutex();
	#endif

    return(E_ZCL_SUCCESS);

}

/****************************************************************************
 **
 ** NAME:       eZCL_ReadLocalAttributeValue
 **
 ** DESCRIPTION:
 ** Reads Local Attribute value
 **
 ** PARAMETERS:                 Name                    Usage
 ** uint16                      u16ClusterId          Cluster Id
 ** uint16                      u16AttributeId        Attribute Id
 ** bool_t                      bManufacturerSpecific
 ** bool_t                      bIsClientAttribute     Whether client attribute
 ** void                        *pvAttributeValuen      Attribute Value
 ** bool_t                        bIsServerClusterInstance Cluster instance type
 ** uint8                       u8Endpoint              End Point Number
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  ZPS_teStatus eZCL_ReadLocalAttributeValue(
                                            uint8  u8SrcEndpoint,
                                            uint16 u16ClusterId,
											bool_t   bIsServerClusterInstance,
											bool_t   bManufacturerSpecific,
                                            bool_t bIsClientAttribute,
                                            uint16 u16AttributeId,
                                            void   *pvAttributeValue)
{
    teZCL_Status eZCL_Status;
    void *psCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    /* Find cluster Entry */
    // error check via EP number
    eZCL_Status = eZCL_FindCluster(u16ClusterId,
                                u8SrcEndpoint,
                                bIsServerClusterInstance,
                                &psEndPointDefinition,
                                &psClusterInstance,
                                &psCustomDataStructure);

    /* check cluster find status */
    if(eZCL_Status != E_ZCL_SUCCESS)
    {
        return eZCL_Status;
    }

    return eZCL_GetLocalAttributeValue(
                        u16AttributeId,
                        bManufacturerSpecific,
                        bIsClientAttribute,
                        psEndPointDefinition,
                        psClusterInstance,
                        pvAttributeValue);

}
/****************************************************************************
 **
 ** NAME:       eZCL_SetLocalAttributeValue
 **
 ** DESCRIPTION:
 ** Returns a Local Attribute value
 **
 ** PARAMETERS:                 Name                    Usage
 ** uint16                      u16AttributeId          Attribute Id
 ** bool_t                      bManufacturerSpecific
 ** tsZCL_EndPointDefinition   *psEndPointDefinition    EP definition
 ** tsZCL_ClusterInstance      *psClusterInstance       Cluster Definition
 ** void                       *pvAttributeValue        Attribute Data Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status    eZCL_SetLocalAttributeValue(
                    uint16                      u16AttributeId,
                    bool_t                      bManufacturerSpecific,
                    bool_t                      bIsClientAttribute,
                    tsZCL_EndPointDefinition   *psEndPointDefinition,
                    tsZCL_ClusterInstance      *psClusterInstance,
                    void                       *pvAttributeValue)
{

    ///uint32 u32attributeOffset;
    uint16 u16attributeIndex;
    tsZCL_AttributeDefinition *psAttributeDefinition;
    tsZCL_OctetString *psZCL_OctetString;
    tsZCL_LongOctetString *psZCL_LongOctetString;

    teZCL_Status eZCL_SearchForAttributeEntryReturn;
    uint8 u8attributeSize;

    #ifdef STRICT_PARAM_CHECK
        if((psEndPointDefinition==NULL) || (psClusterInstance==NULL) || (pvAttributeValue==NULL))
        {
            return(E_ZCL_ERR_PARAMETER_NULL);
        }
    #endif
    
    #ifndef COOPERATIVE
        // get ZCL mutex
        vZCL_GetInternalMutex();
    #endif
    // check if attribute is present
    eZCL_SearchForAttributeEntryReturn =
        eZCL_SearchForAttributeEntry(
            psEndPointDefinition->u8EndPointNumber,
            u16AttributeId,
            bManufacturerSpecific,
            bIsClientAttribute,
            psClusterInstance,
            &psAttributeDefinition,
            &u16attributeIndex);
    if(eZCL_SearchForAttributeEntryReturn != E_ZCL_SUCCESS)
    {
        #ifndef COOPERATIVE
            vZCL_ReleaseInternalMutex();
		#endif
        return(eZCL_SearchForAttributeEntryReturn);
    }
    // value - structure base address in cluster struct, offset in attribute structure
    //u32attributeOffset = psAttributeDefinition->u32OffsetFromStructBase;
    eZCL_GetAttributeTypeSize(psAttributeDefinition->eAttributeDataType, &u8attributeSize);

    /* check data type, if it is string handle differently */
    if(psAttributeDefinition->eAttributeDataType == E_ZCL_OSTRING ||
            psAttributeDefinition->eAttributeDataType == E_ZCL_CSTRING)
    {
        psZCL_OctetString = (tsZCL_OctetString*)pvZCL_GetAttributePointer(psAttributeDefinition, psClusterInstance, u16AttributeId);


        /* check length parameter & data pointer */
        if( (((tsZCL_OctetString*)pvAttributeValue)->u8Length > psZCL_OctetString->u8MaxLength)  ||
                (((tsZCL_OctetString*)pvAttributeValue)->pu8Data == NULL) ||
                psZCL_OctetString->pu8Data == NULL)
        {
            #ifndef COOPERATIVE
                vZCL_ReleaseInternalMutex();
			#endif
            return E_ZCL_ERR_PARAMETER_RANGE;
        }
        else
        {
            psZCL_OctetString->u8Length = ((tsZCL_OctetString*)pvAttributeValue)->u8Length;
            memcpy(psZCL_OctetString->pu8Data, ((tsZCL_OctetString*)pvAttributeValue)->pu8Data, psZCL_OctetString->u8Length);
        }

    }
    else if(psAttributeDefinition->eAttributeDataType == E_ZCL_LOSTRING ||
            psAttributeDefinition->eAttributeDataType == E_ZCL_LCSTRING)
    {
        psZCL_LongOctetString = (tsZCL_LongOctetString*)pvZCL_GetAttributePointer(psAttributeDefinition, psClusterInstance, u16AttributeId);

        /* check length parameter & data pointer */
        if( (((tsZCL_LongOctetString*)pvAttributeValue)->u16Length > psZCL_LongOctetString->u16MaxLength)  ||
                (((tsZCL_LongOctetString*)pvAttributeValue)->pu8Data == NULL) ||
                psZCL_LongOctetString->pu8Data == NULL)
        {
		#ifndef COOPERATIVE
            vZCL_ReleaseInternalMutex();
			#endif
            return E_ZCL_ERR_PARAMETER_RANGE;
        }
        else
        {
            psZCL_LongOctetString->u16Length = ((tsZCL_LongOctetString*)pvAttributeValue)->u16Length;
            memcpy(psZCL_LongOctetString->pu8Data, ((tsZCL_LongOctetString*)pvAttributeValue)->pu8Data, psZCL_LongOctetString->u16Length);
        }

    }
    else
    {
        memcpy((uint8 *)pvZCL_GetAttributePointer(psAttributeDefinition, psClusterInstance, u16AttributeId),(uint8 *)pvAttributeValue, u8ZCL_GetAttributeAllignToFourBytesBoundary(u8attributeSize));
    }
    #ifndef COOPERATIVE
        vZCL_ReleaseInternalMutex();
	#endif

    return(E_ZCL_SUCCESS);

}

/****************************************************************************
 **
 ** NAME:       eZCL_WriteLocalAttributeValue
 **
 ** DESCRIPTION:
 ** Sets Local Attribute value
 **
 ** PARAMETERS:                 Name                    Usage
 ** uint16                      u16ClusterId          Cluster Id
 ** uint16                      u16AttributeId        Attribute Id
 ** bool_t                      bManufacturerSpecific
 ** bool_t                      bIsClientAttribute     Whether client attribute
 ** void                        *pvAttributeValuen      Attribute Value
 ** bool_t                        bIsServerClusterInstance Cluster instance type
 ** uint8                       u8Endpoint              End Point Number
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  ZPS_teStatus eZCL_WriteLocalAttributeValue(
                    uint8                       u8SrcEndpoint,
                    uint16                      u16ClusterId,
					bool_t                        bIsServerClusterInstance,
					bool_t                        bManufacturerSpecific,
                    bool_t                      bIsClientAttribute,
                    uint16                      u16AttributeId,
                    void                       *pvAttributeValue)
{
    teZCL_Status eZCL_Status;
    void *psCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    /* Find cluster Entry */
    // error check via EP number
    eZCL_Status = eZCL_FindCluster(u16ClusterId,
                                u8SrcEndpoint,
                                bIsServerClusterInstance,
                                &psEndPointDefinition,
                                &psClusterInstance,
                                &psCustomDataStructure);

    /* check cluster find status */
    if(eZCL_Status != E_ZCL_SUCCESS)
    {
        return eZCL_Status;
    }

    return eZCL_SetLocalAttributeValue(
                        u16AttributeId,
                        bManufacturerSpecific,
                        bIsClientAttribute,
                        psEndPointDefinition,
                        psClusterInstance,
                        pvAttributeValue);

}

/****************************************************************************
 **
 ** NAME:       eZCL_FindCluster
 **
 ** DESCRIPTION:
 ** Finds all pointers related to a cluster on a given endpoint
 **
 ** PARAMETERS:                     Name                        Usage
 ** uint16                          u16ClusterId                Cluster Id
 ** uint8                           u8SourceEndPointId          Source EP Id
 ** bool_t                          bIsServer                   Is server
 ** tsZCL_EndPointDefinition      **ppsEndPointDefinition       EP
 ** tsZCL_ClusterInstance         **ppsClusterInstance          Cluster
 ** void                          **ppsCustomDataStructure      Event data
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_FindCluster(
                                uint16                          u16ClusterId,
                                uint8                           u8SourceEndPointId,
                                bool_t                          bIsServer,
                                tsZCL_EndPointDefinition      **ppsEndPointDefinition,
                                tsZCL_ClusterInstance         **ppsClusterInstance,
                                void                          **ppsCustomDataStructure)
{

    /* Check pointers are valid */
    if((ppsEndPointDefinition == NULL)  ||
       (ppsClusterInstance == NULL)     ||
       (ppsCustomDataStructure == NULL))
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    /* check EP is registered and cluster is present on the end point */
    if(eZCL_SearchForEPentry(u8SourceEndPointId, ppsEndPointDefinition) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_EP_RANGE);
    }

    if(eZCL_SearchForClusterEntry(u8SourceEndPointId, u16ClusterId, bIsServer, ppsClusterInstance) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_CLUSTER_NOT_FOUND);
    }

    /* initialise custom data pointer */
    *ppsCustomDataStructure = (*ppsClusterInstance)->pvEndPointCustomStructPtr;

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       pvZCL_GetAttributePointer
 **
 ** DESCRIPTION:
 ** Finds pointers related to a particular Attribute value in shared structure
 **
 ** PARAMETERS:                     Name                        Usage
 **
 ** RETURN:
 ** void pointer to attribute value
 **
 ****************************************************************************/
PUBLIC void *pvZCL_GetAttributePointer(
                    tsZCL_AttributeDefinition  *psAttributeDefinition,
                    tsZCL_ClusterInstance      *psClusterInstance,
                    uint16                      u16AttributeId)
{
    uint8 u8attributeSize;
    uint16 u16attributeOffset, u16AttributeArrayIndex;


    if((psAttributeDefinition==NULL)||(psClusterInstance == NULL))
    {
        return(NULL);
    }

    u16attributeOffset = psAttributeDefinition->u16OffsetFromStructBase;

    if(0 == psAttributeDefinition->u16AttributeArrayLength)
    {
        return((void *)&(((uint8 *)(psClusterInstance->pvEndPointSharedStructPtr))[u16attributeOffset]));
    }
    else
    {
        eZCL_GetAttributeTypeSize(psAttributeDefinition->eAttributeDataType, &u8attributeSize);
        if(u16AttributeId < psAttributeDefinition->u16AttributeEnum)
        {
            return(NULL);
        }
        u16AttributeArrayIndex = u16AttributeId - psAttributeDefinition->u16AttributeEnum;

        /* check attribute type */
        /* character & octet strings should be handled separately */
        if(psAttributeDefinition->eAttributeDataType == E_ZCL_OSTRING ||
            psAttributeDefinition->eAttributeDataType == E_ZCL_CSTRING)
        {
            u8attributeSize = sizeof(tsZCL_OctetString);
        }
        else if(psAttributeDefinition->eAttributeDataType == E_ZCL_LOSTRING ||
                    psAttributeDefinition->eAttributeDataType == E_ZCL_LCSTRING)
        {
            u8attributeSize = sizeof(tsZCL_LongOctetString);
        }
        /* 24 & 48 bit data types copied separately */
        else
        {
            u8attributeSize = u8ZCL_GetAttributeAllignToFourBytesBoundary(u8attributeSize);
        }

        return((void *)&(((uint8 *)(psClusterInstance->pvEndPointSharedStructPtr))[u16attributeOffset + (u8attributeSize*u16AttributeArrayIndex)]));
    }
}
/****************************************************************************
 **
 ** NAME:       vZCL_RegisterCheckForManufCodeCallBack
 **
 ** DESCRIPTION:
 ** Registers the call back to check whether Manufacturer Code is supported by
 ** application or not
 **
 ** PARAMETERS:               Name                    Usage
 ** void                     *fnPtr                   Function pointer to registered callback
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/
PUBLIC void vZCL_RegisterCheckForManufCodeCallBack(void* fnPtr)
{
    prfnbZCLIsManufacturerCodeSupported = (bool_t (*)( uint16 ))fnPtr;
}

/****************************************************************************
 **
 ** NAME:       bZCL_IsManufacturerCodeSupported
 **
 ** DESCRIPTION:
 ** Checks whether a manufacturer Code is supported by application
 ** or not
 **
 ** PARAMETERS:               Name                    Usage
 **
 ** RETURN:
 ** bool
 **
 ****************************************************************************/

PRIVATE bool_t bZCL_IsManufacturerCodeSupported(uint16 u16ManufacturerCode)
{
    if(prfnbZCLIsManufacturerCodeSupported != NULL)
    {
        return prfnbZCLIsManufacturerCodeSupported(u16ManufacturerCode);
    }
    return FALSE;
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
