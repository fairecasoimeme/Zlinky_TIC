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
 * MODULE:             ZGP Notification command
 *
 * COMPONENT:          GreenPowerNotification.c
 *
 * DESCRIPTION:        ZGP notification command send and receive functions
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

/****************************************************************************
 **
 ** NAME:       eGP_ZgpNotificationSend
 **
 ** DESCRIPTION:
 ** Sends ZGP notification command
 **
 ** PARAMETERS:         Name                           Usage
 ** uint8               u8SourceEndPointId             Source EP Id
 ** uint8               u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address      *psDestinationAddress           Destination Address
 ** uint8              *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsGP_ZgpNotificationCmdPayload *psZgpNotificationPayload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_ZgpNotificationSend(
                    uint8                                  u8SourceEndPointId,
                    uint8                                  u8DestinationEndPointId,
                    tsZCL_Address                          *psDestinationAddress,
                    uint8                                  *pu8TransactionSequenceNumber,
                    tsGP_ZgpNotificationCmdPayload         *psZgpNotificationPayload)
{

    zbmap16 b16Options;
    uint8 u8ItemsInPayload;

    /* Payload with application id 2 will take an entry more */
    tsZCL_TxPayloadItem asPayloadDefinition[8] = {
            {1,  E_ZCL_BMAP16,   &psZgpNotificationPayload->b16Options}, //1
            {1,  E_ZCL_UINT32,   &psZgpNotificationPayload->uZgpdDeviceAddr.u32ZgpdSrcId}, //2
            {1,  E_ZCL_UINT32,   &psZgpNotificationPayload->u32ZgpdSecFrameCounter}, //3
            {1,  E_ZCL_UINT8,    &psZgpNotificationPayload->eZgpdCmdId}, //4
            {1,  E_ZCL_OSTRING,  &psZgpNotificationPayload->sZgpdCommandPayload}, //5
            {1,  E_ZCL_UINT16,   &psZgpNotificationPayload->u16ZgppShortAddr}, //6
            {1,  E_ZCL_UINT8,     &psZgpNotificationPayload->u8GPP_GPD_Link} //7
           };

    /* Get options field from payload */
    b16Options = psZgpNotificationPayload->b16Options;

    u8ItemsInPayload = 5; //  mandatory items in command payload
#ifdef GP_IEEE_ADDR_SUPPORT
    if((b16Options & GP_APPLICATION_ID_MASK) == 2)
    {
        asPayloadDefinition[1].eType = E_ZCL_IEEE_ADDR;
        asPayloadDefinition[1].pvData = &psZgpNotificationPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;

        asPayloadDefinition[2].eType = E_ZCL_UINT8;
        asPayloadDefinition[2].pvData = &psZgpNotificationPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;

        asPayloadDefinition[3].eType = E_ZCL_UINT32;
        asPayloadDefinition[3].pvData = &psZgpNotificationPayload->u32ZgpdSecFrameCounter;

        asPayloadDefinition[4].eType = E_ZCL_UINT8;
        asPayloadDefinition[4].pvData = &psZgpNotificationPayload->eZgpdCmdId;

        asPayloadDefinition[5].eType = E_ZCL_OSTRING;
        asPayloadDefinition[5].pvData = &psZgpNotificationPayload->sZgpdCommandPayload;

        asPayloadDefinition[6].eType = E_ZCL_UINT16;
        asPayloadDefinition[6].pvData = &psZgpNotificationPayload->u16ZgppShortAddr;


        asPayloadDefinition[7].u16Quantity = 1;
        asPayloadDefinition[7].eType = E_ZCL_UINT8;
        asPayloadDefinition[7].pvData = &psZgpNotificationPayload->u8GPP_GPD_Link;
        u8ItemsInPayload++;
    }
#else
    if((b16Options & GP_APPLICATION_ID_MASK) == 2)
    {
        return E_ZCL_ERR_INVALID_VALUE;
    }
#endif
    /* check if ZGP short address and ZGP distance fields are present in payload */
    if((b16Options & GP_NOTIFICATION_PROXY_INFO_PRESENT) || (b16Options & GP_NOTIFICATION_APPOINT_TEMP_MASTER_MASK))
    {
        u8ItemsInPayload += 2; //  increase optional 2 field
    }

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                              u8DestinationEndPointId,
                              psDestinationAddress,
                              GREENPOWER_CLUSTER_ID,
                              FALSE,
                              E_GP_ZGP_NOTIFICATION,
                              pu8TransactionSequenceNumber,
                              asPayloadDefinition,
                              FALSE,
                              0,
                              u8ItemsInPayload);

}

