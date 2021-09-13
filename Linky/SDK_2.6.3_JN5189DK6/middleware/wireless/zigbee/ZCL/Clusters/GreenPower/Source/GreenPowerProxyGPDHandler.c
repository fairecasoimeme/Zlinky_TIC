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
 * MODULE:             Green Power Proxy GPD handling
 *
 * COMPONENT:          GreenPowerProxyGPDHandler.c
 *
 * DESCRIPTION:        Green Power Proxy GPD handling
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "zcl.h"
#include "zcl_customcommand.h"
#include "GreenPower.h"
#include "GreenPower_internal.h"
#include "dbg.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef TRACE_GP_DEBUG
#define TRACE_GP_DEBUG FALSE
#endif
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

 PRIVATE void vGP_TxGPNotificationCmd(
		tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
		tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
		tsZCL_EndPointDefinition                     *psEndPointDefinition);


 PRIVATE teZCL_Status eGP_GreenPowerCommCmdHandler(
		 tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
         tsZCL_EndPointDefinition                    *psEndPointDefinition,
         tsGP_GreenPowerCustomData                   *psGpCustomDataStructure);


 PRIVATE teZCL_Status eGP_GreenPowerDataCmdHandler(
		 tsGP_ZgpBufferedApduRecord                 *psZgpBufferedApduRecord,
		 tsZCL_EndPointDefinition                   *psEndPointDefinition,
		 tsGP_GreenPowerCustomData                  *psGpCustomDataStructure);
#ifdef GP_COMBO_BASIC_DEVICE
 PRIVATE void vGP_TxDeviceAnnounce(
 		tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
 		tsGP_GreenPowerCustomData                    *psGpCustomDataStructure,
 		tsZCL_EndPointDefinition                     *psEndPointDefinition);
 PRIVATE void vGP_CreateCommReply(
 		tsGP_ZgpBufferedApduRecord                        *psZgpBufferedApduRecord,
 		tsGP_ZgpResponseCmdPayload                         *psZgpResponseCmdPayload,
 		uint8                                               *pu8GPResponsePayload,
 		tsZCL_EndPointDefinition                            *psEndPointDefinition,
 		 tsGP_GreenPowerCustomData                          *psGpCustomDataStructure);
PRIVATE void vGP_TxPairing(
	 		tsZCL_EndPointDefinition                           *psEndPointDefinition,
	 		tsGP_GreenPowerCustomData                          *psGpCustomDataStructure,
		    tsGP_ZgpBufferedApduRecord                         *psZgpBufferedApduRecord
		);

 PRIVATE void vGP_TxGPResponse(
  		tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
          tsZCL_EndPointDefinition                    *psEndPointDefinition,
          tsGP_GreenPowerCustomData                   *psGpCustomDataStructure);
 PRIVATE void vGP_CreateChannelConfig(
 		tsGP_ZgpBufferedApduRecord                        *psZgpBufferedApduRecord,
 		tsGP_ZgpResponseCmdPayload                         *psZgpResponseCmdPayload,
 		uint8                                               *pu8GPResponsePayload);
 PRIVATE void vGP_AddCommReplySecurity(
 		uint8												*pu8Payloadlength,
 		tsGP_ZgpBufferedApduRecord                          *psZgpBufferedApduRecord,
 		uint8                                               *pu8GPResponsePayload,
 		tsZCL_EndPointDefinition                            *psEndPointDefinition,
 		tsGP_GreenPowerCustomData                          *psGpCustomDataStructure);

#endif

PRIVATE void  vCreateNotification(
		uint8                                        u8KeyType,
		tsGP_ZgpNotificationCmdPayload               *psZgpNotificationPayload,
		 tsGP_ZgpBufferedApduRecord                 *psZgpBufferedApduRecord );
PRIVATE void vGP_TxCommissionNotification(
		 tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
		 tsZCL_EndPointDefinition                     *psEndPointDefinition,
		 tsGP_GreenPowerCustomData                    *psGpCustomDataStructure
);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
#ifdef GP_COMBO_BASIC_DEVICE
static uint8 au8GPResponsePayload[GP_MAX_COMMISSION_REPLY_PAYLOAD_LENGTH];
static PDUM_thAPduInstance                         hAPduInst = NULL;
static  ZPS_tsAfZgpGreenPowerReq                    sZgpDataReq;;
static tsGP_ZgpResponseCmdPayload sZgpResponseCmdPayload;
#endif
/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eGP_HandleProxyCommissioningMode
 **
 ** DESCRIPTION:
 ** Handles Green power Cluster proxy commissioning mode command
 **
 ** PARAMETERS:               Name                      Usage
 ** tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
 ** tsZCL_EndPointDefinition                     *psEndPointDefinition,
 ** tsGP_GreenPowerCustomData                    *psGpCustomDataStructure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  void vGP_HandleGPDCommand(
		 tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
		 tsZCL_EndPointDefinition                     *psEndPointDefinition,
		 tsGP_GreenPowerCustomData                    *psGpCustomDataStructure)
{

	switch(psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand)
	{
		case E_GP_COMMISSIONING_CMDS:
		{
			eGP_GreenPowerCommCmdHandler( psZgpBufferedApduRecord,psEndPointDefinition,psGpCustomDataStructure	);
			break;
		}

		case E_GP_COMMISSION_NOTIFICATION:
		{
			vGP_TxCommissionNotification( psZgpBufferedApduRecord,psEndPointDefinition,psGpCustomDataStructure	);
			break;
		}


#ifdef GP_COMBO_BASIC_DEVICE
		case E_GP_RESPONSE:
		case E_GP_RESPONSE_AFTER_DELAY:
		{
			vGP_TxGPResponse(psZgpBufferedApduRecord,psEndPointDefinition,psGpCustomDataStructure);
			break;
		}

		case E_GP_ADD_PAIRING:
		case E_GP_REMOVE_PAIRING: //fal through
		{
			DBG_vPrintf(TRACE_GP_DEBUG, "\n E_GP_REMOVE_PAIRING \n");
			vGP_TxPairing(psEndPointDefinition,psGpCustomDataStructure,psZgpBufferedApduRecord );
			break;
		}
		case E_GP_PROXY_COMMISION:
		{
			vGP_ExitCommMode(psEndPointDefinition,psGpCustomDataStructure);
			break;
		}


	case E_GP_DEVICE_ANNOUNCE:
	{
		vGP_TxDeviceAnnounce(psZgpBufferedApduRecord,psGpCustomDataStructure, psEndPointDefinition);
		break;
	}
#endif
	case E_GP_NOTIFICATION:
		DBG_vPrintf(TRACE_GP_DEBUG, "\n vGP_TxGPNotificationCmd  \n");
		vGP_TxGPNotificationCmd(psZgpBufferedApduRecord,psGpCustomDataStructure,psEndPointDefinition);
		break;
	case E_GP_DATA:

		eGP_GreenPowerDataCmdHandler(psZgpBufferedApduRecord,psEndPointDefinition,psGpCustomDataStructure);
		break;

	default:
		DBG_vPrintf(TRACE_GP_DEBUG, "Unknown command: \n");
		break;
	}

 }
