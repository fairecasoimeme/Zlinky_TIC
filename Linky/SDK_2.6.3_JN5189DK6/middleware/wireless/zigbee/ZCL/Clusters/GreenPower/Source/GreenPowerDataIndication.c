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
 * COMPONENT:          GreenPowerDataIndication.c
 *
 * DESCRIPTION:        The handling functions for the Green Power direct data commands
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "dlist.h"
#include "zcl.h"
#include "GreenPower.h"
#include "GreenPower_internal.h"
#include "zcl_common.h"
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
PRIVATE bool_t bGP_DuplicateFilterForDirectGpdf(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint8                                  u8GpEndpoint,
                    bool_t                                 bIsServer,
                    tsGP_GreenPowerCustomData              *psGpCustomDataStructure);
void vAddRemoveBufferedTxQMsg(ZPS_tsAfEvent *pZPSevent,
		 tsGP_GreenPowerCustomData   *psGpCustomDataStructure);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/


/****************************************************************************
 **
 ** NAME:       vZCL_HandleZgpDataIndication
 **
 ** DESCRIPTION:
 ** Valididates the ZGP(Green Power) Data Indication Event
 **
 ** PARAMETERS:               Name                    Usage
 ** ZPS_tsAfEvent           *pZPSevent               Zigbee stack event structure
 ** uint8                   u8GpEndpoint             Green Power Local End Point
 ** RETURN:
 ** None
 **
 ****************************************************************************/
PUBLIC void vZCL_HandleZgpDataIndication(ZPS_tsAfEvent *pZPSevent, uint8 u8GpEndpoint)
{
    tsZCL_CallBackEvent         sZCL_CallBackEvent;
    tsZCL_EndPointDefinition    *psEndPointDefinition;
    tsZCL_ClusterInstance       *psClusterInstance;
    tsGP_GreenPowerCustomData   *psGpCustomDataStructure;
    bool_t                      bIsServer = TRUE;

    // fill in callback event
    sZCL_CallBackEvent.u8TransactionSequenceNumber = 0;
    sZCL_CallBackEvent.psClusterInstance = NULL;
    sZCL_CallBackEvent.pZPSevent = pZPSevent;
    sZCL_CallBackEvent.eZCL_Status = E_ZCL_SUCCESS;

#ifndef GP_IEEE_ADDR_SUPPORT
    if(pZPSevent->uEvent.sApsZgpDataIndEvent.u2ApplicationId == 2)
    {
        sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_INVALID_VALUE;
    }
#endif
    /* application id should not other than 0 and 2 OR if application id 2 then
     * MAC source address mode should be 3*/
    if(((pZPSevent->uEvent.sApsZgpDataIndEvent.u2ApplicationId != 0) &&
         (pZPSevent->uEvent.sApsZgpDataIndEvent.u2ApplicationId != 2)) ||
        ((pZPSevent->uEvent.sApsZgpDataIndEvent.u2ApplicationId == 2) &&
        (pZPSevent->uEvent.sApsZgpDataIndEvent.u8SrcAddrMode != 3))||
        (sZCL_CallBackEvent.eZCL_Status != E_ZCL_SUCCESS))
    {
        sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_MALFORMED_MESSAGE;
    }
    /* Default cluster instance is based on device type */
    /* This will be set in next release code */
    // find GP Cluster
    else if(eGP_FindGpCluster(u8GpEndpoint,
            bIsServer,
            &psEndPointDefinition,
            &psClusterInstance,
            &psGpCustomDataStructure) != E_ZCL_SUCCESS)
    {
        bIsServer = FALSE;

        if(eGP_FindGpCluster(u8GpEndpoint,
                        FALSE,
                        &psEndPointDefinition,
                        &psClusterInstance,
                        &psGpCustomDataStructure) != E_ZCL_SUCCESS)
        {
            sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_CLUSTER_NOT_FOUND;
        }
    }

    /* check for duplicate pkt, if it is a duplicate then just drop it */
  if(sZCL_CallBackEvent.eZCL_Status == E_ZCL_SUCCESS)
    {
        /* Check Duplicate only for when device is in operation mode */
        if(bGP_DuplicateFilterForDirectGpdf(pZPSevent, u8GpEndpoint, bIsServer, psGpCustomDataStructure))
        {
            PDUM_eAPduFreeAPduInstance(pZPSevent->uEvent.sApsZgpDataIndEvent.hAPduInst);
            DBG_vPrintf(TRACE_GP_DEBUG, "bGP_IsDuplicatePkt  TRUE, buffer freed\n");
            return;
        }

        eGP_GreenPowerDirectCommandHandler( pZPSevent,
                                            psEndPointDefinition,
                                            psClusterInstance);
    }


    if(E_ZCL_SUCCESS != sZCL_CallBackEvent.eZCL_Status)
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, "E_ZCL_CBET_ZGP_DATA_IND_ERROR\n");
        sZCL_CallBackEvent.eEventType = E_ZCL_CBET_ZGP_DATA_IND_ERROR;
        psZCL_Common->pfZCLinternalCallBackFunction(&sZCL_CallBackEvent);
    }

    PDUM_eAPduFreeAPduInstance(pZPSevent->uEvent.sApsZgpDataIndEvent.hAPduInst);
}

