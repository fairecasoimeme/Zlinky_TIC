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
 * COMPONENT:          zcl_WriteAttributesRequestHandle.c
 *
 * DESCRIPTION:       Receive Write Attributes Request
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

#ifdef CLD_ANALOG_INPUT_BASIC
#include "AnalogInputBasic_internal.h"
#endif
#ifdef CLD_BINARY_INPUT_BASIC
#include "BinaryInputBasic_internal.h"
#endif
#ifdef CLD_ANALOG_OUTPUT_BASIC
#include "AnalogOutputBasic_internal.h"
#endif
#ifdef CLD_BINARY_OUTPUT_BASIC
#include "BinaryOutputBasic_internal.h"
#endif
#ifdef CLD_MULTISTATE_INPUT_BASIC
#include "MultistateInputBasic_internal.h"
#endif
#ifdef CLD_MULTISTATE_OUTPUT_BASIC
#include "MultistateOutputBasic_internal.h"
#endif
#ifdef CLD_POLL_CONTROL
#include "PollControl.h"
#include <string.h>
#endif
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
** NAME:       vZCL_HandleAttributesWriteRequest
**
** DESCRIPTION:
** Handle attribute write request command
**
** PARAMETERS:               Name                       Usage
** ZPS_tsAfEvent            *pZPSevent                 Zigbee Stack Event
** tsZCL_EndPointDefinition *psZCL_EndPointDefinition  EP structure
** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
** bool_t                    bIsUndivided              Is Undivided
** bool_t                    bNoResponse               Is No Response command
**
** RETURN:
** None
**
****************************************************************************/

