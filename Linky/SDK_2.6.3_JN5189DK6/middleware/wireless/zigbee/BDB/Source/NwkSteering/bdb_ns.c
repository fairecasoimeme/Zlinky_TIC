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
# COMPONENT:   bdb_ns.c
#
# DESCRIPTION: BDB Network Steering Implementation
#              
#
###############################################################################*/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
#include "bdb_api.h"
#include "bdb_ns.h"
#include "bdb_start.h"
#include "dbg.h"
#include "pdum_gen.h"
#include <string.h>
#include <stdlib.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void vNsDiscoverNwk(void);
PRIVATE void vNsTryNwkJoin(bool_t bStartWithIndex0,
                           ZPS_tsAfNwkDiscoveryEvent *psNwkDiscoveryEvent);
PRIVATE void vNsAfterNwkJoin(void);
PRIVATE void vNsStartTclk(void);
PRIVATE void vNsTclkSendNodeDescReq(void);
PRIVATE void vNsTclkSendReqKey(void);
PRIVATE void vNsTclkResendVerKey(void);
PRIVATE void vNsLeaveNwk(void);
PRIVATE void vNsTerminateNwkSteering(void);
PRIVATE void vNsSendPermitJoin(void);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
PUBLIC teNS_State eNS_State = E_NS_IDLE;
PUBLIC uint8 u8TimerBdbNs;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
static bool_t              bDoPrimaryScan;  //vDoPrimaryScan
static uint32              u32ScanChannels; //vScanChannels
static uint8               u8ScanChannel;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: BDB_eNsStartNwkSteering
 *
 * DESCRIPTION: As per, BDB Specification 13-0402 Section 8.2
 *  This API is used for steering the nwk
 *  1. For a node already on the nwk - this API opens up the entire nwk
 *     so that Factory New devices can be commissioned into this nwk. 
 *     The node broadcasts Mgmt_Permit_Joining_req ZDO command with the
 *     PermitDuration field set to at least bdbcMinCommissioningTime which is
 *     180 sec by default but can be altered through bdb_options.h file.
 *     After this, BDB_EVENT_NWK_STEERING_SUCCESS call back event is generated
 *     to application via APP_vBdbCallback.
 *  2. For ZigBee Router or End Device node which is not on the nwk (FN node), 
 *     below steps are performed.
 *      a. Discovery is performed over each channel in primary channel set.
 *      b. After discovery is complete, network join is attempted one by one on
 *          each discovered open network for a maximum of BDBC_MAX_SAME_NETWORK_RETRY_ATTEMPTS
 *          times.
 *      c. When network join is successful, bbdbNodeIsOnANetwork is set to TRUE
 *      d. If no network found or join was unsuccessful then above steps a to call
 *         are performed using secondary channel set.
 *      e. If ApsTrustCenterAddress is not 0xFFFFFFFFFFFFFFFFULL (i.e. Centralized
 *          network), then node_desc_req/resp is check for r21 Trust center.
 *          If that is the case then TCLK (Trust center link key exchange) procedure
 *          is carried out.
 *          Failure at any stage of TCLK procedure results into device sending
 *          network leave command and generates BDB_EVENT_NWK_JOIN_FAILURE to
 *          Application via APP_vBdbCallback.
 *      f.  On successful join or on successful completion of TCLK procedure,
 *          Mgmt_Permit_Joining_req is broadcasted to extend the network open window. 
 *          BDB_EVENT_NWK_STEERING_SUCCESS callback event is generated to application.
 *
 * PARAMETERS:      Name            RW  Usage
 *
 * RETURNS:
 *  BDB_E_ERROR_IMPROPER_COMMISSIONING_MODE, if bdbCommissioningMode doesn't 
 *          support nwk steering.
 *  BDB_E_ERROR_COMMISSIONING_IN_PROGRESS, if Base device is already in one 
 *          of the commissioning mode.
 *  BDB_E_ERROR_INVALID_DEVICE, if Factory new device is ZigBee Coordinator
 *
 * EVENTS:
 *  BDB_EVENT_NWK_STEERING_SUCCESS,     see description above
 *  BDB_EVENT_NWK_JOIN_FAILURE,         see description above 
 *  BDB_EVENT_NO_NETWORK,               if no open network discovered to join
 *
 ****************************************************************************/
