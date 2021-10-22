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


/*###############################################################################
#
# MODULE:      BDB
#
# COMPONENT:   bdb_fb_initiator.c
#
# DESCRIPTION: BDB Find & Bind Initiator Implementation
#              
#
###############################################################################*/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <string.h>
#include <jendefs.h>
#include "pdum_gen.h"
#include "zps_gen.h"
#include "zps_apl.h"
#include "zps_apl_aib.h"
#include "dbg.h"
#include "bdb_api.h"
#include "bdb_fb_api.h"
#include "Groups.h"
#include "Identify.h"
#include "appZdpExtraction.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef DEBUG_FB_INTIATOR
#define TRACE_FB_INTIATOR FALSE
#else
#define TRACE_FB_INTIATOR TRUE
#endif

#define E_CLD_IDENTIFY_CMD_IDENTIFY_QUERY_RESPONSE  E_CLD_IDENTIFY_CMD_IDENTIFY
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE BDB_teStatus eFbStartFbInitiator(uint8 u8SourceEndPointId);
PRIVATE BDB_teStatus eFbSendIdentifyQuery(uint8 u8SourceEndPointId);
PRIVATE void vFbSendIeeeAddrReq( ZPS_tuAddress     uSrcAddress, 
                                      uint16            u16DstAddr);
PRIVATE void vFbSendSimpleDescReq(uint8           u8DstEndpoint, 
                                        uint16          u16DstAddr); 
PRIVATE void vFbHandleSimpleDescResp(ZPS_tsAfZdpEvent      *psAfZdpEvent);
PRIVATE void vFbBindCluster(ZPS_tsAfZdpEvent        *psAfZdpEvent,
                                uint64                  *pu64DstIeeeAddr,
                                bool_t                  bAddAddressMapEntry,
                                bool_t                  bServer,
                                bool_t*                     bGroupAdded);
PRIVATE bool_t bFbIsOpertationalCluster(uint16         u16ClusterID,
                                             bool_t         bServer); 
PRIVATE void vFbCreateSourceBinding(
                                uint16              u16ClusterID,
                                ZPS_tsAfZdpEvent    *psAfZdpEvent,
                                uint64              *pu64DstIeeeAddr,
                                bool_t              bAddAddressMapEntry,
                                uint8               u8AddressMode);  
PRIVATE void vFbSendAddGroupCommand(uint8      u8SourceEndpoint, 
                                       uint8        u8DstEndpoint,
                                       uint16       u16ShortAddress,
                                       bool_t       bGroupCast);
                                        
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
PUBLIC uint8 u8TimerBdbFb;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsFindAndBind sFindAndBind = {0};
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: BDB_eFbTriggerAsInitiator
 *
 * DESCRIPTION:
 *  Find and Bind process for an initiator endpoint.
 *  Identifying target endpoints are searched first. Then simple descriptor is
 *  requested. If cluster mapping is a match then an entry is created in
 *  binding table.
 *
 *  Find and Bind process will continue for BDBC_MIN_COMMISSIONING_TIME. If find
 *  and bind couldn't succeed during this time, BDB_EVENT_FB_TIMEOUT event is
 *  generated using APP_vBdbCallback.
 *
 *  Identify query broadcast is repeated at every BDB_FB_RESEND_IDENTIFY_QUERY_TIME.
 *  This macro can be defined in bdb_options.h file. Before each broadcast attempt
 *  application callback event is generated BDB_EVENT_FB_NO_QUERY_RESPONSE. So if
 *  application chooses to quit the find and bind process as initiator, it can do so
 *  by calling BDB_vFbExitAsInitiator API.
 *
 *  For BDB to process Identify Query response, application must call
 *  BDB_vZclEventHandler with tsBDB_ZCLEvent eType as BDB_E_ZCL_EVENT_IDENTIFY_QUERY
 *  and pointer to tsZCL_CallBackEvent.
 *
 *  For each respondents of IdentifyQueryResponse, simple descriptor request is sent
 *  for the next endpoint. Upon reception of Simple Descriptor response, cluster
 *  match is performed and if found local(source) binding is created. At this point,
 *  application is notified using BDB_EVENT_FB_HANDLE_SIMPLE_DESC_RESP_OF_TARGET event
 *  using APP_vBdbCallback.
 *
 *  If user has intimated to create group binding using u16bdbCommissioningGroupID set
 *  to groupId other than 0xFFFF, then group binding is created otherwise IEEE source
 *  binding is created.
 *
 *  BDB_EVENT_FB_CHECK_BEFORE_BINDING_CLUSTER_FOR_TARGET
 *
 *  For IEEE source binding creation successful, BDB_EVENT_FB_BIND_CREATED_FOR_TARGET
 *  event is generated via APP_vBdbCallback. At this point application can send stop
 *  identification command to the Find and Bind target nodes.
 *
 *  During source binding creation, either BDB_EVENT_FB_GROUP_ADDED_TO_TARGET or
 *  BDB_EVENT_FB_ERR_GROUPING_FAILED events are generated via APP_vBdbCallback if
 *  group binding was selected.
 *
 * PARAMETERS:  Name                            Usage
 *              u8SourceEndpoint                the source endpoint
 *              u8State                         Grouping state or Find and bind state
 * RETURNS:
 *  BDB_E_FAILURE,      if Src EndPoint provided in argument is invalid.
 *                      if couldn't broadcast identify query command.
 *  BDB_E_ERROR_COMMISSIONING_IN_PROGRESS, if F&B process is already ongoing.
 *  BDB_E_SUCCESS, if F&B process has started as initiator
 *
 * EVENTS:
 *  BDB_EVENT_FB_TIMEOUT
 *  Others in description above.
 ****************************************************************************/
