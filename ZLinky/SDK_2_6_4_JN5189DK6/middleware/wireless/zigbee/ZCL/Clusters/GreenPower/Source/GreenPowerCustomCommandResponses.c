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
 * MODULE:             ZGP custom command handler functions
 *
 * COMPONENT:          GreenPowerCustomCommandResponses.c
 *
 * DESCRIPTION:        ZGP custom command handler functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "zps_apl_af.h"
#include "zcl.h"
#include "zcl_customcommand.h"
#include "GreenPower.h"
#include "GreenPower_internal.h"
#include "dbg.h"
#include "portmacro.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef TRACE_GP_DEBUG
#define TRACE_GP_DEBUG FALSE
#endif
#define LQI_MASK            0xC0
#define RSSI_MASK           0x3F

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/


enum
{
	LINK_MORE,
	LINK_LESS,
	LINK_EQUAL
};
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/


void vFillDataIndicationFromNotif(
		tsGP_ZgpNotificationCmdPayload  		            *psZgpNotificationCmdPayload,
		 tsGP_ZgpDataIndication                      		*psZgpDataIndication
		);
bool_t bGP_CheckIfCmdToBeBuffered(
		tsGP_GreenPowerCustomData                           *psGpCustomDataStructure,
		tsGP_ZgpCommissioningNotificationCmdPayload         *psZgpCommissioningNotificationCmdPayload);
void vGP_SendResponseGPDF(
		bool_t                                         bAddMsg,
		 tsZCL_EndPointDefinition                    *psEndPointDefinition,
		 tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
		  tsGP_ZgpResponseCmdPayload                  *psZgpResponseCmdPayload);
bool_t bGP_IsPairingCmdValid( tsGP_ZgpPairingCmdPayload        *psZgpPairingCmdPayload);
uint8 bBestGPD_GPPLink(
		uint8                                                 u8RecevdLinkVal,
		uint8                                                 u8BufferedLinkVal);
void vGetRSSIAndLQIFromGPPGPDLink( tsGP_ZgpDataIndication    *psZgpDataIndication);
bool_t bValidityCheckPairingConfig(
		 ZPS_tsAfEvent                  				  *pZPSevent,
		tsGP_ZgpPairingConfigCmdPayload                   *psZgpPairingConfigPayload,
		tsCLD_GreenPower                                  *pAttribData);
void vFillAndSendPairingCmdFromSinkTable(uint8      u8EndPoint,
		uint8                                      u8GroupIdIndex,
		 uint8                                      u8AddSinkTableMask,
		tsGP_ZgppProxySinkTable                      *psSinkTableEntry);
void vSendPairingRemoveGroup(
		tsZCL_EndPointDefinition                   *psEndPointDefinition,
		tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
		tsGP_ZgpPairingConfigCmdPayload            *psZgpPairingConfigPayload,
		tsGP_ZgppProxySinkTable                     *psSinkTableEntry);
void vGiveIndicationForTranslationTableUpdate(
		teGP_PairingConfigTranslationTableAction                         eTransTableAction,
		tsZCL_EndPointDefinition                                        *psEndPointDefinition,
		tsGP_GreenPowerCustomData                                       *psGpCustomDataStructure,
		tsGP_ZgpPairingConfigCmdPayload                                 *psZgpPairingConfigPayload);

uint8  u8AddGroupIdToSinkTable(uint16                                                        u16GroupId,
							uint16                                                           u16Alias,
							tsGP_ZgppProxySinkTable                                         *psSinkTableEntry);
void vSendpairing(uint8                                                 u8EndPointNumber ,
		          bool_t                                                    bdeviceAnnce,
		          tsGP_ZgpPairingConfigCmdPayload                       *psZgpPairingConfigPayload,
		          tsGP_ZgppProxySinkTable                                *psSinkTableEntry);
void vRemoveGroupAndSinkTableOnReplace(
		tsZCL_EndPointDefinition                   *psEndPointDefinition,
		tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
		tsGP_ZgpPairingConfigCmdPayload            *psZgpPairingConfigPayload,
		tsGP_ZgppProxySinkTable                     *psSinkTableEntry);
bool_t bGP_CheckIfCmdToBeNotified(
		tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
		tsGP_ZgpDataIndication                       *psZgpDataIndication);

bool_t bCheckIfAtleastOneGroupSupported(tsGP_ZgpPairingConfigCmdPayload                       *psZgpPairingConfigPayload);
bool_t bIsNotificationValid(tsGP_ZgpNotificationCmdPayload *psNotificationCmdPyld,
		tsZCL_EndPointDefinition       *psEndPointDefinition,
		 tsZCL_ClusterInstance          *psClusterInstance,
		ZPS_tsAfEvent                  *pZPSevent);
bool_t bDecryptGPDFData(tsGP_ZgpCommissioningNotificationCmdPayload *psZgpCommissioningNotificationCmdPayload,
		tsZCL_EndPointDefinition             *psEndPointDefinition,
	    tsGP_GreenPowerCustomData            *psGpCustomDataStructure);
#ifdef GP_COMBO_BASIC_DEVICE
void bGP_RemoveDuplicateGPResponse(
		tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
		tsGP_ZgpResponseCmdPayload                  *psZgpResponseCmdPayload );
