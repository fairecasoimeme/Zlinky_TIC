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
 * MODULE:             Green Power Cluster
 *
 * COMPONENT:          GreenPowerCommon.c
 *
 * DESCRIPTION:        The API for the Green Power Cluster
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>
#include <stdlib.h>
#include "zcl.h"
#include "zcl_options.h"
#include "GreenPower.h"
#include "GreenPower_internal.h"
#include "Identify.h"
#include "LevelControl.h"
#include "OnOff.h"
#include "dbg.h"
#include "rnd_pub.h"
#include "zcl_customcommand.h"
#include "pdum_gen.h"
#include "portmacro.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef TRACE_GP_DEBUG
#define TRACE_GP_DEBUG FALSE
#endif



#define QUALITY_BASED_DELAY_FACTOR		32
#define RSSI_MIN                        -110
#define RSSI_MAX						8
#define LINK_QUALITY_BIT_POS			6

#define D_MAX                           100
#define E_GP_ALL_ENPOINTS               0xFF
#define E_GP_EP_INDEPENDENT				0x00
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef GP_COMBO_BASIC_DEVICE
PRIVATE teZCL_Status eGP_TranslateCommandIntoZcl(
                    uint8                                   u8GpEndPointId,
                    uint8                                   u8MatchedIndex,
                    uint8                                   u8MatchedCmdIndex,
                    tsGP_GreenPowerCustomData               *psGpCustomDataStructure,
                    uint8                                   u8GpdCommandPayloadLength,
                    uint8                                   *pu8GpdCommandPayload);


PRIVATE teZCL_Status eGP_BufferLoopBackPacket(
                    tsZCL_EndPointDefinition                *psEndPointDefinition,
                    tsGP_GreenPowerCustomData               *psGpCustomDataStructure,
                    uint8                                   u8TranslationIndex,
                    uint8                                   u8ApplicationId,
                    tuGP_ZgpdDeviceAddr                     *puZgpdAddress,
                    teGP_ZgpdCommandId                      eZgpdCommandId,
                    uint8                                   u8GpdCommandPayloadLength,
                    uint8                                   *pu8GpdCommandPayload);

PRIVATE bool_t bIsCommandMapped(
		            teGP_ZgpdCommandId                      eZgpdCommandId,
					uint8                					*pMatchedCmdIndex,
					tsGP_TranslationTableEntry 			    *psTranslationTableEntry);
PRIVATE void vGP_TxLoopBackCmds(
		            uint8                                   u8SrcEndPointId,
		            tsGP_ZgpBufferedApduRecord              *psZgpBufferedApduRecord,
                    tsGP_GreenPowerCustomData               *psGpCustomDataStructure);

PRIVATE bool_t bGP_IsGPDPresent(
                    uint8                                  u8GpEndPointId,
                    bool								   bIsServer,
                    uint8                                  u8ApplicationId,
                    tuGP_ZgpdDeviceAddr                    *puZgpdAddress,
                    tsGP_ZgppProxySinkTable                **psProxySinkTableEntry);

#endif
bool_t  bGP_SecurityValidation(
		            tsGP_ZgpDataIndication                  *psZgpDataIndication,
		            tsZCL_EndPointDefinition    			*psEndPointDefinition,
		            tsGP_GreenPowerCustomData               *psGpCustomDataStructure,
		            tsGP_ZgppProxySinkTable                 *psZgpsSinkTable);
bool_t bGP_ValidateComissionCmdFields(
		           tsGP_ZgpDataIndication               	*psZgpDataIndication ,
		           tsZCL_EndPointDefinition    				*psEndPointDefinition,
	               tsGP_GreenPowerCustomData                *psGpCustomDataStructure);


uint8 u8GP_GetLQIQuality(
		           uint8                                    u8LinkQuality);



bool_t bGP_FillCommInd(
		          tsGP_ZgpCommissionIndication              *psZgpCommissionIndication,
		          tsGP_ZgpDataIndication               		*psZgpDataIndication,
		          tsZCL_EndPointDefinition    							*psEndPointDefinition,
		          tsGP_GreenPowerCustomData                              *psGpCustomDataStructure);

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
 ** NAME:       bGP_IsSinkTableEntryPresent
 **
 ** DESCRIPTION:
 ** Checks if sink table entry present for given GPD
 **
 ** PARAMETERS:                         Name                            Usage
 ** uint8                               u8GpEndPointId                  Endpoint Id
 ** uint8                               u8ApplicationId                 Application Id
 ** tuGP_ZgpdDeviceAddr                 *puZgpdAddress                  Pointer to GP device address
 ** tsGP_ZgppProxySinkTable                  *psSinkTableEntry               Pointer to Sink Table Entry
 ** teGP_GreenPowerCommunicationMode    eCommunicationMode              communication mode for GPD
 **
 ** RETURN:
 ** TRUE if present, FALSE otherwise
 **
 ****************************************************************************/
#ifdef GP_COMBO_BASIC_DEVICE
PUBLIC bool_t bGP_IsSinkTableEntryPresent(
                    uint8                                  u8GpEndPointId,
                    uint8                                  u8ApplicationId,
                    tuGP_ZgpdDeviceAddr                    *puZgpdAddress,
                    tsGP_ZgppProxySinkTable                **psSinkTableEntry,
                    teGP_GreenPowerCommunicationMode       eCommunicationMode)
{
    tsZCL_EndPointDefinition                *psEndPointDefinition;
    tsZCL_ClusterInstance                   *psClusterInstance;
    tsGP_GreenPowerCustomData               *psGpCustomDataStructure;
    uint8                   				u8Status;
    uint8                                   i;
    bool_t                                  bZgpIdMatch = FALSE;

    /* Check pointers */
    if((puZgpdAddress == NULL) )
    {
        return FALSE;
    }
   //find GP cluster
   if((u8Status = eGP_FindGpCluster(
            u8GpEndPointId,
            TRUE,
            &psEndPointDefinition,
            &psClusterInstance,
            &psGpCustomDataStructure)) != E_ZCL_SUCCESS)
    {
        return FALSE;
    }

   // get EP mutex
   #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
   #endif
    /* get communication from attribute if not */
	if(eCommunicationMode == 0xff)
	{
		eZCL_ReadLocalAttributeValue(
		   psEndPointDefinition->u8EndPointNumber,
		   GREENPOWER_CLUSTER_ID,
		   TRUE,
		   FALSE,
		   FALSE,
		   E_CLD_GP_ATTR_ZGPS_COMMUNICATION_MODE,
		   &eCommunicationMode);
	}
    for(i = 0; i< GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES; i++)
    {

    	/* check if sink table entry is not empty */
        if(psGpCustomDataStructure->asZgpsSinkProxyTable[i].eGreenPowerSinkTablePriority != 0)
        {
            /* check if address is same */
        	if( bGP_CheckGPDAddressMatch(
        			((uint8)(psGpCustomDataStructure->asZgpsSinkProxyTable[i].b16Options & GP_APPLICATION_ID_MASK)),
        			u8ApplicationId,
        			&(psGpCustomDataStructure->asZgpsSinkProxyTable[i].uZgpdDeviceAddr),
        			puZgpdAddress
        			))
        	{
        		/* check if coomunication mode same */
        		/*if(eCommunicationMode == (psGpCustomDataStructure->asZgpsSinkProxyTable[i].b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK))*/
        		{
        			 bZgpIdMatch = TRUE;
        			 *psSinkTableEntry = &psGpCustomDataStructure->asZgpsSinkProxyTable[i];
        			 break;
        		}
        	}

        }
    }

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return bZgpIdMatch;
}

#endif
/****************************************************************************
 **
 ** NAME:       bGP_IsProxyTableEntryPresent
 **
 ** DESCRIPTION:
 ** Checks if Proxy Table entry present for the given GPD
 **
 ** PARAMETERS:                         Name                            Usage
 ** uint8                                  u8GpEndPointId,
 **  bool								   bIsServer,
 ** uint8                                  u8ApplicationId,
 ** tuGP_ZgpdDeviceAddr                    *puZgpdAddress,
 ** tsGP_ZgppProxySinkTable                **psProxySinkTableEntry
 **
 ** RETURN:
 ** TRUE if present, FALSE otherwise
 **
 ****************************************************************************/

PUBLIC bool_t bGP_IsProxyTableEntryPresent(
                    uint8                                  u8GpEndPointId,
                    bool_t								   bIsServer,
                    uint8                                  u8ApplicationId,
                    tuGP_ZgpdDeviceAddr                    *puZgpdAddress,
                    tsGP_ZgppProxySinkTable                **psProxySinkTableEntry)

{
    tsZCL_EndPointDefinition                *psEndPointDefinition;
    tsZCL_ClusterInstance                   *psClusterInstance;
    tsGP_GreenPowerCustomData               *psGpCustomDataStructure;
    uint8                   				u8Status;
    uint8                                   i;
    bool_t                                  bZgpIdMatch = FALSE;


    /* Check pointers */
    if((puZgpdAddress == NULL) )
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, "\nbIsSinkTableEntryPresent puZgpdAddress NULL\n");
        return FALSE;
    }
   //find GP cluster
   if((u8Status = eGP_FindGpCluster(
            u8GpEndPointId,
            bIsServer,
            &psEndPointDefinition,
            &psClusterInstance,
            &psGpCustomDataStructure)) != E_ZCL_SUCCESS)
    {
	   DBG_vPrintf(TRACE_GP_DEBUG, "\nbIsSinkTableEntryPresent eGP_FindGpCluster FALSE \n");
        return FALSE;
    }

   // get EP mutex
   #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
   #endif


    for(i = 0; i< GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES; i++)
    {   /* check if sink table entry is not empty */

    	if(psGpCustomDataStructure->asZgpsSinkProxyTable[i].bProxyTableEntryOccupied == TRUE)        {

            /* check if application id and address is same */
    		if( bGP_CheckGPDAddressMatch(((uint8)(psGpCustomDataStructure->asZgpsSinkProxyTable[i].b16Options & GP_APPLICATION_ID_MASK)),
    		        			u8ApplicationId,
    		        			&psGpCustomDataStructure->asZgpsSinkProxyTable[i].uZgpdDeviceAddr,
    		        			puZgpdAddress
    		        			))
    		{
				bZgpIdMatch = TRUE;
				*psProxySinkTableEntry = &psGpCustomDataStructure->asZgpsSinkProxyTable[i];
				break;
			}
		}

	}

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif
	if(bZgpIdMatch == FALSE)
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "\n Proxy table entry does not exist bZgpIdMatch FALSE \n");
	}
    return bZgpIdMatch;
}
/****************************************************************************
 **
 ** NAME:       bGP_GPDFValidityCheck
 **
 ** DESCRIPTION:
 ** Validates GPDF received for fields, security , freshness, length etc
 **
 ** PARAMETERS:                         Name                            Usage
 **  tsGP_ZgpDataIndication                         *psZgpDataIndication,
 **  tsZCL_EndPointDefinition                      *psEndPointDefinition,
 **  tsZCL_ClusterInstance                         *psClusterInstance,
 **  teGP_GreenPowerBufferedCommands               *pBufferedCmdId
 **
 ** RETURN:
 ** TRUE if valid, FALSE otherwise
 **
 ****************************************************************************/
bool_t bGP_GPDFValidityCheck(
		 tsGP_ZgpDataIndication                         *psZgpDataIndication,
		 tsZCL_EndPointDefinition                      *psEndPointDefinition,
		 tsZCL_ClusterInstance                         *psClusterInstance,
		 teGP_GreenPowerBufferedCommands               *pBufferedCmdId)
{
	bool_t bIsEntryPresent;
	tsGP_GreenPowerCustomData   *psGpCustomDataStructure;
	psGpCustomDataStructure = (tsGP_GreenPowerCustomData*)psClusterInstance->pvEndPointCustomStructPtr;
	tsGP_ZgppProxySinkTable                              *psZgpsSinkTable;


	/* check the validity of frame received : address fields, command id and RxAfterTx*/
	if((bGP_ValidateGPDF_Fields(psZgpDataIndication,psGpCustomDataStructure ) == FALSE))
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "bGP_ValidateGPDF_Fields failed: %d\n", psZgpDataIndication->u32SecFrameCounter);
		return FALSE;
	}

	/*  validate for freshness , packet length, and check if sink and proxy entries exist */
	if(FALSE ==  bValidatePacket(
			psEndPointDefinition,
			psGpCustomDataStructure,
			&bIsEntryPresent,
			(uint8	)psZgpDataIndication->u2ApplicationId,
			psZgpDataIndication->u32SecFrameCounter,
		    &psZgpsSinkTable,
		 &psZgpDataIndication->uZgpdDeviceAddr
	))
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "bValidatePacket failed: %d\n", psZgpDataIndication->u32SecFrameCounter);
		return FALSE;
	}
	/* Check Green Power Payload length */
	if(bValidatePacketLength(psEndPointDefinition, psZgpDataIndication, psGpCustomDataStructure ) == FALSE)
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "bValidatePacketLength failed: %d\n", psZgpDataIndication->u32SecFrameCounter);
		return FALSE;
	}
	/* Check whether command valid in the current mode  */
	if( bGP_ValidateCommandType(psZgpDataIndication->u8Status,
			pBufferedCmdId,
			psZgpDataIndication->bAutoCommissioning,
			psZgpDataIndication->u8CommandId,psGpCustomDataStructure) == FALSE)
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "bGP_ValidateCommandType failed: %d\n", psZgpDataIndication->u32SecFrameCounter);
		return FALSE;
	}

	if(psGpCustomDataStructure->eGreenPowerDeviceMode == E_GP_OPERATING_MODE)
	{
		if(bGP_SecurityValidation(psZgpDataIndication, psEndPointDefinition,psGpCustomDataStructure,psZgpsSinkTable) == FALSE)
		{
			DBG_vPrintf(TRACE_GP_DEBUG, "bGP_SecurityValidation failed: %d\n", psZgpDataIndication->u32SecFrameCounter);
			return FALSE;
		}
	}
#ifdef GP_COMBO_BASIC_DEVICE
	if((psGpCustomDataStructure->bIsCommissionReplySent == 0)
		&& (psZgpDataIndication->u8CommandId == E_GP_SUCCESS) )
	{
		if(bIsEntryPresent &&
		(psZgpsSinkTable->b8SinkOptions & GP_SINK_TABLE_RX_ON_MASK))
		{
			DBG_vPrintf(TRACE_GP_DEBUG, "psGpCustomDataStructure->bIsCommissionReplySent == 0\n");
			return FALSE;
		}
	}


#endif
	if((psZgpDataIndication->u8CommandId == E_GP_DECOMMISSIONING) ||(psZgpDataIndication->u8CommandId == E_GP_SUCCESS) ||
			((psZgpDataIndication->u8CommandId == E_GP_COMMISSIONING) && (psGpCustomDataStructure->eGreenPowerDeviceMode == E_GP_OPERATING_MODE))
					|| (psGpCustomDataStructure->eGreenPowerDeviceMode == E_GP_OPERATING_MODE))
	{
		uint8         u8KeyType, u8SecLevel;
		if((bIsEntryPresent == FALSE)&&((psZgpDataIndication->u8CommandId == E_GP_DECOMMISSIONING)|| (psZgpDataIndication->u8CommandId == E_GP_COMMISSIONING) ))
		{
			DBG_vPrintf(TRACE_GP_DEBUG, "bIsEntryPresent == FALSE for :E_GP_DECOMMISSIONING %d\n", psZgpDataIndication->u32SecFrameCounter);
			return FALSE;
		}
		if(bIsEntryPresent)
		{
			u8KeyType = (psZgpsSinkTable->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2;
			u8SecLevel = (psZgpsSinkTable->b8SecOptions & GP_SECURITY_LEVEL_MASK);

			if((psZgpDataIndication->u2SecurityKeyType == 1)&& (u8KeyType < E_GP_OUT_OF_THE_BOX_ZGPD_KEY))
			{
				DBG_vPrintf(TRACE_GP_DEBUG, "key type mismatch 1 %d %d \n", u8KeyType,psZgpDataIndication->u2SecurityKeyType );
				return FALSE;
			}
			/*else if((psZgpDataIndication->u2SecurityKeyType == 0)&& (u8KeyType == E_GP_OUT_OF_THE_BOX_ZGPD_KEY))
			{
				DBG_vPrintf(TRACE_GP_DEBUG, "key type mismatch 2 %d %d \n", u8KeyType,psZgpDataIndication->u2SecurityKeyType );
				return FALSE;
			}*/
			if(u8SecLevel != psZgpDataIndication->u2SecurityLevel)
			{
				DBG_vPrintf(TRACE_GP_DEBUG, "u8SecLevel == %d for :psZgpDataIndication->u2SecurityLevel %d\n",u8SecLevel,
						psZgpDataIndication->u2SecurityLevel);
				return FALSE;
			}
		}
	}
	/* Freshness check */
	if(bIsEntryPresent)
	{
		bool_t bIsFreshNessPass =  ( bGP_IsFreshPkt ( psEndPointDefinition->u8EndPointNumber,
									               ( uint8	) psZgpDataIndication->u2ApplicationId,
									               &psZgpDataIndication->uZgpdDeviceAddr,
									               psGpCustomDataStructure,
									               psZgpsSinkTable,
									               psZgpDataIndication->u32SecFrameCounter,
									               psZgpDataIndication->u2SecurityLevel ) );

		return bIsFreshNessPass;
	}


	return TRUE;
}
/****************************************************************************
 **
 ** NAME:       bGP_CheckGroupTable
 **
 ** DESCRIPTION:
 ** Check if given group id present in group table
 **
 ** PARAMETERS:               		  Name                      Usage
 ** uint16 u16GrpId
 **
 ** RETURN:
 **
 ** Index in sink table where group added
 ****************************************************************************/
bool_t bGP_CheckGroupTable(uint16 u16GrpId )
{
	ZPS_tsAplAib * tsAplAib  = ZPS_psAplAibGetAib();
	uint8 i,u8DstEndpoint, u8ByteOffset, u8BitOffset;
	/*for(j=0; j < tsAplAib->psAplApsmeGroupTable->u32SizeOfGroupTable;j++)
	{
		if( ( tsAplAib->psAplApsmeGroupTable->psAplApsmeGroupTableId[j].u16Groupid == u16GrpId) &&
				(tsAplAib->psAplApsmeGroupTable->psAplApsmeGroupTableId[j].u16Groupid == u16GrpId)) {
			DBG_vPrintf(TRUE, "GroupId Exist\n");
			return TRUE;
		}
	}
	DBG_vPrintf(TRUE, "GroupId not Exist\n");
	return FALSE;
*/


	for (i = 0; i < tsAplAib->psAplApsmeGroupTable->u32SizeOfGroupTable; i++)
	{

		if ((tsAplAib->psAplApsmeGroupTable->psAplApsmeGroupTableId[i].u16Groupid == u16GrpId) || (u16GrpId == 0xFFFF)){
			for (u8DstEndpoint = 1; u8DstEndpoint <=( 0xf2 - 1); u8DstEndpoint++)
			{
				 u8ByteOffset = (u8DstEndpoint - 1) / 8;
				 u8BitOffset  = (u8DstEndpoint- 1) % 8;

				//uint8 u8EndpointIdx = 0xf2 - 1; // group table array does not contain ep 0

				if ((tsAplAib->psAplApsmeGroupTable->psAplApsmeGroupTableId[i].au8Endpoint[u8ByteOffset] & (1 << u8BitOffset)) != 0)
				{
					DBG_vPrintf(TRACE_GP_DEBUG, "GroupId supported 0x%4x, EP= %d Exist\n",tsAplAib->psAplApsmeGroupTable->psAplApsmeGroupTableId[i].u16Groupid,
							u8DstEndpoint);
					return TRUE;
				}
			}

		}
	}
	return FALSE;
}
/****************************************************************************
 **
 ** NAME:       bGP_GiveCommIndAndUpdateTable
 **
 ** DESCRIPTION:
 ** Commission indication to upper layer
 **
 ** PARAMETERS:                    Name                           Usage
 ** ZPS_tsAfEvent              	  *pZPSevent              		ZPS event
 ** tsGP_GreenPowerCustomData     *psGpCustomDataStructure       Proxy table
 **
 ** RETURN:
 ** tsGP_ZgpCommissionIndication
 ****************************************************************************/
 bool_t bGP_GiveCommIndAndUpdateTable(
		 tsGP_ZgpDataIndication               		           *psZgpDataIndication,
		 tsZCL_EndPointDefinition    							*psEndPointDefinition,
		 tsGP_GreenPowerCustomData                              *psGpCustomDataStructure)
 {

	/* Give a call back to the user as Commission data indication event */
	/* Populate commission data indication event structure */
	tsGP_ZgpCommissionIndication                   sZgpCommissionIndication;
	sZgpCommissionIndication.eStatus = E_ZCL_SUCCESS; /* Assume Positive status */
	sZgpCommissionIndication.psGpToZclCommandInfo = NULL;
	sZgpCommissionIndication.bIsTunneledCmd = psZgpDataIndication->bTunneledPkt;
	sZgpCommissionIndication.b8AppId = 	(uint8)psZgpDataIndication->u2ApplicationId;

	DBG_vPrintf(TRACE_GP_DEBUG, "%s: b8AppId=%d CmdId=%d AutoCom=%d Status=%d\n", __FUNCTION__, sZgpCommissionIndication.b8AppId, psZgpDataIndication->u8CommandId, psZgpDataIndication->bAutoCommissioning, psZgpDataIndication->u8Status);

	/* Copy Application Id */
	if(sZgpCommissionIndication.b8AppId == GP_APPL_ID_4_BYTE)
	{
		sZgpCommissionIndication.uZgpdDeviceAddr.u32ZgpdSrcId =
				psZgpDataIndication->uZgpdDeviceAddr.u32ZgpdSrcId;
	}
#ifdef GP_IEEE_ADDR_SUPPORT
	else
	{
		sZgpCommissionIndication.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr =
				psZgpDataIndication->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
		sZgpCommissionIndication.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint =
				psZgpDataIndication->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
	}
#endif
#ifdef GP_COMBO_BASIC_DEVICE
	if(psZgpDataIndication->u8CommandId == E_GP_CHANNEL_REQUEST)
	{
		psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_RECEIVED_CHANNEL_REQUEST;
	    psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.bIsActAsTempMaster = TRUE;
		/* Give Application Callback  */
		psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
		return TRUE;
	}
	else if(psZgpDataIndication->bAutoCommissioning)
	{
		sZgpCommissionIndication.eCmdType = E_GP_DATA_CMD_AUTO_COMM;

		sZgpCommissionIndication.uCommands.sZgpDataCmd.eZgpdCmdId = psZgpDataIndication->u8CommandId;
		/* commissioning continues */
		sZgpCommissionIndication.uCommands.sZgpDataCmd.u8ZgpdCmdPayloadLength =
			psZgpDataIndication->u8PDUSize;
		sZgpCommissionIndication.uCommands.sZgpDataCmd.pu8ZgpdCmdPayload =
				psZgpDataIndication->pu8Pdu;


	}
	else if(psZgpDataIndication->u8CommandId == E_GP_SUCCESS)
	{
		if((psZgpDataIndication->u8Status == E_GP_SEC_COUNTER_FAILURE) || (psZgpDataIndication->u8Status == E_GP_SEC_AUTH_FAILURE) ||
				(psZgpDataIndication->u8Status == E_GP_SEC_UPROCESSED))
		{
			DBG_vPrintf(TRACE_GP_DEBUG, "\n Auth failed Success Cmd \n");
			return FALSE;
		}
		psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_SUCCESS_CMD_RCVD;
		/* Give Application Callback  */
		psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
		return TRUE;
	}
	else if(psZgpDataIndication->u8CommandId == E_GP_DECOMMISSIONING)
	{
		tsGP_ZgpDecommissionIndication                  sZgpDecommissionIndication;

		sZgpDecommissionIndication.uZgpdDeviceAddr  =              psZgpDataIndication->uZgpdDeviceAddr;
		sZgpDecommissionIndication.u8ApplicationId =(uint8)psZgpDataIndication->u2ApplicationId;
		psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psZgpDecommissionIndication =
				&sZgpDecommissionIndication;

		psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_DECOMM_CMD_RCVD;

		/* Give Application Callback  */
		psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
		return TRUE;
	}
	else if(psZgpDataIndication->u8CommandId == E_GP_COMMISSIONING)
	{
		sZgpCommissionIndication.eCmdType = E_GP_COMM_CMD;
		if( bGP_FillCommInd(&sZgpCommissionIndication, psZgpDataIndication,psEndPointDefinition ,psGpCustomDataStructure) == FALSE)
		{
			//vGP_ExitCommMode(psEndPointDefinition,psGpCustomDataStructure);
			 return FALSE;
		}

		if( bGP_ValidateComissionCmdFields(psZgpDataIndication,psEndPointDefinition,psGpCustomDataStructure)
				== FALSE)
		{
			DBG_vPrintf(TRACE_GP_DEBUG, "\n bGP_ValidateComissionCmdFields failed\n");
			return FALSE;
		}
	}
	else
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "\n Invalid command\n");
		return FALSE;
	}
	if((psZgpDataIndication->u8Status == E_GP_SEC_COUNTER_FAILURE) || (psZgpDataIndication->u8Status == E_GP_SEC_AUTH_FAILURE) ||
			(psZgpDataIndication->u8Status == E_GP_SEC_UPROCESSED))
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "\n Auth failed \n");
		return FALSE;
	}
	psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_COMMISSION_DATA_INDICATION;
	psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psZgpCommissionIndication =
			&sZgpCommissionIndication;

	/* Give Application Callback for functionality matching */
	psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
	if((psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.bIsActAsTempMaster) ||
			(psZgpDataIndication->bRxAfterTx == FALSE))
	{
		if(psGpCustomDataStructure->eGreenPowerDeviceMode != E_GP_OPERATING_MODE)
		{
			/* Update table */
			if(bGP_AddOrUpdateProxySinkTableEntries(psEndPointDefinition,psGpCustomDataStructure,psZgpDataIndication,&sZgpCommissionIndication)
					== FALSE)
			{
				// vGP_ExitCommMode(psEndPointDefinition,psGpCustomDataStructure);
				 return FALSE;
			}
		}
	}


