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
 * MODULE:             ZGP Translation Table Response command
 *
 * COMPONENT:          GreenPowerTranslationTableResponse.c
 *
 * DESCRIPTION:        ZGP Translation table response command send and receive functions
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
#ifndef GP_DISABLE_TRANSLATION_TABLE_RSP_CMD
/****************************************************************************
 **
 ** NAME:       eGP_ZgpTranslationTableResponseSend
 **
 ** DESCRIPTION:
 ** Sends ZGP Translation table response command
 **
 ** PARAMETERS:         Name                           Usage
 ** uint8               u8SourceEndPointId             Source EP Id
 ** uint8               u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address      *psDestinationAddress           Destination Address
 ** uint8              *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsGP_ZgpTransTableResponseCmdPayload *psZgpTransTableResponsePayload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_ZgpTranslationTableResponseSend(
                    uint8                                  u8SourceEndPointId,
                    uint8                                  u8DestinationEndPointId,
                    tsZCL_Address                          *psDestinationAddress,
                    uint8                                  *pu8TransactionSequenceNumber,
                    tsGP_ZgpTransTableResponseCmdPayload   *psZgpTransTableResponsePayload)
{

    zbmap8 b8Options;
    uint8 u8ItemsInPayload, i;
    /* Payload with application id 2 will take an entry more */
    tsZCL_TxPayloadItem asPayloadDefinition[6 + (GP_MAX_TRANSLATION_RESPONSE_ENTRY * 8)] = {
            {1,  E_ZCL_UINT8,   &psZgpTransTableResponsePayload->u8Status}, //1
            {1,  E_ZCL_UINT8,   &psZgpTransTableResponsePayload->b8Options},//2
            {1,  E_ZCL_UINT8,   &psZgpTransTableResponsePayload->u8TotalNumOfEntries},//3
            {1,  E_ZCL_UINT8,   &psZgpTransTableResponsePayload->u8StartIndex}, //4
            {1,  E_ZCL_UINT8,   &psZgpTransTableResponsePayload->u8EntriesCount},//5
           };

    /* Get options field from payload */
    b8Options = psZgpTransTableResponsePayload->b8Options;

#ifndef GP_IEEE_ADDR_SUPPORT
        if((b8Options & GP_APPLICATION_ID_MASK) == 2)
        {
            return E_ZCL_ERR_INVALID_VALUE;
        }
#endif

    u8ItemsInPayload = 5; //  mandatory items in command payload


    for(i=0; i<psZgpTransTableResponsePayload->u8EntriesCount; i++)
    {
        asPayloadDefinition[u8ItemsInPayload].u16Quantity = 1;

        if((b8Options & GP_APPLICATION_ID_MASK) == 0)
        {
            asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT32;
            asPayloadDefinition[u8ItemsInPayload].pvData = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].uZgpdDeviceAddr.u32ZgpdSrcId;


        }
#ifdef GP_IEEE_ADDR_SUPPORT
        else
        {
        	asPayloadDefinition[u8ItemsInPayload].u16Quantity = 1;
            asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_IEEE_ADDR;
            asPayloadDefinition[u8ItemsInPayload].pvData = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;

            u8ItemsInPayload++;
            asPayloadDefinition[u8ItemsInPayload].u16Quantity = 1;
            asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT8;
            asPayloadDefinition[u8ItemsInPayload].pvData = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;

        }