/****************************************************************************/
/***        Private Functions                                              ***/
/****************************************************************************/
 /****************************************************************************
   **
   ** NAME:       vGP_TxGPNotificationCmd
   **
   ** DESCRIPTION:
   ** Transmits GP notification command on buffered data packet
   **
   ** PARAMETERS:                                  Name                        Usage
   ** tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord   Buffered command
   ** tsGP_GreenPowerCustomData                   *psGpCustomDataStructure   GP custom data structure
   **
   ** RETURN:
   ** None
   **
   ****************************************************************************/
  PRIVATE void vGP_TxGPNotificationCmd(
		tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
		tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
		tsZCL_EndPointDefinition                     *psEndPointDefinition)
 {
	  uint8                        au8GPCmdpayload[GP_MAX_ZB_CMD_PAYLOAD_LENGTH];
	  uint8 u8Status;
	  uint8 u8AliasSeqNum, u8TransactionSequenceNumber;
	  tsZCL_Address                               sDestinationAddress;
	 tsGP_ZgpsGroupList                  asGroupList[GP_MAX_SINK_GROUP_LIST];
	 uint8                               u8GroupListEntriesCount = 1,i;
	  uint16 u16AliasShortAddr;
	  tsGP_ZgppProxySinkTable                          *psSinkProxyTableEntry;
	 tsGP_ZgpNotificationCmdPayload    sZgpNotificationPayload;
#ifdef GP_COMBO_BASIC_DEVICE

	 bool_t bIsServer = TRUE;
#else
	 bool_t bIsServer = FALSE;
#endif


	 if(bGP_IsProxyTableEntryPresent(
		     		psEndPointDefinition->u8EndPointNumber,
		     		bIsServer,
		     		(uint8)(psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId),
		     		&(psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr)	,
		     		
		     		&psSinkProxyTableEntry ) == FALSE)
	 {
		 DBG_vPrintf(TRACE_GP_DEBUG, "\n vGP_TxGPNotificationCmd  bGP_IsProxyTableEntryPresent FALSE \n");
		return;
	 }

	sDestinationAddress.eAddressMode = E_ZCL_AM_GROUP;
	sZgpNotificationPayload.sZgpdCommandPayload.pu8Data = au8GPCmdpayload;
	vCreateNotification(((psSinkProxyTableEntry->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2), &sZgpNotificationPayload,psZgpBufferedApduRecord );
	/* Update group cast radius in custom data structure which will be used for transmission */
	psGpCustomDataStructure->u8GroupCastRadius = psSinkProxyTableEntry->u8GroupCastRadius;

	if(psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_DERIVED_GROUP_GPS_MASK)
	{
		sZgpNotificationPayload.b16Options |= GP_NOTIFICATION_ALSO_DERIVED_GROUP_MASK;
	}
	if(psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_COMMISSION_GROUP_GPS_MASK )
	{
		sZgpNotificationPayload.b16Options |= GP_NOTIFICATION_ALSO_COMMISSION_GROUP_MASK;
	}
	if(psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_UNICAST_GPS_MASK )
	{
		sZgpNotificationPayload.b16Options |= GP_NOTIFICATION_ALSO_UNICAST_MASK;
	}
	if(psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_DERIVED_GROUP_GPS_MASK)
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "\n vGP_TxGPNotificationCmd  E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID\n");
		sDestinationAddress.uAddress.u16GroupAddress = u16GP_DeriveAliasSrcAddr(
															(uint8)psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId,
															sZgpNotificationPayload.uZgpdDeviceAddr);
	   if(psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_ASSIGNED_ALIAS_MASK)
	   {
		   u16AliasShortAddr = psSinkProxyTableEntry->u16ZgpdAssignedAlias;
	   }
	   else
	   {
		   u16AliasShortAddr =  u16GP_DeriveAliasSrcAddr((uint8)psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId, sZgpNotificationPayload.uZgpdDeviceAddr) ;
	   }
       /* get alias sequence number */
       u8AliasSeqNum = u8GP_DeriveAliasSeqNum(psZgpBufferedApduRecord->sBufferedApduInfo.u8SeqNum,
                                              E_GP_ZGP_NOTIFICATION,
                                              E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID);
           /* set alias short address and alias sequence number in stack before sending the notification command */
	   ZPS_vAfSetZgpAlias(u16AliasShortAddr, u8AliasSeqNum, u8AliasSeqNum);

	   u8Status = eGP_ZgpNotificationSend(
						   psEndPointDefinition->u8EndPointNumber,
						   ZCL_GP_PROXY_ENDPOINT_ID,
						   &sDestinationAddress,
						   &u8TransactionSequenceNumber,
						   &sZgpNotificationPayload);
	   DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_ZgpNotificationSend status =%d, address mode = %d, address = %d ,u8AliasSeqNum = %d, GPD_Link = %d\n",
			   u8Status,sDestinationAddress.eAddressMode, sDestinationAddress.uAddress.u16GroupAddress , u8AliasSeqNum,
			   sZgpNotificationPayload.u8GPP_GPD_Link );
	}
	if(sZgpNotificationPayload.eZgpdCmdId >= E_GP_COMMISSIONING )
	{
		return;
	}

	if(psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_COMMISSION_GROUP_GPS_MASK )
	{
		 DBG_vPrintf(TRACE_GP_DEBUG, "\n E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID \n");

		u8GroupListEntriesCount = psSinkProxyTableEntry->u8SinkGroupListEntries;
		memcpy(asGroupList,psSinkProxyTableEntry->asSinkGroupList,u8GroupListEntriesCount*sizeof(tsGP_ZgpsGroupList));

		for(i = 0; i < u8GroupListEntriesCount; i++)
		{
			sDestinationAddress.uAddress.u16GroupAddress = asGroupList[i].u16SinkGroup;

			if(asGroupList[i].u16Alias != 0xFFFF )
			{
				u16AliasShortAddr = asGroupList[i].u16Alias;
			}
			else
			{
				u16AliasShortAddr = u16GP_DeriveAliasSrcAddr(
						   (uint8)psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId,
						   sZgpNotificationPayload.uZgpdDeviceAddr);
			}
            /* get alias sequence number */
            u8AliasSeqNum = u8GP_DeriveAliasSeqNum(psZgpBufferedApduRecord->sBufferedApduInfo.u8SeqNum,
                                                     E_GP_ZGP_NOTIFICATION,
                                                     E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID);
			/* set alias short address and alias sequence number in stack before sending the notification command */
			ZPS_vAfSetZgpAlias(u16AliasShortAddr, u8AliasSeqNum, u8AliasSeqNum);
			DBG_vPrintf(TRACE_GP_DEBUG, "\n Alias Sequence number %d \n",u8AliasSeqNum );
			u8Status = eGP_ZgpNotificationSend(
								   psEndPointDefinition->u8EndPointNumber,
								   ZCL_GP_PROXY_ENDPOINT_ID,
								   &sDestinationAddress,
								   &u8TransactionSequenceNumber,
								   &sZgpNotificationPayload);
			 DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_ZgpNotificationSend status2 =%d, address mode = %d, address = %d \n",
						   u8Status,sDestinationAddress.eAddressMode, sDestinationAddress.uAddress.u16GroupAddress );

		}
	}
	if(psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_UNICAST_GPS_MASK )
	{

		sDestinationAddress.eAddressMode = E_ZCL_AM_SHORT;
		for(i = 0; i < psSinkProxyTableEntry->u8NoOfUnicastSink; i++)
		{
			sDestinationAddress.uAddress.u16DestinationAddress = psSinkProxyTableEntry->u16SinkUnicastNWKAddress[i];
			 eGP_ZgpNotificationSend(
			   psEndPointDefinition->u8EndPointNumber,
			   ZCL_GP_PROXY_ENDPOINT_ID,
			   &sDestinationAddress,
			   &u8TransactionSequenceNumber,
			   &sZgpNotificationPayload);
		}

	}
 }