PUBLIC BDB_teStatus BDB_eNsStartNwkSteering(void)
{
    BDB_tsBdbEvent sBdbEvent;

    if(!(sBDB.sAttrib.u8bdbCommissioningMode & BDB_COMMISSIONING_MODE_NWK_STEERING))
    {
        return BDB_E_ERROR_IMPROPER_COMMISSIONING_MODE;
    }

    if(sBDB.sAttrib.ebdbCommissioningStatus == E_BDB_COMMISSIONING_STATUS_IN_PROGRESS)
    {
       return BDB_E_ERROR_COMMISSIONING_IN_PROGRESS;
    }

    sBDB.eState = E_STATE_BASE_SLAVE;

    if(sBDB.sAttrib.bbdbNodeIsOnANetwork && !bAssociationJoin)
    {
        /* Network steering procedure for a node on a network
         * BDB Specification 13-0402 Section 8.2 */

        /* 8.2-1. The node first sets bdbCommissioningStatus to IN_PROGRESS. */
        sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_IN_PROGRESS;

        /* 8.2-2. The node SHALL broadcast the Mgmt_Permit_Joining_req ZDO command
                  with the PermitDuration field set to at least bdbcMinCommissioningTime and
                  the TC_Significance field set to 0x01. */
        /* 8.2-3. NLME-PERMIT-JOINING.request */
        vNsSendPermitJoin();

        /* 8.2-4. The node then sets bdbCommissioningStatus to SUCCESS and it SHALL
                  terminate the network steering procedure for a node on a network. */
        sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_SUCCESS;

        sBDB.eState = E_STATE_BASE_ACTIVE;
        sBdbEvent.eEventType = BDB_EVENT_NWK_STEERING_SUCCESS;
        APP_vBdbCallback(&sBdbEvent);
        return BDB_E_SUCCESS;
    }
    else
    {
        /* Network steering procedure for a node NOT on a network
         * BDB Specification 13-0402 Section 8.3 */

        if(ZPS_ZDO_DEVICE_COORD == ZPS_eAplZdoGetDeviceType())
        {
            /* 6.1 A ZigBee coordinator SHALL form a centralized security
             *     network and, as such, SHALL NOT attempt to join another network. */
            DBG_vPrintf(TRACE_BDB,"BDB: Coordinator SHALL NOT attempt to join another nwk \n");
            return BDB_E_ERROR_INVALID_DEVICE;
        }

        /* 8.3.1 The node first sets bdbCommissioningStatus to IN_PROGRESS,
                 vDoPrimaryScan to TRUE and vScanChannels set to bdbPrimaryChannelSet. */
        sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_IN_PROGRESS;
        bDoPrimaryScan = TRUE;
        u32ScanChannels = sBDB.sAttrib.u32bdbPrimaryChannelSet;
        u8ScanChannel = BDB_CHANNEL_MIN;
        bAssociationJoin = FALSE;
        vNsDiscoverNwk();
        return BDB_E_SUCCESS;
    }
}

/****************************************************************************
 *
 * NAME: BDB_vNsStateMachine
 *
 * DESCRIPTION:
 *  BDB Network Steering state machine; this is utilized internally by BDB.
 *
 * PARAMETERS:      Name            RW  Usage
 *                  BDB_tsZpsAfEvent    Zps event
 * RETURNS:
 *  void
 *
 ****************************************************************************/