#endif
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/
#ifdef GP_COMBO_BASIC_DEVICE
/****************************************************************************
 **
 ** NAME:       eGP_HandleSinkTableRequest
 **
 ** DESCRIPTION:
 ** Handles Green power Sink Table request command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status  eGP_HandleSinkTableRequest(
		ZPS_tsAfEvent                  *pZPSevent,
        tsZCL_EndPointDefinition       *psEndPointDefinition,
        tsZCL_ClusterInstance          *psClusterInstance,
        uint16                         u16Offset)
{
	uint8 i,u8Index = 0;
	bool_t bIsRequestByAddr;
	tsGP_GreenPowerCustomData                   *psGPCustomDataStructure;
	tsGP_ZgpSinkTableRequestCmdPayload           sSinkTableRequestCmdPayload;
	teZCL_Status                                eStatus = E_ZCL_SUCCESS;
	tsZCL_Address                               sDestAddress = {E_ZCL_AM_SHORT,  {pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr}};
	tsGP_SinkTableRespCmdPayload                sSinkTableRespCmdPayload = {0, 0, 0xff, 0,0 } ;
	uint8 u8SinkTableEntries[MAX_SINK_TABLE_ENTRIES_LENGTH];

	// initialise pointer
	psGPCustomDataStructure = (tsGP_GreenPowerCustomData *)psClusterInstance->pvEndPointCustomStructPtr;
	sSinkTableRespCmdPayload.puSinkTableEntries = u8SinkTableEntries;
	// get EP mutex
	#ifndef COOPERATIVE
		eZCL_GetMutex(psEndPointDefinition);
	#endif


	// fill in callback event structure
	eZCL_SetCustomCallBackEvent(&psGPCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
				pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);
	// get information
	if((eStatus = eGP_SinkTableRequestReceive(
									pZPSevent,
									u16Offset,
									&sSinkTableRequestCmdPayload)) != E_ZCL_SUCCESS)
	{
		// release mutex
		#ifndef COOPERATIVE
			eZCL_ReleaseMutex(psEndPointDefinition);
		#endif


		return eStatus;
	}
	bIsRequestByAddr = (sSinkTableRequestCmdPayload.b8Options & BIT_MAP_REQUEST_TYPE)? 0:1;
	if(bIsRequestByAddr)
	{
		sSinkTableRespCmdPayload.u8StartIndex = 0xFF;
		i=0;
	}
	else
	{
		sSinkTableRespCmdPayload.u8StartIndex = sSinkTableRequestCmdPayload.u8Index;
		i = sSinkTableRespCmdPayload.u8StartIndex;
	}

	sSinkTableRespCmdPayload.u8EntriesCount = 0;
	sSinkTableRespCmdPayload.u16SizeOfSinkTableEntries =
			u16GP_GetStringSizeOfSinkTable(psEndPointDefinition->u8EndPointNumber, &sSinkTableRespCmdPayload.u8TotalNoOfEntries, NULL);
	sSinkTableRespCmdPayload.u8Status = E_ZCL_CMDS_NOT_FOUND;
	if((sSinkTableRespCmdPayload.u8TotalNoOfEntries > sSinkTableRequestCmdPayload.u8Index) ||
			(sSinkTableRespCmdPayload.u16SizeOfSinkTableEntries != 0))
	{
		sSinkTableRespCmdPayload.u16SizeOfSinkTableEntries = 0;
		for( ; i< GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES; i++)
		{
			/* check for entry */
			if(psGPCustomDataStructure->asZgpsSinkProxyTable[i].eGreenPowerSinkTablePriority != 0)
			{
				if(bIsRequestByAddr)
				{
					if(bGP_CheckGPDAddressMatch((psGPCustomDataStructure->asZgpsSinkProxyTable[i].b16Options & GP_APPLICATION_ID_MASK),
							(sSinkTableRequestCmdPayload.b8Options &GP_APPLICATION_ID_MASK),
							&(psGPCustomDataStructure->asZgpsSinkProxyTable[i].uZgpdDeviceAddr),
							&sSinkTableRequestCmdPayload.uZgpdDeviceAddr
							) == FALSE)
					{
						/* The address may be set to 0xFFFFF in Sink table address request . To check this ,
						 * check address match with sink table request as table address 					*/
						if(bGP_CheckGPDAddressMatch(
								(sSinkTableRequestCmdPayload.b8Options &GP_APPLICATION_ID_MASK),
								(psGPCustomDataStructure->asZgpsSinkProxyTable[i].b16Options & GP_APPLICATION_ID_MASK),
								&sSinkTableRequestCmdPayload.uZgpdDeviceAddr,
								&(psGPCustomDataStructure->asZgpsSinkProxyTable[i].uZgpdDeviceAddr)
								) == FALSE)
						{
							/* ignore this entry */
							continue;
						}
					}
				}
				sSinkTableRespCmdPayload.u8Status = E_ZCL_SUCCESS;
				sSinkTableRespCmdPayload.u8EntriesCount++;
				sSinkTableRespCmdPayload.u16SizeOfSinkTableEntries +=
						u16GetSinkTableString(&sSinkTableRespCmdPayload.puSinkTableEntries[u8Index],
								&psGPCustomDataStructure->asZgpsSinkProxyTable[i]);
				u8Index += sSinkTableRespCmdPayload.u16SizeOfSinkTableEntries;
			}
		}
	}

	eStatus = eGP_SinkTableResponseSend(
			 	 	 	 psEndPointDefinition->u8EndPointNumber,
			             ZCL_GP_PROXY_ENDPOINT_ID,
	                      &sDestAddress,
	                      &sSinkTableRespCmdPayload);


	return eStatus;
}
#endif
/****************************************************************************
 **
 ** NAME:       eGP_HandleSinkTableResponse
 **
 ** DESCRIPTION:
 ** Handles Green power Sink Table response command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eGP_HandleSinkTableResponse(
                    ZPS_tsAfEvent                  *pZPSevent,
                    tsZCL_EndPointDefinition       *psEndPointDefinition,
                    tsZCL_ClusterInstance          *psClusterInstance,
                    uint16							u16Offset)
{

	tsGP_GreenPowerCustomData                   *psGPCustomDataStructure;
	tsGP_SinkTableRespCmdPayload                sZgpSinkTableRespCmdPayload;
	teZCL_Status                                eStatus = E_ZCL_SUCCESS;
	uint8 u8SinkTableEntries[MAX_SINK_TABLE_ENTRIES_LENGTH];
	// initialise pointer
	psGPCustomDataStructure = (tsGP_GreenPowerCustomData *)psClusterInstance->pvEndPointCustomStructPtr;

	// get EP mutex
	#ifndef COOPERATIVE
	eZCL_GetMutex(psEndPointDefinition);
	#endif

	DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_HandleSinkTableResponse =%d\n",u16Offset);
	// fill in callback event structure
	eZCL_SetCustomCallBackEvent(&psGPCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
			pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);

	// get information
	sZgpSinkTableRespCmdPayload.puSinkTableEntries = u8SinkTableEntries;
	DBG_vPrintf(TRACE_GP_DEBUG, "\n calling  =%d\n",u16Offset);
	if((eStatus = eGP_SinkTableResponseReceive(pZPSevent,u16Offset, &sZgpSinkTableRespCmdPayload)) != E_ZCL_SUCCESS)
	{
		// release mutex
		#ifndef COOPERATIVE
			eZCL_ReleaseMutex(psEndPointDefinition);
		#endif

		return eStatus;
	}
	DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_SinkTableResponseReceive  Success \n");
	/* give the event to application */
	 psGPCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_ZGPD_SINK_TABLE_RESPONSE_RCVD;
	 psGPCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psZgpSinkTableRespCmdPayload =
    		 &sZgpSinkTableRespCmdPayload;
	psEndPointDefinition->pCallBackFunctions(&psGPCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
	return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eGP_HandleProxyTableRequest
 **
 ** DESCRIPTION:
 ** Handles Green power Proxy Table request command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status  eGP_HandleProxyTableRequest(
		ZPS_tsAfEvent                  *pZPSevent,
        tsZCL_EndPointDefinition       *psEndPointDefinition,
        tsZCL_ClusterInstance          *psClusterInstance,
        uint16                         u16Offset)
{
	uint8 i,  u8Index=0;
	bool_t bIsRequestByAddr;
	tsGP_GreenPowerCustomData                   *psGPCustomDataStructure;
	tsGP_ZgpProxyTableRequestCmdPayload           sProxyTableRequestCmdPayload;
	teZCL_Status                                eStatus = E_ZCL_SUCCESS;
	tsZCL_Address                               sDestAddress = {E_ZCL_AM_SHORT,  {pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr}};
	tsGP_ProxyTableRespCmdPayload                sProxyTableRespCmdPayload = {0, 0, 0xff, 0,0 } ;
	uint8 u8ProxyTableEntries[MAX_SINK_TABLE_ENTRIES_LENGTH];
    uint16 u16SizeFits = MAX_SINK_TABLE_ENTRIES_LENGTH;
    // initialise pointer
	psGPCustomDataStructure = (tsGP_GreenPowerCustomData *)psClusterInstance->pvEndPointCustomStructPtr;
	sProxyTableRespCmdPayload.puProxyTableEntries = u8ProxyTableEntries;
	// get EP mutex
	#ifndef COOPERATIVE
		eZCL_GetMutex(psEndPointDefinition);
	#endif


	// fill in callback event structure
	eZCL_SetCustomCallBackEvent(&psGPCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
				pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);

	if((eStatus = eGP_ProxyTableRequestReceive(
									pZPSevent,
									u16Offset,
									&sProxyTableRequestCmdPayload)) != E_ZCL_SUCCESS)
	{
		// release mutex
		#ifndef COOPERATIVE
			eZCL_ReleaseMutex(psEndPointDefinition);
		#endif


		return eStatus;
	}
	bIsRequestByAddr = (sProxyTableRequestCmdPayload.b8Options & BIT_MAP_REQUEST_TYPE)? 0:1;
	if(bIsRequestByAddr)
	{
		i=0;
		sProxyTableRespCmdPayload.u8StartIndex = 0xFF;
		DBG_vPrintf(TRACE_GP_DEBUG, "\n bIsRequestByAddr \n");
	}
	else
	{
		sProxyTableRespCmdPayload.u8StartIndex = sProxyTableRequestCmdPayload.u8Index;
		i = sProxyTableRespCmdPayload.u8StartIndex;
		DBG_vPrintf(TRACE_GP_DEBUG, "\n bIsRequestByIndex \n");
	}
	sProxyTableRespCmdPayload.u16SizeOfProxyTableEntries =
			u16GP_GetStringSizeOfProxyTable(psEndPointDefinition->u8EndPointNumber, &sProxyTableRespCmdPayload.u8TotalNoOfEntries, NULL);
	sProxyTableRespCmdPayload.u8Status = E_ZCL_CMDS_NOT_FOUND;

	sProxyTableRespCmdPayload.u8EntriesCount = 0;


	if(((sProxyTableRespCmdPayload.u8TotalNoOfEntries >= sProxyTableRequestCmdPayload.u8Index)
			&& (sProxyTableRespCmdPayload.u8StartIndex != 0xff))||
			(sProxyTableRespCmdPayload.u16SizeOfProxyTableEntries != 0))
	{
		sProxyTableRespCmdPayload.u16SizeOfProxyTableEntries = 0;

		for( ; i< GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES; i++)
		{
			/* check for entry */

			if(psGPCustomDataStructure->asZgpsSinkProxyTable[i].bProxyTableEntryOccupied)
			{
				DBG_vPrintf(TRACE_GP_DEBUG, "\n psGPCustomDataStructure->asZgpsSinkProxyTable[i].bProxyTableEntryOccupied = %d \n",i);
				if(bIsRequestByAddr)
				{
					if(bGP_CheckGPDAddressMatch((psGPCustomDataStructure->asZgpsSinkProxyTable[i].b16Options & GP_APPLICATION_ID_MASK),
							(sProxyTableRequestCmdPayload.b8Options &GP_APPLICATION_ID_MASK),
							&(psGPCustomDataStructure->asZgpsSinkProxyTable[i].uZgpdDeviceAddr),
							&sProxyTableRequestCmdPayload.uZgpdDeviceAddr
							) == FALSE)
					{
						/* The address may be set to 0xFFFFF in Sink table address request . To check this ,
						 * check address match with sink table request as table address 					*/
						if(bGP_CheckGPDAddressMatch(
								(sProxyTableRequestCmdPayload.b8Options &GP_APPLICATION_ID_MASK),
								(psGPCustomDataStructure->asZgpsSinkProxyTable[i].b16Options & GP_APPLICATION_ID_MASK),
								&sProxyTableRequestCmdPayload.uZgpdDeviceAddr,
								&(psGPCustomDataStructure->asZgpsSinkProxyTable[i].uZgpdDeviceAddr)
								) == FALSE)
						{
							/* ignore this entry */
							continue;
						}
					}
				}
				sProxyTableRespCmdPayload.u8Status = E_ZCL_SUCCESS;
                uint16 u16Value = u16GetProxyTableEntrySize ( &(psGPCustomDataStructure->asZgpsSinkProxyTable[i]) );
				if( u16Value < u16SizeFits)
				{
				    sProxyTableRespCmdPayload.u16SizeOfProxyTableEntries +=
						u16GetProxyTableString(&sProxyTableRespCmdPayload.puProxyTableEntries[u8Index],
								&(psGPCustomDataStructure->asZgpsSinkProxyTable[i]));
				    u8Index += sProxyTableRespCmdPayload.u16SizeOfProxyTableEntries;
				    u16SizeFits -= u16Value;
				    sProxyTableRespCmdPayload.u8EntriesCount++;
				}
			}
		}
	}

	eStatus = eGP_ProxyTableResponseSend(
			 	 	 	 psEndPointDefinition->u8EndPointNumber,
			             ZCL_GP_PROXY_ENDPOINT_ID,
	                      &sDestAddress,
	                      &sProxyTableRespCmdPayload);


	return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eGP_HandleProxyTableResponse
 **
 ** DESCRIPTION:
 ** Handles Green power Proxy Table response command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eGP_HandleProxyTableResponse(
                    ZPS_tsAfEvent                  *pZPSevent,
                    tsZCL_EndPointDefinition       *psEndPointDefinition,
                    tsZCL_ClusterInstance          *psClusterInstance,
                    uint16							u16Offset)
{

	tsGP_GreenPowerCustomData                   *psGPCustomDataStructure;
	tsGP_ProxyTableRespCmdPayload                sZgpProxyTableRespCmdPayload;
	teZCL_Status                                eStatus = E_ZCL_SUCCESS;
	uint8 u8ProxyTableEntries[MAX_SINK_TABLE_ENTRIES_LENGTH];
	// initialise pointer
	psGPCustomDataStructure = (tsGP_GreenPowerCustomData *)psClusterInstance->pvEndPointCustomStructPtr;

	// get EP mutex
	#ifndef COOPERATIVE
	eZCL_GetMutex(psEndPointDefinition);
	#endif

	DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_HandleProxyTableResponse =%d\n",u16Offset);
	// fill in callback event structure
	eZCL_SetCustomCallBackEvent(&psGPCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
			pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);

	// get information
	sZgpProxyTableRespCmdPayload.puProxyTableEntries = u8ProxyTableEntries;
	DBG_vPrintf(TRACE_GP_DEBUG, "\n calling  =%d\n",u16Offset);
	if((eStatus = eGP_ProxyTableResponseReceive(pZPSevent,u16Offset, &sZgpProxyTableRespCmdPayload)) != E_ZCL_SUCCESS)
	{
		// release mutex
		#ifndef COOPERATIVE
			eZCL_ReleaseMutex(psEndPointDefinition);
		#endif

		return eStatus;
	}
	DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_ProxyTableResponseReceive  Success \n");
	/* give the event to application */
	 psGPCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_ZGPD_PROXY_TABLE_RESPONSE_RCVD;
	 psGPCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psZgpProxyTableRespCmdPayload =
    		 &sZgpProxyTableRespCmdPayload;
	psEndPointDefinition->pCallBackFunctions(&psGPCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
	return eStatus;
}
#ifndef GP_DISABLE_PROXY_COMMISSION_MODE_CMD
/****************************************************************************
 **
 ** NAME:       eGP_HandleProxyCommissioningMode
 **
 ** DESCRIPTION:
 ** Handles Green power Cluster proxy commissioning mode command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_HandleProxyCommissioningMode(
                    ZPS_tsAfEvent                  *pZPSevent,
                    tsZCL_EndPointDefinition       *psEndPointDefinition,
                    tsZCL_ClusterInstance          *psClusterInstance,
                    uint16                         u16Offset)
{
    tsGP_GreenPowerCustomData                   *psGPCustomDataStructure;
    tsGP_ZgpProxyCommissioningModeCmdPayload    sZgpProxyCommissioningModeCmdPayload;
    teZCL_Status                                eStatus = E_ZCL_SUCCESS;
    tsZCL_Address                               sDestAddress = {E_ZCL_AM_SHORT,  {pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr}};
    uint64                                      u64RemoteIeeeAddress = 0;

    // initialise pointer
    psGPCustomDataStructure = (tsGP_GreenPowerCustomData *)psClusterInstance->pvEndPointCustomStructPtr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psGPCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
                pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);

    // get information
    if((eStatus = eGP_ProxyCommissioningModeReceive(
                                    pZPSevent,
                                    u16Offset,
                                    &sZgpProxyCommissioningModeCmdPayload)) != E_ZCL_SUCCESS)
    {
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif


        return eStatus;
    }
    DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_HandleProxyCommissioningMode- received\n");

#ifndef PC_PLATFORM_BUILD
    /* Check for IEEE address and device will go into commission mode only if
     * finds the IEEE address in any of ZPS tables */
    u64RemoteIeeeAddress = ZPS_u64NwkNibFindExtAddr(ZPS_pvNwkGetHandle(),
                                sDestAddress.uAddress.u16DestinationAddress);

    /* If IEEE address is not present then return */
    if(!u64RemoteIeeeAddress)
    {
        DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_HandleProxyCommissioningMode- IEEE not available \n");
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif


        return E_ZCL_FAIL;
    }
#endif

    /* Check device mode and action field of proxy commission mode command */
    /* Device address matched to device that put in commission mode */
    if( ( psGPCustomDataStructure->eGreenPowerDeviceMode != E_GP_OPERATING_MODE ) &&
            psGPCustomDataStructure->u64CommissionSetAddress == u64RemoteIeeeAddress &&
            !(sZgpProxyCommissioningModeCmdPayload.b8Options & GP_PROXY_CMSNG_MODE_ACTION_FIELD_MASK))
    {

        psGPCustomDataStructure->eGreenPowerDeviceMode = E_GP_OPERATING_MODE;
        psGPCustomDataStructure->u64CommissionSetAddress = 0x00;

        /* Set Event Type */
        psGPCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_COMMISSION_MODE_EXIT;
		psGPCustomDataStructure->u16CommissionWindow = 0x00;

        /* Call End Point Callback event informing that device exiting commissioning */
        psEndPointDefinition->pCallBackFunctions(&psGPCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
        psGPCustomDataStructure->bCommissionUnicast = FALSE;
    }
    else if(psGPCustomDataStructure->eGreenPowerDeviceMode == E_GP_OPERATING_MODE &&
           ( sZgpProxyCommissioningModeCmdPayload.b8Options & GP_PROXY_CMSNG_MODE_ACTION_FIELD_MASK))
    {
        /* Set device mode to Remote commission mode */
        psGPCustomDataStructure->eGreenPowerDeviceMode = E_GP_REMOTE_COMMISSION_MODE;

        /* Set Commission Exit Mode Details */
        /* If proxy commission mode command has commission timeout field then set timeout value */
        psGPCustomDataStructure->u16CommissionWindow = 0x00;
        if(sZgpProxyCommissioningModeCmdPayload.b8Options & GP_PROXY_CMSNG_MODE_EXIT_MODE_ON_WINDOW_EXPIRE_MASK)
        {
            psGPCustomDataStructure->u16CommissionWindow =
                    (sZgpProxyCommissioningModeCmdPayload.u16CommissioningWindow * 50);
        }
        psGPCustomDataStructure->bCommissionExitModeOnFirstPairSuccess = FALSE;
        psGPCustomDataStructure->bCommissionExitModeOnCommissionModeExitCmd = FALSE;
        if(sZgpProxyCommissioningModeCmdPayload.b8Options & GP_PROXY_CMSNG_MODE_EXIT_MODE_ON_PAIRING_SUCCESS_MASK)
        {
            psGPCustomDataStructure->bCommissionExitModeOnFirstPairSuccess = TRUE;
        }
        if(sZgpProxyCommissioningModeCmdPayload.b8Options & GP_PROXY_CMSNG_MODE_EXIT_MODE_ON_PROXY_MODE_CMD_MASK)
        {
            psGPCustomDataStructure->bCommissionExitModeOnCommissionModeExitCmd = TRUE;
        }
        psGPCustomDataStructure->bCommissionUnicast = FALSE;
        if(sZgpProxyCommissioningModeCmdPayload.b8Options & GP_PROXY_CMSNG_MODE_UNICAST_COMMUN_MASK)
        {
            psGPCustomDataStructure->bCommissionUnicast = TRUE;
            psGPCustomDataStructure->u16CommissionUnicastAddress = pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr;
            DBG_vPrintf(TRACE_GP_DEBUG, "\n psGPCustomDataStructure->bCommissionUnicast  \n");
        }

        psGPCustomDataStructure->u64CommissionSetAddress = u64RemoteIeeeAddress;

         /* Call end point callback event for informing that device moved to remote pairing mode */
         /* Set Event Type */
        psGPCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_COMMISSION_MODE_ENTER;
        psEndPointDefinition->pCallBackFunctions(&psGPCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);

    }

    /* Set Commission Indication Given Flag */
    psGPCustomDataStructure->bIsCommissionIndGiven = FALSE;

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return eStatus;
}
#endif

/****************************************************************************
 **
 ** NAME:       eGP_HandleZgpNotification
 **
 ** DESCRIPTION:
 ** Handles Green power Cluster ZGP Notification command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_HandleZgpNotification(
                    ZPS_tsAfEvent                  *pZPSevent,
                    tsZCL_EndPointDefinition       *psEndPointDefinition,
                    tsZCL_ClusterInstance          *psClusterInstance,
                    uint16                         u16Offset)
{
    teGP_GreenPowerBufferedCommands eBufferedCmdId = E_GP_DATA;
    bool_t IsPacketValid = TRUE;
    tsGP_ZgpDataIndication                      		sZgpDataIndication;
    uint8 au8Data[GP_MAX_ZB_CMD_PAYLOAD_LENGTH] = {0};
    tsGP_GreenPowerCustomData *psGPCustomDataStructure;
    tsGP_ZgpNotificationCmdPayload sNotificationCmdPyld = {0};
    teZCL_Status eStatus;

    // initialise pointer
    psGPCustomDataStructure = (tsGP_GreenPowerCustomData *)psClusterInstance->pvEndPointCustomStructPtr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    // if cluster is not in operating mode then drop the Notification packet
    if(psGPCustomDataStructure->eGreenPowerDeviceMode != E_GP_OPERATING_MODE)
    {
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif


        return E_ZCL_SUCCESS;
    }

    sNotificationCmdPyld.sZgpdCommandPayload.pu8Data = au8Data;
    sNotificationCmdPyld.sZgpdCommandPayload.u8Length = 0;
    sNotificationCmdPyld.sZgpdCommandPayload.u8MaxLength = GP_MAX_ZB_CMD_PAYLOAD_LENGTH;

    // get information
    eStatus = eGP_ZgpNotificationReceive(pZPSevent,  u16Offset, &sNotificationCmdPyld);


    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_GP_DEBUG, "sNotificationCmdPyld.eZgpdCmdId  = %d sNotificationCmdPyld->sZgpdCommandPayload.u8Length = %d, eStatus = %d\n", sNotificationCmdPyld.eZgpdCmdId,
        		sNotificationCmdPyld.sZgpdCommandPayload.u8Length, eStatus);
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif


        return eStatus;
    }
#ifdef GP_COMBO_BASIC_DEVICE
    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psGPCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
                pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);
    psGPCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_NOTIFICATION_RCVD;
    psGPCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psZgpNotificationCmdPayload =
    		&sNotificationCmdPyld;
   /* Give Application Callback for sink table update */
    psEndPointDefinition->pCallBackFunctions(&psGPCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);

#endif
    vFillDataIndicationFromNotif(&sNotificationCmdPyld, &sZgpDataIndication);

    /* Validate addressing fields and command id */
    if(bGP_ValidateGPDF_Fields(&sZgpDataIndication,psGPCustomDataStructure) == FALSE)
    {
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif
            DBG_vPrintf(TRACE_GP_DEBUG, "bGP_ValidateGPDF_Fields FALSE \n");

        return E_ZCL_SUCCESS;
    }
     if((sZgpDataIndication.u8CommandId == E_GP_COMMISSIONING) || (sZgpDataIndication.u8CommandId == E_GP_SUCCESS)||
    		 (sZgpDataIndication.u8CommandId == E_GP_CHANNEL_REQUEST))
     {

		 DBG_vPrintf(TRACE_GP_DEBUG, "\n Invalid command in notification \n",
				 sZgpDataIndication.u8CommandId );
		 return FALSE;
	 }
#ifdef GP_COMBO_BASIC_DEVICE
    /* Validate communication mode, duplicate filtering, send pairing for wrong comm mode */
    if(bIsNotificationValid(&sNotificationCmdPyld,  psEndPointDefinition,psClusterInstance,pZPSevent) == FALSE)
    {
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif
            DBG_vPrintf(TRACE_GP_DEBUG, "bIsNotificationValid FALSE \n");

        return E_ZCL_SUCCESS;
    }
#else
    if(bGP_IsDuplicatePkt(GP_ZGP_DUPLICATE_TIMEOUT,
    					  psEndPointDefinition->u8EndPointNumber,
                          psClusterInstance->bIsServer,
                          (uint8)sZgpDataIndication.u2ApplicationId,
                          sNotificationCmdPyld.uZgpdDeviceAddr,
                          ((sNotificationCmdPyld.b16Options & GP_NOTIFICATION_SECURITY_LEVEL_MASK) >> 6),
                          sNotificationCmdPyld.u32ZgpdSecFrameCounter,
                          sNotificationCmdPyld.eZgpdCmdId,
                          psGPCustomDataStructure))
#endif
    {
	    IsPacketValid = bGP_GPDFValidityCheck(&sZgpDataIndication,psEndPointDefinition, psClusterInstance,&eBufferedCmdId);
    }
	if(IsPacketValid)
	{
		IsPacketValid = bGP_CheckIfCmdToBeNotified(psGPCustomDataStructure,	&sZgpDataIndication);
		DBG_vPrintf(TRACE_GP_DEBUG, "packet validitiy %x\n",IsPacketValid);
	}
#ifdef GP_COMBO_BASIC_DEVICE
	if(IsPacketValid)
	{
		if(sNotificationCmdPyld.eZgpdCmdId == E_GP_DECOMMISSIONING)
		{
			eBufferedCmdId =E_GP_COMMISSIONING_CMDS;
		}
		else if ((sNotificationCmdPyld.eZgpdCmdId ==E_GP_COMMISSIONING)||
				(sNotificationCmdPyld.eZgpdCmdId == E_GP_SUCCESS))
		{
		    // release mutex
		    #ifndef COOPERATIVE
		        eZCL_ReleaseMutex(psEndPointDefinition);
		    #endif


		    return E_ZCL_SUCCESS;
		}

		/* Buffer the packet, packet will be processed in vGp_TransmissionTimerCallback*/
		eGp_BufferTransmissionPacket(
							&sZgpDataIndication,
							eBufferedCmdId,
							psGPCustomDataStructure);
		DBG_vPrintf(TRACE_GP_DEBUG, "\n Data packet received in notification buffered \n");

	}
#endif
    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return E_ZCL_SUCCESS;
}

void vGetRSSIAndLQIFromGPPGPDLink( tsGP_ZgpDataIndication                      		*psZgpDataIndication)
{
  uint8 u8LQI = (psZgpDataIndication->u8GPP_GPDLink & LQI_MASK) >> 6;
  if(u8LQI == LQI_POOR)
  	{
	  psZgpDataIndication->u8LinkQuality = 75;
  	}
  	else if((u8LQI > 100) &&( u8LQI <= 150))
  	{
  		psZgpDataIndication->u8LinkQuality = 125;
  	}
  	else if((u8LQI > 150) && (u8LQI <= 200))
  	{
  		psZgpDataIndication->u8LinkQuality = 175 ;
  	}
  	else if(u8LQI > 200)
  	{
  		psZgpDataIndication->u8LinkQuality = 225;
  	}

   psZgpDataIndication->u8RSSI =psZgpDataIndication->u8GPP_GPDLink & RSSI_MASK;
}
/****************************************************************************
 **
 ** NAME:       vFillDataIndication
 **
 ** DESCRIPTION:
 ** Fills Data Indication from commission notification
 **
 ** PARAMETERS:               Name                      Usage
 ** tsGP_ZgpCommissioningNotificationCmdPayload 		*psZgpCommissioningNotificationCmdPayload,
 ** tsGP_ZgpDataIndication                      		*psZgpDataIndication
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/

void vFillDataIndicationFromNotif(
		tsGP_ZgpNotificationCmdPayload  		            *psZgpNotificationCmdPayload,
		 tsGP_ZgpDataIndication                      		*psZgpDataIndication
		)
{
	 /* check Application Id in option field */
	if( (psZgpNotificationCmdPayload->b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_4_BYTE)
	{
		psZgpDataIndication->uZgpdDeviceAddr.u32ZgpdSrcId = psZgpNotificationCmdPayload->uZgpdDeviceAddr.u32ZgpdSrcId;
	}
#ifdef GP_IEEE_ADDR_SUPPORT
	else
	{
		psZgpDataIndication->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr =
				psZgpNotificationCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
		psZgpDataIndication->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint =
				psZgpNotificationCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
	}
#endif


	psZgpDataIndication->u8CommandId = psZgpNotificationCmdPayload->eZgpdCmdId;
	psZgpDataIndication->u2ApplicationId = (psZgpNotificationCmdPayload->b16Options & GP_APPLICATION_ID_MASK);
	psZgpDataIndication->u16NwkShortAddr = psZgpNotificationCmdPayload->u16ZgppShortAddr;
	psZgpDataIndication->u8GPP_GPDLink = psZgpNotificationCmdPayload->u8GPP_GPD_Link;
	vGetRSSIAndLQIFromGPPGPDLink(psZgpDataIndication);
	psZgpDataIndication->bTunneledPkt = TRUE;
	psZgpDataIndication->bFrameType = 0;
	/* Check Key Type */
	if(((psZgpNotificationCmdPayload->b16Options & GP_NOTIFICATION_SECURITY_KEY_TYPE_MASK) >> 8) > E_GP_NWK_KEY_DERIVED_ZGPD_GROUP_KEY)
	{
		psZgpDataIndication->u2SecurityKeyType = 0x01; /* Individual Key */
	}
	else
	{
		psZgpDataIndication->u2SecurityKeyType = 0x00; /* Common Key */
	}

	psZgpDataIndication->u2SecurityLevel = (psZgpNotificationCmdPayload->b16Options & GP_NOTIFICATION_SECURITY_LEVEL_MASK) >> 6;

	psZgpDataIndication->u32SecFrameCounter = psZgpNotificationCmdPayload->u32ZgpdSecFrameCounter;

	if(psZgpDataIndication->u2SecurityLevel == 0)
	{
		psZgpDataIndication->u8SeqNum = (uint8)psZgpNotificationCmdPayload->u32ZgpdSecFrameCounter;
		psZgpDataIndication->u8Status = E_GP_SEC_NO_SECURITY;
	}
	else
	{
		psZgpDataIndication->u8Status = E_GP_SEC_SUCCESS;
	}


	psZgpDataIndication->bAutoCommissioning = FALSE;

	psZgpDataIndication->bRxAfterTx = FALSE;



	/* Payload length 0xff allowed */
	if(psZgpNotificationCmdPayload->sZgpdCommandPayload.u8Length == 0xFF)
	{
		psZgpDataIndication->u8PDUSize = 0; /* set to zero */
		psZgpDataIndication->pu8Pdu = NULL;
	}
	else
	{
		psZgpDataIndication->u8PDUSize = psZgpNotificationCmdPayload->sZgpdCommandPayload.u8Length ;
		/* Copy Payload */
		psZgpDataIndication->pu8Pdu = psZgpNotificationCmdPayload->sZgpdCommandPayload.pu8Data;
	}

	DBG_vPrintf(TRACE_GP_DEBUG, "psZgpDataIndication->u8CommandId  = %d\n", psZgpDataIndication->u8CommandId );
}
/****************************************************************************
 **
 ** NAME:       vFillDataIndication
 **
 ** DESCRIPTION:
 ** Fills Data Indication from commission notification
 **
 ** PARAMETERS:               Name                      Usage
 ** tsGP_ZgpCommissioningNotificationCmdPayload 		*psZgpCommissioningNotificationCmdPayload,
 ** tsGP_ZgpDataIndication                      		*psZgpDataIndication
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/

void vFillDataIndication(
		tsGP_ZgpCommissioningNotificationCmdPayload 		*psZgpCommissioningNotificationCmdPayload,
		 tsGP_ZgpDataIndication                      		*psZgpDataIndication
		)
{

	 /* check Application Id in option field */
	psZgpDataIndication->u2ApplicationId = (psZgpCommissioningNotificationCmdPayload->b16Options & GP_APPLICATION_ID_MASK) ;
	if( (psZgpCommissioningNotificationCmdPayload->b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_4_BYTE)
	{
		psZgpDataIndication->uZgpdDeviceAddr.u32ZgpdSrcId = psZgpCommissioningNotificationCmdPayload->uZgpdDeviceAddr.u32ZgpdSrcId;
	}
#ifdef GP_IEEE_ADDR_SUPPORT
	else
	{
		psZgpDataIndication->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr =
			psZgpCommissioningNotificationCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
		psZgpDataIndication->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint =
			psZgpCommissioningNotificationCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
	}
#endif


	psZgpDataIndication->u8CommandId = psZgpCommissioningNotificationCmdPayload->u8ZgpdCmdId;
	psZgpDataIndication->u2ApplicationId = (psZgpCommissioningNotificationCmdPayload->b16Options & GP_APPLICATION_ID_MASK);
	psZgpDataIndication->u16NwkShortAddr = psZgpCommissioningNotificationCmdPayload->u16ZgppShortAddr;
	psZgpDataIndication->u8GPP_GPDLink = psZgpCommissioningNotificationCmdPayload->u8GPP_GPD_Link;
	vGetRSSIAndLQIFromGPPGPDLink(psZgpDataIndication);
	psZgpDataIndication->bTunneledPkt = TRUE;
	/* . If the GPD command was received with the Maintenance FrameType, the ApplicationID sub-field of
	 *  the Options field is 0 and the GPD ID will be 0x00000000.*/
	if((psZgpDataIndication->uZgpdDeviceAddr.u32ZgpdSrcId == 0) &&
			((psZgpCommissioningNotificationCmdPayload->b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_4_BYTE))
	{
		psZgpDataIndication->bFrameType = 1;
	}
	else
	{
		psZgpDataIndication->bFrameType = 0;
	}
	psZgpDataIndication->bRxAfterTx = (psZgpCommissioningNotificationCmdPayload->b16Options & GP_CMSNG_NOTIFICATION_RX_AFTER_TX_MASK) >> 3;
	/* Check Key Type */
	if(((psZgpCommissioningNotificationCmdPayload->b16Options & GP_CMSNG_NOTIFICATION_SECURITY_KEY_TYPE_MASK) >> 6) > E_GP_NWK_KEY_DERIVED_ZGPD_GROUP_KEY)
	{
		psZgpDataIndication->u2SecurityKeyType = 0x01; /* Individual Key */
	}
	else
	{
		psZgpDataIndication->u2SecurityKeyType = 0x00; /* Common Key */
	}

	psZgpDataIndication->u2SecurityLevel = (psZgpCommissioningNotificationCmdPayload->b16Options & GP_CMSNG_NOTIFICATION_SECURITY_LEVEL_MASK) >> 4;

	psZgpDataIndication->u32SecFrameCounter = psZgpCommissioningNotificationCmdPayload->u32ZgpdSecFrameCounter;

	if(psZgpDataIndication->u2SecurityLevel == 0)
	{
		psZgpDataIndication->u8SeqNum = (uint8)psZgpCommissioningNotificationCmdPayload->u32ZgpdSecFrameCounter;
		psZgpDataIndication->u8Status = E_GP_SEC_NO_SECURITY;
	}
	else
	{
		psZgpDataIndication->u8Status = E_GP_SEC_SUCCESS;
	}

	/* for auto commissioned, check Cmd type */
	if(psZgpDataIndication->u8CommandId < E_GP_COMMISSIONING)
	{
		psZgpDataIndication->bAutoCommissioning = TRUE;

	}
	else
	{

		psZgpDataIndication->bAutoCommissioning = FALSE;
	}

	/* For Mic, check security processing flag */
	if(psZgpCommissioningNotificationCmdPayload->b16Options & GP_CMSNG_NOTIFICATION_SEC_PROCESS_FAIL_MASK)
	{
		psZgpDataIndication->u32Mic = psZgpCommissioningNotificationCmdPayload->u32Mic;
	}


	/* Payload length 0xff allowed */
	if(psZgpCommissioningNotificationCmdPayload->sZgpdCommandPayload.u8Length == 0xFF)
	{
		psZgpDataIndication->u8PDUSize = 0; /* set to zero */
	}
	else
	{
		psZgpDataIndication->u8PDUSize = psZgpCommissioningNotificationCmdPayload->sZgpdCommandPayload.u8Length ;
	}
	/* Copy Payload */
	psZgpDataIndication->pu8Pdu = psZgpCommissioningNotificationCmdPayload->sZgpdCommandPayload.pu8Data;

	DBG_vPrintf(TRACE_GP_DEBUG, "vFillDataIndication Done %d \n" ,psZgpCommissioningNotificationCmdPayload->sZgpdCommandPayload.u8Length);

}

/****************************************************************************
 **
 ** NAME:       eGP_HandleZgpCommissioningNotification
 **
 ** DESCRIPTION:
 ** Handles Green power Cluster ZGP Commissioning Notification command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_HandleZgpCommissioningNotification(
                    ZPS_tsAfEvent                  *pZPSevent,
                    tsZCL_EndPointDefinition       *psEndPointDefinition,
                    tsZCL_ClusterInstance          *psClusterInstance,
                    uint16                         u16Offset)
{
    tsGP_GreenPowerCustomData                   *psGpCustomDataStructure;
    tsGP_ZgpCommissioningNotificationCmdPayload sZgpCommissioningNotificationCmdPayload;
    tsGP_ZgpDataIndication                      sZgpDataIndication = { 0};
#ifdef GP_COMBO_BASIC_DEVICE
    teGP_GreenPowerBufferedCommands eBufferedCmdId = E_GP_DATA;
#endif
    bool_t                                        IsPacketValid;
    uint8                                		u8Status = E_ZCL_FAIL;
    teZCL_Status                                eStatus;
    uint8                                       au8Data[GP_MAX_ZB_CMD_PAYLOAD_LENGTH] = {0};
    // initialise pointer
    psGpCustomDataStructure = (tsGP_GreenPowerCustomData *)psClusterInstance->pvEndPointCustomStructPtr;

    // if cluster is not in commission mode then drop the commission Notification packet
    DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_HandleZgpCommissioningNotification \n");
    if(psGpCustomDataStructure->eGreenPowerDeviceMode == E_GP_OPERATING_MODE)
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, "\n E_GP_OPERATING_MODE\n");
        return E_ZCL_SUCCESS;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
                pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);

    sZgpCommissioningNotificationCmdPayload.sZgpdCommandPayload.pu8Data = au8Data;
    sZgpCommissioningNotificationCmdPayload.sZgpdCommandPayload.u8Length = 0;
    sZgpCommissioningNotificationCmdPayload.sZgpdCommandPayload.u8MaxLength = GP_MAX_ZB_CMD_PAYLOAD_LENGTH;

    // get information
    eStatus = eGP_ZgpCommissioningNotificationReceive(
                                pZPSevent,
                                u16Offset,
                                &sZgpCommissioningNotificationCmdPayload);
    DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_ZgpCommissioningNotificationReceive %d %d %d \n",
    		sZgpCommissioningNotificationCmdPayload.sZgpdCommandPayload.u8Length,au8Data[0],au8Data[1]);
    if(eStatus == E_ZCL_SUCCESS)
    {
        // fill in callback event structure
        eZCL_SetCustomCallBackEvent(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
                    pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);
        psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_COMM_NOTIFICATION_RCVD;
        psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psZgpCommissioningNotificationCmdPayload =
        		&sZgpCommissioningNotificationCmdPayload;
       /* Give Application Callback for sink table update */
        psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);


		vFillDataIndication(&sZgpCommissioningNotificationCmdPayload, &sZgpDataIndication);
		IsPacketValid = 1;

	   /* Check Status */
		if(IsPacketValid)
		{
#ifdef GP_COMBO_BASIC_DEVICE
			if(sZgpCommissioningNotificationCmdPayload.b16Options & GP_CMSNG_NOTIFICATION_SEC_PROCESS_FAIL_MASK)
			{
				if(bDecryptGPDFData(&sZgpCommissioningNotificationCmdPayload,
						psEndPointDefinition,
					    psGpCustomDataStructure))
				{
					DBG_vPrintf(TRACE_GP_DEBUG, "\n bDecryptGPDFData Success %d \n", sZgpCommissioningNotificationCmdPayload.u8ZgpdCmdId);
					/* Fill decrypted payload */
					vFillDataIndication(&sZgpCommissioningNotificationCmdPayload, &sZgpDataIndication);
				}
				else
				{
					DBG_vPrintf(TRACE_GP_DEBUG, "\n bDecryptGPDFData Failed %d \n",sZgpCommissioningNotificationCmdPayload.u8ZgpdCmdId);

#ifndef COOPERATIVE
    eZCL_ReleaseMutex(psEndPointDefinition);
#endif
               return E_ZCL_SUCCESS;
				}
			}
#endif
			if((sZgpCommissioningNotificationCmdPayload.b16Options & GP_CMSNG_NOTIFICATION_RX_AFTER_TX_MASK) ||
					(psGpCustomDataStructure->eGreenPowerDeviceMode == E_GP_PAIRING_COMMISSION_MODE))
			{

				IsPacketValid = bGP_CheckIfCmdToBeBuffered(psGpCustomDataStructure,&sZgpCommissioningNotificationCmdPayload);
				DBG_vPrintf(TRACE_GP_DEBUG, "\n bGP_CheckIfCmdToBeBuffered = %d\n", IsPacketValid);
#ifdef GP_COMBO_BASIC_DEVICE
				if(IsPacketValid)
				{
					DBG_vPrintf(TRACE_GP_DEBUG, "\n commissioning command not present in buffered\n");

					/* Perform Duplicate filtering */
					IsPacketValid = bGP_IsDuplicatePkt(GP_ZGP_DUPLICATE_TIMEOUT,
										  psEndPointDefinition->u8EndPointNumber,
										  psClusterInstance->bIsServer,
										  (uint8)sZgpDataIndication.u2ApplicationId,
										  sZgpCommissioningNotificationCmdPayload.uZgpdDeviceAddr,
										  (uint8)((sZgpCommissioningNotificationCmdPayload.b16Options & GP_CMSNG_NOTIFICATION_SECURITY_LEVEL_MASK) >> 4),
										  sZgpCommissioningNotificationCmdPayload.u32ZgpdSecFrameCounter,
										  sZgpCommissioningNotificationCmdPayload.u8ZgpdCmdId,
										  psGpCustomDataStructure);
					if(IsPacketValid == FALSE)
					{
						IsPacketValid = TRUE;
						DBG_vPrintf(TRACE_GP_DEBUG, "\n bGP_IsDuplicatePkt FALSE \n");
						if((sZgpDataIndication.u8CommandId == E_GP_CHANNEL_REQUEST) &&
								((sZgpCommissioningNotificationCmdPayload.b16Options & GP_CMSNG_NOTIFICATION_RX_AFTER_TX_MASK) == 0))
						{
							DBG_vPrintf(TRACE_GP_DEBUG, "\n E_GP_CHANNEL_REQUEST without RxAfterTX  \n");
							IsPacketValid = FALSE;
						}
						if(IsPacketValid)
						{
							IsPacketValid = bGP_GPDFValidityCheck(&sZgpDataIndication,psEndPointDefinition, psClusterInstance,&eBufferedCmdId);
						}

						if(IsPacketValid)
						{
							/* Buffer the packet, packet will be processed in vGp_TransmissionTimerCallback*/

							DBG_vPrintf(TRACE_GP_DEBUG, "\n commissioning command buffered from eGP_HandleZgpCommissioningNotification  bRxAfterTx = %d\n",
									sZgpDataIndication.bRxAfterTx);

							if(eBufferedCmdId == E_GP_COMMISSIONING_CMDS )
							{

								if(bGP_GiveCommIndAndUpdateTable(&sZgpDataIndication, psEndPointDefinition,psGpCustomDataStructure) == FALSE)
								{
								    // release mutex
								    #ifndef COOPERATIVE
								        eZCL_ReleaseMutex(psEndPointDefinition);
								    #endif

									return FALSE;
								}
							}
							eGp_BufferTransmissionPacket(
												&sZgpDataIndication,
												eBufferedCmdId,
												psGpCustomDataStructure);
							DBG_vPrintf(TRACE_GP_DEBUG, "\n Buffered comm notif  TRUE\n");
						}
						else
						{
							DBG_vPrintf(TRACE_GP_DEBUG, "\n bGP_GPDFValidityCheck  failed in comm notif\n");

						}
					}
					else
					{
						DBG_vPrintf(TRACE_GP_DEBUG, "\n bGP_IsDuplicatePkt  TRUE\n");
					}
				}

	#endif
			}

		}

    }
    else
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_ZgpCommissioningNotificationReceive Failed  \n");
    }



    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return u8Status;
}