#endif
	return TRUE;
 }

/****************************************************************************
 **
 ** NAME:       bGP_ValidateCommandType
 **
 ** DESCRIPTION:
 ** Checks if the received command is valid in this mode
 **
 ** PARAMETERS:                               Name                           Usage
 ** uint8                                     u8Status
 ** teGP_GreenPowerBufferedCommands          *pBufferedCmdId,
 ** bool_t                                    bAutoCommissioning,
 ** uint8                                     u8ZgpdCmdId,
 ** tsGP_GreenPowerCustomData                 *psGpCustomDataStructure
 **
 ** RETURN:
 ** TRUE or FALSE
 **
 ****************************************************************************/
bool_t bGP_ValidateCommandType(
		uint8                                          u8Status,
		teGP_GreenPowerBufferedCommands               *pBufferedCmdId,
		bool_t                                         bAutoCommissioning,
		uint8                                          u8ZgpdCmdId,
		tsGP_GreenPowerCustomData                      *psGpCustomDataStructure)
{

	if( (psGpCustomDataStructure->eGreenPowerDeviceMode != E_GP_OPERATING_MODE) &&bAutoCommissioning)
	{
		*pBufferedCmdId = E_GP_COMMISSIONING_CMDS;
		return TRUE;
	}
	/* If GP device is commission mode and rxed GP commands related to commission */
	/* When GP sends commission related commands, Auto Commission Flag shall be false */
	else if( (psGpCustomDataStructure->eGreenPowerDeviceMode!= E_GP_OPERATING_MODE) && (bAutoCommissioning == FALSE ))
	{
		/* DO NOT CHECK COMMAND IF  WE COULD NOT DECRYPT SUCCESSFULLY */
		if( ( ( ( u8ZgpdCmdId >= E_GP_COMMISSIONING ) && (u8ZgpdCmdId <= E_GP_COMMISSIONING_RANGE_END) ) ||
				( (u8ZgpdCmdId >= 0xB0 ) && (u8ZgpdCmdId <= 0xBF) )	) ||
				(u8Status == E_GP_SEC_UPROCESSED) || (u8Status == E_GP_SEC_AUTH_FAILURE))
		{
			   *pBufferedCmdId = E_GP_COMMISSIONING_CMDS;
				return TRUE;
		}
	}
	/* if device is in operating mode then auto commission flag does not matter,
	   packet would be treated as data packet */
	else if(psGpCustomDataStructure->eGreenPowerDeviceMode == E_GP_OPERATING_MODE)
	{
		if(!((u8ZgpdCmdId >= E_GP_COMMISSIONING ) && (u8ZgpdCmdId <= E_GP_COMMISSIONING_RANGE_END)))
		{
			    *pBufferedCmdId = E_GP_DATA;
				return TRUE;

		}
		else
		{
			/*  decommissioning command will be accepted in both  modes */
			if(u8ZgpdCmdId == E_GP_DECOMMISSIONING)
			{
			    *pBufferedCmdId = E_GP_COMMISSIONING_CMDS;
				return TRUE;
			}
			else if ((u8ZgpdCmdId == E_GP_COMMISSIONING ) && (u8Status == E_GP_SEC_SUCCESS))
			{
			    *pBufferedCmdId = E_GP_COMMISSIONING_CMDS;
				return TRUE;
			}
			else
			{
			DBG_vPrintf(TRACE_GP_DEBUG, "u8ZgpdCmdId = %d, E_GP_COMMISSIONING = %d, E_GP_CHANNEL_REQUEST = %d \n",
					u8ZgpdCmdId,E_GP_COMMISSIONING,E_GP_CHANNEL_REQUEST);
			}
		}
	}
	else
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "psGpCustomDataStructure->eGreenPowerDeviceMod failed: %d\n", psGpCustomDataStructure->eGreenPowerDeviceMode);
	}
	DBG_vPrintf(TRACE_GP_DEBUG, "psGpCustomDataStructure->eGreenPowerDeviceMod Failed: %d\n", psGpCustomDataStructure->eGreenPowerDeviceMode);
	return FALSE;
}
#ifdef GP_COMBO_BASIC_DEVICE
/****************************************************************************
 **
 ** NAME:       bIsCommandMapped
 **
 ** DESCRIPTION:
 ** Checks if the received command is mapped in translation table
 **
 ** PARAMETERS:                   Name                           Usage
 ** teGP_ZgpdCommandId                     eZgpdCommandId,
 ** uint8                					*pMatchedCmdIndex
 ** tsGP_TranslationTableEntry *psTranslationTableEntry
 **
 ** RETURN:
 ** TRUE or FALSE
 **
 ****************************************************************************/
PRIVATE bool_t bIsCommandMapped(teGP_ZgpdCommandId                     eZgpdCommandId,
							  uint8                					*pMatchedCmdIndex,
							  tsGP_TranslationTableEntry 			*psTranslationTableEntry)
{
	uint8 i;
	for(i = 0; i < psTranslationTableEntry->u8NoOfCmdInfo; i ++ )
	{
			if((psTranslationTableEntry->psGpToZclCmdInfo[i].eZgpdCommandId == eZgpdCommandId)&&
			  (psTranslationTableEntry->psGpToZclCmdInfo[i].u8EndpointId != 0xFD))
			{
				*pMatchedCmdIndex = i;
				return TRUE;
			}
		}
	return FALSE;

}


/****************************************************************************
 **
 ** NAME:       eGP_GPDFHandler
 **
 ** DESCRIPTION:
 ** Process received command through translation table
 **
 ** PARAMETERS:                   Name                           Usage
 ** ZPS_tsAfEvent                 *pZPSevent                     ZPS event
 ** tsZCL_EndPointDefinition      *psEndPointDefinition          Endpoint definition
 ** tsGP_GreenPowerCustomData     *psGpCustomDataStructure       Custom data
 ** uint8                         u8ApplicationId                Application ID
 ** tuGP_ZgpdDeviceAddr           *puZgpdAddress                 ZGP device address
 ** teGP_ZgpdCommandId            eZgpdCommandId                 command id
 ** uint8                         u8GpdCommandPayloadLength      GPD command length
 ** uint8                         *pu8GpdCommandPayload          GPD command payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eGP_GPDFHandler(
                    ZPS_tsAfEvent                          *pZPSevent,
                    tsZCL_EndPointDefinition               *psEndPointDefinition,
                    tsGP_GreenPowerCustomData              *psGpCustomDataStructure,
                    uint8                                  u8ApplicationId,
                    tuGP_ZgpdDeviceAddr                    *puZgpdAddress,
                    teGP_ZgpdCommandId                     eZgpdCommandId,
                    uint8                                  u8GpdCommandPayloadLength,
                    uint8                                  *pu8GpdCommandPayload)
{
    uint8 i, u8MatchedCmdIndex;
    bool_t bMatchSuccess = FALSE, bZgpIdMatch = FALSE, bPacketBuffered = FALSE;

    #ifdef STRICT_PARAM_CHECK
        /* Check pointers */
        if((psEndPointDefinition==NULL)  ||
           (psGpCustomDataStructure==NULL))
        {
        	DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_GPDFHandler E_ZCL_ERR_PARAMETER_NULL \n");
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    /* Traverse translation table */
    for(i = 0; i < GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES; i++)
    {
        /* check for translation pointer is populated or not */
        if(psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo != NULL)
        {
            /* check if address matches */
        	if(bGP_CheckGPDAddressMatch(
        			(psGpCustomDataStructure->psZgpsTranslationTableEntry[i].b8Options & (uint8)GP_APPLICATION_ID_MASK),
        			u8ApplicationId,
        			&psGpCustomDataStructure->psZgpsTranslationTableEntry[i].uZgpdDeviceAddr,
        			puZgpdAddress
        			))
        	{
        		DBG_vPrintf(TRACE_GP_DEBUG, "eGP_GPDFHandler Address Match ..*pu8GpdCommandPayload = %d\n", *pu8GpdCommandPayload);
        		 bZgpIdMatch = TRUE;

        	}
        }
		/* Check entry found in translation table or not */
		if(bZgpIdMatch)
		{
			bZgpIdMatch = FALSE;

			/*if((eZgpdCommandId == E_GP_ATTRIBUTE_REPORTING)&&
			   (psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo->eZgpdCommandId == E_GP_SENSOR_COMMAND))
			{
				DBG_vPrintf(TRACE_GP_DEBUG, "eGP_GPDFHandler E_GP_SENSOR_COMMAND ...\n");
				eZgpdCommandId = E_GP_SENSOR_COMMAND;
			}*/
			/* checking for matched GP command id */
			if(bIsCommandMapped(eZgpdCommandId, &u8MatchedCmdIndex, &psGpCustomDataStructure->psZgpsTranslationTableEntry[i]))
			{
				/* if once we already pushed the ZCL packet to stack for this GPD command then
				 * buffer the next match */
				if(bMatchSuccess)
				{
					bPacketBuffered = TRUE;

					eGP_BufferLoopBackPacket(
							psEndPointDefinition,
							psGpCustomDataStructure,
							i,
							u8ApplicationId,
							puZgpdAddress,
							eZgpdCommandId,
							u8GpdCommandPayloadLength,
							pu8GpdCommandPayload);
				}
				else
				{
					bMatchSuccess = TRUE;
					/* translate ZGP Command into ZCL command and send it to linked endpoint */
					eGP_TranslateCommandIntoZcl(
							psEndPointDefinition->u8EndPointNumber,
							i,
							u8MatchedCmdIndex,
							psGpCustomDataStructure,
							u8GpdCommandPayloadLength,
							pu8GpdCommandPayload);
				}
			}

		}
        if(bPacketBuffered)
            break;
	}

    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
                pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);

    /* Checking for entry found in translation table or not */
    if(bMatchSuccess)
    {
         /* Set Event Type */
        psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_ZGPD_COMMAND_RCVD;
        /* Give a call back event to app */
        psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
        return E_ZCL_SUCCESS;
    }
    else
    {     /* Set Event Type */
        psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_ZGPD_CMD_RCVD_WO_TRANS_ENTRY;
        /* Give a call back event to app */
        psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);

        return E_ZCL_FAIL;
    }
}

/****************************************************************************
 **
 ** NAME:       u16GP_GetStringSizeOfSinkTable
 **
 ** DESCRIPTION:
 ** Get Size of Sink Table
 **
 ** PARAMETERS:                     Name                            Usage
 ** uint8                           u8GreenPowerEndPointId          End Point id
 ** uint8						    *pu8NoOfSinkTableEntries
 ** RETURN:
 ** uint16
 **
 ****************************************************************************/