/****************************************************************************
 **
 ** NAME:       vZCL_HandleZgpDataConfirm
 **
 ** DESCRIPTION:
 ** Valididates the ZGP(Green Power) Data Indication Event
 **
 ** PARAMETERS:               Name                    Usage
 ** ZPS_tsAfEvent           *pZPSevent               Zigbee stack event structure
 ** uint8                   u8GpEndpoint             Green Power Local End Point
 ** RETURN:
 ** None
 **
 ****************************************************************************/
PUBLIC void vZCL_HandleZgpDataConfirm(ZPS_tsAfEvent *pZPSevent, uint8 u8GpEndpoint)
{
    tsZCL_CallBackEvent         sZCL_CallBackEvent;
    tsZCL_EndPointDefinition    *psEndPointDefinition;
    tsZCL_ClusterInstance       *psClusterInstance;
    tsGP_GreenPowerCustomData   *psGpCustomDataStructure;
    bool_t                      bIsServer = TRUE;
    DBG_vPrintf(TRACE_GP_DEBUG, "vZCL_HandleZgpDataConfirm received \r\n");
    // fill in callback event
    sZCL_CallBackEvent.u8TransactionSequenceNumber = 0;
    sZCL_CallBackEvent.psClusterInstance = NULL;
    sZCL_CallBackEvent.pZPSevent = pZPSevent;
    sZCL_CallBackEvent.eZCL_Status = E_ZCL_SUCCESS;

    /* Default cluster instance is based on device type */
    /* This will be set in next release code */
    // find GP Cluster
    if(eGP_FindGpCluster(u8GpEndpoint,
            bIsServer,
            &psEndPointDefinition,
            &psClusterInstance,
            &psGpCustomDataStructure) != E_ZCL_SUCCESS)
    {
        bIsServer = FALSE;

        if(eGP_FindGpCluster(u8GpEndpoint,
                        FALSE,
                        &psEndPointDefinition,
                        &psClusterInstance,
                        &psGpCustomDataStructure) != E_ZCL_SUCCESS)
        {
            sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_CLUSTER_NOT_FOUND;
            return;
        }
    }

    /* check for duplicate pkt, if it is a duplicate then just drop it */
 //   if(sZCL_CallBackEvent.eZCL_Status == E_ZCL_SUCCESS)
    {
    	 DBG_vPrintf(TRACE_GP_DEBUG, "sZCL_CallBackEvent.eZCL_Status == E_ZCL_SUCCESS\r\n");
        uint8 u8DataReqHandle;

        u8DataReqHandle = psGpCustomDataStructure->u8GPDataReqHandle;

        if(u8DataReqHandle == ZPS_NWK_GP_BASE_HANDLE + 1)
        {
            u8DataReqHandle = ZPS_NWK_GP_MAX_HANDLE - 1;
        }
        else
        {
            u8DataReqHandle--;
        }

        if(pZPSevent->uEvent.sApsZgpDataConfirmEvent.u8Handle == u8DataReqHandle)
        {
            if(psGpCustomDataStructure->u16TransmitChannelTimeout)
            {
            	 DBG_vPrintf(TRACE_GP_DEBUG, "vZCL_HandleZgpDataConfirm psGpCustomDataStructure->u16TransmitChannelTimeout reset \r\n");
              //  psGpCustomDataStructure->u16TransmitChannelTimeout = 0;
#ifndef PC_PLATFORM_BUILD
              //  ZPS_vNwkNibSetChannel( ZPS_pvAplZdoGetNwkHandle(), psGpCustomDataStructure->u8OperationalChannel);
                DBG_vPrintf(TRACE_GP_DEBUG, "\n vZCL_HandleZgpDataConfirm: setting channel to back to u8OperationalChannel = %d  \n",
                		 psGpCustomDataStructure->u8OperationalChannel);
#endif
            }
        }
        vAddRemoveBufferedTxQMsg(pZPSevent, psGpCustomDataStructure);
    }


    if(E_ZCL_SUCCESS != sZCL_CallBackEvent.eZCL_Status)
    {
        sZCL_CallBackEvent.eEventType = E_ZCL_CBET_ZGP_DATA_IND_ERROR;
        psZCL_Common->pfZCLinternalCallBackFunction(&sZCL_CallBackEvent);
    }
}
/****************************************************************************
 **
 ** NAME:       vAddRemoveBufferedTxQMsg
 **
 ** DESCRIPTION:
 ** Duplicate filter
 **
 ** PARAMETERS:                    Name                           Usage
 ** ZPS_tsAfEvent *pZPSevent
 **
 ** RETURN:
 ** bool_t
 ****************************************************************************/