PUBLIC   void vZCL_HandleAttributesWriteRequest(
                    ZPS_tsAfEvent              *pZPSevent,
                    tsZCL_EndPointDefinition   *psZCL_EndPointDefinition,
                    tsZCL_ClusterInstance      *psClusterInstance,
                    bool_t                      bIsUndivided,
                    bool_t                      bNoResponse)
{
    tsZCL_CallBackEvent sZCL_CallBackEvent;

   // int i;
    uint16 u16inputOffset, u16outputOffset = 0;

    uint8 u8errorCode;
    uint16 u16AttributeId;
    
    tsZCL_HeaderParams sZCL_HeaderParams;
    tsZCL_Address sZCL_Address;
    uint16 u16payloadSize;
   // uint16 u16responseBufferSize;
    teZCL_Status eZCL_SearchForAttributeResult;
    uint16 u16typeSize;
    tsZCL_AttributeDefinition   *psAttributeDefinition;
    teZCL_ZCLAttributeType eAttributeDataType=0;
    PDUM_thAPduInstance myPDUM_thAPduInstance = 0;
    uint16 u16attributeIndex;
    bool_t bNoErrors = TRUE;
    uint16 u16InputAttsStart;
    uint8 u8Pass;

    // get buffer to write the response in
    if (!bNoResponse)
    {
        myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();

        // no buffers - return. we can make this descision a bit more exotic using the incoming buffer to store the
        // outgoing command but for the moment we are implementing the KISS principle.
        if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
        {
            return;
        }
    }

    // build command packet for response
    // read incoming request
    u16inputOffset = u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    if (!bNoResponse)
    {
        // modify and write back
        u16outputOffset = u16ZCL_WriteCommandHeader(myPDUM_thAPduInstance,
            sZCL_HeaderParams.eFrameType,
            sZCL_HeaderParams.bManufacturerSpecific,
            sZCL_HeaderParams.u16ManufacturerCode,
            !sZCL_HeaderParams.bDirection,
            psZCL_EndPointDefinition->bDisableDefaultResponse,
            sZCL_HeaderParams.u8TransactionSequenceNumber,
            E_ZCL_WRITE_ATTRIBUTES_RESPONSE);


        // size of outgoing buffer
       // u16responseBufferSize = PDUM_u16APduGetSize(psZCL_Common->hZCL_APdu);
    }
    // fill in non-attribute specific values in callback event structure
    sZCL_CallBackEvent.u8EndPoint = pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint;
    sZCL_CallBackEvent.psClusterInstance = psClusterInstance;
    sZCL_CallBackEvent.pZPSevent = pZPSevent;
    sZCL_CallBackEvent.eZCL_Status = E_ZCL_SUCCESS;
    // Lock if doing an undivided write
#ifndef COOPERATIVE
    if (bIsUndivided)
    {
        eZCL_GetMutex(psZCL_EndPointDefinition);
    }
#endif
    // parse the incoming message, read each attribute from the pdu
   // i = 0;
    // payload investigation
    u16payloadSize = PDUM_u16APduInstanceGetPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);

    // loop round the attributes in the list
    // parse the message whilst checking for malformed messages.
    // Need two passes because undivided needs to check all atts before writing any of them.
    // First pass - check the message.  Second pass - write the att and do user CB.
    u16InputAttsStart = u16inputOffset;
    for(u8Pass = 0; u8Pass < 2; u8Pass++)
    {
        if(((u8Pass == 0)&&(bIsUndivided))||(u8Pass == 1))
        {
            u16inputOffset = u16InputAttsStart;
            sZCL_CallBackEvent.eZCL_Status = E_ZCL_SUCCESS;
            while((u16payloadSize-u16inputOffset > 0) && (sZCL_CallBackEvent.eZCL_Status == E_ZCL_SUCCESS))
            {
                u8errorCode = E_ZCL_CMDS_SUCCESS;

                // read the attribute record from the message
                // read attribute Id
                u16inputOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_ATTRIBUTE_ID, &u16AttributeId);

                // fill in attribute specific values in callback event structure
                sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.u16AttributeEnum = u16AttributeId;
                // unknown for now
                sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeDataType = E_ZCL_UNKNOWN;
                // null data and status pointers for now
                sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.pvAttributeData = NULL;

                // update attribute status table
                // find attribute entry
                eZCL_SearchForAttributeResult =
                    eZCL_SearchForAttributeEntry(
                            psZCL_EndPointDefinition->u8EndPointNumber,
                            u16AttributeId,
                            sZCL_HeaderParams.bManufacturerSpecific,
                            sZCL_HeaderParams.bDirection,
                            psClusterInstance,
                            &psAttributeDefinition,
                            &u16attributeIndex);

                // parse the attibute record further
                u16inputOffset +=u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT8, &eAttributeDataType);
                // read the value once we've syntax checked the record so far ....
                // attribute is present - check that the ZCL suports it and that everything matches up
                if(eZCL_GetAttributeTypeSizeFromBuffer(eAttributeDataType, pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,u16inputOffset, &u16typeSize)!=E_ZCL_SUCCESS)
                {
                    // Write invalid data type into response buffer
                    u8errorCode = E_ZCL_CMDS_INVALID_DATA_TYPE;
                    if(u16typeSize==0)
                    {
                        // we have no idea how to locate the next attribute, so abort.
                        sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_MALFORMED_MESSAGE;
                    }
                }

                // check if attribute is present in the cluster
                if( eZCL_SearchForAttributeResult != E_ZCL_SUCCESS)
                {
                    // not serious yet - possible we can step over the attribute record if we know its type
                    u8errorCode = E_ZCL_CMDS_UNSUPPORTED_ATTRIBUTE;
                }
                if(u8errorCode == E_ZCL_CMDS_SUCCESS)
                {
                    // attribute is present

                    // Att type is known but not same as we expect
                    if(psAttributeDefinition->eAttributeDataType != eAttributeDataType)
                    {
                        u8errorCode = E_ZCL_CMDS_INVALID_DATA_TYPE;
                    }

                    // Att is read only or client att when on server or vice versa.
                    if(((psAttributeDefinition->u8AttributeFlags & E_ZCL_AF_WR) != E_ZCL_AF_WR) ||
                        (bZCL_CheckAttributeDirectionFlagMatch(psAttributeDefinition,psClusterInstance->bIsServer) == FALSE)
                      )
                    {
                        u8errorCode = E_ZCL_CMDS_READ_ONLY;
                    }

                }

                // Range check.
                if((u8errorCode == E_ZCL_CMDS_SUCCESS)&&((!bIsUndivided && (u8Pass == 1))||(u8Pass == 0)))
                {
                    // Check string will fit into structure
                    if (!bZCL_CheckStringFitsInStructure(
                        psZCL_EndPointDefinition->u8EndPointNumber,
                        u16AttributeId,
                        sZCL_HeaderParams.bManufacturerSpecific,
                        sZCL_HeaderParams.bDirection,
                        psClusterInstance,
                        u16inputOffset,
                        pZPSevent->uEvent.sApsDataIndEvent.hAPduInst))
                        // check for attribute being too small for the structure slot as could happen with a string
                    {
                        u8errorCode = E_ZCL_CMDS_INSUFFICIENT_SPACE;
                    }
                    // Ask user to check the range.  Need to ask on both passes, first pass to build response and second to decide whether to write the att.
                    sZCL_CallBackEvent.eEventType = E_ZCL_CBET_CHECK_ATTRIBUTE_RANGE;
                    sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeStatus = E_ZCL_SUCCESS;
                    sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeDataType = psAttributeDefinition->eAttributeDataType;
                    sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.pvAttributeData =
                        (void *)((uint8 *)PDUM_pvAPduInstanceGetPayload(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst) + u16inputOffset);
                    psZCL_EndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);
                    if (sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeStatus != E_ZCL_CMDS_SUCCESS)
                    {
                        if(E_ZCL_ERR_ATTRIBUTES_ACCESS == (teZCL_Status )sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeStatus)
                        {
                            u8errorCode = E_ZCL_CMDS_NOT_AUTHORIZED;
                        }
                        else
                        {
                            u8errorCode = E_ZCL_CMDS_INVALID_VALUE;
                        }
                    }
                }

                sZCL_CallBackEvent.eEventType = E_ZCL_CBET_WRITE_INDIVIDUAL_ATTRIBUTE;

