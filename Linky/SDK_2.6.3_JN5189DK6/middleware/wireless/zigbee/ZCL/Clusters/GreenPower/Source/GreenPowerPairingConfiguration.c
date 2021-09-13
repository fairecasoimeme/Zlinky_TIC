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
 * MODULE:             ZGP Pairing Configuration command
 *
 * COMPONENT:          GreenPowerPairingConfiguration.c
 *
 * DESCRIPTION:        ZGP pairing configuration command send and receive functions
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
#include "dbg.h"
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
 ** NAME:       eGP_ZgpPairingConfigSend
 **
 ** DESCRIPTION:
 ** Sends ZGP pairing configuration command
 **
 ** PARAMETERS:         Name                           Usage
 ** uint8               u8SourceEndPointId             Source EP Id
 ** uint8               u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address      *psDestinationAddress           Destination Address
 ** uint8              *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsGP_ZgpPairingSearchCmdPayload *psZgpPairingSearchPayload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_ZgpPairingConfigSend(
                    uint8                                  u8SourceEndPointId,
                    uint8                                  u8DestinationEndPointId,
                    tsZCL_Address                          *psDestinationAddress,
                    uint8                                  *pu8TransactionSequenceNumber,
                    tsGP_ZgpPairingConfigCmdPayload        *psZgpPairingConfigPayload)
{

    zbmap16 b16Options;
    uint8 u8ItemsInPayload, u8NumOfPairedEndpoints = 0;
    /* Payload with application id 2 will take an entry more */
    tsZCL_TxPayloadItem asPayloadDefinition[14] = {
            {1,  E_ZCL_UINT8,    &psZgpPairingConfigPayload->u8Actions}, //1
            {1,  E_ZCL_BMAP16,   &psZgpPairingConfigPayload->b16Options},//2
            {1,  E_ZCL_UINT32,   &psZgpPairingConfigPayload->uZgpdDeviceAddr.u32ZgpdSrcId}, //3
            {1,  E_ZCL_UINT8,    &psZgpPairingConfigPayload->eZgpdDeviceId},//4
            {1,  E_ZCL_UINT8,    &psZgpPairingConfigPayload->u8SinkGroupListEntries},//5
            {1,  E_ZCL_UINT16,   psZgpPairingConfigPayload->asSinkGroupList},//6
            {1,  E_ZCL_UINT16,   &psZgpPairingConfigPayload->u16ZgpdAssignedAlias},//7
            {1,  E_ZCL_UINT8,    &psZgpPairingConfigPayload->u8ForwardingRadius},//8
            {1,  E_ZCL_BMAP8,    &psZgpPairingConfigPayload->b8SecOptions},//9
            {1,  E_ZCL_UINT32,   &psZgpPairingConfigPayload->u32ZgpdSecFrameCounter},//10
            {1,  E_ZCL_KEY_128,  &psZgpPairingConfigPayload->sZgpdKey},//11
            {1,  E_ZCL_UINT8,    &psZgpPairingConfigPayload->u8NumberOfPairedEndpoints},//12
            {u8NumOfPairedEndpoints,  E_ZCL_UINT8,    &psZgpPairingConfigPayload->au8PairedEndpoints[0]}//13
           };

    /* Get options field from payload */
    b16Options = psZgpPairingConfigPayload->b16Options;

    u8ItemsInPayload = 4; //  mandatory items in command payload
#ifdef GP_IEEE_ADDR_SUPPORT
    if((b16Options & GP_APPLICATION_ID_MASK) == 2)
    {
    	u8ItemsInPayload++;
        asPayloadDefinition[2].eType = E_ZCL_IEEE_ADDR;
        asPayloadDefinition[2].pvData = &psZgpPairingConfigPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
		/* add Endpoint */
        asPayloadDefinition[3].eType = E_ZCL_UINT8;
        asPayloadDefinition[3].pvData = &psZgpPairingConfigPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;

        asPayloadDefinition[4].eType = E_ZCL_UINT8;
        asPayloadDefinition[4].pvData = &psZgpPairingConfigPayload->eZgpdDeviceId;

        asPayloadDefinition[5].eType = E_ZCL_UINT8;
        asPayloadDefinition[5].pvData = &psZgpPairingConfigPayload->u8SinkGroupListEntries;

        asPayloadDefinition[6].eType = E_ZCL_UINT16;
        asPayloadDefinition[6].pvData =  psZgpPairingConfigPayload->asSinkGroupList;

        asPayloadDefinition[7].eType = E_ZCL_UINT16;
        asPayloadDefinition[7].pvData = &psZgpPairingConfigPayload->u16ZgpdAssignedAlias;

        asPayloadDefinition[8].eType = E_ZCL_UINT8;
        asPayloadDefinition[8].pvData = &psZgpPairingConfigPayload->u8ForwardingRadius;

        asPayloadDefinition[9].eType = E_ZCL_UINT8;
        asPayloadDefinition[9].pvData = &psZgpPairingConfigPayload->b8SecOptions;

        asPayloadDefinition[10].eType = E_ZCL_UINT32;
        asPayloadDefinition[10].pvData = &psZgpPairingConfigPayload->u32ZgpdSecFrameCounter;

        asPayloadDefinition[11].eType = E_ZCL_UINT8;
        asPayloadDefinition[11].pvData = &psZgpPairingConfigPayload->sZgpdKey;


        asPayloadDefinition[12].eType = E_ZCL_UINT8;
        asPayloadDefinition[12].pvData = &psZgpPairingConfigPayload->u8NumberOfPairedEndpoints;

        asPayloadDefinition[13].eType = E_ZCL_UINT8;
        asPayloadDefinition[13].pvData = &psZgpPairingConfigPayload->au8PairedEndpoints[0];
        asPayloadDefinition[13].u16Quantity = u8NumOfPairedEndpoints;

     }
#else
    if((b16Options & GP_APPLICATION_ID_MASK) == 2)
    {
        return E_ZCL_ERR_INVALID_VALUE;
    }
#endif

    if(((b16Options & GP_PAIRING_CONFIG_COMM_MODE_MASK) >> 3) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
    {
        u8ItemsInPayload += 2;
        if((b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_4_BYTE)
        {
        	asPayloadDefinition[5].u16Quantity = psZgpPairingConfigPayload->u8SinkGroupListEntries*2;
        }
        else
        {
        	asPayloadDefinition[6].u16Quantity = psZgpPairingConfigPayload->u8SinkGroupListEntries*2;
        }
    }

    if(b16Options & GP_PAIRING_CONFIG_ASSIGNED_ADDR_MASK)
    {
        u8ItemsInPayload++;
        if((u8ItemsInPayload == 5) && ((b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_4_BYTE))
        {
            asPayloadDefinition[4].eType = E_ZCL_UINT16;
            asPayloadDefinition[4].pvData = &psZgpPairingConfigPayload->u16ZgpdAssignedAlias;
        }
        else
        if((u8ItemsInPayload == 6) && ((b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_8_BYTE))
		{
			asPayloadDefinition[5].eType = E_ZCL_UINT16;
			asPayloadDefinition[5].pvData = &psZgpPairingConfigPayload->u16ZgpdAssignedAlias;
		}
    }

    u8ItemsInPayload++; //forwarding radius mandatory field

    asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT8;
    asPayloadDefinition[u8ItemsInPayload - 1].pvData = &psZgpPairingConfigPayload->u8ForwardingRadius;

    if(b16Options & GP_PAIRING_CONFIG_SECURITY_USE_MASK)
    {
        u8ItemsInPayload++; //security options field
        asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_BMAP8;
        asPayloadDefinition[u8ItemsInPayload - 1].pvData = &psZgpPairingConfigPayload->b8SecOptions;
    }

    /* check if sequence number capability or security is present*/
    if((b16Options & GP_PROXY_TABLE_SEQ_NUM_CAP_MASK)||
                (b16Options & GP_PAIRING_CONFIG_SECURITY_USE_MASK))
    {
        u8ItemsInPayload++; //security frame counter
        asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT32;
        asPayloadDefinition[u8ItemsInPayload - 1].pvData = &psZgpPairingConfigPayload->u32ZgpdSecFrameCounter;
    }

    //if security options are available
    if(b16Options & GP_PAIRING_CONFIG_SECURITY_USE_MASK)
    {
        //teGP_GreenPowerSecLevel    eSecLevel;
        //teGP_GreenPowerSecKeyType  eSecKeyType;

        //eSecLevel = psZgpPairingConfigPayload->b8SecOptions & GP_SECURITY_LEVEL_MASK;
        //eSecKeyType = (psZgpPairingConfigPayload->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2;
        //if security key is available
        //if((eSecLevel != E_GP_NO_SECURITY)&&(eSecKeyType != E_GP_DERIVED_INDIVIDUAL_ZGPD_KEY))
        {
            u8ItemsInPayload++; //security key
            asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_KEY_128;
            asPayloadDefinition[u8ItemsInPayload - 1].pvData = &psZgpPairingConfigPayload->sZgpdKey;
        }
    }

    u8ItemsInPayload++; //Number of paired endpoints mandatory field

    asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT8;
    asPayloadDefinition[u8ItemsInPayload - 1].pvData = &psZgpPairingConfigPayload->u8NumberOfPairedEndpoints;

    if( (psZgpPairingConfigPayload->u8NumberOfPairedEndpoints > 0) &&
	    (psZgpPairingConfigPayload->u8NumberOfPairedEndpoints < 0xfd))
    {
        u8ItemsInPayload++; //paired endpoints optional field
        u8NumOfPairedEndpoints = psZgpPairingConfigPayload->u8NumberOfPairedEndpoints;
        asPayloadDefinition[u8ItemsInPayload - 1].u16Quantity = u8NumOfPairedEndpoints;
        asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT8;
        asPayloadDefinition[u8ItemsInPayload - 1].pvData = &psZgpPairingConfigPayload->au8PairedEndpoints[0];
    }

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                              u8DestinationEndPointId,
                              psDestinationAddress,
                              GREENPOWER_CLUSTER_ID,
                              FALSE,
                              E_GP_ZGP_PAIRING_CONFIGURATION,
                              pu8TransactionSequenceNumber,
                              asPayloadDefinition,
                              FALSE,
                              0,
                              u8ItemsInPayload);

}

/****************************************************************************
 **
 ** NAME:       eGP_ZgpPairingConfigReceive
 **
 ** DESCRIPTION:
 ** Handles ZGP Pairing Search comamnd
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent            *pZPSevent                     Zigbee stack event structure
 ** uint16                    u16Offset,                    Offset
 ** tsGP_ZgpPairingConfigCmdPayload *psZgpPairingConfigPayload payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_ZgpPairingConfigReceive(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint16                                 u16Offset,
                    tsGP_ZgpPairingConfigCmdPayload        *psZgpPairingConfigPayload)
{

    uint8 u8TransactionSequenceNumber,i;
    uint16 u16ActualQuantity;
    zbmap16 b16Options;
    uint8 u8ItemsInPayload, u8NumOfPairedEndpoints = 0, u8NumOfPairedEndpointIndex = 0, u8GroupListEntries;
    tsGP_ZgpsGroupList asSinkGroupList[20];
    teZCL_Status eStatus;
    uint8 b8SecOptions;
    /* Payload with application id 2 will take an entry more */
    tsZCL_RxPayloadItem asPayloadDefinition[14] = {
            {1, &u16ActualQuantity,  E_ZCL_UINT8,    &psZgpPairingConfigPayload->u8Actions},  //1
            {1, &u16ActualQuantity,  E_ZCL_BMAP16,   &psZgpPairingConfigPayload->b16Options}, //2
            {1, &u16ActualQuantity,  E_ZCL_UINT32,   &psZgpPairingConfigPayload->uZgpdDeviceAddr.u32ZgpdSrcId}, //3
            {1, &u16ActualQuantity,  E_ZCL_UINT8,    &psZgpPairingConfigPayload->eZgpdDeviceId},//4
            {1, &u16ActualQuantity,  E_ZCL_UINT8,    &psZgpPairingConfigPayload->u8SinkGroupListEntries},//5
            {1, &u16ActualQuantity,  E_ZCL_UINT16,   asSinkGroupList},//6
            {1, &u16ActualQuantity,  E_ZCL_UINT16,   &psZgpPairingConfigPayload->u16ZgpdAssignedAlias},//7
            {1, &u16ActualQuantity,  E_ZCL_UINT8,    &psZgpPairingConfigPayload->u8ForwardingRadius},//8
            {1, &u16ActualQuantity,  E_ZCL_BMAP8,    &psZgpPairingConfigPayload->b8SecOptions},//9
            {1, &u16ActualQuantity,  E_ZCL_UINT32,   &psZgpPairingConfigPayload->u32ZgpdSecFrameCounter},//10
            {1, &u16ActualQuantity,  E_ZCL_KEY_128,  &psZgpPairingConfigPayload->sZgpdKey},//1
            {1, &u16ActualQuantity,  E_ZCL_UINT8,    &psZgpPairingConfigPayload->u8NumberOfPairedEndpoints},//12
            {u8NumOfPairedEndpoints, &u16ActualQuantity, E_ZCL_UINT8,    &psZgpPairingConfigPayload->au8PairedEndpoints[0]} //13
           };

    /* Get options field from payload */
    u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                               u16Offset+1,
                               E_ZCL_BMAP16,
                               &b16Options);

    u8ItemsInPayload = 4; //  mandatory items in command payload
    u8NumOfPairedEndpointIndex = 8;
#ifdef GP_IEEE_ADDR_SUPPORT
    if((b16Options & GP_APPLICATION_ID_MASK) == 2)
    {
    	u8ItemsInPayload++;
        asPayloadDefinition[2].eType = E_ZCL_IEEE_ADDR;
        asPayloadDefinition[2].pvDestination = &psZgpPairingConfigPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;

     		/* add Endpoint */
        asPayloadDefinition[3].eType = E_ZCL_UINT8;
        asPayloadDefinition[3].pvDestination = &psZgpPairingConfigPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;

        asPayloadDefinition[4].eType = E_ZCL_UINT8;
        asPayloadDefinition[4].pvDestination = &psZgpPairingConfigPayload->eZgpdDeviceId;

        asPayloadDefinition[5].eType = E_ZCL_UINT8;
        asPayloadDefinition[5].pvDestination = &psZgpPairingConfigPayload->u8SinkGroupListEntries;

        asPayloadDefinition[6].eType = E_ZCL_UINT16;
        asPayloadDefinition[6].pvDestination = asSinkGroupList;

        asPayloadDefinition[7].eType = E_ZCL_UINT16;
        asPayloadDefinition[7].pvDestination = &psZgpPairingConfigPayload->u16ZgpdAssignedAlias;

        asPayloadDefinition[8].eType = E_ZCL_UINT8;
        asPayloadDefinition[8].pvDestination = &psZgpPairingConfigPayload->u8ForwardingRadius;

        asPayloadDefinition[9].eType = E_ZCL_BMAP8;
        asPayloadDefinition[9].pvDestination = &psZgpPairingConfigPayload->b8SecOptions;

        asPayloadDefinition[10].eType = E_ZCL_UINT32;
        asPayloadDefinition[10].pvDestination = &psZgpPairingConfigPayload->u32ZgpdSecFrameCounter;

        asPayloadDefinition[11].eType = E_ZCL_KEY_128;
        asPayloadDefinition[11].pvDestination = &psZgpPairingConfigPayload->sZgpdKey;

        asPayloadDefinition[12].eType = E_ZCL_UINT8;
        asPayloadDefinition[12].pvDestination = &psZgpPairingConfigPayload->u8NumberOfPairedEndpoints;

        asPayloadDefinition[13].eType = E_ZCL_UINT8;
        asPayloadDefinition[13].pvDestination = &psZgpPairingConfigPayload->au8PairedEndpoints[0];
        asPayloadDefinition[13].u16MaxQuantity = u8NumOfPairedEndpoints;

        u8NumOfPairedEndpointIndex += ( 1 + 4); // 1 for endpoint 4 bytes to count for sixe of ieee and 4 byte src
    }
#else
    if((b16Options & GP_APPLICATION_ID_MASK) == 2)
    {
        return E_ZCL_ERR_INVALID_VALUE;
    }
#endif

    if(((b16Options & GP_PAIRING_CONFIG_COMM_MODE_MASK) >> 3) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
    {
        /* Get Number of group list entries field from payload */
        u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                                   u16Offset+u8NumOfPairedEndpointIndex,
                                   E_ZCL_UINT8,
                                   &u8GroupListEntries);
        if(u8GroupListEntries > 20)
        {
            return E_ZCL_ERR_INSUFFICIENT_SPACE;
        }
        u8ItemsInPayload += 2;
        if((b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_4_BYTE)
        {
        	asPayloadDefinition[5].u16MaxQuantity = u8GroupListEntries*2;
        }
        else
        {
        	asPayloadDefinition[6].u16MaxQuantity = u8GroupListEntries*2;
        }
        u8NumOfPairedEndpointIndex += 1+ (u8GroupListEntries*4);
    }

    if(b16Options & GP_PAIRING_CONFIG_ASSIGNED_ADDR_MASK)
    {
        u8ItemsInPayload++;
        if((u8ItemsInPayload == 5) && ((b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_4_BYTE))
        {
            asPayloadDefinition[4].eType = E_ZCL_UINT16;
            asPayloadDefinition[4].pvDestination = &psZgpPairingConfigPayload->u16ZgpdAssignedAlias;
        }
        else if((u8ItemsInPayload == 5) && ((b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_8_BYTE))
        {
            asPayloadDefinition[5].eType = E_ZCL_UINT16;
            asPayloadDefinition[5].pvDestination = &psZgpPairingConfigPayload->u16ZgpdAssignedAlias;
        }
        u8NumOfPairedEndpointIndex += 2;
    }

    u8ItemsInPayload++; //forwarding radius mandatory field

    asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT8;
    asPayloadDefinition[u8ItemsInPayload - 1].pvDestination = &psZgpPairingConfigPayload->u8ForwardingRadius;
    u8NumOfPairedEndpointIndex++;

    if(b16Options & GP_PAIRING_CONFIG_SECURITY_USE_MASK)
    {

        u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                                   u16Offset+u8NumOfPairedEndpointIndex,
                                   E_ZCL_UINT8,
                                   &b8SecOptions);

         u8ItemsInPayload++; //security options field
         asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_BMAP8;
         asPayloadDefinition[u8ItemsInPayload - 1].pvDestination = &psZgpPairingConfigPayload->b8SecOptions;
         u8NumOfPairedEndpointIndex++;
    }

    /* check if sequence number capability or security is present*/
    if((b16Options & GP_PROXY_TABLE_SEQ_NUM_CAP_MASK)||
               (b16Options & GP_PAIRING_CONFIG_SECURITY_USE_MASK))
    {
        u8ItemsInPayload++; //security frame counter
        asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT32;
        asPayloadDefinition[u8ItemsInPayload - 1].pvDestination = &psZgpPairingConfigPayload->u32ZgpdSecFrameCounter;
        u8NumOfPairedEndpointIndex += 4;
    }

    //if security options are available
    if(b16Options & GP_PAIRING_CONFIG_SECURITY_USE_MASK)
    {
        teGP_GreenPowerSecLevel    eSecLevel;
        teGP_GreenPowerSecKeyType  eSecKeyType;

        eSecLevel = b8SecOptions & GP_SECURITY_LEVEL_MASK;
        eSecKeyType = (b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2;
        //if security key is available
        if((eSecLevel != E_GP_NO_SECURITY)&&(eSecKeyType != E_GP_DERIVED_INDIVIDUAL_ZGPD_KEY))
        {
            u8ItemsInPayload++; //security key
            asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_KEY_128;
            asPayloadDefinition[u8ItemsInPayload - 1].pvDestination = &psZgpPairingConfigPayload->sZgpdKey;
            u8NumOfPairedEndpointIndex += 16;
        }
    }

    u8ItemsInPayload++; //Number of paired endpoints mandatory field

    asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT8;
    asPayloadDefinition[u8ItemsInPayload - 1].pvDestination = &psZgpPairingConfigPayload->u8NumberOfPairedEndpoints;

    u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                               u16Offset+u8NumOfPairedEndpointIndex,
                               E_ZCL_UINT8,
                               &u8NumOfPairedEndpoints);

    if((u8NumOfPairedEndpoints > 0)&&
       (u8NumOfPairedEndpoints < 0xfd))
    {
        if(u8NumOfPairedEndpoints > GP_MAX_PAIRED_ENDPOINTS)
        {
            return E_ZCL_ERR_INVALID_VALUE;
        }
        u8ItemsInPayload++; //paired endpoints optional field

        asPayloadDefinition[u8ItemsInPayload - 1].u16MaxQuantity = u8NumOfPairedEndpoints;
        asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT8;
        asPayloadDefinition[u8ItemsInPayload - 1].pvDestination = &psZgpPairingConfigPayload->au8PairedEndpoints[0];
    }


    eStatus =  eZCL_CustomCommandReceive(pZPSevent,
                                 &u8TransactionSequenceNumber,
                                 asPayloadDefinition,
                                 u8ItemsInPayload,
                                 E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

    if(psZgpPairingConfigPayload->u8SinkGroupListEntries > GP_MAX_SINK_GROUP_LIST)
    {
        psZgpPairingConfigPayload->u8SinkGroupListEntries = GP_MAX_SINK_GROUP_LIST;
    }
    for(i = 0; i < psZgpPairingConfigPayload->u8SinkGroupListEntries; i++)
    {
        psZgpPairingConfigPayload->asSinkGroupList[i].u16SinkGroup = asSinkGroupList[i].u16SinkGroup;
        psZgpPairingConfigPayload->asSinkGroupList[i].u16Alias = asSinkGroupList[i].u16Alias;
    }

    return eStatus;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