/****************************************************************************
 **
 ** NAME:       eGP_ZgpNotificationReceive
 **
 ** DESCRIPTION:
 ** Handles ZGP Notification comamnd
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent            *pZPSevent                     Zigbee stack event structure
 ** uint16                    u16Offset,                    Offset
 ** tsGP_ZgpNotificationCmdPayload *psZgpNotificationPayload payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_ZgpNotificationReceive(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint16                                 u16Offset,
                    tsGP_ZgpNotificationCmdPayload         *psZgpNotificationPayload)
{

    uint8 u8TransactionSequenceNumber;
    uint16 u16ActualQuantity;
    zbmap16 b16Options;
    uint8  u8ItemsInPayload;
    /* Payload with application id 2 will take an entry more */
    tsZCL_RxPayloadItem asPayloadDefinition[8] = {
            {1, &u16ActualQuantity, E_ZCL_BMAP16,   &psZgpNotificationPayload->b16Options}, //1
            {1, &u16ActualQuantity, E_ZCL_UINT32,   &psZgpNotificationPayload->uZgpdDeviceAddr.u32ZgpdSrcId}, //2
            {1, &u16ActualQuantity, E_ZCL_UINT32,   &psZgpNotificationPayload->u32ZgpdSecFrameCounter}, //3
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psZgpNotificationPayload->eZgpdCmdId}, //4
            {1, &u16ActualQuantity, E_ZCL_OSTRING,  &psZgpNotificationPayload->sZgpdCommandPayload}, //5
            {1, &u16ActualQuantity, E_ZCL_UINT16,   &psZgpNotificationPayload->u16ZgppShortAddr}, //6
            {1, &u16ActualQuantity, E_ZCL_UINT8,     &psZgpNotificationPayload->u8GPP_GPD_Link}//7
           };
    /* Get options field from payload */
    u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                               u16Offset,
                               E_ZCL_BMAP16,
                               &b16Options);

    u8ItemsInPayload = 5; //  mandatory items in command payload
#ifdef GP_IEEE_ADDR_SUPPORT
    if((b16Options & GP_APPLICATION_ID_MASK) == 2)
    {
        asPayloadDefinition[1].eType = E_ZCL_IEEE_ADDR;
        asPayloadDefinition[1].pvDestination = &psZgpNotificationPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;

        asPayloadDefinition[2].eType = E_ZCL_UINT8;
        asPayloadDefinition[2].pvDestination = &psZgpNotificationPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;

        asPayloadDefinition[3].eType = E_ZCL_UINT32;
        asPayloadDefinition[3].pvDestination = &psZgpNotificationPayload->u32ZgpdSecFrameCounter;

        asPayloadDefinition[4].eType = E_ZCL_UINT8;
        asPayloadDefinition[4].pvDestination = &psZgpNotificationPayload->eZgpdCmdId;

        asPayloadDefinition[5].eType = E_ZCL_OSTRING;
        asPayloadDefinition[5].pvDestination = &psZgpNotificationPayload->sZgpdCommandPayload;

        asPayloadDefinition[6].eType = E_ZCL_UINT16;
        asPayloadDefinition[6].pvDestination = &psZgpNotificationPayload->u16ZgppShortAddr;

        asPayloadDefinition[7].eType = E_ZCL_UINT8;
        asPayloadDefinition[7].pvDestination = &psZgpNotificationPayload->u8GPP_GPD_Link;
        asPayloadDefinition[7].u16MaxQuantity = 1;
        asPayloadDefinition[7].pu16ActualQuantity = &u16ActualQuantity;

        u8ItemsInPayload++;
    }
#else
    if((b16Options & GP_APPLICATION_ID_MASK) == 2)
    {
        return E_ZCL_ERR_INVALID_VALUE;
    }
#endif
    /* check if ZGP short address and ZGP distance fields are present in payload */
    if((b16Options & GP_NOTIFICATION_PROXY_INFO_PRESENT) || (b16Options & GP_NOTIFICATION_APPOINT_TEMP_MASTER_MASK))
    {
        u8ItemsInPayload += 2; //  increase optional 2 field
    }

    return eZCL_CustomCommandReceive(pZPSevent,
                                 &u8TransactionSequenceNumber,
                                 asPayloadDefinition,
                                 u8ItemsInPayload,
                                 E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
