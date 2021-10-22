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
 * COMPONENT:          zcl_configureReportingCommandHandle.c
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


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void vZCL_CalculateNumberOfReportRecords(
                        ZPS_tsAfEvent               *pZPSevent,
                        tsZCL_EndPointDefinition    *psEndPointDefinition,
                        tsZCL_ClusterInstance       *psClusterInstance,
                        uint8                       *pu8NumberReportsInRequest,
                        uint8                       *pu8NumberValidReportsInRequest);

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
 ** NAME:       vZCL_HandleConfigureReportingCommand
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent             *pZPSevent                Zigbee Stack Event
 ** tsZCL_EndPointDefinition *psEndPointDefinition  EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/

PUBLIC  void vZCL_HandleConfigureReportingCommand(
                                             ZPS_tsAfEvent              *pZPSevent,
                                             tsZCL_EndPointDefinition   *psEndPointDefinition,
                                             tsZCL_ClusterInstance      *psClusterInstance)
{
    int i;

    tsZCL_CallBackEvent sZCL_CallBackEvent;

    uint16 u16inputOffset, u16outputOffset = 0, u16outputStartOffset = 0;

     tsZCL_HeaderParams sZCL_HeaderParams;

    uint8   u8NumberReportsInRequest;
    uint8   u8NumberValidReportsInRequest;



    tsZCL_Address sZCL_Address;

    uint16 u16payloadSize;
    uint16 u16responseBufferSize = 0;

    uint8 u8typeSize;

    tsZCL_AttributeDefinition   *psAttributeDefinition;

    PDUM_thAPduInstance myPDUM_thAPduInstance = 0;

    uint16 u16attributeIndex;
    uint8 u8NumberOfEntries;
    tsZCL_AttributeReportingConfigurationRecord    *psAttributeReportingRecord = &sZCL_CallBackEvent.uMessage.sAttributeReportingConfigurationRecord;

    void *pvReportableChange;

    bool_t bDontReReadInputMessage;
    bool_t bBufferAllocated;
    bool_t bCommandSuccess;

    teZCL_CommandStatus eCommandStatus;
    teZCL_Status        eZCL_Status = E_ZCL_SUCCESS;
    // build command packet for response
    // read incoming request
    u16inputOffset =  u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // set user callback
    sZCL_CallBackEvent.pZPSevent = pZPSevent;
    sZCL_CallBackEvent.u8TransactionSequenceNumber = sZCL_HeaderParams.u8TransactionSequenceNumber;
    sZCL_CallBackEvent.u8EndPoint = pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint;
    sZCL_CallBackEvent.eEventType = E_ZCL_CBET_REPORT_INDIVIDUAL_ATTRIBUTES_CONFIGURE;
    sZCL_CallBackEvent.psClusterInstance = psClusterInstance;
    sZCL_CallBackEvent.eZCL_Status = E_ZCL_SUCCESS;

    // number of reports in request
    vZCL_CalculateNumberOfReportRecords(
    pZPSevent, psEndPointDefinition, psClusterInstance, &u8NumberReportsInRequest, &u8NumberValidReportsInRequest);

    eZCL_GetNumberOfFreeReportRecordEntries(&u8NumberOfEntries);

    if((u8NumberReportsInRequest==0) || (psClusterInstance == NULL))
    {
        // fail if no report entries
        // don't check u8NumberValidReportsInRequest as if there are any errors we have to respond to them individually
        //
        // Don't call back user on a null cluster but we are allowed to send back a default response despite Trac 60.
        // There is a separate paragraph in the ZCL that says config report and read report config are only mandatory
        // for clusters with reportable attributes.
        if (psClusterInstance != NULL)
        {
            sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_NO_REPORT_ENTRIES;
            psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);
        }
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SOFTWARE_FAILURE);
        return;
    }

    // M$
    psAttributeReportingRecord->eAttributeDataType = 0;
    // payload investigation
    u16payloadSize = PDUM_u16APduInstanceGetPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);
    // size sanity check
    if(u16payloadSize < u16inputOffset)
    {
        // send response if possible/required
        sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_MALFORMED_MESSAGE;
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_MALFORMED_COMMAND);
        return;
    }

    // build address structure
    eZCL_BuildTransmitAddressStructure(pZPSevent, &sZCL_Address);

    // read attributes from the device

    // parse the incoming message, read each attribute from the device and write into the outgoing buffer

    // loop round the report list
    i=0;
    bDontReReadInputMessage=FALSE;
    bBufferAllocated=FALSE;
    bCommandSuccess = FALSE;

    while(i<u8NumberReportsInRequest)
    {
        // (re)set status for loop
        eCommandStatus = E_ZCL_CMDS_SUCCESS;
        sZCL_CallBackEvent.eZCL_Status = E_ZCL_SUCCESS;

        if(bDontReReadInputMessage==FALSE)
        {
            // get report entry
            u16inputOffset += u16ZCL_APduInstanceReadNBO(
                pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT8, &psAttributeReportingRecord->u8DirectionIsReceived);
            u16inputOffset += u16ZCL_APduInstanceReadNBO(
                pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT16, &psAttributeReportingRecord->u16AttributeEnum);

            // find attribute definition
            if(eZCL_SearchForAttributeEntry(
                psEndPointDefinition->u8EndPointNumber, psAttributeReportingRecord->u16AttributeEnum, sZCL_HeaderParams.bManufacturerSpecific, sZCL_HeaderParams.bDirection, psClusterInstance, &psAttributeDefinition, &u16attributeIndex)!= E_ZCL_SUCCESS)
            {
                // not supported - but we can still carry on
                eCommandStatus = E_ZCL_CMDS_UNSUPPORTED_ATTRIBUTE;
                sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_ATTRIBUTE_NOT_FOUND;
            }
            // All except LSB are reserved and so we must ignore their values
            psAttributeReportingRecord->u8DirectionIsReceived &= 0x01;

            if((psAttributeReportingRecord->u8DirectionIsReceived == 0) &&
               (eCommandStatus == E_ZCL_CMDS_SUCCESS))
            {
                u16inputOffset += u16ZCL_APduInstanceReadNBO(
                    pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT8, &psAttributeReportingRecord->eAttributeDataType);
                 // check size to see if the implementation supports the data type requested
                if((eZCL_GetAttributeTypeSize(psAttributeReportingRecord->eAttributeDataType, &u8typeSize) != E_ZCL_SUCCESS) ||
                    (psAttributeDefinition->eAttributeDataType != psAttributeReportingRecord->eAttributeDataType))
                {
                    // not supported - but we can still carry on
                    eCommandStatus = E_ZCL_CMDS_INVALID_DATA_TYPE;
                    sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_ATTRIBUTE_TYPE_UNSUPPORTED;
                }

                // check valid type for report command
                if((psAttributeReportingRecord->eAttributeDataType ==  E_ZCL_ARRAY)      ||
                   (psAttributeReportingRecord->eAttributeDataType ==  E_ZCL_STRUCT)     ||
                   (psAttributeReportingRecord->eAttributeDataType ==  E_ZCL_SET)        ||
                   (psAttributeReportingRecord->eAttributeDataType ==  E_ZCL_BAG)        ||
                   (psAttributeReportingRecord->eAttributeDataType ==  E_ZCL_UNKNOWN)
                   )
                {
                    // not supported
                    eCommandStatus = E_ZCL_CMDS_UNREPORTABLE_ATTRIBUTE;
                    sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_ATTRIBUTE_NOT_REPORTABLE;
                }

                if(psAttributeDefinition != NULL)
                {
                    //  stream and local attribute types match check
                    if(((psAttributeDefinition->u8AttributeFlags & E_ZCL_AF_RD) != E_ZCL_AF_RD) ||
                      (ZCL_IS_BIT_CLEAR(uint8,psClusterInstance->pu8AttributeControlBits[u16attributeIndex],E_ZCL_ACF_RP)) ||
                      (bZCL_CheckAttributeDirectionFlagMatch(psAttributeDefinition,psClusterInstance->bIsServer) == FALSE)
                    )
                    {
                        eCommandStatus = E_ZCL_CMDS_UNREPORTABLE_ATTRIBUTE;
                        sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_ATTRIBUTE_NOT_REPORTABLE;
                    }
                }

                //read reporting intervals
                u16inputOffset += u16ZCL_APduInstanceReadNBO(
                pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT16, &psAttributeReportingRecord->u16MinimumReportingInterval);
                u16inputOffset += u16ZCL_APduInstanceReadNBO(
                pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT16, &psAttributeReportingRecord->u16MaximumReportingInterval);
                if((psAttributeReportingRecord->u16MaximumReportingInterval) && (psAttributeReportingRecord->u16MaximumReportingInterval < psAttributeReportingRecord->u16MinimumReportingInterval))
                {
                    eCommandStatus = E_ZCL_CMDS_INVALID_VALUE;
                    sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_INVALID_VALUE;
                }
                
                if((psAttributeReportingRecord->u16MinimumReportingInterval == REPORTING_MINIMUM_NOT_SET) && 
                    (psAttributeReportingRecord->u16MaximumReportingInterval == REPORTING_MAXIMUM_PERIODIC_TURNED_OFF) &&
                     (eCommandStatus == E_ZCL_CMDS_SUCCESS))
                {
                    sZCL_CallBackEvent.eZCL_Status = E_ZCL_RESTORE_DEFAULT_REPORT_CONFIGURATION;
                }
                
                if(eZCL_DoesAttributeHaveReportableChange(psAttributeReportingRecord->eAttributeDataType)== E_ZCL_SUCCESS)
                {
                    if(eCommandStatus == E_ZCL_CMDS_INVALID_DATA_TYPE)
                    {
                        // we are stuffed - can't parse the message anymore
                        eCommandStatus = E_ZCL_CMDS_SOFTWARE_FAILURE;
                        sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_ATTRIBUTE_TYPE_UNSUPPORTED;
                        break;
                    }

                    // read reportable change into structure
                    eZCL_GetAttributeReportableEntry(
                   &pvReportableChange, psAttributeReportingRecord->eAttributeDataType, &psAttributeReportingRecord->uAttributeReportableChange);
                    u16inputOffset += u16ZCL_APduInstanceReadNBO(
                    pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, psAttributeReportingRecord->eAttributeDataType, pvReportableChange);
                }
            }
            else
            {
                u16inputOffset += u16ZCL_APduInstanceReadNBO(
                pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT16, &psAttributeReportingRecord->u16TimeoutPeriodField);
            }
        }

        if(bBufferAllocated==FALSE)
        {
            // get buffer to write the response in
            myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();
            // no buffers - break out of the loop
            if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
            {
                eCommandStatus = E_ZCL_CMDS_SOFTWARE_FAILURE;
                sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_ZBUFFER_FAIL;
                break;
            }

            // size of outgoing buffer
            u16responseBufferSize = u16ZCL_GetTxPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr);

            // modify and write back
            u16outputOffset = u16ZCL_WriteCommandHeader(myPDUM_thAPduInstance,
                                                   sZCL_HeaderParams.eFrameType,
                                                   sZCL_HeaderParams.bManufacturerSpecific,
                                                   sZCL_HeaderParams.u16ManufacturerCode,
                                                   !sZCL_HeaderParams.bDirection,
                                                   psEndPointDefinition->bDisableDefaultResponse,
                                                   sZCL_HeaderParams.u8TransactionSequenceNumber,
                                                   E_ZCL_CONFIGURE_REPORTING_RESPONSE);

            // store start of message and indicate buffer allocated
            u16outputStartOffset = u16outputOffset;
            bBufferAllocated = TRUE;

        }

        // check if report will fit in buffer
        if((4 + u16outputOffset) > u16responseBufferSize)
        {
            bBufferAllocated = FALSE;

            if(u16outputStartOffset != u16outputOffset)
            {
                // can't fit report in - transmit and continue
                if(eZCL_TransmitDataRequest(myPDUM_thAPduInstance,
                                    u16outputOffset,
                                    pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                    pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                    pZPSevent->uEvent.sApsDataIndEvent.u16ClusterId,
                                    &sZCL_Address) != E_ZCL_SUCCESS)
                {
                    eCommandStatus = E_ZCL_CMDS_SOFTWARE_FAILURE;
                    sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_ZTRANSMIT_FAIL;
                    break;
                }
                // don't increment loop count we haven't processed any more reports just sent what we already had
                bDontReReadInputMessage=TRUE;
                continue;
            }
            else
            {
                // we are skrewed buffer(s) too small

                eCommandStatus = E_ZCL_CMDS_SOFTWARE_FAILURE;
                sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_ZTRANSMIT_FAIL;
                break;
            }
        }

        // add report to table if valid
        if(eCommandStatus == E_ZCL_CMDS_SUCCESS)
        {
            eZCL_Status = eZCLAddReport(psEndPointDefinition, psClusterInstance, psAttributeDefinition, psAttributeReportingRecord);
            // report config error
            if(eZCL_Status == E_ZCL_ERR_INVALID_VALUE)
            {
                // bad report request
                sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_INVALID_VALUE;
                eCommandStatus = E_ZCL_CMDS_INVALID_VALUE;
            }
            // no report storage resources left
            if(eZCL_Status == E_ZCL_ERR_NO_REPORT_ENTRIES)
            {
                // no code for full so just go with this
                sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_NO_REPORT_ENTRIES;
                eCommandStatus = E_ZCL_CMDS_SOFTWARE_FAILURE;
            }
            
        }

        // DON'T COMBINE THE 2 IF's INTO AN IF/ELSE

        // if failure write into response buffer
        if(eCommandStatus != E_ZCL_CMDS_SUCCESS)
        {
            // write status
            u16outputOffset += u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16outputOffset, E_ZCL_UINT8, &eCommandStatus);
            // write direction
            u16outputOffset += u16ZCL_APduInstanceWriteNBO(
            myPDUM_thAPduInstance, u16outputOffset, E_ZCL_UINT8, &psAttributeReportingRecord->u8DirectionIsReceived);
            // write response into message
            u16outputOffset += u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16outputOffset, E_ZCL_ATTRIBUTE_ID, &psAttributeReportingRecord->u16AttributeEnum);
        }
        else
        {
            // indicate command success - nothing is written into the buffer in this case
            bCommandSuccess = TRUE;
        }

        // call user
        psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

        // increment attribute count
        i++;
    }

    // user command terminate callback
    sZCL_CallBackEvent.eEventType = E_ZCL_CBET_REPORT_ATTRIBUTES_CONFIGURE;
    psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

    // transmit
    if((sZCL_CallBackEvent.eZCL_Status == E_ZCL_SUCCESS)                        ||
       (sZCL_CallBackEvent.eZCL_Status == E_ZCL_ERR_INVALID_VALUE)              ||
       (sZCL_CallBackEvent.eZCL_Status == E_ZCL_ERR_NO_REPORT_ENTRIES)          ||
       (sZCL_CallBackEvent.eZCL_Status == E_ZCL_ERR_ATTRIBUTE_NOT_REPORTABLE)   ||
       (sZCL_CallBackEvent.eZCL_Status == E_ZCL_ERR_ATTRIBUTE_NOT_FOUND)        ||
       (sZCL_CallBackEvent.eZCL_Status == E_ZCL_ERR_ATTRIBUTE_TYPE_UNSUPPORTED) ||
       (sZCL_CallBackEvent.eZCL_Status == E_ZCL_RESTORE_DEFAULT_REPORT_CONFIGURATION)
    )
    {
        // transmit request
        if(bBufferAllocated==TRUE)
        {
            // check to see if we have had all successes for the records
            if((bCommandSuccess==TRUE) && (u16outputStartOffset == u16outputOffset))
            {
                // eCommandStatus = E_ZCL_CMDS_SUCCESS - already set on way out of loop
                // write a single success code
                u16outputOffset += u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16outputOffset, E_ZCL_UINT8, &eCommandStatus);
            }

            eZCL_TransmitDataRequest(myPDUM_thAPduInstance,
                                     u16outputOffset,
                                     pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                     pZPSevent->uEvent.sApsDataIndEvent.u16ClusterId,
                                     &sZCL_Address);

        }
        // NTS testing Nov 09. Don't send a good default response here as we have a unicast response that is valid.

    }
    else
    {
        // error buffer too small for 1 record free buffer and return
        eZCL_SendDefaultResponse(pZPSevent, eCommandStatus);
        // don't send the message
        if(bBufferAllocated==TRUE)
        {
            // free buffer and return
            PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
        }
    }


}