/****************************************************************************
 **
 ** NAME:       eGP_HandleZgpPairing
 **
 ** DESCRIPTION:
 ** Handles Green power Cluster ZGP pairing command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_HandleZgpPairing(
                    ZPS_tsAfEvent                  *pZPSevent,
                    tsZCL_EndPointDefinition       *psEndPointDefinition,
                    tsZCL_ClusterInstance          *psClusterInstance,
                    uint16                         u16Offset)
{
    tsGP_GreenPowerCustomData                   *psGpCustomDataStructure;
    tsGP_ZgpPairingCmdPayload                   sZgpPairingCmdPayload;
    tsGP_ZgppProxySinkTable                         *psZgppProxySinkTable;
    teZCL_Status                                eStatus;
    uint8 										u8CommunicationMode;
    uint8                                       u8ApplicationId, i;
    bool_t                                      bIsProxyEntryPresent;
    teGP_GreenPowerSecLevel    eSecLevel;
    teGP_GreenPowerSecKeyType  eSecKeyType;
        // initialise pointer
    psGpCustomDataStructure = (tsGP_GreenPowerCustomData *)psClusterInstance->pvEndPointCustomStructPtr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
                pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);
    /* Initialize alias to 0xFFFF so that if it is not sent over the air, the default value is assigned */
    sZgpPairingCmdPayload.u16AssignedAlias = 0xFFFF;
    // get information
    eStatus = eGP_ZgpPairingReceive(
                                pZPSevent,
                                u16Offset,
                                &sZgpPairingCmdPayload);

    /* Check Status */
    if(eStatus != E_ZCL_SUCCESS)
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_ZgpPairingReceive=%d\n",eStatus);
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif


        return E_ZCL_SUCCESS;
    }

    psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_PAIRING_CMD_RCVD;
    psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psZgpPairingCmdPayload =
		&sZgpPairingCmdPayload;
    psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);

    if(bGP_IsPairingCmdValid(&sZgpPairingCmdPayload) == FALSE)
    {
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif
            DBG_vPrintf(TRACE_GP_DEBUG, "\n bGP_IsPairingCmdValid failed=%d\n",eStatus);

        return E_ZCL_SUCCESS;
    }

    u8ApplicationId = (uint8)(sZgpPairingCmdPayload.b24Options & GP_APPLICATION_ID_MASK);

    bIsProxyEntryPresent = 	bGP_IsProxyTableEntryPresent(
    		   	     		psEndPointDefinition->u8EndPointNumber,
    		#ifdef GP_COMBO_BASIC_DEVICE
    							TRUE,
    		#else
    							FALSE,
    		#endif
    							u8ApplicationId,
    		   	     	&sZgpPairingCmdPayload.uZgpdDeviceAddr,
    		   	     		
    		   	     		&psZgppProxySinkTable);
    if((bIsProxyEntryPresent == FALSE) && (sZgpPairingCmdPayload.b24Options & GP_PAIRING_ADD_SINK_MASK))
    {
#ifndef GP_COMBO_BASIC_DEVICE
    	if( (( sZgpPairingCmdPayload.b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5 ) != E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_BOTH )
#endif
		{
    	/*  Proxy table entry does not exist. add new entry */
    	bIsProxyEntryPresent = bGP_GetFreeProxySinkTableEntry(psEndPointDefinition->u8EndPointNumber,
    	#ifdef GP_COMBO_BASIC_DEVICE
    		                    TRUE,
    	#else
    		                    FALSE,
    	#endif
    		                    &psZgppProxySinkTable);
		}

    	if(bIsProxyEntryPresent && psZgppProxySinkTable != NULL)
    	{
    	    /* add address */
    	    if(u8ApplicationId == GP_APPL_ID_4_BYTE )
		    {
    	       psZgppProxySinkTable->b16Options = GP_APPL_ID_4_BYTE;
    	       psZgppProxySinkTable->uZgpdDeviceAddr.u32ZgpdSrcId = sZgpPairingCmdPayload.uZgpdDeviceAddr.u32ZgpdSrcId;

		    }
	    #ifdef GP_IEEE_ADDR_SUPPORT
		    else
		    {
		       psZgppProxySinkTable->b16Options = GP_APPL_ID_8_BYTE;
		       psZgppProxySinkTable->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr =
				       sZgpPairingCmdPayload.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;

		      psZgppProxySinkTable->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint =
				      sZgpPairingCmdPayload.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
		    }
    	 #endif
    	}

    }

    if((bIsProxyEntryPresent) && ( psZgppProxySinkTable != NULL ))
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, "bIsProxyEntryPresent = %d\n",bIsProxyEntryPresent);

        if(sZgpPairingCmdPayload.b24Options & GP_PAIRING_REMOVE_GPD_MASK )
        {

        	vGP_RemoveGPDFromProxySinkTable(psEndPointDefinition->u8EndPointNumber,
        			u8ApplicationId,
        			&sZgpPairingCmdPayload.uZgpdDeviceAddr);
        }
        else
        {
            if(!(sZgpPairingCmdPayload.b24Options & GP_PAIRING_ADD_SINK_MASK ))
            {
                u8CommunicationMode = (sZgpPairingCmdPayload.b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5;

                if((psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_COMMISSION_GROUP_GPS_MASK)&&
                        (psZgppProxySinkTable->u8SinkGroupListEntries != 0)&&(u8CommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID))
                {
                    for(i=0;i < psZgppProxySinkTable->u8SinkGroupListEntries; i++)
                    {
                        if(sZgpPairingCmdPayload.u16SinkGroupID == psZgppProxySinkTable->asSinkGroupList[i].u16SinkGroup)
                        {

                        	uint8 j = i;
                        	for(j=i; j < (psZgppProxySinkTable->u8SinkGroupListEntries - 1); j++)
                        	{
								psZgppProxySinkTable->asSinkGroupList[j].u16SinkGroup = psZgppProxySinkTable->asSinkGroupList[j+1].u16SinkGroup;
								psZgppProxySinkTable->asSinkGroupList[j].u16Alias = psZgppProxySinkTable->asSinkGroupList[j+1].u16Alias;
                        	}
                            psZgppProxySinkTable->u8SinkGroupListEntries--;
                            if(psZgppProxySinkTable->u8SinkGroupListEntries == 0)
                            {
                                //psZgppProxySinkTable->u8SearchCounter = 0;
#ifdef CLD_GP_ATTR_ZGPP_BLOCKED_GPD_ID
             /* TBD */
#else
                                //psZgppProxySinkTable->b16Options |= ~GP_PROXY_TABLE_ENTRY_ACTIVE_MASK;
                                if((psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_DERIVED_GROUP_GPS_MASK)||( psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_UNICAST_GPS_MASK))
                                {
                                    psZgppProxySinkTable->b16Options &= ~GP_PROXY_TABLE_COMMISSION_GROUP_GPS_MASK;
                                }
                                else
                                {
                                //	memset(psZgppProxySinkTable,0,sizeof(tsGP_ZgppProxySinkTable));
                                	vGP_RemoveGPDFromProxySinkTable( psEndPointDefinition->u8EndPointNumber,
                                									u8ApplicationId,
                                									&sZgpPairingCmdPayload.uZgpdDeviceAddr);

                                }
#endif
                            }
                            break;
                        }
                    }
                }
                if(u8CommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)
                {
                    if(psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_DERIVED_GROUP_GPS_MASK)
                    {
                        if((psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_COMMISSION_GROUP_GPS_MASK) ||( psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_UNICAST_GPS_MASK))
                        {
                            psZgppProxySinkTable->b16Options &= ~GP_PROXY_TABLE_DERIVED_GROUP_GPS_MASK;

                        }
                        else
                        {
                        	//memset(psZgppProxySinkTable,0,sizeof(tsGP_ZgppProxySinkTable));
                        	vGP_RemoveGPDFromProxySinkTable( psEndPointDefinition->u8EndPointNumber,
                        									u8ApplicationId,
                        									&sZgpPairingCmdPayload.uZgpdDeviceAddr);

                        }
                    }
                }
                if((u8CommunicationMode == E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_LIGHTWEIGHT)
#ifdef GP_COMBO_BASIC_DEVICE
                		||
               					(u8CommunicationMode == E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_BOTH)
#endif
                )
                {

                	  for(i=0;i < psZgppProxySinkTable->u8NoOfUnicastSink; i++)
					  {
						  if(sZgpPairingCmdPayload.u64SinkIEEEAddress == psZgppProxySinkTable->u64SinkUnicastIEEEAddres[i])
						  {

							  uint8 j = i;
							 for(j=i; j < (psZgppProxySinkTable->u8NoOfUnicastSink - 1); j++)
							 {
								  psZgppProxySinkTable->asSinkGroupList[j].u16SinkGroup = psZgppProxySinkTable->asSinkGroupList[j+1].u16SinkGroup;
								  psZgppProxySinkTable->asSinkGroupList[j].u16Alias = psZgppProxySinkTable->asSinkGroupList[j+ 1].u16Alias;
							 }
							  psZgppProxySinkTable->u8NoOfUnicastSink--;
							  if(psZgppProxySinkTable->u8NoOfUnicastSink == 0)
							  {

								  if((psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_DERIVED_GROUP_GPS_MASK) ||(psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_COMMISSION_GROUP_GPS_MASK))
								  {
									  psZgppProxySinkTable->b16Options &= ~GP_PROXY_TABLE_UNICAST_GPS_MASK;
								  }
								  else
								  {
									//memset(psZgppProxySinkTable,0,sizeof(tsGP_ZgppProxySinkTable));
									  vGP_RemoveGPDFromProxySinkTable( psEndPointDefinition->u8EndPointNumber,
	                                									u8ApplicationId,
	                                									&sZgpPairingCmdPayload.uZgpdDeviceAddr);

								  }

							  }
							  break;
						  }
					  }
                }
            }
            else
            {
                psZgppProxySinkTable->b16Options |= GP_PROXY_TABLE_ENTRY_VALID_MASK;
                psZgppProxySinkTable->b16Options |= GP_PROXY_TABLE_ENTRY_ACTIVE_MASK;
                u8CommunicationMode = (sZgpPairingCmdPayload.b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5;
                if(u8CommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)
                {
                    psZgppProxySinkTable->b16Options |= GP_PROXY_TABLE_DERIVED_GROUP_GPS_MASK;
                }
                else if(u8CommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
                {
                    psZgppProxySinkTable->b16Options |= GP_PROXY_TABLE_COMMISSION_GROUP_GPS_MASK;
                }
                else if((u8CommunicationMode == E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_LIGHTWEIGHT)
#ifdef GP_COMBO_BASIC_DEVICE
                		||
               					(u8CommunicationMode == E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_BOTH)
#endif
                )
                {
                	  psZgppProxySinkTable->b16Options |= GP_PROXY_TABLE_UNICAST_GPS_MASK;
                	  if(psZgppProxySinkTable->u8NoOfUnicastSink < GP_MAX_UNICAST_SINK)
                	  {

                		  psZgppProxySinkTable->u16SinkUnicastNWKAddress[psZgppProxySinkTable->u8NoOfUnicastSink] =
                				  sZgpPairingCmdPayload.u16SinkNwkAddress;
                		  psZgppProxySinkTable->u64SinkUnicastIEEEAddres[psZgppProxySinkTable->u8NoOfUnicastSink] =
                				  sZgpPairingCmdPayload.u64SinkIEEEAddress;
                		  psZgppProxySinkTable->u8NoOfUnicastSink++;
                	  }
                }

                if((psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_COMMISSION_GROUP_GPS_MASK)&&
                   (u8CommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID))
                {
                    for(i=0;i < psZgppProxySinkTable->u8SinkGroupListEntries; i++)
                    {
                        if(sZgpPairingCmdPayload.u16SinkGroupID == psZgppProxySinkTable->asSinkGroupList[i].u16SinkGroup)
                        {
                            break;
                        }
                    }
                    if((i == psZgppProxySinkTable->u8SinkGroupListEntries)&&(psZgppProxySinkTable->u8SinkGroupListEntries < GP_MAX_SINK_GROUP_LIST))
                    {
                        psZgppProxySinkTable->asSinkGroupList[psZgppProxySinkTable->u8SinkGroupListEntries].u16SinkGroup = sZgpPairingCmdPayload.u16SinkGroupID;
                        psZgppProxySinkTable->asSinkGroupList[psZgppProxySinkTable->u8SinkGroupListEntries].u16Alias = sZgpPairingCmdPayload.u16AssignedAlias;
                        psZgppProxySinkTable->u8SinkGroupListEntries++;
                        eStatus = ZPS_eAplZdoGroupEndpointAdd(sZgpPairingCmdPayload.u16SinkGroupID, ZCL_GP_PROXY_ENDPOINT_ID);
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

                if(sZgpPairingCmdPayload.b24Options & GP_PAIRING_GPD_FIXED_MASK)
                {
                    psZgppProxySinkTable->b16Options |= GP_PROXY_TABLE_GPD_FIXED_MASK;
                }

                if(sZgpPairingCmdPayload.b24Options & GP_PAIRING_SEQ_NUM_CAP_MASK)
                {
                    psZgppProxySinkTable->b16Options |= GP_PROXY_TABLE_SEQ_NUM_CAP_MASK;
                }

                eSecLevel = (sZgpPairingCmdPayload.b24Options & GP_PAIRING_SEC_LEVEL_MASK) >> 9;
                eSecKeyType = (sZgpPairingCmdPayload.b24Options & GP_PAIRING_SEC_KEY_TYPE_MASK) >> 11;
                //if security key is available
                if(eSecLevel != E_GP_NO_SECURITY)
                {
                    psZgppProxySinkTable->b16Options |= GP_PROXY_TABLE_SECURITY_USE_MASK;
                    psZgppProxySinkTable->b8SecOptions = eSecLevel;
                    psZgppProxySinkTable->b8SecOptions |= eSecKeyType << 2;
                    if((eSecKeyType != E_GP_OUT_OF_THE_BOX_ZGPD_KEY) && (eSecKeyType != E_GP_DERIVED_INDIVIDUAL_ZGPD_KEY))
                    {
                    		/* Set the group key. we might have missed an update */
                    	eZCL_WriteLocalAttributeValue(psEndPointDefinition->u8EndPointNumber,GREENPOWER_CLUSTER_ID,FALSE,FALSE,TRUE,E_CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE, &eSecKeyType);
                    }
                }
                else
                {
                    psZgppProxySinkTable->b16Options &= ~GP_PROXY_TABLE_SECURITY_USE_MASK;
                }

                if(sZgpPairingCmdPayload.b24Options & GP_PAIRING_SECURITY_COUNTER_PRESENT_MASK)
                {
                    psZgppProxySinkTable->u32ZgpdSecFrameCounter = sZgpPairingCmdPayload.u32ZgpdSecFrameCounter;
                }

                if(sZgpPairingCmdPayload.b24Options & GP_PAIRING_SECURITY_KEY_PRESENT_MASK)
                {
                    memcpy(&psZgppProxySinkTable->sZgpdKey.au8Key, sZgpPairingCmdPayload.sZgpdKey.au8Key, E_ZCL_KEY_128_SIZE);
                    if((eSecKeyType != E_GP_OUT_OF_THE_BOX_ZGPD_KEY) && (eSecKeyType != E_GP_DERIVED_INDIVIDUAL_ZGPD_KEY))
                    {
                    		/* Set the group key. we might have missed an update */
                    	eZCL_WriteLocalAttributeValue(psEndPointDefinition->u8EndPointNumber,GREENPOWER_CLUSTER_ID,FALSE,FALSE,TRUE,E_CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY, sZgpPairingCmdPayload.sZgpdKey.au8Key);
                    }
                }

                if((sZgpPairingCmdPayload.b24Options & GP_PAIRING_ASSIGNED_ALIAS_PRESENT_MASK)&&
                   (u8CommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID))
                {
                    psZgppProxySinkTable->u16ZgpdAssignedAlias = sZgpPairingCmdPayload.u16AssignedAlias;
                    psZgppProxySinkTable->b16Options |= GP_PROXY_TABLE_ASSIGNED_ALIAS_MASK;
                }

                if(sZgpPairingCmdPayload.b24Options & GP_PAIRING_FORWARDING_RADIUS_PRESENT_MASK)
                {
                    psZgppProxySinkTable->u8GroupCastRadius = sZgpPairingCmdPayload.u8ForwardingRadius;
                }
                /* check if  GPD address conflict with self address*/
                bGP_AliasPresent( ZPS_u16AplZdoGetNwkAddr(), ZPS_u64AplZdoGetIeeeAddr(),0 /* ignored */);

            }
        }
        //pairing success

    }

    if(!bIsProxyEntryPresent &&
       (sZgpPairingCmdPayload.b24Options & GP_PAIRING_ADD_SINK_MASK) &&
	   (pZPSevent->uEvent.sApsDataIndEvent.uDstAddress.u16Addr == ZPS_u16AplZdoGetNwkAddr() ) )
    {
#ifndef GP_COMBO_BASIC_DEVICE
    	if( (( sZgpPairingCmdPayload.b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5 ) == E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_BOTH )
		{
    	    eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_FIELD);
		}
		else
		{
#endif
			eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INSUFFICIENT_SPACE);
#ifndef GP_COMBO_BASIC_DEVICE
		}
#endif
    }


    if((psGpCustomDataStructure->eGreenPowerDeviceMode != E_GP_OPERATING_MODE)&&
    		( psGpCustomDataStructure->bCommissionExitModeOnFirstPairSuccess))
    {
    	vGP_ExitCommMode(psEndPointDefinition,
    			psGpCustomDataStructure);
    }
    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return E_ZCL_SUCCESS;

}


#ifdef GP_COMBO_BASIC_DEVICE
#ifndef GP_DISABLE_TRANSLATION_TABLE_REQ_CMD
/****************************************************************************
 **
 ** NAME:       eGP_HandleZgpTranslationRequest
 **
 ** DESCRIPTION:
 ** Handles Green power Cluster ZGP translation request command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_HandleZgpTranslationRequest(
                    ZPS_tsAfEvent                  *pZPSevent,
                    tsZCL_EndPointDefinition       *psEndPointDefinition,
                    tsZCL_ClusterInstance          *psClusterInstance,
                    uint16                         u16Offset)
{
    tsGP_GreenPowerCustomData                   *psGpCustomDataStructure;
    tsGP_ZgpTransTableResponseCmdPayload        sZgpTransRspCmdPayload;
    teZCL_Status                                eStatus;
    uint8                                       u8Count = 0,u8TotalCount = 0, i, u8TransactionSequenceNumber, u8StartIndex;
    tsZCL_Address                               sZCL_Address;
    uint16                                      u16hApduSize, u16PayloadSize = 3/* header size */;
#ifdef GP_IEEE_ADDR_SUPPORT
    uint8 j;
#endif
    // initialise pointer
    psGpCustomDataStructure = (tsGP_GreenPowerCustomData *)psClusterInstance->pvEndPointCustomStructPtr;


    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
                pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);

    // get information
    eStatus = eGP_ZgpTranslationTableRequestReceive(
                                pZPSevent,
                                &u8StartIndex);

    /* Check Status */
    if(eStatus != E_ZCL_SUCCESS)
    {
        // release mutex
        #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


        return eStatus;
    }
    /* Get hAPdu Size */
    u16hApduSize = PDUM_u16APduGetSize(hZCL_GetBufferPoolHandle());

    sZgpTransRspCmdPayload.u8Status = E_ZCL_SUCCESS;
    sZgpTransRspCmdPayload.b8Options = GP_APPL_ID_4_BYTE;
    /* Traverse translation table */
    for(i = 0; i < GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES; i++)
    {
        /* check for translation pointer is populated or not */
        if(psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo != NULL)
        {
            u8TotalCount++;
        }
    }

    sZgpTransRspCmdPayload.u8TotalNumOfEntries = u8TotalCount;
#ifdef GP_IEEE_ADDR_SUPPORT
    /* Traverse translation table 2 times - first for GP_APPL_ID_4_BYTE and second for GP_APPL_ID_8_BYTE */
    for(j = 0; j < 2; j++)
    {
#endif
        sZgpTransRspCmdPayload.u8StartIndex = u8StartIndex;

        if(u8StartIndex < GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES)
        {
            /* Traverse translation table */
            for(i = u8StartIndex; i < GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES; i++)
            {
                /* check for translation pointer is populated or not */
                if((psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo != NULL)&&
                  (psGpCustomDataStructure->psZgpsTranslationTableEntry[i].b8Options == sZgpTransRspCmdPayload.b8Options))
                {
                    u8Count++;

                    if(u8Count == 1)
                    {
                        sZgpTransRspCmdPayload.u8StartIndex = i;
                    }
                }
            }
        }

        if(u8Count)
        {
            sZgpTransRspCmdPayload.u8EntriesCount = u8Count;
            u8Count = 0;
            u16PayloadSize += 5; //length of first 5 field of translation table response command payload
            /* Traverse translation table */
            for(i = u8StartIndex; i < GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES; i++)
            {
                /* check for translation pointer is populated or not */
                if((psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo != NULL)&&
                   (psGpCustomDataStructure->psZgpsTranslationTableEntry[i].b8Options == sZgpTransRspCmdPayload.b8Options))
                {
                    sZgpTransRspCmdPayload.asTransTblRspEntry[u8Count].uZgpdDeviceAddr =
                            psGpCustomDataStructure->psZgpsTranslationTableEntry[i].uZgpdDeviceAddr;

                    if(sZgpTransRspCmdPayload.b8Options == GP_APPL_ID_4_BYTE)
                    {
                        u16PayloadSize += 4;
                    }
                    else
                    {
                        u16PayloadSize += 8;
                    }
                    sZgpTransRspCmdPayload.asTransTblRspEntry[u8Count].eZgpdCommandId = psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo->eZgpdCommandId;
                    sZgpTransRspCmdPayload.asTransTblRspEntry[u8Count].u8EndpointId = psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo->u8EndpointId;
                    if(sZgpTransRspCmdPayload.asTransTblRspEntry[u8Count].u8EndpointId == 0xFD)
                    {
                        sZgpTransRspCmdPayload.asTransTblRspEntry[u8Count].u16ProfileID = 0xFFFF;
                    }
                    else
                    {
                        sZgpTransRspCmdPayload.asTransTblRspEntry[u8Count].u16ProfileID = psGpCustomDataStructure->u16ProfileId;
                    }
                    sZgpTransRspCmdPayload.asTransTblRspEntry[u8Count].u16ZbClusterId = psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo->u16ZbClusterId;
                    sZgpTransRspCmdPayload.asTransTblRspEntry[u8Count].u8ZbCommandId = psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo->u8ZbCommandId;
                    sZgpTransRspCmdPayload.asTransTblRspEntry[u8Count].u8ZbCmdLength = psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo->u8ZbCmdLength;
                    u16PayloadSize += 8; // length of fix fields of translation table list field

                    if((psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo->u8ZbCmdLength  != 0)&&
                      (psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo->u8ZbCmdLength  != 0xFF))
                    {
                        memcpy(sZgpTransRspCmdPayload.asTransTblRspEntry[u8Count].au8ZbCmdPayload,
                                psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo->au8ZbCmdPayload,
                                psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo->u8ZbCmdLength);
                        u16PayloadSize += psGpCustomDataStructure->psZgpsTranslationTableEntry[i].psGpToZclCmdInfo->u8ZbCmdLength; // length of ZB command payload of translation table list field
                    }
                    /* check if payload size increase by APDU size than send till last entry updated */
                    if(u16hApduSize < u16PayloadSize)
                    {
                        sZgpTransRspCmdPayload.u8EntriesCount = u8Count;
                        break;
                    }

                    u8Count++;

                    if(u8Count >= GP_MAX_TRANSLATION_RESPONSE_ENTRY)
                    {
                        sZgpTransRspCmdPayload.u8EntriesCount = u8Count;
                        break;
                    }
                }
            }
        }
        else
        {
            sZgpTransRspCmdPayload.u8EntriesCount = 0;
        }

        // build address structure
        eZCL_BuildTransmitAddressStructure(pZPSevent, &sZCL_Address);
#ifndef GP_DISABLE_TRANSLATION_TABLE_RSP_CMD
        eStatus = eGP_ZgpTranslationTableResponseSend(
                            pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                            pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                            &sZCL_Address,
                            &u8TransactionSequenceNumber,
                            &sZgpTransRspCmdPayload);
#endif
#ifdef GP_IEEE_ADDR_SUPPORT
        sZgpTransRspCmdPayload.b8Options = GP_APPL_ID_8_BYTE;
        u8Count = 0;
    }
#endif
    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return eStatus;

}
#endif

