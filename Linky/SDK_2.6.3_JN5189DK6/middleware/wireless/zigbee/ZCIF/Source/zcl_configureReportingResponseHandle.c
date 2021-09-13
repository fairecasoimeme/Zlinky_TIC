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
 * COMPONENT:          zcl_readAttributesResponseHandle.c
 *
 * DESCRIPTION:       Receive a Read Attributes Response
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
** NAME:       vZCL_HandleConfigureReportingResponse
**
** DESCRIPTION:
**
**
** PARAMETERS:               Name                      Usage
** ZPS_tsAfEvent            *pZPSevent                 Zigbee Stack Event
** tsZCL_EndPointDefinition *psZCL_EndPointDefinition  EP structure
** tsZCL_ClusterInstance    *psClusterInstance     Cluster structure
**
** RETURN:
** bool_t - TRUE delete input buffer, FALSE don't
**
****************************************************************************/

PUBLIC  void vZCL_HandleConfigureReportingResponse(
    ZPS_tsAfEvent              *pZPSevent,
    tsZCL_EndPointDefinition   *psZCL_EndPointDefinition,
    tsZCL_ClusterInstance      *psClusterInstance)
{
    tsZCL_CallBackEvent sZCL_CallBackEvent = {0};

    uint16 u16inputOffset;

    tsZCL_HeaderParams sZCL_HeaderParams;



    uint16 u16payloadSize;

    // fill in non-attribute specific values in callback event structure
    sZCL_CallBackEvent.u8EndPoint = pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint;
    sZCL_CallBackEvent.psClusterInstance = psClusterInstance;
    sZCL_CallBackEvent.pZPSevent = pZPSevent;
    sZCL_CallBackEvent.eEventType = E_ZCL_CBET_REPORT_INDIVIDUAL_ATTRIBUTES_CONFIGURE_RESPONSE;
    sZCL_CallBackEvent.eZCL_Status = E_ZCL_SUCCESS;

    if (psClusterInstance == NULL)
    {
        sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_CLUSTER_NOT_FOUND;
        sZCL_CallBackEvent.eEventType = E_ZCL_CBET_ERROR;
    }

    // build command packet for response
    // read incoming request
    u16inputOffset = u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                                              &sZCL_HeaderParams);
    sZCL_CallBackEvent.u8TransactionSequenceNumber = sZCL_HeaderParams.u8TransactionSequenceNumber;

    if (sZCL_CallBackEvent.eEventType != E_ZCL_CBET_ERROR)
    {
        // parse the incoming message, read each attribute from the device and write into the outgoing buffer
        u16payloadSize = PDUM_u16APduInstanceGetPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);

        // fill in non-attribute specific values in callback event structure
        sZCL_CallBackEvent.u8EndPoint = pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint;
        sZCL_CallBackEvent.psClusterInstance = psClusterInstance;
        sZCL_CallBackEvent.pZPSevent = pZPSevent;
        sZCL_CallBackEvent.eEventType = E_ZCL_CBET_REPORT_INDIVIDUAL_ATTRIBUTES_CONFIGURE_RESPONSE;
        sZCL_CallBackEvent.eZCL_Status = E_ZCL_SUCCESS;

        // check if response has any errors, if its equal to 1 then its a rip-roaring success and we have no
        // payload to check as the first byte will be a ststus byte of E_ZCL_CMDS_SUCCESS
        if(u16payloadSize-u16inputOffset > 1)
        {
            // parse the message whilst checking for malformed messages
            while((u16payloadSize-u16inputOffset > 0) && (sZCL_CallBackEvent.eZCL_Status == E_ZCL_SUCCESS))
            {
                // read the attribute report record from the message
                // read and check status
                u16inputOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT8, &sZCL_CallBackEvent.uMessage.sReportingConfigurationResponse.u8Status);
                u16inputOffset += u16ZCL_APduInstanceReadNBO(
                    pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT8, &sZCL_CallBackEvent.uMessage.sReportingConfigurationResponse.u8DirectionIsReceived);
                // read attribute Id
                u16inputOffset += u16ZCL_APduInstanceReadNBO(
                    pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_ATTRIBUTE_ID, &sZCL_CallBackEvent.uMessage.sReportingConfigurationResponse.u16AttributeEnum);

                // call user for every attribute
                if(sZCL_CallBackEvent.uMessage.sReportingConfigurationResponse.u8Status != E_ZCL_CMDS_SUCCESS)
                {
                    psZCL_EndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);
                }

                // check for length error
                if((u16payloadSize-u16inputOffset !=0) && (u16payloadSize-u16inputOffset < 3))
                {
                    sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_MALFORMED_MESSAGE;
                }
            }
        }

        sZCL_CallBackEvent.eEventType = E_ZCL_CBET_REPORT_ATTRIBUTES_CONFIGURE_RESPONSE;
    }

    psZCL_EndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

    if(sZCL_CallBackEvent.eZCL_Status != E_ZCL_SUCCESS)
    {
        // early termination send default response indicating error
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_MALFORMED_COMMAND);
        // free buffer in return
    }
    else
    {
        // incoming message is now parsed ok - send the default OK, if required
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SUCCESS);
    }

}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
