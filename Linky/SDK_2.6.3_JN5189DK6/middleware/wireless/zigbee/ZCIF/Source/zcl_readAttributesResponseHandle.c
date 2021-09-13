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
uAttribData uAttributeVal;
/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

/****************************************************************************
**
** NAME:       vZCL_HandleAttributesReadResponse
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
** bool_t - TRUE delete input buffer, FALSE don't
**
****************************************************************************/
PUBLIC   void vZCL_HandleAttributesReadResponse(
                    ZPS_tsAfEvent              *pZPSevent,
                    tsZCL_EndPointDefinition   *psZCL_EndPointDefinition,
                    tsZCL_ClusterInstance      *psClusterInstance)
{
    tsZCL_CallBackEvent sZCL_CallBackEvent;
    uint16 u16inputOffset = 0;
    uint16 u16typeSize;
    tsZCL_HeaderParams sZCL_HeaderParams;
    uint16 u16payloadSize;


    // fill in non-attribute specific values in callback event structure
    sZCL_CallBackEvent.u8EndPoint = pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint;
    sZCL_CallBackEvent.psClusterInstance = psClusterInstance;
    sZCL_CallBackEvent.pZPSevent = pZPSevent;
    sZCL_CallBackEvent.eEventType = E_ZCL_CBET_READ_INDIVIDUAL_ATTRIBUTE_RESPONSE;
    sZCL_CallBackEvent.eZCL_Status = E_ZCL_SUCCESS;


    // read incoming request
    u16inputOffset = u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                                              &sZCL_HeaderParams);
    sZCL_CallBackEvent.u8TransactionSequenceNumber = sZCL_HeaderParams.u8TransactionSequenceNumber;

    // get received payload length
    u16payloadSize = PDUM_u16APduInstanceGetPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);


    /* report attribute handling */
    if(sZCL_HeaderParams.u8CommandIdentifier == E_ZCL_REPORT_ATTRIBUTES)
    {

        //Following code is required only for Mirroring in SE
        //Call-back is generated to validate the ZCL attribute report,
        //Based on the received Status byte flow is continued, else returned with default response
        if ((psClusterInstance != NULL) && (psClusterInstance->psClusterDefinition->u8ClusterControlFlags & CLUSTER_MIRROR_BIT))
        {

             sZCL_CallBackEvent.eEventType = E_ZCL_CBET_ATTRIBUTE_REPORT_MIRROR;
             sZCL_CallBackEvent.uMessage.sReportAttributeMirror.u8DestinationEndPoint = pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint;
             sZCL_CallBackEvent.uMessage.sReportAttributeMirror.u16ClusterId = psClusterInstance->psClusterDefinition->u16ClusterEnum;

             if (pZPSevent->uEvent.sApsDataIndEvent.u8SrcAddrMode == E_ZCL_AM_SHORT)
             {

#ifdef PC_PLATFORM_BUILD
                 sZCL_CallBackEvent.uMessage.sReportAttributeMirror.u64RemoteIeeeAddress  = IEEE_SRC_ADD;
#else
                 // Find out IEEE address from the received Short address
                 sZCL_CallBackEvent.uMessage.sReportAttributeMirror.u64RemoteIeeeAddress = ZPS_u64NwkNibFindExtAddr((void *)ZPS_pvNwkGetHandle(), pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr);
#endif
             }
             else
             {
                sZCL_CallBackEvent.uMessage.sReportAttributeMirror.u64RemoteIeeeAddress = (uint64) pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u64Addr;
             }

             sZCL_CallBackEvent.uMessage.sReportAttributeMirror.eStatus = E_ZCL_ATTR_REPORT_ERR;

             psZCL_EndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

             if ((sZCL_CallBackEvent.uMessage.sReportAttributeMirror.eStatus == E_ZCL_ATTR_REPORT_ERR) ||
                     (sZCL_CallBackEvent.uMessage.sReportAttributeMirror.eStatus == E_ZCL_ATTR_REPORT_EP_MISMATCH) ||
                     (sZCL_CallBackEvent.uMessage.sReportAttributeMirror.eStatus == E_ZCL_ATTR_REPORT_ADDR_MISMATCH))
             {
                 eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_AUTHORIZED);
                 return;
             }
        }

        sZCL_CallBackEvent.eEventType = E_ZCL_CBET_REPORT_INDIVIDUAL_ATTRIBUTE;
    }

    // parse the message whilst checking for malformed messages
    while((u16payloadSize-u16inputOffset > 0) && (sZCL_CallBackEvent.eZCL_Status != E_ZCL_ERR_MALFORMED_MESSAGE))   {

        // read the attribute record from the message
        // read attribute Id
        u16inputOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_ATTRIBUTE_ID,
                &sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.u16AttributeEnum);

        // For report attribute make sure the status is set to SUCCESS before callback
        sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeStatus = E_ZCL_CMDS_SUCCESS;
                
        if(sZCL_HeaderParams.u8CommandIdentifier== E_ZCL_READ_ATTRIBUTES_RESPONSE)
        {
            // read and check status
            u16inputOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT8,
                    &sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeStatus);
        }

        // unknown for now
        sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeDataType = E_ZCL_UNKNOWN;

        // null data and status pointers for now
        sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.pvAttributeData = NULL;

        if((sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeStatus ==E_ZCL_CMDS_SUCCESS) ||
                (sZCL_HeaderParams.u8CommandIdentifier == E_ZCL_REPORT_ATTRIBUTES))
        {

            // parse the attibute record further
            u16inputOffset +=u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT8,
                    &sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeDataType);
            // read the attribute data type
            if(eZCL_GetAttributeTypeSizeFromBuffer(sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeDataType,
                    pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,u16inputOffset, &u16typeSize)!=E_ZCL_SUCCESS)
            {

                if(u16typeSize==0)
                {
                    // we have no idea how to locate the next attribute, so abort
                    sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_MALFORMED_MESSAGE;
                }

            }

           switch(sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeDataType)
           {
               case(E_ZCL_LOSTRING):
               case(E_ZCL_LCSTRING):
               case(E_ZCL_OSTRING):
               case(E_ZCL_CSTRING):
               {
                   uint16                     u16stringDataLength;
                   uint16                     u16stringDataLengthField;
                   uint8                     *pu8stringData;
                 //  uint8                     u8stringLengthPosition;

                  /* if(eZCL_GetAttributeTypeSizeFromBuffer(sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeDataType,
                                                          pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                                                          u16inputOffset,
                                                          &u16typeSize)!= E_ZCL_SUCCESS)
                   {
                       sZCL_CallBackEvent.eZCL_Status  = E_ZCL_FAIL;
                   }*/
                   u16typeSize = u16ZCL_GetStringAttributeTypeSizeFromBuffer(
                           sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeDataType,
                                           &u16stringDataLength,
                                           &u16stringDataLengthField,
                                           &pu8stringData,
                                           (uint8 *)(PDUM_pvAPduInstanceGetPayload(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst)) + u16inputOffset);

                   if((sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeDataType ==E_ZCL_OSTRING ) ||
                           (sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeDataType ==E_ZCL_CSTRING))
                   {
                       uAttributeVal.sStrData.u8Length = (uint8 )u16stringDataLength;
                       uAttributeVal.sStrData.pu8Data = pu8stringData;
                   }
                   else
                   {
                       uAttributeVal.sLString.u16Length = u16stringDataLength;
                       uAttributeVal.sLString.pu8Data = pu8stringData;
                   }
               }
               break;
               default:
               {
                    u16ZCL_APduInstanceReadNBO(
                             pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             u16inputOffset,
                             sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.eAttributeDataType,
                             &(uAttributeVal.sKeyData.au8Key[0]));
               }
               break;
           }
           sZCL_CallBackEvent.uMessage.sIndividualAttributeResponse.pvAttributeData = (void *)&uAttributeVal;

           // step over the attribute - the error flags will determine whether we go round the loop again
           u16inputOffset += u16typeSize;
        }

        // call user for every attribute
        psZCL_EndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);
    }

    // re-use last attribute generated structure for message but indicate command is complete, last attribute
    // will get 2 callbacks effectively
    sZCL_CallBackEvent.eEventType = E_ZCL_CBET_READ_ATTRIBUTES_RESPONSE;
    if(sZCL_HeaderParams.u8CommandIdentifier == E_ZCL_REPORT_ATTRIBUTES)
    {
        sZCL_CallBackEvent.eEventType = E_ZCL_CBET_REPORT_ATTRIBUTES;
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