/****************************************************************************
 **
 ** NAME:       eGP_HandleZgpTranslationTableUpdate
 **
 ** DESCRIPTION:
 ** Handles Green power Cluster ZGP Translation table update command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_HandleZgpTranslationTableUpdate(
                    ZPS_tsAfEvent                  *pZPSevent,
                    tsZCL_EndPointDefinition       *psEndPointDefinition,
                    tsZCL_ClusterInstance          *psClusterInstance,
                    uint16                         u16Offset)
{
    tsGP_GreenPowerCustomData                   *psGpCustomDataStructure;
    tsGP_ZgpTranslationUpdateCmdPayload         sZgpTransTableUpdatePayload;
    teZCL_Status                                eStatus;
    uint8                                       u8ApplicationId, i, u8NumbersOfTranslations;
    bool_t                                      bTransUpdateFail = FALSE;
    teGP_TranslationTableUpdateAction eAction;
    uint16 u16ClusterId;
    uint8 u8EndPointIndex = 0;
    
    // initialise pointer
    psGpCustomDataStructure = (tsGP_GreenPowerCustomData *)psClusterInstance->pvEndPointCustomStructPtr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
                pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);

    // get information
    eStatus = eGP_ZgpTranslationTableUpdateReceive(
                                pZPSevent,
                                u16Offset,
                                &sZgpTransTableUpdatePayload);

    /* Check Status */
    if(eStatus != E_ZCL_SUCCESS)
    {
        // release mutex
        #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


        return eStatus;
    }

    u8ApplicationId = sZgpTransTableUpdatePayload.b16Options & GP_APPLICATION_ID_MASK;
    eAction = (sZgpTransTableUpdatePayload.b16Options & GP_TRANS_UPDATE_ACTION_MASK) >> 3;

    if(((u8ApplicationId != GP_APPL_ID_4_BYTE)&&(u8ApplicationId != GP_APPL_ID_8_BYTE))||(eAction > E_GP_TRANSLATION_TABLE_REMOVE_ENTRY))
    {
        // release mutex
        #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


        return E_ZCL_ERR_INVALID_VALUE;
    }

    u8NumbersOfTranslations = (sZgpTransTableUpdatePayload.b16Options & GP_TRANS_UPDATE_NUM_OF_TRANSLATIONS_MASK) >> 5;

    for(i=0; i<u8NumbersOfTranslations; i++)
    {
        u16ClusterId = sZgpTransTableUpdatePayload.asTranslationUpdateEntry[i].u16ZbClusterId;

        if(u16ClusterId == 0xFFFF)
        {
            u16ClusterId = u16GP_GetClusterId(sZgpTransTableUpdatePayload.asTranslationUpdateEntry[i].eZgpdCommandId);
        }

        if (((sZgpTransTableUpdatePayload.asTranslationUpdateEntry[i].u8Index < GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES)||
          (sZgpTransTableUpdatePayload.asTranslationUpdateEntry[i].u8Index == 0xFF))&&
          (TRUE == bGP_IsZgpdCommandSupported(sZgpTransTableUpdatePayload.asTranslationUpdateEntry[i].eZgpdCommandId,sZgpTransTableUpdatePayload.asTranslationUpdateEntry[i].u8ZbCommandId, u16ClusterId ))&&
          ((eZCL_SearchForEPIndex(sZgpTransTableUpdatePayload.asTranslationUpdateEntry[i].u8EndpointId,&u8EndPointIndex) == E_ZCL_SUCCESS) || (sZgpTransTableUpdatePayload.asTranslationUpdateEntry[i].u8EndpointId == 0xFF))&&
          (sZgpTransTableUpdatePayload.asTranslationUpdateEntry[i].u16ProfileID == psGpCustomDataStructure->u16ProfileId))
        {

            tsGP_ZgpsTranslationTableUpdate sZgpsTransTableUpdate;

            psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_TRANSLATION_TABLE_UPDATE;
            sZgpsTransTableUpdate.eAction = eAction;
            sZgpsTransTableUpdate.eStatus = E_GP_TRANSLATION_UPDATE_SUCCESS;
            sZgpsTransTableUpdate.u8ApplicationId = u8ApplicationId;
            sZgpsTransTableUpdate.uZgpdDeviceAddr = sZgpTransTableUpdatePayload.uZgpdDeviceAddr;
            sZgpsTransTableUpdate.psTranslationUpdateEntry = &sZgpTransTableUpdatePayload.asTranslationUpdateEntry[i];

            psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psTransationTableUpdate =
                                                                         &sZgpsTransTableUpdate;

            /* Give Application Callback to pass received Translation table response */
            psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);

            if(psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psTransationTableUpdate->eStatus == E_GP_TRANSLATION_UPDATE_FAIL)
            {
                bTransUpdateFail = TRUE;
            }

        }
        else
        {
            bTransUpdateFail = TRUE;
        }
    }

    if(bTransUpdateFail)
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_FAILURE);
        eStatus = E_ZCL_SUCCESS;
    }

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return eStatus;

}

#endif
#ifndef GP_DISABLE_TRANSLATION_TABLE_RSP_CMD
#if((defined GP_COMBO_BASIC_DEVICE) || (defined GP_TEST_HARNESS))
/****************************************************************************
 **
 ** NAME:       eGP_HandleZgpTranslationResponse
 **
 ** DESCRIPTION:
 ** Handles Green power Cluster ZGP translation response command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_HandleZgpTranslationResponse(
                    ZPS_tsAfEvent                  *pZPSevent,
                    tsZCL_EndPointDefinition       *psEndPointDefinition,
                    tsZCL_ClusterInstance          *psClusterInstance,
                    uint16                         u16Offset)
{
    tsGP_GreenPowerCustomData                   *psGpCustomDataStructure;
    tsGP_ZgpTransTableResponseCmdPayload        sZgpTransRspCmdPayload;
    teZCL_Status                                eStatus;

    // initialise pointer
    psGpCustomDataStructure = (tsGP_GreenPowerCustomData *)psClusterInstance->pvEndPointCustomStructPtr;


    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
                pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);

    // get information
    eStatus = eGP_ZgpTranslationTableResponseReceive(
                                pZPSevent,
                                u16Offset,
                                &sZgpTransRspCmdPayload);

    /* Check Status */
    if(eStatus != E_ZCL_SUCCESS)
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_ZgpTranslationTableResponseReceive failed eStatus = %d\n", eStatus);
        // release mutex
        #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


        return eStatus;
    }

    psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_TRANSLATION_TABLE_RESPONSE_RCVD;
    psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psZgpTransRspCmdPayload =
                                                                 &sZgpTransRspCmdPayload;

    /* Give Application Callback to pass received Translation table response */
    psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return eStatus;

}
#endif
#endif
#ifndef GP_DISABLE_ZGP_RESPONSE_CMD
/****************************************************************************
 **
 ** NAME:       eGP_HandleZgpReponse
 **
 ** DESCRIPTION:
 ** Handles Green power Cluster ZGP Response Command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_HandleZgpReponse(
                    ZPS_tsAfEvent                  *pZPSevent,
                    tsZCL_EndPointDefinition       *psEndPointDefinition,
                    tsZCL_ClusterInstance          *psClusterInstance,
                    uint16                         u16Offset)
{
	bool_t                                        bIsProxyTableEntryPresent;
    tsGP_GreenPowerCustomData                   *psGpCustomDataStructure;
    tsGP_ZgpResponseCmdPayload                  sZgpResponseCmdPayload;
    uint8                                       au8Payload[GP_MAX_ZB_CMD_PAYLOAD_LENGTH] = {0};
    tsGP_ZgppProxySinkTable                     *psZgppProxySinkTable;

    /* Get the psGpCustomDataStructure */
    psGpCustomDataStructure = (tsGP_GreenPowerCustomData*)psClusterInstance->pvEndPointCustomStructPtr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

        DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_HandleZgpReponse \r\n");
    /* Initialise string pointer for pay load */
    sZgpResponseCmdPayload.sZgpdCommandPayload.pu8Data = au8Payload;
    sZgpResponseCmdPayload.sZgpdCommandPayload.u8Length = 0;
    sZgpResponseCmdPayload.sZgpdCommandPayload.u8MaxLength = GP_MAX_ZB_CMD_PAYLOAD_LENGTH;

    /* Get the GP Response payload from received packet */
    if(eGP_ZgpResponseReceive(
                    pZPSevent,
                    u16Offset,
                    &sZgpResponseCmdPayload) == E_ZCL_SUCCESS)
    {
        /* Check Temp Master field, if it matches to own network address then add to Tx Queue for transmission to GPD */
        uint16 u16NwkAddr = 0;

#ifndef PC_PLATFORM_BUILD
        u16NwkAddr = ZPS_u16AplZdoGetNwkAddr();
#endif
        bIsProxyTableEntryPresent = 	bGP_IsProxyTableEntryPresent(
        		   	     		psEndPointDefinition->u8EndPointNumber,
        		#ifdef GP_COMBO_BASIC_DEVICE
        							TRUE,
        		#else
        							FALSE,
        		#endif
        							 sZgpResponseCmdPayload.b8Options,
        							 &sZgpResponseCmdPayload.uZgpdDeviceAddr,
        		   	     		
        		   	     		&psZgppProxySinkTable);

        // fill in callback event structure
        eZCL_SetCustomCallBackEvent(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
                    pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);
        psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_RESPONSE_RCVD;
        psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psZgpResponseCmdPayload =
        		&sZgpResponseCmdPayload;
       /* Give Application Callback for sink table update */
        psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);

        if(u16NwkAddr == sZgpResponseCmdPayload.u16TempMasterShortAddr)
        {

            vGP_SendResponseGPDF(TRUE,psEndPointDefinition,psGpCustomDataStructure, &sZgpResponseCmdPayload);
            if( bIsProxyTableEntryPresent)
            {
            	psZgppProxySinkTable->b16Options |= GP_PROXY_TABLE_FIRST_TO_FWD_MASK;
            }
        }
        else
        {
#ifdef GP_COMBO_BASIC_DEVICE
        bGP_RemoveDuplicateGPResponse(psGpCustomDataStructure,&sZgpResponseCmdPayload);
#endif
        	/* remove from queue if already present */
        	vGP_SendResponseGPDF(FALSE,psEndPointDefinition,psGpCustomDataStructure, &sZgpResponseCmdPayload);

        	if( bIsProxyTableEntryPresent)
			{
        		psZgppProxySinkTable->b16Options &= ~GP_PROXY_TABLE_FIRST_TO_FWD_MASK;
			}

        }

    }

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return E_ZCL_SUCCESS;
}

#endif