PUBLIC uint16 u16GP_GetStringSizeOfSinkTable(
		                    uint8                                       u8GreenPowerEndPointId,
		                    uint8										*pu8NoOfSinkTableEntries,
		                    tsGP_GreenPowerCustomData                   *psGpCustomData
		                    )
{
    tsZCL_EndPointDefinition                *psEndPointDefinition;
    tsZCL_ClusterInstance                   *psClusterInstance;
    tsGP_GreenPowerCustomData               *psGpCustomDataStructure;
    uint16                                  u16Length = 0, b16Options;
    uint8                                   i;
    *pu8NoOfSinkTableEntries = 0;
    if(psGpCustomData == NULL)
    {
		//find GP cluster
		if((eGP_FindGpCluster(  u8GreenPowerEndPointId,
								TRUE,
								&psEndPointDefinition,
								&psClusterInstance,
								&psGpCustomDataStructure)) != E_ZCL_SUCCESS)
		{
			return 0;
		}
    }
    else
    {
    	psGpCustomDataStructure =psGpCustomData;
    }

    for(i = 0; i< GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES; i++)
    {
        /* check for entry */
		if ((psGpCustomDataStructure->asZgpsSinkProxyTable[i].eGreenPowerSinkTablePriority != 0) && (psGpCustomDataStructure->asZgpsSinkProxyTable[i].u8GPDPaired == E_GP_PAIRED))
        {
        	(*pu8NoOfSinkTableEntries)++;
            u16Length += 2; //2 byte options field
            b16Options = psGpCustomDataStructure->asZgpsSinkProxyTable[i].b16Options;

            if((b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_4_BYTE)
            {
                u16Length += 4; //4 byte GPD ID
            }
            else
            {
                u16Length += 9; //8 byte GPD ID + 1 byte endpoint
            }

            u16Length += 1; //1 byte Device ID
            /* if communication mode is pre commissioned then take group list length also */
            if((psGpCustomDataStructure->asZgpsSinkProxyTable[i].b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK) ==
            		E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
            {
            	/* length sink group , assigned alias for each group */
                u16Length += (psGpCustomDataStructure->asZgpsSinkProxyTable[i].u8SinkGroupListEntries * 4) +1;
            }
            /* check if assigned address is present */
            if(psGpCustomDataStructure->asZgpsSinkProxyTable[i].b16Options & GP_PROXY_TABLE_ASSIGNED_ALIAS_MASK)
            {
                u16Length += 2; //2 byte GPD Assigned Alias
            }

            u16Length += 1; //1 byte Radius
            //if security options are available
            if(psGpCustomDataStructure->asZgpsSinkProxyTable[i].b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK)
            {
                teGP_GreenPowerSecLevel    eSecLevel;
              //  teGP_GreenPowerSecKeyType  eSecKeyType;
                u16Length += 1; //1 byte security options
                eSecLevel = psGpCustomDataStructure->asZgpsSinkProxyTable[i].b8SecOptions & GP_SECURITY_LEVEL_MASK;
              //  eSecKeyType = (psGpCustomDataStructure->asZgpsSinkProxyTable[i].b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2;
                //if security key is available
                if((eSecLevel != E_GP_NO_SECURITY))
                {
                    u16Length += 16; //16 byte security key
                }
            }
            /* check if sequence number capability or security is present*/
            if((psGpCustomDataStructure->asZgpsSinkProxyTable[i].b16Options & GP_PROXY_TABLE_SEQ_NUM_CAP_MASK)||
                        (psGpCustomDataStructure->asZgpsSinkProxyTable[i].b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK))
            {
                u16Length += 4; //4 byte security frame counter
            }
        }
    }
    //return calculated length
    return u16Length;
}

/****************************************************************************
 **
 ** NAME:       u16GP_ReadFromSinkTableToString
 **
 ** DESCRIPTION:
 ** Write sink table as string into passed data pointer
 **
 ** PARAMETERS:                      Name                           Usage
 ** uint8                         u8GreenPowerEndPointId        Green Power Endpoint id
 ** uint8                         *pu8Data                      data pointer to write
 ** uint16                        u16StringSize                 String size
 **
 ** RETURN:
 ** uint16
 **
 ****************************************************************************/
PUBLIC uint16 u16GP_ReadFromSinkTableToString(
                    uint8                                  u8GreenPowerEndPointId,
                    uint8                                  *pu8Data,
                    uint16                                 u16StringSize)
{
    tsZCL_EndPointDefinition                *psEndPointDefinition;
    tsZCL_ClusterInstance                   *psClusterInstance;
    tsGP_GreenPowerCustomData               *psGpCustomDataStructure;
    uint8                                   i;

    uint16 u16Len= 0;

    //find GP Cluster
    if((eGP_FindGpCluster(  u8GreenPowerEndPointId,
                            TRUE,
                            &psEndPointDefinition,
                            &psClusterInstance,
                            &psGpCustomDataStructure)) != E_ZCL_SUCCESS)
    {
        return 0;
    }
    /* write first 2 bytes as string length */
    *pu8Data++ = (uint8)u16StringSize;
    *pu8Data++ = (uint8)(u16StringSize >> 8);

    for(i = 0; i< GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES; i++)
    {
        /* check for entry */
		if ((psGpCustomDataStructure->asZgpsSinkProxyTable[i].eGreenPowerSinkTablePriority != 0) && (psGpCustomDataStructure->asZgpsSinkProxyTable[i].u8GPDPaired == E_GP_PAIRED))
        {
        	u16Len += u16GetSinkTableString(pu8Data, &psGpCustomDataStructure->asZgpsSinkProxyTable[i]);
        	pu8Data +=u16Len;
		}
    }

    return (u16Len + 2); // 2 added to account for 2 byte string length
}

#endif
/****************************************************************************
 **
 ** NAME:       vGP_RemoveGPDFromProxySinkTable
 **
 ** DESCRIPTION:
 ** removes GPD entry from sink table
 **
 ** PARAMETERS:                   Name                           Usage
 ** uint8                                u8EndPointNumber,
 ** uint8                                u8AppID,
 ** tuGP_ZgpdDeviceAddr                  *puZgpdDeviceAddr
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
void vGP_RemoveGPDFromProxySinkTable( uint8                                u8EndPointNumber,
		                           uint8                                 u8AppID,
		                           tuGP_ZgpdDeviceAddr                  *puZgpdDeviceAddr)
{
	 uint8 i;
	 uint8 u8Index = 0;
	 ZPS_tuAfZgpGreenPowerId uGreenPowerId;
	 ZPS_tsAfZgpGpstEntry *psAfZgpGpstEntry;
	 tsGP_ZgppProxySinkTable                              *psSinkProxyTableEntry;
	 for(i=0; i < GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES; i++)
	 {
			if(bGP_IsProxyTableEntryPresent(
					u8EndPointNumber,
		#ifdef GP_COMBO_BASIC_DEVICE
			     		TRUE,
		#else
			     		FALSE,
		#endif
			     		u8AppID,
			     		puZgpdDeviceAddr,
			     		&psSinkProxyTableEntry) == TRUE)
		    {
#ifdef GP_COMBO_BASIC_DEVICE
				psSinkProxyTableEntry->u8GPDPaired = E_GP_NOT_PAIRED;
				psSinkProxyTableEntry->eGreenPowerSinkTablePriority =0;
				psSinkProxyTableEntry->eZgpdDeviceId =0;
				psSinkProxyTableEntry->b8SinkOptions =0;
#endif
				psSinkProxyTableEntry->bProxyTableEntryOccupied =0;

				psSinkProxyTableEntry->u8SinkGroupListEntries =0;
				psSinkProxyTableEntry->u8GroupCastRadius =0;
				psSinkProxyTableEntry->u8SearchCounter =0;
				psSinkProxyTableEntry->u8NoOfUnicastSink =0;
				psSinkProxyTableEntry->b8SecOptions =0;
				psSinkProxyTableEntry->b16Options &= ~GP_APPLICATION_ID_MASK ;
				psSinkProxyTableEntry->u16ZgpdAssignedAlias =0;
				memset(&psSinkProxyTableEntry->sZgpdKey,0,sizeof(psSinkProxyTableEntry->sZgpdKey));
		    }
	 }
	 /* remove the security details in stack also*/
	 uGreenPowerId.u32SrcId =puZgpdDeviceAddr->u32ZgpdSrcId;
	 if(u8AppID == GP_APPL_ID_8_BYTE)
	 {
		 uGreenPowerId.u64Address  =  puZgpdDeviceAddr->sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
	 }
	 psAfZgpGpstEntry = ZPS_psZgpFindGpstEntry(u8AppID, uGreenPowerId, &u8Index);
	 if(psAfZgpGpstEntry != NULL)
	 {
		 memset(psAfZgpGpstEntry,0,sizeof(ZPS_tsAfZgpGpstEntry));
	 }
	 vGP_CallbackForPersistData();
}
/****************************************************************************
 **
 ** NAME:       bGP_GetFreeProxySinkTableEntry
 **
 ** DESCRIPTION:
 ** Get a free Proxy Table entry
 **
 ** PARAMETERS:                         Name                            Usage
 ** uint8                               u8GreenPowerEndPointId          Endpoint Id
 ** bool								bIsServer
 ** tsGP_ZgppProxySinkTable            *psSinkTableEntry               Pointer to Sink Table
 **
 ** RETURN:
 ** TRUE if free entry available, FALSE otherwise
 **
 ****************************************************************************/
PUBLIC bool_t bGP_GetFreeProxySinkTableEntry(
                    uint8                                  u8GreenPowerEndPointId,
                    bool_t									bIsServer,
                    tsGP_ZgppProxySinkTable                 **psProxySinkTableEntry)
{
    uint8                          i;
    tsZCL_EndPointDefinition       *psEndPointDefinition;
    tsZCL_ClusterInstance          *psClusterInstance;
    tsGP_GreenPowerCustomData      *psGpCustomDataStructure;
    uint8				            u8Status;
    *psProxySinkTableEntry = NULL;
    //find GP cluster
    if((u8Status = eGP_FindGpCluster(
            u8GreenPowerEndPointId,
            bIsServer,
            &psEndPointDefinition,
            &psClusterInstance,
            &psGpCustomDataStructure)) != E_ZCL_SUCCESS)
    {
        return FALSE;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    for(i = 0; i< GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES; i++)
    {
        /* check for empty entry */
        if(psGpCustomDataStructure->asZgpsSinkProxyTable[i].bProxyTableEntryOccupied == FALSE)
        {
        	*psProxySinkTableEntry = &psGpCustomDataStructure->asZgpsSinkProxyTable[i];

            // release mutex
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif
             memset(&psGpCustomDataStructure->asZgpsSinkProxyTable[i],0,sizeof(tsGP_ZgppProxySinkTable));
             psGpCustomDataStructure->asZgpsSinkProxyTable[i].bProxyTableEntryOccupied = TRUE;
            return TRUE;
        }
    }

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return FALSE;
}

/****************************************************************************
 **
 ** NAME:       u16GP_GetStringSizeOfProxyTable
 **
 ** DESCRIPTION:
 ** Get Size of Proxy Table
 **
 ** PARAMETERS:                     Name                            Usage
 ** uint8                           u8GreenPowerEndPointId          End Point id
 **
 ** RETURN:
 ** uint16
 **
 ****************************************************************************/
PUBLIC uint16 u16GP_GetStringSizeOfProxyTable(
		uint8                                       u8GreenPowerEndPointId,
		uint8										*pu8NoOfSinkTableEntries,
		tsGP_GreenPowerCustomData                   *psGpCustomData)
{

	uint8 u8NoOfValidEntries = 0;
    tsZCL_EndPointDefinition                *psEndPointDefinition;
    tsZCL_ClusterInstance                   *psClusterInstance;
    tsGP_GreenPowerCustomData               *psGpCustomDataStructure;
    uint16                                  u16Length = 0, b16Options;
    uint8                                   i;

    if(psGpCustomData == NULL)
    {
   		//find GP cluster
    	if((eGP_FindGpCluster(  u8GreenPowerEndPointId,
    	                            FALSE,
    	                            &psEndPointDefinition,
    	                            &psClusterInstance,
    	                            &psGpCustomDataStructure)) != E_ZCL_SUCCESS)
    	    {
			DBG_vPrintf(TRACE_GP_DEBUG, " \n u16GP_GetStringSizeOfProxyTable feGP_FindGpCluster failed \n");
    	        return 0;
    	    }
    }
    else
    {
       	psGpCustomDataStructure =psGpCustomData;
    }

    for(i = 0; i< GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES; i++)
    {
        /* check for entry */
        if(psGpCustomDataStructure->asZgpsSinkProxyTable[i].bProxyTableEntryOccupied == TRUE)
        {
        	u8NoOfValidEntries++;

            u16Length += 2; //2 byte options field
            b16Options = psGpCustomDataStructure->asZgpsSinkProxyTable[i].b16Options;

            if((b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_4_BYTE)
            {
                u16Length += 4; //4 byte GPD ID
            }
            else
            {
                u16Length += 9; //8 byte GPD ID, 1 byte endpoint id
            }

            /* check if assigned address is present */
            if(b16Options & GP_PROXY_TABLE_ASSIGNED_ALIAS_MASK)
            {
                u16Length += 2; //2 byte GPD Assigned Alias
            }

            /* if communication mode is pre commissioned then take group list length also */
            if(b16Options & GP_PROXY_TABLE_COMMISSION_GROUP_GPS_MASK)
            {
                u16Length += (psGpCustomDataStructure->asZgpsSinkProxyTable[i].u8SinkGroupListEntries * 4) +1;
            }
        	if(b16Options & GP_PROXY_TABLE_UNICAST_GPS_MASK)
        	{
        		 u16Length += (psGpCustomDataStructure->asZgpsSinkProxyTable[i].u8NoOfUnicastSink * 10) +1;
        	}
            u16Length += 1; //1 byte Radius
            //if security options are available
            if(b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK)
            {
                teGP_GreenPowerSecLevel    eSecLevel;
                teGP_GreenPowerSecKeyType  eSecKeyType;
                u16Length += 1; //1 byte security options
                eSecLevel = psGpCustomDataStructure->asZgpsSinkProxyTable[i].b8SecOptions & GP_SECURITY_LEVEL_MASK;
                eSecKeyType = (psGpCustomDataStructure->asZgpsSinkProxyTable[i].b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2;
                //if security key is available
                if((eSecLevel != E_GP_NO_SECURITY)&&(eSecKeyType != E_GP_DERIVED_INDIVIDUAL_ZGPD_KEY))
                {
                    u16Length += 16; //16 byte security key
                }
            }
            /* check if sequence number capability or security is present*/
            if((b16Options & GP_PROXY_TABLE_SEQ_NUM_CAP_MASK)||
                        (b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK))
            {
                u16Length += 4; //4 byte security frame counter
            }

            /* check if search counter is present*/
            if((!(b16Options & GP_PROXY_TABLE_ENTRY_ACTIVE_MASK))||
                        (!(b16Options & GP_PROXY_TABLE_ENTRY_VALID_MASK)))
            {
                u16Length += 1; //1 byte search counter
            }
        }
    }
    //return calculated length
    *pu8NoOfSinkTableEntries = u8NoOfValidEntries;
    return u16Length;
}

/****************************************************************************
 **
 ** NAME:       u16GP_ReadProxyTableToString
 **
 ** DESCRIPTION:
 ** Write proxy table as string into passed data pointer
 **
 ** PARAMETERS:                      Name                           Usage
 ** uint8                         u8GreenPowerEndPointId        Green Power Endpoint id
 ** uint8                         *pu8Data                      data pointer to write
 ** uint16                        u16StringSize                 String size
 **
 ** RETURN:
 ** uint16
 **
 ****************************************************************************/
PUBLIC uint16 u16GP_ReadProxyTableToString(
                    uint8                                  u8GreenPowerEndPointId,
                    uint8                                  *pu8Data,
                    uint16                                 u16StringSize)
{
    tsZCL_EndPointDefinition                *psEndPointDefinition;
    tsZCL_ClusterInstance                   *psClusterInstance;
    tsGP_GreenPowerCustomData               *psGpCustomDataStructure;
    uint8                                   i;
    uint16 u16Len = 0;

    //find GP Cluster
    if((eGP_FindGpCluster(  u8GreenPowerEndPointId,
                            FALSE,
                            &psEndPointDefinition,
                            &psClusterInstance,
                            &psGpCustomDataStructure)) != E_ZCL_SUCCESS)
    {
        return 0;
    }
    /* write first 2 bytes as string length */
   *pu8Data++ = (uint8)u16StringSize;
   *pu8Data++ = (u16StringSize >> 8);

   for(i = 0; i< GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES; i++)
   {
	   /* check for entry */
	   if(psGpCustomDataStructure->asZgpsSinkProxyTable[i].bProxyTableEntryOccupied == TRUE)
	   {
		   u16Len += u16GetProxyTableString(pu8Data, &psGpCustomDataStructure->asZgpsSinkProxyTable[i]);
		   pu8Data +=u16Len;
	   }
   }

   return u16Len +2;
}


/****************************************************************************
 **
 ** NAME:       bGP_IsFreshPkt
 **
 ** DESCRIPTION:
 ** Checking for fresh packet
 **
 ** PARAMETERS:                    Name                           Usage
 ** uint8                       u8EndPointId                  Source endpoint id
 ** uint8                       u8ApplicationId               Application id
 ** tuGP_ZgpdDeviceAddr         *puZgpdAddress                address of GPD
 ** tsGP_GreenPowerCustomData   *psGpCustomDataStructure      custom data structure
 ** tsGP_ZgppProxySinkTable          *psSinkTableEntry             sink table entry
 ** uint32                      u32SeqNoOrCounter             sequence number or frame counter
 ** RETURN:
 ** bool_t
 ****************************************************************************/
PUBLIC bool_t bGP_IsFreshPkt(
                    uint8                                  u8EndPointId,
                    uint8                                  u8ApplicationId,
                    tuGP_ZgpdDeviceAddr                    *puZgpdAddress,
                    tsGP_GreenPowerCustomData              *psGpCustomDataStructure,
                    tsGP_ZgppProxySinkTable                     *psTableEntry,
                    uint32                                 u32SeqNoOrCounter,
					uint8                                  u8SecurityLevel )
{


    /* If the GPD command used SecurityLevel 0b10 or 0b11, then the filtering of duplicate messages is performed based on the
     * GPD security  frame counter, stored in the Proxy/Sink Table entry for this GPD */

    if(psTableEntry->b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK)
	{

		if((u32SeqNoOrCounter == 0)&&(psTableEntry->u32ZgpdSecFrameCounter == 0))
		{
			return TRUE;
		}
		else if((uint32)u32SeqNoOrCounter <= (uint32)psTableEntry->u32ZgpdSecFrameCounter)
		{
			DBG_vPrintf(TRACE_GP_DEBUG, " \n bGP_IsFreshPkt fAIL, RECVD COUNTER = 0x%8x, EXISTING =  0x%8x \n",u32SeqNoOrCounter,
					psTableEntry->u32ZgpdSecFrameCounter);

			return FALSE;
		}
    }
	/* updates the Security frame counter field of the Sink Table entry */
    if( ( psTableEntry->b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK) &&
    	  ( ( u8SecurityLevel & 0x3 ) != 0 ) )
    {
	    psTableEntry->u32ZgpdSecFrameCounter = u32SeqNoOrCounter;
	    vGP_CallbackForPersistData();
    }
    else if ( ( (psTableEntry->b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK) == 0) &&
    		( ( u8SecurityLevel & 0x3 ) == 0 ) )
    {
    	 psTableEntry->u32ZgpdSecFrameCounter = u32SeqNoOrCounter;
    	 vGP_CallbackForPersistData();
    }

    return TRUE;
}

/****************************************************************************
 **
 ** NAME:       eGp_BufferTransmissionPacket
 **
 ** DESCRIPTION:
 ** Buffer PDU Info for Delay Transmission
 **
 ** PARAMETERS:                                  Name                        Usage
 ** tsGP_ZgpDataIndication                  *psZgpDataIndication           pointer of Data indication structure
 ** teGP_GreenPowerBufferedCommands         eGreenPowerBufferedCommand     enum of command to buffer
 ** tsGP_GreenPowerCustomData               *psGreenPowerCustomData        Custom Data Structure
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eGp_BufferTransmissionPacket(
                    tsGP_ZgpDataIndication              *psZgpDataIndication,
                    teGP_GreenPowerBufferedCommands     eGreenPowerBufferedCommand,
                    tsGP_GreenPowerCustomData           *psGreenPowerCustomData)
{
    tsGP_ZgpBufferedApduRecord          *psZgpBufferedApduRecord;
    uint8                               u8Delay = 0;
    uint8 								u8Dmin = 5;


    /*
    Dmin =
    	if the triggering GPDF had RxAfterTx = 0b0: Dmin_u = 5 ms;
    	if the triggering GPDF had RxAfterTx = 0b1: Dmin_b = 32 ms;*/
    DBG_vPrintf(TRACE_GP_DEBUG, " \n eGp_BufferTransmissionPacket eGreenPowerBufferedCommand=  %d payload = %d\n",eGreenPowerBufferedCommand,psZgpDataIndication->pu8Pdu[0] );
    if(psZgpDataIndication->bRxAfterTx)
    {
    	u8Dmin = 32;
    }
    u8Delay = u8Dmin + u8GP_GetProxyDelay((uint8)psZgpDataIndication->u8LinkQuality);


    /* Allocate buffer for Saving PDU */
    psZgpBufferedApduRecord = (tsGP_ZgpBufferedApduRecord *)psDLISTgetHead(&psGreenPowerCustomData->lGpDeAllocList);

    /* check allocated pointer */
    if(psZgpBufferedApduRecord)
    {
        /* Copy Buffered PDU info which will be used for scheduling transmission */
    	if (E_GP_RESPONSE == eGreenPowerBufferedCommand)
    	{
    		psZgpBufferedApduRecord->sBufferedApduInfo.u16Delay = 5;
    	}
    	if (E_GP_ADD_MSG_TO_TX_QUEUE_AFTER_DELAY == eGreenPowerBufferedCommand)
    	{
    		psZgpBufferedApduRecord->sBufferedApduInfo.u16Delay = 400; //msec
    	}
    	else if (E_GP_ADD_MSG_TO_TX_QUEUE == eGreenPowerBufferedCommand)
    	{
    		psZgpBufferedApduRecord->sBufferedApduInfo.u16Delay = 5000; //5sec
    	}
    	else if(E_GP_DEV_ANCE == eGreenPowerBufferedCommand)
    	{
    		psZgpBufferedApduRecord->sBufferedApduInfo.u16Delay = 1;
    	}
    	else
    	{
    		psZgpBufferedApduRecord->sBufferedApduInfo.u16Delay = u8Delay;

    	}
        psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand = eGreenPowerBufferedCommand;
        psZgpBufferedApduRecord->sBufferedApduInfo.u8Status = psZgpDataIndication->u8Status;
        psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId = psZgpDataIndication->u8CommandId;
        psZgpBufferedApduRecord->sBufferedApduInfo.u8LinkQuality = psZgpDataIndication->u8LinkQuality;
        psZgpBufferedApduRecord->sBufferedApduInfo.u8RSSI = psZgpDataIndication->u8RSSI;
        psZgpBufferedApduRecord->sBufferedApduInfo.u2SecurityKeyType = psZgpDataIndication->u2SecurityKeyType;
        psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId = psZgpDataIndication->u2ApplicationId;
        psZgpBufferedApduRecord->sBufferedApduInfo.u2SecurityLevel = psZgpDataIndication->u2SecurityLevel;
        psZgpBufferedApduRecord->sBufferedApduInfo.bAutoCommissioning = psZgpDataIndication->bAutoCommissioning;
        psZgpBufferedApduRecord->sBufferedApduInfo.bRxAfterTx = psZgpDataIndication->bRxAfterTx;
        psZgpBufferedApduRecord->sBufferedApduInfo.u32Mic = psZgpDataIndication->u32Mic;
        psZgpBufferedApduRecord->sBufferedApduInfo.u32SecFrameCounter = psZgpDataIndication->u32SecFrameCounter;
        psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr = psZgpDataIndication->uZgpdDeviceAddr;
        psZgpBufferedApduRecord->sBufferedApduInfo.u8PDUSize = psZgpDataIndication->u8PDUSize;
        psZgpBufferedApduRecord->sBufferedApduInfo.u8SeqNum = psZgpDataIndication->u8SeqNum;
        psZgpBufferedApduRecord->sBufferedApduInfo.u8TranslationIndex = psZgpDataIndication->u8TranslationIndex;
        psZgpBufferedApduRecord->sBufferedApduInfo.u16NwkShortAddr = psZgpDataIndication->u16NwkShortAddr;
        psZgpBufferedApduRecord->sBufferedApduInfo.bTunneledPkt = psZgpDataIndication->bTunneledPkt;
        psZgpBufferedApduRecord->sBufferedApduInfo.bFrameType = psZgpDataIndication->bFrameType;

        if(psZgpDataIndication->bTunneledPkt)
        {
            psZgpBufferedApduRecord->sBufferedApduInfo.u8GPP_GPDLink = psZgpDataIndication->u8GPP_GPDLink;
        }
        else
        {
            psZgpBufferedApduRecord->sBufferedApduInfo.u8GPP_GPDLink = u8GP_GetProxyDistance( psZgpBufferedApduRecord->sBufferedApduInfo.u8LinkQuality ,
            		psZgpBufferedApduRecord->sBufferedApduInfo.u8RSSI  );
        }


        /* Copy Data only if pu8PDU != NULL */
        if(psZgpDataIndication->pu8Pdu != NULL)
        {
            /* Copy Payload */
            memcpy(psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU, psZgpDataIndication->pu8Pdu, psZgpDataIndication->u8PDUSize);
        }

        /* Remove from the dealloc list */
        psDLISTremove(&psGreenPowerCustomData->lGpDeAllocList,
                (DNODE *)psZgpBufferedApduRecord);

        /* Add to the Alloc list as tail */
        vDLISTaddToTail(&psGreenPowerCustomData->lGpAllocList,
                            (DNODE *)psZgpBufferedApduRecord);
        DBG_vPrintf(TRACE_GP_DEBUG, "eGp_BufferTransmissionPacket delay = %d\n",
                psZgpBufferedApduRecord->sBufferedApduInfo.u16Delay);
    }
    else
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, "eGp_BufferTransmissionPacket E_ZCL_FAIL\n");
        return E_ZCL_FAIL;

    }

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       bEncryptDecryptKey
 **
 ** DESCRIPTION:
 ** Decrypts the key received in commissioning command
 **
 ** PARAMETERS:                    Name                           Usage
 ** bool_t											 bEncrypt,
 ** uint8                                             u8AppId,
 ** uint32                                            u32SrcId,
 ** uint64 											u64IeeeAddress,
 ** tsGP_ZgpBufferedApduRecord                      *psZgpBufferedApduRecord,
 ** uint8											*pu8DataEncyptDecrypt,
 ** uint8											*pu8DataOut,
 ** uint8											*u8MIC,
 ** tsZCL_EndPointDefinition    					*psEndPointDefinition,
 ** tsGP_GreenPowerCustomData                       *psGpCustomDataStructure
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
bool_t bEncryptDecryptKey(
		bool_t											 bEncrypt,
		uint8                                             u8AppId,
		tuGP_ZgpdDeviceAddr                              *puGPDAddress,
		uint32                                           u32FrameCounterIfOutGoing,
		uint8											*pu8DataEncyptDecrypt,
		uint8											*pu8DataOut,
		uint8											*u8MIC,
		tsZCL_EndPointDefinition    					*psEndPointDefinition,
		tsGP_GreenPowerCustomData                       *psGpCustomDataStructure
		)
{
#ifdef CLD_GP_ATTR_ZGP_LINK_KEY
	uint32 u32SrcId;
	uint64 u64IeeeAddress;
	uint32 u32FrameCounter;
	AESSW_Block_u uNonce,   uLinkKey ;
	bool_t bIsSsever = FALSE;

	uint8 u8Header[4]/*, u8KeyMic[4]*/;
#ifndef PC_PLATFORM_BUILD
	bool_t bVerified;
	uint8 i;
#endif
#ifdef 	GP_COMBO_BASIC_DEVICE
	bIsSsever = TRUE;
#endif
	// get EP mutex
#ifndef COOPERATIVE
   eZCL_GetMutex(psEndPointDefinition);
#endif

	/* Get Green Power communication Mode attribute value */
	eZCL_ReadLocalAttributeValue(
			psEndPointDefinition->u8EndPointNumber,
			GREENPOWER_CLUSTER_ID,
			bIsSsever,
			FALSE,
			!bIsSsever,
			E_CLD_GP_ATTR_ZGP_LINK_KEY,
			(uint8*)&uLinkKey.au8);


	// release mutex
#ifndef COOPERATIVE
	  eZCL_ReleaseMutex(psEndPointDefinition);
#endif
	  if(u8AppId)
	  {
		  u64IeeeAddress = puGPDAddress->sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
		  u32SrcId = (uint32)u64IeeeAddress;
	  }
	  else
	  {
		  u32SrcId = puGPDAddress->u32ZgpdSrcId;
	  }

	u8Header[0] = (uint8)u32SrcId;
	u8Header[1] = (uint8)(u32SrcId >> 8);
	u8Header[2] = (uint8)(u32SrcId >> 16);
	u8Header[3] = (uint8)(u32SrcId >> 24);

	if(u8AppId)
	{
		uNonce.au8[1] = (uint8)(u64IeeeAddress);
		uNonce.au8[2] = (uint8)((u64IeeeAddress) >> 8);
		uNonce.au8[3] = (uint8)((u64IeeeAddress) >> 16);
		uNonce.au8[4] = (uint8)((u64IeeeAddress) >> 24);
		uNonce.au8[5] = (uint8)((u64IeeeAddress) >> 32);
		uNonce.au8[6] = (uint8)((u64IeeeAddress) >> 40);
		uNonce.au8[7] = (uint8)((u64IeeeAddress) >> 48);
		uNonce.au8[8] = (uint8)((u64IeeeAddress) >> 56);
		if(bEncrypt == FALSE)
		{
			uNonce.au8[13] = 0x05;
		}
		else
		{
			uNonce.au8[13] = 0xC5;
		}
	}
	else
	{
		memset(uNonce.au8,0,sizeof(uNonce.au8));
		uNonce.au8[5] =(uint8)u32SrcId;
		uNonce.au8[6] =(uint8)(u32SrcId >> 8);
		uNonce.au8[7] =(uint8)(u32SrcId >> 16);
		uNonce.au8[8] = (uint8)(u32SrcId >> 24);
		if(bEncrypt == FALSE)
		{
			memcpy(&uNonce.au8[1], &uNonce.au8[5], 4);
		}
		uNonce.au8[13] = 0x05;
	}


	if(bEncrypt == TRUE)
	{
		u32FrameCounter = u32FrameCounterIfOutGoing ;
	}
	else
	{
	   u32FrameCounter =  u32SrcId;
	}
	uNonce.au8[9] = (uint8)u32FrameCounter;
	uNonce.au8[10] = (uint8)(u32FrameCounter >> 8);
	uNonce.au8[11] = (uint8)(u32FrameCounter >> 16);
	uNonce.au8[12] = (uint8)(u32FrameCounter >> 24);


#ifndef PC_PLATFORM_BUILD
	DBG_vPrintf(TRACE_GP_DEBUG, " \n Printing Data ") ;
	for(i=0 ; i< 16; i++)
	{
		DBG_vPrintf(TRACE_GP_DEBUG, " 0x%02x ", pu8DataEncyptDecrypt[i]);
	}
	DBG_vPrintf(TRACE_GP_DEBUG, " \n Printing nonce ") ;
	for(i=0 ; i< 16; i++)
	{
		DBG_vPrintf(TRACE_GP_DEBUG, " 0x%02x ", uNonce.au8[i]);
	}
	DBG_vPrintf(TRACE_GP_DEBUG, " \n Printing Header ") ;
	for(i=0 ; i< sizeof(u8Header); i++)
	{
		DBG_vPrintf(TRACE_GP_DEBUG, " 0x%02x ", u8Header[i]);
	}
	DBG_vPrintf(TRACE_GP_DEBUG, " \n ") ;

#if (defined LITTLE_ENDIAN_PROCESSOR) && (defined JENNIC_CHIP_FAMILY_JN517x)
	tsReg128 sKey;
	memcpy(&sKey, &uLinkKey.au8[0], 16);
	vSwipeEndian(&uLinkKey, &sKey, FALSE);
#endif

	memcpy(pu8DataOut, pu8DataEncyptDecrypt, 16);
	if (!bACI_WriteKey((tsReg128*)&uLinkKey))
	    return FALSE;
    vACI_OptimisedCcmStar(bEncrypt,				/* TRUE=Encrypt / FALSE=Decrypt */
    	 	 	 	 	  4,					/* Required number of checksum bytes */
						  4,					/* Length of authentication data in bytes */
						  16,					/* Length of input data in bytes */
						  &uNonce,				/* A pointer to the 128bit nonce data */
						  u8Header,				/* Authentication data */
						  pu8DataOut,			/* Input and output data  */
						  u8MIC,   				/* Checksum (MIC)   */
						  &bVerified);			/* Result of checksum verify if in decode mode */

	DBG_vPrintf(TRACE_GP_DEBUG, "\n Output :");
	for(i=0;i<16;i++)
		DBG_vPrintf(TRACE_GP_DEBUG, " 0x%02x,", pu8DataOut[i]);
	DBG_vPrintf(TRACE_GP_DEBUG, "\n LinkKey  :");
	for(i=0;i<16;i++)
		DBG_vPrintf(TRACE_GP_DEBUG, " 0x%02x,", uLinkKey.au8[i]);
	DBG_vPrintf(TRACE_GP_DEBUG, " \n");

	if(bEncrypt == FALSE)
	{
		if(bVerified == FALSE)
		{
			DBG_vPrintf(TRACE_GP_DEBUG|1, "Decrypted key verified %d",bVerified);
			/* Give a callback to user saying that security processing failed */
			psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_SECURITY_PROCESSING_FAILED;
			psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);

			/* If security processing failed, then drop the packet */
			return FALSE;
		}
	}
#endif

	return TRUE;
#else
	/* Give a callback to user saying that link key not enabled */
	psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_LINK_KEY_IS_NOT_ENABLED;
	psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);

	 /* drop the packet */
	 // release mutex

	return FALSE;
#endif
}

/****************************************************************************
 **
 ** NAME:       vCreateDataIndFromBufferedCmd
 **
 ** DESCRIPTION:
 ** Create Data indication from buffered command
 **
 ** PARAMETERS:                         Name                            Usage
 ** tsGP_ZgpBufferedApduRecord     psZgpBufferedApduRecord       The buffered record
 ** tsGP_ZgpDataIndication        *psZgpDataIndication           Data indication
 **
 **  RETURN:
 ** void
 **
 ****************************************************************************/
void vCreateDataIndFromBufferedCmd(
		tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
		tsGP_ZgpDataIndication                       *psZgpDataIndication)
{

	psZgpDataIndication->u2ApplicationId = psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId;
	psZgpDataIndication->u8Status = psZgpBufferedApduRecord->sBufferedApduInfo.u8Status;
	psZgpDataIndication->uZgpdDeviceAddr = psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr;
	psZgpDataIndication->bAutoCommissioning = psZgpBufferedApduRecord->sBufferedApduInfo.bAutoCommissioning;


	psZgpDataIndication->u32SecFrameCounter = psZgpBufferedApduRecord->sBufferedApduInfo.u32SecFrameCounter;
	psZgpDataIndication->u8CommandId = psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId;
	psZgpDataIndication->bTunneledPkt = psZgpBufferedApduRecord->sBufferedApduInfo.bTunneledPkt;
	psZgpDataIndication->bFrameType = psZgpBufferedApduRecord->sBufferedApduInfo.bFrameType;
	psZgpDataIndication->bRxAfterTx =  psZgpBufferedApduRecord->sBufferedApduInfo.bRxAfterTx;
	psZgpDataIndication->u2SecurityLevel = psZgpBufferedApduRecord->sBufferedApduInfo.u2SecurityLevel;
	psZgpDataIndication->u2SecurityKeyType = psZgpBufferedApduRecord->sBufferedApduInfo.u2SecurityKeyType;
	psZgpDataIndication->u32Mic=  psZgpBufferedApduRecord->sBufferedApduInfo.u32Mic;
	psZgpDataIndication->u8PDUSize = psZgpBufferedApduRecord->sBufferedApduInfo.u8PDUSize;
	psZgpDataIndication->pu8Pdu = psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU;
	psZgpDataIndication->u8SeqNum = psZgpBufferedApduRecord->sBufferedApduInfo.u8SeqNum ;
	psZgpDataIndication->u16NwkShortAddr = psZgpBufferedApduRecord->sBufferedApduInfo.u16NwkShortAddr;
	psZgpDataIndication->u8TranslationIndex = psZgpBufferedApduRecord->sBufferedApduInfo.u8TranslationIndex;
	psZgpDataIndication->u8GPP_GPDLink = psZgpBufferedApduRecord->sBufferedApduInfo.u8GPP_GPDLink ;
	psZgpDataIndication->u8RSSI = psZgpBufferedApduRecord->sBufferedApduInfo.u8RSSI ;
	psZgpDataIndication->u8LinkQuality = psZgpBufferedApduRecord->sBufferedApduInfo.u8LinkQuality;
	DBG_vPrintf(TRACE_GP_DEBUG, "psZgpDataIndication->u32Micc  = 0x%4x\n",psZgpDataIndication->u32Mic );
}

/****************************************************************************
 **
 ** NAME:       vGp_TransmissionTimerCallback
 **
 ** DESCRIPTION:
 ** Transmitting Buffered Packets for Broadcast
 **
 ** PARAMETERS:                         Name                            Usage
 ** uint8                           u8SrcEndPointId                 Source Endpoint Id
 ** tsZCL_EndPointDefinition        *psEndPointDefinition           End Point defintion
 ** tsGP_GreenPowerCustomData       *psGpCustomDataStructure         Custom Data Structure
 ** RETURN:
 ** void
 **
 ****************************************************************************/