void vAddRemoveBufferedTxQMsg(ZPS_tsAfEvent *pZPSevent,
		 tsGP_GreenPowerCustomData   *psGpCustomDataStructure)
{
	bool_t IsResponseBackedUp= FALSE;
	/* set delay to 0       	      	 */
	tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord;
	bool_t                                        bIsHeadNode = FALSE;

	/* Get Head Pointer of Alloc List */
	psZgpBufferedApduRecord = (tsGP_ZgpBufferedApduRecord *)psDLISTgetHead(&psGpCustomDataStructure->lGpAllocList);
	DBG_vPrintf(TRACE_GP_DEBUG, "vAddRemoveBufferedTxQMsg \r\n");
	/* Check Pointer */
	while(psZgpBufferedApduRecord)
	{
		if(psZgpBufferedApduRecord->sBufferedApduInfo.eGreenPowerBufferedCommand == E_GP_ADD_MSG_TO_TX_QUEUE)
		{
			if(pZPSevent->uEvent.sApsZgpDataConfirmEvent.u8Status == MAC_ENUM_SUCCESS)
			{
				DBG_vPrintf(TRACE_GP_DEBUG, "Set Delay to 0pZPSevent->uEvent.sApsZgpDataConfirmEvent.u8Status = %d \r\n", pZPSevent->uEvent.sApsZgpDataConfirmEvent.u8Status);
				psZgpBufferedApduRecord->sBufferedApduInfo.u16Delay = 1;
				psZgpBufferedApduRecord->sBufferedApduInfo.u8Status = E_ZCL_SUCCESS;
	        	/*if (psGpCustomDataStructure->u16TransmitChannelTimeout)
	        	{
	        		psGpCustomDataStructure->u16TransmitChannelTimeout = 0x00;
					ZPS_vNwkNibSetChannel(ZPS_pvAplZdoGetNwkHandle(),
							psGpCustomDataStructure->u8OperationalChannel);
	        	}*/
				IsResponseBackedUp = TRUE;
			}
			else
			{
				tsGP_ZgpBufferedApduRecord *psTempZgpBufferedApduRecord =
														(tsGP_ZgpBufferedApduRecord *)psDLISTgetHead(&psGpCustomDataStructure->lGpAllocList);
				DBG_vPrintf(TRACE_GP_DEBUG, "Remove Msg pZPSevent->uEvent.sApsZgpDataConfirmEvent.u8Status = %d \r\n", pZPSevent->uEvent.sApsZgpDataConfirmEvent.u8Status);
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
	if(IsResponseBackedUp ==  FALSE)
	{
		if (psGpCustomDataStructure->u16TransmitChannelTimeout)
		{
			psGpCustomDataStructure->u16TransmitChannelTimeout = 0x00;
			ZPS_vNwkNibSetChannel(ZPS_pvAplZdoGetNwkHandle(),
					psGpCustomDataStructure->u8OperationalChannel);
		}
	}
}

/****************************************************************************
 **
 ** NAME:       bGP_DuplicateFilterForDirectGpdf
 **
 ** DESCRIPTION:
 ** Duplicate filter
 **
 ** PARAMETERS:                    Name                           Usage
 ** ZPS_tsAfEvent               *pZPSevent                ZPS event
 ** uint8                       u8SourceEndPoint          Source endpoint
 ** bool_t                      bIsServer                 Is cluster server or client
 **
 ** RETURN:
 ** bool_t
 ****************************************************************************/
PRIVATE bool_t bGP_DuplicateFilterForDirectGpdf(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint8                                  u8SourceEndPoint,
                    bool_t                                 bIsServer,
                    tsGP_GreenPowerCustomData              *psGpCustomDataStructure)
{

    tuGP_ZgpdDeviceAddr uZgpdDeviceAddr;
    uint32 u32SeqNoOrCounter;

    /* Check Application Id */
    /* Copy Green Power Device Address */
    if(pZPSevent->uEvent.sApsZgpDataIndEvent.u2ApplicationId == GP_APPL_ID_4_BYTE)
    {
        uZgpdDeviceAddr.u32ZgpdSrcId = pZPSevent->uEvent.sApsZgpDataIndEvent.uGpAddress.u32SrcId;
    }
#ifdef GP_IEEE_ADDR_SUPPORT
    else
    {
        uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr = pZPSevent->uEvent.sApsZgpDataIndEvent.uGpAddress.u64Addr;
        uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint = pZPSevent->uEvent.sApsZgpDataIndEvent.u8Endpoint;
    }
#endif
    /* check security level */
    if(pZPSevent->uEvent.sApsZgpDataIndEvent.u2SecurityLevel == 0)
    {
       u32SeqNoOrCounter = (uint32)pZPSevent->uEvent.sApsZgpDataIndEvent.u8SeqNum;
    }
    else
    {
        u32SeqNoOrCounter = pZPSevent->uEvent.sApsZgpDataIndEvent.u32SecFrameCounter;
    }


    return(bGP_IsDuplicatePkt(
    		GP_ZGP_DUPLICATE_TIMEOUT,
                  u8SourceEndPoint,
                  bIsServer,
                  (uint8)pZPSevent->uEvent.sApsZgpDataIndEvent.u2ApplicationId,
                  uZgpdDeviceAddr,
                  pZPSevent->uEvent.sApsZgpDataIndEvent.u2SecurityLevel ,
                  u32SeqNoOrCounter,
                  (uint8)pZPSevent->uEvent.sApsZgpDataIndEvent.u8CommandId,
                  psGpCustomDataStructure));
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