#ifdef GP_COMBO_BASIC_DEVICE
#ifndef GP_DISABLE_PAIRING_SEARCH_CMD
/****************************************************************************
 **
 ** NAME:       eGP_HandleZgpPairingSearch
 **
 ** DESCRIPTION:
 ** Handles Green power Cluster ZGP pairing search command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eGP_HandleZgpPairingSearch(
                    ZPS_tsAfEvent                  *pZPSevent,
                    tsZCL_EndPointDefinition       *psEndPointDefinition,
                    tsZCL_ClusterInstance          *psClusterInstance,
                    uint16                         u16Offset)
{
    tsGP_GreenPowerCustomData                   *psGpCustomDataStructure;
    tsGP_ZgpPairingSearchCmdPayload             sZgpPairingSearhPayload;
    teZCL_Status                                eStatus;
    uint8                                       u8ApplicationId, u8TransactionSequenceNumber;
    tsGP_ZgppProxySinkTable                     *psSinkTableEntry;
    bool_t                                      bSendPairing = FALSE, bIsSinkTableEntryPresent = FALSE;
    tsGP_ZgpPairingCmdPayload                   sZgpPairingPayload;
    tsZCL_Address                               sDestinationAddress;
    uint16                                      u16AliasShortAddr;
    teGP_GreenPowerCommunicationMode            eCommunicationMode = E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_BOTH;
    // initialise pointer
    psGpCustomDataStructure = (tsGP_GreenPowerCustomData *)psClusterInstance->pvEndPointCustomStructPtr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
                pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);

    // get information
    eStatus = eGP_ZgpPairingSearchReceive(
                                pZPSevent,
                                u16Offset,
                                &sZgpPairingSearhPayload);

    /* Check Status */
    if(eStatus != E_ZCL_SUCCESS)
    {
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif


        return eStatus;
    }

    u8ApplicationId = sZgpPairingSearhPayload.b16Options & GP_APPLICATION_ID_MASK;

    if((u8ApplicationId != GP_APPL_ID_4_BYTE)&&(u8ApplicationId != GP_APPL_ID_8_BYTE))
    {
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif


        return E_ZCL_ERR_INVALID_VALUE;
    }

    if(sZgpPairingSearhPayload.b16Options & GP_PAIRING_SEARCH_RQST_UNICAST_SINK_MASK)
    {
        eCommunicationMode = E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_LIGHTWEIGHT;
    }

    bIsSinkTableEntryPresent = bGP_IsSinkTableEntryPresent(psEndPointDefinition->u8EndPointNumber,
                                            u8ApplicationId,
                                            &sZgpPairingSearhPayload.uZgpdDeviceAddr,
                                            &psSinkTableEntry,
                                            eCommunicationMode);

    if(bIsSinkTableEntryPresent == FALSE)
    {
        if(sZgpPairingSearhPayload.b16Options & GP_PAIRING_SEARCH_RQST_DERIVED_GROUPCAST_SINK_MASK)
        {
            eCommunicationMode = E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID;
        }

        bIsSinkTableEntryPresent = bGP_IsSinkTableEntryPresent(psEndPointDefinition->u8EndPointNumber,
                                                u8ApplicationId,
                                                &sZgpPairingSearhPayload.uZgpdDeviceAddr,
                                                &psSinkTableEntry,
                                                eCommunicationMode);
    }

    if(bIsSinkTableEntryPresent == FALSE)
    {
        if(sZgpPairingSearhPayload.b16Options & GP_PAIRING_SEARCH_RQST_COMMISSION_GROUPCAST_SINK_MASK)
        {
            eCommunicationMode = E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID;
        }

        bIsSinkTableEntryPresent = bGP_IsSinkTableEntryPresent(psEndPointDefinition->u8EndPointNumber,
                                                u8ApplicationId,
                                                &sZgpPairingSearhPayload.uZgpdDeviceAddr,
                                                &psSinkTableEntry,
                                                eCommunicationMode);
    }


    if(bIsSinkTableEntryPresent)
    {

        /* check requested and supported communication mode */

        /* Get communication mode */
        eCommunicationMode = (psSinkTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK);

        if((eCommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)&&
           (sZgpPairingSearhPayload.b16Options & GP_PAIRING_SEARCH_RQST_DERIVED_GROUPCAST_SINK_MASK))
        {
            bSendPairing = TRUE;
        }
        else if((eCommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)&&
                (sZgpPairingSearhPayload.b16Options & GP_PAIRING_SEARCH_RQST_COMMISSION_GROUPCAST_SINK_MASK))
        {
            bSendPairing = TRUE;
        }
        if(bSendPairing)
        {
            sZgpPairingPayload.b24Options = u8ApplicationId;
            sZgpPairingPayload.b24Options |= GP_PAIRING_ADD_SINK_MASK;
            sZgpPairingPayload.b24Options |= eCommunicationMode << 5;

            if(psSinkTableEntry->b16Options & GP_PROXY_TABLE_GPD_FIXED_MASK)
            {
                sZgpPairingPayload.b24Options |= GP_PAIRING_GPD_FIXED_MASK;
            }

            if(psSinkTableEntry->b16Options & GP_PROXY_TABLE_SEQ_NUM_CAP_MASK)
            {
                sZgpPairingPayload.b24Options |= GP_PAIRING_SEQ_NUM_CAP_MASK;
            }

            if(psSinkTableEntry->b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK)
            {
                teGP_GreenPowerSecLevel    eSecLevel;
                teGP_GreenPowerSecKeyType  eSecKeyType;

                eSecLevel = psSinkTableEntry->b8SecOptions & GP_SECURITY_LEVEL_MASK;
                eSecKeyType = (psSinkTableEntry->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2;

                sZgpPairingPayload.b24Options |= eSecLevel << 9;
                sZgpPairingPayload.b24Options |= eSecKeyType << 11;
            }

            if(sZgpPairingSearhPayload.b16Options & GP_PAIRING_SEARCH_RQST_SECURITY_COUNTER_MASK)
            {
                sZgpPairingPayload.b24Options |= GP_PAIRING_SECURITY_COUNTER_PRESENT_MASK;
                sZgpPairingPayload.u32ZgpdSecFrameCounter = psSinkTableEntry->u32ZgpdSecFrameCounter;
            }

            if(sZgpPairingSearhPayload.b16Options & GP_PAIRING_SEARCH_RQST_SECURITY_KEY_MASK)
            {
                sZgpPairingPayload.b24Options |= GP_PAIRING_SECURITY_KEY_PRESENT_MASK;
                sZgpPairingPayload.sZgpdKey = psSinkTableEntry->sZgpdKey;
            }

            if(psSinkTableEntry->b16Options & GP_PROXY_TABLE_ASSIGNED_ALIAS_MASK)
            {
                sZgpPairingPayload.b24Options |= GP_PAIRING_ASSIGNED_ALIAS_PRESENT_MASK;
                sZgpPairingPayload.u16AssignedAlias = psSinkTableEntry->u16ZgpdAssignedAlias;
            }

            sZgpPairingPayload.b24Options |= GP_PAIRING_FORWARDING_RADIUS_PRESENT_MASK;
            sZgpPairingPayload.u8ForwardingRadius = psSinkTableEntry->u8GroupCastRadius;

            sZgpPairingPayload.uZgpdDeviceAddr = sZgpPairingSearhPayload.uZgpdDeviceAddr;
            if(eCommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)
            {
                sZgpPairingPayload.u16SinkGroupID = u16GP_DeriveAliasSrcAddr(u8ApplicationId, sZgpPairingSearhPayload.uZgpdDeviceAddr);
            }
            else if(eCommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
            {
                sZgpPairingPayload.u16SinkGroupID = psSinkTableEntry->asSinkGroupList[0].u16SinkGroup;
                sZgpPairingPayload.u16AssignedAlias = psSinkTableEntry->asSinkGroupList[0].u16Alias;
                sZgpPairingPayload.b24Options |= GP_PAIRING_ASSIGNED_ALIAS_PRESENT_MASK;
            }

            sZgpPairingPayload.u8DeviceId = psSinkTableEntry->eZgpdDeviceId;

            sDestinationAddress.eAddressMode = E_ZCL_AM_BROADCAST;
            sDestinationAddress.uAddress.eBroadcastMode = ZPS_E_APL_AF_BROADCAST_RX_ON;
            eStatus = eGP_ZgpPairingSend(
                                psEndPointDefinition->u8EndPointNumber,
                                ZCL_GP_PROXY_ENDPOINT_ID,
                                &sDestinationAddress,
                                &u8TransactionSequenceNumber,
                                &sZgpPairingPayload);

            if(eCommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
            {
                uint8 i;
                for(i = 1; i < psSinkTableEntry->u8SinkGroupListEntries; i++)
                {
                    sZgpPairingPayload.u16SinkGroupID = psSinkTableEntry->asSinkGroupList[i].u16SinkGroup;
                    sZgpPairingPayload.u16AssignedAlias = psSinkTableEntry->asSinkGroupList[i].u16Alias;
                    sZgpPairingPayload.b24Options |= GP_PAIRING_ASSIGNED_ALIAS_PRESENT_MASK;
                    /* Send GP Pairing command as broadcast */
                    eGP_ZgpPairingSend(
                            psEndPointDefinition->u8EndPointNumber,
                            ZCL_GP_PROXY_ENDPOINT_ID,
                            &sDestinationAddress,
                            &u8TransactionSequenceNumber,
                            &sZgpPairingPayload);
                }
            }
            /* send device announce */
            if(eCommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
            {
                uint8 i;
                for(i = 0; i < psSinkTableEntry->u8SinkGroupListEntries; i++)
                {
                    u16AliasShortAddr = psSinkTableEntry->asSinkGroupList[i].u16Alias;
                	/* send device announce */
                	vGP_SendDeviceAnnounce(u16AliasShortAddr,0xFFFFFFFFFFFFFFFFULL);
                }
            }
            else
            {
                if(psSinkTableEntry->b16Options & GP_PROXY_TABLE_ASSIGNED_ALIAS_MASK)
                {
                    u16AliasShortAddr = psSinkTableEntry->u16ZgpdAssignedAlias;
                }
                else
                {
                    u16AliasShortAddr = u16GP_DeriveAliasSrcAddr(u8ApplicationId,
                                                                 sZgpPairingPayload.uZgpdDeviceAddr);
                }
            	/* send device announce */
            	vGP_SendDeviceAnnounce(u16AliasShortAddr, 0xFFFFFFFFFFFFFFFFULL);
            }

        }
    }
    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return eStatus;

}
#endif

/****************************************************************************
 **
 ** NAME:       eGP_HandleZgpPairingConfig
 **
 ** DESCRIPTION:
 ** Handles Green power Cluster ZGP pairing configuration command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_HandleZgpPairingConfig(
                    ZPS_tsAfEvent                  *pZPSevent,
                    tsZCL_EndPointDefinition       *psEndPointDefinition,
                    tsZCL_ClusterInstance          *psClusterInstance,
                    uint16                         u16Offset)
{
    tsGP_GreenPowerCustomData                   *psGpCustomDataStructure;
    tsGP_ZgpPairingConfigCmdPayload             sZgpPairingConfigPayload = {0};
    bool_t 										bIsSinkTablePresent;
    uint8		                                u8Status,i;
    uint8                                       u8ApplicationId ;
    tsGP_ZgppProxySinkTable                      *psSinkTableEntry ;
    teGP_GreenPowerPairingConfigAction          ePairingConfigAction;
    teGP_GreenPowerCommunicationMode            eCommMode;
    // initialise pointer
    psGpCustomDataStructure = (tsGP_GreenPowerCustomData *)psClusterInstance->pvEndPointCustomStructPtr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
                pZPSevent, 0, psEndPointDefinition->u8EndPointNumber);

    // get information
    u8Status = eGP_ZgpPairingConfigReceive(
                                pZPSevent,
                                u16Offset,
                                &sZgpPairingConfigPayload);

    /* Check Status */
    if(u8Status != E_ZCL_SUCCESS)
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, "\eGP_ZgpPairingConfigReceive Failed %d \n", u8Status);
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif


        return u8Status;
    }
    psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_PAIRING_CONFIG_CMD_RCVD;
    psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psZgpPairingConfigCmdPayload =
		&sZgpPairingConfigPayload;
    psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);

    ePairingConfigAction = sZgpPairingConfigPayload.u8Actions & GP_PAIRING_CONFIG_ACTION_MASK;

    u8ApplicationId = sZgpPairingConfigPayload.b16Options & GP_APPLICATION_ID_MASK;
    DBG_vPrintf(TRACE_GP_DEBUG, "E_GP_PAIRING_CONFIG_CMD_RCVD ind given u8ApplicationId = %d \n",
    		u8ApplicationId);
    if(bValidityCheckPairingConfig(pZPSevent, &sZgpPairingConfigPayload, (tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr)) == FALSE)
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, "\bValidityCheckPairingConfig Failed %d \n", u8Status);
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif


        return E_ZCL_SUCCESS;
    }

    DBG_vPrintf(TRACE_GP_DEBUG, "bValidityCheckPairingConfig done \n");
    if(E_GP_PAIRING_CONFIG_REMOVE_GPD == ePairingConfigAction)
    {
    	eCommMode = 0xff;
    }
    else
    {
    	eCommMode = (teGP_GreenPowerCommunicationMode)((sZgpPairingConfigPayload.b16Options & GP_PAIRING_CONFIG_COMM_MODE_MASK) >> 3);
    }
    bIsSinkTablePresent = bGP_IsSinkTableEntryPresent(psEndPointDefinition->u8EndPointNumber,
                                   u8ApplicationId,
                                   &sZgpPairingConfigPayload.uZgpdDeviceAddr,
                                   &psSinkTableEntry,
                                   eCommMode);
   /* In case of endpoint 0xFE , we need to pair to endpoints that are supported in the groups.
    * Check if we support any group with the group id given */

    if((bCheckIfAtleastOneGroupSupported(&sZgpPairingConfigPayload) == FALSE) &&
    		(sZgpPairingConfigPayload.u8NumberOfPairedEndpoints== 0xFE) &&
    		((teGP_GreenPowerCommunicationMode)((sZgpPairingConfigPayload.b16Options & GP_PAIRING_CONFIG_COMM_MODE_MASK) >> 3) ==
    				E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID) &&
    				(ePairingConfigAction != E_GP_PAIRING_CONFIG_REMOVE_SINK_TABLE_ENTRY) &&
    				(ePairingConfigAction != E_GP_PAIRING_CONFIG_REMOVE_GPD) )
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "\n bCheckIfAtleastOneGroupSupported failed  \n");
#ifndef COOPERATIVE
		eZCL_ReleaseMutex(psEndPointDefinition);
#endif
		return E_ZCL_SUCCESS;
	}
    if(bIsSinkTablePresent)
    {

    	if((ePairingConfigAction == E_GP_PAIRING_CONFIG_NO_ACTION))
    	{
    		vSendpairing(psEndPointDefinition->u8EndPointNumber, FALSE, &sZgpPairingConfigPayload, psSinkTableEntry);
#ifndef COOPERATIVE
				eZCL_ReleaseMutex(psEndPointDefinition);
#endif
				return E_ZCL_SUCCESS;
    	}

        if(ePairingConfigAction == E_GP_PAIRING_CONFIG_REMOVE_SINK_TABLE_ENTRY)
        {
        	DBG_vPrintf(TRACE_GP_DEBUG, "\n Removing pairing  \n");
        	vSendPairingRemoveGroup(psEndPointDefinition,psGpCustomDataStructure,&sZgpPairingConfigPayload, psSinkTableEntry);

#ifndef COOPERATIVE
				eZCL_ReleaseMutex(psEndPointDefinition);
#endif
				return E_ZCL_SUCCESS;
        }
        else if((ePairingConfigAction == E_GP_PAIRING_CONFIG_REPLACE_SINK_TABLE_ENTRY) || (E_GP_PAIRING_CONFIG_REMOVE_GPD == ePairingConfigAction))
        {
        	vRemoveGroupAndSinkTableOnReplace(psEndPointDefinition,psGpCustomDataStructure,&sZgpPairingConfigPayload, psSinkTableEntry);
        	if((E_GP_PAIRING_CONFIG_REMOVE_GPD == ePairingConfigAction)||(sZgpPairingConfigPayload.u8NumberOfPairedEndpoints== 0xFD)||
        			  (sZgpPairingConfigPayload.u8NumberOfPairedEndpoints == 0x00))
        	{
        		vGiveIndicationForTranslationTableUpdate(
        				E_GP_PAIRING_CONFIG_TRANSLATION_TABLE_REMOVE_ENTRY,
        				psEndPointDefinition,
        				psGpCustomDataStructure,
        				&sZgpPairingConfigPayload);
#ifndef COOPERATIVE
				eZCL_ReleaseMutex(psEndPointDefinition);
#endif
				return E_ZCL_SUCCESS;

        	}
        }

    }
    else
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_HandleZgpPairingConfig bIsSinkTablePresent false 0x%lx \n",
    			u8ApplicationId,sZgpPairingConfigPayload.uZgpdDeviceAddr.u32ZgpdSrcId);
    }

    if((ePairingConfigAction == E_GP_PAIRING_CONFIG_EXTEND_SINK_TABLE_ENTRY)||
      (ePairingConfigAction == E_GP_PAIRING_CONFIG_REPLACE_SINK_TABLE_ENTRY))
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, "\n Extend / replace sink entry  \n");
    	/* allocate new entry if the communication mode is different */
    	if((bIsSinkTablePresent) && ( (sZgpPairingConfigPayload.b16Options & GP_PAIRING_CONFIG_COMM_MODE_MASK) >> 3) !=
    			(psSinkTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK))
    	{
    		/* Sink table is updated below */
    		DBG_vPrintf(TRACE_GP_DEBUG, "\n Adding new to support new communication mode  \n");
    		bIsSinkTablePresent = FALSE;
    	}
    	else
    	{
    		if(bIsSinkTablePresent)
    		{
    			uint8 u8GroupIndex = 0xFF, i;
    			for(i=0 ; i< sZgpPairingConfigPayload.u8SinkGroupListEntries; i++)
    			{
					u8GroupIndex = u8AddGroupIdToSinkTable(  sZgpPairingConfigPayload.asSinkGroupList[i].u16SinkGroup,
							  sZgpPairingConfigPayload.asSinkGroupList[i].u16Alias,psSinkTableEntry);
					DBG_vPrintf(TRACE_GP_DEBUG, "\n Extending sink table to support more EP  u8GroupIndex = %d \n", u8GroupIndex);
					if((sZgpPairingConfigPayload.u8Actions & GP_PAIRING_CONFIG_SEND_GP_PAIRING_MASK)&& (u8GroupIndex != 0xFF))
					{

						vFillAndSendPairingCmdFromSinkTable(psEndPointDefinition->u8EndPointNumber,
								GP_PAIRING_ADD_SINK_MASK,
								u8GroupIndex,
								psSinkTableEntry);
					}
					vGiveIndicationForTranslationTableUpdate(E_GP_PAIRING_CONFIG_TRANSLATION_TABLE_EXTEND_ENTRY,psEndPointDefinition,psGpCustomDataStructure, &sZgpPairingConfigPayload);
    			}
    			if((sZgpPairingConfigPayload.u8Actions & GP_PAIRING_CONFIG_SEND_GP_PAIRING_MASK) && (eCommMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID))
				{

					vFillAndSendPairingCmdFromSinkTable(psEndPointDefinition->u8EndPointNumber,
							GP_PAIRING_ADD_SINK_MASK,
							0,
							psSinkTableEntry);
				}

#ifndef COOPERATIVE
				eZCL_ReleaseMutex(psEndPointDefinition);
#endif
				return E_ZCL_SUCCESS;


    		}
    	}

    	/* reject the frame if number of paired endpoint is 0 or 0xFD */
    	if((bIsSinkTablePresent == FALSE) &&(!( (sZgpPairingConfigPayload.u8NumberOfPairedEndpoints == 0x00) ||
    			(sZgpPairingConfigPayload.u8NumberOfPairedEndpoints == 0xFD))))
    	{

    		bIsSinkTablePresent = bGP_GetFreeProxySinkTableEntry(psEndPointDefinition->u8EndPointNumber,
	                    TRUE,
	                    &psSinkTableEntry);
    		DBG_vPrintf(TRACE_GP_DEBUG, "\nAllocating new sink table entry    \n");
    		if(bIsSinkTablePresent == FALSE)
    		{
    			DBG_vPrintf(TRACE_GP_DEBUG, "\bbGP_GetFreeProxySinkTableEntry Failed %d \n", bIsSinkTablePresent);
#ifndef COOPERATIVE
    			eZCL_ReleaseMutex(psEndPointDefinition);
#endif
    			return E_ZCL_FAIL;
    		}

    	}
    	else
    	{

    		DBG_vPrintf(TRACE_GP_DEBUG, "\n Not updating  sZgpPairingConfigPayload.u8NumberOfPairedEndpoints %d \n", sZgpPairingConfigPayload.u8NumberOfPairedEndpoints);
#ifndef COOPERATIVE
    			eZCL_ReleaseMutex(psEndPointDefinition);
#endif
    			return E_ZCL_SUCCESS;
    	}

        /* Populate sink table entry */
        psSinkTableEntry->b16Options = sZgpPairingConfigPayload.b16Options & GP_APPLICATION_ID_MASK; // application id
        psSinkTableEntry->b16Options |= GP_PROXY_TABLE_ENTRY_ACTIVE_MASK;
        psSinkTableEntry->b16Options |= GP_PROXY_TABLE_ENTRY_VALID_MASK;
        psSinkTableEntry->b16Options |= sZgpPairingConfigPayload.b16Options & GP_PROXY_TABLE_SEQ_NUM_CAP_MASK;
        psSinkTableEntry->b8SinkOptions =( sZgpPairingConfigPayload.b16Options & GP_PAIRING_CONFIG_COMM_MODE_MASK) >> 3;
        psSinkTableEntry->b8SinkOptions |= ((sZgpPairingConfigPayload.b16Options & 0x40) >> 6) << 4; //Rx On
        psSinkTableEntry->b16Options |= ((sZgpPairingConfigPayload.b16Options & 0x80) >> 7) << 11; //Fixed loc
        psSinkTableEntry->b16Options |= ((sZgpPairingConfigPayload.b16Options & GP_PAIRING_CONFIG_ASSIGNED_ADDR_MASK) >> 8) << 13; //Assigned alias
        DBG_vPrintf(TRACE_GP_DEBUG, "\n  psSinkTableEntry->b16Options 5 = %d \n",  psSinkTableEntry->b16Options);
        psSinkTableEntry->b16Options |= ((sZgpPairingConfigPayload.b16Options & GP_PAIRING_CONFIG_SECURITY_USE_MASK )>> 9) << 14; //Sec use
        if(eCommMode == E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_LIGHTWEIGHT)
        {
        	psSinkTableEntry->b16Options |= GP_PROXY_TABLE_UNICAST_GPS_MASK;
        }
        else if (eCommMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)
        {
        	psSinkTableEntry->b16Options |= GP_PROXY_TABLE_DERIVED_GROUP_GPS_MASK;
        }
        else if(eCommMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
        {
        	psSinkTableEntry->b16Options |= GP_PROXY_TABLE_COMMISSION_GROUP_GPS_MASK;
        }

        if(u8ApplicationId == GP_APPL_ID_4_BYTE)
        {
            psSinkTableEntry->uZgpdDeviceAddr.u32ZgpdSrcId = sZgpPairingConfigPayload.uZgpdDeviceAddr.u32ZgpdSrcId;
        }
#ifdef GP_IEEE_ADDR_SUPPORT
        else
        {
            psSinkTableEntry->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr =
            		sZgpPairingConfigPayload.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
            psSinkTableEntry->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint =
            		sZgpPairingConfigPayload.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
        }
#endif
        //psSinkTableEntry->uZgpdDeviceAddr = sZgpPairingConfigPayload.uZgpdDeviceAddr;
        psSinkTableEntry->eZgpdDeviceId = sZgpPairingConfigPayload.eZgpdDeviceId;
        psSinkTableEntry->u16ZgpdAssignedAlias = sZgpPairingConfigPayload.u16ZgpdAssignedAlias;
        psSinkTableEntry->u8GroupCastRadius = sZgpPairingConfigPayload.u8ForwardingRadius;
        psSinkTableEntry->b8SecOptions = sZgpPairingConfigPayload.b8SecOptions;
        psSinkTableEntry->u32ZgpdSecFrameCounter = sZgpPairingConfigPayload.u32ZgpdSecFrameCounter;
        psSinkTableEntry->sZgpdKey = sZgpPairingConfigPayload.sZgpdKey;
		psSinkTableEntry->u8GPDPaired = E_GP_PAIRED;
        psSinkTableEntry->bProxyTableEntryOccupied = 1;

		if((teGP_GreenPowerCommunicationMode)((sZgpPairingConfigPayload.b16Options & GP_PAIRING_CONFIG_COMM_MODE_MASK) >> 3) ==
							E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
		{
			for(i=0; i < sZgpPairingConfigPayload.u8SinkGroupListEntries; i++ )
			{

				psSinkTableEntry->asSinkGroupList[i].u16SinkGroup = sZgpPairingConfigPayload.asSinkGroupList[i].u16SinkGroup;
				psSinkTableEntry->asSinkGroupList[i].u16Alias = sZgpPairingConfigPayload.asSinkGroupList[i].u16Alias;
				psSinkTableEntry->u8SinkGroupListEntries++;
			}
		}

        DBG_vPrintf(TRACE_GP_DEBUG, "\n Updating sink table 4  psSinkTableEntry->asSinkGroupList = %d \n", psSinkTableEntry->u8SinkGroupListEntries);
        psSinkTableEntry->eGreenPowerSinkTablePriority = E_GP_SINK_TABLE_P_2;

        vGP_CallbackForPersistData();
        /* give call back accordingly */


		psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_SINK_PROXY_TABLE_ENTRY_ADDED;
		psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psZgpsProxySinkTable = psSinkTableEntry;


        /* Give Application Callback for sink table update */
        psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
        vSendpairing(psEndPointDefinition->u8EndPointNumber, TRUE, &sZgpPairingConfigPayload, psSinkTableEntry);


		DBG_vPrintf(TRACE_GP_DEBUG, "\n vGP_AddGPEPToGroup\n");
		/* Add GPEP to group at APS level */
		vGP_AddGPEPToGroup(psGpCustomDataStructure, psEndPointDefinition, psSinkTableEntry);
		vGiveIndicationForTranslationTableUpdate(E_GP_PAIRING_CONFIG_TRANSLATION_TABLE_ADD_ENTRY,psEndPointDefinition,psGpCustomDataStructure, &sZgpPairingConfigPayload);
	}

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif
    return E_ZCL_SUCCESS;
}
#endif