PUBLIC BDB_teStatus BDB_eFbTriggerAsInitiator(uint8 u8SourceEndPointId)
{
    DBG_vPrintf(TRACE_FB_INTIATOR, "BDB_eFbTriggerAsInitiator state = %d \n",sFindAndBind.eFBState);

    if( u8SourceEndPointId > BDB_FB_NUMBER_OF_ENDPOINTS )
    {
        DBG_vPrintf(TRACE_FB_INTIATOR, "BDB_eFbTriggerAsInitiator: E_ZCL_ERR_EP_RANGE \n");
        return BDB_E_FAILURE;
    }

    /* initiate find and bind in idle(initial) state only */
    if(E_FB_IDLE_STATE == sFindAndBind.eFBState)
    {
        return eFbStartFbInitiator(u8SourceEndPointId);
    }
    else
    {
        return BDB_E_ERROR_COMMISSIONING_IN_PROGRESS;
    }
    
    return BDB_E_FAILURE;
}

/****************************************************************************
 **
 ** NAME:       BDB_vFbHandleQueryResponse
 **
 ** DESCRIPTION:
 ** This function is used to to handle identify query response 
 **
 ** PARAMETERS:                 Name               Usage
 ** tsZCL_CallBackEvent         pCallBackEvent      ZCL call back structure
 **
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/
PUBLIC void BDB_vFbHandleQueryResponse(tsZCL_CallBackEvent *pCallBackEvent)
{
    uint8 i;
    uint64 u64IeeeAddr  = 0;

    /*Step 3: Identify Query Response received? */
    if(E_FB_IDLE_STATE != sFindAndBind.eFBState)
    {
        /*Save the discovery result */
        for(i=0;i < BDB_FB_MAX_TARGET_DEVICES; i++)
        {
            /* Find next free entry */
            if(sFindAndBind.asTargetInfo[i].u8DstEndpoint == 0)
            {
                sFindAndBind.asTargetInfo[i].u8DstEndpoint =
                                                pCallBackEvent->pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint;

                sFindAndBind.asTargetInfo[i].u16NwkAddr =
                                                pCallBackEvent->pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr;
                break;
            }
        }
        /* check if IEEE address of device is available */
        u64IeeeAddr  = ZPS_u64AplZdoLookupIeeeAddr(pCallBackEvent->pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr);
        
        DBG_vPrintf(TRACE_FB_INTIATOR, "BDB_vFbHandleQueryResponse event %0x\r\n", pCallBackEvent->eEventType);
        if(!u64IeeeAddr )
        {
            vFbSendIeeeAddrReq(pCallBackEvent->pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress,
                                            pCallBackEvent->pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr);
            sFindAndBind.eFBState = E_FB_WAIT_FOR_IEEE_RESPONSE_STATE;
        }
        else
        {
            /*Step 4: Request simple descriptor for the next endpoint */
            /* IEEE address of target known, issue simple descriptor request */
            vFbSendSimpleDescReq(pCallBackEvent->pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                            pCallBackEvent->pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr);
            sFindAndBind.eFBState = E_FB_WAIT_FOR_SIMPLE_DESCRIPTOR_RESPONSE_STATE;
        }

    }

}

/****************************************************************************
 *
 * NAME: vFbZdpHandler
 *
 * DESCRIPTION:
 * This function handles ZDP responses while Find And Bind
 *
 * PARAMETERS:  Name                            Usage
 *              pZPSevent                       Stack events
 *
 * RETURNS:
 * None
 *
 ****************************************************************************/