PUBLIC void BDB_vNsStateMachine(BDB_tsZpsAfEvent *psZpsAfEvent)
{
    BDB_tsBdbEvent sBdbEvent;
    uint32 u32Index,i;
    ZPS_tsAplApsKeyDescriptorEntry *psApsKeyDesc;

    switch(eNS_State)
    {
        case E_NS_IDLE:
            break;

        case E_NS_WAIT_DISCOVERY:
            switch(psZpsAfEvent->sStackEvent.eType)
            {
                case ZPS_EVENT_NWK_DISCOVERY_COMPLETE:
                    if((MAC_ENUM_SUCCESS == psZpsAfEvent->sStackEvent.uEvent.sNwkDiscoveryEvent.eStatus) ||\
                       (ZPS_NWK_ENUM_NEIGHBOR_TABLE_FULL == psZpsAfEvent->sStackEvent.uEvent.sNwkDiscoveryEvent.eStatus))
                    {
                        /* 8.3-4 The node SHALL determine whether any suitable networks with a permit
                                 joining flag set to TRUE were found by analyzing the NetworkCount and
                                 NetworkDescriptor parameters. The decision regarding what constitutes a
                                 suitable network is application specific

                                 Trying all but must have been filtered through beacon filtering. */
                        eNS_State = E_NS_WAIT_JOIN;
                        vNsTryNwkJoin(TRUE, &(psZpsAfEvent->sStackEvent.uEvent.sNwkDiscoveryEvent));
                    }
                    else
                    {
                        /* 8.3-3 If the Status parameter from the NLME-NETWORK-DISCOVERY.confirm
                             primitive is not equal to SUCCESS, indicating that the channel scan was not
                             successful, the node SHALL continue from step 12 (8.3-12). */
#if ((SINGLE_CHANNEL < BDB_CHANNEL_MIN) || (SINGLE_CHANNEL > BDB_CHANNEL_MAX))
                        if((!u32ScanChannels) || (u8ScanChannel > BDB_CHANNEL_MAX))
                        {
                            if(bDoPrimaryScan == FALSE)
#else
                        if (TRUE)
                        {
#endif
                            {
                                /* 8.3.16 ... set bdbCommissioningStatus to NO_NETWORK and then it SHALL terminate
                                the network steering procedure for a node not on a network. */
                                vNsTerminateNwkSteering();;
                            }
                        }
                        else
                        {
                            vNsDiscoverNwk();
                        }
                    }
                    break;
                default:
                    break;
            }
            break;

        case E_NS_WAIT_JOIN:
            switch(psZpsAfEvent->sStackEvent.eType)
            {
                case ZPS_EVENT_NWK_FAILED_TO_JOIN:
                    /* 8.3-8 Retry */
                    DBG_vPrintf(TRACE_BDB,"FailedToJoin sNwkJoinFailedEvent.u8Status=%d !!!\n",psZpsAfEvent->sStackEvent.uEvent.sNwkJoinFailedEvent.u8Status);
                    vNsTryNwkJoin(FALSE, NULL);
                    break;
                case ZPS_EVENT_NWK_JOINED_AS_ROUTER:
                case ZPS_EVENT_NWK_JOINED_AS_ENDDEVICE:
                     /* 8.3-8  SHALL set bdbNodeJoinLinkKeyType accordingly to indicate the type of link
                               key used to decrypt the received network key. */
                    psApsKeyDesc = ZPS_psGetActiveKey(ZPS_u64AplZdoLookupIeeeAddr(psZpsAfEvent->sStackEvent.uEvent.sNwkJoinedEvent.u16Addr), &u32Index);

                    for(i=0;i<ZPS_SEC_KEY_LENGTH;i++)
                    {
                        DBG_vPrintf(TRACE_BDB,"%0x:",psApsKeyDesc->au8LinkKey[i]);
                    }

                    if(!memcmp(sBDB.pu8DefaultTCLinkKey, &psApsKeyDesc->au8LinkKey[0], ZPS_SEC_KEY_LENGTH))
                    {
                        sBDB.sAttrib.u8bdbNodeJoinLinkKeyType = DEFAULT_GLOBAL_TRUST_CENTER_LINK_KEY;
                    }
                    else if(!memcmp(sBDB.pu8DistributedLinkKey, &psApsKeyDesc->au8LinkKey[0], ZPS_SEC_KEY_LENGTH))
                    {
                        sBDB.sAttrib.u8bdbNodeJoinLinkKeyType = DISTRIBUTED_SECURITY_GLOBAL_LINK_KEY;
                    }
                    else if(!memcmp(sBDB.pu8TouchLinkKey, &psApsKeyDesc->au8LinkKey[0], ZPS_SEC_KEY_LENGTH))
                    {
                        sBDB.sAttrib.u8bdbNodeJoinLinkKeyType = TOUCHLINK_PRECONFIGURED_LINK_KEY;
                    }
                    else if(!memcmp(sBDB.pu8PreConfgLinkKey, &psApsKeyDesc->au8LinkKey[0], ZPS_SEC_KEY_LENGTH))
                    {
                        sBDB.sAttrib.u8bdbNodeJoinLinkKeyType =  INSTALL_CODE_DERIVED_PRECONFIGURED_LINK_KEY;
                    }

                    /* 8.3-10 The node sets bdbNodeIsOnANetwork to TRUE and then broadcasts a
                              Device_annce ZDO command. If apsTrustCenterAddress is equal to
                              0xffffffffffffffff, the node SHALL and continue from step 13. */
                    sBDB.sAttrib.bbdbNodeIsOnANetwork = TRUE;

                    /* Device announce happens automatically by the stack! */

                    eNS_State = E_NS_WAIT_AFTER_NWK_JOIN;
                    ZTIMER_eStart(u8TimerBdbNs, ZTIMER_TIME_MSEC(300));
                    break;
                default:
                    break;
            }
            break;

        case E_NS_WAIT_AFTER_NWK_JOIN:
            break;

        case E_NS_TCLK_WAIT_NODE_DESC_RES:
            switch(psZpsAfEvent->sStackEvent.eType)
            {
                case ZPS_EVENT_APS_DATA_INDICATION:
                    if ((psZpsAfEvent->sStackEvent.uEvent.sApsDataIndEvent.u16ClusterId == ZPS_ZDP_NODE_DESC_RSP_CLUSTER_ID) &&
                        (0 == psZpsAfEvent->u8EndPoint))
                    {
                        ZPS_tsAfZdpEvent sAfZdpEvent;
                        ZTIMER_eStop(u8TimerBdbNs);

                        zps_bAplZdpUnpackNodeDescResponse(&psZpsAfEvent->sStackEvent, &sAfZdpEvent);
                        if((sAfZdpEvent.uZdpData.sNodeDescRsp.u8Status != ZPS_E_SUCCESS) ||
                           ((sAfZdpEvent.uZdpData.sNodeDescRsp.sNodeDescriptor.u16ServerMask >> 9) < 21))
                        {
                            DBG_vPrintf(TRACE_BDB,"BDB: Non r21 Trust Center \n");
                            eNS_State = E_NS_IDLE;
                            vNsAfterNwkJoin();
                        }
                        else
                        {
                            DBG_vPrintf(TRACE_BDB,"BDB: r21 Trust Center \n");
                            sBDB.sAttrib.u8bdbTCLinkKeyExchangeAttempts = 0;
                            vNsTclkSendReqKey();
                        }
                    }
                    break;
                default:
                    break;
            }
            break;

        case E_NS_TCLK_WAIT_SEND_REQ_KEY:
        case E_NS_TCLK_WAIT_SEND_VER_KEY:
            switch(psZpsAfEvent->sStackEvent.eType)
            {
                case ZPS_EVENT_TC_STATUS:
                    if ( (psZpsAfEvent->sStackEvent.uEvent.sApsTcEvent.u8Status == ZPS_APL_APS_E_SECURED_LINK_KEY) &&
                         (psZpsAfEvent->sStackEvent.uEvent.sApsTcEvent.uTcData.u64ExtendedAddress == ZPS_psAplAibGetAib()->u64ApsTrustCenterAddress) )
                    {
                        DBG_vPrintf(TRACE_BDB,"ZPS_EVENT_TC_STATUS Received Transport Key from TC\n");
                        eNS_State = E_NS_TCLK_WAIT_SEND_VER_KEY;
                        sBDB.sAttrib.u8bdbTCLinkKeyExchangeAttempts = 0;
                        break;
                    }
                    ZTIMER_eStop(u8TimerBdbNs);
                    if (psZpsAfEvent->sStackEvent.uEvent.sApsTcEvent.u8Status == 0x00)
                    {
                        int i;
                        DBG_vPrintf(TRACE_BDB,"ZPS_EVENT_TC_STATUS Success\n");
                        for(i=0;i<16;i++)
                        {
                            DBG_vPrintf(TRACE_BDB,"%02x:",psZpsAfEvent->sStackEvent.uEvent.sApsTcEvent.uTcData.pKeyDesc->au8LinkKey[i]);
                        }
                        DBG_vPrintf(TRACE_BDB,"\n");
                        vNsAfterNwkJoin();
                    }
                    else
                    {
                        DBG_vPrintf(TRACE_BDB,"ZPS_EVENT_TC_STATUS failure\n");
                        vNsLeaveNwk();
                    }
                    break;
                default:
                    DBG_vPrintf(1,"psZpsAfEvent->sStackEvent.eType=%d if ErrCode %d\n",
                            psZpsAfEvent->sStackEvent.eType, psZpsAfEvent->sStackEvent.uEvent.sAfErrorEvent.eError);
                    break;
            }
            break;
        case E_NS_TCLK_WAIT_LEAVE_CFM:
            switch(psZpsAfEvent->sStackEvent.eType)
            {
                case ZPS_EVENT_NWK_LEAVE_CONFIRM:
                    if(psZpsAfEvent->sStackEvent.uEvent.sNwkLeaveConfirmEvent.u64ExtAddr == 0UL)
                    {
                        DBG_vPrintf(TRACE_BDB,"ZPS_EVENT_NWK_LEAVE_CONFIRM status %d \n",
                                               psZpsAfEvent->sStackEvent.uEvent.sNwkLeaveConfirmEvent.eStatus);
                        sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_TCLK_EX_FAILURE;
                        sBDB.sAttrib.bbdbNodeIsOnANetwork = FALSE;
                        eNS_State = E_NS_IDLE;
                        sBDB.eState = E_STATE_BASE_FAIL;
                        sBdbEvent.eEventType = BDB_EVENT_NWK_JOIN_FAILURE;
                        APP_vBdbCallback(&sBdbEvent);
                    }
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }
}

/****************************************************************************
 *
 * NAME: BDB_vNsTimerCb
 *
 * DESCRIPTION:
 *  a.  After device announce from joining device, BDB takes 300 ms of delay
 *      before sending node_desc_req.
 *  b.  After sending  node_desc_req, BDB starts 5 second of timer to track
 *      timeout.  As per BDBC_TC_LINK_KEY_EXCHANGE_TIMEOUT
 *  c.  After TCLK started, BDB starts 5 seconds of timer to trance TCLK
 *  exchange timeout. As per BDBC_TC_LINK_KEY_EXCHANGE_TIMEOUT
 *
 * RETURNS:
 *  void
 *
 ****************************************************************************/
PUBLIC void BDB_vNsTimerCb(void *pvParam)
{
    switch(eNS_State)
    {
        case E_NS_WAIT_AFTER_NWK_JOIN:
            DBG_vPrintf(TRACE_BDB, "BDB: BDB_vNsTimerCb 1\n");

            if(ZPS_psAplAibGetAib()->u64ApsTrustCenterAddress == 0xFFFFFFFFFFFFFFFFULL)
            {
                eNS_State = E_NS_IDLE;
                vNsAfterNwkJoin();
            }
            else
            {
                /*  11. The node SHALL perform the procedure for retrieving a new Trust Center
                        link key (see sub-clause 10.2.5). If the procedure is successful, the node
                        SHALL continue from step 13. If the procedure is not successful, the node
                        SHALL perform a leave request on its old network and resets its network
                        parameters. The node then sets bdbNodeIsOnANetwork to FALSE and sets
                        bdbCommissioningStatus to TCLK_EX_FAILURE. To perform a leave
                        request, the node issues the NLME-LEAVE.request primitive to the NWK
                        layer with the DeviceAddress parameter set to NULL, the RemoveChildren
                        parameter set to FALSE and the Rejoin parameter set to FALSE. On receipt
                        of the NLME-LEAVE.confirm primitive, the node is notified of the status of
                        the request to leave the network. The node SHALL then terminate the */
                if(0x00 == sBDB.sAttrib.u8bdbTCLinkKeyExchangeMethod)
                {
                    vNsStartTclk();
                }
                else
                {
                    DBG_vPrintf(TRACE_BDB,"Only ApsReqKey is supported !\n");
                    eNS_State = E_NS_IDLE;
                    vNsAfterNwkJoin();
                }
            }
            break;
        case E_NS_TCLK_WAIT_NODE_DESC_RES:
            DBG_vPrintf(TRACE_BDB, "BDB: BDB_vNsTimerCb 2\n");

            if(sBDB.sAttrib.u8bdbTCLinkKeyExchangeAttempts < sBDB.sAttrib.u8bdbTCLinkKeyExchangeAttemptsMax)
            {
                vNsTclkSendNodeDescReq();
            }
            else
            {
                DBG_vPrintf(TRACE_BDB, "BDB: TCLK DiscReq attempts over \n");
                vNsLeaveNwk();
            }
            break;
        case E_NS_TCLK_WAIT_SEND_VER_KEY:
        case E_NS_TCLK_WAIT_SEND_REQ_KEY:
            DBG_vPrintf(TRACE_BDB, "BDB: BDB_vNsTimerCb 3\n");

            if(sBDB.sAttrib.u8bdbTCLinkKeyExchangeAttempts < sBDB.sAttrib.u8bdbTCLinkKeyExchangeAttemptsMax)
            {
                if (eNS_State == E_NS_TCLK_WAIT_SEND_REQ_KEY)
                {
                    vNsTclkSendReqKey();
                }
                else
                {
                    vNsTclkResendVerKey();
                }
            }
            else
            {
                DBG_vPrintf(TRACE_BDB, "BDB: TCLK SendReqKey attempts over \n");
                vNsLeaveNwk();
            }
            break;
        default:
            break;
    }
}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: vNsDiscoverNwk
 *
 * DESCRIPTION:
 * Attempts discovery on each of the channels, if unsuccessful initiate
 * discovery on next channel
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vNsDiscoverNwk()
{
    if((!u32ScanChannels) || (u8ScanChannel > BDB_CHANNEL_MAX))
    {
        /* 8.3.12 If vDoPrimaryScan is equal to FALSE or bdbSecondaryChannelSet is equal to
                  0x00000000, the node SHALL continue from step 16. If
                  bdbSecondaryChannelSet is not equal to 0x00000000, the node SHALL set
                  vDoPrimaryScan to FALSE, set vScanChannels to bdbSecondaryChannelSet
                  and continue from step 2.*/
        if(bDoPrimaryScan == FALSE)
        {
            return;
        }
        else
        {
            bDoPrimaryScan = FALSE;
            u32ScanChannels = sBDB.sAttrib.u32bdbSecondaryChannelSet;
            u8ScanChannel = BDB_CHANNEL_MIN;
            vNsDiscoverNwk();
            return;
        }
    }
#ifdef ENABLE_SUBG_IF
    void *pvNwk = ZPS_pvAplZdoGetNwkHandle();
	ZPS_vNwkNibSetMacEnhancedMode(pvNwk, TRUE);
#endif

    /* 8.3-2 The node SHALL perform a channel scan in order to discover which networks
             are available within its radio range on a set of channels. */
    while(u8ScanChannel <= BDB_CHANNEL_MAX)
    {
        if(u32ScanChannels & (1<<u8ScanChannel))
        {
            BDB_vSetAssociationFilter();
            ZPS_vNwkNibClearDiscoveryNT(ZPS_pvAplZdoGetNwkHandle());
            eNS_State = E_NS_WAIT_DISCOVERY;
#ifdef ENABLE_SUBG_IF
            if(ZPS_E_SUCCESS == ZPS_eAplZdoDiscoverNetworks(SUBG_PAGE_28 | (u32ScanChannels & (1<<u8ScanChannel))))
#else
            if(ZPS_E_SUCCESS == ZPS_eAplZdoDiscoverNetworks(u32ScanChannels & (1<<u8ScanChannel)))
#endif
            {
                DBG_vPrintf(TRACE_BDB,"BDB: Disc on Ch %d from 0x%08x\n", u8ScanChannel, u32ScanChannels);
                /* Rest of the process starts after DicsoveryComplete event. */
                u8ScanChannel++;
                return;
            }
        }
        u8ScanChannel++;
   }

    vNsDiscoverNwk();
    return;
}

/****************************************************************************
 *
 * NAME: vNsTerminateNwkSteering
 *
 * DESCRIPTION:
 *  Terminate network steering if no networks join
 *
 * RETURNS:
 *  void
 *
 ****************************************************************************/
PRIVATE void vNsTerminateNwkSteering()
{
    BDB_tsBdbEvent sBdbEvent;

    sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_NO_NETWORK;
    eNS_State = E_NS_IDLE;
    sBdbEvent.eEventType = BDB_EVENT_NO_NETWORK;
    APP_vBdbCallback(&sBdbEvent);
}

/****************************************************************************
 *
 * NAME: vNsTryNwkJoin
 *
 * DESCRIPTION:
 * Attempts to join each of the discovered networks, if unsuccessful initiate
 * discovery on next channel
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vNsTryNwkJoin(bool_t bStartWithIndex0,
                             ZPS_tsAfNwkDiscoveryEvent *psNwkDiscoveryEvent)
{
    static uint8           u8NwkIndex = 0;
    static uint8           u8RecSameNwkRetryAttempts = 0;
    static uint8           u8NumOfNwks;
    ZPS_tsNwkNetworkDescr  *pNwkDescr;
    ZPS_teStatus           eStatus = ZPS_E_SUCCESS;

    if( bStartWithIndex0 )
    {
        u8NwkIndex = 0;
    }
    pNwkDescr = ZPS_psGetNetworkDescriptors( &u8NumOfNwks );

    DBG_vPrintf(TRACE_BDB,"BDB: vNsTryNwkJoin - try %d index %d of %d Nwks \n", u8RecSameNwkRetryAttempts, u8NwkIndex, u8NumOfNwks);
    while(u8NwkIndex < u8NumOfNwks)
    {
        if((pNwkDescr[u8NwkIndex].u8PermitJoining) && \
           ((0 == ZPS_psAplAibGetAib()->u64ApsUseExtendedPanid) ||\
            (pNwkDescr[u8NwkIndex].u64ExtPanId == ZPS_psAplAibGetAib()->u64ApsUseExtendedPanid)))
        {
            /* 8.3-6 The node SHALL attempt to join the network found using MAC association. */

            /* 8.3-7 ... it is permissible to try to
                     join the same network again, but this SHALL NOT be attempted more than
                     bdbcMaxSameNetworkRetryAttempts times in succession (bdbcRecSame-
                     NetworkRetryAttempts times in succession is RECOMMENDED)... */
            u8RecSameNwkRetryAttempts++;
            if( ( u8RecSameNwkRetryAttempts < BDBC_REC_SAME_NETWORK_RETRY_ATTEMPTS ) && \
               ( u8RecSameNwkRetryAttempts < BDBC_MAX_SAME_NETWORK_RETRY_ATTEMPTS ) )
            {
                DBG_vPrintf(TRACE_BDB, "BDB: Try To join %016llx on Ch %d\n",
                                       pNwkDescr[u8NwkIndex].u64ExtPanId,
                                       pNwkDescr[u8NwkIndex].u8LogicalChan);
                eNS_State = E_NS_WAIT_JOIN;
                eStatus = ZPS_eAplZdoJoinNetwork(&pNwkDescr[u8NwkIndex]);
                if(eStatus == ZPS_E_SUCCESS)
                {
                    /* Wait for join complete */
                    return;
                }
                else
                {
                    DBG_vPrintf(TRACE_BDB,"BDB: ZPS_eAplZdoJoinNetwork failure reason - %02x \n",eStatus);
                }
            }
            else
            {
                u8RecSameNwkRetryAttempts = 0;
                u8NwkIndex++;
            }
        }
        else /* u8PermitJoining disabled, try next nwk entry */
        {
            u8NwkIndex++;
        }
    }

