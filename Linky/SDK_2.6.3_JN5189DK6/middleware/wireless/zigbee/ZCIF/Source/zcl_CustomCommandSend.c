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
 * MODULE:             ZCL
 *
 * COMPONENT:          zcl_CustomCommandSend.c
 *
 * DESCRIPTION:       ZCL custom command construction functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>

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

/****************************************************************************
 **
 ** NAME:       eZCL_CustomCommandSend
 **
 ** DESCRIPTION:
 ** Builds ZCL custom command and send
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                 u8SourceEndPointId            Source end point
 ** uint8                 u8DestinationEndPointId       Destination end point
 ** tsZCL_Address         *psDestinationAddress         pointer to destination address
 ** uint16                u16ClusterId                  cluster id
 ** bool_t                bDirection                    From Client/Server
 ** uint8                 u8CommandId                   Command Id
 ** uint8                 *pu8TransactionSequenceNumber ZCL Transaction Seq Number
 ** tsZCL_TxPayloadItem   *psPayloadDefinition          pointer to payload definition
 ** bool_t                bIsManufacturerSpecific       is manufacturer specific
 ** uint16                u16ManufacturerCode           manufacture code
 ** uint8                 u8ItemsInPayload              Items in payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eZCL_CustomCommandSend(
                        uint8               u8SourceEndPointId,
                        uint8               u8DestinationEndPointId,
                        tsZCL_Address       *psDestinationAddress,
                        uint16              u16ClusterId,
                        bool_t              bDirection,
                        uint8               u8CommandId,
                        uint8               *pu8TransactionSequenceNumber,
                        tsZCL_TxPayloadItem *psPayloadDefinition,
                        bool_t              bIsManufacturerSpecific,
                        uint16              u16ManufacturerCode,
                        uint8               u8ItemsInPayload)
{
    int n, x;
    uint16 u16Offset;
    uint16 u16HeaderSize = 0;
    uint16 u16PayloadSize = 0;
    uint8  u8TypeSize;
    bool_t   bDisableDefaultResponse = TRUE;

    tsZCL_ClusterInstance       *psZCL_ClusterInstance;
    tsZCL_EndPointDefinition    *psEndPointDefinition;

    PDUM_thAPdu hZCLbufferPoolHandle;
    PDUM_thAPduInstance myPDUM_thAPduInstance;
    teZCL_Status eStatus;

    tfpZCL_ZCLMessageSignatureCreationFunction pfCreateSignature = psZCL_Common->pfZCLSignatureCreationFunction;

    /* Do parameter checks */
    if((psDestinationAddress == NULL)   ||
       (pu8TransactionSequenceNumber == NULL)   ||
       ((u8ItemsInPayload > 0) && (psPayloadDefinition == NULL)))
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    /* Check pointers in payload definition */
    for(n = 0; n < u8ItemsInPayload; n++)
    {
        if((psPayloadDefinition[n].pvData == NULL) &&
            (psPayloadDefinition[n].u16Quantity != 0))
        {
            return(E_ZCL_ERR_PARAMETER_NULL);
        }
    }

    if(u8SourceEndPointId != ZCL_GP_PROXY_ENDPOINT_ID)
    {
        /* Check end point is registered and cluster is present in the send device */
        if(eZCL_SearchForEPentry(u8SourceEndPointId, &psEndPointDefinition) != E_ZCL_SUCCESS)
        {
            return(E_ZCL_ERR_EP_UNKNOWN);
        }

        bDisableDefaultResponse = psEndPointDefinition->bDisableDefaultResponse;
        if(eZCL_SearchForClusterEntry(u8SourceEndPointId, u16ClusterId, bDirection, &psZCL_ClusterInstance) != E_ZCL_SUCCESS)
        {
            return(E_ZCL_ERR_CLUSTER_NOT_FOUND);
        }
    }

    /* Build command packet */

    /* Get a buffer */
    hZCLbufferPoolHandle = hZCL_GetBufferPoolHandle();
    myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();

    if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
    {
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }

    /* If we are a client, get a new TSN, otherwise use the TSN passed in*/
    if (!bDirection)
    {
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
    }

    /* write command header */
    u16HeaderSize = u16ZCL_WriteCommandHeader(
                        myPDUM_thAPduInstance,
                        eFRAME_TYPE_COMMAND_IS_SPECIFIC_TO_A_CLUSTER,
                        bIsManufacturerSpecific,
                        u16ManufacturerCode,
                        bDirection,
                        bDisableDefaultResponse,
                        *pu8TransactionSequenceNumber,
                        u8CommandId);

    /* Calculate payload size */
    for(n = 0; n < u8ItemsInPayload; n++)
    {
        switch(psPayloadDefinition[n].eType)
        {
        case(E_ZCL_OSTRING):
            if((((tsZCL_OctetString*)psPayloadDefinition[n].pvData)->u8Length == 0xff) ||
                    ((((tsZCL_OctetString*)psPayloadDefinition[n].pvData)->u8Length == 0x00)))
            {
                u16PayloadSize += 1;
            }
            else
            {
                u16PayloadSize += ((tsZCL_OctetString*)psPayloadDefinition[n].pvData)->u8Length;
            }
            break;

        case(E_ZCL_CSTRING):
            if(((tsZCL_CharacterString*)psPayloadDefinition[n].pvData)->u8Length == 0xff)
            {
                u16PayloadSize += 1;
            }
            else
            {
                u16PayloadSize += ((tsZCL_CharacterString*)psPayloadDefinition[n].pvData)->u8Length;
            }
            break;

        case(E_ZCL_LOSTRING):
            if((((tsZCL_LongOctetString*)psPayloadDefinition[n].pvData)->u16Length == 0xffff)   ||
            (((tsZCL_LongOctetString*)psPayloadDefinition[n].pvData)->u16Length == 0x0000))
            {
                u16PayloadSize += 2;
            }
            else
            {
                u16PayloadSize += ((tsZCL_LongOctetString*)psPayloadDefinition[n].pvData)->u16Length;
            }
            break;

        case(E_ZCL_LCSTRING):
            if(((tsZCL_LongCharacterString*)psPayloadDefinition[n].pvData)->u16Length == 0xffff)
            {
                u16PayloadSize += 2;
            }
            else
            {
                u16PayloadSize += ((tsZCL_LongCharacterString*)psPayloadDefinition[n].pvData)->u16Length;
            }
            break;

        default:
            eStatus = eZCL_GetAttributeTypeSize(psPayloadDefinition[n].eType, &u8TypeSize);
            if(eStatus == E_ZCL_SUCCESS)
            {
                u16PayloadSize += (u8TypeSize * psPayloadDefinition[n].u16Quantity);
            }
            else
            {
                /* Probably an unsupported attribute type */
                return eStatus;
            }
            break;
        }
    }
    u16PayloadSize += u16HeaderSize;

    /* check buffer size */
    if(PDUM_u16APduGetSize(hZCLbufferPoolHandle) < u16PayloadSize)
    {
        // free buffer and return
        PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }

    /* Where to start writing the payload */
    u16Offset = u16HeaderSize;

    /* Write payload, one item at a time */
    for(n = 0; n < u8ItemsInPayload; n++)
    {
        switch(psPayloadDefinition[n].eType)
        {
        case(E_ZCL_OSTRING):
        case(E_ZCL_CSTRING):
        case(E_ZCL_LOSTRING):
        case(E_ZCL_LCSTRING):
            /* Write item into payload */
            u16Offset += u16ZCL_APduInstanceWriteStringNBO(myPDUM_thAPduInstance, u16Offset, psPayloadDefinition[n].eType, psPayloadDefinition[n].pvData);
            break;

        case(E_ZCL_SIGNATURE):
            memset(psPayloadDefinition[n].pvData, 0xFF, E_ZCL_SIGNATURE_SIZE);
            if(psZCL_Common->pfZCLSignatureCreationFunction != NULL)
            {
                pfCreateSignature(u8SourceEndPointId,
                                  (uint8*)(PDUM_pvAPduInstanceGetPayload(myPDUM_thAPduInstance)) + u16HeaderSize,
                                  (uint32)u16Offset - u16HeaderSize,
                                  (uint8*)(psPayloadDefinition[n].pvData),
                                  (uint8*)(psPayloadDefinition[n].pvData) + (E_ZCL_SIGNATURE_SIZE/2));
            }
            /* Write the signature into the payload */
            for(x = 0; x < E_ZCL_SIGNATURE_SIZE; x++)
            {
                /* Write item into payload */
                u16Offset += u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16Offset, E_ZCL_UINT8, (uint8 *)psPayloadDefinition[n].pvData + x);
            }
            break;

        default:
            /* Get item size */
            eZCL_GetAttributeTypeSize(psPayloadDefinition[n].eType, &u8TypeSize);

            for(x = 0; x < psPayloadDefinition[n].u16Quantity; x++)
            {
                /* Write item into payload */
                u16Offset += u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16Offset, psPayloadDefinition[n].eType, (uint8 *)(psPayloadDefinition[n].pvData) + (x * u8ZCL_GetAttributeAllignToFourBytesBoundary(u8TypeSize)));
            }
            break;
        }
    }

    /* transmit the custom command */
    if(eZCL_TransmitDataRequest(myPDUM_thAPduInstance,
            u16Offset,  // Payload size that is correct even if structs are not packed
            u8SourceEndPointId,
            u8DestinationEndPointId,
            u16ClusterId,
            psDestinationAddress
            ) != ZPS_E_SUCCESS)
    {
        return(E_ZCL_ERR_ZTRANSMIT_FAIL);
    }

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       bZCL_SendCommandResponse
 **
 ** DESCRIPTION:
 ** send command response
 **
 ** PARAMETERS:                 Name                        Usage
 ** ZPS_tsAfEvent             *pZPSevent               Zigbee Buffer
 **
 ** RETURN:
 ** bool
 **
 ****************************************************************************/
PUBLIC bool_t bZCL_SendCommandResponse( ZPS_tsAfEvent *pZPSevent)
{
    if((pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode == ZPS_E_ADDR_MODE_BOUND)  ||
           (pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode == ZPS_E_ADDR_MODE_GROUP) ||
           (pZPSevent->uEvent.sApsDataIndEvent.uDstAddress.u16Addr > 0xfff7))
    {
       return FALSE;
    }
    return TRUE;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