PUBLIC void vFbZdpHandler(ZPS_tsAfEvent *pZPSevent)
{
    int i;
    ZPS_tsAfZdpEvent sAfZdpEvent;

    if((ZPS_EVENT_APS_DATA_INDICATION == pZPSevent->eType) &&
            (0 == pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint))
    {
        for(i=0; i < BDB_FB_MAX_TARGET_DEVICES;i++)
        {

            if(ZPS_ZDP_IEEE_ADDR_RSP_CLUSTER_ID == pZPSevent->uEvent.sApsDataIndEvent.u16ClusterId)
            {
               zps_bAplZdpUnpackIeeeAddressResponse(pZPSevent, &sAfZdpEvent);
               DBG_vPrintf(TRACE_FB_INTIATOR, "ZDP_IEEE_ADDR_RSP_CLUSTER_ID \r\n");
                /* Back up iEEE address */
                if(sAfZdpEvent.uZdpData.sIeeeAddrRsp.u16NwkAddrRemoteDev == sFindAndBind.asTargetInfo[i].u16NwkAddr )
                {
                    sFindAndBind.asTargetInfo[i].u64IeeeAddr = sAfZdpEvent.uZdpData.sIeeeAddrRsp.u64IeeeAddrRemoteDev;

                    vFbSendSimpleDescReq(sFindAndBind.asTargetInfo[i].u8DstEndpoint,sFindAndBind.asTargetInfo[i].u16NwkAddr );
                    return;
                }
            }

            else if(ZPS_ZDP_SIMPLE_DESC_RSP_CLUSTER_ID == pZPSevent->uEvent.sApsDataIndEvent.u16ClusterId)
            {
                zps_bAplZdpUnpackSimpleDescResponse(pZPSevent, &sAfZdpEvent);
                DBG_vPrintf(TRACE_FB_INTIATOR, "ZDP_SIMPLE_DESC_RSP_CLUSTER_ID \r\n");
                if(sAfZdpEvent.uZdpData.sSimpleDescRsp.u16NwkAddrOfInterest == sFindAndBind.asTargetInfo[i].u16NwkAddr )
                {
                    /* Handle simple descriptor response: check for match and bind */
                    vFbHandleSimpleDescResp(&sAfZdpEvent);
                    /* As we have handled this target , free the entry */
                    memset(&sFindAndBind.asTargetInfo[i],0,sizeof(tsFB_TargetInfo));
                    return;
                }
            }
        }
    }
}

/****************************************************************************
 *
 * NAME: BDB_vFbExitAsInitiator
 *
 * DESCRIPTION:
 *  Stops the ongoing Find And Bind process as initiator.
 *
 * PARAMETERS:  Name                            Usage
 *
 * RETURNS:
 *  void
 *
 ****************************************************************************/
PUBLIC void BDB_vFbExitAsInitiator(void)
{
    DBG_vPrintf(TRACE_FB_INTIATOR, "BDB_vFbExitAsInitiator  \n");
    /* clear state variables */
    memset(&sFindAndBind,0,sizeof(tsFindAndBind));
    ZTIMER_eStop(u8TimerBdbFb);
    /*Step 10: Change BDB status into Success */
    sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_SUCCESS;      
}

/****************************************************************************
 *
 * NAME:       BDB_vFbTimerCb
 *
 * DESCRIPTION:
 *  Timer Callback to keep sending continuous identify request and to keep
 *  track of find and Bind time
 *
 * PARAMETERS:                 Name               Usage
 *
 * RETURN:
 * None
 *
 ****************************************************************************/
PUBLIC void BDB_vFbTimerCb(void *pvParam)
{
    bool_t bContinue = FALSE;
    BDB_tsFindAndBindEvent sEvent ={0};
    BDB_tsBdbEvent sBdbEvent = {0};
    
    sBdbEvent.uEventData.psFindAndBindEvent = &sEvent;
    
    if(E_FB_IDLE_STATE != sFindAndBind.eFBState)
    {
        if(sFindAndBind.u8CommTime > 0)
        {
            bContinue = TRUE;
            sFindAndBind.u8CommTime--;
        }
        else
        {
            BDB_vFbExitAsInitiator();
            sBdbEvent.eEventType = BDB_EVENT_FB_TIMEOUT;
            sEvent.u8InitiatorEp = sFindAndBind.u8Endpoint;
            APP_vBdbCallback(&sBdbEvent);
            return;
        }

        if(sFindAndBind.u8ReSendQueryTime > 0)
        {
            sFindAndBind.u8ReSendQueryTime--;
        }
        else
        {
            /* Repeat the discovery Again */
            if(sFindAndBind.eFBState == E_FB_WAIT_FOR_QUERY_RESPONSE_STATE)
            {
                /* Change BDB status into NO Identify Query */
                sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_NO_IDENTIFY_QUERY_RESPONSE;
                sBdbEvent.eEventType = BDB_EVENT_FB_NO_QUERY_RESPONSE;
                sEvent.u8InitiatorEp = sFindAndBind.u8Endpoint;
                APP_vBdbCallback(&sBdbEvent);
                /* As user has exited the procedure , return from this function */
                if(E_FB_IDLE_STATE == sFindAndBind.eFBState)
                    return;
            }
            eFbSendIdentifyQuery(sFindAndBind.u8Endpoint);
            sFindAndBind.u8ReSendQueryTime = BDB_FB_RESEND_IDENTIFY_QUERY_TIME;
        }
    }

    if(bContinue)
        ZTIMER_eStart(u8TimerBdbFb, ZTIMER_TIME_MSEC(1000));

}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME:       eFbStartFbInitiator
 *
 * DESCRIPTION:
 * Function to check if there is a client to server match with received simple
 * descriptor and to source bind if there is a match.
 *
 * PARAMETERS:             Name                Usage
 * uint8                   u8SourceEndPointId  EZ source EP Id
 *
 * RETURN:
 * None
 *
 ****************************************************************************/