#ifdef GP_COMBO_BASIC_DEVICE
 /****************************************************************************
  **
  ** NAME:       vGP_TxDeviceAnnounce
  **
  ** DESCRIPTION:
  ** Transmits device announce on behalf of GPD based on command buffered
  **
  ** PARAMETERS:                                  Name                        Usage
  ** tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord   Buffered command
  ** tsGP_GreenPowerCustomData                   *psGpCustomDataStructure   GP custom data structure
  ** tsGP_ZgppProxySinkTable                          *psZgpsSinkTable			Sink Table entry
  ** tsGP_ZgppProxySinkTable                         *psZgppProxyTable			Proxy table entry
  **
  ** RETURN:
  ** None
  **
  ****************************************************************************/
 PRIVATE void vGP_TxDeviceAnnounce(
 		tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
 		tsGP_GreenPowerCustomData                    *psGpCustomDataStructure,
 		tsZCL_EndPointDefinition                     *psEndPointDefinition)
{
	 uint16 u16AliasShortAddr;

	 u16AliasShortAddr = u16GP_DeriveAliasSrcAddr(
						 (uint8)(psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId),
						psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr);

	vGP_SendDeviceAnnounce(u16AliasShortAddr,0xFFFFFFFFFFFFFFFFULL);

}
#endif

 /****************************************************************************
  **
  ** NAME:       vGP_TxCommissionNotification
  **
  ** DESCRIPTION:
  ** Transmits Commissioning notification commands buffered
  **
  ** PARAMETERS:                                  Name                        Usage
  **  tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
  ** tsZCL_EndPointDefinition                     *psEndPointDefinition,
  ** tsGP_GreenPowerCustomData                    *psGpCustomDataStructure
  **
  ** RETURN:
  ** None
  **
  ****************************************************************************/
 PRIVATE void vGP_TxCommissionNotification(
		 tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
		 tsZCL_EndPointDefinition                     *psEndPointDefinition,
		 tsGP_GreenPowerCustomData                    *psGpCustomDataStructure
 )
 {
 	uint8 u8AliasSeqNum;
 	uint8                                       u8SecLevel = 0, u8SecKeyType = E_GP_NO_KEY;
 	bool_t bIsEntryPresent = FALSE;
	tsGP_ZgppProxySinkTable                              *psSinkProxyTableEntry;
 	uint16 u16AliasShortAddr;
 	tsGP_ZgpCommissioningNotificationCmdPayload sZgpCommissioningNotificationCmdPayload;
    tsZCL_Address                               sDestinationAddress;
    uint8 u8TransactionSequenceNumber;
    sDestinationAddress.eAddressMode = E_ZCL_AM_BROADCAST;
    sDestinationAddress.uAddress.eBroadcastMode = ZPS_E_APL_AF_BROADCAST_RX_ON;

    bIsEntryPresent =
   		 bGP_IsProxyTableEntryPresent(
   		    		     		psEndPointDefinition->u8EndPointNumber,
#ifdef GP_COMBO_BASIC_DEVICE
   		    		     		TRUE,
#else
   		    		     		FALSE,
#endif
   		    		     	psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId,
   		    		        &psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr,

   		    		     		&psSinkProxyTableEntry );

   	 DBG_vPrintf(TRACE_GP_DEBUG, "\n vGP_TxCommissionNotification bGP_IsProxyTableEntryPresent =%d\n",
   			 bIsEntryPresent);

	/* Derive alias sequence number */
	u8AliasSeqNum = u8GP_DeriveAliasSeqNum(
		 psZgpBufferedApduRecord->sBufferedApduInfo.u8SeqNum,
		  E_GP_ZGP_COMMOSSIONING_NOTIFICATION,0xFF);
	if(psZgpBufferedApduRecord->sBufferedApduInfo.bFrameType)
	{
		sZgpCommissioningNotificationCmdPayload.uZgpdDeviceAddr.u32ZgpdSrcId = 0;
		sZgpCommissioningNotificationCmdPayload.b16Options = 0;
	}
	else
	{
		/* Check Application Id */
		if(psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId == GP_APPL_ID_4_BYTE)
		{
			sZgpCommissioningNotificationCmdPayload.uZgpdDeviceAddr.u32ZgpdSrcId =
			 psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.u32ZgpdSrcId;
		}
	 #ifdef GP_IEEE_ADDR_SUPPORT
		 else
		 {
			 sZgpCommissioningNotificationCmdPayload.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr =
				 psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
			 sZgpCommissioningNotificationCmdPayload.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint =
				   psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
			 DBG_vPrintf(TRACE_GP_DEBUG, "psZgpBufferedApduRecord->sBufferedApduInfo.u8EndPoint : %d\n",
					 psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint);
		 }
	 #endif
		 sZgpCommissioningNotificationCmdPayload.b16Options =
										 (uint16)psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId;
	}
     DBG_vPrintf(TRACE_GP_DEBUG, "psZgpBufferedApduRecord->sBufferedApduInfo.u8Status : %d\n", psZgpBufferedApduRecord->sBufferedApduInfo.u8Status);
     /* Security related */
     if(psZgpBufferedApduRecord->sBufferedApduInfo.u8Status > E_GP_SEC_NO_SECURITY)
     {
    	 u8SecKeyType = 0;
    	 /* When security check failed and the Security processing failed sub-field is set to 0b1, the Securi-tyKeyType sub-field SHALL
    	  *  be set to 0b000 if the SecurityKey sub-field of the Extended NWK Frame Control field of the received GPDF was set to 0b0,
    	  *  or to 0b100 if the SecurityKey sub-field of the Ex-tended NWK Frame Control field of the received GPDF was set to 0b1.
    	  *  If the Extended NWK Frame Control field is not present in the received GPDF, the SecurityKeyType sub-field is set to 0b000 */
    	 if(psZgpBufferedApduRecord->sBufferedApduInfo.u2SecurityKeyType)
    	 {
    		 u8SecKeyType = E_GP_OUT_OF_THE_BOX_ZGPD_KEY;
    	 }
         sZgpCommissioningNotificationCmdPayload.b16Options |= GP_CMSNG_NOTIFICATION_SEC_PROCESS_FAIL_MASK;
         /* Set Security Level */
        // u8SecLevel = 0;

     }
	 /* Set Key type */
	 /* Check Key Type */
	 /* check Sec Status */
     else if(psZgpBufferedApduRecord->sBufferedApduInfo.u8Status == E_GP_SEC_NO_SECURITY)
	  {
		  u8SecKeyType = E_GP_NO_KEY;
	  }
	 else
	 {
		 /* Overwrite  Security Level */
	//	  u8SecLevel = (uint8)psZgpBufferedApduRecord->sBufferedApduInfo.u2SecurityLevel;
		  u8SecKeyType = u8GetGPDFKeyMAP (psEndPointDefinition->u8EndPointNumber,
				  (uint8)psZgpBufferedApduRecord->sBufferedApduInfo.u2SecurityKeyType);

	 }


     u8SecLevel =  (uint8)psZgpBufferedApduRecord->sBufferedApduInfo.u2SecurityLevel;
     sZgpCommissioningNotificationCmdPayload.b16Options |= (u8SecLevel << 4);
     //u8SecKeyType = (uint8)psZgpBufferedApduRecord->sBufferedApduInfo.u2SecurityKeyType;
     sZgpCommissioningNotificationCmdPayload.b16Options |= (u8SecKeyType << 6);

     /* Set the command id */
     sZgpCommissioningNotificationCmdPayload.u8ZgpdCmdId =
         (teGP_ZgpdCommandId)psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId;

     /* Set the Security Frame Counter */
     sZgpCommissioningNotificationCmdPayload.u32ZgpdSecFrameCounter =
         psZgpBufferedApduRecord->sBufferedApduInfo.u32SecFrameCounter;

    /* else if ((bIsEntryPresent == FALSE) && (sZgpCommissioningNotificationCmdPayload.u8ZgpdCmdId == E_GP_COMMISSIONING))
     {
    	 if(psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[1] & GP_COMM_CMD_EXT_OPT_FIELD_MASK )
    	 {
			 uint8 u8FrameCounterIndex = 3;
			 if(psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[2] & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_PRESENT_MASK)
			 {
			   u8FrameCounterIndex += E_ZCL_KEY_128_SIZE;
			   if(psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[2] & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_ENC_MASK)
			   {
				   u8FrameCounterIndex += 4; //MIC size
			   }
			 }
			 sZgpCommissioningNotificationCmdPayload.u32ZgpdSecFrameCounter = psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[u8FrameCounterIndex++];
			 sZgpCommissioningNotificationCmdPayload.u32ZgpdSecFrameCounter |= psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[u8FrameCounterIndex++] << 8;
			 sZgpCommissioningNotificationCmdPayload.u32ZgpdSecFrameCounter |= psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[u8FrameCounterIndex++]<< 16;
			 sZgpCommissioningNotificationCmdPayload.u32ZgpdSecFrameCounter |= psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[u8FrameCounterIndex]<< 24;
    	 }

     }*/

     if(sZgpCommissioningNotificationCmdPayload.b16Options & GP_CMSNG_NOTIFICATION_SEC_PROCESS_FAIL_MASK)
     {
         sZgpCommissioningNotificationCmdPayload.u32Mic = psZgpBufferedApduRecord->sBufferedApduInfo.u32Mic;
     }

     /* Copy payload */
     sZgpCommissioningNotificationCmdPayload.sZgpdCommandPayload.u8Length =
                             psZgpBufferedApduRecord->sBufferedApduInfo.u8PDUSize;
     sZgpCommissioningNotificationCmdPayload.sZgpdCommandPayload.u8MaxLength =
                             psZgpBufferedApduRecord->sBufferedApduInfo.u8PDUSize;
     sZgpCommissioningNotificationCmdPayload.sZgpdCommandPayload.pu8Data =
                             psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU;

     /* check cmd type
      * Appoint temp master for channel request command */
     if( ( psZgpBufferedApduRecord->sBufferedApduInfo.bRxAfterTx) ||
    	((psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId == E_GP_CHANNEL_REQUEST)))
     {
         /* SetRxAfterTx bit in option field */
         sZgpCommissioningNotificationCmdPayload.b16Options |= (1 << 3);
     }

         /* Set Proxy infor */
         sZgpCommissioningNotificationCmdPayload.b16Options |= (1 << 11);
#ifndef PC_PLATFORM_BUILD
        sZgpCommissioningNotificationCmdPayload.u16ZgppShortAddr = ZPS_u16AplZdoGetNwkAddr();
#endif
        sZgpCommissioningNotificationCmdPayload.u8GPP_GPD_Link = psZgpBufferedApduRecord->sBufferedApduInfo.u8GPP_GPDLink;
     if(psGpCustomDataStructure->bCommissionUnicast)
     {
         sDestinationAddress.eAddressMode = E_ZCL_AM_SHORT;
         sDestinationAddress.uAddress.u16DestinationAddress = psGpCustomDataStructure->u16CommissionUnicastAddress;

     }
     else
     {
    	 u16AliasShortAddr =  u16GP_DeriveAliasSrcAddr(
    	     	 						   (uint8)psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId,
    	     	 						  sZgpCommissioningNotificationCmdPayload.uZgpdDeviceAddr);
    	 ZPS_vAfSetZgpAlias(u16AliasShortAddr, u8AliasSeqNum, u8AliasSeqNum);
     }

       eGP_ZgpCommissioningNotificationSend(
    		             psEndPointDefinition->u8EndPointNumber,
                         ZCL_GP_PROXY_ENDPOINT_ID,
                         &sDestinationAddress,
                         &u8TransactionSequenceNumber,
                         &sZgpCommissioningNotificationCmdPayload);

 }



 /****************************************************************************
  **
  ** NAME:       eGP_GreenPowerCommCmdHandler
  **
  ** DESCRIPTION:
  ** Green Power channel Request command handler
  **
  ** PARAMETERS:                    Name                           Usage
  ** tsGP_ZgpBufferedApduRecord  *psZgpBufferedApduRecord
  ** tsZCL_EndPointDefinition    *psEndPointDefinition       endpoint definition
  ** tsZCL_ClusterInstance       *psClusterInstance          cluster instance
  **
  ** RETURN:
  ** teZCL_Status
  ****************************************************************************/
 PRIVATE teZCL_Status eGP_GreenPowerCommCmdHandler(
		 tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
         tsZCL_EndPointDefinition                    *psEndPointDefinition,
         tsGP_GreenPowerCustomData                   *psGpCustomDataStructure)
 {

     teZCL_Status                                    eStatus = E_ZCL_SUCCESS;
     tsGP_ZgpDataIndication       sZgpDataIndication;

     vCreateDataIndFromBufferedCmd(psZgpBufferedApduRecord,&sZgpDataIndication);

     if((psGpCustomDataStructure->u16TransmitChannelTimeout) /*&& (sZgpDataIndication.u8CommandId == E_GP_CHANNEL_REQUEST)*/)
     {
    	 DBG_vPrintf(TRACE_GP_DEBUG, "\n psGpCustomDataStructure->u16TransmitChannelTimeout return  = %d \n", psGpCustomDataStructure->u16TransmitChannelTimeout);
         return eStatus;
     }
     DBG_vPrintf(TRACE_GP_DEBUG, "\n psGpCustomDataStructure->u16TransmitChannelTimeout = %d \n", psGpCustomDataStructure->u16TransmitChannelTimeout);
     /* If device is proxy then it has to wait upto zgppTunnelDelay ms */
     /* If device is combo min then it has to wait upto Dmax ms */
     /* If GP device is proxy, proxy always go into remote commission mode */
     /* Sink device will act as proxy when it goes into remote pairing mode */

     if(psGpCustomDataStructure->eGreenPowerDeviceMode == E_GP_REMOTE_COMMISSION_MODE)
     {
            /* Buffer this PKT for tunneling commission notification As this broadcast packet, transmit packet after zgppTunneling Delay */
    	 	if(psZgpBufferedApduRecord->sBufferedApduInfo.bTunneledPkt == FALSE)
    	 	{
    	 		eStatus = eGp_BufferTransmissionPacket(
                                &sZgpDataIndication,
                                E_GP_COMMISSION_NOTIFICATION,
                                psGpCustomDataStructure);
    	 	}
            DBG_vPrintf(TRACE_GP_DEBUG, "\n E_GP_COMMISSION_NOTIFICATION buffered \n");
     }
     else  if((psGpCustomDataStructure->eGreenPowerDeviceMode == E_GP_OPERATING_MODE) &&
    		( (sZgpDataIndication.u8CommandId == E_GP_DECOMMISSIONING) || (sZgpDataIndication.u8CommandId == E_GP_COMMISSIONING)))
     {
         /*If the proxy is in operational mode, and the GPDF carries a correctly protected GPD Commissioning or 4451 GPD Decommissioning
          *  command from a GPD the proxy has a Proxy Table entry for, the proxy 4452 SHALL forward the GPD command to the paired sinks
          *  using GP Notification command in the appropriate communication mode; the RxAfterTx sub-field of the Extended NWK Control
          *   Field of the triggering GPDF SHALL be ignored, and the RxAfterTx sub-field of the Options field of the resulting GP
          *    Notification command SHALL always be set to 0b0*/
         sZgpDataIndication.bRxAfterTx = 0;
		 eStatus = eGp_BufferTransmissionPacket(
							&sZgpDataIndication,
							E_GP_NOTIFICATION,
							psGpCustomDataStructure);
     }
#ifdef GP_COMBO_BASIC_DEVICE
     /* Buffer this PKT for tunneling ZGP Response, transmit packet after Dmax Delay */
     /* Only for Combo Min device only */

	 if(( psGpCustomDataStructure->eGreenPowerDeviceMode == E_GP_PAIRING_COMMISSION_MODE) &&
			 ((sZgpDataIndication.u8CommandId == E_GP_COMMISSIONING) ||
			 (sZgpDataIndication.u8CommandId == E_GP_CHANNEL_REQUEST)))
	 {
		 	 if(sZgpDataIndication.bRxAfterTx || sZgpDataIndication.bFrameType)

		 	 {
#ifdef GP_AT_TESTING
		 		 if(u8gpATTestAvoidGPResponse == 0)
#endif
		 		 {
				 eStatus = eGp_BufferTransmissionPacket(
						 &sZgpDataIndication,
						 E_GP_RESPONSE,
						 psGpCustomDataStructure);
				 DBG_vPrintf(TRACE_GP_DEBUG, "\n E_GP_RESPONSE buffered \n");
		 		 }
		 	 }
		 	 else
		 	 {
			    DBG_vPrintf(TRACE_GP_DEBUG, "\n E_GP_RESPONSE buffered \n");
				if(sZgpDataIndication.u8CommandId == E_GP_COMMISSIONING) 
				{
					eGP_UpdateApsGroupTableAndDeviceAnnounce(
											psEndPointDefinition,
											psGpCustomDataStructure,
											&sZgpDataIndication,
											E_ZCL_SUCCESS);
				}
		 	 }

	 }
	 else
	 {
		//pairing success
		if(psGpCustomDataStructure->eGreenPowerDeviceMode == E_GP_REMOTE_COMMISSION_MODE)
		{
			if(( psGpCustomDataStructure->bCommissionExitModeOnFirstPairSuccess) &&
					(((sZgpDataIndication.bAutoCommissioning) && (sZgpDataIndication.u8CommandId != E_GP_CHANNEL_REQUEST))
							|| (sZgpDataIndication.u8CommandId == E_GP_SUCCESS)))
			{
				vGP_ExitCommMode(psEndPointDefinition,
					psGpCustomDataStructure);
				DBG_vPrintf(TRACE_GP_DEBUG, "\n vGP_ExitCommMode  \n");
			}
		}
		else
		{
			//if( psGpCustomDataStructure->eGreenPowerDeviceMode == E_GP_PAIRING_COMMISSION_MODE)
			{
				if(sZgpDataIndication.u8CommandId == E_GP_DECOMMISSIONING)
				{
					eGp_BufferTransmissionPacket(
										 &sZgpDataIndication,
										 E_GP_REMOVE_PAIRING,
										 psGpCustomDataStructure);
					DBG_vPrintf(TRACE_GP_DEBUG, "\n Buffered E_GP_REMOVE_PAIRING on E_GP_DECOMMISSIONING\n");
				}
				else
				{
					DBG_vPrintf(TRACE_GP_DEBUG, "\n E_GP_PAIRING_COMMISSION_MODE eGP_UpdateApsGroupTableAndDeviceAnnounce \n");
					eGP_UpdateApsGroupTableAndDeviceAnnounce(
						psEndPointDefinition,
						psGpCustomDataStructure,
						&sZgpDataIndication,
						E_ZCL_SUCCESS);
				}
			}
		}
	 }

#endif


     return eStatus;
 }


 /****************************************************************************
  **
  ** NAME:       vGP_TxGPResponse
  **
  ** DESCRIPTION:
  ** Transmits GP response commands buffered
  **
  ** PARAMETERS:                                  Name                        Usage
  ** tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord   Buffered command
  ** tsZCL_EndPointDefinition                     psEndPointDefinition 		Endpoint Definition
  ** tsGP_GreenPowerCustomData                   *psGpCustomDataStructure   GP custom data structure
  ** tsGP_ZgppProxySinkTable                          *psZgpsSinkTable			Sink Table entry
  **
  ** RETURN:
  ** None
  **
  ****************************************************************************/