PUBLIC void vGp_TransmissionTimerCallback(
                    uint8                                  u8SrcEndPointId,
                    tsZCL_EndPointDefinition               *psEndPointDefinition,
                    tsGP_GreenPowerCustomData              *psGpCustomDataStructure)
{


    tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord;
    bool_t                                        bIsHeadNode = FALSE;

    /* Get Head Pointer of Alloc List */
    psZgpBufferedApduRecord = (tsGP_ZgpBufferedApduRecord *)psDLISTgetHead(&psGpCustomDataStructure->lGpAllocList);

    /* Check Pointer */
    while(psZgpBufferedApduRecord)
    {
        /* Decrement delay, transmit packet if delay reached zero */
        if(psZgpBufferedApduRecord->sBufferedApduInfo.u16Delay)
        {
            psZgpBufferedApduRecord->sBufferedApduInfo.u16Delay--;
        }

        if(psZgpBufferedApduRecord->sBufferedApduInfo.u16Delay == 0x00)
        {

#ifdef GP_COMBO_BASIC_DEVICE
            if(E_GP_LOOP_BACK == psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand)
            {
            	/* Transmit loop back packets */
            	DBG_vPrintf(TRACE_GP_DEBUG, "\n E_GP_LOOP_BACK \n");
            	vGP_TxLoopBackCmds(u8SrcEndPointId,psZgpBufferedApduRecord,psGpCustomDataStructure);
            }
            else
            {
#else
            {
#endif
            	if(E_GP_DEV_ANCE == psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand)
				{
					/* Transmit loop back packets */

					DBG_vPrintf(TRACE_GP_DEBUG, "\n E_GP_DEV_ANCE \n");
					if(psZgpBufferedApduRecord->sBufferedApduInfo.u8Status == 0)
					{
						vGP_SendDeviceAnnounce(psZgpBufferedApduRecord->sBufferedApduInfo.u16NwkShortAddr, 0xFFFFFFFFFFFFFFFFULL);
					}
					else
					{
						vGP_SendDeviceAnnounce(psZgpBufferedApduRecord->sBufferedApduInfo.u16NwkShortAddr, ZPS_u64AplZdoGetIeeeAddr());
					}

				}
            	else if(E_GP_ADD_MSG_TO_TX_QUEUE == psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand)
				{
					/* Transmit loop back packets */
					DBG_vPrintf(TRACE_GP_DEBUG, "\n E_GP_ADD_MSG_TO_TX_QUEUE \n");
					if(psZgpBufferedApduRecord->sBufferedApduInfo.u8Status == E_ZCL_SUCCESS)
					{
						if (psGpCustomDataStructure->u16TransmitChannelTimeout)
						{
							psGpCustomDataStructure->u16TransmitChannelTimeout = 0x00;
							ZPS_vNwkNibSetChannel(ZPS_pvAplZdoGetNwkHandle(),
									psGpCustomDataStructure->u8OperationalChannel);
						}

						vSendToTxQueue(psZgpBufferedApduRecord,
							psGpCustomDataStructure);
					}
					else
					{
						DBG_vPrintf(TRACE_GP_DEBUG, "\n E_GP_ADD_MSG_TO_TX_QUEUE removed \n");
					}
				}
            	else if(E_GP_ADD_MSG_TO_TX_QUEUE_AFTER_DELAY== psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand)
            	{

					DBG_vPrintf(TRACE_GP_DEBUG, "\n E_GP_ADD_MSG_TO_TX_QUEUE_AFTER_DELAY \n");
					/* check if we need to move to Transmit channel.u8Status indicates whether a channel change required
					 * and  u8TranslationIndex holds the transit channel */
					if(psZgpBufferedApduRecord->sBufferedApduInfo.u8Status != E_ZCL_SUCCESS)
					{

						DBG_vPrintf(TRACE_GP_DEBUG, "\n changing channel to %d \n", psZgpBufferedApduRecord->sBufferedApduInfo.u8TranslationIndex);

						psGpCustomDataStructure->u8OperationalChannel = ZPS_u8AplZdoGetRadioChannel();
						ZPS_vNwkNibSetChannel( ZPS_pvAplZdoGetNwkHandle(), psZgpBufferedApduRecord->sBufferedApduInfo.u8TranslationIndex);
						psGpCustomDataStructure->u16TransmitChannelTimeout = 250; //5 sec timeout

					}
					vSendToTxQueue(psZgpBufferedApduRecord,
							psGpCustomDataStructure);
            	}

            	else
            	{

					DBG_vPrintf(TRACE_GP_DEBUG, "\n calling  vGP_HandleGPDCommand = %d \n", psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand);
					vGP_HandleGPDCommand(psZgpBufferedApduRecord, psEndPointDefinition,psGpCustomDataStructure);
            	}

            }
			/* if it is a loop back command then remove from the allocated list only when it traversed for all translation table entry */
			if(((E_GP_LOOP_BACK == psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand)&&
			  (GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES == psZgpBufferedApduRecord->sBufferedApduInfo.u8TranslationIndex))||
			  (E_GP_LOOP_BACK != psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand))
			{
				tsGP_ZgpBufferedApduRecord *psTempZgpBufferedApduRecord =
												(tsGP_ZgpBufferedApduRecord *)psDLISTgetHead(&psGpCustomDataStructure->lGpAllocList);

				/* Check, removed node is head of the list? */
				if(psZgpBufferedApduRecord == psTempZgpBufferedApduRecord)
				{
					bIsHeadNode = TRUE;
				}

				/* Before Transmission delete from alloc list and add to the dealloc list */
				psDLISTremove(&psGpCustomDataStructure->lGpAllocList,
						(DNODE *)psZgpBufferedApduRecord);

				// add to free list
				vDLISTaddToTail(&psGpCustomDataStructure->lGpDeAllocList,
						(DNODE *)psZgpBufferedApduRecord);


			}

			/* After device announce, everything should be sequential, buffer respective packets for broadcast transmission */
			if(psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand == E_GP_DEVICE_ANNOUNCE)
			{
				tsGP_ZgpDataIndication   sZgpDataIndication;

				vCreateDataIndFromBufferedCmd(psZgpBufferedApduRecord,&sZgpDataIndication);

				/* Send Pairing Information as Broadcast, Buffer the packet as it is broadcast transmission */
				eGp_BufferTransmissionPacket(
									&sZgpDataIndication,
									E_GP_ADD_PAIRING,
									psGpCustomDataStructure);


			}
			else if (( psGpCustomDataStructure->bCommissionExitModeOnFirstPairSuccess) &&
					((psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand == E_GP_ADD_PAIRING) ||
					(psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand == E_GP_REMOVE_PAIRING)))
			{
				/* Only devices are in pairing mode, GP pairing commands would send Commission exit command*/
				/* So exit the commission mode and send exit mode command */
				tsGP_ZgpDataIndication   sZgpDataIndication ;
				sZgpDataIndication.pu8Pdu = NULL; /* Pairing command transmitted using Sink Table information */
                sZgpDataIndication.u8LinkQuality = 0;
				sZgpDataIndication.bRxAfterTx  = FALSE;
				  /* Send Pairing Information as Broadcast, Buffer the packet as it is broadcast transmission */
				eGp_BufferTransmissionPacket(
									  &sZgpDataIndication,
									  E_GP_PROXY_COMMISION,
									  psGpCustomDataStructure);

			}

        }
        if(bIsHeadNode)
        {
            /* Get Head Pointer of Alloc List */
            psZgpBufferedApduRecord = (tsGP_ZgpBufferedApduRecord *)psDLISTgetHead(&psGpCustomDataStructure->lGpAllocList);
            bIsHeadNode = FALSE;
        }
        else
        {
            /* get the next pointer */
            psZgpBufferedApduRecord = (tsGP_ZgpBufferedApduRecord *)psDLISTgetNext((DNODE *)psZgpBufferedApduRecord);
        }
    }
}

/****************************************************************************
 **
 ** NAME:       u8GP_GetProxyDistance
 **
 ** DESCRIPTION:
 ** Calculation proxy distance based on link quality
 **
 ** PARAMETERS:                      Name                           Usage
 ** uint8                           u8LinkQuality                   Link Quality of received packet
 **
 ** RETURN:
 ** int8
 **
 ****************************************************************************/
PUBLIC uint8 u8GP_GetProxyDistance(
		uint8                              u8LinkQuality ,
		uint8								u8RSSI)
{
	uint8 u8ProxyDistance = 0;
	uint8	u8LQI;
      /* check Link Quality */
    u8LQI =u8GP_GetLQIQuality(u8LinkQuality);

    u8ProxyDistance = u8LQI << LINK_QUALITY_BIT_POS;
    u8ProxyDistance |= u8RSSI;
    return u8ProxyDistance;
}

/****************************************************************************
 **
 ** NAME:       u8GP_GetProxyDelay
 **
 ** DESCRIPTION:
 ** Calculation tunneling delay based on link quality
 **
 ** PARAMETERS:                      Name                           Usage
 ** uint8                           u8LinkQuality                   Link Quality of received packet
 **
 ** RETURN:
 ** int8
 **
 ****************************************************************************/
PUBLIC uint8 u8GP_GetProxyDelay(uint8 u8LinkQuality)
{
/* QualityBasedDelay is calculated as follows
 *  For Link quality = 0b11: 0 ms;
	For Link quality = 0b10: 32ms;
	For Link quality = 0b01: 64ms;
	For Link quality = 0b00: 96ms;
 * */

    uint8 u8Delay = u8GP_GetLQIQuality(u8LinkQuality)  ;
    u8Delay = (LQI_EXCELLENT - u8Delay)* QUALITY_BASED_DELAY_FACTOR ;

    return u8Delay;
}

/****************************************************************************
 **
 ** NAME:       u8GP_GetLQIQuality
 **
 ** DESCRIPTION:
 ** Determines the quality of link quality
 **
 ** PARAMETERS:                      Name                           Usage
 ** uint8                           u8LinkQuality                   Link Quality of received packet
 **
 ** RETURN:
 ** uint8
 ** 0b00	Poor
 ** 0b01	Moderate
 ** 0b10	High
 ** 0b11	Excellent
 **
 **
 ****************************************************************************/

uint8 u8GP_GetLQIQuality(uint8 u8LinkQuality)
{
    /* Link quality excellent > 200, Link quality High = between 150 to 200
    * Moderate = between 100 to 200, Poor = < 100 */
	uint8 LQIVal = 0;

	/* check Link Quality */
	if(u8LinkQuality <= 100)
	{
		LQIVal = LQI_POOR;
	}
	else if(u8LinkQuality > 100 && u8LinkQuality <= 150)
	{
		LQIVal = LQI_MODERATE;
	}
	else if(u8LinkQuality > 150 && u8LinkQuality <= 200)
	{
		LQIVal = LQI_HIGH ;
	}
	else if(u8LinkQuality > 200)
	{
		LQIVal = LQI_EXCELLENT;
	}

	return LQIVal;
}
#ifdef GP_COMBO_BASIC_DEVICE
/****************************************************************************
 **
 ** NAME:       bGP_IsZgpdCommandSupported
 **
 ** DESCRIPTION:
 ** check if ZGPD command is supported or not
 **
 ** PARAMETERS:                         Name                            Usage
    teGP_ZgpdCommandId           eZgpdCommandId                      Zgpd Command id
 ** RETURN:
 ** bool_t
 **
 ****************************************************************************/
PUBLIC bool_t bGP_IsZgpdCommandSupported(teGP_ZgpdCommandId   eZgpdCommandId, uint8 u8ZbCommandId, uint16 u16ClusterId)
{

    if((u8ZbCommandId == E_CLD_IDENTIFY_CMD_IDENTIFY)&&
       (u16ClusterId == GP_GENERAL_CLUSTER_ID_IDENTIFY))
    {
       return TRUE;
    }


   //case E_GP_LEVEL_CONTROL_STOP:
    else if((((u8ZbCommandId == E_CLD_ONOFF_CMD_OFF)||
           (u8ZbCommandId == E_CLD_ONOFF_CMD_ON)||
           (u8ZbCommandId == E_CLD_ONOFF_CMD_TOGGLE))&&
           (u16ClusterId == GP_GENERAL_CLUSTER_ID_ONOFF))||
           ((u8ZbCommandId == E_CLD_LEVELCONTROL_CMD_STOP)&&
            (u16ClusterId == GP_GENERAL_CLUSTER_ID_LEVEL_CONTROL)))
    {
        return TRUE;
    }
    else
    {
        switch(eZgpdCommandId)
        {

            case E_GP_MOVE_UP_WITH_ON_OFF:
            case E_GP_MOVE_DOWN_WITH_ON_OFF:
            case E_GP_STEP_UP_WITH_ON_OFF:
            case E_GP_STEP_DOWN_WITH_ON_OFF:
            case E_GP_COMMISSIONING:
            case E_GP_DECOMMISSIONING:
            case E_GP_SUCCESS:
            case E_GP_CHANNEL_REQUEST:
            case E_GP_COMMISSIONING_REPLY:
            case E_GP_CHANNEL_CONFIGURATION:
                return TRUE;
            default:
                return FALSE;
        }
    }
}

/****************************************************************************
 **
 ** NAME:       vGP_RemoveGPEPFromGroup
 **
 ** DESCRIPTION:
 ** Remove GPEP from group at APS level
 **
 ** PARAMETERS:                      Name                           Usage
 ** tsGP_GreenPowerCustomData      *psGpCustomDataStructure
 ** tsZCL_EndPointDefinition       *psEndPointDefinition
 ** tsGP_ZgppProxySinkTable             *psSinkTableEntry
 **
 ** RETURN:
 ** none
 **
 ****************************************************************************/
PUBLIC void vGP_RemoveGPEPFromGroup(tsGP_GreenPowerCustomData   *psGpCustomDataStructure,
                                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                                    tsGP_ZgppProxySinkTable          *psSinkTableEntry)
{
    teZCL_Status   eStatus = E_ZCL_SUCCESS;

    if((psSinkTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)
    {
        /* remove from derived group Id */
        uint16  u16DerivedGroupId;
        u16DerivedGroupId = u16GP_DeriveAliasSrcAddr(
                                 (uint8)psSinkTableEntry->b16Options & GP_APPLICATION_ID_MASK,
                                 psSinkTableEntry->uZgpdDeviceAddr);

        eStatus = ZPS_eAplZdoGroupEndpointRemove(u16DerivedGroupId, ZCL_GP_PROXY_ENDPOINT_ID);
        /* Check Status */
        if(eStatus != E_ZCL_SUCCESS)
        {
            psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_REMOVING_GROUP_TABLE_FAIL;
            psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.eRemoveGroupTableStatus = eStatus;
           /* Give Application Callback for group table remove failure */
            psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
        }
    }
    else if((psSinkTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
	{
		/* Find No. of pre-commissioned groups are presented in the list */
		uint8   u8Count;

		for(u8Count = 0; u8Count < psSinkTableEntry->u8SinkGroupListEntries; u8Count++)
		{
			eStatus = ZPS_eAplZdoGroupEndpointRemove(psSinkTableEntry->asSinkGroupList[u8Count].u16SinkGroup, ZCL_GP_PROXY_ENDPOINT_ID);
			/* Check Status */
			 if(eStatus != E_ZCL_SUCCESS)
			 {
				 psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_REMOVING_GROUP_TABLE_FAIL;
				 psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.eRemoveGroupTableStatus = eStatus;
				/* Give Application Callback for sink table update */
				 psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
			}
		}
	}

}
/****************************************************************************
 **
 ** NAME:       vGP_AddGPEPToGroup
 **
 ** DESCRIPTION:
 ** Add GPEP to group at APS level
 **
 ** PARAMETERS:                      Name                           Usage
 ** tsGP_GreenPowerCustomData      *psGpCustomDataStructure
 ** tsZCL_EndPointDefinition       *psEndPointDefinition
 ** tsGP_ZgppProxySinkTable             *psSinkTableEntry
 **
 ** RETURN:
 ** none
 **
 ****************************************************************************/
PUBLIC void vGP_AddGPEPToGroup(     tsGP_GreenPowerCustomData   *psGpCustomDataStructure,
                                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                                    tsGP_ZgppProxySinkTable          *psSinkTableEntry)
{
    teZCL_Status   eStatus = E_ZCL_SUCCESS;

    if((psSinkTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)
    {
        /* remove from derived group Id */
        uint16  u16DerivedGroupId;
        u16DerivedGroupId = u16GP_DeriveAliasSrcAddr(
                                 (uint8)psSinkTableEntry->b16Options & GP_APPLICATION_ID_MASK,
                                 psSinkTableEntry->uZgpdDeviceAddr);

        eStatus = ZPS_eAplZdoGroupEndpointAdd(u16DerivedGroupId, ZCL_GP_PROXY_ENDPOINT_ID);

        /* Check Status */
        if(eStatus != E_ZCL_SUCCESS)
        {
            psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_ADDING_GROUP_TABLE_FAIL;
            psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.eAddGroupTableStatus = eStatus;
           /* Give Application Callback for group table remove failure */
            psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
        }
    }
    else if((psSinkTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
	{
		/* Find No. of pre-commissioned groups are presented in the list */
		uint8   u8Count;

		for(u8Count = 0; u8Count < psSinkTableEntry->u8SinkGroupListEntries; u8Count++)
		{
			eStatus = ZPS_eAplZdoGroupEndpointAdd(psSinkTableEntry->asSinkGroupList[u8Count].u16SinkGroup, ZCL_GP_PROXY_ENDPOINT_ID);
			/* Check Status */
			 if(eStatus != E_ZCL_SUCCESS)
			 {
				 psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_ADDING_GROUP_TABLE_FAIL;
				 psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.eAddGroupTableStatus = eStatus;
				/* Give Application Callback for sink table update */
				 psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
			}
		}
	}

}

/****************************************************************************
 **
 ** NAME:       eGP_TranslateCommandIntoZcl
 **
 ** DESCRIPTION:
 ** Translate GPD command into ZCL command
 **
 ** PARAMETERS:                      Name                           Usage
 ** uint8                         u8MatchedIndex                Matched index of table
 ** tsGP_GreenPowerCustomData     *psGpCustomDataStructure      Custom data
 ** uint8                         u8GpdCommandPayloadLength     command length
 ** uint8                         *pu8GpdCommandPayload         GPD comamnd payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eGP_TranslateCommandIntoZcl(
                    uint8                                  u8GpEndPointId,
                    uint8                                  u8MatchedIndex,
                    uint8                                  u8MatchedCmdIndex,
                    tsGP_GreenPowerCustomData              *psGpCustomDataStructure,
                    uint8                                  u8GpdCommandPayloadLength,
                    uint8                                  *pu8GpdCommandPayload)
{
    uint8  u8DstEndpoint;
    teZCL_Status    eStatus = E_ZCL_SUCCESS;
    uint16          u16ClusterId;
    tsZCL_CallBackEvent sZCLcallBackEvent;
    ZPS_tsAfEvent sAFEvent;
    uint16 u16Pos = 0;
    uint8  u8ZbCmd, u8GPCmdId;
    uint8 u8FrameControl = 0x11;
    sZCLcallBackEvent.eEventType = E_ZCL_CBET_ZIGBEE_EVENT;
    sZCLcallBackEvent.u8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
    sZCLcallBackEvent.u8EndPoint =ZCL_GP_PROXY_ENDPOINT_ID;
    sZCLcallBackEvent.eZCL_Status =E_ZCL_SUCCESS;
    sZCLcallBackEvent.pZPSevent = &sAFEvent;
    sAFEvent.eType =ZPS_EVENT_APS_DATA_INDICATION;
    sAFEvent.uEvent.sApsDataIndEvent.u8DstAddrMode = ZPS_E_ADDR_MODE_SHORT;
    sAFEvent.uEvent.sApsDataIndEvent.uDstAddress.u16Addr = ZPS_u16AplZdoGetNwkAddr();
    u8DstEndpoint = psGpCustomDataStructure->psZgpsTranslationTableEntry[u8MatchedIndex].psGpToZclCmdInfo[u8MatchedCmdIndex].u8EndpointId;
   // u8GPCmdId = psGpCustomDataStructure->psZgpsTranslationTableEntry[u8MatchedIndex].psGpToZclCmdInfo[u8MatchedCmdIndex].eZgpdCommandId;
    u8ZbCmd= psGpCustomDataStructure->psZgpsTranslationTableEntry[u8MatchedIndex].psGpToZclCmdInfo[u8MatchedCmdIndex].u8ZbCommandId;
    u8GPCmdId = psGpCustomDataStructure->psZgpsTranslationTableEntry[u8MatchedIndex].psGpToZclCmdInfo[u8MatchedCmdIndex].eZgpdCommandId;
	/* if endpoint id is 0xFE in translation table then use endpoint id 0xFF */
	if(0xFE == psGpCustomDataStructure->psZgpsTranslationTableEntry[u8MatchedIndex].psGpToZclCmdInfo[u8MatchedCmdIndex].u8EndpointId)
	{
		u8DstEndpoint = 0xFF;
	}
	if(psGpCustomDataStructure->psZgpsTranslationTableEntry[u8MatchedIndex].psGpToZclCmdInfo[u8MatchedCmdIndex].u16ZbClusterId == 0xFFFF)
	{
		u16ClusterId = u16GP_GetClusterId(psGpCustomDataStructure->psZgpsTranslationTableEntry[u8MatchedIndex].psGpToZclCmdInfo[u8MatchedCmdIndex].eZgpdCommandId);
	}
	else
	{
		u16ClusterId = psGpCustomDataStructure->psZgpsTranslationTableEntry[u8MatchedIndex].psGpToZclCmdInfo[u8MatchedCmdIndex].u16ZbClusterId;
	}
    sAFEvent.uEvent.sApsDataIndEvent.u8DstEndpoint = u8DstEndpoint;
    sAFEvent.uEvent.sApsDataIndEvent.u8SrcAddrMode = ZPS_E_ADDR_MODE_SHORT;
    sAFEvent.uEvent.sApsDataIndEvent.uSrcAddress.u16Addr = ZPS_u16AplZdoGetNwkAddr();
    sAFEvent.uEvent.sApsDataIndEvent.u8SrcEndpoint = ZCL_GP_PROXY_ENDPOINT_ID;
    sAFEvent.uEvent.sApsDataIndEvent.u16ProfileId = 0xA1E0; // Green power profile id
    sAFEvent.uEvent.sApsDataIndEvent.u16ClusterId = u16ClusterId;
    sAFEvent.uEvent.sApsDataIndEvent.eStatus = E_ZCL_SUCCESS;
    sAFEvent.uEvent.sApsDataIndEvent.eSecurityStatus = ZPS_APL_APS_E_SECURED_NWK_KEY;
    sAFEvent.uEvent.sApsDataIndEvent.hAPduInst = hZCL_AllocateAPduInstance();


	if(sAFEvent.uEvent.sApsDataIndEvent.hAPduInst == PDUM_INVALID_HANDLE)
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "  No Buffer \n ");
		return(E_ZCL_ERR_ZBUFFER_FAIL);
	}

	u16Pos += u16ZCL_APduInstanceWriteNBO(sAFEvent.uEvent.sApsDataIndEvent.hAPduInst, u16Pos, E_ZCL_UINT8, &u8FrameControl);
	u16Pos += u16ZCL_APduInstanceWriteNBO(sAFEvent.uEvent.sApsDataIndEvent.hAPduInst, u16Pos, E_ZCL_UINT8, &sZCLcallBackEvent.u8TransactionSequenceNumber);
	u16Pos += u16ZCL_APduInstanceWriteNBO(sAFEvent.uEvent.sApsDataIndEvent.hAPduInst, u16Pos, E_ZCL_UINT8,
              &psGpCustomDataStructure->psZgpsTranslationTableEntry[u8MatchedIndex].psGpToZclCmdInfo[u8MatchedCmdIndex].u8ZbCommandId);


	DBG_vPrintf(TRACE_GP_DEBUG, " u8ZbCmd = %d \n ", u8ZbCmd);


    //case E_GP_IDENTIFY:
    if((u8ZbCmd == E_CLD_IDENTIFY_CMD_IDENTIFY) && (u16ClusterId == GENERAL_CLUSTER_ID_IDENTIFY))
    {
    	uint16 u16IdentifyTime = 60;

    	u16Pos += u16ZCL_APduInstanceWriteNBO(sAFEvent.uEvent.sApsDataIndEvent.hAPduInst, u16Pos, E_ZCL_UINT16, &u16IdentifyTime);


    }
    else if(((u8ZbCmd == E_CLD_ONOFF_CMD_OFF) && (u16ClusterId == GENERAL_CLUSTER_ID_ONOFF))||
            ((u8ZbCmd == E_CLD_ONOFF_CMD_ON)&& (u16ClusterId == GENERAL_CLUSTER_ID_ONOFF))||
            ((u8ZbCmd == E_CLD_ONOFF_CMD_TOGGLE)&& (u16ClusterId == GENERAL_CLUSTER_ID_ONOFF)) ||
            ((u8ZbCmd == E_CLD_LEVELCONTROL_CMD_STOP)&& (u16ClusterId == GENERAL_CLUSTER_ID_LEVEL_CONTROL)))
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, " GP_GENERAL_CLUSTER_ID_LEVEL_CONTROL1 u16Pos = %d \n ", u16Pos);
    }
    else
    {
    	uint8 u8Mode = 0;
    	if((u8GPCmdId == E_GP_MOVE_DOWN) || (u8GPCmdId == E_GP_STEP_DOWN)||(E_GP_MOVE_DOWN_WITH_ON_OFF ==u8GPCmdId ) || (E_GP_STEP_DOWN_WITH_ON_OFF ==u8GPCmdId))
    	{
    		 u8Mode = 1;
    	}

    	if(u16ClusterId == GENERAL_CLUSTER_ID_LEVEL_CONTROL)
    	{
			switch(u8ZbCmd)
			{
			/*case E_GP_MOVE_DOWN_WITH_ON_OFF:
			case E_GP_MOVE_UP_WITH_ON_OFF:
			case E_GP_MOVE_DOWN:
			case E_GP_MOVE_UP: */
			case E_CLD_LEVELCONTROL_CMD_MOVE:
			case E_CLD_LEVELCONTROL_CMD_MOVE_WITH_ON_OFF:
				/*if((E_GP_MOVE_UP == u8ZbCmd ) ||
				(E_GP_MOVE_UP_WITH_ON_OFF == u8ZbCmd))
				{
					*pu8Start++ = 0x01; //u8MoveMode
					u8Mode = 0;

				}*/
				u16Pos += u16ZCL_APduInstanceWriteNBO(sAFEvent.uEvent.sApsDataIndEvent.hAPduInst, u16Pos, E_ZCL_UINT8, &u8Mode);
				u16Pos += u16ZCL_APduInstanceWriteNBO(sAFEvent.uEvent.sApsDataIndEvent.hAPduInst, u16Pos, E_ZCL_UINT8, pu8GpdCommandPayload);


				break;

			/*case E_GP_STEP_DOWN_WITH_ON_OFF:
			case E_GP_STEP_UP_WITH_ON_OFF:
			case E_GP_STEP_DOWN:
			case E_GP_STEP_UP:*/
			case E_CLD_LEVELCONTROL_CMD_STEP:
			case E_CLD_LEVELCONTROL_CMD_STEP_WITH_ON_OFF:
				/*if((E_GP_STEP_UP == u8ZbCmd ) ||
				  (E_GP_STEP_UP_WITH_ON_OFF == u8ZbCmd))
				{
					u8Mode = 0;
				}*/

				u16Pos += u16ZCL_APduInstanceWriteNBO(sAFEvent.uEvent.sApsDataIndEvent.hAPduInst, u16Pos, E_ZCL_UINT8, &u8Mode);
				/* if optional fields are present in GPD command then copy in step command payload */
				if(u8GpdCommandPayloadLength != 0)
				{

					u16Pos += u16ZCL_APduInstanceWriteNBO(sAFEvent.uEvent.sApsDataIndEvent.hAPduInst, u16Pos, E_ZCL_UINT8, pu8GpdCommandPayload++);
					if(u8GpdCommandPayloadLength == 3)
					{
						u16Pos += u16ZCL_APduInstanceWriteNBO(sAFEvent.uEvent.sApsDataIndEvent.hAPduInst, u16Pos, E_ZCL_UINT8, pu8GpdCommandPayload++);
						u16Pos += u16ZCL_APduInstanceWriteNBO(sAFEvent.uEvent.sApsDataIndEvent.hAPduInst, u16Pos, E_ZCL_UINT8, pu8GpdCommandPayload++);
					}
				}

				break;
			default:
				 DBG_vPrintf(TRACE_GP_DEBUG, " default u16Pos = %d \n ", u16Pos);
				eStatus = E_ZCL_FAIL;
			}
    	}
    }
    if(eStatus == E_ZCL_SUCCESS)
    {
    	PDUM_eAPduInstanceSetPayloadSize(sAFEvent.uEvent.sApsDataIndEvent.hAPduInst, u16Pos);
    	vZCL_EventHandler(&sZCLcallBackEvent);
    }
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       u16GP_GetClusterId
 **
 ** DESCRIPTION:
 ** Return back cluster id based on GPD command id
 **
 ** PARAMETERS:                         Name                            Usage
    teGP_ZgpdCommandId           eZgpdCommandId                      Zgpd Command id
 ** RETURN:
 ** uint16
 **
 ****************************************************************************/
PUBLIC uint16 u16GP_GetClusterId(teGP_ZgpdCommandId   eZgpdCommandId)
{
    uint8 i = 0;
    /* search ZGP command cluster table for ZGPD command id */
    while(asZgpCommadClusterTable[i].eZgpdCommandId != E_GP_ZGPD_CMD_ID_ENUM_END)
    {
        /* if ZGPD command id is present then return linked cluster id */
        if(asZgpCommadClusterTable[i].eZgpdCommandId == eZgpdCommandId)
        {
            return(asZgpCommadClusterTable[i].u16ClusterId);
        }
        i++;
    }
    /* if ZGPD command id is not present in ZGP command cluster table return invalid cluster id */
    return 0xFFFF;
}
/****************************************************************************
 **
 ** NAME:       eGP_BufferLoopBackPacket
 **
 ** DESCRIPTION:
 ** buffer loopback packet if there is need to push same packet multiple times
 **
 ** PARAMETERS:                         Name                            Usage
 ** tsZCL_EndPointDefinition      *psEndPointDefinition          Endpoint definition
 ** tsGP_GreenPowerCustomData     *psGpCustomDataStructure       Custom data
 ** uint8                         u8TranslationIndex             translation table index
 ** uint8                         u8ApplicationId                Application ID
 ** tuGP_ZgpdDeviceAddr           *puZgpdAddress                 ZGP device address
 ** teGP_ZgpdCommandId            eZgpdCommandId                 command id
 ** uint8                         u8GpdCommandPayloadLength      GPD command length
 ** uint8                         *pu8GpdCommandPayload          GPD command payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eGP_BufferLoopBackPacket(
                    tsZCL_EndPointDefinition               *psEndPointDefinition,
                    tsGP_GreenPowerCustomData              *psGpCustomDataStructure,
                    uint8                                  u8TranslationIndex,
                    uint8                                  u8ApplicationId,
                    tuGP_ZgpdDeviceAddr                    *puZgpdAddress,
                    teGP_ZgpdCommandId                     eZgpdCommandId,
                    uint8                                  u8GpdCommandPayloadLength,
                    uint8                                  *pu8GpdCommandPayload)
{

    tsGP_ZgpDataIndication                          sZgpDataIndication = {0};
     //uint8                                          au8Pdu[GP_MAX_ZB_CMD_PAYLOAD_LENGTH];
    /* Copy payload length */
    sZgpDataIndication.u8PDUSize = u8GpdCommandPayloadLength;

    /* Check Green Power Payload length */
    if(sZgpDataIndication.u8PDUSize > GP_MAX_ZB_CMD_PAYLOAD_LENGTH)
    {
        /* Inform application unsupported payload length as we are buffering packet for delayed transmission */
        psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_CMD_UNSUPPORTED_PAYLOAD_LENGTH;

        /* Give Application Callback  */
        psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
        return E_ZCL_SUCCESS;
    }

    sZgpDataIndication.u8CommandId = eZgpdCommandId;
    sZgpDataIndication.u2ApplicationId = u8ApplicationId;
    sZgpDataIndication.uZgpdDeviceAddr = *puZgpdAddress;
    /* Copy Payload */
    //sZgpDataIndication.pu8Pdu = au8Pdu;
    sZgpDataIndication.pu8Pdu = pu8GpdCommandPayload;
    //memcpy(sZgpDataIndication.pu8Pdu,pu8GpdCommandPayload,u8GpdCommandPayloadLength);

    sZgpDataIndication.u8TranslationIndex = u8TranslationIndex;
    /* push this packet to stack after some delay */
    return eGp_BufferTransmissionPacket(
                        &sZgpDataIndication,
                        E_GP_LOOP_BACK,
                        psGpCustomDataStructure);
}

/****************************************************************************
 **
 ** NAME:       eGP_Table
 **
 ** DESCRIPTION:
 ** Update sink table information for ZGPD
 **
 ** PARAMETERS:                         Name                            Usage
 ** tsZCL_EndPointDefinition        *psEndPointDefinition,
 ** tsGP_GreenPowerCustomData       *psGpCustomDataStructure,
 ** tsGP_ZgppProxySinkTable         *psZgpsSinkTable,
 ** tsGP_ZgpDataIndication          *psZgpDataIndication,
 ** tsGP_ZgpCommissionIndication    *psZgpCommissionIndication
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eGP_UpdateSinkTable(
                                        tsZCL_EndPointDefinition        *psEndPointDefinition,
                                        tsGP_GreenPowerCustomData       *psGpCustomDataStructure,
                                        tsGP_ZgppProxySinkTable         *psZgpsSinkTable,
                               		    tsGP_ZgpDataIndication          *psZgpDataIndication,
                               		    tsGP_ZgpCommissionIndication    *psZgpCommissionIndication)
{

    uint8                       u8ZgpsCommModeAttr;

	/* Get Green Power communication Mode attribute value */
	eZCL_ReadLocalAttributeValue(
	   psEndPointDefinition->u8EndPointNumber,
	   GREENPOWER_CLUSTER_ID,
	   TRUE,
	   FALSE,
	   FALSE,
	   E_CLD_GP_ATTR_ZGPS_COMMUNICATION_MODE,
	   &u8ZgpsCommModeAttr);


    /* Set Communication Mode based on sink communication mode attribute value */
    /* If default communication mode is pre-commissioned,
       assumed that sink table is populated before device put in commission mode
       Group list would be populated by CT */
    psZgpsSinkTable->b8SinkOptions  = u8ZgpsCommModeAttr ;
    if(u8ZgpsCommModeAttr == E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_LIGHTWEIGHT)
    {
    	psZgpsSinkTable->b16Options |= GP_PROXY_TABLE_UNICAST_GPS_MASK;
    }
    else if (u8ZgpsCommModeAttr == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)
    {
    	psZgpsSinkTable->b16Options |= GP_PROXY_TABLE_DERIVED_GROUP_GPS_MASK;
    }
    else if(u8ZgpsCommModeAttr == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
    {
    	psZgpsSinkTable->b16Options |= GP_PROXY_TABLE_COMMISSION_GROUP_GPS_MASK;
    }


    if(psZgpCommissionIndication->eStatus == E_ZCL_SUCCESS)
    {
        psZgpsSinkTable->eGreenPowerSinkTablePriority = E_GP_SINK_TABLE_P_1;
    }
    else
    {
        psZgpsSinkTable->eGreenPowerSinkTablePriority = E_GP_SINK_TABLE_P_2;
    }


    /* check translation table pointer which was passed by app */
    if((psZgpCommissionIndication->psGpToZclCommandInfo != NULL) && (psZgpCommissionIndication->eCmdType == E_GP_DATA_CMD_AUTO_COMM))
    {
        psZgpsSinkTable->eZgpdDeviceId =  psZgpCommissionIndication->psGpToZclCommandInfo->eZgpdDeviceId;
        /* Set RxOnCap and Fixed Location fields */
        psZgpsSinkTable->b8SinkOptions |= (( psZgpDataIndication->bRxAfterTx) << 4);    /* Populate sink table entry */
    }
    else
    {
    	psZgpsSinkTable->eZgpdDeviceId =psZgpDataIndication->pu8Pdu[0];
        /* Set RxOnCap and  */
        psZgpsSinkTable->b8SinkOptions |= ((psZgpDataIndication->pu8Pdu[1] & GP_COMM_CMD_RX_ON_CAP_MASK)<< 3);    /* Populate sink table entry */

    }

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eGP_UpdateApsGroupTableAndDeviceAnnounce
 **
 ** DESCRIPTION:
 ** Adding group Id to APS group table and making ZGPD device announce
 **
 ** PARAMETERS:                         Name                            Usage
 ** ZPS_tsAfEvent                 *pZPSevent                     ZPS Event
 ** tsZCL_EndPointDefinition      *psEndPointDefinition          Endpoint definition
 ** tsGP_GreenPowerCustomData     *psGpCustomDataStructure       Custom data
 ** tsGP_ZgppProxySinkTable            *psZgpsSinkTable               Pointer to sink table
 ** tsGP_ZgpDataIndication        *psZgpDataIndication           GPDF Data Indication
 ** teZCL_Status                  eCommIndStatus                 Commission Indication status by App
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eGP_UpdateApsGroupTableAndDeviceAnnounce(
                            tsZCL_EndPointDefinition        *psEndPointDefinition,
                            tsGP_GreenPowerCustomData       *psGpCustomDataStructure,
                            tsGP_ZgpDataIndication          *psZgpDataIndication,
                            teZCL_Status                    eCommIndStatus)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
	 bool								bIsSinkTableEntryPresent;
	 tsGP_ZgppProxySinkTable                  *psZgpsSinkTable;

	 bIsSinkTableEntryPresent =  bGP_IsSinkTableEntryPresent(
			 psEndPointDefinition->u8EndPointNumber,
			 (uint8)psZgpDataIndication->u2ApplicationId,
			 &psZgpDataIndication->uZgpdDeviceAddr,
             &psZgpsSinkTable,
             0xff);

	if(bIsSinkTableEntryPresent == FALSE)
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "\nbIsSinkTableEntryPresent false\n");
		return E_ZCL_FAIL;
	}
	else
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "\n psZgpsSinkTable->u8GPDPaired = %d\n", psZgpsSinkTable->u8GPDPaired);
		/*if(psZgpsSinkTable->u8GPDPaired != E_GP_PAIRING_NOT_COMPLETED)
		{
			return E_ZCL_FAIL;
		}*/
		DBG_vPrintf(TRACE_GP_DEBUG, "\nbIsSinkTableEntryPresent true\n");
	}
    /* Add GP endpoint to ZPS group tables */
    if(eCommIndStatus == E_ZCL_SUCCESS)
    {
        /* For device doesn't have any payload */
        psZgpDataIndication->pu8Pdu = NULL;

        eGp_BufferTransmissionPacket(  psZgpDataIndication,
                            E_GP_DEVICE_ANNOUNCE,
                            psGpCustomDataStructure);

    	vGP_AddGPEPToGroup( psGpCustomDataStructure,
    			            psEndPointDefinition,
    	                    psZgpsSinkTable);

    }

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       vGP_RemoveTemperorySinkTableEntries
 **
 ** DESCRIPTION:
 ** Fills tsGP_ZgpDataIndication from ZPS_tsAfEvent
 **
 ** PARAMETERS:                    Name                           Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition		Data Indication
 ** tsGP_GreenPowerCustomData                       *psGpCustomDataStructure      			ZPS event
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/

