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
 * COMPONENT:          zcl_readAttributesRequestHandle.c
 *
 * DESCRIPTION:       Receive Read Attributes Request
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
#include "dbg.h"

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

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       vZCL_HandleAttributesReadRequest
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent              *pZPSevent                 Zigbee Stack Event
 ** tsZCL_EndPointDefinition *psZCL_EndPointDefinition  EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance     Cluster structure
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/

PUBLIC   void vZCL_HandleAttributesReadRequest(
                                             ZPS_tsAfEvent              *pZPSevent,
                                             tsZCL_EndPointDefinition   *psZCL_EndPointDefinition,
                                             tsZCL_ClusterInstance      *psClusterInstance)
{
    int i;

    tsZCL_CallBackEvent sZCL_CallBackEvent;

    uint16 u16inputOffset, u16outputOffset;

	tsZCL_HeaderParams sZCL_HeaderParams;


    uint8 u8errorCode;

    uint16 u16AttributeId;

    tsZCL_Address sZCL_Address;

    uint8 u8NumberAttributesInRequest;
    uint16 u16payloadSize;
    uint16 u16responseBufferSize;

    uint16 u16typeSize;

    tsZCL_AttributeDefinition   *psAttributeDefinition;

    PDUM_thAPduInstance myPDUM_thAPduInstance;

    uint16 u16clusterRecordSize;

    uint16 u16attributeIndex;

    // parameter checks - clusters and EP's have been sanity checked at the event level
    // We don't null check the cluster id because we may want to generate a read att response for a null
    // cluster (mars 60).  When we call eZCL_SearchForAttributeEntry with a null cluster, it will return
    // gracefully and E_ZCL_CMDS_UNSUPPORTED_ATTRIBUTE will get written into the buffer

    // get buffer to write the response in
    myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();
    // no buffers - return. we can make this descision a bit more exotic using the incoming buffer to store the
    // outgoing command but for the moment we are implementing the KISS principle.
    if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
    {
        return;
    }

    // build command packet for response
    // read incoming request
    u16inputOffset = u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                                              &sZCL_HeaderParams);

    // modify and write back
    u16outputOffset = u16ZCL_WriteCommandHeader(myPDUM_thAPduInstance,
                                           sZCL_HeaderParams.eFrameType,
                                           sZCL_HeaderParams.bManufacturerSpecific,
                                           sZCL_HeaderParams.u16ManufacturerCode,
                                           !sZCL_HeaderParams.bDirection,
                                           psZCL_EndPointDefinition->bDisableDefaultResponse,
                                           sZCL_HeaderParams.u8TransactionSequenceNumber,
                                           E_ZCL_READ_ATTRIBUTES_RESPONSE);

    // payload investigation
    u16payloadSize = PDUM_u16APduInstanceGetPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);
    // size sanity check
    if((u16payloadSize < u16inputOffset) || ((u16payloadSize - u16inputOffset)%2))
    {
        // send response if possible/required
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_MALFORMED_COMMAND);
        // free buffer and return
        PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
        return;
    }

    // number of attributes in request
    u8NumberAttributesInRequest = (u16payloadSize - u16inputOffset)/2;

    // size of outgoing buffer
    u16responseBufferSize = u16ZCL_GetTxPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr);
    // read attributes from the device

    // allow the device to update the attributes - express stylee
    sZCL_CallBackEvent.pZPSevent = pZPSevent;
    sZCL_CallBackEvent.u8TransactionSequenceNumber = sZCL_HeaderParams.u8TransactionSequenceNumber;
    sZCL_CallBackEvent.u8EndPoint = pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint;
    sZCL_CallBackEvent.eEventType = E_ZCL_CBET_READ_REQUEST;
    sZCL_CallBackEvent.psClusterInstance = psClusterInstance;
    sZCL_CallBackEvent.eZCL_Status = E_ZCL_SUCCESS;

    // call user unless null cluster instance (as no atts will be read in that case and the user may attempt to access the cluster without checking and get an exception)
    if (psClusterInstance != NULL)
    {
        psZCL_EndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);
    }


    // parse the incoming message, read each attribute from the device and write into the outgoing buffer
    i = 0;
    // loop round the attributes in the list
    while((i<u8NumberAttributesInRequest) && (sZCL_CallBackEvent.eZCL_Status == E_ZCL_SUCCESS))
    {
        // read input attribute Id
        u16inputOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_ATTRIBUTE_ID, &u16AttributeId);
        // initial size of cluster record - Id + status
        u16clusterRecordSize = 3;

        // check if attribute is present
        if(eZCL_SearchForAttributeEntry(
                psZCL_EndPointDefinition->u8EndPointNumber,
                u16AttributeId,
                sZCL_HeaderParams.bManufacturerSpecific,
                sZCL_HeaderParams.bDirection,
                psClusterInstance,
                &psAttributeDefinition,
                &u16attributeIndex) != E_ZCL_SUCCESS)
        {
            // write 'unsupported' into buffer
            u8errorCode = E_ZCL_CMDS_UNSUPPORTED_ATTRIBUTE;
        }
        else // check if the attribute type is supported by the ZCL
        {
            if(eZCL_GetAttributeTypeSizeFromStructure(psAttributeDefinition, pvZCL_GetAttributePointer(psAttributeDefinition,psClusterInstance, u16AttributeId), &u16typeSize)!=E_ZCL_SUCCESS)
            {
                // write 'invalid' into buffer
                u8errorCode = E_ZCL_CMDS_INVALID_FIELD;
            }
            else if((psAttributeDefinition->u8AttributeFlags & (E_ZCL_AF_RD|E_ZCL_AF_WR)) == E_ZCL_AF_WR)
            {
               u8errorCode = E_ZCL_CMDS_WRITE_ONLY;
            }
            else if(bZCL_CheckAttributeDirectionFlagMatch(psAttributeDefinition,psClusterInstance->bIsServer) == FALSE) // check attribute access status
            {
               u8errorCode = E_ZCL_CMDS_UNSUPPORTED_ATTRIBUTE;
            }
            /* PR - lpsw2919 - for an attribute that is supported but not yet available,
             * mirror should send ATTRIBUTE_UNSUPPORTED response */
            else if((psClusterInstance->psClusterDefinition->u8ClusterControlFlags & CLUSTER_MIRROR_BIT)&&
                    (ZCL_IS_BIT_CLEAR(uint8,psClusterInstance->pu8AttributeControlBits[u16attributeIndex],E_ZCL_ACF_MR)))
            {
                u8errorCode = E_ZCL_CMDS_UNSUPPORTED_ATTRIBUTE;
            }
            else
            {
                /* CCB 2092: ZCL Read Attributes Response too big for single frame if fragmentation is not supported*/
                if((u16outputOffset + u16typeSize + u16clusterRecordSize + 1) > u16responseBufferSize)
                {
                    u8errorCode = E_ZCL_CMDS_INSUFFICIENT_SPACE;
                }
                else
                {
                    // Add data type
                    u16clusterRecordSize++;
                    // add data
                    u8errorCode = E_ZCL_CMDS_SUCCESS;
                    u16clusterRecordSize += u16typeSize;
                }
            }
        }

        // check the full record can fit into the buffer
        if((u16outputOffset + u16clusterRecordSize) > u16responseBufferSize)
        {
           sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_MALFORMED_MESSAGE;
        }
        else // if((u16outputOffset + u16clusterRecordSize) <= u16responseBufferSize)
        {
            // write Attribute Id into outgoing buffer - no matter what
            u16outputOffset += u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16outputOffset, E_ZCL_ATTRIBUTE_ID, &u16AttributeId);
            // write status
            u16outputOffset += u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16outputOffset, E_ZCL_UINT8, &u8errorCode);
            if(u8errorCode == E_ZCL_CMDS_SUCCESS)
            {
                // write type
                u16outputOffset += u16ZCL_APduInstanceWriteNBO(
                                    myPDUM_thAPduInstance,
                                    u16outputOffset,
                                    E_ZCL_UINT8,
                                   &psAttributeDefinition->eAttributeDataType);
                // value - structure base address in cluster struct, offset in attribute structure
                u16outputOffset += u16ZCL_WriteAttributeValueIntoBuffer(
                                    psZCL_EndPointDefinition->u8EndPointNumber,
                                     u16AttributeId,
                                    psClusterInstance,
                                    psAttributeDefinition,
                                    u16outputOffset,
                                    myPDUM_thAPduInstance);
             }
        }

        // increment attribute count
        i++;
    }

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

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