#ifdef GP_COMBO_BASIC_DEVICE
PRIVATE void vGP_TxGPResponse(
 		tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
         tsZCL_EndPointDefinition                    *psEndPointDefinition,
         tsGP_GreenPowerCustomData                   *psGpCustomDataStructure)
 {

     /* If device is combo min , Send GP Response frame to Proxy device or add to queue if device
      * itself if temp master */
     /* Combo Min has to wait upto Dmax ms  (100 ms) to collect the tunnel pkts from other and choose best one */
     /* As combo min doing both proxy and sink wait upto Dmax ms */
     /* Send the GP response to Proxy or add to queue for sending to GP based on temp master */
     /* Check NWK Address of buffered PKT */
     uint16 u16NwkAddr = 0;

     tsZCL_Address                               sDestinationAddress;

     uint8 u8TransactionSequenceNumber, u8Status;
     sDestinationAddress.eAddressMode = E_ZCL_AM_BROADCAST;
     sDestinationAddress.uAddress.eBroadcastMode = ZPS_E_APL_AF_BROADCAST_RX_ON;

 #ifndef PC_PLATFORM_BUILD
     u16NwkAddr = ZPS_u16AplZdoGetNwkAddr();
 #endif
     /* Send GP Response for Channel Configuration to the Proxy Device */

     if(psGpCustomDataStructure->u16TransmitChannelTimeout)
     {
    	 return;
     }
     if((psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand == E_GP_RESPONSE_AFTER_DELAY) && (hAPduInst != NULL))
     {

		 psGpCustomDataStructure->u8OperationalChannel = ZPS_u8AplZdoGetRadioChannel();
		 if(( (sZgpResponseCmdPayload.b8TempMasterTxChannel & 0x0F) + 11) != psGpCustomDataStructure->u8OperationalChannel)
		 {
			 ZPS_vNwkNibSetChannel( ZPS_pvAplZdoGetNwkHandle(), (sZgpResponseCmdPayload.b8TempMasterTxChannel & 0x0F) + 11);
			 psGpCustomDataStructure->u16TransmitChannelTimeout = 250;
		 }
		 DBG_vPrintf(TRACE_GP_DEBUG, "\n vGP_TxGPResponse: psGpCustomDataStructure->u16TransmitChannelTimeout == 250 , set channel to %d u8OperationalChannel %d\n",
				 ((sZgpResponseCmdPayload.b8TempMasterTxChannel & 0x0F) + 11),
				 psGpCustomDataStructure->u8OperationalChannel );
		 zps_vDStub_DataReq(&sZgpDataReq, hAPduInst,TRUE,(uint8)sZgpResponseCmdPayload.eZgpdCmdId);
		 vBackUpTxQueueMsg(E_GP_ADD_MSG_TO_TX_QUEUE,
						   &sZgpResponseCmdPayload,
						   &sZgpDataReq,
						   psGpCustomDataStructure);

     }
     else
     {
    	 memset(au8GPResponsePayload,0,sizeof(au8GPResponsePayload));
    	 memset(&sZgpDataReq,0,sizeof(sZgpDataReq));
    	 memset(&sZgpResponseCmdPayload,0,sizeof(sZgpResponseCmdPayload));
    	 hAPduInst = NULL;
		 /* Populate the Response payload */
		 sZgpResponseCmdPayload.b8Options = (uint8)psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId;

		 /* Check Application Id */
		 if(psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId == GP_APPL_ID_4_BYTE)
		 {
			 sZgpResponseCmdPayload.uZgpdDeviceAddr.u32ZgpdSrcId =
				 psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.u32ZgpdSrcId;
		 }
	 #ifdef GP_IEEE_ADDR_SUPPORT
		 else
		 {
			 sZgpResponseCmdPayload.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr =
					psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;

			 sZgpResponseCmdPayload.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint =
				 psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;

		 }
	 #endif

		 sZgpResponseCmdPayload.u16TempMasterShortAddr = psZgpBufferedApduRecord->sBufferedApduInfo.u16NwkShortAddr;
		 /* Set the payload to zero */
		 memset(au8GPResponsePayload, 0, sizeof(au8GPResponsePayload));
		 /* check GP Cmd Id */
		 if(psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId == E_GP_CHANNEL_REQUEST)
		 {
			 /* broadcast to the proxy */
			 /* u8TransmitChannel is the channel on which Response GPDF will be sent */
			vGP_CreateChannelConfig(psZgpBufferedApduRecord, &sZgpResponseCmdPayload, au8GPResponsePayload );
		 }
		 else
		 {
			vGP_CreateCommReply(psZgpBufferedApduRecord, &sZgpResponseCmdPayload, au8GPResponsePayload , psEndPointDefinition, psGpCustomDataStructure);
			psGpCustomDataStructure->bIsCommissionReplySent = TRUE;
		 }
		 DBG_vPrintf(TRACE_GP_DEBUG, "eGP_ZgpResponseSend 2 \r\n");
		 /* Send ZGP Response Pkt */
			 u8Status = eGP_ZgpResponseSend(
					   psEndPointDefinition->u8EndPointNumber,
						 ZCL_GP_PROXY_ENDPOINT_ID,
						 &sDestinationAddress,
						 &u8TransactionSequenceNumber,
						 &sZgpResponseCmdPayload);
			  DBG_vPrintf(TRACE_GP_DEBUG, "vGP_TxGPResponse to broadcast status = %d \r\n", u8Status);
			  DBG_vPrintf(TRACE_GP_DEBUG, "vGP_TxGPResponse 1 :\n");

		 if(u16NwkAddr == psZgpBufferedApduRecord->sBufferedApduInfo.u16NwkShortAddr)
		 {
			 /* Add to Tx Queue of Stack for sending GP channel configuration */

			 uint8                                       *pu8Start,i;

			 hAPduInst = hZCL_AllocateAPduInstance();
			 DBG_vPrintf(TRACE_GP_DEBUG, "vGP_TxGPResponse to self\r\n");
			 /* check buffer size */
			 if((hAPduInst == PDUM_INVALID_HANDLE)||
				(PDUM_u16APduGetSize(hZCL_GetBufferPoolHandle()) < sZgpResponseCmdPayload.sZgpdCommandPayload.u8Length))
			 {
				 DBG_vPrintf(TRACE_GP_DEBUG, "vGP_TxGPResponse to self buffer failed\r\n");
				 // free buffer and return
				 PDUM_eAPduFreeAPduInstance(hAPduInst);
				 return;
			 }
		  //   sZgpDataReq.bDataFrame = TRUE;
			 pu8Start = (uint8 *)(PDUM_pvAPduInstanceGetPayload(hAPduInst));

			 for(i = 0; i<sZgpResponseCmdPayload.sZgpdCommandPayload.u8Length;i++)
			 {
				 *pu8Start++ = sZgpResponseCmdPayload.sZgpdCommandPayload.pu8Data[i];
			 }
			 PDUM_eAPduInstanceSetPayloadSize(hAPduInst, sZgpResponseCmdPayload.sZgpdCommandPayload.u8Length);

			 sZgpDataReq.u8TxOptions = 0x01;
			 sZgpDataReq.u8ApplicationId = sZgpResponseCmdPayload.b8Options;
			 if(sZgpDataReq.u8ApplicationId == GP_APPL_ID_4_BYTE)
			 {
				 sZgpDataReq.uGpId.u32SrcId = sZgpResponseCmdPayload.uZgpdDeviceAddr.u32ZgpdSrcId;
			 }
	 #ifdef GP_IEEE_ADDR_SUPPORT
			 else
			 {
				 sZgpDataReq.uGpId.u64Address =
						sZgpResponseCmdPayload.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
				 sZgpDataReq.u8Endpoint =
						sZgpResponseCmdPayload.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
			 }
	 #endif
			 sZgpDataReq.u16TxQueueEntryLifetime = 5000; //5 sec
			 sZgpDataReq.u8Handle = u8GP_GetDataReqHandle(psGpCustomDataStructure);
			 sZgpDataReq.u8SeqNum = psZgpBufferedApduRecord->sBufferedApduInfo.u8SeqNum;
			 sZgpDataReq.u16Panid = 0xFFFF;
			 sZgpDataReq.u16DstAddr = 0xFFFF;

			 if((sZgpResponseCmdPayload.eZgpdCmdId == E_GP_CHANNEL_CONFIGURATION)&&
				(sZgpDataReq.uGpId.u32SrcId == 0)&&
				(sZgpDataReq.u8ApplicationId == GP_APPL_ID_4_BYTE))
			 {
				 sZgpDataReq.u8TxOptions |= ( ((uint8)3) << 3); // set maintenance bit
			 }
			 if(psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId == E_GP_CHANNEL_REQUEST)
			 {
				 tsGP_ZgpDataIndication sZgpDataIndication;
				 vCreateDataIndFromBufferedCmd(psZgpBufferedApduRecord,&sZgpDataIndication);
				 eGp_BufferTransmissionPacket(
											&sZgpDataIndication,
											E_GP_RESPONSE_AFTER_DELAY,
											psGpCustomDataStructure);
			 }
			 else
			 {
				 zps_vDStub_DataReq(&sZgpDataReq, hAPduInst,TRUE,(uint8)sZgpResponseCmdPayload.eZgpdCmdId);
				 vBackUpTxQueueMsg(E_GP_ADD_MSG_TO_TX_QUEUE,
								  &sZgpResponseCmdPayload,
								  &sZgpDataReq,
								  psGpCustomDataStructure);
			 }
		 }
     }
 }

 /****************************************************************************
  **
  ** NAME:       vGP_CreateCommReply
  **
  ** DESCRIPTION:
  ** Creates commissioning reply command from buffered command
  **
  ** PARAMETERS:                                  Name                        Usage
  ** tsGP_ZgpBufferedApduRecord             *psZgpBufferedApduRecord,
  ** tsGP_ZgpResponseCmdPayload             *psZgpResponseCmdPayload,
  ** uint8                                  *pu8GPResponsePayload,
  ** tsZCL_EndPointDefinition               *psEndPointDefinition,
  ** tsGP_GreenPowerCustomData              *psGpCustomDataStructure
  **
  ** RETURN:
  ** None
  **
  ****************************************************************************/
 PRIVATE void vGP_CreateCommReply(
 		tsGP_ZgpBufferedApduRecord                        *psZgpBufferedApduRecord,
 		tsGP_ZgpResponseCmdPayload                         *psZgpResponseCmdPayload,
 		uint8                                               *pu8GPResponsePayload,
 		tsZCL_EndPointDefinition                            *psEndPointDefinition,
 		 tsGP_GreenPowerCustomData                          *psGpCustomDataStructure)
 {
 	//uint8 u8GPCommissionPayloadOptions;
     uint8 u8Payloadlength = 1;

    // u8GPCommissionPayloadOptions = psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[1];

    psZgpResponseCmdPayload->b8TempMasterTxChannel = ZPS_u8AplZdoGetRadioChannel() - 11;
    psZgpResponseCmdPayload->eZgpdCmdId = E_GP_COMMISSIONING_REPLY;

     /* check PAN ID request */
     if(psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[1] & GP_COMM_CMD_PANID_REQ_MASK)
     {
         uint16 u16PanId = 0;

         /* Set PANId present flag */
         pu8GPResponsePayload[0] = TRUE;
 #ifndef PC_PLATFORM_BUILD
         u16PanId = ZPS_u16AplZdoGetNetworkPanId();
 #endif
         /* Get Network Pan Id */
         pu8GPResponsePayload[1] = (uint8)u16PanId;
         pu8GPResponsePayload[2] = (uint8)(u16PanId >> 8);

         u8Payloadlength += 2;
     }

     /* check Sec Level */
     if((psZgpBufferedApduRecord->sBufferedApduInfo.u2SecurityLevel)||
    		 (psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[1] & GP_COMM_CMD_ZGP_SEC_KEY_REQ_MASK) ||
    		 (psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[1] & GP_COMM_CMD_EXT_OPT_SEC_LEVEL_MASK))
     {
     	vGP_AddCommReplySecurity( &u8Payloadlength,psZgpBufferedApduRecord,pu8GPResponsePayload, psEndPointDefinition, psGpCustomDataStructure);
     }

    psZgpResponseCmdPayload->sZgpdCommandPayload.u8Length = u8Payloadlength;
    psZgpResponseCmdPayload->sZgpdCommandPayload.u8MaxLength = u8Payloadlength;
    psZgpResponseCmdPayload->sZgpdCommandPayload.pu8Data = pu8GPResponsePayload;


 }


 /****************************************************************************
  **
  ** NAME:       vGP_AddCommReplySecurity
  **
  ** DESCRIPTION:
  ** Adds seciurity headers to commissioning reply command
  **
  ** PARAMETERS:                                  Name                        Usage
  ** uint8                                  *pu8Payloadlength,
  ** tsGP_ZgpBufferedApduRecord             *psZgpBufferedApduRecord,
  ** uint8                                  *pu8GPResponsePayload,
  ** tsGP_ZgppProxySinkTable                     *psZgpsSinkTable,
  ** tsZCL_EndPointDefinition               *psEndPointDefinition,
  ** tsGP_GreenPowerCustomData              *psGpCustomDataStructure
  **
  ** RETURN:
  ** None
  **
  ****************************************************************************/
 PRIVATE void vGP_AddCommReplySecurity(
 		uint8												*pu8Payloadlength,
 		tsGP_ZgpBufferedApduRecord                          *psZgpBufferedApduRecord,
 		uint8                                               *pu8GPResponsePayload,
 		tsZCL_EndPointDefinition                            *psEndPointDefinition,
 		tsGP_GreenPowerCustomData                          *psGpCustomDataStructure)
 {
	 uint8 u8SecKeyType,u8SecLevel;
	 uint8 u8Length = *pu8Payloadlength;
     uint32 u32FrameCounter = 0;
	 AESSW_Block_u                   uSecurityKey;
	 AESSW_Block_u                   uOutKey;
#ifdef CLD_GP_ATTR_ZGP_LINK_KEY
	 uint32 u32MIC = 0;
#endif

     bGP_GetSecurityDetails(
    		 psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId,
    		 &u8SecLevel,
    		 &u8SecKeyType,
    		 &uSecurityKey,
    		 &psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr,
    		 psEndPointDefinition,
    		 psGpCustomDataStructure);

	 pu8GPResponsePayload[0] |=((uint8)(psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[2] & GP_COMM_CMD_EXT_OPT_SEC_LEVEL_MASK) )<< 3; //Security level

	// pu8GPResponsePayload[0] |= ((psSinkProxyTableEntry->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2) << 5;//key type

	 DBG_vPrintf(TRACE_GP_DEBUG, "psZgpBufferedApduRecord->sBufferedApduInfo.u2SecurityLevel = %d \r\n", psZgpBufferedApduRecord->sBufferedApduInfo.u2SecurityLevel);
	/* Check ZGPD requested key from Commission Payload */
	if((psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[1] & GP_COMM_CMD_ZGP_SEC_KEY_REQ_MASK) && (u8SecKeyType != E_GP_NO_KEY ) &&
				 (u8SecKeyType != E_GP_OUT_OF_THE_BOX_ZGPD_KEY))
	/*if((psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[1] & GP_COMM_CMD_ZGP_SEC_KEY_REQ_MASK) )*/
	{
		/*if((u8SecKeyType == E_GP_NO_KEY )|| (u8SecKeyType == E_GP_OUT_OF_THE_BOX_ZGPD_KEY))
		{
			uint8 u8SinkTableKeyType = (psSinkProxyTableEntry->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2;
			if(u8SinkTableKeyType == E_GP_OUT_OF_THE_BOX_ZGPD_KEY)
			{
				memcpy(&uSecurityKey.au8[0],psSinkProxyTableEntry->sZgpdKey.au8Key, E_ZCL_KEY_128_SIZE);
				u8SecKeyType =u8SinkTableKeyType;
			}
		}*/
		#ifdef CLD_GP_ATTR_ZGP_LINK_KEY
		 /* check whether is GPD is capable of decrypting security key or not */
		if(psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[2] & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_ENC_MASK)
		{
			uint8 u8FrameCounterIndex = 3;

			if(psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[2] & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_PRESENT_MASK)
			{
				u8FrameCounterIndex += E_ZCL_KEY_128_SIZE + 4; //key size and MIC size
			}

			u32FrameCounter = psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[u8FrameCounterIndex++];
			u32FrameCounter |= psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[u8FrameCounterIndex++] << 8;
			u32FrameCounter|= psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[u8FrameCounterIndex++]<< 16;
			u32FrameCounter|= psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[u8FrameCounterIndex]<< 24;

			u32FrameCounter++;

			/* Encryption of Key and Adding MIC */
			bEncryptDecryptKey(
								TRUE,
							   psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId,
							   &psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr,
							  u32FrameCounter,
								 &uSecurityKey.au8[0],
								 &uOutKey.au8[0],
								 (uint8*)&u32MIC,
								psEndPointDefinition,
								psGpCustomDataStructure
								);


			pu8GPResponsePayload[0] |= 0x04; // key encryption bit

			uSecurityKey = uOutKey;


		}
 #endif
    pu8GPResponsePayload[0] |= (1 << 1); //key present

	 memcpy(&pu8GPResponsePayload[u8Length],
			uSecurityKey.au8,
			E_ZCL_KEY_128_SIZE);

         u8Length += E_ZCL_KEY_128_SIZE;
 #ifdef CLD_GP_ATTR_ZGP_LINK_KEY
         /* check whether is GPD is capable of decrypting security key or not */
         if(psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[2] & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_ENC_MASK)
         {

#ifdef LITTLE_ENDIAN_PROCESSOR
             pu8GPResponsePayload[u8Length++] = (uint8)(u32MIC );
             pu8GPResponsePayload[u8Length++] = (uint8)(u32MIC >> 8);
             pu8GPResponsePayload[u8Length++] = (uint8)(u32MIC >> 16);
             pu8GPResponsePayload[u8Length++] = (uint8)(u32MIC >> 24);
             DBG_vPrintf(TRACE_GP_DEBUG, "\nMIC HERE  :0x%08x\r\n", u32MIC);
#else
             pu8GPResponsePayload[u8Length++] = (uint8)(u32MIC >> 24);
             pu8GPResponsePayload[u8Length++] = (uint8)(u32MIC >> 16);
             pu8GPResponsePayload[u8Length++] = (uint8)(u32MIC >> 8);
             pu8GPResponsePayload[u8Length++] = (uint8)(u32MIC);
             DBG_vPrintf(TRACE_GP_DEBUG, "\nMIC HERE  :0x%08x\r\n", u32MIC);
#endif
             /* add frame counter */
             pu8GPResponsePayload[u8Length++] = (uint8)(u32FrameCounter );
             pu8GPResponsePayload[u8Length++] = (uint8)(u32FrameCounter >> 8);
             pu8GPResponsePayload[u8Length++] = (uint8)(u32FrameCounter >> 16);
             pu8GPResponsePayload[u8Length++] = (uint8)(u32FrameCounter >> 24);
         }

         else
         {
         	 DBG_vPrintf(TRACE_GP_DEBUG, "\nMIC NOT ADDED = option = %d   condition = %d\r\n", psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[2],
         			 (psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[2] & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_ENC_MASK));
         }

 #endif
         pu8GPResponsePayload[0] |= (u8SecKeyType) << 5;//key type
     }
     if(psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[2] & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_PRESENT_MASK)
	 {

    	 DBG_vPrintf(TRACE_GP_DEBUG, "\n key present\r\n");
    	 /* update received key in sink table */

    	 /* decrypt key if sent encrypted */
    	 if(psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[2] & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_ENC_MASK)
    	 {
    		 /* Decryption of Key and Adding MIC */
			bEncryptDecryptKey(
					FALSE,
					psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId,
					&psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr,
					0,
					&(psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[0])+3,
					 &uOutKey.au8[0],
					&(psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[0])+3+E_ZCL_KEY_128_SIZE,
					psEndPointDefinition,
					psGpCustomDataStructure);


    	 }
    	 else
    	 {
    		 memcpy(uOutKey.au8,(&(psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[0])+3),E_ZCL_KEY_128_SIZE );
    	 }
    	 if((u8SecKeyType == E_GP_NO_KEY ) ||
    				 (u8SecKeyType == E_GP_OUT_OF_THE_BOX_ZGPD_KEY) || ((psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[1] & GP_COMM_CMD_ZGP_SEC_KEY_REQ_MASK) ==
    						 FALSE))
    	 {
    		 uint8 u8GPDKeyType = (psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[2] & GP_COMM_CMD_EXT_OPT_KEY_TYPE_MASK)>> 2;
    		 pu8GPResponsePayload[0] |= (u8GPDKeyType) << 5;//key type

    	 }

	 }
     *pu8Payloadlength = u8Length ;

 }