/****************************************************************************
 **
 ** NAME:       bGP_CheckIfCmdToBeBuffered
 **
 ** DESCRIPTION:
 ** Checks whether command to be buffered
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
bool_t bGP_CheckIfCmdToBeBuffered(
		tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
		tsGP_ZgpCommissioningNotificationCmdPayload *psZgpCommissioningNotificationCmdPayload)
{

	/* check any GP channel request/Commission Packet buffer as it can receive over directly or tunnel notification */
	tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord;

	/* take head of list */
	psZgpBufferedApduRecord = (tsGP_ZgpBufferedApduRecord *)psDLISTgetHead(&psGpCustomDataStructure->lGpAllocList);

	if(psZgpBufferedApduRecord == NULL)
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "No Buffered Lists\r\n");
		return TRUE;
	}

	/* traverse through list look for this cmd Id and GP Device address */
	while(psZgpBufferedApduRecord)
	{
		uint8 u8Distance = psZgpBufferedApduRecord->sBufferedApduInfo.u8GPP_GPDLink;


		/* check command ID and GP device address */
		if( (psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId == E_GP_CHANNEL_REQUEST ||
				psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId == E_GP_COMMISSIONING) &&
			(psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand == E_GP_COMMISSION_NOTIFICATION ||
				psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand == E_GP_RESPONSE) &&
			( (psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.u32ZgpdSrcId ==
											psZgpCommissioningNotificationCmdPayload->uZgpdDeviceAddr.u32ZgpdSrcId &&
			   psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId == GP_APPL_ID_4_BYTE)
#ifdef GP_IEEE_ADDR_SUPPORT
				|| (psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr ==
											psZgpCommissioningNotificationCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr &&
					psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId == GP_APPL_ID_8_BYTE)
#endif
		))
		{
			/* If device is proxy, stop transmission if proxy distance of received packet is less */
			/* check proxy distance
			 * Delete buffered record from the list, if proxy distance of rxed pkt is less or
			 * distance same and address of temp address is lower than previous */
			if( (psGpCustomDataStructure->eGreenPowerDeviceType == E_GP_ZGP_PROXY_BASIC_DEVICE) &&
			( (bBestGPD_GPPLink(psZgpCommissioningNotificationCmdPayload->u8GPP_GPD_Link, u8Distance) == LINK_MORE) ||
					((bBestGPD_GPPLink(psZgpCommissioningNotificationCmdPayload->u8GPP_GPD_Link, u8Distance) == LINK_EQUAL) &&
				  (psZgpCommissioningNotificationCmdPayload->u16ZgppShortAddr <
					psZgpBufferedApduRecord->sBufferedApduInfo.u16NwkShortAddr) ) ))

			{
				tsGP_ZgpBufferedApduRecord                  *psTempZgpBufferedApduRecord;

				/* For combo min device, it has to check any GP response packet buffered or not */
				/* get the next list */
				psTempZgpBufferedApduRecord = (tsGP_ZgpBufferedApduRecord *)psDLISTgetNext((DNODE *)psZgpBufferedApduRecord);

				/* Delete Node */
				psDLISTremove(&psGpCustomDataStructure->lGpAllocList,
						(DNODE *)psZgpBufferedApduRecord);

				// add to free list
				vDLISTaddToTail(&psGpCustomDataStructure->lGpDeAllocList,
						(DNODE *)psZgpBufferedApduRecord);


				/* If no records in the list, break the loop */
				if(psTempZgpBufferedApduRecord == NULL)
				{
					return FALSE;
				}

			}
		}

		/* If device is combo min, device has to select temp master */
		if( psGpCustomDataStructure->eGreenPowerDeviceType == E_GP_ZGP_COMBO_BASIC_DEVICE &&
			(psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId == E_GP_CHANNEL_REQUEST ||
					psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId == E_GP_COMMISSIONING) &&
			psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand == E_GP_RESPONSE &&
			( (psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.u32ZgpdSrcId ==
											psZgpCommissioningNotificationCmdPayload->uZgpdDeviceAddr.u32ZgpdSrcId &&
					psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId == GP_APPL_ID_4_BYTE)
#ifdef GP_IEEE_ADDR_SUPPORT
				|| (psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr ==
											psZgpCommissioningNotificationCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr &&
					psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId == GP_APPL_ID_8_BYTE)
#endif
		))
		{
			/* check proxy distance
			 * if proxy distance of rxed pkt is less or
			 * distance same and address of temp address is lower than previous */
            if(  (bBestGPD_GPPLink(psZgpCommissioningNotificationCmdPayload->u8GPP_GPD_Link, u8Distance) == LINK_MORE) ||
    				((bBestGPD_GPPLink(psZgpCommissioningNotificationCmdPayload->u8GPP_GPD_Link, u8Distance) == LINK_EQUAL) &&
        		  (psZgpCommissioningNotificationCmdPayload->u16ZgppShortAddr <
                    psZgpBufferedApduRecord->sBufferedApduInfo.u16NwkShortAddr) )  )

			{
				/* Overwrite previous record */
				psZgpBufferedApduRecord->sBufferedApduInfo.u16NwkShortAddr =
												psZgpCommissioningNotificationCmdPayload->u16ZgppShortAddr;
				psZgpBufferedApduRecord->sBufferedApduInfo.u8GPP_GPDLink = psZgpCommissioningNotificationCmdPayload->u8GPP_GPD_Link;
				DBG_vPrintf(TRACE_GP_DEBUG, "\n Direct command overwrote u8GPP_GPDLink = %d, u8Distance = %d \n",
						psZgpCommissioningNotificationCmdPayload->u8GPP_GPD_Link,u8Distance);

				return FALSE;

			}
            else
            {
				DBG_vPrintf(TRACE_GP_DEBUG, "\n Comm Notification with Less GPP GPD link  u8GPP_GPDLink = %d, u8Distance = %d \n",
						psZgpCommissioningNotificationCmdPayload->u8GPP_GPD_Link,u8Distance);

            }
		}
		else
		{
			DBG_vPrintf(TRACE_GP_DEBUG, "\n Buffered command id = %d, received = %d, delay = %d \n",
					psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId,	psZgpCommissioningNotificationCmdPayload->u8ZgpdCmdId,
					psZgpBufferedApduRecord->sBufferedApduInfo.u16Delay);
		}

		/* get the next list */
		psZgpBufferedApduRecord = (tsGP_ZgpBufferedApduRecord *)psDLISTgetNext((DNODE *)psZgpBufferedApduRecord);
	}
	return TRUE;

}
/****************************************************************************
 **
 ** NAME:       bGP_CheckIfCmdToBeNotified
 **
 ** DESCRIPTION:
 ** Checks whether command to be buffered
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
bool_t bGP_CheckIfCmdToBeNotified(
		tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
		tsGP_ZgpDataIndication                       *psZgpDataIndication)
{

	/* check any GP channel request/Commission Packet buffer as it can receive over directly or tunnel notification */
	tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord;

	/* take head of list */
	psZgpBufferedApduRecord = (tsGP_ZgpBufferedApduRecord *)psDLISTgetHead(&psGpCustomDataStructure->lGpAllocList);

	if(psZgpBufferedApduRecord == NULL)
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "No Buffered Lists\r\n");
		return TRUE;
	}

	/* traverse through list look for this cmd Id and GP Device address */
	while(psZgpBufferedApduRecord)
	{
		uint8 u8Distance = psZgpBufferedApduRecord->sBufferedApduInfo.u8GPP_GPDLink;


		/* check command ID and GP device address */
		if( (psZgpBufferedApduRecord->sBufferedApduInfo.u8CommandId ==  psZgpDataIndication->u8CommandId) &&
			( (psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.u32ZgpdSrcId ==
					psZgpDataIndication->uZgpdDeviceAddr.u32ZgpdSrcId &&
			   psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId == GP_APPL_ID_4_BYTE)
#ifdef GP_IEEE_ADDR_SUPPORT
				|| (psZgpBufferedApduRecord->sBufferedApduInfo.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr ==
						psZgpDataIndication->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr &&
					psZgpBufferedApduRecord->sBufferedApduInfo.u2ApplicationId == GP_APPL_ID_8_BYTE)
#endif
		) &&(psZgpBufferedApduRecord->sBufferedApduInfo.u32SecFrameCounter ==  psZgpDataIndication->u32SecFrameCounter)  )
		{
			/* If device is proxy, stop transmission if proxy distance of received packet is less */
			/* check proxy distance
			 * Delete buffered record from the list, if proxy distance of rxed pkt is less or
			 * distance same and address of temp address is lower than previous */
			if(	( (bBestGPD_GPPLink(psZgpDataIndication->u8GPP_GPDLink, u8Distance) == LINK_MORE) ||
					((bBestGPD_GPPLink(psZgpDataIndication->u8GPP_GPDLink, u8Distance) == LINK_EQUAL) &&
				  (psZgpDataIndication->u16NwkShortAddr <
					psZgpBufferedApduRecord->sBufferedApduInfo.u16NwkShortAddr) ) ))

			{

				/* Delete Node */
				psDLISTremove(&psGpCustomDataStructure->lGpAllocList,
						(DNODE *)psZgpBufferedApduRecord);

				// add to free list
				vDLISTaddToTail(&psGpCustomDataStructure->lGpDeAllocList,
						(DNODE *)psZgpBufferedApduRecord);
				return FALSE;
			}
		}

		/* get the next list */
		psZgpBufferedApduRecord = (tsGP_ZgpBufferedApduRecord *)psDLISTgetNext((DNODE *)psZgpBufferedApduRecord);
	}
	return TRUE;

}


#ifdef GP_COMBO_BASIC_DEVICE
/****************************************************************************
 **
 ** NAME:       bGP_RemoveDuplicateGPResponse
 **
 ** DESCRIPTION:
 ** Removes the response
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint16                    u16Offset                 offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
void bGP_RemoveDuplicateGPResponse(
		tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
		tsGP_ZgpResponseCmdPayload                  *psZgpResponseCmdPayload )
{
	bool_t                                        bIsHeadNode = FALSE;

	/* check any GP channel request/Commission Packet buffer as it can receive over directly or tunnel notification */
	tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord;

	/* take head of list */
	psZgpBufferedApduRecord = (tsGP_ZgpBufferedApduRecord *)psDLISTgetHead(&psGpCustomDataStructure->lGpAllocList);
	DBG_vPrintf(TRACE_GP_DEBUG, "bGP_RemoveDuplicateGPResponse \r\n");
	if(psZgpBufferedApduRecord == NULL)
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "No Buffered Lists\r\n");
		return;
	}

	/* traverse through list look for this cmd Id and GP Device address */
	while(psZgpBufferedApduRecord)
	{
		/* check command ID and GP device address */
		if( psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand == E_GP_ADD_MSG_TO_TX_QUEUE_AFTER_DELAY )

		{
			tsGP_ZgpBufferedApduRecord *psTempZgpBufferedApduRecord =
													(tsGP_ZgpBufferedApduRecord *)psDLISTgetHead(&psGpCustomDataStructure->lGpAllocList);
			DBG_vPrintf(TRACE_GP_DEBUG, "Remove Msg Duplicate Response \r\n");
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
        	if (psGpCustomDataStructure->u16TransmitChannelTimeout)
        	{
        		psGpCustomDataStructure->u16TransmitChannelTimeout = 0x00;
				ZPS_vNwkNibSetChannel(ZPS_pvAplZdoGetNwkHandle(),
						psGpCustomDataStructure->u8OperationalChannel);
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
#endif
/****************************************************************************
 **
 ** NAME:       vGP_SendResponseGPDF
 **
 ** DESCRIPTION:
 ** Buffers GPDF response in TxQueue
 **
 ** PARAMETERS:               Name                      Usage
 ** uint8                        u8Handle,
 ** tsGP_ZgpResponseCmdPayload   *psZgpResponseCmdPayload
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/
void vGP_SendResponseGPDF(
		bool_t                                         bAddMsg,
		 tsZCL_EndPointDefinition                    *psEndPointDefinition,
		 tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
		  tsGP_ZgpResponseCmdPayload                  *psZgpResponseCmdPayload)
{
//	uint8 *pu8Start;
	ZPS_tsAfZgpGreenPowerReq                   sZgpDataReq;
   // PDUM_thAPduInstance                         hAPduInst;
 //   uint8 										eZPSStatus, i;
    memset(&sZgpDataReq,0,sizeof(sZgpDataReq));

	sZgpDataReq.u8TxOptions = 0x01;
	//sZgpDataReq.bDataFrame = TRUE;
	sZgpDataReq.u8ApplicationId = psZgpResponseCmdPayload->b8Options;

	if(sZgpDataReq.u8ApplicationId == GP_APPL_ID_4_BYTE)
	{
		sZgpDataReq.uGpId.u32SrcId = psZgpResponseCmdPayload->uZgpdDeviceAddr.u32ZgpdSrcId;
	}

#ifdef GP_IEEE_ADDR_SUPPORT
	else
	{
		sZgpDataReq.uGpId.u64Address =
				psZgpResponseCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
	   sZgpDataReq.u8Endpoint =
				psZgpResponseCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
	}
#endif
	sZgpDataReq.u16TxQueueEntryLifetime = 5000; //5 sec
	sZgpDataReq.u8Handle = u8GP_GetDataReqHandle(psGpCustomDataStructure);
	sZgpDataReq.u8SeqNum = u8GetTransactionSequenceNumber();
	sZgpDataReq.u16Panid = 0xFFFF;
	sZgpDataReq.u16DstAddr = 0xFFFF;
    if((psZgpResponseCmdPayload->eZgpdCmdId == E_GP_CHANNEL_CONFIGURATION)&&
    				(sZgpDataReq.uGpId.u32SrcId == 0)&&
    				(sZgpDataReq.u8ApplicationId == GP_APPL_ID_4_BYTE))
	 {
		 sZgpDataReq.u8TxOptions |= ( ((uint8)3) << 3); // set maintenance bit
	 }

    if(bAddMsg)
    {
#if 0
    	// hAPduInst = hZCL_AllocateAPduInstance();
		/* check buffer size */
		if((hAPduInst == PDUM_INVALID_HANDLE)||
		   (PDUM_u16APduGetSize(hZCL_GetBufferPoolHandle()) < psZgpResponseCmdPayload->sZgpdCommandPayload.u8Length) )
		{
			if(hAPduInst != PDUM_INVALID_HANDLE)
			{

				// free buffer and return
				PDUM_eAPduFreeAPduInstance(hAPduInst);
			}
			DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_HandleZgpReponse E_ZCL_ERR_ZBUFFER_FAIL \r\n");

			return;
		}
		pu8Start = (uint8 *)(PDUM_pvAPduInstanceGetPayload(hAPduInst));

		for(i = 0; i<psZgpResponseCmdPayload->sZgpdCommandPayload.u8Length;i++)
		{
			*pu8Start++ = psZgpResponseCmdPayload->sZgpdCommandPayload.pu8Data[i];
		}
		PDUM_eAPduInstanceSetPayloadSize(hAPduInst, psZgpResponseCmdPayload->sZgpdCommandPayload.u8Length);
		eZPSStatus = zps_vDStub_DataReq(&sZgpDataReq, hAPduInst,TRUE,(uint8)psZgpResponseCmdPayload->eZgpdCmdId);
#endif
	//	DBG_vPrintf(TRACE_GP_DEBUG, "\n  zps_vDStub_DataReq:%d\r\n", eZPSStatus);
		DBG_vPrintf(TRACE_GP_DEBUG, "\n Buffering  E_GP_ADD_MSG_TO_TX_QUEUE_AFTER_DELAY \r\n");
		vBackUpTxQueueMsg(E_GP_ADD_MSG_TO_TX_QUEUE_AFTER_DELAY,
								  psZgpResponseCmdPayload,
								  &sZgpDataReq,
								  psGpCustomDataStructure);
    }
    else
    {
    	ZPS_tsAfZgpTxGpQueueEntry sAfZgpTxGpQueueEntry;
    	sAfZgpTxGpQueueEntry.hNPdu = NULL;
    	sAfZgpTxGpQueueEntry.bValid = TRUE;
    	memcpy(&sAfZgpTxGpQueueEntry.sReq, &sZgpDataReq,sizeof(sAfZgpTxGpQueueEntry.sReq));
    	if(ZPS_bZgpRemoveTxQueue(&sAfZgpTxGpQueueEntry))
		{
		    if (psGpCustomDataStructure->u16TransmitChannelTimeout)
        	{
        		psGpCustomDataStructure->u16TransmitChannelTimeout = 0x00;
				ZPS_vNwkNibSetChannel(ZPS_pvAplZdoGetNwkHandle(),
						psGpCustomDataStructure->u8OperationalChannel);
        	}
		}
    }
}
/****************************************************************************
 **
 ** NAME:       bBestGPD_GPPLink
 **
 ** DESCRIPTION:
 ** Checks the fields of Pairing command
 **
 ** PARAMETERS:               Name                      Usage
 ** uint8                   u8RecevdLinkVal
 **  uint8                  u8BufferedLinkVal
 **
 ** RETURN:
 ** TRUE if valid, FALSE otherwise
 **
 ****************************************************************************/

uint8 bBestGPD_GPPLink(uint8 u8RecevdLinkVal, uint8 u8BufferedLinkVal)
{
	if((u8RecevdLinkVal & LQI_MASK) > (u8BufferedLinkVal &  LQI_MASK) )
	{
		return LINK_MORE;
	}
	else	if((u8RecevdLinkVal & LQI_MASK) < (u8BufferedLinkVal &  LQI_MASK) )
	{
		return LINK_LESS;
	}
	if((u8RecevdLinkVal & RSSI_MASK) < (u8BufferedLinkVal &  RSSI_MASK) )
	{
		return LINK_MORE;
	}
	else
	{
		if((u8RecevdLinkVal & RSSI_MASK) > (u8BufferedLinkVal &  RSSI_MASK) )
		{
			return LINK_LESS;
		}
		else
		{
			return LINK_EQUAL;
		}
	}
}
/****************************************************************************
 **
 ** NAME:       bGP_IsPairingCmdValid
 **
 ** DESCRIPTION:
 ** Checks the fields of Pairing command
 **
 ** PARAMETERS:               Name                      Usage
 ** tsGP_ZgpPairingCmdPayload                   *psZgpPairingCmdPayload
 **
 ** RETURN:
 ** TRUE if valid, FALSE otherwise
 **
 ****************************************************************************/
bool_t bGP_IsPairingCmdValid( tsGP_ZgpPairingCmdPayload      *psZgpPairingCmdPayload)
{
	/* security level 1 deprecated*/
	if(((psZgpPairingCmdPayload->b24Options & GP_PAIRING_SEC_LEVEL_MASK) >> 9) == 1)
	{
		return FALSE;
	}
	if(((psZgpPairingCmdPayload->b24Options & GP_APPLICATION_ID_MASK) ==GP_APPL_ID_4_BYTE ) &&
			(psZgpPairingCmdPayload->uZgpdDeviceAddr.u32ZgpdSrcId == 0))
	{
		return FALSE;
	}
	if(((psZgpPairingCmdPayload->b24Options & GP_APPLICATION_ID_MASK) ==GP_APPL_ID_8_BYTE ) &&
			(psZgpPairingCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr == 0))
	{
		return FALSE;
	}
	return TRUE;
}
/****************************************************************************
 **
 ** NAME:       bValidityCheckPairingConfig
 **
 ** DESCRIPTION:
 **  Parameter validation of received pairing config command
 **
 ** PARAMETERS:               		  Name                      Usage
 **tsGP_ZgpPairingConfigCmdPayload    *psZgpPairingConfigPayload
 **sCLD_GreenPower                     *pAttribData
 **
 ** RETURN:
 **
 **TRUE if command valid, FALSE otherwise
 ****************************************************************************/
#ifdef GP_COMBO_BASIC_DEVICE
bool_t bValidityCheckPairingConfig( ZPS_tsAfEvent                                     *pZPSevent,
								  tsGP_ZgpPairingConfigCmdPayload                   *psZgpPairingConfigPayload,
		                          tsCLD_GreenPower                                  *pAttribData)
{
    teGP_GreenPowerPairingConfigAction          ePairingConfigAction;
    teGP_GreenPowerCommunicationMode            eCommunicationMode;
    teGP_GreenPowerSecLevel                     eSecurityLevel;

    ePairingConfigAction = psZgpPairingConfigPayload->u8Actions & GP_PAIRING_CONFIG_ACTION_MASK;

	if(((psZgpPairingConfigPayload->b16Options & GP_APPLICATION_ID_MASK) != GP_APPL_ID_4_BYTE)&&((psZgpPairingConfigPayload->b16Options & GP_APPLICATION_ID_MASK) != GP_APPL_ID_8_BYTE))
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "\n GP_APPLICATION_ID_MASK not supported  %d \n", psZgpPairingConfigPayload->b16Options & GP_APPLICATION_ID_MASK);
		return FALSE;
	}
	/* address zero check  */
	if(((psZgpPairingConfigPayload->b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_4_BYTE))
	{
		if((psZgpPairingConfigPayload->uZgpdDeviceAddr.u32ZgpdSrcId == 0x00))
		{
			DBG_vPrintf(TRACE_GP_DEBUG, "\n psZgpPairingConfigPayload->uZgpdDeviceAddr.u32ZgpdSrcId == %d \n", psZgpPairingConfigPayload->uZgpdDeviceAddr.u32ZgpdSrcId );
			return FALSE;
		}
	}
	else
	{
		if(((psZgpPairingConfigPayload->b16Options & GP_APPLICATION_ID_MASK) == GP_APPL_ID_8_BYTE))
		{
			if(psZgpPairingConfigPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr == 0x00)
			{
				DBG_vPrintf(TRACE_GP_DEBUG, "\n psZgpPairingConfigPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr == %d \n",
						psZgpPairingConfigPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr);
				return FALSE;
			}
		}
	}

	eCommunicationMode = (psZgpPairingConfigPayload->b16Options & GP_PAIRING_CONFIG_COMM_MODE_MASK)  >> 3;
	if(psZgpPairingConfigPayload->b16Options & GP_PAIRING_CONFIG_SECURITY_USE_MASK)
	{
		eSecurityLevel = psZgpPairingConfigPayload->b8SecOptions & GP_SECURITY_LEVEL_MASK;
		if(eSecurityLevel == 1) //invalid security level
		{
			DBG_vPrintf(TRACE_GP_DEBUG, "\neSecurityLevel == 1\n");
			return FALSE;
		}
	}
	else
	{
		eSecurityLevel = 0;
	}

	 if((ePairingConfigAction == E_GP_PAIRING_CONFIG_EXTEND_SINK_TABLE_ENTRY)||
	  (ePairingConfigAction == E_GP_PAIRING_CONFIG_REPLACE_SINK_TABLE_ENTRY))
	{
		/* check if GPS supports the Security level requested */
		if(psZgpPairingConfigPayload->b16Options & GP_PAIRING_CONFIG_SECURITY_USE_MASK)
		{
			if(eSecurityLevel < (pAttribData->b8ZgpsSecLevel & GP_ATTRIB_SEC_BITS))
			{

				DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_HandleZgpPairingConfig  sec eSecurityLevel  = %d b8ZgpsSecLevel=%d  \n",
						eSecurityLevel,
						(pAttribData->b8ZgpsSecLevel & GP_ATTRIB_SEC_BITS));
				eZCL_SendDefaultResponse(pZPSevent, E_ZCL_FAIL);

				return FALSE;
			}
		}
		/* check if GPS supports the communication mode requested */
		if(((eCommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID) &&
		  (!(pAttribData->b24ZgpsFunctionality & GP_FEATURE_ZGPS_COMMISSION_GC_COMM)))||
		  ((eCommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID) &&
		  (!(pAttribData->b24ZgpsFunctionality & GP_FEATURE_ZGPS_DERIVED_GC_COMM))) ||
		  ((eCommunicationMode == E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_LIGHTWEIGHT) &&
				  (!(pAttribData->b24ZgpsFunctionality & GP_FEATURE_ZGPS_LIGHTWEIGHT_UNICAST_COMM))))
		{
			DBG_vPrintf(TRACE_GP_DEBUG, "\n eCommunicationMode  check failed %d, 0x%4x \n", eCommunicationMode, pAttribData->b24ZgpsFunctionality );
			eZCL_SendDefaultResponse(pZPSevent, E_ZCL_FAIL);
			return FALSE;

	   }
	}
	return TRUE;
}
/****************************************************************************
 **
 ** NAME:       vFillAndSendPairingCmdFromSinkTable
 **
 ** DESCRIPTION:
 **  Fills pairingCmdfrom sink tabledetails
 **   **
 ** PARAMETERS:               		  Name                      Usage
 ** tsGP_ZgpPairingCmdPayload        *psZgpPairingPayload,
 ** tsGP_ZgppProxySinkTable                      *psSinkTableEntry,
 **
 ** RETURN:
 **
 **None
 ****************************************************************************/