PRIVATE BDB_teStatus eFbStartFbInitiator(uint8 u8SourceEndPointId)
{
    BDB_teStatus eStatus;

    /* clear state variables */
    memset(&sFindAndBind,0,sizeof(tsFindAndBind));

    /* Step 1 of 8.6: Change BDB status into progress */
    sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_IN_PROGRESS;  
    /* Step 2 of 8.6: Broadcast identify Query */
    eStatus = eFbSendIdentifyQuery(u8SourceEndPointId);
    
    if(BDB_E_FAILURE == eStatus )
    {
        DBG_vPrintf(TRACE_FB_INTIATOR, "eFbStartFbInitiator %d \n",eStatus);
        return eStatus;
    }

    /* EZ mode time backed up. Will be decremented on each ZCL tick */
    sFindAndBind.u8CommTime = BDBC_MIN_COMMISSIONING_TIME;
    sFindAndBind.u8ReSendQueryTime = BDB_FB_RESEND_IDENTIFY_QUERY_TIME;
    
    /* Kicks off Find and Bind Timer */
    ZTIMER_eStop(u8TimerBdbFb);
    ZTIMER_eStart(u8TimerBdbFb, ZTIMER_TIME_MSEC(500));
    
    return BDB_E_SUCCESS;
}

/****************************************************************************
 *
 * NAME:       eFbSendIdentifyQuery
 *
 * DESCRIPTION:
 * Starts a discovery
 *
 * PARAMETERS:             Name                Usage
 * uint8                   u8SourceEndPointId  Find and Bind Source EP Id
 *
 * RETURN:
 *  BDB_E_FAILURE  ,    if couldn't broadcast identify query command.
 *  BDB_E_SUCCESS
 ****************************************************************************/
PRIVATE BDB_teStatus eFbSendIdentifyQuery(uint8 u8SourceEndPointId)
{
#ifdef IDENTIFY_CLIENT
    uint8               u8TransactionSeqNo;
    tsZCL_Address       sAddress;
    teZCL_Status        eStatus;
    
    /* Broadcast Identify query request */
    sAddress.eAddressMode               = E_ZCL_AM_BROADCAST;
    sAddress.uAddress.eBroadcastMode    = ZPS_E_APL_AF_BROADCAST_ALL;

    eStatus =  eCLD_IdentifyCommandIdentifyQueryRequestSend(
                u8SourceEndPointId,
                0xFF, /* To All the endpoints */
                &sAddress,
                &u8TransactionSeqNo);
                
    DBG_vPrintf(TRACE_FB_INTIATOR, "eCLD_IdentifyCommandIdentifyQueryRequestSend %d \n",eStatus);                
    if(eStatus == E_ZCL_SUCCESS)
    {
        /* change the state to in - progress to wait for discovery */
        sFindAndBind.eFBState = E_FB_WAIT_FOR_QUERY_RESPONSE_STATE;
        sFindAndBind.u8Endpoint = u8SourceEndPointId;
        return BDB_E_SUCCESS;
    }
#endif
    return BDB_E_FAILURE;
}


/****************************************************************************
 **
 ** NAME:       vFbSendIeeeAddrReq
 **
 ** DESCRIPTION:
 ** This function is used to send a ZDP IEEE Address request
 **
 ** PARAMETERS:              Name               Usage
 ** ZPS_tuAddress           uSrcAddress         Source Address
 ** uint16                  u16DstAddr          Destination source address to which request to be sent
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/
PRIVATE void vFbSendIeeeAddrReq( ZPS_tuAddress         uSrcAddress, 
                                      uint16                u16DstAddr)
{
    uint8 u8TransactionSequenceNumber;
    ZPS_tsAplZdpIeeeAddrReq sZdpIeeeAddrReq;
    PDUM_thAPduInstance hAPduInst;

    /* IEEE address of target unknown, issue IEEE address request */
    sZdpIeeeAddrReq.u16NwkAddrOfInterest    = u16DstAddr;
    sZdpIeeeAddrReq.u8RequestType           = 0;
    sZdpIeeeAddrReq.u8StartIndex            = 0;

    hAPduInst = PDUM_hAPduAllocateAPduInstance(apduZDP);

    if (hAPduInst == PDUM_INVALID_HANDLE)
    {
        DBG_vPrintf(TRACE_FB_INTIATOR, "IEEE Address Request - PDUM_INVALID_HANDLE\n");
    }
    else
    {

        ZPS_teStatus eStatus = ZPS_eAplZdpIeeeAddrRequest(hAPduInst,
                                                      uSrcAddress,
                                                      FALSE,
                                                      &u8TransactionSequenceNumber,
                                                      &sZdpIeeeAddrReq);
        if( ZPS_APL_ZDP_E_INV_REQUESTTYPE  == eStatus ||
            ZPS_APL_ZDP_E_INSUFFICIENT_SPACE== eStatus )
        {
            PDUM_eAPduFreeAPduInstance(hAPduInst);
        }
        DBG_vPrintf(TRACE_FB_INTIATOR, "ZPS_eAplZdpIeeeAddrRequest status %0x\r\n", eStatus);
    }
}

