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
 * COMPONENT:          ZCL
 *
 * DESCRIPTION:       Attribute Discovery
 *
 * $ $
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_common.h"
#include "zcl_internal.h"

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
PRIVATE teZCL_Status eZCL_SearchForClosestAttributeEntry(
                        uint16                      u16AttributeEnum,
                        bool_t                      bManufacturerSpecific,
                        tsZCL_ClusterInstance      *psClusterInstance,
                        uint16                     *pu16FirstAttributeId,
                        uint16                     *pu16attributeIndex);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       vZCL_HandleAttributesDiscoverExtendedRequest
 **
 ** DESCRIPTION:
 ** Handles an attribute discovery request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee Stack Event
 ** tsZCL_EndPointDefinition *psZCL_EndPointDefinition  EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/

PUBLIC   void vZCL_HandleAttributesDiscoverExtendedRequest(
                                             ZPS_tsAfEvent              *pZPSevent,
                                             tsZCL_EndPointDefinition   *psZCL_EndPointDefinition,
                                             tsZCL_ClusterInstance      *psClusterInstance)
{
    uint16 i, j;

    uint16 u16inputOffset, u16outputOffset, u16BufferOverflowFlagOffset, u16Index = 0;

    tsZCL_HeaderParams sZCL_HeaderParams;

    bool_t bDiscoveryComplete;

    uint16 u16AttributeId, u16FirstAttributeId, u16TempAttrId;


    tsZCL_Address sZCL_Address;

    uint8 u8MaximumNumberOfIdentifiers;
    uint16 u16payloadSize;
    uint16 u16responseBufferSize;


    PDUM_thAPduInstance myPDUM_thAPduInstance;

    uint16 u16FirstAttributeIndex=0;
    bool_t bFoundAttribute;
    
    uint8 u8TempAttributeFlagwithMask;

    // build command packet for response
    // read incoming request
    u16inputOffset = u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                                              &sZCL_HeaderParams);


    // payload investigation
    u16payloadSize = PDUM_u16APduInstanceGetPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);
    // size of outgoing buffer
    u16responseBufferSize = u16ZCL_GetTxPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr);

    // size sanity checks
    if((u16payloadSize < u16inputOffset) || ((u16payloadSize - u16inputOffset)!=3))
    {
        // send response if possible
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_MALFORMED_COMMAND);
        return;
    }

    // get buffer to write the response in
    myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();
    // no buffers - return. we can make this descision a bit more exotic using the incoming buffer to store the
    // outgoing command but for the moment we are implementing the KISS principle.
    if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
    {
        return;
    }

    if(u16responseBufferSize < (u16inputOffset+3))
    {
        // send response if possible
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SOFTWARE_FAILURE);
        // free buffer and return
        PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
        return;
    }

    // modify and write back
    u16outputOffset = u16ZCL_WriteCommandHeader(myPDUM_thAPduInstance,
                                           sZCL_HeaderParams.eFrameType,
                                           sZCL_HeaderParams.bManufacturerSpecific,
                                           sZCL_HeaderParams.u16ManufacturerCode,
                                           !sZCL_HeaderParams.bDirection,
                                           psZCL_EndPointDefinition->bDisableDefaultResponse,
                                           sZCL_HeaderParams.u8TransactionSequenceNumber,
                                           E_ZCL_DISCOVER_ATTRIBUTES_EXTENDED_RESPONSE);

    // read input attribute Id
    u16inputOffset += u16ZCL_APduInstanceReadNBO(
                        pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_ATTRIBUTE_ID, &u16AttributeId);
    u16inputOffset += u16ZCL_APduInstanceReadNBO(
                        pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT8, &u8MaximumNumberOfIdentifiers);

    u16BufferOverflowFlagOffset = u16outputOffset;
    // parse the cluster, read each attribute from the device and write into the outgoing buffer
    i=0;
    j=0;
    // step over discovery complete flag slot
    u16outputOffset++;

    bFoundAttribute = FALSE;
    if(eZCL_SearchForClosestAttributeEntry(u16AttributeId, sZCL_HeaderParams.bManufacturerSpecific, psClusterInstance, &u16FirstAttributeId, &u16FirstAttributeIndex) == E_ZCL_SUCCESS)
    {
        bFoundAttribute = TRUE;
        j = u16FirstAttributeIndex;
        u16Index = u16FirstAttributeIndex;

        u16TempAttrId = u16FirstAttributeId;

        while((j < psClusterInstance->psClusterDefinition->u16NumberOfAttributes) &&
              (i < u8MaximumNumberOfIdentifiers)                                  &&
              (u16responseBufferSize >= (u16outputOffset +4))
        )
        {
            // check attribute flag matches
            if(
                  (bZCL_CheckManufacturerSpecificAttributeFlagMatch(&psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index], sZCL_HeaderParams.bManufacturerSpecific)) &&
                  (bZCL_CheckAttributeDirectionFlagMatch(&psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index],!(sZCL_HeaderParams.bDirection)))
              )
            {
                // write Attribute Id and type into outgoing buffer
                u16outputOffset += u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16outputOffset, E_ZCL_ATTRIBUTE_ID,
                &u16TempAttrId);
                u16outputOffset += u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16outputOffset, E_ZCL_UINT8,
                &psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index].eAttributeDataType);
                /*Mask all other bits to 0 in the payload*/
                u8TempAttributeFlagwithMask = psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index].u8AttributeFlags & 0x07;
                u16outputOffset += u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16outputOffset, E_ZCL_UINT8,
                &u8TempAttributeFlagwithMask);
                // increment attribute count
                i++;
            }

            // increment loop count
            j++;
            if((psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeArrayLength != 0)&&
               ( (u16TempAttrId - psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum) < psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeArrayLength))
            {
                u16TempAttrId++;
            }
            else
            {
                u16Index++;
                u16TempAttrId = psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum;
            }
        }
    }

    // we need to check if there are any valid attributes left to discover, this must take into account the MS flag
    bDiscoveryComplete = TRUE;
    while(bFoundAttribute && (j < psClusterInstance->psClusterDefinition->u16NumberOfAttributes) && (bDiscoveryComplete != FALSE))
    {
        // check attribute flag matches
        if(bZCL_CheckManufacturerSpecificAttributeFlagMatch(&psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index], sZCL_HeaderParams.bManufacturerSpecific))
        {
            bDiscoveryComplete = FALSE;
        }

        // increment loop count
        j++;
        if((psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeArrayLength != 0)&&
        ((u16TempAttrId - psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum) < psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeArrayLength))
        {
            u16TempAttrId++;
        }
        else
        {
            u16Index++;
            u16TempAttrId = psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum;
        }
    }

    // write discovery result in first byte
    u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16BufferOverflowFlagOffset, E_ZCL_BOOL, &bDiscoveryComplete);

    // transmit
    // NTS testing Nov 09. Don't send a good default response here as we have a unicast response that is valid.

    // build address structure
    eZCL_BuildTransmitAddressStructure(pZPSevent, &sZCL_Address);
    // transmit request
    eZCL_TransmitDataRequest(myPDUM_thAPduInstance,
                                u16outputOffset,
                                pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                pZPSevent->uEvent.sApsDataIndEvent.u16ClusterId,
                                &sZCL_Address);
}