/****************************************************************************
 **
 ** NAME:       vZCL_CalculateNumberOfReportRecords
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent            *pZPSevent                     Zigbee Stack Event
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8NumberReportsInRequest     Number of reports in message
 ** uint8                    *pu8NumberValidReportsInRequest Number of valid reports in message
 **
 ** RETURN:
 ** Nothing
 **
 ****************************************************************************/

PRIVATE void vZCL_CalculateNumberOfReportRecords(
                        ZPS_tsAfEvent               *pZPSevent,
                        tsZCL_EndPointDefinition    *psEndPointDefinition,
                        tsZCL_ClusterInstance       *psClusterInstance,
                        uint8                       *pu8NumberReportsInRequest,
                        uint8                       *pu8NumberValidReportsInRequest)
{

    tsZCL_HeaderParams sZCL_HeaderParams;

    bool_t bSupported;
    teZCL_Status eReturnValue;
    // M$
    teZCL_ZCLAttributeType eAttributeDataType=0;
    uint8 u8Direction, u8typeSize;
    uint16  u16AttributeEnum, u16attributeIndex, u16payloadSize, u16inputOffset;
    tsZCL_AttributeDefinition   *psAttributeDefinition;

    *pu8NumberReportsInRequest = 0;
    *pu8NumberValidReportsInRequest = 0;

    // build command packet for response
    // read incoming request
    u16inputOffset = u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

   // payload investigation
    u16payloadSize = PDUM_u16APduInstanceGetPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);

    while((u16payloadSize-u16inputOffset) > 0)
    {
        bSupported=TRUE;
        // always increment this
        (*pu8NumberReportsInRequest)++;

        u16inputOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT8, &u8Direction);
        u16inputOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_ATTRIBUTE_ID, &u16AttributeEnum);

        // find attribute definition
        eReturnValue = eZCL_SearchForAttributeEntry(psEndPointDefinition->u8EndPointNumber, u16AttributeEnum, sZCL_HeaderParams.bManufacturerSpecific, sZCL_HeaderParams.bDirection, psClusterInstance, &psAttributeDefinition, &u16attributeIndex);
        if(eReturnValue != E_ZCL_SUCCESS)
        {
            // not supported - we can still carry on parsing the stream
            bSupported=FALSE;
        }

        // Must only check bottom bit as others are reserved
        if((u8Direction & 0x01) ==0)
        {
            // read attribute type and check against local reference
            u16inputOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT8, &eAttributeDataType);

            // check valid type for report command
            if((eAttributeDataType ==  E_ZCL_ARRAY)      ||
               (eAttributeDataType ==  E_ZCL_STRUCT)     ||
               (eAttributeDataType ==  E_ZCL_SET)        ||
               (eAttributeDataType ==  E_ZCL_BAG)        ||
               (eAttributeDataType ==  E_ZCL_UNKNOWN)
               )
            {
                // we can't carry on parsing the stream as we don't know if the s/w
                // that produced the stream 'thinks' this is reportable or not
                return;
            }

            // more checks if we've found the attribute on the device
            if(psAttributeDefinition != NULL)
            {
                if((eAttributeDataType != psAttributeDefinition->eAttributeDataType) ||
                  ((psAttributeDefinition->u8AttributeFlags & E_ZCL_AF_RD) != E_ZCL_AF_RD) ||
                  (bZCL_CheckAttributeDirectionFlagMatch(psAttributeDefinition,psClusterInstance->bIsServer) == FALSE)
                )
                {
                    // not supported - we can still carry on parsing the stream
                    bSupported=FALSE;
                }
            }

            // check if the reportable value is in the payload
            u16inputOffset += 2 + 2;
            if(eZCL_DoesAttributeHaveReportableChange(eAttributeDataType)==E_ZCL_SUCCESS)
            {
                if(eZCL_GetAttributeTypeSize(eAttributeDataType, &u8typeSize) !=E_ZCL_SUCCESS)
                {
                    // we can't continue due to the variable length we can't identify
                    return;
                }
                u16inputOffset += u8typeSize;
            }
        }
        else
        {
            // step over remaining data
            u16inputOffset += 2;
        }

        // not supported
        if(bSupported==TRUE)
        {
            (*pu8NumberValidReportsInRequest)++;
        }
    }

}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