void vGP_RemoveTemperorySinkTableEntries(uint8 u8EndpointID)
{
#ifdef GP_COMBO_BASIC_DEVICE
	bool_t bIsServer = TRUE;
#else
	bool_t bIsServer = FALSE;
#endif
	uint8 u8Status,i;
    tsZCL_EndPointDefinition                *psEndPointDefinition;
    tsZCL_ClusterInstance                   *psClusterInstance;
    tsGP_GreenPowerCustomData               *psGpCustomDataStructure;
   //find GP cluster
   if((u8Status = eGP_FindGpCluster(
			u8EndpointID,
			bIsServer,
			&psEndPointDefinition,
			&psClusterInstance,
			&psGpCustomDataStructure)) != E_ZCL_SUCCESS)
	{
	   DBG_vPrintf(TRACE_GP_DEBUG, "\n vGP_RemoveTemperorySinkTableEntries eGP_FindGpClusterr FALSE \n");
		return;
	}

   // get EP mutex
   #ifndef COOPERATIVE
		eZCL_GetMutex(psEndPointDefinition);
   #endif


	for(i = 0; i< GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES; i++)
	{   /* check if sink table entry is not empty */
		if (psGpCustomDataStructure->asZgpsSinkProxyTable[i].u8GPDPaired == E_GP_PAIRING_NOT_COMPLETED)
		{
			vGP_RemoveGPDFromProxySinkTable(u8EndpointID,
					((uint8)(psGpCustomDataStructure->asZgpsSinkProxyTable[i].b16Options & GP_APPLICATION_ID_MASK)),
					 &psGpCustomDataStructure->asZgpsSinkProxyTable[i].uZgpdDeviceAddr);
		}
	}
	  // release mutex
	  #ifndef COOPERATIVE
		  eZCL_ReleaseMutex(psEndPointDefinition);
	  #endif


}
#endif
/****************************************************************************
 **
 ** NAME:       vGP_SendDeviceAnnounce
 **
 ** DESCRIPTION:
 ** Sends device announce for the given alias short addrss
 **
 ** PARAMETERS:                         Name                            Usage

 ** RETURN:
 ** uint8
 **
 ****************************************************************************/
void vGP_SendDeviceAnnounce(uint16 u16AliasShortAddr, uint64 u64IeeeAddr)
{
	uint8 u8Status;
	void *pvNwk;
	uint16 u16NwkAddr;
    PDUM_thAPduInstance                         hAPduInst;
    ZPS_tsAplZdpDeviceAnnceReq                  sZdpDeviceAnnceReq;
    uint8                                        u8TransactionSequenceNumber;

    if(0xFFFFFFFFFFFFFFFFULL != u64IeeeAddr)
    {

    	u16NwkAddr = (uint16)RND_u32GetRand(1,0xfff7);
    	pvNwk = ZPS_pvAplZdoGetNwkHandle();
    	ZPS_vNwkNibSetNwkAddr(pvNwk, u16NwkAddr);
    	sZdpDeviceAnnceReq.u16NwkAddr = u16NwkAddr;
    	sZdpDeviceAnnceReq.u8Capability = 0x8E;
    	DBG_vPrintf(TRACE_GP_DEBUG, "vGP_SendDeviceAnnounce  u8SelfIEEEAddress = 0x%16llx\n",
    			u64IeeeAddr);
    }
    else
    {
    	sZdpDeviceAnnceReq.u16NwkAddr = u16AliasShortAddr;
    	sZdpDeviceAnnceReq.u8Capability = 0x00;
    	DBG_vPrintf(TRACE_GP_DEBUG, "vGP_SendDeviceAnnounce  u64IeeeAddr = 0x%16llx\n",
    			u64IeeeAddr);
        /* Before announcing call ZPS API to set alias short address  */
        ZPS_vAfZgpDeviceActive(u16AliasShortAddr);
    	ZPS_vAfSetZgpAlias(u16AliasShortAddr, 0x00, 0x00);
    }
    /* Populate device announce request */

	sZdpDeviceAnnceReq.u64IeeeAddr = u64IeeeAddr;


	/* allocate Apdu instance */
	hAPduInst = PDUM_hAPduAllocateAPduInstance(apduZDP);

	if(hAPduInst != NULL)
	{


	 /* send device announce */
	 u8Status = ZPS_eAplZdpDeviceAnnceRequest(
		 hAPduInst,
		 &u8TransactionSequenceNumber,
		 &sZdpDeviceAnnceReq);

	 if(u8Status)
	 {
		 /* Free hAPDU  in case of failure */
		 PDUM_eAPduFreeAPduInstance(hAPduInst);

	 }

	}
	else
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "ZPS_eAplZdpDeviceAnnceRequest Buffer alloc failed \n");
	}

}
/****************************************************************************
 **
 ** NAME:       u8GP_GetDataReqHandle
 **
 ** DESCRIPTION:
 ** Return back Data Req Handle
 **
 ** PARAMETERS:                         Name                            Usage

 ** RETURN:
 ** uint8
 **
 ****************************************************************************/
PUBLIC uint8 u8GP_GetDataReqHandle(tsGP_GreenPowerCustomData  *psGpCustomDataStructure)
{
    psGpCustomDataStructure->u8GPDataReqHandle++;
    if( psGpCustomDataStructure->u8GPDataReqHandle == ZPS_NWK_GP_MAX_HANDLE)
    {
        psGpCustomDataStructure->u8GPDataReqHandle = ZPS_NWK_GP_BASE_HANDLE + 1;
    }
    return psGpCustomDataStructure->u8GPDataReqHandle;
}

/****************************************************************************
 **
 ** NAME:       vGP_ExitCommMode
 **
 ** DESCRIPTION:
 ** Fills tsGP_ZgpDataIndication from ZPS_tsAfEvent
 **
 ** PARAMETERS:                    Name                           Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition		Data Indication
 ** tsGP_GreenPowerCustomData                       *psGpCustomDataStructure      			ZPS event
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/

void vGP_ExitCommMode(tsZCL_EndPointDefinition    *psEndPointDefinition,
		tsGP_GreenPowerCustomData                       *psGpCustomDataStructure)
{
	uint8 b8ZgpsCommExitModeAttr;
	tsZCL_Address                                   sDestinationAddress;
	DBG_vPrintf(TRACE_GP_DEBUG, "\n vGP_ExitCommMode \n");
	/* Get Green Power Exit Mode attribute value */
	eZCL_ReadLocalAttributeValue(
			psEndPointDefinition->u8EndPointNumber,
			GREENPOWER_CLUSTER_ID,
			TRUE,
			FALSE,
			FALSE,
			E_CLD_GP_ATTR_ZGPS_COMMISSIONING_EXIT_MODE,
			&b8ZgpsCommExitModeAttr);

	/* Do not worry about current mode , this check is done while sending the proxy commissioning mode command */
	eZCL_SetCustomCallBackEvent(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent, 0, 0,
			psEndPointDefinition->u8EndPointNumber);

	psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_COMMISSION_MODE_EXIT;
	psGpCustomDataStructure->u16CommissionWindow = 0x00;
	psGpCustomDataStructure->u64CommissionSetAddress = 0x00;
	psGpCustomDataStructure->bCommissionUnicast = 0x00;
    psGpCustomDataStructure->bIsCommissionReplySent = 0;
#ifdef GP_COMBO_BASIC_DEVICE
    vGP_RemoveTemperorySinkTableEntries(psEndPointDefinition->u8EndPointNumber);
#endif
	/* Change to device mode to operation mode */
	psGpCustomDataStructure->eGreenPowerDeviceMode = E_GP_OPERATING_MODE;
	/* Call End Point Call back to indicate exit commission mode */
	psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
#ifndef GP_DISABLE_PROXY_COMMISSION_MODE_CMD
	/* send proxy commissioning mode command (exit) */
	sDestinationAddress.eAddressMode = E_ZCL_AM_BROADCAST;
	sDestinationAddress.uAddress.eBroadcastMode = ZPS_E_APL_AF_BROADCAST_RX_ON;
	b8ZgpsCommExitModeAttr = eGP_ProxyCommissioningMode(psEndPointDefinition->u8EndPointNumber,
								ZCL_GP_PROXY_ENDPOINT_ID,
								sDestinationAddress,
								E_GP_PROXY_COMMISSION_EXIT);
#endif


}



/****************************************************************************
 **
 ** NAME:       bValidatePacket
 **
 ** DESCRIPTION:
 ** Checks freshness and length of packet
 **
 ** PARAMETERS:                    Name                           Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       endpoint definition
 ** tsGP_GreenPowerCustomData   *psGpCustomDataStructure     Custom data structure
 ** bool_t						 *pbIsEntryPresent          Checks if entry present
 **
 ** RETURN:
 ** TRUE if packet is valid, FALSE otherwise
 ****************************************************************************/
bool_t bValidatePacket(
		tsZCL_EndPointDefinition    						*psEndPointDefinition,
		tsGP_GreenPowerCustomData                       	*psGpCustomDataStructure,
	    bool_t     										    *pbIsEntryPresent,
	    uint8												u8AppId,
	    uint32												u32FrameCounter,
	    tsGP_ZgppProxySinkTable                              **psZgpsSinkTable,
		tuGP_ZgpdDeviceAddr                           *puZgpdAddress

)
{
	bool_t bPacketValid = TRUE;

	*pbIsEntryPresent =  bGP_IsProxyTableEntryPresent(
	     		psEndPointDefinition->u8EndPointNumber,
#ifdef GP_COMBO_BASIC_DEVICE
	     		TRUE,
#else
	     		FALSE,
#endif
	     		u8AppId,
	     		puZgpdAddress,
	     		psZgpsSinkTable);

    return bPacketValid;
}

/****************************************************************************
 **
 ** NAME:       bValidatePacketLength
 **
 ** DESCRIPTION:
 ** Checks packet length
 ** The payload of any GPD Data command sent by the GPD SHALL NOT exceed:
 ** 	For a GPD with ApplicationID = 0b000: 59 octets;
 ** 	For a GPD with ApplicationID = 0b010: 54 octets.
 ** The payload of any GPD commissioning command sent by the GPD SHALL NOT exceed:
 ** 	For a GPD with ApplicationID = 0b000: 55 octets;
 **     For a GPD with ApplicationID = 0b010: 50 octets.
 **
 **
 **
 ** PARAMETERS:                    Name                           Usage
 ** tsZCL_EndPointDefinition    psEndPointDefinition,                endpoint Definition
 ** tsGP_GreenPowerCustomData  *psGpCustomDataStructure               Cluster custom data structure
 ** tsGP_ZgpDataIndication       *psZgpDataIndication
 **
 ** RETURN:
 ** teGP_GreenPowerStatus
 ****************************************************************************/
bool_t bValidatePacketLength(
		tsZCL_EndPointDefinition    						*psEndPointDefinition,
		tsGP_ZgpDataIndication                              *psZgpDataIndication,
		tsGP_GreenPowerCustomData                       	*psGpCustomDataStructure )
{
	bool_t IsPacketLenOK = TRUE;

	bool_t bIsCommissionPacket = ( ( psZgpDataIndication->u8CommandId >= E_GP_COMMISSIONING &&
			                       psZgpDataIndication->u8CommandId <= E_GP_COMMISSIONING_RANGE_END) ||
			                     ( psZgpDataIndication->u8CommandId >= 0xB0 &&
			                       psZgpDataIndication->u8CommandId <= 0xBF))? 1:0;
    /* Check Green Power Payload length */

	/** The payload of any GPD Data command sent by the GPD SHALL NOT exceed:
	 ** 	For a GPD with ApplicationID = 0b000: 59 octets
	 ** 	For a GPD with ApplicationID = 0b010: 54 octets*/
	if((bIsCommissionPacket == FALSE ) &&
       ((psZgpDataIndication->u8PDUSize > GP_MAX_ZB_CMD_PAYLOAD_LENGTH) &&
       (psZgpDataIndication->u2ApplicationId == GP_APPL_ID_4_BYTE)))
	{
		IsPacketLenOK = FALSE;
	}
	else if((bIsCommissionPacket == FALSE ) &&
			((psZgpDataIndication->u8PDUSize > GP_MAX_ZB_CMD_PAYLOAD_LENGTH_APP_ID_2) &&
			(psZgpDataIndication->u2ApplicationId == GP_APPL_ID_8_BYTE)))
	{
		IsPacketLenOK = FALSE;
	}
	else if((bIsCommissionPacket == TRUE ) &&
			((psZgpDataIndication->u8PDUSize > GP_MAX_ZB_COMM_CMD_PAYLOAD_LENGTH) &&
			(psZgpDataIndication->u2ApplicationId == GP_APPL_ID_4_BYTE)))
	{
		IsPacketLenOK = FALSE;
	}
	 /** The payload of any GPD commissioning command sent by the GPD SHALL NOT exceed:
	 ** 	For a GPD with ApplicationID = 0b000: 55 octets
	 **     For a GPD with ApplicationID = 0b010: 50 octets*/
	else if((bIsCommissionPacket == TRUE ) &&
			((psZgpDataIndication->u8PDUSize > GP_MAX_ZB_COMM_CMD_PAYLOAD_LENGTH_APP_ID_2) &&
			(psZgpDataIndication->u2ApplicationId == GP_APPL_ID_8_BYTE)))
	{
		IsPacketLenOK = FALSE;
	}
    if(IsPacketLenOK == FALSE)
    {
        /* Inform application unsupported payload length as we are buffering packet for delayed transmission */
        psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_CMD_UNSUPPORTED_PAYLOAD_LENGTH;

        /* Give Application Callback for functionality matching */
        psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);

    }
    return IsPacketLenOK;
}

