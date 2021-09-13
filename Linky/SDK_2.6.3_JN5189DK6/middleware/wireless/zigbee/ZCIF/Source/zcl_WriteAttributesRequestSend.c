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
 * COMPONENT:          zcl_WriteAttributesRequestSend.c
 *
 * DESCRIPTION:       Send Write Attributes Command
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

/****************************************************************************
 **
 ** NAME:       eZCL_SendWriteAttributesRequestInternal
 **
 ** DESCRIPTION:
 ** Sends a write attributes request command. Called inline by the public functions
 ** for the various write attributes commands
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
 ** uint8            u8CommandId                    Command ID to send (write, undivided, no response)
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC   teZCL_Status  eZCL_SendWriteAttributesRequestInternal(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8NumberOfAttributesInRequest,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    tsZCL_WriteAttributeRecord  *pu16AttributeRequestList,
                    uint8                       u8CommandId)
{

    int i;
    uint16 u16offset;
    teZCL_Status eReturnValue;
    tsZCL_EndPointDefinition    *psZCL_EndPointDefinition;
    PDUM_thAPduInstance myPDUM_thAPduInstance;
    uint16 u16payloadSize = 0;


    // parameter checks
    if((psDestinationAddress == NULL) || (pu8TransactionSequenceNumber == NULL) || (pu16AttributeRequestList == NULL))
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    if(u8NumberOfAttributesInRequest == 0)
    {
        return(E_ZCL_ERR_ATTRIBUTES_0);
    }

    // check if default response is required
    eReturnValue = eZCL_SearchForEPentryAndCheckManufacturerId(u8SourceEndPointId, bIsManufacturerSpecific, u16ManufacturerCode, &psZCL_EndPointDefinition);
    if(eReturnValue != E_ZCL_SUCCESS)
    {
        return(eReturnValue);
    }

    // handle sequence number pass present value back to user
    *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();

    // get buffer
    myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();

    if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
    {
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }

    // write command header
    u16offset = u16ZCL_WriteCommandHeader(myPDUM_thAPduInstance,
                                       eFRAME_TYPE_COMMAND_ACTS_ACCROSS_ENTIRE_PROFILE,
                                       bIsManufacturerSpecific,
                                       u16ManufacturerCode,
                                       bDirectionIsServerToClient,
                                       psZCL_EndPointDefinition->bDisableDefaultResponse,
                                       *pu8TransactionSequenceNumber,
                                       u8CommandId);

    u16payloadSize += u16offset;
        // check buffer size
    if(PDUM_u16APduGetSize(psZCL_Common->hZCL_APdu) < u16payloadSize)
    {
        // free buffer and return
        PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }

    // Build the command
    for(i=0; i<u8NumberOfAttributesInRequest; i++)
    {



        // write Attribute Id into outgoing buffer
        u16offset += u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16offset, E_ZCL_ATTRIBUTE_ID,
        		&(pu16AttributeRequestList->u16AttributeEnum));
        // write type
        u16offset += u16ZCL_APduInstanceWriteNBO(
            myPDUM_thAPduInstance,
            u16offset,
            E_ZCL_UINT8,
            &pu16AttributeRequestList->eAttributeDataType);
        // value - structure base address in cluster struct, offset in attribute structure
       /* u16offset += u16ZCL_WriteAttributeValueIntoBuffer(u8SourceEndPointId,
            pu16AttributeRequestList[i],
            psClusterInstance,
            psAttributeDefinition,
            u16offset,
            myPDUM_thAPduInstance);
        for(j=0; j < pu16AttributeRequestList->u8AttributeDataLength; j++)
        {
        	DBG_vPrintf(TRUE, "Value = %d\n",pu16AttributeRequestList->pu8AttributeData[j]);
        	u16offset += u16ZCL_APduInstanceWriteNBO(
        	            myPDUM_thAPduInstance,
        	            u16offset,
        	            E_ZCL_UINT8,
        	            &pu16AttributeRequestList->pu8AttributeData[j]);
        }*/
        switch(pu16AttributeRequestList->eAttributeDataType)
		{
#if 0
#ifdef CLD_GREENPOWER
			case(E_ZCL_LOSTRING):
			case(E_ZCL_LCSTRING):
			{
				tsZCL_ClusterInstance       *psClusterInstance;
				uint16 u16NoOfBytes;
				eReturnValue = eZCL_SearchForClusterEntry(u8SourceEndPointId,
															u16ClusterId,
															bDirectionIsServerToClient,
															&psClusterInstance);
			if(eReturnValue != E_ZCL_SUCCESS)
				{
					return(eReturnValue);
				}
				if(psClusterInstance->pStringCallBackFunction)
				{
					eReturnValue = psClusterInstance->pStringCallBackFunction(
						  u8SourceEndPointId,
						  pu16AttributeRequestList->u16AttributeEnum,
						  !bDirectionIsServerToClient,
						  TRUE,
						  myPDUM_thAPduInstance,
						  u16offset,
						  &u16NoOfBytes);
					if(eReturnValue!= E_ZCL_SUCCESS)
					{
						return eReturnValue;
					}

				}

			}

			break;
#endif
#endif
			case(E_ZCL_LOSTRING):
			case(E_ZCL_LCSTRING):
			case(E_ZCL_OSTRING):
			case(E_ZCL_CSTRING):
			{

				u16offset +=  u16ZCL_APduInstanceWriteStringNBO(
						 myPDUM_thAPduInstance,
						 u16offset,
						 pu16AttributeRequestList->eAttributeDataType,
						 pu16AttributeRequestList->pu8AttributeData);
			}
			break;
			default:
			{
				u16offset +=  u16ZCL_APduInstanceWriteNBO(
									myPDUM_thAPduInstance,
									u16offset,
									pu16AttributeRequestList->eAttributeDataType,
									pu16AttributeRequestList->pu8AttributeData);
			}
			break;
		}
        pu16AttributeRequestList++;
    }


    // transmit the request
    if(eZCL_TransmitDataRequest(myPDUM_thAPduInstance,
                                u16offset,
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
