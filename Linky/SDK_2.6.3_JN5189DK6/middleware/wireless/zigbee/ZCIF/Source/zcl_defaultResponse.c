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
 * COMPONENT:          zcl_defaultResponse.c
 *
 * DESCRIPTION:       ZCL top level functions
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
 ** NAME:       eZCL_SendDefaultResponse
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:               Name                    Usage
 ** ZPS_tsAfEvent              *pZPSevent               Zigbee Event Structure
 ** teZCL_CommandStatus       eZCLcommandStatus       Error code
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_SendDefaultResponse(
                       ZPS_tsAfEvent              *pZPSevent,
                       teZCL_CommandStatus         eZCLcommandStatus)
{

    uint16 u16offset;
    uint16 u16payloadSize;

    tsZCL_HeaderParams sZCL_HeaderParams;

    teZCL_ZCLSendSecurity  eSecuritySupportedSaved =  psZCL_Common->eSecuritySupported;
    tsZCL_Address         sZCL_Address;

    PDUM_thAPduInstance myPDUM_thAPduInstance;


    #ifdef STRICT_PARAM_CHECK
        // parameter checks
        if(pZPSevent == NULL)
        {
            return(E_ZCL_ERR_PARAMETER_NULL);
        }

        if(pZPSevent->eType != ZPS_EVENT_APS_DATA_INDICATION)
        {
            // can't send a response
            return(E_ZCL_FAIL);
        }
    #endif

    // read incoming request command header to get response setting
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                                              &sZCL_HeaderParams);

    if ((sZCL_HeaderParams.bDisableDefaultResponse) && (eZCLcommandStatus == E_ZCL_CMDS_SUCCESS))
    {
        return E_ZCL_CMDS_SUCCESS; 
    }

    if ((pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode != ZPS_E_ADDR_MODE_SHORT) &&
        (pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode != ZPS_E_ADDR_MODE_IEEE))
    {
        return eZCLcommandStatus;
    }

    /* Avoid sending default response for a received broadcast request;
       Address >= 0xFFF8 includes reserved and broadcast address  */
    if ((pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode == ZPS_E_ADDR_MODE_SHORT) &&
        (pZPSevent->uEvent.sApsDataIndEvent.uDstAddress.u16Addr >= 0xFFF8))
    {
        return eZCLcommandStatus;
    }

    // get buffer
    myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();

    if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
    {
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }

    // write command header - using old values where appropriate
    u16offset = u16ZCL_WriteCommandHeader(
                                       myPDUM_thAPduInstance,
                                       eFRAME_TYPE_COMMAND_ACTS_ACCROSS_ENTIRE_PROFILE,
                                       sZCL_HeaderParams.bManufacturerSpecific,
                                       sZCL_HeaderParams.u16ManufacturerCode,
                                       !sZCL_HeaderParams.bDirection,
                                       TRUE,
                                       sZCL_HeaderParams.u8TransactionSequenceNumber,
                                       E_ZCL_DEFAULT_RESPONSE);

    // calculate payload size
    u16payloadSize = u16offset + 2;
    // check buffer size
    if(PDUM_u16APduGetSize(psZCL_Common->hZCL_APdu) < u16payloadSize)
    {
        // free buffer and return
        PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }

    // write payload - the command ID of the incoming command that caused the response followed by the error code
    u16offset +=  u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &(sZCL_HeaderParams.u8CommandIdentifier));
    u16offset +=  u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &eZCLcommandStatus);

    // build address structure
    eZCL_BuildTransmitAddressStructure(pZPSevent, &sZCL_Address);


    // 5.4.6 Cluster Usage of Security Keys
    // NTS compliance testing - if the message was sent to us with only NWK security,
    // send the default response back with NWK security
    if (pZPSevent->uEvent.sApsDataIndEvent.eSecurityStatus != ZPS_APL_APS_E_SECURED_LINK_KEY)
    {
         psZCL_Common->eSecuritySupported  = E_ZCL_SECURITY_NETWORK;
    }


    // transmit request - reverse event parameters - src and dst
    if (eZCL_TransmitDataRequest(myPDUM_thAPduInstance,
                                u16payloadSize,
                                pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                pZPSevent->uEvent.sApsDataIndEvent.u16ClusterId,
                                &sZCL_Address) != E_ZCL_SUCCESS)
    {
        psZCL_Common->eSecuritySupported = eSecuritySupportedSaved;
        return(E_ZCL_ERR_ZTRANSMIT_FAIL);
    }
    psZCL_Common->eSecuritySupported = eSecuritySupportedSaved;
    return(E_ZCL_SUCCESS);
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