/****************************************************************************
 **
 ** NAME:       vGP_TxLoopBackCmds
 **
 ** DESCRIPTION:
 ** Transmits the loop back commands buffered
 **
 ** PARAMETERS:                                  Name                        Usage
 ** uint8                                       u8SrcEndPointId				Source Endpont
 ** tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord   Buffered command
 ** tsGP_GreenPowerCustomData                   *psGpCustomDataStructure   GP custom data structure
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/
#ifdef GP_COMBO_BASIC_DEVICE
PRIVATE void vGP_TxLoopBackCmds(
		uint8                                       u8SrcEndPointId,
		tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
        tsGP_GreenPowerCustomData                   *psGpCustomDataStructure
        )
{

	bool_t bZgpIdMatch = FALSE, bMatchSuccess = FALSE;
    uint8 u8MatchedCmdIndex;
	uint8 i, u8ApplicationId = (uint8)psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId;
	/* Traverse translation table */
	for(i = psZgpBufferedApduRecord->sBufferedApduInfo.u8TranslationIndex; i < GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES ; i++)
	{
		/* check for translation pointer is populated or not */
		if(psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo != NULL)
		{
			/* check application id type */
			if((psGpCustomDataStructure->psZgpsTranslationTableEntry[i].b8Options & (uint8)GP_APPLICATION_ID_MASK) == u8ApplicationId)
			{
				/* Is application id type is 4 bytes source id */
				if(u8ApplicationId == GP_APPL_ID_4_BYTE)
				{
					if(psGpCustomDataStructure->psZgpsTranslationTableEntry[i].uZgpdDeviceAddr.u32ZgpdSrcId == psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.u32ZgpdSrcId)
					{
						bZgpIdMatch = TRUE;
					}
				}
#ifdef GP_IEEE_ADDR_SUPPORT
				else
				{
					if((psGpCustomDataStructure->psZgpsTranslationTableEntry[i].uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr ==
							psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr) &&
									((psGpCustomDataStructure->psZgpsTranslationTableEntry[i].uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint) ||
									(psGpCustomDataStructure->psZgpsTranslationTableEntry[i].uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint == 0)))
					{
						bZgpIdMatch = TRUE;
					}
				}
#endif

				/* Check entry found in translation table or not */
				if(bZgpIdMatch)
				{
					bZgpIdMatch = FALSE;
					/* checking for matched GP command id */
					if(bIsCommandMapped((teGP_ZgpdCommandId)psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId,
					  &u8MatchedCmdIndex,
					  &psGpCustomDataStructure->psZgpsTranslationTableEntry[i]))
				  /*  if((psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo[u8MatchedCmdIndex].eZgpdCommandId == (teGP_ZgpdCommandId)psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId)&&
							(psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo[u8MatchedCmdIndex].u8EndpointId != 0xFD))*/
					{

						bMatchSuccess = TRUE;
						/* translate ZGP Command into ZCL command and send it to linked endpoint */
						eGP_TranslateCommandIntoZcl(
								u8SrcEndPointId,
								i,
								u8MatchedCmdIndex,
								psGpCustomDataStructure,
								psZgpBufferedApduRecord->sBufferedApduInfo.u8PDUSize,
								psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU);
					}
				}
			}
		}
		psZgpBufferedApduRecord->sBufferedApduInfo.u8TranslationIndex++;
		/* if ZCL packet is already pushed to stack then wait for some time to push next packet */
		if(bMatchSuccess)
		{
			psZgpBufferedApduRecord->sBufferedApduInfo.u16Delay = 3;
			break;
		}
	}

}
#endif

/****************************************************************************
  **
  ** NAME:       u8GetGPDFKeyMAP
  **
  ** DESCRIPTION:
  ** Green Power direct data command handler
  **
  ** PARAMETERS:                    Name                           Usage
  **  uint8                       u8EndPoint,
  **  uint8                       u8GPDFKeyType
  **
  ** RETURN:
  ** Gets the keytype mapped from GPDF keytype
  ****************************************************************************/
uint8 u8GetGPDFKeyMAP(uint8 u8EndPoint, uint8 u8GPDFKeyType)
{
#ifdef GP_COMBO_BASIC_DEVICE
	bool_t bIsServer = TRUE;
#else
	bool_t bIsServer = FALSE;
#endif
	 uint8 u8KeyType = 0;
	 if(eZCL_ReadLocalAttributeValue(
					         u8EndPoint,
							 GREENPOWER_CLUSTER_ID,
							 bIsServer,
							 FALSE,
			                !bIsServer,
							 E_CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE,
							 &u8KeyType) != E_ZCL_SUCCESS)
	 {
		 u8KeyType = 0;
	 }

	 if(u8GPDFKeyType == E_GPD_INDIVIDUAL_KEY)
	 {
		 if(u8KeyType != E_GP_DERIVED_INDIVIDUAL_ZGPD_KEY)
		 {
			 u8KeyType = E_GP_OUT_OF_THE_BOX_ZGPD_KEY;
		 }
	 }

	 return u8KeyType;
}
/****************************************************************************
 **
 ** NAME:       eGP_AddOrUpdateProxyTable
 **
 ** DESCRIPTION:
 ** Adds a proxy table entry if not present, otherwise updates
 **
 ** PARAMETERS:                    Name                           Usage
 ** uint8 						u8EndPointNumber,                endpoint number
 ** bool						bIsEntryPresent,                 Entry present in proxy table
 ** tsGP_ZgppProxySinkTable         *psZgppProxyTable,               Proxy table
 ** tsGP_ZgpBufferedApduRecord  *psZgpBufferedApduRecord           Buffered details
 **
 ** RETURN:
 ** teGP_GreenPowerStatus
 ****************************************************************************/
#ifdef GP_COMBO_BASIC_DEVICE
PUBLIC  bool_t bGP_AddOrUpdateProxySinkTableEntries   (
		tsZCL_EndPointDefinition    						*psEndPointDefinition,
		tsGP_GreenPowerCustomData                           *psGpCustomDataStructure,
   	    tsGP_ZgpDataIndication               		        *psZgpDataIndication,
   	    tsGP_ZgpCommissionIndication                        *psZgpCommissionIndication
   )
{
	bool_t bUpdateKeyType = 0, bFCCheck = FALSE;
	uint8 u8SharedKeyType = 0;
	tsZCL_LongOctetString sSinkProxyTable;
	uint8 u8NoOfEntries;
	tsGP_ZgppProxySinkTable                 *psZgppProxySinkTable;
	bool_t         bIsTableEntryPresent;
	AESSW_Block_u uInKey;
#ifdef GP_COMBO_BASIC_DEVICE
	bool_t bIsServer = TRUE;
#else
	bool_t bIsServer = FALSE;
#endif
	bIsTableEntryPresent =  bGP_IsGPDPresent(
	     		psEndPointDefinition->u8EndPointNumber,
	     		bIsServer,
	     		(uint8)psZgpDataIndication->u2ApplicationId,
	     		&psZgpCommissionIndication->uZgpdDeviceAddr,
	     		
	     		&psZgppProxySinkTable);


	 if(bIsTableEntryPresent == FALSE)
	 {
		 bIsTableEntryPresent = bGP_GetFreeProxySinkTableEntry(psEndPointDefinition->u8EndPointNumber,
				           bIsServer,
	                    &psZgppProxySinkTable);

		 if(bIsTableEntryPresent == FALSE)
		 {
				DBG_vPrintf(TRACE_GP_DEBUG, "\nE_GP_SINK_PROXY_TABLE_FULL \n");

				psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_SINK_PROXY_TABLE_FULL;
				return FALSE;
		 }

		 DBG_vPrintf(TRACE_GP_DEBUG, "\n Proxy table entry does not exist , allocated new \n");
		 psZgppProxySinkTable->u8GPDPaired = E_GP_PAIRING_NOT_COMPLETED;

	 }
	 else
	 {
#ifdef GP_COMBO_BASIC_DEVICE
		 if (psZgppProxySinkTable->u8GPDPaired != E_GP_PAIRED)
		 {
			 psZgppProxySinkTable->u8GPDPaired = E_GP_PAIRING_NOT_COMPLETED;
		 }
#endif
		 bFCCheck = TRUE;
		 DBG_vPrintf(TRACE_GP_DEBUG, "\n bFCCheck = TRUE \n");
	 }

   /* Before creating proxy table entry, update proxy table entry info */
   if(psZgpDataIndication->u2ApplicationId == GP_APPL_ID_4_BYTE)
   {
	   psZgppProxySinkTable->b16Options = GP_APPL_ID_4_BYTE;
	   psZgppProxySinkTable->uZgpdDeviceAddr.u32ZgpdSrcId = psZgpDataIndication->uZgpdDeviceAddr.u32ZgpdSrcId;

   }
#ifdef GP_IEEE_ADDR_SUPPORT
   else
   {
	   psZgppProxySinkTable->b16Options = GP_APPL_ID_8_BYTE;
	   psZgppProxySinkTable->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr =
			   psZgpDataIndication->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;

	  psZgppProxySinkTable->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint =
			  psZgpDataIndication->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
   }
#endif
	if(psZgpDataIndication->bTunneledPkt == FALSE)
	{
		psZgppProxySinkTable->b16Options |= GP_PROXY_TABLE_IN_RANGE_MASK;
	}
   psZgppProxySinkTable->b16Options |= GP_PROXY_TABLE_ENTRY_ACTIVE_MASK; /* Active and Invalid flag */
   psZgppProxySinkTable->b16Options |= GP_PROXY_TABLE_ENTRY_VALID_MASK;
   psZgppProxySinkTable->u32ZgpdSecFrameCounter =psZgpDataIndication->u32SecFrameCounter;

   if(psZgpDataIndication->u8CommandId == E_GP_COMMISSIONING)
   {

	   psZgppProxySinkTable->b16Options |= ((psZgpDataIndication->pu8Pdu[1] & GP_COMM_CMD_MAC_SEQ_NUM_CAP_MASK) << 5); /* Seq Cap flag */
	   psZgppProxySinkTable->b16Options |= ((psZgpDataIndication->pu8Pdu[1] &  GP_COMM_CMD_FIXED_LOCATION_MASK) << 5) ;  /* Fixed location flag */
	   if((psZgpDataIndication->pu8Pdu[1] & GP_COMM_CMD_ZGP_SEC_KEY_REQ_MASK))
	   {
		   bUpdateKeyType = TRUE;
	   }
	   if(psZgpDataIndication->pu8Pdu[1] & GP_COMM_CMD_EXT_OPT_FIELD_MASK )
	   {
		   if((psZgpDataIndication->pu8Pdu[2] & GP_SECURITY_LEVEL_MASK) )
		   {


			   uint8 u8FrameCounterIndex = 3;

			   psZgppProxySinkTable->b16Options |= GP_PROXY_TABLE_SECURITY_USE_MASK;
			   psZgppProxySinkTable->b8SecOptions = psZgpDataIndication->pu8Pdu[2] & GP_SECURITY_LEVEL_MASK; // sec level
			   psZgppProxySinkTable->b8SecOptions |= psZgpDataIndication->pu8Pdu[2] & GP_SECURITY_KEY_TYPE_MASK;// sec key type

			   if(psZgpDataIndication->pu8Pdu[2] & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_PRESENT_MASK)
			   {
				   u8FrameCounterIndex += E_ZCL_KEY_128_SIZE;
				   if(psZgpDataIndication->pu8Pdu[2] & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_ENC_MASK)
				   {
					   u8FrameCounterIndex += 4; //MIC size
				   }
					memcpy( psZgppProxySinkTable->sZgpdKey.au8Key,psZgpCommissionIndication->uCommands.sZgpCommissionCmd.sZgpdKey.au8Key , E_ZCL_KEY_128_SIZE);
			   }


			   /* get 4 byte frame counter from payload */
			   if(psZgpDataIndication->pu8Pdu[2] & GP_COMM_CMD_EXT_OPT_ZGPD_OUT_GOING_COUNTER_MASK)
			   {

				   uint32 u32FrameCounter ;
				   u32FrameCounter = psZgpDataIndication->pu8Pdu[u8FrameCounterIndex++];
				   u32FrameCounter |= psZgpDataIndication->pu8Pdu[u8FrameCounterIndex++] << 8;
				   u32FrameCounter |= psZgpDataIndication->pu8Pdu[u8FrameCounterIndex++]<< 16;
				   u32FrameCounter |= psZgpDataIndication->pu8Pdu[u8FrameCounterIndex]<< 24;
				   if(bFCCheck)
				   {

					   DBG_vPrintf(TRACE_GP_DEBUG, "\n psZgppProxySinkTable->u32ZgpdSecFrameCounter,u32FrameCounter =  0x%8x  u32FrameCounter 0x%8x \n",
							   psZgppProxySinkTable->u32ZgpdSecFrameCounter,u32FrameCounter, u32FrameCounter );
					  if(psZgppProxySinkTable->u32ZgpdSecFrameCounter < u32FrameCounter)
					  {
						  return FALSE;
					  }
					   psZgppProxySinkTable->bProxyTableEntryOccupied = TRUE;
				   }
				   psZgppProxySinkTable->u32ZgpdSecFrameCounter =u32FrameCounter;

			   }
		   }
	   }
   }
	eZCL_ReadLocalAttributeValue(
							   psEndPointDefinition->u8EndPointNumber,
					  						 GREENPOWER_CLUSTER_ID,
					  						 bIsServer,
					  						 FALSE,
					  		                !bIsServer,
					  		                 E_CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE,
					  		                 &u8SharedKeyType);
   /* Check Security status */
   if(( psZgpDataIndication->u8CommandId != E_GP_COMMISSIONING) && ((psZgpDataIndication->u8Status == E_GP_SEC_SUCCESS) ||
	   (psZgpDataIndication->u8Status > E_GP_SEC_NO_SECURITY)) )
   {
	   uint8 u8SecKeyType;
	   u8SecKeyType = u8GetGPDFKeyMAP(psEndPointDefinition->u8EndPointNumber,psZgpDataIndication->u2SecurityKeyType);

	   psZgppProxySinkTable->b16Options |= GP_PROXY_TABLE_SECURITY_USE_MASK;
	   psZgppProxySinkTable->b8SecOptions = (uint8)psZgpDataIndication->u2SecurityLevel;
	   psZgppProxySinkTable->b8SecOptions |= (u8SecKeyType << 2);
	   if(u8SecKeyType == u8SharedKeyType)
	   {
		   bUpdateKeyType = TRUE;
	   }
	   DBG_vPrintf(TRACE_GP_DEBUG, "\n Update sec from ZPS_psZgpFindGpstEntry %d  u8SecKeyType = %d, command id = %d\n",
					   psZgppProxySinkTable->b8SecOptions, u8SecKeyType, psZgpDataIndication->u8CommandId);

   }

   DBG_vPrintf(TRACE_GP_DEBUG, "\nbUpdateKeyType= %d\n",bUpdateKeyType);
	if(((u8SharedKeyType == E_GP_ZGPD_GROUP_KEY) || (u8SharedKeyType == E_GP_NWK_KEY_DERIVED_ZGPD_GROUP_KEY) ||
			(u8SharedKeyType == E_GP_ZIGBEE_NWK_KEY)) && bUpdateKeyType)
	{

		psZgppProxySinkTable->b8SecOptions &= ~GP_SECURITY_KEY_TYPE_MASK;
		psZgppProxySinkTable->b8SecOptions |= u8SharedKeyType << 2;

		/* overwrite keytype and key in the table */
		eZCL_ReadLocalAttributeValue(
						   psEndPointDefinition->u8EndPointNumber,
							 GREENPOWER_CLUSTER_ID,
							 bIsServer,
							 FALSE,
							!bIsServer,
							 E_CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY,
							uInKey.au8);
	   if(u8SharedKeyType == E_GP_NWK_KEY_DERIVED_ZGPD_GROUP_KEY)
	   {
		   AESSW_Block_u uOutKey;
			psZgppProxySinkTable->b8SecOptions &= ~GP_SECURITY_KEY_TYPE_MASK;
			psZgppProxySinkTable->b8SecOptions |= u8SharedKeyType << 2;

		   ZPS_vZgpTransformKey(ZPS_E_ZGP_NWK_KEY_DERIVED_GRP_KEY,
				   (uint8)psZgpDataIndication->u2ApplicationId,
				   psZgpDataIndication->uZgpdDeviceAddr.u32ZgpdSrcId,
				   psZgpDataIndication->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr,
				   &uInKey,
				   &uOutKey);
		   memcpy( psZgppProxySinkTable->sZgpdKey.au8Key,uOutKey.au8 , E_ZCL_KEY_128_SIZE);
	   }
	   else if(u8SharedKeyType == E_GP_ZGPD_GROUP_KEY)
	   {
		   memcpy( psZgppProxySinkTable->sZgpdKey.au8Key,uInKey.au8 , E_ZCL_KEY_128_SIZE);
	   }
	   else if((u8SharedKeyType == E_GP_ZIGBEE_NWK_KEY))
		{
			   AESSW_Block_u uOutKey;

			   ZPS_vZgpTransformKey(ZPS_E_ZGP_ZIGBEE_NWK_KEY,
					   (uint8)psZgpDataIndication->u2ApplicationId,
					   psZgpDataIndication->uZgpdDeviceAddr.u32ZgpdSrcId,
					   psZgpDataIndication->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr,
					   NULL,
					   &uOutKey);
			   memcpy( psZgppProxySinkTable->sZgpdKey.au8Key,uOutKey.au8 , E_ZCL_KEY_128_SIZE);
		}
	}
   /* On combo minimum update sink table if in pairing mode*/
#ifdef GP_COMBO_BASIC_DEVICE
   if( psGpCustomDataStructure->eGreenPowerDeviceMode == E_GP_PAIRING_COMMISSION_MODE)
   {
	   eGP_UpdateSinkTable(psEndPointDefinition,psGpCustomDataStructure,psZgppProxySinkTable,psZgpDataIndication,psZgpCommissionIndication);

   }
   sSinkProxyTable.u16MaxLength = u16GP_GetStringSizeOfSinkTable(psEndPointDefinition->u8EndPointNumber, &u8NoOfEntries, NULL);
   sSinkProxyTable.u16Length = sSinkProxyTable.u16MaxLength;
   sSinkProxyTable.pu8Data = (uint8 *) psGpCustomDataStructure->asZgpsSinkProxyTable;
   eZCL_WriteLocalAttributeValue(psEndPointDefinition->u8EndPointNumber,GREENPOWER_CLUSTER_ID,TRUE,FALSE,FALSE,E_CLD_GP_ATTR_ZGPS_SINK_TABLE, (uint8*)&sSinkProxyTable);
#endif
   sSinkProxyTable.u16MaxLength = u16GP_GetStringSizeOfProxyTable(psEndPointDefinition->u8EndPointNumber, &u8NoOfEntries, NULL);
   sSinkProxyTable.u16Length = sSinkProxyTable.u16MaxLength;
   sSinkProxyTable.pu8Data = (uint8 *) psGpCustomDataStructure->asZgpsSinkProxyTable;
   eZCL_WriteLocalAttributeValue(psEndPointDefinition->u8EndPointNumber,GREENPOWER_CLUSTER_ID,TRUE,FALSE,FALSE,E_CLD_GP_ATTR_ZGPS_SINK_TABLE, (uint8*)&sSinkProxyTable);

   return TRUE;

}
#endif

/****************************************************************************
 **
 ** NAME:       bGP_ValidateGPDF_Fields
 **
 ** DESCRIPTION:
 ** Green Power autocommission packet  field validation
 **
 ** PARAMETERS:                    Name                           Usage
 ** tsGP_ZgpDataIndication 	*psZgpDataIndication			tHE gpdf in format tsGP_ZgpDataIndication
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
bool_t bGP_ValidateGPDF_Fields(
		tsGP_ZgpDataIndication                         *psZgpDataIndication,
		tsGP_GreenPowerCustomData                      *psGpCustomDataStructure)
{

	 /* If Auto-Commissioning sub-field was set to 0b1 in a GPDF carrying GPD Commissioning command
	   silently drop the frame */
	 if((psZgpDataIndication->bAutoCommissioning)  &&
			 ((psZgpDataIndication->u8CommandId == E_GP_COMMISSIONING) || (psZgpDataIndication->u8CommandId == E_GP_SUCCESS)))
	 {

		 DBG_vPrintf(TRACE_GP_DEBUG, "\n bGP_ValidateGPDF_Field psZgpDataIndication->bAutoCommissioning =%d, psZgpDataIndication ->u8CommandId  = %d\n",
				 psZgpDataIndication->bAutoCommissioning, psZgpDataIndication ->u8CommandId );
		 return FALSE;
	 }
	 /* If RxAfterTx sub-field was set to 0b1 in a Data GPDF (i.e. with GPD CommandID other than 0xE0)
	  * with Auto-Commissioning sub-field set to 0b1: silently drop the frame */
	 if((psZgpDataIndication->bAutoCommissioning)  &&
			 (psZgpDataIndication->bRxAfterTx))
	 {
		 DBG_vPrintf(TRACE_GP_DEBUG, "\n bGP_ValidateGPDF_Field psZgpDataIndication->bAutoCommissioning =%d, psZgpDataIndication->bRxAfterTx  = %d\n",
				 psZgpDataIndication->bAutoCommissioning, psZgpDataIndication->bRxAfterTx );

		 return FALSE;
	 }

	 /* If for ApplicationID = GP_APPL_ID_4_BYTE the SrcID was set to 0x00000000 or for ApplicationID =
	  *  GP_APPL_ID_8_BYTE the 	  *  GPD IEEE address was set to 0x0000000000000000,
	  *   the proxy and sink will silently drop the frame. */
	 if(((psZgpDataIndication->u2ApplicationId == GP_APPL_ID_4_BYTE) &&
			 (psZgpDataIndication->uZgpdDeviceAddr.u32ZgpdSrcId == 0)&&
			 (psZgpDataIndication->bFrameType  == 0))||
		 ((psZgpDataIndication->u2ApplicationId == GP_APPL_ID_8_BYTE) &&
		 			 (psZgpDataIndication->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr == 0)))
		{
		 DBG_vPrintf(TRACE_GP_DEBUG, "\n bGP_ValidateGPDF_Field psZgpDataIndication->u32ZgpdSrcId =%d, psZgpDataIndication->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr  = %d\n",
				 psZgpDataIndication->uZgpdDeviceAddr.u32ZgpdSrcId,
				 psZgpDataIndication->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr);

		 	 return FALSE;

		}
		/* Maintenance  frame should be set to only channel request  in commissioning mode*/
	 if(psZgpDataIndication->bFrameType )
	 {
		 if(psGpCustomDataStructure->eGreenPowerDeviceMode == E_GP_OPERATING_MODE )
		 {
			 DBG_vPrintf(TRACE_GP_DEBUG, "\n Maintenance  frame should not be set in operational mode %d %d \n",
								 psZgpDataIndication->bFrameType,
								 psGpCustomDataStructure->eGreenPowerDeviceMode);

						return FALSE;
		 }
		 else
		 {
			 if(!((psZgpDataIndication->u8CommandId == E_GP_CHANNEL_REQUEST) || (psZgpDataIndication->u8CommandId == E_GP_CHANNEL_CONFIGURATION)))
			 {
				 DBG_vPrintf(TRACE_GP_DEBUG, "\n Maintenance  frame should be set to only channel request  in commissioning mode %d, %d, %d\n",
				 					 psZgpDataIndication->u8CommandId ,
				 					 psZgpDataIndication->bFrameType,
				 					 psGpCustomDataStructure->eGreenPowerDeviceMode);
				 return FALSE;
			 }
		 }

	 }

	 return TRUE;
}

/****************************************************************************
 **
 ** NAME:       vFillCommInd
 **
 ** DESCRIPTION:
 ** Security validation of GPDF
 **
 ** PARAMETERS:                    		Name                           Usage
 ** tsGP_ZgpCommissionIndication	*psZgpCommissionIndication		Commissioin Indication
 ** ZPS_tsAfEvent      				*pZPSevent        					 ZPS event
 **
 ** RETURN:
 ** TRUE or FALSE
 ****************************************************************************/
