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
 * COMPONENT:          zcl_readAttributesRequestSend.c
 *
 * DESCRIPTION:       Send Read Attributes Request
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

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

#ifdef CLD_SIMPLE_METERING
/****************************************************************************
*
* NAME: eZCL_HandleReadAttributesResponse
*
* DESCRIPTION:
* Handles the response from a read attributes command. If not all
* attributes were read in the initial request, this function will continue
* to send requests until all attributes in the original read attributes
* request have returned either a value or an error status.
*
* PARAMETERS:  Name                            Usage
*              psEvent                         pointer to the ZCL event
*              pu8TransactionSequenceNumber    Sequence number
*
* RETURNS:
* teSE_Status
*
****************************************************************************/
PUBLIC   teZCL_Status eZCL_HandleReadAttributesResponse(
                    tsZCL_CallBackEvent        *psEvent,
                    uint8                      *pu8TransactionSequenceNumber)
{

    int n;
    int iNumAttributesToRead = 0;
    tsZCL_Address sAddress;
    uint16 au16AttributeRequestList[20], u16ManufacturerCode = 0;
    uint8 u8Direction, u8FrameFrameControlField;
    bool_t bIsManufacturerSpecific;
    uint16 u16Index = 0, u16TempAttrId;
    /* Check parameter arguments */
    if(psEvent == NULL || pu8TransactionSequenceNumber == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    /* check cluster instance is present in the call back event */
    if(psEvent->psClusterInstance == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    /* Check that we're handling the correct event. Exit if not */
    if(psEvent->eEventType != E_ZCL_CBET_READ_ATTRIBUTES_RESPONSE || psEvent->pZPSevent->eType != ZPS_EVENT_APS_DATA_INDICATION)
    {
        return E_ZCL_FAIL;
    }

    /* Get the rxed zcl frame control field */
    u8FrameFrameControlField = *((uint8*)PDUM_pvAPduInstanceGetPayload(psEvent->pZPSevent->uEvent.sApsDataIndEvent.hAPduInst));

    /* Get Direction bit */
    u8Direction = (u8FrameFrameControlField >> 3) & 0x1;

    /* Get Manufacturer specific bit */
    bIsManufacturerSpecific = (u8FrameFrameControlField >> 2) & 0x1;

    /* Check for Manufacturer code */
    if(bIsManufacturerSpecific)
    {
        u16ZCL_APduInstanceWriteNBO(psEvent->pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                1, E_ZCL_UINT16, &u16ManufacturerCode);
    }

    u16TempAttrId = psEvent->psClusterInstance->psClusterDefinition->psAttributeDefinition[0].u16AttributeEnum;
    /* Check the status of all attributes in this cluster */
    for(n = 0; n < psEvent->psClusterInstance->psClusterDefinition->u16NumberOfAttributes; n++)
    {

        /* If its still set to read request, try and read it again */
        if((ZCL_IS_BIT_SET(uint8,psEvent->psClusterInstance->pu8AttributeControlBits[n],E_ZCL_ACF_RS)) &&
            bZCL_CheckManufacturerSpecificAttributeFlagMatch(&psEvent->psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index], bIsManufacturerSpecific) )
        {
            au16AttributeRequestList[iNumAttributesToRead] = u16TempAttrId;
            iNumAttributesToRead++;
        }

        /* If no more space in request list storage, exit loop */
        if(iNumAttributesToRead == sizeof(au16AttributeRequestList) / sizeof(uint16))
        {
            break;
        }

        if((psEvent->psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeArrayLength != 0)&&
          ((u16TempAttrId - psEvent->psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum) < psEvent->psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeArrayLength))
        {
            u16TempAttrId++;
        }
        else
        {
            u16Index++;
            u16TempAttrId = psEvent->psClusterInstance->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum;
        }
    }

    /* If no more attributes to read, exit, we're done */
    if(iNumAttributesToRead == 0)
    {
        return E_ZCL_SUCCESS;
    }

    /* Get address to send next request to */
    sAddress.eAddressMode = psEvent->pZPSevent->uEvent.sApsDataIndEvent.u8SrcAddrMode;
    sAddress.uAddress.u64DestinationAddress = psEvent->pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u64Addr;

    /* Issue another read request */
    if(eZCL_SendReadAttributesRequest(psEvent->pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,  // Source endpoint id
        psEvent->pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,  // Destination endpoint id
        psEvent->pZPSevent->uEvent.sApsDataIndEvent.u16ClusterId,   // Cluster id
        !u8Direction,
        &sAddress,                                    // Destination address
        pu8TransactionSequenceNumber,                 // Sequence number
        iNumAttributesToRead,                         // Number of attributes to read
        bIsManufacturerSpecific,
        u16ManufacturerCode,
        au16AttributeRequestList) != E_ZCL_SUCCESS)   // List of attributes to read
    {
        /* Something failed when trying to read more attributes */
        return E_ZCL_FAIL;
    }

    /* Let application know not all attributes requested yet */
    return E_ZCL_READ_ATTR_REQ_NOT_FINISHED;
}

#endif
/****************************************************************************
 **
 ** NAME:       eZCL_SendReadAttributesRequest
 **
 ** DESCRIPTION:
 ** checks validity of a user EP structure
 **
 ** PARAMETERS:      Name                           Usage
 ** uint8            u8SourceEndPointId             Source EP Id
 ** uint8            u8DestinationEndPointId        Destination EP Id
 ** uint16           u16ClusterId                   Cluster Id
 ** bool_t           bDirectionIsServerToClient     Direction
 ** tsZCL_Address   *psDestinationAddress           Destination Address
 ** uint8           *pu8TransactionSequenceNumber   Sequence number Pointer
 ** uint8            u8NumberOfAttributesInRequest  No. Attributes in Request
 ** bool_t           bIsManufacturerSpecific        Attribute manufacturer specific
 ** uint16           u16ManufacturerCode            Manufacturer code
 ** uint16          *pu16AttributeRequestList       Attributes request list
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC   teZCL_Status  eZCL_SendReadAttributesRequest(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8NumberOfAttributesInRequest,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    uint16                     *pu16AttributeRequestList)
{
    int i;

    uint16 u16offset;
    uint16 u16payloadSize;
    teZCL_Status eReturnValue;
   /* tsZCL_ClusterInstance       *psClusterInstance;
    tsZCL_AttributeDefinition   *psAttributeDefinition;*/
    tsZCL_EndPointDefinition    *psZCL_EndPointDefinition;

    PDUM_thAPduInstance myPDUM_thAPduInstance;

    // parameter checks
    if((psDestinationAddress == NULL) || (pu8TransactionSequenceNumber == NULL) || (pu16AttributeRequestList == NULL))
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    if(u8NumberOfAttributesInRequest == 0)
    {
        return(E_ZCL_ERR_ATTRIBUTES_0);
    }


    /* check if default response is required */
    eReturnValue = eZCL_SearchForEPentryAndCheckManufacturerId(u8SourceEndPointId, bIsManufacturerSpecific, u16ManufacturerCode, &psZCL_EndPointDefinition);
    if(eReturnValue != E_ZCL_SUCCESS)
    {
        return(eReturnValue);
    }


    // get buffer
    myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();

    if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
    {
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }

    // handle sequence number pass present value back to user
    *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();

    // write command header
    u16offset = u16ZCL_WriteCommandHeader(myPDUM_thAPduInstance,
                                       eFRAME_TYPE_COMMAND_ACTS_ACCROSS_ENTIRE_PROFILE,
                                       bIsManufacturerSpecific,
                                       u16ManufacturerCode,
                                       bDirectionIsServerToClient,
                                       psZCL_EndPointDefinition->bDisableDefaultResponse,
                                       *pu8TransactionSequenceNumber,
                                       E_ZCL_READ_ATTRIBUTES);

    // calculate payload size
    u16payloadSize = u16offset + 2*u8NumberOfAttributesInRequest;
    // check buffer size
    if(PDUM_u16APduGetSize(psZCL_Common->hZCL_APdu) < u16payloadSize)
    {
        // free buffer and return
        PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }

    // write payload, attribute at a time
    for(i=0; i<u8NumberOfAttributesInRequest; i++)
    {
        u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16offset+(2*i), E_ZCL_ATTRIBUTE_ID, &pu16AttributeRequestList[i]);
    }

    // transmit the request
    if(eZCL_TransmitDataRequest(myPDUM_thAPduInstance,
                                u16payloadSize,
                                u8SourceEndPointId,
                                u8DestinationEndPointId,
                                u16ClusterId,
                                psDestinationAddress) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_ZTRANSMIT_FAIL);
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