#endif
 /****************************************************************************
  **
  ** NAME:       vGP_TxPairing
  **
  ** DESCRIPTION:
  ** Transmits device announce on behalf of GPD based on command buffered
  **
  ** PARAMETERS:                                  Name                        Usage
  ** uint8 							            u8SrcEndPointId             Source Endpoint
  ** tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord    Buffered Command
  ** tsGP_ZgppProxySinkTable                          *psZgpsSinkTable			Sink Table entry
  **
  ** RETURN:
  ** None
  **
  ****************************************************************************/
 #ifdef GP_COMBO_BASIC_DEVICE
 PRIVATE void vGP_TxPairing(
	 		tsZCL_EndPointDefinition                           *psEndPointDefinition,
	 		tsGP_GreenPowerCustomData                          *psGpCustomDataStructure,
 		    tsGP_ZgpBufferedApduRecord                         *psZgpBufferedApduRecord
 		)
 {
	 uint8 u8CommunicationMode;
     tsGP_ZgpPairingCmdPayload sZgpPairingCmdPayload;
	 tsGP_ZgppProxySinkTable                              *psSinkProxyTableEntry;
     tsZCL_Address                               sDestinationAddress;
     uint8 u8Status;
     uint8 u8TransactionSequenceNumber;
     sDestinationAddress.eAddressMode = E_ZCL_AM_BROADCAST;
     sDestinationAddress.uAddress.eBroadcastMode = ZPS_E_APL_AF_BROADCAST_RX_ON;

	DBG_vPrintf(TRACE_GP_DEBUG, "vGP_TxPairing\n");

   	 if( bGP_IsSinkTableEntryPresent(
			 psEndPointDefinition->u8EndPointNumber,
			 psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId,
			 &psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr,
             &psSinkProxyTableEntry,
             0xff) == FALSE)
   	 {
   		DBG_vPrintf(TRACE_GP_DEBUG, "\n vGP_TxPairingbGP_IsSinkTableEntryPresent FALSE \n" );
   		return;
	 }

   	 if(/*( psSinkProxyTableEntry->u8GPDPaired == E_GP_PAIRING_NOT_COMPLETED) && */(E_GP_ADD_PAIRING == psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand))
   	 {

		if(psGpCustomDataStructure->bIsCommissionReplySent)
		{
			psGpCustomDataStructure->bIsCommissionReplySent = 0;
		}

		psSinkProxyTableEntry->u8GPDPaired = E_GP_PAIRED;
   		psSinkProxyTableEntry->bProxyTableEntryOccupied = 1;
		psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_SINK_PROXY_TABLE_ENTRY_ADDED;
		psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psZgpsProxySinkTable = psSinkProxyTableEntry;
		psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
		/*  add check if group exits for precommissioned mode */
		 if((psSinkProxyTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
		 {
			psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_UPDATE_SINK_TABLE_GROUP_INFO;
			psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psZgpsProxySinkTable = psSinkProxyTableEntry;
			psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
			if(psSinkProxyTableEntry->u8SinkGroupListEntries == 0)
			 {
				psSinkProxyTableEntry->bProxyTableEntryOccupied = TRUE;
				 /* invalid case, remove sink table  */
				 vGP_RemoveGPDFromProxySinkTable( psEndPointDefinition->u8EndPointNumber, psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId,
						 &psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr);
				 DBG_vPrintf(TRACE_GP_DEBUG, "\n psSinkProxyTableEntry->u8SinkGroupListEntries = %d \n", psSinkProxyTableEntry->u8SinkGroupListEntries );
				 return ;
			 }
			else
			{
		    	vGP_AddGPEPToGroup( psGpCustomDataStructure,
		    						psEndPointDefinition,
		    						psSinkProxyTableEntry);
			}
		}
		vGP_CallbackForPersistData();

   	 }



     /* initializing pairing Command */
     memset((uint8*)&sZgpPairingCmdPayload, 0, sizeof(tsGP_ZgpPairingCmdPayload));


     /* remove PS GROUP created if pairing to be removed */
    if(psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand == E_GP_REMOVE_PAIRING)
    {
		if((psSinkProxyTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK )== E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)
		{
			/*  remove derived group id */
			uint16  u16DerivedGroupId;
			u16DerivedGroupId = u16GP_DeriveAliasSrcAddr(
					(uint8)psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId,
					psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr);

			 ZPS_eAplZdoGroupEndpointRemove(u16DerivedGroupId, ZCL_GP_PROXY_ENDPOINT_ID);
			 DBG_vPrintf(TRACE_GP_DEBUG, "\nRemoved EP = 0x%4x \n" , u16DerivedGroupId);
		}
		else if((psSinkProxyTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK)== E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
		{
			/* Find No. of pre-commissioned groups are presented in the list  and remove */
			uint8   u8Count;

			for(u8Count = 0; u8Count < psSinkProxyTableEntry->u8SinkGroupListEntries; u8Count++)
			{
				 ZPS_eAplZdoGroupEndpointRemove(psSinkProxyTableEntry->asSinkGroupList[u8Count].u16SinkGroup, ZCL_GP_PROXY_ENDPOINT_ID);
			}

		}
    }
     sZgpPairingCmdPayload.b24Options = psSinkProxyTableEntry->b16Options & GP_APPLICATION_ID_MASK;

     if((psSinkProxyTableEntry->b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_4_BYTE)
     {
         sZgpPairingCmdPayload.uZgpdDeviceAddr.u32ZgpdSrcId = psSinkProxyTableEntry->uZgpdDeviceAddr.u32ZgpdSrcId;
     }
 #ifdef GP_IEEE_ADDR_SUPPORT
     else
     {
         sZgpPairingCmdPayload.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr =
         		psSinkProxyTableEntry->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
         if(psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand == E_GP_REMOVE_PAIRING)
         {
        	 sZgpPairingCmdPayload.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint =
        			 psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
         }
         else
         {
         sZgpPairingCmdPayload.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint =
         		psSinkProxyTableEntry->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
         }
     }
 #endif
     /* check communication mode */
     u8CommunicationMode = psSinkProxyTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK;

     if(psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand == E_GP_REMOVE_PAIRING)
	 {
    	 sZgpPairingCmdPayload.b24Options |= GP_PAIRING_REMOVE_GPD_MASK;
    	 /* If  pairing is being removed; then, the following fields are not present: DeviceID, GPD security Frame Counter,
    	  *  GPD key, AssignedAlias, and ForwardingRadius.*/
	 }
     else
     {

    	 sZgpPairingCmdPayload.b24Options |= GP_PAIRING_ADD_SINK_MASK;

    	 if(psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK )
		 {
			 /* Set ZGPDKey present flag */
			 sZgpPairingCmdPayload.b24Options |=  GP_PAIRING_SECURITY_KEY_PRESENT_MASK;

			 /* Copy Key from Sink Table */
			 memcpy(sZgpPairingCmdPayload.sZgpdKey.au8Key, psSinkProxyTableEntry->sZgpdKey.au8Key, E_ZCL_KEY_128_SIZE);
		 }
		 /* Set Assigned allias flag based on sink table flag for this ZGPD */
		 if(psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_ASSIGNED_ALIAS_MASK)
		 {
			 sZgpPairingCmdPayload.b24Options |= GP_PAIRING_ASSIGNED_ALIAS_PRESENT_MASK;
			 sZgpPairingCmdPayload.u16AssignedAlias = psSinkProxyTableEntry->u16ZgpdAssignedAlias;
		 }
	     /* Set security frame counter flag */
	     sZgpPairingCmdPayload.b24Options |= GP_PAIRING_SECURITY_COUNTER_PRESENT_MASK;
	     if((u8CommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID )||
	               ( u8CommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)||
	               (u8CommunicationMode == E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_LIGHTWEIGHT))
	        {
	            /* Set Forward Radius flag */
	            sZgpPairingCmdPayload.b24Options |= GP_PAIRING_FORWARDING_RADIUS_PRESENT_MASK;
	            sZgpPairingCmdPayload.u8ForwardingRadius = psSinkProxyTableEntry->u8GroupCastRadius;

	            /* check communication mode */
	            if(u8CommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)
	            {
	                sZgpPairingCmdPayload.u16SinkGroupID = u16GP_DeriveAliasSrcAddr(
	                                                        (uint8)(psSinkProxyTableEntry->b16Options & GP_APPLICATION_ID_MASK),
	                                                        sZgpPairingCmdPayload.uZgpdDeviceAddr);
	            }
	            if(u8CommunicationMode == E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_LIGHTWEIGHT)
	            {
	                sZgpPairingCmdPayload.u16SinkNwkAddress = ZPS_u16AplZdoGetNwkAddr();
	                sZgpPairingCmdPayload.u64SinkIEEEAddress = ZPS_u64AplZdoGetIeeeAddr();
	            }
	        }

	        /* Set ZGPD Device Id */
	        sZgpPairingCmdPayload.u8DeviceId = psSinkProxyTableEntry->eZgpdDeviceId;
     }

     /* Set Default Communication mode of the Sink  */
     sZgpPairingCmdPayload.b24Options |= u8CommunicationMode << 5;

     /* Set ZGPD Fixed flag, if it is the auto commission packet, set fixed to 0 */
     if(!psZgpBufferedApduRecord->sBufferedApduInfo.bAutoCommissioning)
     {
         sZgpPairingCmdPayload.b24Options |=( (psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_GPD_FIXED_MASK) >> 11) << 7;

         /* Set MAC Seq Cap Flag */
         sZgpPairingCmdPayload.b24Options |=
             (((psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_SEQ_NUM_CAP_MASK) >> 5) << 8);
     }
     else
     {
         /* For Auto Comm Pkt, this flag always to be set 1 */
         //sZgpPairingCmdPayload.b24Options |= (TRUE << 8);
     }

     /* Set Security Level */
     sZgpPairingCmdPayload.b24Options |= ((psSinkProxyTableEntry->b8SecOptions & GP_SECURITY_LEVEL_MASK) << 9);
     /* Set Add/Remove Sink flag for this ZGPD */

     /* Set Sec Key type */
    // sZgpPairingCmdPayload.b24Options |= ((u8KeyType) << 11);
     sZgpPairingCmdPayload.b24Options |= ((psSinkProxyTableEntry->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2)  << 11;
     DBG_vPrintf(TRACE_GP_DEBUG, "\n u8KeyType = 0x%x psSinkProxyTableEntry->u32ZgpdSecFrameCounter = %d \n" ,
    		 ((psSinkProxyTableEntry->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2)  << 11,
    		 psSinkProxyTableEntry->u32ZgpdSecFrameCounter);

     /* check security level or MAC seq number Capable */
     sZgpPairingCmdPayload.u32ZgpdSecFrameCounter = psSinkProxyTableEntry->u32ZgpdSecFrameCounter;

     if(u8CommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
     {
         uint8 i;
         tsGP_ZgpPairingConfigCmdPayload sZgpPairingConfigPayload;
         memset(&sZgpPairingConfigPayload , 0, sizeof(tsGP_ZgpPairingConfigCmdPayload));
         for(i = 0; i < psSinkProxyTableEntry->u8SinkGroupListEntries; i++)
         {
        	 if(psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand == E_GP_ADD_PAIRING)
			 {
				 sZgpPairingCmdPayload.u16SinkGroupID = psSinkProxyTableEntry->asSinkGroupList[i].u16SinkGroup;
				 sZgpPairingCmdPayload.u16AssignedAlias = psSinkProxyTableEntry->asSinkGroupList[i].u16Alias;
				 if(sZgpPairingCmdPayload.u16AssignedAlias != 0xFFFF)
				 {
					 sZgpPairingCmdPayload.b24Options |= GP_PAIRING_ASSIGNED_ALIAS_PRESENT_MASK;
				 }

			 }
             /* Send GP Pairing command as broadcast */
        	 u8Status = eGP_ZgpPairingSend(
            		 psEndPointDefinition->u8EndPointNumber,
                      ZCL_GP_PROXY_ENDPOINT_ID,
                      &sDestinationAddress,
                      &u8TransactionSequenceNumber,
                      &sZgpPairingCmdPayload);
             DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_ZgpPairingSend status = %d  i = %d group id = 0x%2x \n" , u8Status, i,sZgpPairingCmdPayload.u16SinkGroupID );

         }

         /* Send Pairing Configuration Command */
         if(psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand == E_GP_REMOVE_PAIRING)
         {
        	 sZgpPairingConfigPayload.u8Actions = E_GP_PAIRING_CONFIG_REMOVE_GPD;
         }
         else
         {
        	 sZgpPairingConfigPayload.u8Actions = E_GP_PAIRING_CONFIG_EXTEND_SINK_TABLE_ENTRY;
         }
         sZgpPairingConfigPayload.b16Options = psSinkProxyTableEntry->b16Options & GP_APPLICATION_ID_MASK ;
         sZgpPairingConfigPayload.b16Options |= (u8CommunicationMode << 3) ;
         sZgpPairingConfigPayload.b16Options |= (psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_SEQ_NUM_CAP_MASK);
         sZgpPairingConfigPayload.b16Options |= (psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_UNICAST_GPS_MASK);
         sZgpPairingConfigPayload.b16Options |= (psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_GPD_FIXED_MASK) >>  4 ;
         sZgpPairingConfigPayload.b16Options |= (psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_ASSIGNED_ALIAS_MASK) >> 5;
         sZgpPairingConfigPayload.b16Options |= (psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK) >> 5 ;
         sZgpPairingConfigPayload.u8ForwardingRadius = psSinkProxyTableEntry->u8GroupCastRadius;
         sZgpPairingConfigPayload.u8NumberOfPairedEndpoints = 0xFE;

         /* Check App Id */
         if((psSinkProxyTableEntry->b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_4_BYTE)
         {
             sZgpPairingConfigPayload.uZgpdDeviceAddr.u32ZgpdSrcId = psSinkProxyTableEntry->uZgpdDeviceAddr.u32ZgpdSrcId;
         }
 #ifdef GP_IEEE_ADDR_SUPPORT
         else
         {
             sZgpPairingConfigPayload.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr =
             		psSinkProxyTableEntry->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
             sZgpPairingConfigPayload.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint =
             		psSinkProxyTableEntry->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
         }
 #endif
         sZgpPairingConfigPayload.eZgpdDeviceId = psSinkProxyTableEntry->eZgpdDeviceId;
         sZgpPairingConfigPayload.u8SinkGroupListEntries = psSinkProxyTableEntry->u8SinkGroupListEntries;
         sZgpPairingConfigPayload.u16ZgpdAssignedAlias = psSinkProxyTableEntry->u16ZgpdAssignedAlias;
         sZgpPairingConfigPayload.b8SecOptions = psSinkProxyTableEntry->b8SecOptions;
         sZgpPairingConfigPayload.u32ZgpdSecFrameCounter = psSinkProxyTableEntry->u32ZgpdSecFrameCounter;
         sZgpPairingConfigPayload.sZgpdKey = psSinkProxyTableEntry->sZgpdKey;

         for(i = 0; i < psSinkProxyTableEntry->u8SinkGroupListEntries; i++)
         {
             sZgpPairingConfigPayload.asSinkGroupList[i].u16SinkGroup = psSinkProxyTableEntry->asSinkGroupList[i].u16SinkGroup;
             sZgpPairingConfigPayload.asSinkGroupList[i].u16Alias = psSinkProxyTableEntry->asSinkGroupList[i].u16Alias;
         }

         u8Status = eGP_ZgpPairingConfigSend(
        		 psEndPointDefinition->u8EndPointNumber,
                 ZCL_GP_PROXY_ENDPOINT_ID,
                 &sDestinationAddress,
                 &u8TransactionSequenceNumber,
                 &sZgpPairingConfigPayload);
         DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_ZgpPairingConfigSend u8CommunicationMode = %d  sZgpPairingConfigPayload.b16Options = 0x%4x \n" ,
        		 u8CommunicationMode,
        		 sZgpPairingConfigPayload.b16Options);

     }
     else
     {

         /* Send GP Pairing command as broadcast */
    	 u8Status = eGP_ZgpPairingSend(
        		     psEndPointDefinition->u8EndPointNumber,
                     ZCL_GP_PROXY_ENDPOINT_ID,
                     &sDestinationAddress,
                     &u8TransactionSequenceNumber,
                     &sZgpPairingCmdPayload);
         DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_ZgpPairingSend status = %d \n" , u8Status);
     }
     /* remove sink table and exit commissioning mode if pairing to be removed */
     if((psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand == E_GP_REMOVE_PAIRING) )
	 {
    	 if( psGpCustomDataStructure->bCommissionExitModeOnFirstPairSuccess)
    	 {
			 /* exit commissioning */
			 vGP_ExitCommMode(psEndPointDefinition,psGpCustomDataStructure);
    	 }

    	 /* Remove details of this GPD */
    	 vGP_RemoveGPDFromProxySinkTable( psEndPointDefinition->u8EndPointNumber,  psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId,
    			 &psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr);
	 }
 }




 /****************************************************************************
  **
  ** NAME:       vGP_CreateChannelConfig
  **
  ** DESCRIPTION:
  ** Creates channel configuration command from buffered command
  **
  ** PARAMETERS:                                  Name                        Usage
  ** tsGP_ZgpBufferedApduRecord             *psZgpBufferedApduRecord,
  ** tsGP_ZgpResponseCmdPayload             *psZgpResponseCmdPayload,
  ** uint8                                  *pu8GPResponsePayload
  **
  ** RETURN:
  ** None
  **
  ****************************************************************************/
 PRIVATE void vGP_CreateChannelConfig(
 		tsGP_ZgpBufferedApduRecord                        *psZgpBufferedApduRecord,
 		tsGP_ZgpResponseCmdPayload                         *psZgpResponseCmdPayload,
 		uint8                                               *pu8GPResponsePayload)
 {

 	psZgpResponseCmdPayload->b8TempMasterTxChannel = (psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[0] & 0x0F);
 	psZgpResponseCmdPayload->eZgpdCmdId = E_GP_CHANNEL_CONFIGURATION;
 	psZgpResponseCmdPayload->sZgpdCommandPayload.u8Length = 1;
 	psZgpResponseCmdPayload->sZgpdCommandPayload.u8MaxLength = 1;
 	psZgpResponseCmdPayload->sZgpdCommandPayload.pu8Data = pu8GPResponsePayload;
 	*pu8GPResponsePayload = ZPS_u8AplZdoGetRadioChannel() - 11;
 	*pu8GPResponsePayload |=GP_CHANNEL_CONFIG_BASIC_BIT;
 }
#endif
 /****************************************************************************
  **
  ** NAME:       vCreateNotification
  **
  ** DESCRIPTION:
  ** Creates GP notification command
  **
  ** PARAMETERS:                                  Name                        Usage
  ** uint8                                        u8KeyType
  ** tsGP_ZgpNotificationCmdPayload               *psZgpNotificationPayload
  ** tsGP_ZgpBufferedApduRecord                 *psZgpBufferedApduRecord
  **
  ** RETURN:
  ** None
  **
  ****************************************************************************/
PRIVATE void  vCreateNotification(
		uint8                                        u8KeyType,
		tsGP_ZgpNotificationCmdPayload               *psZgpNotificationPayload,
		 tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord )
 {
     /* Check Application Id */
     /* Copy Green Power Device Address */
	 uint16 u16SecMask;


	 psZgpNotificationPayload->b16Options = GP_NOTIFICATION_PROXY_INFO_PRESENT;
	 psZgpNotificationPayload->b16Options |= GP_NOTIFICATION_TX_QUEUE_FULL;

	 psZgpNotificationPayload->u16ZgppShortAddr = ZPS_u16AplZdoGetNwkAddr();

	 psZgpNotificationPayload->u8GPP_GPD_Link = psZgpBufferedApduRecord->sBufferedApduInfo.u8GPP_GPDLink;

     if(psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId == GP_APPL_ID_4_BYTE)
     {
         psZgpNotificationPayload->b16Options|= GP_APPL_ID_4_BYTE;
     }
 #ifdef GP_IEEE_ADDR_SUPPORT
     else
     {
        psZgpNotificationPayload->b16Options |= GP_APPL_ID_8_BYTE;
     }
 #endif
	 psZgpNotificationPayload->uZgpdDeviceAddr = psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr;
	 psZgpNotificationPayload->eZgpdCmdId = (teGP_ZgpdCommandId)psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId;
	 psZgpNotificationPayload->sZgpdCommandPayload.u8MaxLength = GP_MAX_ZB_CMD_PAYLOAD_LENGTH;

	 psZgpNotificationPayload->sZgpdCommandPayload.u8Length =
			 psZgpBufferedApduRecord->sBufferedApduInfo.u8PDUSize;
	 /* Copy Payload */
	 if((psZgpNotificationPayload->sZgpdCommandPayload.u8Length == 0 ) || (psZgpNotificationPayload->sZgpdCommandPayload.u8Length == 0xFF))
	 {
		 psZgpNotificationPayload->sZgpdCommandPayload.pu8Data  = NULL;
	 }
	 else
	 {
		 memcpy(psZgpNotificationPayload->sZgpdCommandPayload.pu8Data ,
						 psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU,
						 psZgpNotificationPayload->sZgpdCommandPayload.u8Length);
	 }

	 /* Check RxAfter Flag of ZGP data indication */
	if( psZgpBufferedApduRecord->sBufferedApduInfo.bRxAfterTx)
	 {
		 psZgpNotificationPayload->b16Options |= GP_NOTIFICATION_APPOINT_TEMP_MASTER_MASK;

	 }

	 /* Set Security Level */
	 u16SecMask = (uint16)(psZgpBufferedApduRecord->sBufferedApduInfo.u2SecurityLevel << 6);
	 psZgpNotificationPayload->b16Options |= u16SecMask;

	 /* Set Key type  if security level greater than zero*/
	 if(u16SecMask)
	 {
		 u16SecMask = 0;
		 u16SecMask = (uint16)(u8KeyType << 8);
		 psZgpNotificationPayload->b16Options |= u16SecMask;
	 }

	 /* Set the Security Frame Counter */
	 psZgpNotificationPayload->u32ZgpdSecFrameCounter =
	 				 psZgpBufferedApduRecord->sBufferedApduInfo.u32SecFrameCounter;
 }
 /****************************************************************************
  **
  ** NAME:       eGP_GreenPowerDataCmdHandler
  **
  ** DESCRIPTION:
  ** Green Power direct data command handler
  **
  ** PARAMETERS:                    Name                           Usage
  **  tsGP_ZgpBufferedApduRecord  *psZgpBufferedApduRecord,
  ** tsZCL_EndPointDefinition    *psEndPointDefinition       endpoint definition
  ** tsZCL_ClusterInstance       *psClusterInstance          cluster instance
  **
  ** RETURN:
  ** teZCL_Status
  ****************************************************************************/
 PRIVATE teZCL_Status eGP_GreenPowerDataCmdHandler(
		 tsGP_ZgpBufferedApduRecord                 *psZgpBufferedApduRecord,
		 tsZCL_EndPointDefinition                   *psEndPointDefinition,
		 tsGP_GreenPowerCustomData                  *psGpCustomDataStructure)
{
     bool_t                       bIsEntryPresent;
     tsGP_ZgppProxySinkTable          *psSinkProxyTableEntry;
     teZCL_Status                 eStatus = E_ZCL_SUCCESS;

     teGP_GreenPowerSecLevel      eSecurityLevel = E_GP_NO_SECURITY;


     bIsEntryPresent =
    		 bGP_IsProxyTableEntryPresent(
    		    		     		psEndPointDefinition->u8EndPointNumber,
#ifdef GP_COMBO_BASIC_DEVICE
    		    		     		TRUE,
#else
    		    		     		FALSE,
#endif
    		    		     	psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId,
    		    		        &psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr,
    		    		     		
    		    		     		&psSinkProxyTableEntry );

    	 DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_GreenPowerDataCmdHandler bGP_IsProxyTableEntryPresent =%d\n",
    			 bIsEntryPresent);



     if(bIsEntryPresent)
     {
         if((!(psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_ENTRY_VALID_MASK))||
           (!(psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_ENTRY_ACTIVE_MASK)))
         {
        	 DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_GreenPowerDataCmdHandler GP_PROXY_TABLE_ENTRY_VALID_MASK = %d, other = %d\n",
        			 psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_ENTRY_VALID_MASK,
        			 psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_ENTRY_ACTIVE_MASK);
        	 bIsEntryPresent = FALSE;
         }
     }
     //DBG_vPrintf(TRACE_GP_DEBUG, "bGP_IsProxyTableEntryPresent:%d\r\n", bIsProxyTableEntryPresent);
     if(bIsEntryPresent)
     {
         if(psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_ENTRY_ACTIVE_MASK)
         {
             /* If there is an active Proxy Table entry for this GPD ID with the InRange flag set to 0b0,
              * the Proxy sets the InRange flag to 0b1 */
             if(!(psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_IN_RANGE_MASK))
             {
                 psSinkProxyTableEntry->b16Options |= GP_PROXY_TABLE_IN_RANGE_MASK;

             }

             if(psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_ENTRY_VALID_MASK)
             { /*  check the security level */

                 if(psSinkProxyTableEntry->b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK)
                 {
                     eSecurityLevel = psSinkProxyTableEntry->b8SecOptions & GP_SECURITY_LEVEL_MASK;
                 //    eSecurityKeyType = (psSinkProxyTableEntry->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2;

                     /* check for security level, device commissioned with some security level (>0) but send unprotected command */
                     if(eSecurityLevel != (uint8)psZgpBufferedApduRecord->sBufferedApduInfo.u2SecurityLevel)
                     {
                    	 DBG_vPrintf(TRACE_GP_DEBUG,"Proxy table sec mismatch %d, received = %d: \r\n",
                    			 (psSinkProxyTableEntry->b8SecOptions & GP_SECURITY_LEVEL_MASK),
                    			 (uint8)psZgpBufferedApduRecord->sBufferedApduInfo.u2SecurityLevel);
                         return E_ZCL_SUCCESS;
                     }
                 }


             }
         }

     }
#ifdef GP_COMBO_BASIC_DEVICE
     if(bIsEntryPresent)
     {
		 if ((psSinkProxyTableEntry->eGreenPowerSinkTablePriority != 0) && (psSinkProxyTableEntry->u8GPDPaired == E_GP_PAIRED))
    	 {

			 /* if ZGPD is in range , its priority in the sink table should be 2*/
			 if((psSinkProxyTableEntry->eGreenPowerSinkTablePriority == E_GP_SINK_TABLE_P_3) &&
					 (psZgpBufferedApduRecord->sBufferedApduInfo.bTunneledPkt == FALSE))
			 {
				 psSinkProxyTableEntry->eGreenPowerSinkTablePriority = E_GP_SINK_TABLE_P_2;
				 vGP_CallbackForPersistData();
			 }
			 DBG_vPrintf(TRACE_GP_DEBUG, "eGP_GPDFHandler  callig with cmd=%d..\n",
					 psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId, psSinkProxyTableEntry->u8SinkGroupListEntries);
			 if(eGP_GPDFHandler(NULL, // pZPSevent,
								 psEndPointDefinition,
								 psGpCustomDataStructure,
								 (uint8)psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId,
								 &psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr,
								 psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId,
								 psZgpBufferedApduRecord->sBufferedApduInfo.u8PDUSize,
								 psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU) == E_ZCL_SUCCESS)
			{
			 /* if device has translation entry for this ZGPD and paired, its priority in the
				sink table should be 1*/
				 if((psSinkProxyTableEntry->eGreenPowerSinkTablePriority == E_GP_SINK_TABLE_P_2)||
					(psSinkProxyTableEntry->eGreenPowerSinkTablePriority == E_GP_SINK_TABLE_P_3))
				 {
					 psSinkProxyTableEntry->eGreenPowerSinkTablePriority = E_GP_SINK_TABLE_P_1;
				 }
			 }
    	 }

     }

 #endif

	if(bIsEntryPresent)

	{
	  /* Buffer the GP Notification packet for Dmin */
	  if(psZgpBufferedApduRecord->sBufferedApduInfo.bTunneledPkt == FALSE)
	  {
		  tsGP_ZgpDataIndication sZgpDataIndication;
		  vCreateDataIndFromBufferedCmd(psZgpBufferedApduRecord,&sZgpDataIndication);
		 eStatus = eGp_BufferTransmissionPacket(
							&sZgpDataIndication,
							E_GP_NOTIFICATION,
							psGpCustomDataStructure);
		 DBG_vPrintf(TRACE_GP_DEBUG,"eGp_BufferTransmissionPacket E_GP_NOTIFICATION status = %d : \r\n",eStatus);
	  }

	}
	return eStatus;
 }

 /****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