bool_t bGP_FillCommInd(tsGP_ZgpCommissionIndication                    		*psZgpCommissionIndication,
		          tsGP_ZgpDataIndication               		            *psZgpDataIndication,
		          tsZCL_EndPointDefinition    							*psEndPointDefinition,
		         tsGP_GreenPowerCustomData                              *psGpCustomDataStructure)
{
	uint8 *pu8Payload = (uint8*)(psZgpDataIndication->pu8Pdu);
	uint8 *pu8Data = pu8Payload +2;


	/* Populate commission data indication event structure */
	psZgpCommissionIndication->uCommands.sZgpCommissionCmd.eZgpdDeviceId = *pu8Payload;
	psZgpCommissionIndication->uCommands.sZgpCommissionCmd.b8Options = *(pu8Payload + 1);

	/* check extended option field present or not */
	if(psZgpCommissionIndication->uCommands.sZgpCommissionCmd.b8Options & GP_COMM_CMD_EXT_OPT_FIELD_MASK )
	{
		psZgpCommissionIndication->uCommands.sZgpCommissionCmd.b8ExtendedOptions = *(pu8Payload + 2);

		/* check security key present in the commission packet */
		if(psZgpCommissionIndication->uCommands.sZgpCommissionCmd.b8ExtendedOptions & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_PRESENT_MASK)
		{
			/* copy the 16 byte key, also decrypt key  */
			 if(psZgpCommissionIndication->uCommands.sZgpCommissionCmd.b8ExtendedOptions & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_ENC_MASK)
			 {
				 if( bEncryptDecryptKey(
						 FALSE,
						 psZgpCommissionIndication->b8AppId,
						 &psZgpCommissionIndication->uZgpdDeviceAddr,
						 0,
						 (pu8Payload + 3),
						 psZgpCommissionIndication->uCommands.sZgpCommissionCmd.sZgpdKey.au8Key,
						 (pu8Payload + 3 + E_ZCL_KEY_128_SIZE),
						 psEndPointDefinition,
						 psGpCustomDataStructure) == FALSE)
				 {
					 return FALSE;
				 }

			 }
			 else
			 {
				 memcpy(psZgpCommissionIndication->uCommands.sZgpCommissionCmd.sZgpdKey.au8Key,(pu8Payload + 3), E_ZCL_KEY_128_SIZE);
			 }
			 /**/
		}
		else
		{
				/* Copy the shared key. This will be required */
		}

		/* check MIC present in GP commission command */
		if( (psZgpCommissionIndication->uCommands.sZgpCommissionCmd.b8ExtendedOptions & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_PRESENT_MASK) &&
			(psZgpCommissionIndication->uCommands.sZgpCommissionCmd.b8ExtendedOptions & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_ENC_MASK))
		{
			pu8Data = pu8Payload + 19;
#ifdef PC_BUILD
			psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u32ZgpdKeyMic = *((uint32 *)(pu8Payload + 19));
#else
			psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u32ZgpdKeyMic = *pu8Data++;
			psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u32ZgpdKeyMic |= (*pu8Data++) << 8;
			psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u32ZgpdKeyMic |= (*pu8Data++) << 16;
			psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u32ZgpdKeyMic |= (*pu8Data++) << 24;
#endif
		}

		/* check outgoing counter field present or not */
		if(psZgpCommissionIndication->uCommands.sZgpCommissionCmd.b8ExtendedOptions & GP_COMM_CMD_EXT_OPT_ZGPD_OUT_GOING_COUNTER_MASK)
		{
			pu8Data = pu8Payload + 19;

			if( (psZgpCommissionIndication->uCommands.sZgpCommissionCmd.b8ExtendedOptions & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_PRESENT_MASK) &&
				(psZgpCommissionIndication->uCommands.sZgpCommissionCmd.b8ExtendedOptions & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_ENC_MASK))
			{
				pu8Data = pu8Payload + 23;
			}
			else if(psZgpCommissionIndication->uCommands.sZgpCommissionCmd.b8ExtendedOptions & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_PRESENT_MASK)
			{
				pu8Data = pu8Payload + 19;
			}
			else
			{
				pu8Data = pu8Payload + 3;
			}

			psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u32ZgpdOutgoingCounter = *pu8Data++;
			psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u32ZgpdOutgoingCounter |= (*pu8Data++) << 8;
			psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u32ZgpdOutgoingCounter |= (*pu8Data++) << 16;
			psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u32ZgpdOutgoingCounter |= (*pu8Data++) << 24;
		}
	}
	if(psZgpCommissionIndication->uCommands.sZgpCommissionCmd.b8Options & GP_COMM_CMD_APP_INFO_PRESENT )
	{

		uint8 i;
		psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u8ApplicationInfo = *pu8Data++;
		if(psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u8ApplicationInfo  & GP_COMM_CMD_APP_INFO_MANU_ID)
		{
			psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u16ManufID = *pu8Data++;
			psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u16ManufID |= *pu8Data++ << 8;
		}
		if(psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u8ApplicationInfo  & GP_COMM_CMD_APP_INFO_MODEL_ID)
		{
			psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u16ModelID = *pu8Data++;
			psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u16ModelID |= *pu8Data++ << 8;
		}
		if(psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u8ApplicationInfo  & GP_COMM_CMD_APP_INFO_CMD_ID)
		{
			psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u8NoOfGPDCommands = *pu8Data++;
			for(i=0; (i<psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u8NoOfGPDCommands) && (i < GP_COMM_MAX_COUNT_COMMAND_ID); i++ )
			{
				psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u8CommandList[i]= *pu8Data++;
			}
		}
		if(psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u8ApplicationInfo  & GP_COMM_CMD_APP_INFO_CLUSTER_ID)
		{
			psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u8NoOfClusters = *pu8Data++;
			for(i=0; (i<(psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u8NoOfClusters & 0x0f )) && (i < GP_COMM_MAX_COUNT_CLUSTER); i++ )
			{
				psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u16ClusterList[i]= *pu8Data++;
				psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u16ClusterList[i] |= *pu8Data++ << 8;
			}
			for(; (i<(psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u8NoOfClusters & 0xf0 )) && (i < GP_COMM_MAX_COUNT_CLUSTER); i++ )
			{
				psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u16ClusterList[i]= *pu8Data++;
				psZgpCommissionIndication->uCommands.sZgpCommissionCmd.u16ClusterList[i] |= *pu8Data++ << 8;
			}
		}
	}
	return TRUE;
}

/****************************************************************************
 **
 ** NAME:       bGP_SecurityValidation
 **
 ** DESCRIPTION:
 ** Security validation of GPDF
 **
 ** PARAMETERS:                    Name                           Usage
 ** ZPS_tsAfEvent              	  *pZPSevent              		ZPS event
 ** tsZCL_EndPointDefinition      *psEndPointDefinition         Entry present in proxy table
 ** tsGP_GreenPowerCustomData     *psGpCustomDataStructure       Proxy table
 **
 ** RETURN:
 ** teGP_GreenPowerStatus
 ****************************************************************************/
bool_t  bGP_SecurityValidation(
		tsGP_ZgpDataIndication                         *psZgpDataIndication,
		 tsZCL_EndPointDefinition    					*psEndPointDefinition,
		 tsGP_GreenPowerCustomData                      *psGpCustomDataStructure,
		 tsGP_ZgppProxySinkTable                        *psZgpsSinkTable)
 {

	if(psZgpDataIndication->u8Status == E_GP_SEC_SUCCESS ||
			psZgpDataIndication->u8Status == E_GP_SEC_NO_SECURITY)
	{
#ifdef GP_COMBO_BASIC_DEVICE

		uint8 u8ZgpsSecLevel = E_GP_NO_SECURITY;

		/* Get the Zgps Security level attribute */
		if(( eZCL_ReadLocalAttributeValue(
				psEndPointDefinition->u8EndPointNumber,
				GREENPOWER_CLUSTER_ID,
				TRUE,
				FALSE,
				FALSE,
				E_CLD_GP_ATTR_ZGPS_SECURITY_LEVEL,
				&u8ZgpsSecLevel)) != E_ZCL_SUCCESS)
		{
			return FALSE;
		}
		else
		{
			u8ZgpsSecLevel = u8ZgpsSecLevel & GP_SECURITY_LEVEL_MASK;
			/* check ZGPD has minimum security support which required by the GP infra device */
			if(u8ZgpsSecLevel > (uint8)psZgpDataIndication->u2SecurityLevel)
			{
				/* Give a callback to user saying that secuity mismatch */
				psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_SECURITY_LEVEL_MISMATCH;
				psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);

				/* drop the packet */
				return FALSE;
			}
		}
#endif
	}
	else
	{
		/* Give a callback to user saying that security processing failed */
		psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_SECURITY_PROCESSING_FAILED;
		psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);

		/* If security processing failed, then drop the packet */
		return FALSE;
	}
	return TRUE;
 }

/****************************************************************************
 **
 ** NAME:       u16GetProxyTableEntrySize
 **
 ** DESCRIPTION:
 ** Get entry size
 **
 ** PARAMETERS:                    Name                           Usage
 ** RETURN:
 ** Length of bytes written
 ****************************************************************************/