void vFillAndSendPairingCmdFromSinkTable(uint8                                u8EndPoint,
								   uint8                                      u8AddSinkTableMask,
								   uint8                                      u8GroupIdIndex,
								  tsGP_ZgppProxySinkTable                      *psSinkTableEntry)
{
	uint8 u8TransactionSequenceNumber, u8Status;
	tsGP_ZgpPairingCmdPayload                   sZgpPairingPayload = { 0 };
	tsZCL_Address                               sDestinationAddress;
	sDestinationAddress.eAddressMode = E_ZCL_AM_BROADCAST;
	sDestinationAddress.uAddress.eBroadcastMode = ZPS_E_APL_AF_BROADCAST_RX_ON;
	 sZgpPairingPayload.b24Options = psSinkTableEntry->b16Options & GP_APPLICATION_ID_MASK;
	 sZgpPairingPayload.b24Options |= u8AddSinkTableMask;
	 sZgpPairingPayload.uZgpdDeviceAddr = psSinkTableEntry->uZgpdDeviceAddr;
	 if(!(u8AddSinkTableMask & GP_PAIRING_REMOVE_GPD_MASK))
	 {
		 sZgpPairingPayload.b24Options |= (psSinkTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK )<< 5;
		 if((teGP_GreenPowerCommunicationMode)(psSinkTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)
		 {
			 sZgpPairingPayload.u16SinkGroupID = u16GP_DeriveAliasSrcAddr((psSinkTableEntry->b16Options & GP_APPLICATION_ID_MASK), psSinkTableEntry->uZgpdDeviceAddr);
		 }
		 else if((teGP_GreenPowerCommunicationMode)(psSinkTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
		 {
			 sZgpPairingPayload.u16SinkGroupID = psSinkTableEntry->asSinkGroupList[u8GroupIdIndex].u16SinkGroup;
			 sZgpPairingPayload.u16AssignedAlias =psSinkTableEntry->asSinkGroupList[u8GroupIdIndex].u16Alias;
			 if((psSinkTableEntry->asSinkGroupList[u8GroupIdIndex].u16Alias != 0xFFFF)&&(!(sZgpPairingPayload.b24Options& GP_PAIRING_REMOVE_GPD_MASK)))
			 {
				 sZgpPairingPayload.b24Options |=GP_PAIRING_ASSIGNED_ALIAS_PRESENT_MASK;
			 }
		 }

		 sZgpPairingPayload.u16SinkNwkAddress = ZPS_u16AplZdoGetNwkAddr();
		 sZgpPairingPayload.u64SinkIEEEAddress = ZPS_u64NwkNibGetExtAddr(ZPS_pvAplZdoGetNwkHandle());
		 if(u8AddSinkTableMask & GP_PAIRING_ADD_SINK_MASK)
		 {
			 if(psSinkTableEntry->b16Options & GP_PROXY_TABLE_GPD_FIXED_MASK)
			 {
				 sZgpPairingPayload.b24Options |= GP_PAIRING_GPD_FIXED_MASK;
			 }
			 sZgpPairingPayload.b24Options |= GP_PAIRING_FORWARDING_RADIUS_PRESENT_MASK;

			 sZgpPairingPayload.u8ForwardingRadius =  psSinkTableEntry->u8GroupCastRadius;


			 if(psSinkTableEntry->b16Options & GP_PROXY_TABLE_SEQ_NUM_CAP_MASK)
			 {
				 sZgpPairingPayload.b24Options |= GP_PAIRING_SEQ_NUM_CAP_MASK;
			 }

			 if(psSinkTableEntry->b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK)
			 {

				 sZgpPairingPayload.b24Options |= (psSinkTableEntry->b8SecOptions & GP_SECURITY_LEVEL_MASK) << 9;
				 sZgpPairingPayload.b24Options |= ( (psSinkTableEntry->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2) << 11;
				 /* Set ZGPDKey present flag */
				 sZgpPairingPayload.b24Options |=  GP_PAIRING_SECURITY_KEY_PRESENT_MASK;

				 /* Copy Key from Sink Table */
				 memcpy(&sZgpPairingPayload.sZgpdKey, &psSinkTableEntry->sZgpdKey, E_ZCL_KEY_128_SIZE);
			 }



			 sZgpPairingPayload.u8DeviceId = psSinkTableEntry->eZgpdDeviceId;

		 /* Set security frame counter flag */
		 sZgpPairingPayload.b24Options |= GP_PAIRING_SECURITY_COUNTER_PRESENT_MASK;
		 sZgpPairingPayload.u32ZgpdSecFrameCounter = psSinkTableEntry->u32ZgpdSecFrameCounter;
		 }
	 }

	 DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_ZgpPairingSend GP_PROXY_TABLE_SECURITY_USE_MASK = %d GP_SECURITY_LEVEL_MASK %d .GP_SECURITY_KEY_TYPE_MASK = %d\n",
			 (psSinkTableEntry->b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK),
			 ((psSinkTableEntry->b8SecOptions & GP_SECURITY_LEVEL_MASK) << 9),
			 (((psSinkTableEntry->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2) << 11 ));

	 u8Status = eGP_ZgpPairingSend(
    		 u8EndPoint,
              ZCL_GP_PROXY_ENDPOINT_ID,
              &sDestinationAddress,
              &u8TransactionSequenceNumber,
              &sZgpPairingPayload);
	 DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_ZgpPairingSend u8Status = %d %d %d \n",u8Status, psSinkTableEntry->u8SinkGroupListEntries ,
			 ((sZgpPairingPayload.b24Options & GP_PAIRING_COMM_MODE_MASK) >> 5));
}

/****************************************************************************
 **
 ** NAME:       vRemoveGroupAndSonkTableOnReplace
 **
 ** DESCRIPTION:
 **  Sends pairing command and removes groupfrom group table
 **
 ** PARAMETERS:               		  Name                      Usage
 ** tsZCL_EndPointDefinition                   *psEndPointDefinition,
 ** tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
 ** tsGP_ZgpPairingConfigCmdPayload            *psZgpPairingConfigPayload,
 ** tsGP_ZgppProxySinkTable                     *psSinkTableEntry
 **
 ** RETURN:
 **
 **None
 ****************************************************************************/
void vRemoveGroupAndSinkTableOnReplace(
		tsZCL_EndPointDefinition                   *psEndPointDefinition,
		tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
		tsGP_ZgpPairingConfigCmdPayload            *psZgpPairingConfigPayload,
		tsGP_ZgppProxySinkTable                     *psSinkTableEntry)
{
	  uint8                                       i;


	/* Remove GPEP from group at APS level */
	if((teGP_GreenPowerCommunicationMode)((psZgpPairingConfigPayload->b16Options & GP_PAIRING_CONFIG_COMM_MODE_MASK) >> 3) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID )
	{
		vGP_RemoveGPEPFromGroup(psGpCustomDataStructure, psEndPointDefinition, psSinkTableEntry);
	}
	if((psSinkTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK )== E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)
	{
		/* Send pairing remove with the derived group id */
		if((psZgpPairingConfigPayload->u8Actions & GP_PAIRING_CONFIG_SEND_GP_PAIRING_MASK))
		{
			 vFillAndSendPairingCmdFromSinkTable(psEndPointDefinition->u8EndPointNumber,GP_PAIRING_REMOVE_GPD_MASK, 0,psSinkTableEntry);
		}
	}
	else if((teGP_GreenPowerCommunicationMode)(psSinkTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
	{
		for(i = 0; i < psZgpPairingConfigPayload->u8SinkGroupListEntries; i++)
		{
			if((psZgpPairingConfigPayload->u8Actions & GP_PAIRING_CONFIG_SEND_GP_PAIRING_MASK))
			{
				 vFillAndSendPairingCmdFromSinkTable(psEndPointDefinition->u8EndPointNumber,GP_PAIRING_REMOVE_GPD_MASK, i,psSinkTableEntry);
			}
		}
	}
	/* remove sink table entry */
	vGP_RemoveGPDFromProxySinkTable( psEndPointDefinition->u8EndPointNumber,
			psZgpPairingConfigPayload->b16Options & GP_APPLICATION_ID_MASK,
			&psZgpPairingConfigPayload->uZgpdDeviceAddr);
	DBG_vPrintf(TRACE_GP_DEBUG, "\n Removed sink since no group entries  present 3\n");
	vGP_CallbackForPersistData();



}
/****************************************************************************
 **
 ** NAME:       vSendPairingRemoveGroup
 **
 ** DESCRIPTION:
 **  Sends pairing command and removes groupfrom group table
 **
 ** PARAMETERS:               		  Name                      Usage
 ** tsZCL_EndPointDefinition                   *psEndPointDefinition,
 ** tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
 ** tsGP_ZgpPairingConfigCmdPayload            *psZgpPairingConfigPayload,
 ** tsGP_ZgppProxySinkTable                     *psSinkTableEntry
 **
 ** RETURN:
 **
 **None
 ****************************************************************************/
void vSendPairingRemoveGroup(
		tsZCL_EndPointDefinition                   *psEndPointDefinition,
		tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
		tsGP_ZgpPairingConfigCmdPayload            *psZgpPairingConfigPayload,
		tsGP_ZgppProxySinkTable                     *psSinkTableEntry)
{
	  uint8                                     i,j;


	/* Remove GPEP from group at APS level */
	if((teGP_GreenPowerCommunicationMode)(psSinkTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID)
	{
		vGP_RemoveGPEPFromGroup(psGpCustomDataStructure, psEndPointDefinition, psSinkTableEntry);
		if((psZgpPairingConfigPayload->u8Actions & GP_PAIRING_CONFIG_SEND_GP_PAIRING_MASK))
		{
			 vFillAndSendPairingCmdFromSinkTable(psEndPointDefinition->u8EndPointNumber,0, 0,psSinkTableEntry);
		}
		vGiveIndicationForTranslationTableUpdate(
				E_GP_PAIRING_CONFIG_TRANSLATION_TABLE_REMOVE_ENTRY,
				psEndPointDefinition,
				psGpCustomDataStructure,
				psZgpPairingConfigPayload);
		/* remove sink table entry */
		psSinkTableEntry->u8GPDPaired = E_GP_NOT_PAIRED;
		psSinkTableEntry->eGreenPowerSinkTablePriority = 0;
		//memset(psSinkTableEntry,0,sizeof(tsGP_ZgppProxySinkTable));
		vGP_RemoveGPDFromProxySinkTable( psEndPointDefinition->u8EndPointNumber,
    									psZgpPairingConfigPayload->b16Options & GP_APPLICATION_ID_MASK,
    									&psZgpPairingConfigPayload->uZgpdDeviceAddr);
		vGP_CallbackForPersistData();
	}
	else if((teGP_GreenPowerCommunicationMode)(psSinkTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK) == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
	{
		/* remove sink table entry */
		for(i=0;i < psZgpPairingConfigPayload->u8SinkGroupListEntries; i++)
		{
			for(j=0;j<psSinkTableEntry->u8SinkGroupListEntries;j++)
			{
				if(psZgpPairingConfigPayload->asSinkGroupList[i].u16SinkGroup == psSinkTableEntry->asSinkGroupList[j].u16SinkGroup)
				{
					uint8 k;
					if((psZgpPairingConfigPayload->u8Actions & GP_PAIRING_CONFIG_SEND_GP_PAIRING_MASK))
					{
						 vFillAndSendPairingCmdFromSinkTable(psEndPointDefinition->u8EndPointNumber,0, j,psSinkTableEntry);
					}
					for(k = j; k < (psSinkTableEntry->u8SinkGroupListEntries - 1); k++)
					{

						psSinkTableEntry->asSinkGroupList[k].u16SinkGroup = psSinkTableEntry->asSinkGroupList[k+1].u16SinkGroup;
						psSinkTableEntry->asSinkGroupList[k].u16Alias = psSinkTableEntry->asSinkGroupList[k+1].u16Alias;

					}

					psSinkTableEntry->u8SinkGroupListEntries--;
					DBG_vPrintf(TRACE_GP_DEBUG, "\n Removed group id from sink = 0x%4x  %d  \n",psSinkTableEntry->asSinkGroupList[i].u16SinkGroup, psSinkTableEntry->u8SinkGroupListEntries);
					break;
				}
			}
			if(psSinkTableEntry->u8SinkGroupListEntries == 0)
			{
				/* remove sink table entry */
				vGiveIndicationForTranslationTableUpdate(
						E_GP_PAIRING_CONFIG_TRANSLATION_TABLE_REMOVE_ENTRY,
						psEndPointDefinition,
						psGpCustomDataStructure,
						psZgpPairingConfigPayload);
				psSinkTableEntry->u8GPDPaired = E_GP_NOT_PAIRED;
				psSinkTableEntry->eGreenPowerSinkTablePriority = 0;
				//memset(psSinkTableEntry,0,sizeof(tsGP_ZgppProxySinkTable));
				vGP_RemoveGPDFromProxySinkTable( psEndPointDefinition->u8EndPointNumber,
												psZgpPairingConfigPayload->b16Options & GP_APPLICATION_ID_MASK,
				    									&psZgpPairingConfigPayload->uZgpdDeviceAddr);
				DBG_vPrintf(TRACE_GP_DEBUG, "\n Removed sink since no group entries  present \n");
				vGP_CallbackForPersistData();
				break;
			}
		}
	}

}
/****************************************************************************
 **
 ** NAME:       vGiveIndicationForTranslationTableUpdate
 **
 ** DESCRIPTION:
 **  Indication to application to update translation table on receiving pairing config command
 **
 ** PARAMETERS:               		  Name                      Usage
 ** teGP_PairingConfigTranslationTableAction                         eTransTableAction,
 ** tsZCL_EndPointDefinition                                        *psEndPointDefinition,
 ** tsGP_GreenPowerCustomData                                       *psGpCustomDataStructure,
 ** tsGP_ZgpPairingConfigCmdPayload                                 *psZgpPairingConfigPayload
 **
 ** RETURN:
 **
 ** None
 ****************************************************************************/
void vGiveIndicationForTranslationTableUpdate(
		teGP_PairingConfigTranslationTableAction                         eTransTableAction,
		tsZCL_EndPointDefinition                                        *psEndPointDefinition,
		tsGP_GreenPowerCustomData                                       *psGpCustomDataStructure,
		tsGP_ZgpPairingConfigCmdPayload                                 *psZgpPairingConfigPayload)
{
    tsGP_ZgpsPairingConfigCmdRcvd   			sZgpsPairingConfigCmdRcvd;
	psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_PAIRING_CONFIGURATION_CMD_RCVD;
	sZgpsPairingConfigCmdRcvd.eAction = psZgpPairingConfigPayload->u8Actions & GP_PAIRING_CONFIG_ACTION_MASK;
	sZgpsPairingConfigCmdRcvd.eTranslationTableAction = eTransTableAction;

	sZgpsPairingConfigCmdRcvd.eZgpdDeviceId = psZgpPairingConfigPayload->eZgpdDeviceId;
	sZgpsPairingConfigCmdRcvd.u8ApplicationId = psZgpPairingConfigPayload->b16Options & GP_APPLICATION_ID_MASK;
	if(sZgpsPairingConfigCmdRcvd.u8ApplicationId == GP_APPL_ID_4_BYTE)
	{
		sZgpsPairingConfigCmdRcvd.uZgpdDeviceAddr.u32ZgpdSrcId = psZgpPairingConfigPayload->uZgpdDeviceAddr.u32ZgpdSrcId;
	}
#ifdef GP_IEEE_ADDR_SUPPORT
	else
	{
		sZgpsPairingConfigCmdRcvd.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr =
				psZgpPairingConfigPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
		sZgpsPairingConfigCmdRcvd.uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint =
				psZgpPairingConfigPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
	}
#endif
	sZgpsPairingConfigCmdRcvd.u8NumberOfPairedEndpoints = psZgpPairingConfigPayload->u8NumberOfPairedEndpoints;

	if((psZgpPairingConfigPayload->u8NumberOfPairedEndpoints != 0xFD)&&
	  (psZgpPairingConfigPayload->u8NumberOfPairedEndpoints != 0xFE)&&
	  (psZgpPairingConfigPayload->u8NumberOfPairedEndpoints != 0xFF))
	{
		memcpy(sZgpsPairingConfigCmdRcvd.au8PairedEndpointList, psZgpPairingConfigPayload->au8PairedEndpoints, psZgpPairingConfigPayload->u8NumberOfPairedEndpoints);
	}
	sZgpsPairingConfigCmdRcvd.eCommunicationMode =  (teGP_GreenPowerCommunicationMode)((psZgpPairingConfigPayload->b16Options & GP_PAIRING_CONFIG_COMM_MODE_MASK) >> 3);
	if(sZgpsPairingConfigCmdRcvd.eCommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
	{
		sZgpsPairingConfigCmdRcvd.u8SinkGroupListEntries = psZgpPairingConfigPayload->u8SinkGroupListEntries;
		memcpy(sZgpsPairingConfigCmdRcvd.asSinkGroupList, psZgpPairingConfigPayload->asSinkGroupList, psZgpPairingConfigPayload->u8SinkGroupListEntries*sizeof(tsGP_ZgpsGroupList));
	}
	/* Give Application Callback  */
	psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psPairingConfigCmdRcvd = &sZgpsPairingConfigCmdRcvd;
	psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
}
/****************************************************************************
 **
 ** NAME:       u8AddGroupIdToSinkTable
 **
 ** DESCRIPTION:
 **  Add the given group to sink table if applicable
 **
 ** PARAMETERS:               		  Name                      Usage
 ** uint16                                                           u16Alias,
 ** tsGP_ZgppProxySinkTable                                         *psSinkTableEntry
 **
 ** RETURN:
 **
 ** Index in sink table where group added
 ****************************************************************************/
uint8  u8AddGroupIdToSinkTable(uint16                                                        u16GroupId,
							uint16                                                           u16Alias,
							tsGP_ZgppProxySinkTable                                         *psSinkTableEntry)
{
	uint8 j;
	uint8 u8Index = 0xFF;
	bool_t bIsGroupIdExists = FALSE;
	bIsGroupIdExists = FALSE;
	teGP_GreenPowerCommunicationMode eCommMode = psSinkTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK;

	DBG_vPrintf(TRACE_GP_DEBUG, "\n psZgpPairingConfigPayload->asSinkGroupList = 0x%4x  psSinkTableEntry->u8SinkGroupListEntries = %d\n",
			u16GroupId, psSinkTableEntry->u8SinkGroupListEntries);
	for(j=0; j< psSinkTableEntry->u8SinkGroupListEntries; j++)
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "\n psSinkTableEntry->asSinkGroupList[j].u16SinkGroup= 0x%4x psZgpPairingConfigPayload->asSinkGroupList[i].u16SinkGroup = 0x%4x  \n",
				psSinkTableEntry->asSinkGroupList[j].u16SinkGroup, u16GroupId);
		if(psSinkTableEntry->asSinkGroupList[j].u16SinkGroup == u16GroupId )
		{
			bIsGroupIdExists = TRUE;
			return j;
		}
	}
	if(bIsGroupIdExists == FALSE)
	{
		if(psSinkTableEntry->u8SinkGroupListEntries < GP_MAX_SINK_GROUP_LIST)
		{
			//if( eCommMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID )
			{
				ZPS_eAplZdoGroupEndpointAdd(u16GroupId, ZCL_GP_PROXY_ENDPOINT_ID);
			}
			/* If communication mode is E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID and we do not have the group id at
			 * APS level, return */
			if(eCommMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
			{
				u8Index =psSinkTableEntry->u8SinkGroupListEntries;
				psSinkTableEntry->u8SinkGroupListEntries++;
				psSinkTableEntry->asSinkGroupList[u8Index].u16SinkGroup =
						u16GroupId ;
				psSinkTableEntry->asSinkGroupList[u8Index].u16Alias =
						u16Alias ;

				DBG_vPrintf(TRACE_GP_DEBUG, "\n Group added at index j= %d u16SinkGroup = 0x%4x u8SinkGrouu8IndexpListEntries = %d  \n",
						j, u16GroupId, psSinkTableEntry->u8SinkGroupListEntries);
				return (u8Index);
			}
			else
			{
				DBG_vPrintf(TRACE_GP_DEBUG, "\n eCommMode= %d u16SinkGroup = 0x%4x u8SinkGrouu8IndexpListEntries = %d , bGP_CheckGroupTable(u16GroupId) = %d \n",
						eCommMode, u16GroupId, psSinkTableEntry->u8SinkGroupListEntries, bGP_CheckGroupTable(u16GroupId));
			}
		}

	}
	return u8Index;
}

/****************************************************************************
 **
 ** NAME:       vSendpairing
 **
 ** DESCRIPTION:
 ** Creates pairing command from sink table and send the command
 **
 ** PARAMETERS:               		  Name                      Usage
 ** uint8                                                 u8EndPointNumber ,
 **  bool_t                                                   bdeviceAnnce,
 ** tsGP_ZgpPairingConfigCmdPayload                       *psZgpPairingConfigPayload,
 ** tsGP_ZgppProxySinkTable                                *psSinkTableEntry
 **
 ** RETURN:
 **
 ** Index in sink table where group added
 ****************************************************************************/
void vSendpairing(uint8                                                 u8EndPointNumber ,
		          bool_t                                                   bdeviceAnnce,
		          tsGP_ZgpPairingConfigCmdPayload                       *psZgpPairingConfigPayload,
		          tsGP_ZgppProxySinkTable                                *psSinkTableEntry)
{
	uint8 u8ApplicationId;
	uint16 u16AliasShortAddr;
    teGP_GreenPowerCommunicationMode            eCommunicationMode;
    eCommunicationMode = (psZgpPairingConfigPayload->b16Options & GP_PAIRING_CONFIG_COMM_MODE_MASK)  >> 3;
    u8ApplicationId = psZgpPairingConfigPayload->b16Options & GP_APPLICATION_ID_MASK;
	if(eCommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
	{
		uint8 i;
		for(i = 0; i < psSinkTableEntry->u8SinkGroupListEntries; i++)
		{
			u16AliasShortAddr = psSinkTableEntry->asSinkGroupList[i].u16Alias;
			DBG_vPrintf(TRACE_GP_DEBUG, "\n vFillAndSendPairingCmdFromSinkTable    i= %d  = 0x%4x \n", i,psSinkTableEntry->asSinkGroupList[i].u16SinkGroup);
			/* send device announce */
			if(bdeviceAnnce)
			{
				vGP_SendDeviceAnnounce(u16AliasShortAddr,0xFFFFFFFFFFFFFFFFULL);
			}
			if(psZgpPairingConfigPayload->u8Actions & GP_PAIRING_CONFIG_SEND_GP_PAIRING_MASK)
			{

				vFillAndSendPairingCmdFromSinkTable(u8EndPointNumber ,
						GP_PAIRING_ADD_SINK_MASK,
						i,
						psSinkTableEntry);
			}
		}
	}
	else
	{
		if(psSinkTableEntry->b16Options & GP_PROXY_TABLE_ASSIGNED_ALIAS_MASK)
		{
			u16AliasShortAddr = psSinkTableEntry->u16ZgpdAssignedAlias;
		}
		else
		{
			u16AliasShortAddr = u16GP_DeriveAliasSrcAddr(u8ApplicationId, psZgpPairingConfigPayload->uZgpdDeviceAddr);
		}
		/* send device announce */
		DBG_vPrintf(TRACE_GP_DEBUG, "\n vGP_SendDeviceAnnounce    u16AliasShortAddr = 0x%4x \n", u16AliasShortAddr);
		if(bdeviceAnnce)
		{
			vGP_SendDeviceAnnounce(u16AliasShortAddr,0xFFFFFFFFFFFFFFFFULL);
		}
		if(psZgpPairingConfigPayload->u8Actions & GP_PAIRING_CONFIG_SEND_GP_PAIRING_MASK)
		{

			vFillAndSendPairingCmdFromSinkTable(u8EndPointNumber,
					GP_PAIRING_ADD_SINK_MASK,
					0,
					psSinkTableEntry);
		}

	}
}
/****************************************************************************
 **
 ** NAME:       bCheckIfAtleastOneGroupSupported
 **
 ** DESCRIPTION:
 ** Checks if atleast one group in the pairing config command is supported at APS group table
 **
 ** PARAMETERS:               		  Name                      Usage
 ** tsGP_ZgpPairingConfigCmdPayload                       *psZgpPairingConfigPayload
 **
 ** RETURN:
 **
 ** TRUE if group supported at APS, FALSE otherwise
 ****************************************************************************/
bool_t bCheckIfAtleastOneGroupSupported(tsGP_ZgpPairingConfigCmdPayload                       *psZgpPairingConfigPayload)
{
	uint8 i;
	for(i=0 ; i < psZgpPairingConfigPayload->u8SinkGroupListEntries ; i++)
	{
		if(bGP_CheckGroupTable(psZgpPairingConfigPayload->asSinkGroupList[i].u16SinkGroup))
		{
			return TRUE;
		}
	}
	return FALSE;
}
/****************************************************************************
 **
 ** NAME:       bIsNotificationValid
 **
 ** DESCRIPTION:
 ** Checks if notification is valid
 **
 ** PARAMETERS:               		  Name                      Usage
 ** tsGP_ZgpNotificationCmdPayload *psNotificationCmdPyld,
 ** tsZCL_EndPointDefinition       *psEndPointDefinition,
 ** tsZCL_ClusterInstance          *psClusterInstance,
 ** ZPS_tsAfEvent                  *pZPSevent
 **
 ** RETURN:
 **
 ** TRUE if valid, FALSE otherwise
 ****************************************************************************/
bool_t bIsNotificationValid(tsGP_ZgpNotificationCmdPayload *psNotificationCmdPyld,
		tsZCL_EndPointDefinition       *psEndPointDefinition,
		 tsZCL_ClusterInstance          *psClusterInstance,
		ZPS_tsAfEvent                  *pZPSevent)
{
    tsGP_GreenPowerCustomData *psGPCustomDataStructure;

	tsGP_ZgppProxySinkTable                *psSinkTableEntry;
	bool_t bIsSinkTablePresent, IsCommModeMatch = FALSE;
    teGP_GreenPowerCommunicationMode            eCommunicationMode =0;

    // initialise pointer
    psGPCustomDataStructure = (tsGP_GreenPowerCustomData *)psClusterInstance->pvEndPointCustomStructPtr;
    if((psNotificationCmdPyld->eZgpdCmdId == E_GP_COMMISSIONING) || (psNotificationCmdPyld->eZgpdCmdId >= E_GP_SUCCESS ))
    {
		DBG_vPrintf(TRACE_GP_DEBUG, "Rejecting commissioning commands in operational mode \n");
		return FALSE;
    }
	bIsSinkTablePresent = bGP_IsSinkTableEntryPresent(
			 psEndPointDefinition->u8EndPointNumber,
			 (psNotificationCmdPyld->b16Options & GP_APPLICATION_ID_MASK),
			 &psNotificationCmdPyld->uZgpdDeviceAddr,
             &psSinkTableEntry,
             0xFF);



	 if((bIsSinkTablePresent == TRUE) /*&&( (pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode == ZPS_E_ADDR_MODE_SHORT) ||
			 (pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode == ZPS_E_ADDR_MODE_IEEE))*/)
	 {
		 eCommunicationMode = psSinkTableEntry->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK;
		if(((eCommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID) && (psNotificationCmdPyld->b16Options & GP_NOTIFICATION_ALSO_DERIVED_GROUP_MASK))||
		   ((eCommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)&&
				   (psNotificationCmdPyld->b16Options & GP_NOTIFICATION_ALSO_COMMISSION_GROUP_MASK)) ||
				   ((eCommunicationMode == E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_LIGHTWEIGHT)&&
				   				   (psNotificationCmdPyld->b16Options & GP_NOTIFICATION_ALSO_UNICAST_MASK)))
		{
			IsCommModeMatch = TRUE;

			 if(bGP_IsDuplicatePkt(GP_ZGP_DUPLICATE_TIMEOUT,
									  psEndPointDefinition->u8EndPointNumber,
									   psClusterInstance->bIsServer,
									   (psNotificationCmdPyld->b16Options & GP_APPLICATION_ID_MASK),
									   psNotificationCmdPyld->uZgpdDeviceAddr,
									   ((psNotificationCmdPyld->b16Options & GP_NOTIFICATION_SECURITY_LEVEL_MASK) >> 6),
									   psNotificationCmdPyld->u32ZgpdSecFrameCounter,
									   psNotificationCmdPyld->eZgpdCmdId,
									   psGPCustomDataStructure))
			 {

				DBG_vPrintf(TRACE_GP_DEBUG, "bGP_IsDuplicatePkt TRUE \n");
				return FALSE;
			 }
		}
		if(IsCommModeMatch == FALSE)
		{
			 DBG_vPrintf(TRACE_GP_DEBUG, "\n IsCommModeMatch == FALSEE;\n");
			/* send pairing with correct mode */
			vFillAndSendPairingCmdFromSinkTable(psEndPointDefinition->u8EndPointNumber,GP_PAIRING_ADD_SINK_MASK, 0,psSinkTableEntry);
			return FALSE;
		}
	 }

	if((bIsSinkTablePresent == FALSE) || (IsCommModeMatch == FALSE))
	{
		DBG_vPrintf(TRACE_GP_DEBUG, "\n (bIsSinkTablePresent == FALSE) || (IsCommModeMatch == FALSE)\n");

		 if(( (pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode == ZPS_E_ADDR_MODE_SHORT) ||
					 (pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode == ZPS_E_ADDR_MODE_IEEE)))
		 {
			/* send pairing with correct received mode and bAddSink to FALSE*/
			uint8 u8TransactionSequenceNumber;
			tsGP_ZgpPairingCmdPayload                   sZgpPairingPayload = { 0 };
			tsZCL_Address                               sDestinationAddress;
			sDestinationAddress.eAddressMode = E_ZCL_AM_SHORT;
			sDestinationAddress.uAddress.u16DestinationAddress = pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr;
			 sZgpPairingPayload.b24Options = psNotificationCmdPyld->b16Options & GP_APPLICATION_ID_MASK;
			 if(psNotificationCmdPyld->b16Options & GP_NOTIFICATION_ALSO_UNICAST_MASK )
			 {
				 eCommunicationMode = E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_LIGHTWEIGHT;
				 sZgpPairingPayload.u16SinkNwkAddress = ZPS_u16AplZdoGetNwkAddr();
				 sZgpPairingPayload.u64SinkIEEEAddress = ZPS_u64NwkNibGetExtAddr(ZPS_pvAplZdoGetNwkHandle());
			 }
			 else if(psNotificationCmdPyld->b16Options & GP_NOTIFICATION_ALSO_DERIVED_GROUP_MASK )
			 {
				 eCommunicationMode = E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID;
			 }
			 else if (psNotificationCmdPyld->b16Options & GP_NOTIFICATION_ALSO_COMMISSION_GROUP_MASK )
			 {
				 eCommunicationMode = E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID;
			 }

			 sZgpPairingPayload.b24Options |= (eCommunicationMode << 5);


			 sZgpPairingPayload.uZgpdDeviceAddr = psNotificationCmdPyld->uZgpdDeviceAddr;



			  eGP_ZgpPairingSend(
					 psEndPointDefinition->u8EndPointNumber,
		              ZCL_GP_PROXY_ENDPOINT_ID,
		              &sDestinationAddress,
		              &u8TransactionSequenceNumber,
		              &sZgpPairingPayload);
			 return FALSE;
		 }

	}
	if((bIsSinkTablePresent == TRUE) && ((pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode == ZPS_E_ADDR_MODE_GROUP)) &&
			(eCommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID))
	{
		uint8 i;
		bool_t bGroupIDPresentInSinkTable = FALSE;
		/* Check the group address is part of sink table as there can e mismatch between Proxy and sink */
		for(i =0 ; i < psSinkTableEntry->u8SinkGroupListEntries; i++)
		{
			if(psSinkTableEntry->asSinkGroupList[i].u16SinkGroup ==pZPSevent->uEvent.sApsDataIndEvent.uDstAddress.u16Addr )
			{
				bGroupIDPresentInSinkTable = TRUE;
				break;
			}
		}
		if(bGroupIDPresentInSinkTable == FALSE)
		{
			return FALSE;
		}

	}
	return TRUE;
}
/****************************************************************************
 **
 ** NAME:       bDecryptGPDFData
 **
 ** DESCRIPTION:
 ** Checks if notification is valid
 **
 ** PARAMETERS:               		  Name                      Usage
 ** tsGP_ZgpNotificationCmdPayload *psNotificationCmdPyld,
 ** tsZCL_EndPointDefinition       *psEndPointDefinition,
 ** tsZCL_ClusterInstance          *psClusterInstance,
 ** ZPS_tsAfEvent                  *pZPSevent
 **
 ** RETURN:
 **
 ** TRUE if valid, FALSE otherwise
 ****************************************************************************/
bool_t bDecryptGPDFData(
		tsGP_ZgpCommissioningNotificationCmdPayload           *psZgpCommissioningNotificationCmdPayload,
		tsZCL_EndPointDefinition                              *psEndPointDefinition,
	    tsGP_GreenPowerCustomData                             *psGpCustomDataStructure)
{
	 bool_t                       bValid = FALSE;
	 uint8                        u8Alen;
	 AESSW_Block_u                uNonce;
	 uint8                        au8Header[24];
	 uint8                        au8Data[GP_MAX_ZB_CMD_PAYLOAD_LENGTH];
	 uint8                        u8NonceOffset = 0, u8Offset = 1;
	 uint8                        u8Status;
	 uint8                        u8SecLevel, u8SecKeyType;
	 tsGP_ZgppProxySinkTable      *psZgpsSinkTable;
	 bool						  bIsSinkTableEntryPresent;
	 AESSW_Block_u                uSecurityKey;


	 u8NonceOffset = 1;

	u8SecKeyType = (psZgpCommissioningNotificationCmdPayload->b16Options & GP_CMSNG_NOTIFICATION_SECURITY_KEY_TYPE_MASK) >> 6;

	u8SecLevel = ((psZgpCommissioningNotificationCmdPayload->b16Options & GP_CMSNG_NOTIFICATION_SECURITY_LEVEL_MASK) >> 4);
	memset(au8Header,0,sizeof(au8Header));
	memset(uNonce.au8,0,sizeof(uNonce));
	memset(au8Data,0,sizeof(au8Data));
	bIsSinkTableEntryPresent =  bGP_IsSinkTableEntryPresent(
		 psEndPointDefinition->u8EndPointNumber,
		 psZgpCommissioningNotificationCmdPayload->b16Options& GP_APPLICATION_ID_MASK,
		 &psZgpCommissioningNotificationCmdPayload->uZgpdDeviceAddr,
		 &psZgpsSinkTable,
		 0xff);
	if((u8SecKeyType == E_GP_NO_KEY) && (bIsSinkTableEntryPresent))
	{
		u8SecKeyType = (psZgpsSinkTable->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK) >> 2;
	}
	bGP_GetGPDKey(
		psZgpCommissioningNotificationCmdPayload->b16Options& GP_APPLICATION_ID_MASK,
		bIsSinkTableEntryPresent,
		&psZgpCommissioningNotificationCmdPayload->uZgpdDeviceAddr,
		psZgpsSinkTable,
		u8SecKeyType,
		&uSecurityKey,
		psEndPointDefinition,
	   psGpCustomDataStructure);


	/* Create nonce */
	if((psZgpCommissioningNotificationCmdPayload->b16Options & GP_APPLICATION_ID_MASK) == 0)
	{
		vWrite32Nbo(psZgpCommissioningNotificationCmdPayload->uZgpdDeviceAddr.u32ZgpdSrcId,&uNonce.au8[0+u8NonceOffset]);
		vWrite32Nbo(psZgpCommissioningNotificationCmdPayload->uZgpdDeviceAddr.u32ZgpdSrcId,&uNonce.au8[4+u8NonceOffset]);
		uNonce.au8[12+u8NonceOffset] = 0x5; /* Security level according to Zigbee spec.*/
	}
	else
	{
		vWrite64Nbo(psZgpCommissioningNotificationCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr,&uNonce.au8[0+u8NonceOffset]);
		uNonce.au8[12+u8NonceOffset] = 0x5; /* Security level according to Zigbee spec.*/
	}

	/* Encryption and authentication */
	/* First establish payload and header */

	if (1 == u8SecLevel)
	{
		return bValid;
	}
	else
	{
		uint8 GPDFkeyType = 0;
		uint8 u8ExtFC = psZgpCommissioningNotificationCmdPayload->b16Options & GP_APPLICATION_ID_MASK;
		u8ExtFC |= u8SecLevel << 3;

		if(u8SecKeyType >= E_GP_OUT_OF_THE_BOX_ZGPD_KEY)
		{
			GPDFkeyType = 1;
		}
		u8ExtFC |= GPDFkeyType << 5;
		u8ExtFC |= (psZgpCommissioningNotificationCmdPayload->b16Options & GP_CMSNG_NOTIFICATION_RX_AFTER_TX_MASK) << 6;
	//	u8ExtFC |= 1 << 7;
		u8Offset = 1;

		au8Header[0] = 0x8c;
		if (au8Header[0]  & 0x80)
		{
			au8Header[1] = u8ExtFC;
			u8Offset++;
		}
		if((psZgpCommissioningNotificationCmdPayload->b16Options & GP_APPLICATION_ID_MASK) == 0)
		{
			vWrite32Nbo(psZgpCommissioningNotificationCmdPayload->uZgpdDeviceAddr.u32ZgpdSrcId, &au8Header[u8Offset]);
			vWrite32Nbo(psZgpCommissioningNotificationCmdPayload->u32ZgpdSecFrameCounter, &au8Header[u8Offset + 4]);
			u8Alen = u8Offset + 8;
		}
		else
		{
			au8Header[u8Offset] = psZgpCommissioningNotificationCmdPayload->uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
			u8Offset++;
			vWrite32Nbo(psZgpCommissioningNotificationCmdPayload->u32ZgpdSecFrameCounter, &au8Header[u8Offset]);
			u8Alen = u8Offset + 4;
		}
	}

	vWrite32Nbo(psZgpCommissioningNotificationCmdPayload->u32ZgpdSecFrameCounter, &uNonce.au8[8+u8NonceOffset]);
	/* Create M data from payload */
	au8Data[0] = psZgpCommissioningNotificationCmdPayload->u8ZgpdCmdId ;
	memcpy(&au8Data[1], psZgpCommissioningNotificationCmdPayload->sZgpdCommandPayload.pu8Data, psZgpCommissioningNotificationCmdPayload->sZgpdCommandPayload.u8Length);

	/* Transform */

	if (3 == u8SecLevel)
	{
		uint8 j;
		uint8 u8Mlen = psZgpCommissioningNotificationCmdPayload->sZgpdCommandPayload.u8Length +1;
		uint8 au8Mic[4];


		vWrite32Nbo(psZgpCommissioningNotificationCmdPayload->u32Mic, au8Mic);

		DBG_vPrintf(TRACE_GP_DEBUG,"u8ZgpCCMStarDecrypt sec level %d Mlen %x u32mic %x\n ",u8SecLevel,u8Mlen,psZgpCommissioningNotificationCmdPayload->u32Mic  );
		DBG_vPrintf(TRACE_GP_DEBUG,"\n Printing key : "  );
		for(j=0; j < 16 ; j++)
		{
			DBG_vPrintf(TRACE_GP_DEBUG," %x  " ,uSecurityKey.au8[j] );
		}
		DBG_vPrintf(TRACE_GP_DEBUG,"\n Printing uNonce : "  );
		for(j=0; j < sizeof(uNonce.au8); j++)
		{
			DBG_vPrintf(TRACE_GP_DEBUG," %x  " ,uNonce.au8[j] );
		}
		DBG_vPrintf(TRACE_GP_DEBUG,"\n Printing header %d : ",u8Alen  );
		for(j=0; j < u8Alen; j++)
		{
			DBG_vPrintf(TRACE_GP_DEBUG," %x  " ,au8Header[j] );
		}
		DBG_vPrintf(TRACE_GP_DEBUG,"\n Printing data  psZgpCommissioningNotificationCmdPayload->u8ZgpdCmdId  %d: ", u8Mlen, psZgpCommissioningNotificationCmdPayload->u8ZgpdCmdId   );
		for(j=0; j < u8Mlen; j++)
		{
			DBG_vPrintf(TRACE_GP_DEBUG," %x  " ,au8Data[j] );
		}
		DBG_vPrintf(TRACE_GP_DEBUG,"\n Printing au8Mic: " );
		for(j=0; j < 4; j++)
		{
			DBG_vPrintf(TRACE_GP_DEBUG," %x  " ,au8Mic[j] );
		}

#if (defined LITTLE_ENDIAN_PROCESSOR) && (defined JENNIC_CHIP_FAMILY_JN517x)
		tsReg128 sKey;
		memcpy(&sKey, &uSecurityKey.au8[0], 16);
		vSwipeEndian(&uSecurityKey, &sKey, FALSE);
#endif

		u8Status = bACI_WriteKey((tsReg128*)&uSecurityKey);
		vACI_OptimisedCcmStar(
			FALSE,
			4,
			u8Alen,
			u8Mlen,
			&uNonce,
			&au8Header[0],
			&au8Data[0],                // overwrite the i/p data
			au8Mic,    // append to the o/p data
			&bValid);
		DBG_vPrintf(TRACE_GP_DEBUG,"\n Printing decrypted data  %d bValid =%d: ", u8Mlen ,bValid );
		for(j=0; j < u8Mlen; j++)
		{
			DBG_vPrintf(TRACE_GP_DEBUG," %x  " ,au8Data[j] );
		}
		if(bValid)
		{
			psZgpCommissioningNotificationCmdPayload->u8ZgpdCmdId =au8Data[0];
			if(u8Mlen > 1 )
			{
				/* Copy payload and MIC into payload buffer */
				memcpy(psZgpCommissioningNotificationCmdPayload->sZgpdCommandPayload.pu8Data, &au8Data[1], (u8Mlen -1));
			}
		}
		else
		{
			DBG_vPrintf(TRACE_GP_DEBUG,"vACI_OptimisedCcmStar API failed %x\n ", u8Status);
		}
	}
	else
	{
		uint8 au8Mic[4],i,au8CmpMic[4] ;
		u8Alen = u8Alen +  psZgpCommissioningNotificationCmdPayload->sZgpdCommandPayload.u8Length;
		vWrite32Nbo(psZgpCommissioningNotificationCmdPayload->u32Mic, au8CmpMic);
		u8Status = bACI_WriteKey((tsReg128*)&uSecurityKey);
		vACI_OptimisedCcmStar(
				TRUE,
			4,
			u8Alen,
			0,
			&uNonce,
			&au8Data[0],
			NULL,                // overwrite the i/p data
			au8Mic,    // append to the o/p data
			&bValid);
		 if(u8Status)
		 {
			 for(i =0; i<2; i++)
			 {
				 if(au8Mic[i] != au8CmpMic[i])
				 {
					 return FALSE;
				 }
			 }

			bValid = TRUE;
		 }
	}

	return bValid;
}
#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