/****************************************************************************
 **
 ** NAME:       vFbSendSimpleDescReq
 **
 ** DESCRIPTION:
 ** This function is used to send a ZDP simple descriptor request
 **
 ** PARAMETERS:                 Name               Usage
 ** uint8                   u8DstEndpoint       Destination end point to which request to be sent
 ** uint16                  u16DstAddr          Destination source address to which request to be sent
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/
PRIVATE void vFbSendSimpleDescReq(uint8           u8DstEndpoint, 
                                  uint16          u16DstAddr)
{
    uint8 u8TransactionSequenceNumber;
    PDUM_thAPduInstance hAPduInst;
    ZPS_tsAplZdpSimpleDescReq sZdpSimpleDescReq;
    ZPS_tuAddress uDstAddr;
    uDstAddr.u16Addr = u16DstAddr;


    sZdpSimpleDescReq.u16NwkAddrOfInterest = u16DstAddr;
    sZdpSimpleDescReq.u8EndPoint = u8DstEndpoint;

    hAPduInst = PDUM_hAPduAllocateAPduInstance(apduZDP);

    if (hAPduInst == PDUM_INVALID_HANDLE)
    {
        DBG_vPrintf(TRACE_FB_INTIATOR, "Simple Desc Request - PDUM_INVALID_HANDLE\n");
    }
    else
    {
        ZPS_teStatus eStatus = ZPS_eAplZdpSimpleDescRequest(
             hAPduInst,
             uDstAddr,
             FALSE,
             &u8TransactionSequenceNumber,
             &sZdpSimpleDescReq);
        if( ZPS_APL_ZDP_E_INV_REQUESTTYPE  == eStatus ||
                    ZPS_APL_ZDP_E_INSUFFICIENT_SPACE== eStatus )
        {
            PDUM_eAPduFreeAPduInstance(hAPduInst);
        }
        DBG_vPrintf(TRACE_FB_INTIATOR, "ZPS_eAplZdpSimpleDescRequest status %0x\r\n", eStatus);
    }
}

/****************************************************************************
 *
 * NAME:       vFbHandleSimpleDescResp
 *
 * DESCRIPTION:
 * Function to check if there is a client to server match with received simple
 * descriptor and to source bind if there is a match.
 *
 * PARAMETERS:                 Name               Usage
 * tsFindAndBind              *psFindAndBind         backed up EZ details entry
 * ZPS_tsAfEvent               pZPSevent         ZPS response received
 *
 * RETURN:
 * None
 *
 ****************************************************************************/
PRIVATE void vFbHandleSimpleDescResp(ZPS_tsAfZdpEvent      *psAfZdpEvent)
{
    BDB_tsFindAndBindEvent sEvent = {0};
    BDB_tsBdbEvent sBdbEvent = {0};
    bool_t bGroupAdded = FALSE;
    
    int i=0;

    uint64 u64DstIeeeAddr = 0;
    bool_t bAddAddressMapEntry=FALSE;

    DBG_vPrintf(TRACE_FB_INTIATOR, "BDB_FB_vHandleSimpleDescResponse started \r\n");

    /* Retreive IEEE from stack/ or from backed up data*/
    u64DstIeeeAddr = ZPS_u64AplZdoLookupIeeeAddr(psAfZdpEvent->uZdpData.sSimpleDescRsp.u16NwkAddrOfInterest);

    /* IEEE address is not present with ZStack, get it from backed up data */
    if(u64DstIeeeAddr == 0)
    {
        for(i = 0; i < BDB_FB_MAX_TARGET_DEVICES; i++ )
        {
            if(sFindAndBind.asTargetInfo[i].u16NwkAddr == psAfZdpEvent->uZdpData.sSimpleDescRsp.u16NwkAddrOfInterest)
            {
                DBG_vPrintf(TRACE_FB_INTIATOR, "address match %d \r\n",sFindAndBind.asTargetInfo[i].u16NwkAddr);
                u64DstIeeeAddr = sFindAndBind.asTargetInfo[i].u64IeeeAddr ;
                bAddAddressMapEntry = TRUE;
                break;
            }
        }
    }

    DBG_vPrintf(TRACE_FB_INTIATOR, "BDB_FB_vHandleSimpleDescResponse IEEE address = %016llx \r\n",u64DstIeeeAddr);

    if(ZPS_NWK_ENUM_SUCCESS == psAfZdpEvent->uZdpData.sSimpleDescRsp.u8Status)
    {
        sBdbEvent.eEventType = BDB_EVENT_FB_HANDLE_SIMPLE_DESC_RESP_OF_TARGET;
        sBdbEvent.uEventData.psFindAndBindEvent = &sEvent;
        sEvent.u8InitiatorEp = sFindAndBind.u8Endpoint;
        sEvent.u8TargetEp = psAfZdpEvent->uZdpData.sSimpleDescRsp.sSimpleDescriptor.u8Endpoint;
        sEvent.u16TargetAddress = psAfZdpEvent->uZdpData.sSimpleDescRsp.u16NwkAddrOfInterest;
        sEvent.u16ProfileId = psAfZdpEvent->uZdpData.sSimpleDescRsp.sSimpleDescriptor.u16ApplicationProfileId;
        sEvent.u16DeviceId = psAfZdpEvent->uZdpData.sSimpleDescRsp.sSimpleDescriptor.u16DeviceId;
        sEvent.u8DeviceVersion = psAfZdpEvent->uZdpData.sSimpleDescRsp.sSimpleDescriptor.uBitUnion.u8Value;
        sEvent.psAfZdpEvent = psAfZdpEvent;
        APP_vBdbCallback(&sBdbEvent);
        
        /*Step 5: Valid Descriptor Response Received */
        bGroupAdded = FALSE;
        vFbBindCluster(psAfZdpEvent,&u64DstIeeeAddr,bAddAddressMapEntry,FALSE, &bGroupAdded);
        vFbBindCluster(psAfZdpEvent,&u64DstIeeeAddr,bAddAddressMapEntry,TRUE,&bGroupAdded);

    }
}