/****************************************************************************
 **
 ** NAME:       eZCL_SearchForClosestAttributeEntry
 **
 ** DESCRIPTION:
 ** Searches For An Attribute Definition In The Endpoint
 ** assumes EP has already been validiated
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint16                      u16AttributeEnum            Attribute Id
 ** bool_t                      bIsManufacturerSpecific     Attribute manufacturer specific
 ** tsZCL_ClusterInstance      *psClusterInstance           Cluster Structure
 ** tsZCL_AttributeDefinition **ppsAttributeDefinition      Attribute Structure
 ** uint16                     *pu16attributeIndex          Index of attribute definition
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PRIVATE teZCL_Status eZCL_SearchForClosestAttributeEntry(
                        uint16                      u16AttributeEnum,
                        bool_t                      bManufacturerSpecific,
                        tsZCL_ClusterInstance      *psClusterInstance,
                        uint16                     *pu16FirstAttributeId,
                        uint16                     *pu16attributeIndex)
{

    tsZCL_AttributeDefinition  *psAttributeDefinition;
    uint16 u16TempAttrId;
    if (psClusterInstance == NULL)
    {
        return(E_ZCL_ERR_ATTRIBUTE_NOT_FOUND);
    }

    psAttributeDefinition = psClusterInstance->psClusterDefinition->psAttributeDefinition;
    u16TempAttrId = psAttributeDefinition->u16AttributeEnum;
    // not found
    for(*pu16attributeIndex=0; *pu16attributeIndex < psClusterInstance->psClusterDefinition->u16NumberOfAttributes; (*pu16attributeIndex)++)
    {
        if((u16TempAttrId >= u16AttributeEnum) && bZCL_CheckManufacturerSpecificAttributeFlagMatch(psAttributeDefinition, bManufacturerSpecific))
        {
            *pu16FirstAttributeId = u16TempAttrId;
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

    return(E_ZCL_ERR_ATTRIBUTE_NOT_FOUND);
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
