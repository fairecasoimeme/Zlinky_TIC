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
 * MODULE:             ZGP Response command
 *
 * COMPONENT:          GreenPowerResponse.c
 *
 * DESCRIPTION:        ZGP Response command send and receive functions
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
#ifndef GP_DISABLE_ZGP_RESPONSE_CMD
/****************************************************************************
 **
 ** NAME:       eGP_ZgpResponseSend
 **
 ** DESCRIPTION:
 ** Sends ZGP Response command
 **
 ** PARAMETERS:                     Name                           Usage
 ** uint8                           u8SourceEndPointId             Source EP Id
 ** uint8                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                   *psDestinationAddress           Destination Address
 ** uint8                           *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsGP_ZgpResponseCmdPayload      *psZgpResponseCmdPayload        Response Cmd Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_ZgpResponseSend(
                    uint8                                  u8SourceEndPointId,
                    uint8                                  u8DestinationEndPointId,
                    tsZCL_Address                          *psDestinationAddress,
                    uint8                                  *pu8TransactionSequenceNumber,
                    tsGP_ZgpResponseCmdPayload             *psZgpResponseCmdPayload)
{
	 uint8  u8ItemsInPayload = 6;
	/* Payload with application id 2 will take an entry more */
    tsZCL_TxPayloadItem asPayloadDefinition[7] = {
            {1,  E_ZCL_BMAP8,    &psZgpResponseCmdPayload->b8Options}, //1
            {1,  E_ZCL_UINT16,   &psZgpResponseCmdPayload->u16TempMasterShortAddr}, //2
            {1,  E_ZCL_BMAP8,    &psZgpResponseCmdPayload->b8TempMasterTxChannel},//3
            {1,  E_ZCL_UINT32,   &psZgpResponseCmdPayload->uZgpdDeviceAddr.u32ZgpdSrcId},//4
            {1,  E_ZCL_UINT8,    &psZgpResponseCmdPayload->eZgpdCmdId},//5
            {1,  E_ZCL_OSTRING,  &psZgpResponseCmdPayload->sZgpdCommandPayload} //6
           };


#ifdef GP_IEEE_ADDR_SUPPORT
    if((psZgpResponseCmdPayload->b8Options & GP_APPLICATION_ID_MASK) == 2)
    {

        asPayloadDefinition[3].eType = E_ZCL_IEEE_ADDR;
        asPayloadDefinition[3].pvData = &psZgpResponseCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;

        /* add Endpoint */
        asPayloadDefinition[4].eType = E_ZCL_UINT8;
        asPayloadDefinition[4].pvData = &psZgpResponseCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;

        asPayloadDefinition[5].eType = E_ZCL_UINT8;
        asPayloadDefinition[5].pvData = &psZgpResponseCmdPayload->eZgpdCmdId;


        asPayloadDefinition[6].eType = E_ZCL_OSTRING;
        asPayloadDefinition[6].pvData = &psZgpResponseCmdPayload->sZgpdCommandPayload;
        asPayloadDefinition[6].u16Quantity = 1;

        u8ItemsInPayload++;
    }
#else
    if((psZgpResponseCmdPayload->b8Options & GP_APPLICATION_ID_MASK) == 2)
    {
        return E_ZCL_ERR_INVALID_VALUE;
    }
#endif


    return eZCL_CustomCommandSend(u8SourceEndPointId,
                              u8DestinationEndPointId,
                              psDestinationAddress,
                              GREENPOWER_CLUSTER_ID,
                              TRUE,
                              E_GP_ZGP_RESPONSE,
                              pu8TransactionSequenceNumber,
                              asPayloadDefinition,
                              FALSE,
                              0,
                              u8ItemsInPayload);

}

/****************************************************************************
 **
 ** NAME:       eGP_ZgpResponseReceive
 **
 ** DESCRIPTION:
 ** Handles ZGP response comamnd
 **
 ** PARAMETERS:                 Name                            Usage
 ** ZPS_tsAfEvent               *pZPSevent                      Zigbee stack event structure
 ** uint16                      u16Offset,                      Offset
 ** tsGP_ZgpResponseCmdPayload  *psZgpResponseCmdPayload        payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eGP_ZgpResponseReceive(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint16                                 u16Offset,
                    tsGP_ZgpResponseCmdPayload             *psZgpResponseCmdPayload)
{

    uint8 u8TransactionSequenceNumber;
    zbmap8 b8Options;
    uint16 u16ActualQuantity;
	 uint8  u8ItemsInPayload = 6;
    /* Payload with application id 2 will take an entry more */
    tsZCL_RxPayloadItem asPayloadDefinition[7] = {
            {1, &u16ActualQuantity, E_ZCL_BMAP8,    &psZgpResponseCmdPayload->b8Options}, //1
            {1, &u16ActualQuantity, E_ZCL_UINT16,   &psZgpResponseCmdPayload->u16TempMasterShortAddr},//2
            {1, &u16ActualQuantity, E_ZCL_BMAP8,    &psZgpResponseCmdPayload->b8TempMasterTxChannel},//3
            {1, &u16ActualQuantity, E_ZCL_UINT32,   &psZgpResponseCmdPayload->uZgpdDeviceAddr.u32ZgpdSrcId},//4
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psZgpResponseCmdPayload->eZgpdCmdId},//5
            {1, &u16ActualQuantity, E_ZCL_OSTRING,  &psZgpResponseCmdPayload->sZgpdCommandPayload},//6
            {1, &u16ActualQuantity, E_ZCL_OSTRING,  &psZgpResponseCmdPayload->sZgpdCommandPayload},//7 payload not correct. overwritten below
           };
    /* Get options field from payload */
    u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                               u16Offset,
                               E_ZCL_BMAP8,
                               &b8Options);

#ifdef GP_IEEE_ADDR_SUPPORT
    if((b8Options & GP_APPLICATION_ID_MASK) == 2)
    {
        asPayloadDefinition[3].eType = E_ZCL_IEEE_ADDR;
        asPayloadDefinition[3].pvDestination = &psZgpResponseCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;

        /* add Endpoint */
        asPayloadDefinition[4].eType = E_ZCL_UINT8;
        asPayloadDefinition[4].pvDestination = &psZgpResponseCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;

        asPayloadDefinition[5].eType = E_ZCL_UINT8;
        asPayloadDefinition[5].pvDestination = &psZgpResponseCmdPayload->eZgpdCmdId;


        asPayloadDefinition[6].eType = E_ZCL_OSTRING;
        asPayloadDefinition[6].pvDestination = &psZgpResponseCmdPayload->sZgpdCommandPayload;
        asPayloadDefinition[6].u16MaxQuantity = 1;
        u8ItemsInPayload++;
    }
#else
    if((b8Options & GP_APPLICATION_ID_MASK) == 2)
    {
        return E_ZCL_ERR_INVALID_VALUE;
    }
#endif


    return eZCL_CustomCommandReceive(pZPSevent,
                                 &u8TransactionSequenceNumber,
                                 asPayloadDefinition,
                                 u8ItemsInPayload,
                                 E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

}
#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