#endif

        u8ItemsInPayload++;
        asPayloadDefinition[u8ItemsInPayload].u16Quantity = 1;
        asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT8;
        asPayloadDefinition[u8ItemsInPayload].pvData = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].eZgpdCommandId;
        u8ItemsInPayload++;
        asPayloadDefinition[u8ItemsInPayload].u16Quantity = 1;
        asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT8;
        asPayloadDefinition[u8ItemsInPayload].pvData = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].u8EndpointId;
        u8ItemsInPayload++;
        asPayloadDefinition[u8ItemsInPayload].u16Quantity = 1;
        asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT16;
        asPayloadDefinition[u8ItemsInPayload].pvData = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].u16ProfileID;
        u8ItemsInPayload++;
        asPayloadDefinition[u8ItemsInPayload].u16Quantity = 1;
        asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT16;
        asPayloadDefinition[u8ItemsInPayload].pvData = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].u16ZbClusterId;
        u8ItemsInPayload++;
        asPayloadDefinition[u8ItemsInPayload].u16Quantity = 1;
        asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT8;
        asPayloadDefinition[u8ItemsInPayload].pvData = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].u8ZbCommandId;
        u8ItemsInPayload++;
        asPayloadDefinition[u8ItemsInPayload].u16Quantity = 1;
        asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT8;
        asPayloadDefinition[u8ItemsInPayload].pvData = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].u8ZbCmdLength;
        u8ItemsInPayload++;

        if((psZgpTransTableResponsePayload->asTransTblRspEntry[i].u8ZbCmdLength  != 0)&&
          (psZgpTransTableResponsePayload->asTransTblRspEntry[i].u8ZbCmdLength  != 0xFF))
        {
            if(psZgpTransTableResponsePayload->asTransTblRspEntry[i].u8ZbCmdLength > GP_MAX_ZB_CMD_PAYLOAD_LENGTH)
            {
                return E_ZCL_CMDS_INSUFFICIENT_SPACE;
            }

            asPayloadDefinition[u8ItemsInPayload].u16Quantity = psZgpTransTableResponsePayload->asTransTblRspEntry[i].u8ZbCmdLength;
            asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT8;
            asPayloadDefinition[u8ItemsInPayload].pvData = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].au8ZbCmdPayload[0];
            u8ItemsInPayload++;

        }
    }

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                              u8DestinationEndPointId,
                              psDestinationAddress,
                              GREENPOWER_CLUSTER_ID,
                              TRUE,
                              E_GP_ZGP_TRANSLATION_TABLE_RESPONSE,
                              pu8TransactionSequenceNumber,
                              asPayloadDefinition,
                              FALSE,
                              0,
                              u8ItemsInPayload);

}

/****************************************************************************
 **
 ** NAME:       eGP_ZgpTranslationTableResponseReceive
 **
 ** DESCRIPTION:
 ** Handles ZGP Pairing Search comamnd
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent            *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint16                    u16Offset,                    Offset
 ** tsGP_ZgpTransTableResponseCmdPayload *psZgpTransTableResponsePayload payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_ZgpTranslationTableResponseReceive(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint16                                 u16Offset,
                    tsGP_ZgpTransTableResponseCmdPayload   *psZgpTransTableResponsePayload)
{

    uint8 u8TransactionSequenceNumber;
    uint16 u16ActualQuantity, u16ZbCmdPyldLengthIndex = 0;
    zbmap8 b8Options;
    uint8  u8ItemsInPayload, u8NumbersOfTranslations, i,  u8ZbCmdPyldLength = 0;

    tsZCL_RxPayloadItem asPayloadDefinition[6 + (GP_MAX_TRANSLATION_RESPONSE_ENTRY * 8)] = {
            {1, &u16ActualQuantity, E_ZCL_UINT8,   &psZgpTransTableResponsePayload->u8Status},
            {1, &u16ActualQuantity, E_ZCL_UINT8,   &psZgpTransTableResponsePayload->b8Options},
            {1, &u16ActualQuantity, E_ZCL_UINT8,   &psZgpTransTableResponsePayload->u8TotalNumOfEntries},
            {1, &u16ActualQuantity, E_ZCL_UINT8,   &psZgpTransTableResponsePayload->u8StartIndex},
            {1, &u16ActualQuantity, E_ZCL_UINT8,   &psZgpTransTableResponsePayload->u8EntriesCount},
           };
    /* Get options field from payload */
    u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                               u16Offset+1,
                               E_ZCL_BMAP8,
                               &b8Options);

#ifndef GP_IEEE_ADDR_SUPPORT
    if((b8Options & GP_APPLICATION_ID_MASK) == 2)
    {
        return E_ZCL_ERR_INVALID_VALUE;
    }
#endif

    u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                               u16Offset+4,
                               E_ZCL_UINT8,
                               &u8NumbersOfTranslations);

    if(u8NumbersOfTranslations > GP_MAX_TRANSLATION_RESPONSE_ENTRY)
    {
        return E_ZCL_CMDS_INSUFFICIENT_SPACE;
    }

    u8ItemsInPayload = 5; //  mandatory items in command payload
    u16ZbCmdPyldLengthIndex = 5;

    for(i=0; i<u8NumbersOfTranslations; i++)
    {
        asPayloadDefinition[u8ItemsInPayload].u16MaxQuantity = 1;
        asPayloadDefinition[u8ItemsInPayload].pu16ActualQuantity =  &u16ActualQuantity;

        if((b8Options & GP_APPLICATION_ID_MASK) == 0)
        {
        	asPayloadDefinition[u8ItemsInPayload].u16MaxQuantity = 1;
            asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT32;
            asPayloadDefinition[u8ItemsInPayload].pvDestination = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].uZgpdDeviceAddr.u32ZgpdSrcId;
            u16ZbCmdPyldLengthIndex += 4;
        }