#if ((SINGLE_CHANNEL < BDB_CHANNEL_MIN) || (SINGLE_CHANNEL > BDB_CHANNEL_MAX))
    if((!u32ScanChannels) || (u8ScanChannel > BDB_CHANNEL_MAX))
    {
        if(bDoPrimaryScan == FALSE)
#else
    if (TRUE)
    {
#endif
        {
           /* 8.3.16 ... set bdbCommissioningStatus to NO_NETWORK and then it SHALL terminate
              the network steering procedure for a node not on a network. */
            vNsTerminateNwkSteering();
            return;
        }
    }

    /* 8.3-5 If a suitable network is not found on the channel scan, the node SHALL
             continue from step 12. */
    DBG_vPrintf(TRACE_BDB,"BDB: No suitable network! Continue Discovery\n");
    vNsDiscoverNwk();
}

/****************************************************************************
 *
 * NAME: vNsAfterNwkJoin
 *
 * DESCRIPTION:
 *  Extend the network open network by broadcasting the Mgmt_Permit_Joining_req
 *  and generate the BDB_EVENT_NWK_STEERING_SUCCESS event.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vNsAfterNwkJoin(void)
{
    BDB_tsBdbEvent sBdbEvent;

    /* 8.3-13. The node SHALL broadcast the Mgmt_Permit_Joining_req ZDO command
               with the PermitDuration field set to at least bdbcMinCommissioningTime and
               the TC_Significance field set to 0x01.*/
    /* 8.3-14. NLME-PERMIT-JOINING.request*/
    vNsSendPermitJoin();

    /* 8.3-15 The node then sets bdbCommissioningStatus to SUCCESS. If the node
             supports touchlink, it sets the values of the aplFreeNwkAddrRangeBegin,
             aplFreeNwkAddrRangeEnd, aplFreeGroupID-RangeBegin and
             aplFreeGroupIDRangeEnd attributes all to 0x0000 (indicating the node
             having joined the network using MAC association). The node SHALL then
             terminate the network steering procedure for a node not on a network. */
    DBG_vPrintf(TRACE_BDB,"Nwk Join Success\n");
    //BDB_vStateUpdateEvent(E_STATE_BASE_ACTIVE, psStackEvent);
    sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_SUCCESS;
    if(sBDB.sAttrib.u8bdbNodeCommissioningCapability & (1<<BDB_NODE_COMMISSIONING_CAPABILITY_TOUCHLINK))
    {
        //place holder
    }

    /* Copy the network pan id into the aps use pan id after successful join */
    ZPS_eAplAibSetApsUseExtendedPanId(ZPS_u64NwkNibGetEpid( ZPS_pvAplZdoGetNwkHandle()));

    eNS_State = E_NS_IDLE;
    sBdbEvent.eEventType = BDB_EVENT_NWK_STEERING_SUCCESS;
    APP_vBdbCallback(&sBdbEvent);
}