uint16 u16GetProxyTableEntrySize( tsGP_ZgppProxySinkTable  *psZgppProxySinkTable)
{
	uint16                                  b16Options;
	uint16                                  u16BytesToWrite = 0;
	b16Options = psZgppProxySinkTable->b16Options;

	u16BytesToWrite += 2;
	if((b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_4_BYTE)
	{
	   //4 byte GPD ID
		u16BytesToWrite += 4;
	}
	#ifdef GP_IEEE_ADDR_SUPPORT
	else
	{
		u16BytesToWrite += 9;
	}
	#endif
	/* check if assigned address is present */
	if(b16Options & GP_PROXY_TABLE_ASSIGNED_ALIAS_MASK)
	{
	   u16BytesToWrite += 2;
	}

	//if security options are available
	if(b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK)
	{
	   //1 byte security options
		u16BytesToWrite += 1;
	}

	/* check if sequence number capability or security is present*/
	if((b16Options & GP_PROXY_TABLE_SEQ_NUM_CAP_MASK)||
			   (b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK))
	{
	   //4 byte security frame counter
		u16BytesToWrite += 4;
	}
	//if security options are available
	if(b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK)
	{
	   teGP_GreenPowerSecLevel    eSecLevel;
	 //  teGP_GreenPowerSecKeyType  eSecKeyType;

	   eSecLevel = psZgppProxySinkTable->b8SecOptions & GP_SECURITY_LEVEL_MASK;
	//   eSecKeyType = (psZgppProxySinkTable->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2;
	   /* check if security key is available */
	   if((eSecLevel != E_GP_NO_SECURITY))
	   {
		   //16 byte security key
		   u16BytesToWrite +=  E_ZCL_KEY_128_SIZE;
	   }
	}

	if(b16Options & GP_PROXY_TABLE_UNICAST_GPS_MASK)
	{
	   uint8 j;
	   u16BytesToWrite += 1 ;//1 byte group list length

	   for(j=0; j< psZgppProxySinkTable->u8NoOfUnicastSink; j++)
	   {
			//8 byte u64SinkIEEEAddress
		   u16BytesToWrite += 8;

		   //2 byte sink group field
		   u16BytesToWrite += 2;
		   //2 byte Alias field

	   }
	}
	/* if communication mode is pre commissioned then add group list also */
	if(b16Options & GP_PROXY_TABLE_COMMISSION_GROUP_GPS_MASK)
	{
	   uint8 j;
	   u16BytesToWrite += 1;//1 byte group list length

	   for(j=0; j< psZgppProxySinkTable->u8SinkGroupListEntries; j++)
	   {
		   //2 byte sink group field
		   u16BytesToWrite += 2;
		   //2 byte Alias field
		   u16BytesToWrite += 2;
	   }
	}

	//1 byte Radius
	u16BytesToWrite += 1;
	/* check if search counter is present*/
	if((!(b16Options & GP_PROXY_TABLE_ENTRY_ACTIVE_MASK))||
			   (!(b16Options & GP_PROXY_TABLE_ENTRY_VALID_MASK)))
	{
		u16BytesToWrite += 1;//1 byte search counter field
	} // Not used know


   return u16BytesToWrite;
}

/****************************************************************************
 **
 ** NAME:       u16GetProxyTableString
 **
 ** DESCRIPTION:
 ** Get Proxy Table to buffer
 **
 ** PARAMETERS:                    Name                           Usage
 ** ZPS_tsAfEvent              	  *pZPSevent              		ZPS event
 ** tsZCL_EndPointDefinition      *psEndPointDefinition         Entry present in proxy table
 ** tsGP_GreenPowerCustomData     *psGpCustomDataStructure       Proxy table
 **
 ** RETURN:
 ** Length of bytes written
 ****************************************************************************/
uint16 u16GetProxyTableString(uint8 *pu8Data, tsGP_ZgppProxySinkTable  *psZgppProxySinkTable)
{
	uint8 *pu8Start                          = pu8Data;
	uint16                                  b16Options;
	b16Options = psZgppProxySinkTable->b16Options;
	*pu8Data++ = (uint8)b16Options;//2 byte options field
	*pu8Data++ = (b16Options >> 8);

	if((b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_4_BYTE)
	{
	   //4 byte GPD ID
	   uint32 u32Data = psZgppProxySinkTable->uZgpdDeviceAddr.u32ZgpdSrcId;
	   *pu8Data++ = (uint8)u32Data;
	   *pu8Data++ = (uint8)(u32Data >> 8);
	   *pu8Data++ = (uint8)(u32Data >> 16);
	   *pu8Data++ = (uint8)(u32Data >> 24);
	}
	#ifdef GP_IEEE_ADDR_SUPPORT
	else
	{
	   uint64 u64Data = psZgppProxySinkTable->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
		//8 byte GPD ID
	   *pu8Data++ = (uint8)(u64Data & 0xff);
	   *pu8Data++ = (uint8)(u64Data >> 8);
	   *pu8Data++ = (uint8)(u64Data >> 16);
	   *pu8Data++ = (uint8)(u64Data >> 24);
	   *pu8Data++ = (uint8)(u64Data >> 32);
	   *pu8Data++ = (uint8)(u64Data >> 40);
	   *pu8Data++ = (uint8)(u64Data >> 48);
	   *pu8Data++ = (uint8)(u64Data >> 56);

	   *pu8Data++ = psZgppProxySinkTable->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
	}
	#endif
	/* check if assigned address is present */
	if(b16Options & GP_PROXY_TABLE_ASSIGNED_ALIAS_MASK)
	{
	   uint16 u16AsignedAlias = psZgppProxySinkTable->u16ZgpdAssignedAlias;
	   *pu8Data++ = (uint8)u16AsignedAlias;//2 byte asigned alias field
	   *pu8Data++ = (u16AsignedAlias >> 8);
	}

	//if security options are available
	if(b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK)
	{
	   //1 byte security options
	   *pu8Data++ = psZgppProxySinkTable->b8SecOptions;
	}

	/* check if sequence number capability or security is present*/
	if((b16Options & GP_PROXY_TABLE_SEQ_NUM_CAP_MASK)||
			   (b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK))
	{
	   //4 byte security frame counter
	   uint32 u32Data = psZgppProxySinkTable->u32ZgpdSecFrameCounter;
	   *pu8Data++ = (uint8)u32Data;
	   *pu8Data++ = (uint8)(u32Data >> 8);
	   *pu8Data++ = (uint8)(u32Data >> 16);
	   *pu8Data++ = (uint8)(u32Data >> 24);
	}
	//if security options are available
	if(b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK)
	{
	   teGP_GreenPowerSecLevel    eSecLevel;
	 //  teGP_GreenPowerSecKeyType  eSecKeyType;

	   eSecLevel = psZgppProxySinkTable->b8SecOptions & GP_SECURITY_LEVEL_MASK;
	//   eSecKeyType = (psZgppProxySinkTable->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2;
	   /* check if security key is available */
	   if((eSecLevel != E_GP_NO_SECURITY))
	   {
		   //16 byte security key
		   memcpy(pu8Data, psZgppProxySinkTable->sZgpdKey.au8Key, E_ZCL_KEY_128_SIZE);
		   pu8Data +=  E_ZCL_KEY_128_SIZE;
	   }
	}

	if(b16Options & GP_PROXY_TABLE_UNICAST_GPS_MASK)
	{
	   uint8 j;
	   *pu8Data++ = psZgppProxySinkTable->u8NoOfUnicastSink;//1 byte group list length

	   for(j=0; j< psZgppProxySinkTable->u8NoOfUnicastSink; j++)
	   {

		   uint64 u64Data =psZgppProxySinkTable->u64SinkUnicastIEEEAddres[j];
			//8 byte u64SinkIEEEAddress
		   *pu8Data++ = (uint8)(u64Data & 0xff);
		   *pu8Data++ = (uint8)(u64Data >> 8);
		   *pu8Data++ = (uint8)(u64Data >> 16);
		   *pu8Data++ = (uint8)(u64Data >> 24);
		   *pu8Data++ = (uint8)(u64Data >> 32);
		   *pu8Data++ = (uint8)(u64Data >> 40);
		   *pu8Data++ = (uint8)(u64Data >> 48);
		   *pu8Data++ = (uint8)(u64Data >> 56);

		   //2 byte sink group field
		   *pu8Data++ = (uint8)psZgppProxySinkTable->u16SinkUnicastNWKAddress[j];
		   *pu8Data++ = (uint8)(psZgppProxySinkTable->u16SinkUnicastNWKAddress[j] >> 8);
		   //2 byte Alias field

	   }
	}
	/* if communication mode is pre commissioned then add group list also */
	if(b16Options & GP_PROXY_TABLE_COMMISSION_GROUP_GPS_MASK)
	{
	   uint8 j;
	   *pu8Data++ = psZgppProxySinkTable->u8SinkGroupListEntries;//1 byte group list length

	   for(j=0; j< psZgppProxySinkTable->u8SinkGroupListEntries; j++)
	   {
		   //2 byte sink group field
		   *pu8Data++ = (uint8)psZgppProxySinkTable->asSinkGroupList[j].u16SinkGroup;
		   *pu8Data++ = (uint8)(psZgppProxySinkTable->asSinkGroupList[j].u16SinkGroup >> 8);
		   //2 byte Alias field
		   *pu8Data++ = (uint8)psZgppProxySinkTable->asSinkGroupList[j].u16Alias;
		   *pu8Data++ = (uint8)(psZgppProxySinkTable->asSinkGroupList[j].u16Alias >> 8);
	   }
	}

	//1 byte Radius
	*pu8Data++ = psZgppProxySinkTable->u8GroupCastRadius;
	/* check if search counter is present*/
	if((!(b16Options & GP_PROXY_TABLE_ENTRY_ACTIVE_MASK))||
			   (!(b16Options & GP_PROXY_TABLE_ENTRY_VALID_MASK)))
	{
	   *pu8Data++ = psZgppProxySinkTable->u8SearchCounter;//1 byte search counter field
	} // Not used know


   return (uint16)(pu8Data - pu8Start);
}
#ifdef GP_COMBO_BASIC_DEVICE
/****************************************************************************
 **
 ** NAME:       u16GetSinkTableString
 **
 ** DESCRIPTION:
 ** Get Sink Table to buffer
 **
 ** PARAMETERS:                    Name                           Usage
 ** ZPS_tsAfEvent              	  *pZPSevent              		ZPS event
 ** tsZCL_EndPointDefinition      *psEndPointDefinition         Entry present in proxy table
 ** tsGP_GreenPowerCustomData     *psGpCustomDataStructure       Proxy table
 **
 ** RETURN:
 ** Length of bytes written
 ****************************************************************************/
uint16 u16GetSinkTableString(uint8 *pu8Data, tsGP_ZgppProxySinkTable  *psZgppProxySinkTable)
{
	 uint16                                  b16Options;
	 uint8 *pu8Start                          = pu8Data;

	/* Option field */
	b16Options = psZgppProxySinkTable->b16Options & GP_APPLICATION_ID_MASK;
	b16Options |=( psZgppProxySinkTable->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK )<< 3;
	b16Options |= psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_SEQ_NUM_CAP_MASK;
	b16Options |= (psZgppProxySinkTable->b8SinkOptions & GP_SINK_TABLE_RX_ON_MASK) << 2;
	b16Options |= (psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_GPD_FIXED_MASK) >> 4;
	b16Options |= (psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_ASSIGNED_ALIAS_MASK )>> 5;
	b16Options |= (psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK )>> 5;

	 DBG_vPrintf(TRACE_GP_DEBUG, "\nb16Options %d, vb8SinkOptions %d, psZgppProxySinkTable->b16Options %d\n", b16Options,
			 psZgppProxySinkTable->b8SinkOptions, psZgppProxySinkTable->b16Options);
	*pu8Data++ = (uint8)b16Options;//2 byte options field
	*pu8Data++ = (b16Options >> 8);
	/* GPD ID */
	if((b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_4_BYTE)
	{
		//4 byte GPD ID
		uint32 u32Data = psZgppProxySinkTable->uZgpdDeviceAddr.u32ZgpdSrcId;
		*pu8Data++ = (uint8)u32Data;
		*pu8Data++ = (uint8)(u32Data >> 8);
		*pu8Data++ = (uint8)(u32Data >> 16);
		*pu8Data++ = (uint8)(u32Data >> 24);
	}
#ifdef GP_IEEE_ADDR_SUPPORT
	else
	{
		uint64 u64Data = psZgppProxySinkTable->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
		 //8 byte GPD ID
		*pu8Data++ = (uint8)(u64Data & 0xff);
		*pu8Data++ = (uint8)(u64Data >> 8);
		*pu8Data++ = (uint8)(u64Data >> 16);
		*pu8Data++ = (uint8)(u64Data >> 24);
		*pu8Data++ = (uint8)(u64Data >> 32);
		*pu8Data++ = (uint8)(u64Data >> 40);
		*pu8Data++ = (uint8)(u64Data >> 48);
		*pu8Data++ = (uint8)(u64Data >> 56);
		/* Endpoint */
		*pu8Data++ = psZgppProxySinkTable->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
	}
#endif

	*pu8Data++ = psZgppProxySinkTable->eZgpdDeviceId; //1 byte Device ID
	/* if communication mode is pre commissioned then add group list also */
	if((psZgppProxySinkTable->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
	{
		uint8 j;
		*pu8Data++ = psZgppProxySinkTable->u8SinkGroupListEntries;//1 byte group list length

		for(j=0; j< psZgppProxySinkTable->u8SinkGroupListEntries; j++)
		{
			//2 byte sink group field
			*pu8Data++ = (uint8)psZgppProxySinkTable->asSinkGroupList[j].u16SinkGroup;
			*pu8Data++ = (uint8)(psZgppProxySinkTable->asSinkGroupList[j].u16SinkGroup >> 8);
			//2 byte Alias field
			*pu8Data++ = (uint8)psZgppProxySinkTable->asSinkGroupList[j].u16Alias;
			*pu8Data++ = (uint8)(psZgppProxySinkTable->asSinkGroupList[j].u16Alias >> 8);
		}
	}
	/* check if assigned address is present */
	if(psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_ASSIGNED_ALIAS_MASK)
	{
		//2 byte GPD Assigned Alias
		*pu8Data++ = (uint8)psZgppProxySinkTable->u16ZgpdAssignedAlias;
		*pu8Data++ = (uint8)(psZgppProxySinkTable->u16ZgpdAssignedAlias >> 8);
	}
	//1 byte Radius
	*pu8Data++ = psZgppProxySinkTable->u8GroupCastRadius;
	//if security options are available
	if(psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK)
	{
		//1 byte security options
		*pu8Data++ = psZgppProxySinkTable->b8SecOptions;
	}
	/* check if sequence number capability or security is present*/
	if((psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_SEQ_NUM_CAP_MASK)||
				(psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK))
	{
		//4 byte security frame counter
		uint32 u32Data = psZgppProxySinkTable->u32ZgpdSecFrameCounter;
		*pu8Data++ = (uint8)u32Data;
		*pu8Data++ = (uint8)(u32Data >> 8);
		*pu8Data++ = (uint8)(u32Data >> 16);
		*pu8Data++ = (uint8)(u32Data >> 24);
	}
	//if security options are available
	if(psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK)
	{
		teGP_GreenPowerSecLevel    eSecLevel;
		teGP_GreenPowerSecKeyType  eSecKeyType;

		eSecLevel = psZgppProxySinkTable->b8SecOptions & GP_SECURITY_LEVEL_MASK;
		eSecKeyType = (psZgppProxySinkTable->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2;
		/* check if security key is available */
		 if((eSecLevel != E_GP_NO_SECURITY)&&(eSecKeyType != E_GP_NO_KEY) )
		{
			//16 byte security key
			memcpy(pu8Data, psZgppProxySinkTable->sZgpdKey.au8Key, E_ZCL_KEY_128_SIZE);
			pu8Data +=  E_ZCL_KEY_128_SIZE;
		}
	}
	return( (uint16)(pu8Data - pu8Start));
}
#endif
/****************************************************************************
 **
 ** NAME:       bGP_CheckGPDAddressMatch
 **
 ** DESCRIPTION:
 ** Checks whether two given addresses are same
 **
 ** PARAMETERS:                    Name                           Usage
 ** uint8 						u8AppIdSrc,
 ** uint8                       u8AppIdDst,
 ** tuGP_ZgpdDeviceAddr          *sAddrSrc,
 ** tuGP_ZgpdDeviceAddr          *sAddrDst
 **
 ** RETURN:
 ** TRUE if address matches, false otherwise
 ****************************************************************************/
bool_t  bGP_CheckGPDAddressMatch(
		uint8 						u8GPTableAppIdSrc,
		uint8                       u8AppIdDst,
		tuGP_ZgpdDeviceAddr          *sGPTableAddrSrc,
		tuGP_ZgpdDeviceAddr          *sAddrDst
)
{


	if(u8GPTableAppIdSrc == u8AppIdDst)
	{
		if(u8GPTableAppIdSrc == GP_APPL_ID_4_BYTE)
		{
			if((sGPTableAddrSrc->u32ZgpdSrcId == sAddrDst->u32ZgpdSrcId)|| (sGPTableAddrSrc->u32ZgpdSrcId == 0xFFFFFFFF))
			{
				return TRUE;
			}
		}
		else
		{
			if((sGPTableAddrSrc->sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr == sAddrDst->sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr) &&
					((sGPTableAddrSrc->sZgpdDeviceAddrAppId2.u8EndPoint == sAddrDst->sZgpdDeviceAddrAppId2.u8EndPoint) ||
											(sGPTableAddrSrc->sZgpdDeviceAddrAppId2.u8EndPoint == E_GP_ALL_ENPOINTS) ||
											(sGPTableAddrSrc->sZgpdDeviceAddrAppId2.u8EndPoint == E_GP_EP_INDEPENDENT)))
			{
				return TRUE;
			}
			if((sGPTableAddrSrc->sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr == 0xFFFFFFFFFFFFFFFF) &&
					((sGPTableAddrSrc->sZgpdDeviceAddrAppId2.u8EndPoint == sAddrDst->sZgpdDeviceAddrAppId2.u8EndPoint) ||
					(sGPTableAddrSrc->sZgpdDeviceAddrAppId2.u8EndPoint == E_GP_ALL_ENPOINTS)||
					(sGPTableAddrSrc->sZgpdDeviceAddrAppId2.u8EndPoint == E_GP_EP_INDEPENDENT)))
			{
				return TRUE;
			}
			if((sGPTableAddrSrc->sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr == sAddrDst->sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr) &&
					((sGPTableAddrSrc->sZgpdDeviceAddrAppId2.u8EndPoint == sAddrDst->sZgpdDeviceAddrAppId2.u8EndPoint) ||
											(sAddrDst->sZgpdDeviceAddrAppId2.u8EndPoint == E_GP_ALL_ENPOINTS)||
											(sAddrDst->sZgpdDeviceAddrAppId2.u8EndPoint == E_GP_EP_INDEPENDENT)))
			{
				return TRUE;
			}
			if((sAddrDst->sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr == 0xFFFFFFFFFFFFFFFF) &&
					((sGPTableAddrSrc->sZgpdDeviceAddrAppId2.u8EndPoint == sAddrDst->sZgpdDeviceAddrAppId2.u8EndPoint) ||
					(sAddrDst->sZgpdDeviceAddrAppId2.u8EndPoint == E_GP_ALL_ENPOINTS)||
					(sAddrDst->sZgpdDeviceAddrAppId2.u8EndPoint == E_GP_EP_INDEPENDENT)))
			{
				return TRUE;
			}
		}

	}

	return FALSE;
}
/****************************************************************************
 **
 ** NAME:       bGP_ValidateComissionCmdFields
 **
 ** DESCRIPTION:
 ** Checks validity of commissioning command received
 **
 ** PARAMETERS:                                  Name                        Usage
 ** tsGP_ZgpDataIndication						psZgpDataIndication
 ** tsZCL_EndPointDefinition                    *psEndPointDefinition,
 ** tsGP_GreenPowerCustomData                   *psGpCustomDataStructure
 **
 ** RETURN:
 ** TRUE if packet valid, FALSE otherwise
 **
 ****************************************************************************/

bool_t bGP_ValidateComissionCmdFields(
		tsGP_ZgpDataIndication               		           *psZgpDataIndication,
		tsZCL_EndPointDefinition    							*psEndPointDefinition,
	    tsGP_GreenPowerCustomData                              *psGpCustomDataStructure)
{
    uint8 u8SecLevel,u8SecKeyType;
    uint8 u8SecLevelRecvd = 0;
    AESSW_Block_u                   uSecurityKey;


    if(bGP_GetSecurityDetails(
        psZgpDataIndication->u2ApplicationId,
                         &u8SecLevel,
                         &u8SecKeyType,
                         &uSecurityKey,
                         &psZgpDataIndication->uZgpdDeviceAddr,
                         psEndPointDefinition,
                         psGpCustomDataStructure) == FALSE)
    {
        return FALSE;
    }

    if(psZgpDataIndication->pu8Pdu[1] & GP_COMM_CMD_EXT_OPT_FIELD_MASK )
    {
        u8SecLevelRecvd =(((uint8)(psZgpDataIndication->pu8Pdu[2] & GP_COMM_CMD_EXT_OPT_SEC_LEVEL_MASK) ));
        if(u8SecLevelRecvd== 1) /* security level deprecated now */
        {
            return FALSE;
        }
    }
    else
    {
        u8SecLevelRecvd = (uint8)(psZgpDataIndication->u2SecurityLevel);
    }
	 /*  sec level validation */
	if((u8SecLevel & GP_COMM_CMD_EXT_OPT_SEC_LEVEL_MASK)> u8SecLevelRecvd)
	{
		/* Give a callback to user saying that secuity mismatch */
		psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_SECURITY_LEVEL_MISMATCH;
		psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
		DBG_vPrintf(TRACE_GP_DEBUG, "\n E_GP_SECURITY_LEVEL_MISMATCH in comm request \n");
		/* drop the packet */
		return FALSE;
	}

	if(psZgpDataIndication->pu8Pdu[1] & GP_COMM_CMD_EXT_OPT_FIELD_MASK)
	{
	    /*  key type  validation*/
	    if(u8SecLevelRecvd > 0)
	    {
	        uint8 u8RecvdSecKeyType = (psZgpDataIndication->pu8Pdu[2] & GP_COMM_CMD_EXT_OPT_KEY_TYPE_MASK)>> 2;


	        if(u8RecvdSecKeyType == E_GP_OUT_OF_THE_BOX_ZGPD_KEY )
	        {
	            /* key should be present */

	            if((psZgpDataIndication->pu8Pdu[2] & GP_COMM_CMD_EXT_OPT_KEY_TYPE_MASK)  == FALSE )
	            return FALSE;
	        }
	        else if((u8RecvdSecKeyType== E_GP_ZIGBEE_NWK_KEY) || (u8RecvdSecKeyType== E_GP_ZGPD_GROUP_KEY) ||
	                (u8RecvdSecKeyType== E_GP_NWK_KEY_DERIVED_ZGPD_GROUP_KEY))
	        {
	            if(u8SecKeyType != u8RecvdSecKeyType)
	             {
	                 return FALSE;
	             }
	        }
	        /* reject if there is no way to establish key */
	        if(((psZgpDataIndication->pu8Pdu[1] & GP_COMM_CMD_ZGP_SEC_KEY_REQ_MASK) == 0) &&
	                 (psZgpDataIndication->pu8Pdu[2] & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_PRESENT_MASK) == 0)
	        {
	            return FALSE;
	        }
	        /* reject frame if sent in clear when attribute configured otherwise */

	        if(GP_SEC_ATTRIB_ENCRYPTION & u8SecLevel)
	        {
	            if(((psZgpDataIndication->pu8Pdu[2] & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_ENC_MASK) == 0) &&
	                     (psZgpDataIndication->pu8Pdu[2] & GP_COMM_CMD_EXT_OPT_ZGPD_KEY_PRESENT_MASK))
	            {
	                return FALSE;
	            }
	        }

	    }

	}


	return TRUE;
}


/****************************************************************************
 **
 ** NAME:       bIsInvolveTC
 **
 ** DESCRIPTION:
 ** Checks if invi=olve TC bity is set
 **
 ** PARAMETERS:                                  Name                        Usage
 ** uint8									 u8EndPointNumber
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/
 bool_t bIsInvolveTC(uint8 u8EndPointNumber)
{
	 uint8 u8SecLevel ;

#ifdef GP_COMBO_BASIC_DEVICE
	 bool_t bIsServer = TRUE;
#else
	 bool_t bIsServer = FALSE;
#endif
    if(eZCL_ReadLocalAttributeValue(
    		         u8EndPointNumber,
    				 GREENPOWER_CLUSTER_ID,
    				 bIsServer,
    				 FALSE,
                     !bIsServer,
                     E_CLD_GP_ATTR_ZGPS_SECURITY_LEVEL,
                     &u8SecLevel) != E_ZCL_SUCCESS)
	 {
			DBG_vPrintf(TRACE_GP_DEBUG, "\n E_CLD_GP_ATTR_ZGPS_SECURITY_LEVEL read failed \n");

		return FALSE;
	 }
    if(u8SecLevel & GP_ATTRIB_INVOLVE_TC_BITS )
    {
    	return TRUE;
    }
    return FALSE;
}
#ifdef GP_COMBO_BASIC_DEVICE
/****************************************************************************
 **
 ** NAME:       bGP_GetSecurityDetails
 **
 ** DESCRIPTION:
 ** Transmits Commissioning notification commands buffered
 **
 ** PARAMETERS:                                  Name                        Usage
 ** uint8									      u8AppId,
 ** uint8										  *pu8SecLevel,
 ** uint8										  *pu8KeyType,
 ** uint8										  *pu8Key,
 ** tuGP_ZgpdDeviceAddr                          *puZgpdDeviceAddr,
 ** tsZCL_EndPointDefinition                     *psEndPointDefinition,
 ** tsGP_GreenPowerCustomData                    *psGpCustomDataStructure
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/
bool_t bGP_GetSecurityDetails(
		 uint8									      u8AppId,
		 uint8										  *pu8SecLevel,
		 uint8										  *pu8KeyType,
		 AESSW_Block_u                                *pu8Key,
		 tuGP_ZgpdDeviceAddr                          *puZgpdDeviceAddr,
		 tsZCL_EndPointDefinition                     *psEndPointDefinition,
		 tsGP_GreenPowerCustomData                    *psGpCustomDataStructure)
{
	 uint8 u8ZgpSecKeyType ;
	 AESSW_Block_u                   sKey;
	 tsGP_ZgppProxySinkTable                *psSinkTableEntry;
	 uint8 u8SinkTableKeyType = 0;
#ifdef GP_COMBO_BASIC_DEVICE
	 bool_t bIsServer = TRUE;
#else
	 bool_t bIsServer = FALSE;
#endif
    if(eZCL_ReadLocalAttributeValue(
    				 psEndPointDefinition->u8EndPointNumber,
    				 GREENPOWER_CLUSTER_ID,
    				 bIsServer,
    				 FALSE,
                     !bIsServer,
                     E_CLD_GP_ATTR_ZGPS_SECURITY_LEVEL,
                     pu8SecLevel) != E_ZCL_SUCCESS)
	 {
			DBG_vPrintf(TRACE_GP_DEBUG, "\n E_CLD_GP_ATTR_ZGPS_SECURITY_LEVEL read failed \n");

		return FALSE;
	 }

	 if(eZCL_ReadLocalAttributeValue(
			 psEndPointDefinition->u8EndPointNumber,
			 GREENPOWER_CLUSTER_ID,
			 bIsServer,
			 FALSE,
			!bIsServer,
			 E_CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE,
			 &u8ZgpSecKeyType) != E_ZCL_SUCCESS)
	 {
		 *pu8KeyType = E_GP_NO_KEY;
	 }
	 else
	 {
		 *pu8KeyType = u8ZgpSecKeyType;
	 }
	 DBG_vPrintf(TRACE_GP_DEBUG, "\n  *pu8KeyType %d \n",  *pu8KeyType);
	 if(*pu8KeyType == E_GP_ZGPD_GROUP_KEY)
	  {
#ifdef CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY
		 // get EP mutex
		  #ifndef COOPERATIVE
			  eZCL_GetMutex(psEndPointDefinition);
		  #endif

		  /* Get key */
		  eZCL_ReadLocalAttributeValue(
				   psEndPointDefinition->u8EndPointNumber,
				   GREENPOWER_CLUSTER_ID,
				   TRUE,
				   FALSE,
				   FALSE,
				   E_CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY,
				   (uint8*)pu8Key);

		  // release mutex
		  #ifndef COOPERATIVE
			  eZCL_ReleaseMutex(psEndPointDefinition);
		  #endif

            memcpy(&sKey, pu8Key, sizeof(AESSW_Block_u));

#else
		  // fill in callback event structure
		  eZCL_SetCustomCallBackEvent(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
										0, 0, psEndPointDefinition->u8EndPointNumber);
		   /* Set Event Type */
		  psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_SHARED_SECURITY_KEY_IS_NOT_ENABLED;
		   /* Give a call back event to app */
		  psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
		  DBG_vPrintf(TRACE_GP_DEBUG, "\n  **pu8KeyType == E_GP_ZGPD_GROUP_KEY \n");
		  return FALSE;
#endif
	  }


	 if((*pu8KeyType == E_GP_DERIVED_INDIVIDUAL_ZGPD_KEY) || (*pu8KeyType == E_GP_OUT_OF_THE_BOX_ZGPD_KEY) || (*pu8KeyType == E_GP_NO_KEY))
	 {


		 if(bGP_IsSinkTableEntryPresent(
				 psEndPointDefinition->u8EndPointNumber,
				 u8AppId,
				 puZgpdDeviceAddr,
		         &psSinkTableEntry,
		         0xFF) == TRUE)
		 {
			 u8SinkTableKeyType = (psSinkTableEntry->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2;
			  if(*pu8KeyType == E_GP_NO_KEY)
			  {
				  *pu8KeyType = u8SinkTableKeyType;
			  }
			  if(*pu8KeyType != E_GP_NO_KEY)
			  {
                 memcpy(&sKey, &psSinkTableEntry->sZgpdKey, sizeof(AESSW_Block_u));
				 memcpy(&pu8Key->au8[0],&sKey.au8[0],E_ZCL_KEY_128_SIZE);
			  }
		 }
		 else
		 {
			 *pu8KeyType = E_GP_NO_KEY;
			 DBG_vPrintf(TRACE_GP_DEBUG, "\n *pu8KeyType = E_GP_NO_KEY \n");
		 }
	 }
	  if(((*pu8KeyType == E_GP_DERIVED_INDIVIDUAL_ZGPD_KEY) && (u8SinkTableKeyType == E_GP_OUT_OF_THE_BOX_ZGPD_KEY)) ||
						(*pu8KeyType == E_GP_ZIGBEE_NWK_KEY) ||
						(*pu8KeyType == E_GP_NWK_KEY_DERIVED_ZGPD_GROUP_KEY) )

	 {
			DBG_vPrintf(TRACE_GP_DEBUG, "\n Printing Key before ZPS_vZgpTransformKey \n");
			{
				uint8 i;
				for(i=0; i< 16; i++)
				{
					DBG_vPrintf(TRACE_GP_DEBUG, "%x ",pu8Key->au8[i] );
				}
			}

	   /* Get the derived individual key */
	   ZPS_vZgpTransformKey(*pu8KeyType,
			   u8AppId,
			   puZgpdDeviceAddr->u32ZgpdSrcId,
			   puZgpdDeviceAddr->sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr,
			   &sKey,
			   pu8Key);
	 }
	  if((*pu8KeyType == E_GP_DERIVED_INDIVIDUAL_ZGPD_KEY) && (u8SinkTableKeyType == E_GP_OUT_OF_THE_BOX_ZGPD_KEY))
	  {
		  psSinkTableEntry->b8SecOptions &= ~GP_SECURITY_KEY_TYPE_MASK;
		  psSinkTableEntry->b8SecOptions |= E_GP_DERIVED_INDIVIDUAL_ZGPD_KEY << 2;
		  memcpy(&psSinkTableEntry->sZgpdKey.au8Key,pu8Key,E_ZCL_KEY_128_SIZE);
		  DBG_vPrintf(TRACE_GP_DEBUG, "\n Overwrote sink table key \n");
	  }
	DBG_vPrintf(TRACE_GP_DEBUG, "\n Printing Key \n");
	{
		uint8 i;
		for(i=0; i< 16; i++)
		{
			DBG_vPrintf(TRACE_GP_DEBUG, "%x ",pu8Key->au8[i] );
		}
	}
	DBG_vPrintf(TRACE_GP_DEBUG, "\n");
	return TRUE;

}
#endif
/****************************************************************************
 **
 ** NAME:       vBackUpTxQueueMsg
 **
 ** DESCRIPTION:
 ** Backs up a Tx queue message to be transmitted when the TxQueue sends out the message
 **
 ** PARAMETERS:                                  Name                        Usage
 ** uint8                                       u8Length,
 ** uint8                                      *pu8Data,
 ** uint8			                           u8CmdId,
 **ZPS_tsAfZgpGreenPowerReq                   *psZgpDataReq
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/
void vBackUpTxQueueMsg(
		            teGP_GreenPowerBufferedCommands            eGreenPowerBufferedCommand,
					 tsGP_ZgpResponseCmdPayload                  *psZgpResponseCmdPayload,
					ZPS_tsAfZgpGreenPowerReq                   *psZgpDataReq,
					tsGP_GreenPowerCustomData                   *psGpCustomDataStructure)
{
	tsGP_ZgpDataIndication   sZgpDataIndication;
    memset(&sZgpDataIndication,0,sizeof(sZgpDataIndication));
	sZgpDataIndication.u2ApplicationId =psZgpDataReq->u8ApplicationId;
	if(psZgpDataReq->u8ApplicationId)
	{
		sZgpDataIndication.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr = psZgpDataReq->uGpId.u64Address ;
		sZgpDataIndication.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint = psZgpDataReq->u8Endpoint ;
	}
	else
	{
		sZgpDataIndication.uZgpdDeviceAddr.u32ZgpdSrcId = psZgpDataReq->uGpId.u32SrcId ;
	}
	sZgpDataIndication.u8PDUSize =psZgpResponseCmdPayload->sZgpdCommandPayload.u8Length;
	sZgpDataIndication.pu8Pdu = psZgpResponseCmdPayload->sZgpdCommandPayload.pu8Data;


	sZgpDataIndication.u8CommandId =(uint8)psZgpResponseCmdPayload->eZgpdCmdId;
	if(eGreenPowerBufferedCommand == E_GP_ADD_MSG_TO_TX_QUEUE)
	{
		/* Use status to indicate whether the packet transmitted from TxQueue */
		sZgpDataIndication.u8Status = E_ZCL_FAIL;
	}
	else
	{
		/* Use status to indicate whether a channel change required*/
		if(psZgpResponseCmdPayload->eZgpdCmdId == E_GP_CHANNEL_CONFIGURATION)
		{
			psGpCustomDataStructure->u8OperationalChannel = ZPS_u8AplZdoGetRadioChannel();
			if( psGpCustomDataStructure->u8OperationalChannel != ((psZgpResponseCmdPayload->b8TempMasterTxChannel & 0x0F) + 11))
			{
				/* Use u8TranslationIndex to hold transit channel  */
				sZgpDataIndication.u8TranslationIndex = (psZgpResponseCmdPayload->b8TempMasterTxChannel & 0x0F) + 11;
				sZgpDataIndication.u8Status = 1; //indicate that channel change required
			}

		}
	}

	/* Send Pairing Information as Broadcast, Buffer the packet as it is broadcast transmission */
	eGp_BufferTransmissionPacket(
						&sZgpDataIndication,
						eGreenPowerBufferedCommand,
						psGpCustomDataStructure);
}
/****************************************************************************
 **
 ** NAME:       vSendToTxQueue
 **
 ** DESCRIPTION:
 ** Backs up a Tx queue message to be transmitted when the TxQueue sends out the message
 **
 ** PARAMETERS:                                  Name                        Usage
 ** tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/
void vSendToTxQueue(		tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
		tsGP_GreenPowerCustomData           *psGreenPowerCustomData)
{
	uint8 										*pu8Start;
    uint8 										eZPSStatus,i;
    PDUM_thAPduInstance                         hAPduInst;
	ZPS_tsAfZgpGreenPowerReq                    sZgpDataReq;
	memset(&sZgpDataReq,0,sizeof(sZgpDataReq));
	hAPduInst = hZCL_AllocateAPduInstance();
	if(hAPduInst == PDUM_INVALID_HANDLE)
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "\n vSendToTxQueue E_ZCL_ERR_ZBUFFER_FAIL \r\n");

		return;
	}
	pu8Start = (uint8 *)(PDUM_pvAPduInstanceGetPayload(hAPduInst));

	for(i = 0; i<psZgpBufferedApduRecord->sBufferedApduInfo.u8PDUSize;i++)
	{
		*pu8Start++ = psZgpBufferedApduRecord->sBufferedApduInfo.au8PDU[i];
	}
	PDUM_eAPduInstanceSetPayloadSize(hAPduInst, psZgpBufferedApduRecord->sBufferedApduInfo.u8PDUSize);
	sZgpDataReq.u8TxOptions = 0x01;
	sZgpDataReq.u16TxQueueEntryLifetime = 5000; //5 sec
	sZgpDataReq.u8Handle = u8GP_GetDataReqHandle(psGreenPowerCustomData);
	sZgpDataReq.u8SeqNum = u8GetTransactionSequenceNumber();
	sZgpDataReq.u16Panid = 0xFFFF;
	sZgpDataReq.u16DstAddr = 0xFFFF;
	sZgpDataReq.u8ApplicationId = (uint8)psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId;
	if(sZgpDataReq.u8ApplicationId)
	{
		sZgpDataReq.uGpId.u64Address = psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
		sZgpDataReq.u8Endpoint = psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
	}
	else
	{
		sZgpDataReq.uGpId.u32SrcId = psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.u32ZgpdSrcId;
	}
	 if((psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId == E_GP_CHANNEL_CONFIGURATION)&&
	    				(sZgpDataReq.uGpId.u32SrcId == 0)&&
	    				(sZgpDataReq.u8ApplicationId == GP_APPL_ID_4_BYTE))
	 {
		 sZgpDataReq.u8TxOptions |= ( ((uint8)3) << 3); // set maintenance bit
	 }
	eZPSStatus = zps_vDStub_DataReq(&sZgpDataReq, hAPduInst,TRUE,psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId);
	DBG_vPrintf(TRACE_GP_DEBUG, "\n  zps_vDStub_DataReq after timeout :%d\r\n", eZPSStatus);
}
#ifdef GP_COMBO_BASIC_DEVICE
/****************************************************************************
 **
 ** NAME:       bGP_IsGPDPresent
 **
 ** DESCRIPTION:
 ** Checks if Proxy Table entry present for the given GPD
 **
 ** PARAMETERS:                         Name                            Usage
 ** uint8                                  u8GpEndPointId,
 **  bool								   bIsServer,
 ** uint8                                  u8ApplicationId,
 ** tuGP_ZgpdDeviceAddr                    *puZgpdAddress,
 ** tsGP_ZgppProxySinkTable                **psProxySinkTableEntry
 **
 ** RETURN:
 ** TRUE if present, FALSE otherwise
 **
 ****************************************************************************/

PRIVATE bool_t bGP_IsGPDPresent(
                    uint8                                  u8GpEndPointId,
                    bool								   bIsServer,
                    uint8                                  u8ApplicationId,
                    tuGP_ZgpdDeviceAddr                    *puZgpdAddress,
                    tsGP_ZgppProxySinkTable                **psProxySinkTableEntry)

{
    tsZCL_EndPointDefinition                *psEndPointDefinition;
    tsZCL_ClusterInstance                   *psClusterInstance;
    tsGP_GreenPowerCustomData               *psGpCustomDataStructure;
    uint8                   				u8Status;
    uint8                                   i;
    bool_t                                  bZgpIdMatch = FALSE;


    /* Check pointers */
    if((puZgpdAddress == NULL) )
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, "\nbIsSinkTableEntryPresent puZgpdAddress NULL\n");
        return FALSE;
    }
   //find GP cluster
   if((u8Status = eGP_FindGpCluster(
            u8GpEndPointId,
            bIsServer,
            &psEndPointDefinition,
            &psClusterInstance,
            &psGpCustomDataStructure)) != E_ZCL_SUCCESS)
    {
	   DBG_vPrintf(TRACE_GP_DEBUG, "\nbIsSinkTableEntryPresent eGP_FindGpCluster FALSE \n");
        return FALSE;
    }

   // get EP mutex
   #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
   #endif


    for(i = 0; i< GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES; i++)
    {   /* check if sink table entry is not empty */
    	DBG_vPrintf(TRACE_GP_DEBUG, " bGP_IsGPDPresent appId1 =%d appid 2 = %d Address = 0x%08x, Address 2 = 0x%08x \n",
    	    				 ((uint8)(psGpCustomDataStructure->asZgpsSinkProxyTable[i].b16Options & GP_APPLICATION_ID_MASK)),
    	    				 u8ApplicationId,
    	    				     		        			psGpCustomDataStructure->asZgpsSinkProxyTable[i].uZgpdDeviceAddr.u32ZgpdSrcId,
    	    				     		        			puZgpdAddress->u32ZgpdSrcId);
		/* check if application id and address is same */
		if( bGP_CheckGPDAddressMatch(((uint8)(psGpCustomDataStructure->asZgpsSinkProxyTable[i].b16Options & GP_APPLICATION_ID_MASK)),
							u8ApplicationId,
							&psGpCustomDataStructure->asZgpsSinkProxyTable[i].uZgpdDeviceAddr,
							puZgpdAddress
							))
		{
			bZgpIdMatch = TRUE;
			*psProxySinkTableEntry = &psGpCustomDataStructure->asZgpsSinkProxyTable[i];
			break;
		}
	}

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return bZgpIdMatch;
}
#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