#ifdef GP_IEEE_ADDR_SUPPORT
        else
        {
        	asPayloadDefinition[u8ItemsInPayload].u16MaxQuantity = 1;
            asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_IEEE_ADDR;
            asPayloadDefinition[u8ItemsInPayload].pvDestination = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
            u16ZbCmdPyldLengthIndex += 8;

            u8ItemsInPayload++;
            asPayloadDefinition[u8ItemsInPayload].u16MaxQuantity = 1;
            asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT8;
            asPayloadDefinition[u8ItemsInPayload].pvDestination = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
            asPayloadDefinition[u8ItemsInPayload].pu16ActualQuantity =  &u16ActualQuantity;
            u16ZbCmdPyldLengthIndex += 1;
        }
#endif

        u8ItemsInPayload++;

        asPayloadDefinition[u8ItemsInPayload].u16MaxQuantity = 1;
        asPayloadDefinition[u8ItemsInPayload].pu16ActualQuantity =  &u16ActualQuantity;
        asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT8;
        asPayloadDefinition[u8ItemsInPayload].pvDestination = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].eZgpdCommandId;
        u8ItemsInPayload++;
        asPayloadDefinition[u8ItemsInPayload].u16MaxQuantity = 1;
        asPayloadDefinition[u8ItemsInPayload].pu16ActualQuantity =  &u16ActualQuantity;
        asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT8;
        asPayloadDefinition[u8ItemsInPayload].pvDestination = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].u8EndpointId;
        u8ItemsInPayload++;
        asPayloadDefinition[u8ItemsInPayload].u16MaxQuantity = 1;
        asPayloadDefinition[u8ItemsInPayload].pu16ActualQuantity =  &u16ActualQuantity;
        asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT16;
        asPayloadDefinition[u8ItemsInPayload].pvDestination = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].u16ProfileID;
        u8ItemsInPayload++;
        asPayloadDefinition[u8ItemsInPayload].u16MaxQuantity = 1;
        asPayloadDefinition[u8ItemsInPayload].pu16ActualQuantity =  &u16ActualQuantity;
        asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT16;
        asPayloadDefinition[u8ItemsInPayload].pvDestination = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].u16ZbClusterId;
        u8ItemsInPayload++;
        asPayloadDefinition[u8ItemsInPayload].u16MaxQuantity = 1;
        asPayloadDefinition[u8ItemsInPayload].pu16ActualQuantity =  &u16ActualQuantity;
        asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT8;
        asPayloadDefinition[u8ItemsInPayload].pvDestination = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].u8ZbCommandId;
        u8ItemsInPayload++;
        asPayloadDefinition[u8ItemsInPayload].u16MaxQuantity = 1;
        asPayloadDefinition[u8ItemsInPayload].pu16ActualQuantity =  &u16ActualQuantity;
        asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT8;
        asPayloadDefinition[u8ItemsInPayload].pvDestination = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].u8ZbCmdLength;
        u8ItemsInPayload++;
        u16ZbCmdPyldLengthIndex += 7;

        u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                                           u16Offset+u16ZbCmdPyldLengthIndex,
                                           E_ZCL_UINT8,
                                           &u8ZbCmdPyldLength);

        u16ZbCmdPyldLengthIndex++;

        if((u8ZbCmdPyldLength != 0)&&(u8ZbCmdPyldLength != 0xFF))
        {
            if(u8ZbCmdPyldLength > GP_MAX_ZB_CMD_PAYLOAD_LENGTH)
            {
                return E_ZCL_CMDS_INSUFFICIENT_SPACE;
            }

            asPayloadDefinition[u8ItemsInPayload].u16MaxQuantity = psZgpTransTableResponsePayload->asTransTblRspEntry[i].u8ZbCmdLength;
            asPayloadDefinition[u8ItemsInPayload].pu16ActualQuantity =  &u16ActualQuantity;
            asPayloadDefinition[u8ItemsInPayload].eType = E_ZCL_UINT8;
            asPayloadDefinition[u8ItemsInPayload].pvDestination = &psZgpTransTableResponsePayload->asTransTblRspEntry[i].au8ZbCmdPayload[0];
            u8ItemsInPayload++;
            u16ZbCmdPyldLengthIndex += u8ZbCmdPyldLength;
        }
    }


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