/****************************************************************************
 *
 * NAME: vNsLeaveNwk
 *
 * DESCRIPTION:
 *  Send network leave.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vNsLeaveNwk()
{
    /* If the procedure is not successful, the node
    SHALL perform a leave request on its old network and resets its network
    parameters. The node then sets bdbNodeIsOnANetwork to FALSE and sets
    bdbCommissioningStatus to TCLK_EX_FAILURE. To perform a leave
    request, the node issues the NLME-LEAVE.request primitive to the NWK
    layer with the DeviceAddress parameter set to NULL, the RemoveChildren
    parameter set to FALSE and the Rejoin parameter set to FALSE. On receipt
    of the NLME-LEAVE.confirm primitive, the node is notified of the status of
    the request to leave the network. The node SHALL then terminate the */

    eNS_State = E_NS_TCLK_WAIT_LEAVE_CFM;
    ZPS_eAplZdoLeaveNetwork(0, FALSE,FALSE);

}

/****************************************************************************
 *
 * NAME: vNsSendPermitJoin
 *
 * DESCRIPTION:
 *  Broadcasts Mgmt_Permit_Joining_req and extend the opening of network.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vNsSendPermitJoin(void)
{
    PDUM_thAPduInstance hAPduInst;
    ZPS_tuAddress uDstAddr;
    ZPS_tsAplZdpMgmtPermitJoiningReq sZdpMgmtPermitJoiningReq;
    ZPS_teStatus           eStatus = ZPS_E_SUCCESS;
    hAPduInst = PDUM_hAPduAllocateAPduInstance(apduZDP);        // ToDo - Clarify apduZCL is defined in application scope; how to link the name in BDB (independent of application)

    if (hAPduInst != NULL)
    {
        uDstAddr.u16Addr = 0xFFFC;                              // Broadcast to all routing nodes
        sZdpMgmtPermitJoiningReq.u8PermitDuration = BDBC_MIN_COMMISSIONING_TIME;
        sZdpMgmtPermitJoiningReq.bTcSignificance = 0x01;

        eStatus = ZPS_eAplZdpMgmtPermitJoiningRequest(hAPduInst,
                                            uDstAddr,
                                            FALSE,              // TRUE - 64 bit MAC addr; FALSE - 16 bit nwk addr
                                            NULL,               // uint8 *pu8SeqNumber,
                                            &sZdpMgmtPermitJoiningReq);
        if( ZPS_APL_ZDP_E_INV_REQUESTTYPE  == eStatus ||
                    ZPS_APL_ZDP_E_INSUFFICIENT_SPACE== eStatus )
        {
            PDUM_eAPduFreeAPduInstance(hAPduInst);
        }
    }
    else
    {
        DBG_vPrintf(TRACE_BDB, "BDB: Couldn't get APdu handle!\n");
    }
}

/****************************************************************************
 *
 * NAME: vNsStartTclk
 *
 * DESCRIPTION:
 *  Start Trust Center Link Key exchange procedure
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vNsStartTclk(void)
{
    DBG_vPrintf(TRACE_BDB,"vNsStartTclk \n");

    sBDB.sAttrib.u8bdbTCLinkKeyExchangeAttempts = 0;
    vNsTclkSendNodeDescReq();
}

/****************************************************************************
 *
 * NAME: vNsTclkSendNodeDescReq
 *
 * DESCRIPTION:
 *  Send Node desc request as part of TCLK procedure
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vNsTclkSendNodeDescReq(void)
{
    ZPS_teStatus           eStatus = ZPS_E_SUCCESS;
    eNS_State = E_NS_TCLK_WAIT_NODE_DESC_RES;

    PDUM_thAPduInstance hAPduInst;

    hAPduInst = PDUM_hAPduAllocateAPduInstance(apduZDP);

    if (NULL != hAPduInst)
    {
        ZPS_tsAplZdpNodeDescReq sNodeDescReq;
        uint8 u8SeqNumber;
        ZPS_tuAddress uDstAddr;
        uDstAddr.u16Addr = ZPS_u16AplZdoLookupAddr(ZPS_psAplAibGetAib()->u64ApsTrustCenterAddress);
        /* always send to node of interest rather than a cache */


        sNodeDescReq.u16NwkAddrOfInterest = uDstAddr.u16Addr;

        eStatus = ZPS_eAplZdpNodeDescRequest(hAPduInst,
                                   uDstAddr,
                                   FALSE,       // bExtAddr
                                   &u8SeqNumber,
                                   &sNodeDescReq);
        if( ZPS_APL_ZDP_E_INV_REQUESTTYPE  == eStatus ||
                    ZPS_APL_ZDP_E_INSUFFICIENT_SPACE== eStatus )
        {
            PDUM_eAPduFreeAPduInstance(hAPduInst);
        }
        ZTIMER_eStart(u8TimerBdbNs, ZTIMER_TIME_MSEC(BDBC_TC_LINK_KEY_EXCHANGE_TIMEOUT*1000));
        sBDB.sAttrib.u8bdbTCLinkKeyExchangeAttempts++;
    #if (defined ZBPRO_DEVICE_TYPE_ZED)
        //vStartPolling();
        BDB_tsBdbEvent sBdbEvent;
        sBdbEvent.eEventType = BDB_EVENT_APP_START_POLLING;
        APP_vBdbCallback(&sBdbEvent);
    #endif
    }
    else
    {
        DBG_vPrintf(TRACE_BDB, "BDB: Couldn't get APdu handle!\n");
        vNsAfterNwkJoin();
    }
}