/****************************************************************************
 *
 * NAME:       vFbBindCluster
 *
 * DESCRIPTION:
 * Binds the Source's clusters' clients to the remote server.
 *
 * PARAMETERS:             Name                Usage
 * tsFindAndBind *        psFindAndBind            EZ source
 * ZPS_tsAfEvent*          pZPSevent           Stack Event
 * RETURN:
 * None
 *
 ****************************************************************************/
PRIVATE void vFbBindCluster(ZPS_tsAfZdpEvent        *psAfZdpEvent,
                                uint64                   *pu64DstIeeeAddr,
                                bool_t                   bAddAddressMapEntry,
                                bool_t                   bServer,
                                bool_t*                     bGroupAdded)
{
    int i=0;
    int j=0;
    uint8 u8InitiatorCluster;
    uint8 u8TargetCluster;
    uint8 jj=0;
    uint16 * pCluster;
    ZPS_tsAplAfSimpleDescriptor sDescSelf;
    ZPS_tsAplZdpSimpleDescType *psSimpleDescRemote;
    uint8 u8DstAddrMode = ZPS_E_ADDR_MODE_IEEE;
    BDB_tsFindAndBindEvent sEvent = {0};
    BDB_tsBdbEvent sBdbEvent = {0};
    
    sBdbEvent.uEventData.psFindAndBindEvent = &sEvent;
    
    /* get simple descriptor of local node */
    ZPS_eAplAfGetSimpleDescriptor(sFindAndBind.u8Endpoint, &sDescSelf);
    psSimpleDescRemote = &psAfZdpEvent->uZdpData.sSimpleDescRsp.sSimpleDescriptor;
    
    /*Set up the binding loop counts and initialize for server or client */
    if(!bServer)
    {
        u8InitiatorCluster=sDescSelf.u8OutClusterCount;
        u8TargetCluster = psSimpleDescRemote->u8InClusterCount;
        jj=0;
        pCluster = (uint16*)sDescSelf.pu16OutClusterList;
    }
    else
    {
        u8InitiatorCluster=sDescSelf.u8InClusterCount;
        u8TargetCluster = psSimpleDescRemote->u8InClusterCount+psSimpleDescRemote->u8OutClusterCount;
        jj=psSimpleDescRemote->u8InClusterCount;
        pCluster = (uint16*)sDescSelf.pu16InClusterList;
    }

    /*Step 6: Does the next cluster match? */
    /* Check for Initiator Client With Target Server cluster match */
    for(i=0; i<u8InitiatorCluster;i++ )
    {
        for(j=jj;j < u8TargetCluster ; j++)
        {
            uint16 u16ClusterID;
            /* In cluster First in the list then Out clusters in the received pay load */
            u16ClusterID = psAfZdpEvent->uLists.au16Data[j];
            if( (u16ClusterID ==  pCluster[i]) &&
                (TRUE == bFbIsOpertationalCluster(u16ClusterID,bServer) ))
            {
                sEvent.u8InitiatorEp = sFindAndBind.u8Endpoint;
                sEvent.u8TargetEp = psSimpleDescRemote->u8Endpoint;
                sEvent.uEvent.u16ClusterId = u16ClusterID;
                sEvent.u16TargetAddress = psAfZdpEvent->uZdpData.sSimpleDescRsp.u16NwkAddrOfInterest;
                sEvent.bAllowBindOrGroup = TRUE;
                sEvent.psAfZdpEvent = psAfZdpEvent;
                
                if(E_FB_IDLE_STATE != sFindAndBind.eFBState )
                {
                    sBdbEvent.eEventType = BDB_EVENT_FB_CHECK_BEFORE_BINDING_CLUSTER_FOR_TARGET;
                    APP_vBdbCallback(&sBdbEvent);
                    
                    /* If IEEE Source Binding shall be performed or group binding based on user choice */
                    if(sBDB.sAttrib.u16bdbCommissioningGroupID != 0xFFFF)
                    {
                        u8DstAddrMode = ZPS_E_ADDR_MODE_GROUP; 
                    }
                    
                    /* Has user allowed binding or grouping for this cluster */
                    if(sEvent.bAllowBindOrGroup)
                    {
                        /*Step 7: Attempt to create an entry in the binding table with the next matching cluster */
                        /* Create source Binding in IEEE address mode or group address mode */
                        vFbCreateSourceBinding(u16ClusterID,psAfZdpEvent,pu64DstIeeeAddr,bAddAddressMapEntry,u8DstAddrMode);
                        
                        /*Step 8: is bdbCommissioningGroupID = 0xffff? */
                        /* If Grouping then send add group command as well */
                        if ((u8DstAddrMode == ZPS_E_ADDR_MODE_GROUP) && (FALSE == *bGroupAdded))
                        {
                            vFbSendAddGroupCommand(sFindAndBind.u8Endpoint,psSimpleDescRemote->u8Endpoint,
                                        psAfZdpEvent->uZdpData.sSimpleDescRsp.u16NwkAddrOfInterest,sEvent.bGroupCast);
                            *bGroupAdded = TRUE;
                        }
                    }
                    break;
                }
            }
        }
    }
    
    /* For IEEE address source binding give a final callback so that application can send stop identification */
    if((sBDB.sAttrib.u16bdbCommissioningGroupID == 0xFFFF) && bServer)
    {
        sEvent.u8InitiatorEp = sFindAndBind.u8Endpoint;
        sEvent.u16TargetAddress = psAfZdpEvent->uZdpData.sSimpleDescRsp.u16NwkAddrOfInterest;
        sEvent.u8TargetEp = psSimpleDescRemote->u8Endpoint;
        sBdbEvent.eEventType = BDB_EVENT_FB_BIND_CREATED_FOR_TARGET;
        APP_vBdbCallback(&sBdbEvent);
    }
    /* Change BDB status into Success */
    sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_SUCCESS;  
}

