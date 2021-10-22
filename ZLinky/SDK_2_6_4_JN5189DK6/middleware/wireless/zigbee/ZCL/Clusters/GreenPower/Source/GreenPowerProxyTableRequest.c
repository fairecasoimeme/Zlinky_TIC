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


/****************************************************************************
 *
 * MODULE:             ZGP Proxy Table Request command send and receive functions
 *
 * COMPONENT:          GreenPowerProxyTableRequest.c
 *
 * DESCRIPTION:        Proxy table request send and receive functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "GreenPower.h"
#include "GreenPower_internal.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define APP_ID_MASK_PROXY_TABLE_REQ               (7)
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
 ** NAME:       eGP_ProxyTableRequestSend
 **
 ** DESCRIPTION:
 ** Sends Proxy Table request command
 **
 ** PARAMETERS:         Name                           Usage
 ** uint8               u8SourceEndPointId             Source EP Id
 ** uint8               u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address      *psDestinationAddress           Destination Address
 ** uint8              *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsGP_ZgpProxyCommissioningModeCmdPayload  *psZgpProxyCommissioningModeCmdPayload  command payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eGP_ProxyTableRequestSend(
                        uint8                               u8SourceEndPointId,
                        uint8                               u8DestEndPointId,
                        tsZCL_Address                       sDestinationAddress,
                        tsGP_ZgpProxyTableRequestCmdPayload  *psZgpProxyTableRequestCmdPayload)
{

    zbmap8 b8Options;
    uint8 u8ItemsInPayload = 2;
    uint8 u8TransactionSequenceNumber;
    tsZCL_TxPayloadItem asPayloadDefinition[4] = { //size included for app id 2
     {1,  E_ZCL_BMAP8,   &psZgpProxyTableRequestCmdPayload->b8Options},
     {1,  E_ZCL_UINT32,  &psZgpProxyTableRequestCmdPayload->uZgpdDeviceAddr.u32ZgpdSrcId},
     {1,  E_ZCL_UINT8,  &psZgpProxyTableRequestCmdPayload->u8Index},
     {1,  E_ZCL_UINT8,  &psZgpProxyTableRequestCmdPayload->u8Index}
    };


    b8Options = psZgpProxyTableRequestCmdPayload->b8Options;

    if(( b8Options & APP_ID_MASK_PROXY_TABLE_REQ) == GP_APPL_ID_8_BYTE)
    {
    	u8ItemsInPayload += 1;
    	asPayloadDefinition[1].eType = E_ZCL_UINT64;
		asPayloadDefinition[1].pvData = &psZgpProxyTableRequestCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
		asPayloadDefinition[2].eType = E_ZCL_UINT8;
		asPayloadDefinition[2].pvData = &psZgpProxyTableRequestCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
		asPayloadDefinition[3].eType = E_ZCL_UINT8;
		asPayloadDefinition[3].pvData = &psZgpProxyTableRequestCmdPayload->u8Index;
    }
    if(b8Options & BIT_MAP_REQUEST_TYPE)
    {
    	u8ItemsInPayload = 2;
    	asPayloadDefinition[1].eType = E_ZCL_UINT8;
		asPayloadDefinition[1].pvData = &psZgpProxyTableRequestCmdPayload->u8Index;
    }
    return eZCL_CustomCommandSend(u8SourceEndPointId,
    						  u8DestEndPointId,
                              &sDestinationAddress,
                              GREENPOWER_CLUSTER_ID,
                              TRUE,
                              E_GP_ZGP_PROXY_TABLE_REQUEST,
                              &u8TransactionSequenceNumber,
                              asPayloadDefinition,
                              FALSE,
                              0,
                              u8ItemsInPayload);

}



/****************************************************************************
 **
 ** NAME:       eGP_ProxyTableResponseReceive
 **
 ** DESCRIPTION:
 ** Handles ProxyTable Response command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent            *pZPSevent                     Zigbee stack event structure
 ** tsGP_ZgpProxyCommissioningModeCmdPayload *psZgpProxyCommissioningModeCmdPayload  command payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_ProxyTableRequestReceive(
                    ZPS_tsAfEvent                               *pZPSevent,
                    uint16                                      u16Offset,
                    tsGP_ZgpProxyTableRequestCmdPayload          *psZgpProxyTableRequestCmdPayload)
{

    uint8 u8TransactionSequenceNumber;
    uint16 u16ActualQuantity;
    zbmap8 b8Options;
    uint8 u8ItemsInPayload = 2;

    tsZCL_RxPayloadItem asPayloadDefinition[4] = { //maximum size considered
     {1, &u16ActualQuantity, E_ZCL_BMAP8,    &psZgpProxyTableRequestCmdPayload->b8Options},
     {1, &u16ActualQuantity, E_ZCL_UINT32,   &psZgpProxyTableRequestCmdPayload->uZgpdDeviceAddr.u32ZgpdSrcId},
     {1, &u16ActualQuantity, E_ZCL_UINT8,   &psZgpProxyTableRequestCmdPayload->u8Index},
     {1, &u16ActualQuantity, E_ZCL_UINT8,   &psZgpProxyTableRequestCmdPayload->u8Index}
    };
    /* Read Options field from received command */
    u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                               u16Offset,
                               E_ZCL_BMAP8,
                               &b8Options);


    if(( b8Options & APP_ID_MASK_PROXY_TABLE_REQ) == GP_APPL_ID_8_BYTE)
    {
		u8ItemsInPayload += 1;
		asPayloadDefinition[1].eType = E_ZCL_UINT64;
		asPayloadDefinition[1].pvDestination = &psZgpProxyTableRequestCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
		asPayloadDefinition[2].eType = E_ZCL_UINT8;
		asPayloadDefinition[2].pvDestination = &psZgpProxyTableRequestCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
		asPayloadDefinition[3].eType = E_ZCL_UINT8;
		asPayloadDefinition[3].pvDestination = &psZgpProxyTableRequestCmdPayload->u8Index;

    }
    if(b8Options & BIT_MAP_REQUEST_TYPE)
    {
    	u8ItemsInPayload = 2;
    	asPayloadDefinition[1].eType = E_ZCL_UINT8;
		asPayloadDefinition[1].pvDestination = &psZgpProxyTableRequestCmdPayload->u8Index;
    }
    return eZCL_CustomCommandReceive(pZPSevent,
                              &u8TransactionSequenceNumber,
                              asPayloadDefinition,
                              u8ItemsInPayload,
                              E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);


}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