/****************************************************************************
 *
 * NAME: vNsTclkSendReqKey
 *
 * DESCRIPTION:
 *  Send APS request key and remaining TCLK procedure completion by stack
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vNsTclkSendReqKey(void)
{
    eNS_State = E_NS_TCLK_WAIT_SEND_REQ_KEY;
    ZPS_eAplZdoRequestKeyReq(4 /*ZPS_APL_REQ_KEY_TC_LINK_KEY*/, 0 /*This argument isn't used at the moment */);
    ZTIMER_eStart(u8TimerBdbNs, ZTIMER_TIME_MSEC(BDBC_TC_LINK_KEY_EXCHANGE_TIMEOUT*1000));
    sBDB.sAttrib.u8bdbTCLinkKeyExchangeAttempts++;
}

/****************************************************************************
 *
 * NAME: vNsTclkResendVerKey
 *
 * DESCRIPTION:
 *  Resend APS verify key initially sent by the stack
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vNsTclkResendVerKey(void)
{
    ZPS_eAfVerifyKeyReqRsp(ZPS_psAplAibGetAib()->u64ApsTrustCenterAddress, 4 /*ZPS_APL_REQ_KEY_TC_LINK_KEY*/);
    ZTIMER_eStart(u8TimerBdbNs, ZTIMER_TIME_MSEC(BDBC_TC_LINK_KEY_EXCHANGE_TIMEOUT*1000));
    sBDB.sAttrib.u8bdbTCLinkKeyExchangeAttempts++;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