/****************************************************************************
 *
 * NAME: BDB_FB_bIsOperational
 *
 * DESCRIPTION:
 * Tests if a cluster is Type 1 or Type 2 operational cluster or not
 *
 * PARAMETERS:  Name                            Usage
 *              u16ClusterID                   ClusterID to be excluded from binding
 *              bServer                        Server or client
 *
 * RETURNS:
 * True or False
 ****************************************************************************/
PRIVATE bool_t bFbIsOpertationalCluster(uint16         u16ClusterID,
                                             bool_t         bServer)
{
    
    if(bServer)
    {
        /* Type 2 server to client*/
        switch(u16ClusterID)
        {
            case 0x0007:            /* OOSC */
            case 0x0009:            /* Alarms */
            case 0x0400:            /* Illuminannce Measurement */
            case 0x0401:            /* Illuminannce Level Sensing */
            case 0x0402:            /* Temperature Measurement */
            case 0x0403:            /* Pressure Measurement */
            case 0x0404:            /* Flow Measurement */
            case 0x0405:            /* Relative Humidity Measurement */ 
            case 0x0406:            /* Occupancy Sensing */
            case 0x0201:            /* Thermostat*/            
            case 0x0101:            /* Door Lock*/        
            case 0x0500:            /* IAS Zone*/      
            case 0x001B:            /* Appliance Control */
            case 0x0B00:            /* Appliance Identification */
            case 0x0B01:            /* Appliance Events and Alerts */
            case 0x0B02:            /* Appliance Statistics */    
                return TRUE;
            break;
            default:
                return FALSE;
            break;
        }   
    }
    else{
        /* Type 1 client to server*/
        switch(u16ClusterID)
        {
            case 0x0006:         /* OnOff */
            case 0x0008:         /* LevelControl */
            case 0x0B04:         /* Electrical Measurement */
            case 0x0300:         /* Color Control */
            case 0x0301:         /* Ballast Configuration */
            case 0x0204:         /* Thermostat UI Config */        
            case 0x0501:         /* IAS ACE */  
            case 0x0502:         /* IAS WD */  
            case 0x0700:         /* Price */
            case 0x0701:         /* DRLC */
            case 0x0702:         /* Simple Metering */
                return TRUE;
            break;
            default:
                return FALSE;
            break;
        }
    }
    return FALSE;
}

/****************************************************************************
 **
 ** NAME:       vFbCreateSourceBinding
 **
 ** DESCRIPTION:
 ** Creates a source Binding
 **
 ** PARAMETERS:             Name                Usage
 ** uint8                   u8SourceEndPointId  EZ source EP Id
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/
PRIVATE void vFbCreateSourceBinding(
                                uint16              u16ClusterID,
                                ZPS_tsAfZdpEvent    *psAfZdpEvent,
                                uint64              *pu64DstIeeeAddr,
                                bool_t              bAddAddressMapEntry,
                                uint8               u8AddressMode)
{
    BDB_tsFindAndBindEvent sEvent = {0};
    BDB_tsBdbEvent sBdbEvent = {0};
    
    sBdbEvent.uEventData.psFindAndBindEvent = &sEvent;
    
    ZPS_teStatus eStatus;
    if(u8AddressMode == ZPS_E_ADDR_MODE_IEEE)
    {
        /* IEEE source Bind */
        eStatus = ZPS_eAplZdoBind(  u16ClusterID,
                                    sFindAndBind.u8Endpoint,
                                    psAfZdpEvent->uZdpData.sSimpleDescRsp.u16NwkAddrOfInterest, 
                                    *pu64DstIeeeAddr,
                                    psAfZdpEvent->uZdpData.sSimpleDescRsp.sSimpleDescriptor.u8Endpoint);        
        
    }
    else{
        /* Perform Group mode binding */
        eStatus = ZPS_eAplZdoBindGroup(u16ClusterID,
                                       sFindAndBind.u8Endpoint,
                                       sBDB.sAttrib.u16bdbCommissioningGroupID);
    }
    
    if(ZPS_E_SUCCESS == eStatus)
    {
        /* For IEEE address mode create address map entry */
        if(bAddAddressMapEntry && (u8AddressMode == ZPS_E_ADDR_MODE_IEEE))
        {
            ZPS_eAplZdoAddAddrMapEntry( psAfZdpEvent->uZdpData.sSimpleDescRsp.u16NwkAddrOfInterest,
                                        *pu64DstIeeeAddr, 
                                        FALSE);
        }
        
        sBdbEvent.eEventType = BDB_EVENT_FB_CLUSTER_BIND_CREATED_FOR_TARGET;
        sEvent.u8InitiatorEp = sFindAndBind.u8Endpoint;
        sEvent.u8TargetEp = psAfZdpEvent->uZdpData.sSimpleDescRsp.sSimpleDescriptor.u8Endpoint;
        sEvent.u16TargetAddress = psAfZdpEvent->uZdpData.sSimpleDescRsp.u16NwkAddrOfInterest;
        sEvent.uEvent.u16ClusterId = u16ClusterID;
        
    }
    else if(ZPS_APL_APS_E_TABLE_FULL  == eStatus) /*Step 7: Is the binding table full? */
    {
        sBdbEvent.eEventType = BDB_EVENT_FB_ERR_BINDING_TABLE_FULL;
        /* Change BDB status into Binding Table Full */
        sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_BINDING_TABLE_FULL;
        /* As Binding Table Full exit as initiator */
        BDB_vFbExitAsInitiator();
    }
    else if(ZPS_APL_APS_E_INVALID_BINDING  == eStatus)
    {
        sBdbEvent.eEventType = BDB_EVENT_FB_ERR_BINDING_FAILED;
    }
    
    APP_vBdbCallback(&sBdbEvent);  
}

