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
 * MODULE:             ZGP Pairing command
 *
 * COMPONENT:          GreenPowerPairing.c
 *
 * DESCRIPTION:        ZGP pairing command send and receive functions
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
 ** NAME:       eGP_ZgpPairingSend
 **
 ** DESCRIPTION:
 ** Sends ZGP pairing command
 **
 ** PARAMETERS:         Name                           Usage
 ** uint8               u8SourceEndPointId             Source EP Id
 ** uint8               u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address      *psDestinationAddress           Destination Address
 ** uint8              *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsGP_ZgpPairingCmdPayload *psZgpPairingPayload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_ZgpPairingSend(
                    uint8                                  u8SourceEndPointId,
                    uint8                                  u8DestinationEndPointId,
                    tsZCL_Address                          *psDestinationAddress,
                    uint8                                  *pu8TransactionSequenceNumber,
                    tsGP_ZgpPairingCmdPayload              *psZgpPairingPayload)
{

    zbmap24 b24Options;
    uint8 u8ItemsInPayload;
    /* Payload with application id 2 will take an entry more */
    tsZCL_TxPayloadItem asPayloadDefinition[11] = {
            {1,  E_ZCL_BMAP24,   &psZgpPairingPayload->b24Options}, //1
            {1,  E_ZCL_UINT32,   &psZgpPairingPayload->uZgpdDeviceAddr.u32ZgpdSrcId}, //2
            {1,  E_ZCL_UINT16,   &psZgpPairingPayload->u16SinkGroupID}, //3
            {1,  E_ZCL_UINT8,    &psZgpPairingPayload->u8DeviceId}, //4
            {1,  E_ZCL_UINT32,   &psZgpPairingPayload->u32ZgpdSecFrameCounter},//5
            {1,  E_ZCL_KEY_128,  &psZgpPairingPayload->sZgpdKey}, //6
            {1,  E_ZCL_UINT16,   &psZgpPairingPayload->u16AssignedAlias}, //7
            {1,  E_ZCL_UINT8,    &psZgpPairingPayload->u8ForwardingRadius}, //8
            {1,  E_ZCL_UINT8,    &psZgpPairingPayload->u8ForwardingRadius} //9 values are overwritten below
           };

    /* Get options field from payload */
    b24Options = psZgpPairingPayload->b24Options;

    u8ItemsInPayload = 2; //  mandatory items in command payload
#ifdef GP_IEEE_ADDR_SUPPORT
    if((b24Options & GP_APPLICATION_ID_MASK) == 2)
    {
    	asPayloadDefinition[1].eType = E_ZCL_IEEE_ADDR;
        asPayloadDefinition[1].pvData = &psZgpPairingPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;

		/* add Endpoint */
        asPayloadDefinition[2].eType = E_ZCL_UINT8;
        asPayloadDefinition[2].pvData = &psZgpPairingPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;

        asPayloadDefinition[3].eType = E_ZCL_UINT16;
        asPayloadDefinition[3].pvData = &psZgpPairingPayload->u16SinkGroupID;

        asPayloadDefinition[4].eType = E_ZCL_UINT8;
        asPayloadDefinition[4].pvData = &psZgpPairingPayload->u8DeviceId;

        asPayloadDefinition[5].eType = E_ZCL_UINT32;
        asPayloadDefinition[5].pvData = &psZgpPairingPayload->u32ZgpdSecFrameCounter;

        asPayloadDefinition[6].eType = E_ZCL_KEY_128;
        asPayloadDefinition[6].pvData = &psZgpPairingPayload->sZgpdKey;

        asPayloadDefinition[7].eType = E_ZCL_UINT16;
        asPayloadDefinition[7].pvData = &psZgpPairingPayload->u16AssignedAlias;

        asPayloadDefinition[8].eType = E_ZCL_UINT8;
        asPayloadDefinition[8].pvData = &psZgpPairingPayload->u8ForwardingRadius;
        asPayloadDefinition[8].u16Quantity = 1;
        u8ItemsInPayload++;
    }
#else
    if((b24Options & GP_APPLICATION_ID_MASK) == 2)
    {
        return E_ZCL_ERR_INVALID_VALUE;
    }
#endif

    if(!(b24Options & GP_PAIRING_REMOVE_GPD_MASK))
    {
        if((((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)||
          (((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID))
        {
            u8ItemsInPayload += 1; //  increase sink group id field
        }

		else if((((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_LIGHTWEIGHT)||
					(((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_BOTH))
	    {
			uint8 index = 2;
			if((b24Options & GP_APPLICATION_ID_MASK) == 2)
			{
				index++;
			}

			u8ItemsInPayload += 2;  //add sink ieee and nwk address
			asPayloadDefinition[index].eType = E_ZCL_UINT64;
			asPayloadDefinition[index++].pvData = &psZgpPairingPayload->u64SinkIEEEAddress;

			asPayloadDefinition[index].eType = E_ZCL_UINT16;
			asPayloadDefinition[index++].pvData = &psZgpPairingPayload->u16SinkNwkAddress;

			asPayloadDefinition[index].eType = E_ZCL_UINT8;
			asPayloadDefinition[index++].pvData = &psZgpPairingPayload->u8DeviceId;

			asPayloadDefinition[index].eType = E_ZCL_UINT32;
			asPayloadDefinition[index++].pvData = &psZgpPairingPayload->u32ZgpdSecFrameCounter;

			asPayloadDefinition[index].eType = E_ZCL_KEY_128;
			asPayloadDefinition[index++].pvData = &psZgpPairingPayload->sZgpdKey;

			asPayloadDefinition[index].eType = E_ZCL_UINT16;
			asPayloadDefinition[index++].pvData = &psZgpPairingPayload->u16AssignedAlias;

			asPayloadDefinition[index].eType = E_ZCL_UINT8;
			asPayloadDefinition[index++].pvData = &psZgpPairingPayload->u8ForwardingRadius;

        }

        if(b24Options & GP_PAIRING_ADD_SINK_MASK)
        {
            u8ItemsInPayload += 1; //  increase device id field

            if(b24Options & GP_PAIRING_SECURITY_COUNTER_PRESENT_MASK)
            {
                u8ItemsInPayload += 1; //  increase security frame counter field
            }

            if(b24Options & GP_PAIRING_SECURITY_KEY_PRESENT_MASK)
            {
                u8ItemsInPayload += 1; //  increase security key field
                /* if security frame counter is not present in payload
                 * then security key field would be at 5th position */

                    asPayloadDefinition[(u8ItemsInPayload - 1)].eType = E_ZCL_KEY_128;
                    asPayloadDefinition[(u8ItemsInPayload - 1)].pvData = &psZgpPairingPayload->sZgpdKey;

            }

            if(b24Options & GP_PAIRING_ASSIGNED_ALIAS_PRESENT_MASK)
            {
                u8ItemsInPayload += 1; //  increase assigned alias field
                /* if security frame counter and security key are not present in payload
                 * then assigned alias field would be at 5th position */
                if((((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)||
                          (((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID))
                {
					if(((u8ItemsInPayload == 5) ||(u8ItemsInPayload == 6))&& ((b24Options & GP_APPLICATION_ID_MASK) == 0))
					{
						asPayloadDefinition[(u8ItemsInPayload - 1)].eType = E_ZCL_UINT16;
						asPayloadDefinition[(u8ItemsInPayload - 1)].pvData = &psZgpPairingPayload->u16AssignedAlias;
					}
					else if(((u8ItemsInPayload == 6)|| (u8ItemsInPayload == 7))&& ((b24Options & GP_APPLICATION_ID_MASK) == 2))
					{
						asPayloadDefinition[(u8ItemsInPayload - 1)].eType = E_ZCL_UINT16;
						asPayloadDefinition[(u8ItemsInPayload - 1)].pvData = &psZgpPairingPayload->u16AssignedAlias;
					}
                }
                else
                {
					if((u8ItemsInPayload == 7) && ((b24Options & GP_APPLICATION_ID_MASK) == 0))
					{
						asPayloadDefinition[(u8ItemsInPayload - 1)].eType = E_ZCL_UINT16;
						asPayloadDefinition[(u8ItemsInPayload - 1)].pvData = &psZgpPairingPayload->u16AssignedAlias;
					}
					else if((u8ItemsInPayload == 8) && ((b24Options & GP_APPLICATION_ID_MASK) == 2))
					{
						asPayloadDefinition[(u8ItemsInPayload - 1)].eType = E_ZCL_UINT16;
						asPayloadDefinition[(u8ItemsInPayload - 1)].pvData = &psZgpPairingPayload->u16AssignedAlias;
					}
                }

            }

            if(b24Options & GP_PAIRING_FORWARDING_RADIUS_PRESENT_MASK)
            {
                u8ItemsInPayload += 1; //  increase forwarding radius field
                /* if security frame counter, security key and assigned alias are not present in payload
                 * then forwarding radius field would be at 5th position */
                if((((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)||
                                          (((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID))
                {
				   if(((u8ItemsInPayload == 5)||(u8ItemsInPayload == 6)||(u8ItemsInPayload == 7)) && ((b24Options & GP_APPLICATION_ID_MASK) == 0))
				   {
						asPayloadDefinition[(u8ItemsInPayload - 1)].eType = E_ZCL_UINT8;
						asPayloadDefinition[(u8ItemsInPayload - 1)].pvData = &psZgpPairingPayload->u8ForwardingRadius;
				   }
				   else if(((u8ItemsInPayload == 6) ||(u8ItemsInPayload == 7)||(u8ItemsInPayload == 8))&& ((b24Options & GP_APPLICATION_ID_MASK) == 2))
				   {
						asPayloadDefinition[(u8ItemsInPayload - 1)].eType = E_ZCL_UINT8;
						asPayloadDefinition[(u8ItemsInPayload - 1)].pvData = &psZgpPairingPayload->u8ForwardingRadius;
				   }
                }
                else
                {
                	 if(((u8ItemsInPayload ==7) ||(u8ItemsInPayload ==8)||(u8ItemsInPayload ==9))&& ((b24Options & GP_APPLICATION_ID_MASK) == 0))
					   {
							asPayloadDefinition[(u8ItemsInPayload - 1)].eType = E_ZCL_UINT8;
							asPayloadDefinition[(u8ItemsInPayload - 1)].pvData = &psZgpPairingPayload->u8ForwardingRadius;
					   }
					   else if(((u8ItemsInPayload ==8) ||(u8ItemsInPayload ==9)||(u8ItemsInPayload ==10))&& ((b24Options & GP_APPLICATION_ID_MASK) == 2))
					   {
							asPayloadDefinition[(u8ItemsInPayload - 1)].eType = E_ZCL_UINT8;
							asPayloadDefinition[(u8ItemsInPayload - 1)].pvData = &psZgpPairingPayload->u8ForwardingRadius;
					   }
                }

            }
        }
    }

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                              u8DestinationEndPointId,
                              psDestinationAddress,
                              GREENPOWER_CLUSTER_ID,
                              TRUE,
                              E_GP_ZGP_PAIRING,
                              pu8TransactionSequenceNumber,
                              asPayloadDefinition,
                              FALSE,
                              0,
                              u8ItemsInPayload);

}

/****************************************************************************
 **
 ** NAME:       eGP_ZgpPairingReceive
 **
 ** DESCRIPTION:
 ** Handles ZGP Pairing command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent            *pZPSevent                     Zigbee stack event structure
 ** uint16                    u16Offset,                    Offset
 ** tsGP_ZgpPairingSearchCmdPayload *psZgpPairingSearchPayload payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_ZgpPairingReceive(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint16                                 u16Offset,
                    tsGP_ZgpPairingCmdPayload              *psZgpPairingPayload)
{
	bool_t bIsAppId = 0;
    uint8 u8TransactionSequenceNumber;
    uint16 u16ActualQuantity;
    zbmap24 b24Options;
    uint8  u8ItemsInPayload;
    /* Payload with application id 2 will take an entry more */
    tsZCL_RxPayloadItem asPayloadDefinition[11] = {
             {1, &u16ActualQuantity, E_ZCL_BMAP24,   &psZgpPairingPayload->b24Options}, //1
             {1, &u16ActualQuantity, E_ZCL_UINT32,   &psZgpPairingPayload->uZgpdDeviceAddr.u32ZgpdSrcId}, //2
             {1, &u16ActualQuantity, E_ZCL_UINT16,   &psZgpPairingPayload->u16SinkGroupID}, //3
             {1, &u16ActualQuantity, E_ZCL_UINT8,    &psZgpPairingPayload->u8DeviceId}, //4
             {1, &u16ActualQuantity, E_ZCL_UINT32,   &psZgpPairingPayload->u32ZgpdSecFrameCounter},//5
             {1, &u16ActualQuantity, E_ZCL_KEY_128,  &psZgpPairingPayload->sZgpdKey},//6
             {1, &u16ActualQuantity, E_ZCL_UINT16,   &psZgpPairingPayload->u16AssignedAlias},//7
             {1, &u16ActualQuantity, E_ZCL_UINT8,    &psZgpPairingPayload->u8ForwardingRadius}, //8
             {1, &u16ActualQuantity, E_ZCL_UINT8,    &psZgpPairingPayload->u8ForwardingRadius} ,//VALUES ARE NOT CORRECT, overwritten below
             {1, &u16ActualQuantity, E_ZCL_UINT8,    &psZgpPairingPayload->u8ForwardingRadius},//VALUES ARE NOT CORRECT, overwritten below
             {1, &u16ActualQuantity, E_ZCL_UINT8,    &psZgpPairingPayload->u8ForwardingRadius}//VALUES ARE NOT CORRECT, overwritten below

   };
    /* Get options field from payload */
    u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                               u16Offset,
                               E_ZCL_BMAP24,
                               &b24Options);

    u8ItemsInPayload = 2; //  mandatory items in command payload
#ifdef GP_IEEE_ADDR_SUPPORT
    if((b24Options & GP_APPLICATION_ID_MASK) == 2)
    {
    	bIsAppId = 1;
    	u8ItemsInPayload = 3;
    	asPayloadDefinition[1].eType = E_ZCL_IEEE_ADDR;
        asPayloadDefinition[1].pvDestination = &psZgpPairingPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
		/* add Endpoint */
        asPayloadDefinition[2].eType = E_ZCL_UINT8;
        asPayloadDefinition[2].pvDestination = &psZgpPairingPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;

        asPayloadDefinition[3].eType = E_ZCL_UINT16;
        asPayloadDefinition[3].pvDestination = &psZgpPairingPayload->u16SinkGroupID;

        asPayloadDefinition[4].eType = E_ZCL_UINT8;
        asPayloadDefinition[4].pvDestination = &psZgpPairingPayload->u8DeviceId;

        asPayloadDefinition[5].eType = E_ZCL_UINT32;
        asPayloadDefinition[5].pvDestination = &psZgpPairingPayload->u32ZgpdSecFrameCounter;

        asPayloadDefinition[6].eType = E_ZCL_KEY_128;
        asPayloadDefinition[6].pvDestination = &psZgpPairingPayload->sZgpdKey;

        asPayloadDefinition[7].eType = E_ZCL_UINT16;
        asPayloadDefinition[7].pvDestination = &psZgpPairingPayload->u16AssignedAlias;

        asPayloadDefinition[8].eType = E_ZCL_UINT8;
        asPayloadDefinition[8].pvDestination = &psZgpPairingPayload->u8ForwardingRadius;
        asPayloadDefinition[8].u16MaxQuantity = 1;
    }
#else
    if((b24Options & GP_APPLICATION_ID_MASK) == 2)
    {
        return E_ZCL_ERR_INVALID_VALUE;
    }
#endif

    if(!(b24Options & GP_PAIRING_REMOVE_GPD_MASK))
    {
        if((((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)||
          (((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID))
        {
               u8ItemsInPayload += 1; //  increase sink group id field
        }
        else if((((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_LIGHTWEIGHT)||
                (((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_BOTH))
	   {
        	uint8 index = 2;
        	if((b24Options & GP_APPLICATION_ID_MASK) == 2)
        	{
        		index++;
        	}

        	u8ItemsInPayload += 2;  //add sink ieee and nwk address
        	asPayloadDefinition[index].eType = E_ZCL_UINT64;
			asPayloadDefinition[index++].pvDestination = &psZgpPairingPayload->u64SinkIEEEAddress;

			asPayloadDefinition[index].eType = E_ZCL_UINT16;
			asPayloadDefinition[index++].pvDestination = &psZgpPairingPayload->u16SinkNwkAddress;

			asPayloadDefinition[index].eType = E_ZCL_UINT8;
			asPayloadDefinition[index++].pvDestination = &psZgpPairingPayload->u8DeviceId;

			asPayloadDefinition[index].eType = E_ZCL_UINT32;
			asPayloadDefinition[index++].pvDestination = &psZgpPairingPayload->u32ZgpdSecFrameCounter;

			asPayloadDefinition[index].eType = E_ZCL_KEY_128;
			asPayloadDefinition[index++].pvDestination = &psZgpPairingPayload->sZgpdKey;

			asPayloadDefinition[index].eType = E_ZCL_UINT16;
			asPayloadDefinition[index++].pvDestination = &psZgpPairingPayload->u16AssignedAlias;

			asPayloadDefinition[index].eType = E_ZCL_UINT8;
			asPayloadDefinition[index].pvDestination = &psZgpPairingPayload->u8ForwardingRadius;
			asPayloadDefinition[index].u16MaxQuantity = 1;

	   }

	   if(b24Options & GP_PAIRING_ADD_SINK_MASK)
	   {
		   u8ItemsInPayload += 1; //  increase device id field

		   if(b24Options & GP_PAIRING_SECURITY_COUNTER_PRESENT_MASK)
		   {
			   u8ItemsInPayload += 1; //  increase security frame counter field
		   }

		   if(b24Options & GP_PAIRING_SECURITY_KEY_PRESENT_MASK)
		   {
			   u8ItemsInPayload += 1; //  increase security key field
			   /* if security frame counter is not present in payload
				* then security key field would be at 5th position */
			   if((((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)||
			            (((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID))
			   {
				   if((u8ItemsInPayload == 5)&& (bIsAppId == 0))
				   {
					   asPayloadDefinition[4].eType = E_ZCL_KEY_128;
					   asPayloadDefinition[4].pvDestination = &psZgpPairingPayload->sZgpdKey;
				   }
				   else if ((u8ItemsInPayload == 6)&& (bIsAppId == 1))
				   {
					   asPayloadDefinition[5].eType = E_ZCL_KEY_128;
					   asPayloadDefinition[5].pvDestination = &psZgpPairingPayload->sZgpdKey;
				   }
			   }
			   else
			   {
				   if((u8ItemsInPayload == 7)&& (bIsAppId == 0))
				   {
					   asPayloadDefinition[6].eType = E_ZCL_KEY_128;
					   asPayloadDefinition[6].pvDestination = &psZgpPairingPayload->sZgpdKey;
				   }
				   else if ((u8ItemsInPayload == 8)&& (bIsAppId == 1))
				   {
					   asPayloadDefinition[7].eType = E_ZCL_KEY_128;
					   asPayloadDefinition[7].pvDestination = &psZgpPairingPayload->sZgpdKey;
				   }
			   }
		   }

		   if(b24Options & GP_PAIRING_ASSIGNED_ALIAS_PRESENT_MASK)
		   {
			   u8ItemsInPayload += 1; //  increase assigned alias field
			   /* if security frame counter and security key are not present in payload
				* then assigned alias field would be at 5th position */
			   if((((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)||
			            (((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID))
			   {
				   if(((u8ItemsInPayload == 5) || (u8ItemsInPayload == 6))&& (bIsAppId == 0))
				   {
					   asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT16;
					   asPayloadDefinition[u8ItemsInPayload - 1].pvDestination = &psZgpPairingPayload->u16AssignedAlias;
				   }
				   /* if one of the field, security frame counter or security key is not present in payload
					* then assigned alias field would be at 6th position */

				   else if( ((u8ItemsInPayload == 6) || (u8ItemsInPayload == 7))&& (bIsAppId == 1))
				  {
					  asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT16;
					  asPayloadDefinition[u8ItemsInPayload -1].pvDestination =  &psZgpPairingPayload->u16AssignedAlias;
				  }

			   }
			   else
			   {
				   if(((u8ItemsInPayload ==7) || (u8ItemsInPayload == 8))&& (bIsAppId == 0))
				   {
					   asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT16;
					   asPayloadDefinition[u8ItemsInPayload - 1].pvDestination = &psZgpPairingPayload->u16AssignedAlias;
				   }
				   else if( ((u8ItemsInPayload == 8)|| (u8ItemsInPayload == 9))&& (bIsAppId == 1))
				  {
					  asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT16;
					  asPayloadDefinition[u8ItemsInPayload -1].pvDestination =  &psZgpPairingPayload->u16AssignedAlias;
				  }

			   }
		   }

		   if(b24Options & GP_PAIRING_FORWARDING_RADIUS_PRESENT_MASK)
		   {
			   u8ItemsInPayload += 1; //  increase forwarding radius field
			   /* if security frame counter, security key and assigned alias are not present in payload
				* then forwarding radius field would be at 5th position */
			   if((((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)||
			  			            (((b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID))
			   {
				   if(((u8ItemsInPayload == 5) || (u8ItemsInPayload == 6) || (u8ItemsInPayload == 7))&& (bIsAppId == 0))
				   {
					   asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT8;
					   asPayloadDefinition[u8ItemsInPayload - 1].pvDestination = &psZgpPairingPayload->u8ForwardingRadius;
				   }

				   else if(((u8ItemsInPayload == 6) || (u8ItemsInPayload == 7) ||(u8ItemsInPayload == 8))&& (bIsAppId == 1))
					{
						 asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT8;
						 asPayloadDefinition[u8ItemsInPayload -1].pvDestination = &psZgpPairingPayload->u8ForwardingRadius;
					}
			   }
			   else
			   {
				   if(((u8ItemsInPayload == 7) || (u8ItemsInPayload == 8) || (u8ItemsInPayload == 9))&& (bIsAppId == 0))
				   {
					   asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT8;
					   asPayloadDefinition[u8ItemsInPayload - 1].pvDestination = &psZgpPairingPayload->u8ForwardingRadius;
				   }

				   else if(((u8ItemsInPayload == 8) || (u8ItemsInPayload == 9) ||(u8ItemsInPayload == 10))&& (bIsAppId == 1))
					{
						 asPayloadDefinition[u8ItemsInPayload - 1].eType = E_ZCL_UINT8;
						 asPayloadDefinition[u8ItemsInPayload -1].pvDestination = &psZgpPairingPayload->u8ForwardingRadius;
					}
			   }

           }
	   }
    }
    if( ((b24Options & GP_PAIRING_SEC_LEVEL_MASK) >> 9) == 1)
    {
    	return E_ZCL_ERR_INVALID_VALUE;
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