#if ((defined CLD_POLL_CONTROL) && (defined POLL_CONTROL_SERVER))
                    if((psClusterInstance->psClusterDefinition->u16ClusterEnum == GENERAL_CLUSTER_ID_POLL_CONTROL) && (u8errorCode == E_ZCL_CMDS_SUCCESS))
                    {
                        uint32 u32PollControlAttribute = 0;
                        uint8 *pu8Data;
                        
                        pu8Data = ((uint8 *)PDUM_pvAPduInstanceGetPayload(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst) + u16inputOffset);
                        
                        u32PollControlAttribute  = *pu8Data++;
                        u32PollControlAttribute |= (*pu8Data++) << 8;
                        
                        if(psAttributeDefinition->eAttributeDataType == 0x23){
                            u32PollControlAttribute |= (*pu8Data++) << 16;
                            u32PollControlAttribute |= (*pu8Data++) << 24;
                        }
                        eCLD_PollControlWriteHandler(psZCL_EndPointDefinition->u8EndPointNumber,u16AttributeId,&u32PollControlAttribute,&u8errorCode);
                    }
#endif              
                // Write the att and call the user if all OK on second pass
                if (u8Pass == 1 && (
                    bNoErrors || !bIsUndivided) &&
                    u8errorCode == E_ZCL_CMDS_SUCCESS)
                {
                    // No need to check return value as we checked size in first pass
                    u16ZCL_WriteAttributeValueIntoStructure(
                        psZCL_EndPointDefinition->u8EndPointNumber,
                        u16AttributeId,
                        sZCL_HeaderParams.bManufacturerSpecific,
                        sZCL_HeaderParams.bDirection,
                        psClusterInstance,
                        u16inputOffset,
                        pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);
                    // Inform user the att has been updated
                    sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeStatus = E_ZCL_CMDS_SUCCESS;
                    // add attribute pointer to c/b event structure
                    sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeDataType = psAttributeDefinition->eAttributeDataType;
                    sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.pvAttributeData =
                        pvZCL_GetAttributePointer(psAttributeDefinition, psClusterInstance, u16AttributeId);
#if ((defined CLD_ANALOG_INPUT_BASIC) && (defined ANALOG_INPUT_BASIC_SERVER))
                    if(psClusterInstance->psClusterDefinition->u16ClusterEnum == GENERAL_CLUSTER_ID_ANALOG_INPUT_BASIC)
                        eCLD_AnalogInputBasicHandler(psZCL_EndPointDefinition->u8EndPointNumber);
#endif 
#if ((defined CLD_BINARY_INPUT_BASIC) && (defined BINARY_INPUT_BASIC_SERVER))
                    if(psClusterInstance->psClusterDefinition->u16ClusterEnum == GENERAL_CLUSTER_ID_BINARY_INPUT_BASIC)
                        eCLD_BinaryInputBasicHandler(psZCL_EndPointDefinition->u8EndPointNumber);
#endif 
#if ((defined CLD_ANALOG_OUTPUT_BASIC) && (defined ANALOG_OUTPUT_BASIC_SERVER))
                    if(psClusterInstance->psClusterDefinition->u16ClusterEnum == GENERAL_CLUSTER_ID_ANALOG_OUTPUT_BASIC)
                        eCLD_AnalogOutputBasicHandler(psZCL_EndPointDefinition->u8EndPointNumber);
#endif
#if ((defined CLD_BINARY_OUTPUT_BASIC) && (defined BINARY_OUTPUT_BASIC_SERVER))
                    if(psClusterInstance->psClusterDefinition->u16ClusterEnum == GENERAL_CLUSTER_ID_BINARY_OUTPUT_BASIC)
                        eCLD_BinaryOutputBasicHandler(psZCL_EndPointDefinition->u8EndPointNumber);
#endif
#if ((defined CLD_MULTISTATE_INPUT_BASIC) && (defined MULTISTATE_INPUT_BASIC_SERVER))
                    if(psClusterInstance->psClusterDefinition->u16ClusterEnum == GENERAL_CLUSTER_ID_MULTISTATE_INPUT_BASIC)
                        eCLD_MultistateInputBasicHandler(psZCL_EndPointDefinition->u8EndPointNumber);
#endif
#if ((defined CLD_MULTISTATE_OUTPUT_BASIC) && (defined MULTISTATE_OUTPUT_BASIC_SERVER))
                    if(psClusterInstance->psClusterDefinition->u16ClusterEnum == GENERAL_CLUSTER_ID_MULTISTATE_OUTPUT_BASIC)
                        eCLD_MultistateOutputBasicHandler(psZCL_EndPointDefinition->u8EndPointNumber);
#endif                      
                    psZCL_EndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);
                }

                // Call the user if any errors on the first pass (except null cluster where we don't concern the user)
                if (psClusterInstance != NULL &&
                    (((u8Pass == 1)&&(!bIsUndivided))||(u8Pass == 0)) &&
                    u8errorCode != E_ZCL_CMDS_SUCCESS)
                {
                    // Call the user on error even if its a failed undivided
                    sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeStatus = u8errorCode;
                    psZCL_EndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);
                }


                if (u8errorCode != E_ZCL_CMDS_SUCCESS)
                    bNoErrors = FALSE;
                // update status messages if we encounter syntax problems (first pass only)
                if ((((u8Pass == 1)&&(!bIsUndivided))||(u8Pass == 0)) && u8errorCode != E_ZCL_CMDS_SUCCESS && !bNoResponse)
                {
                    // Update response PDU if any problems
                    // write status
                    u16outputOffset += u16ZCL_APduInstanceWriteNBO(
                        myPDUM_thAPduInstance,
                        u16outputOffset,
                        E_ZCL_UINT8,
                        &u8errorCode);
                    // Write attribute id
                    u16outputOffset += u16ZCL_APduInstanceWriteNBO(
                        myPDUM_thAPduInstance,
                        u16outputOffset,
                        E_ZCL_UINT16,
                        &u16AttributeId);
                }
                // step over the attribute - the error flags will determine whether we go round the loop again
                u16inputOffset += u16typeSize;
            } // End while
        }
    }  // End two pass loop

    // If no errors, write a single OK status with no following att id
    if (bNoErrors && !bNoResponse)
    {
        // write status
        u16outputOffset += u16ZCL_APduInstanceWriteNBO(
            myPDUM_thAPduInstance,
            u16outputOffset,
            E_ZCL_UINT8,
            &u8errorCode);
    }

    // re-use last attribute generated structure for message but indicate command is complete, last attribute
    // will get 2 callbacks effectively
    sZCL_CallBackEvent.eEventType = E_ZCL_CBET_WRITE_ATTRIBUTES;

     if (psClusterInstance != NULL)
     {
        psZCL_EndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);
     }


    // Daniel Nov 09.
    // Do we send back a default resonse (SUCCESS or ERROR) from a write no response?
    // I think I remember read somewhere that said no but I can't find it in the new release of the spec so am not sure
    // this is correct.
    if (!bNoResponse)
    {
        if(sZCL_CallBackEvent.eZCL_Status != E_ZCL_SUCCESS)
        {
            // Can't send a write att response with errors on a fatal error because the rule is that
            // any atts not listed in response are assumed to have been sucesfully written.
            // Started to build a response PDU but incoming command is too corrupt to send it
            // so free the PDU.
            PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
            eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_MALFORMED_COMMAND);
        }
        else
        {
            // build address structure
            eZCL_BuildTransmitAddressStructure(pZPSevent, &sZCL_Address);
            // transmit response
            eZCL_TransmitDataRequest(myPDUM_thAPduInstance,
                u16outputOffset,
                pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                pZPSevent->uEvent.sApsDataIndEvent.u16ClusterId,
                &sZCL_Address);
        }
    }
#ifndef COOPERATIVE
    // Unlock if doing an undivided write
    if (bIsUndivided)
    {
        eZCL_ReleaseMutex(psZCL_EndPointDefinition);
    }
#endif
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