/****************************************************************************
 *
 * NAME: vFbSendAddGroupCommand
 *
 * DESCRIPTION:
 * This function sends a add group command or add group if identifying command
 *
 * PARAMETERS:  Name                            Usage
 *              u8SourceEndpoint              Stack events
 *              u16ShortAddress               network address to which add group if identify should be send
 *              bGroupCast                    If a group cast is required
 * RETURNS:
 * None
 *
 ****************************************************************************/
PRIVATE void vFbSendAddGroupCommand(uint8      u8SourceEndpoint, 
                                       uint8        u8DstEndpoint,
                                       uint16       u16ShortAddress,
                                       bool_t       bGroupCast)
{
    teZCL_Status eZCLStatus = E_ZCL_SUCCESS;

#ifdef GROUPS_CLIENT
    uint8 u8Seq;
    tsZCL_Address sAddress;
    tsCLD_Groups_AddGroupRequestPayload sPayload;
#endif
    BDB_tsFindAndBindEvent sEvent={0};
    BDB_tsBdbEvent sBdbEvent = {0};
    
    sBdbEvent.uEventData.psFindAndBindEvent = &sEvent;
    
#ifdef GROUPS_CLIENT
    /*  Add group request Payload */
    sPayload.sGroupName.pu8Data     = (uint8*)"";
    sPayload.sGroupName.u8Length    = 0;
    sPayload.sGroupName.u8MaxLength = 0;
    /*Set the NwkId as the group ID to make it unique - The nwk update addis not taken care here.*/
    sPayload.u16GroupId             = sBDB.sAttrib.u16bdbCommissioningGroupID;


    if(bGroupCast)
    {
        sAddress.eAddressMode               = E_ZCL_AM_BROADCAST;
        sAddress.uAddress.eBroadcastMode    = ZPS_E_APL_AF_BROADCAST_RX_ON;
        /* Sending the Add group to the device those are identifying in broadcast*/

        eZCLStatus = eCLD_GroupsCommandAddGroupIfIdentifyingRequestSend(
                            u8SourceEndpoint,
                            u8DstEndpoint,
                            &sAddress,
                            &u8Seq,
                            &sPayload);
    }
    else
    {
        sAddress.eAddressMode                   = E_ZCL_AM_SHORT;
        sAddress.uAddress.u16DestinationAddress = u16ShortAddress;
        /* Send the Add group to the individual device which are identifying*/
        eZCLStatus = eCLD_GroupsCommandAddGroupRequestSend(
                            u8SourceEndpoint,
                            u8DstEndpoint,
                            &sAddress,
                            &u8Seq,
                            &sPayload);
    }
#endif

    DBG_vPrintf(TRACE_FB_INTIATOR, "AddgroupCommand status = %d, EP= %d\r\n",eZCLStatus,u8SourceEndpoint);
        
    if(eZCLStatus == E_ZCL_SUCCESS)
    {
        sFindAndBind.u8ReSendQueryTime = BDB_FB_RESEND_IDENTIFY_QUERY_TIME;
        sBdbEvent.eEventType = BDB_EVENT_FB_GROUP_ADDED_TO_TARGET;
        sEvent.u8InitiatorEp = u8SourceEndpoint;
        sEvent.u8TargetEp = u8DstEndpoint;
        sEvent.uEvent.u16GroupId = sBDB.sAttrib.u16bdbCommissioningGroupID ;
        sEvent.u16TargetAddress = u16ShortAddress;        
    }
#ifdef GROUPS_CLIENT
    else
    {
        sBdbEvent.eEventType = BDB_EVENT_FB_ERR_GROUPING_FAILED;
    }
#endif
    APP_vBdbCallback(&sBdbEvent); 
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
