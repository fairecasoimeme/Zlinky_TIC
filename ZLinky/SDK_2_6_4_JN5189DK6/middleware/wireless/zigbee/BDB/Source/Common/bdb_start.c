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
# COMPONENT:   bdb_start.c
#
# DESCRIPTION: BDB initialisation and start up functions
#
#
###############################################################################*/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
#include "bdb_api.h"
#if (defined BDB_SUPPORT_TOUCHLINK)
#include "bdb_tl.h"
#endif
#if (defined BDB_SUPPORT_NWK_STEERING)
#include "bdb_ns.h"
#endif
#if ((defined BDB_SUPPORT_FIND_AND_BIND_INITIATOR) || (defined BDB_SUPPORT_FIND_AND_BIND_TARGET))
#include "bdb_fb_api.h"
#endif
#if (defined BDB_SUPPORT_NWK_FORMATION)
#include "bdb_nf.h"
#endif
#include "bdb_start.h"
#include "bdb_fr.h"
#include "dbg.h"
#include "rnd_pub.h"
#include <string.h>
#include <stdlib.h>
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define BEACON_FILTER_EXT_PAN_LIST_SIZE                 (1)
#define BEACON_FILTER_LQI_VALUE                         (30)
#ifndef BDBC_IMP_MAX_REJOIN_CYCLES
#define BDBC_IMP_MAX_REJOIN_CYCLES                      (3)
#endif
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef enum
{
    E_REJOIN_WITHOUT_DISC = 0,
    E_REJION_WITH_DISC_ON_PRIMARY_CH,
    E_REJOIN_WITH_DISC_ON_SECONDARY_CH,
    E_REJOIN_ATTEMPT_OVER
}teRejoinType;
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void vInitAttribs(void);
PRIVATE void bdb_vStart(bool_t bColdInit);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
PUBLIC teInitState eInitState = E_INIT_IDLE;
PUBLIC bool_t bAssociationJoin = FALSE;
PUBLIC uint8 u8RejoinCycles = 0;
PUBLIC uint8 u8TimerBdbRejoin;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
uint32 u32BackUpApsChannelMask;
tsBeaconFilterType  sBeaconFilter;
uint64 au64ExtPanListForBeaconFilter[BEACON_FILTER_EXT_PAN_LIST_SIZE];
bool_t (*prbCancelRejoinAction)(void)   = NULL;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: BDB_vInit
 *
 * DESCRIPTION:
 *  This API initializes Base Device by below means.
 *  1.  Initializes BDB  attributes to their default values only if those are
 *      not already defined in application (bdb_options.h file).
 *  2.  Registers Base Device message queue passed as argument to this function.
 *      This message queue gets utilized by Base device to capture stack events
 *      and break the context internally.
 *  3.  Sets the initial security keys based on device types -
 *      a.  For ZigBee Coordinator device type – Initial security state
 *          is set using “Default global Trust Center link key”
 *      b.  For ZigBee Router and End device types – Initial security state is
 *          set using both “Default global Trust Center link key” and
 *          “Distributed security global link key”
 *      Note – Above used link key arrays are defined inside
 *              BDB/Source/Common/bdb_link_keys.c file.
 *  4.  Opens the timers for Base Device internal use.
 *
 * PARAMETERS:      Name            RW  Usage
 *                  BDB_tsInitArgs      Handle to the Base Device Event Queue
 *
 * RETURNS:
 *  void
 *
 * EVENTS:
 *  none
 *
 * NOTES:
 *  1.  Before calling this API, one must initialize ZBPro stack (by calling 
 *      ZPS_eAplAfInit).
 *  2.  Before calling this API, one must restore BDB attribute bdbNodeIsOnANetwork
 *      from persistent storage. This attribute can be accessed by shared structure
 *      member sBDB.sAttrib.bbdbNodeIsOnANetwork.
 *  3.  Before calling this API, application must initialize the ZTimers using 
 *      ZTIMER_eInit and reserve extra BDB_ZTIMER_STORAGE size of list for 
 *      BDB's internal use.
 *
 ****************************************************************************/
PUBLIC void BDB_vInit(BDB_tsInitArgs *psInitArgs)
{
    ZPS_tsAplAib *psAib = ZPS_psAplAibGetAib();
    /* Set the BDB state to E_STATE_BASE_INIT */
    sBDB.eState = E_STATE_BASE_INIT;

    /* Register callback BDB Msg Queue */
    sBDB.hBdbEventsMsgQ = psInitArgs->hBdbEventsMsgQ;

    /* Initialize BDB attributes */
    vInitAttribs();
    
    /* Assign Link keys */
    sBDB.pu8DefaultTCLinkKey    = psAib->psAplDefaultTCAPSLinkKey->au8LinkKey;
    sBDB.pu8DistributedLinkKey  = ( *(ZPS_psAplDefaultDistributedAPSLinkKey()) == NULL )? NULL : (*(ZPS_psAplDefaultDistributedAPSLinkKey()))->au8LinkKey;
    sBDB.pu8PreConfgLinkKey     = ( *(ZPS_psAplDefaultGlobalAPSLinkKey()) == NULL )? NULL : (*(ZPS_psAplDefaultGlobalAPSLinkKey()))->au8LinkKey;
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    sBDB.pu8TouchLinkKey        = (uint8*)&sTLCertKey;
#else
    sBDB.pu8TouchLinkKey        = au8TLCertKey;
#endif

    #if (defined BDB_SUPPORT_TOUCHLINK)
        BDB_vTlInit();
    #endif

    /* Open ZTimers to be used in BDB */
    if(ZPS_ZDO_DEVICE_COORD != ZPS_eAplZdoGetDeviceType())
    {
        ZTIMER_eOpen(&u8TimerBdbRejoin,    BDB_vRejoinTimerCb, NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    }
    #if (defined BDB_SUPPORT_NWK_STEERING)
        ZTIMER_eOpen(&u8TimerBdbNs,        BDB_vNsTimerCb,     NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    #endif
    #if (defined BDB_SUPPORT_FIND_AND_BIND_INITIATOR)
        ZTIMER_eOpen(&u8TimerBdbFb,        BDB_vFbTimerCb,     NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    #endif
    #if (defined BDB_SUPPORT_TOUCHLINK)
        ZTIMER_eOpen(&u8TimerBdbTl,        BDB_vTlTimerCb,     NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    #endif

    eInitState = E_INIT_IDLE;
}

/****************************************************************************
 *
 * NAME: BDB_bIsBaseIdle
 *
 * DESCRIPTION:
 *  Using this API, application can figure out if Base device is idle or
 *  busy doing any of the commissioning or recovery tasks.
 *
 * PARAMETERS:      Name            RW  Usage
 *
 * RETURNS:
 *  TRUE  if Base Device is idle
 *  FALSE otherwise
 *
 * EVENTS:
 *  none
 *
 ****************************************************************************/
PUBLIC bool_t BDB_bIsBaseIdle(void)
{
    if(E_BDB_COMMISSIONING_STATUS_IN_PROGRESS == sBDB.sAttrib.ebdbCommissioningStatus)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


/****************************************************************************
 *
 * NAME: BDB_vStart|BDB_vRestart
 *
 * DESCRIPTION:
 *  Initialization procedure - BDB Specification 13-0402 Section 7.1
 *  1.  If node is not on the network, and the device type is router supporting
 *      Touchlink then the logical channel is set to one of those specified
 *      in primary channel set (bdbcTLPrimaryChannelSet). If RAND_CHANNEL macro
 *      is set to TRUE then channel selection is random otherwise the very first
 *      channel is selected from the primary channel set.
 *      Post this APP_vBdbCallback called with event BDB_EVENT_INIT_SUCCESS.
 *  2.  If node is already on the network then,
 *      a.  For ZigBee Coordinator or Router device, APP_vBdbCallback called with
 *          event BDB_EVENT_INIT_SUCCESS.
 *      b.  In normal start mode ZigBee End Device network rejoin attempts are performed for maximum
 *          of BDBC_IMP_MAX_REJOIN_CYCLES times. BDBC_IMP_MAX_REJOIN_CYCLES macro
 *          should be set in application (bdb_options.h file). Each rejoin attempt
 *          consists of below.
 *              a. Rejoin without discovery (direct rejoin)
 *              b. Rejoin with discovery on primary channel set
 *              c. Rejoin with discovery on secondary channel set
 *          If any of the above rejoin attempt was successful then
 *          BDB_EVENT_REJOIN_SUCCESS event is generated via APP_vBdbCallback.
 *
 *          After all the above rejoin attempts fail and if bApsUseInsecureJoin is 
 *          TRUE, then below is attempted.
 *              a. If ApsUseExtendedPanid is set to Non Zero (which will be the
 *                 ideal case); association join on the same network is attempted.
 *              b. If application has set ApsUseExtendedPanid to Zero before 
 *                 calling BDB_vStart(); association join is attempted on any 
 *                 available open network.
 *          BDB performs above association join by calling BDB_eNsStartNwkSteering().
 *
 *          If the network rejoin attempt fails, and the value of the
 *          apsUseInsecureJoin attribute of the AIB has a value of FALSE,
 *          then BDB_EVENT_REJOIN_FAILURE event is generated via APP_vBdbCallback.
 *
 * PARAMETERS:      Name            RW  Usage
 *
 * RETURNS:
 *  void
 *
 * EVENTS:
 *  BDB_EVENT_INIT_SUCCESS
 *  BDB_EVENT_REJOIN_SUCCESS
 *  BDB_EVENT_REJOIN_FAILURE
 *  In case of association join, events from BDB_eNsStartNwkSteering()
 *
 ****************************************************************************/
PUBLIC void BDB_vStart(void)
{
   bdb_vStart(TRUE);
}

PUBLIC void BDB_vRestart(void)
{
    /* Close ZTimers to be used in BDB */
    if(ZPS_ZDO_DEVICE_COORD != ZPS_eAplZdoGetDeviceType())
    {
        ZTIMER_eClose(u8TimerBdbRejoin);
    }
#if (defined BDB_SUPPORT_NWK_STEERING)
    ZTIMER_eClose(u8TimerBdbNs);
#endif
#if (defined BDB_SUPPORT_FIND_AND_BIND_INITIATOR)
    ZTIMER_eClose(u8TimerBdbFb);
#endif
#if (defined BDB_SUPPORT_TOUCHLINK)
    ZTIMER_eClose(u8TimerBdbTl);
#endif
    bdb_vStart(FALSE);
}


PRIVATE void bdb_vStart(bool_t bColdInit)
{
    BDB_tsBdbEvent sBdbEvent;

    /* Initialization procedure - BDB Specification 13-0402 Section 7.1 */
    if(sBDB.sAttrib.bbdbNodeIsOnANetwork)                               /* 7.1-2 */
    {
        sBDB.eState = E_STATE_BASE_ACTIVE;
        switch(ZPS_eAplZdoGetDeviceType())
        {
            case ZPS_ZDO_DEVICE_ENDDEVICE:
			{
                if(bColdInit)
				{
				    eInitState = E_INIT_WAIT_REJOIN;
                    sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_IN_PROGRESS;
                    BDB_vRejoinCycle(FALSE);
				}
                else
				{
				    eInitState = E_INIT_IDLE;
                    sBdbEvent.eEventType = BDB_EVENT_INIT_SUCCESS;
                    APP_vBdbCallback(&sBdbEvent);
                    ZPS_eAplZdoStartStack();
				}
			}
				break;
            case ZPS_ZDO_DEVICE_COORD:
                #if (defined BDB_SUPPORT_NWK_FORMATION)
                    BDB_vNfFormCentralizedNwk();
                #else
                    DBG_vPrintf(TRACE_BDB,"BDB: Error BDB_SUPPORT_NWK_FORMATION undefined !");
                #endif
                eInitState = E_INIT_IDLE;
                sBdbEvent.eEventType = BDB_EVENT_INIT_SUCCESS;
                APP_vBdbCallback(&sBdbEvent);

                break;
            case ZPS_ZDO_DEVICE_ROUTER:
                ZPS_eAplZdoStartRouter(FALSE); // Don't send Device Announce
                eInitState = E_INIT_IDLE;
                sBdbEvent.eEventType = BDB_EVENT_INIT_SUCCESS;
                APP_vBdbCallback(&sBdbEvent);
                break;
            default:
                DBG_vPrintf(TRACE_BDB,"Impossible Device Type!");
                break;
        }
    }
    else
    {
        if(ZPS_ZDO_DEVICE_ROUTER == ZPS_eAplZdoGetDeviceType())         /* 7.1-6 */
        {
            if(BDB_NODE_COMMISSIONING_CAPABILITY_TOUCHLINK && \
                    sBDB.sAttrib.u8bdbNodeCommissioningCapability)      /* 7.1-7 */
            {
                /* the node SHALL set its logical channel to one of those
                 * specified in bdbcTLPrimaryChannelSet */
                ZPS_vNwkNibSetChannel(ZPS_pvAplZdoGetNwkHandle(),
                                      BDB_u8PickChannel(BDBC_TL_PRIMARY_CHANNEL_SET));
            }
        }

        sBDB.eState = E_STATE_BASE_SLAVE;

        eInitState = E_INIT_IDLE;
        sBdbEvent.eEventType = BDB_EVENT_INIT_SUCCESS;
        APP_vBdbCallback(&sBdbEvent);
    }
}

/****************************************************************************
 *
 * NAME: BDB_vInitStateMachine
 *
 * DESCRIPTION:
 *  BDB init state machine; this is utilized internally by BDB.
 *
 * PARAMETERS:      Name            RW  Usage
 *                  BDB_tsZpsAfEvent    Zps event
 * RETURNS:
 *  void
 *
 ****************************************************************************/
PUBLIC void BDB_vInitStateMachine(BDB_tsZpsAfEvent *psZpsAfEvent)
{
    switch(eInitState)
    {
        case E_INIT_IDLE:
            break;

        case E_INIT_WAIT_REJOIN:
            switch(psZpsAfEvent->sStackEvent.eType)
            {
                case ZPS_EVENT_NWK_JOINED_AS_ENDDEVICE:
                case ZPS_EVENT_NWK_JOINED_AS_ROUTER:
                    BDB_vRejoinSuccess();
                    break;
                case ZPS_EVENT_NWK_FAILED_TO_JOIN:
                    BDB_vRejoinCycle(TRUE);
                    break;
                default:
                    break;
            }
            break;
    }
}

/****************************************************************************
 *
 * NAME: BDB_u8PickChannel
 *
 * DESCRIPTION:
 *  Selects a channel from the mask. If RAND_CHANNEL macro is set to TRUE, then
 *  the selection is random the first channel is returned.
 *
 * PARAMETERS:      Name            RW  Usage
 *                  u32ChannelMask      Channel Mask
 * RETURNS:
 *  u8Channel
 *
 ****************************************************************************/
PUBLIC uint8 BDB_u8PickChannel(uint32 u32ChannelMask)
{
    uint8 au8ChanAr[BDB_CHANNEL_MAX-BDB_CHANNEL_MIN+1]={0}, u8ChanArSize=0, u8i=0;

    /* Fill the u8Ar with channel number from u32ChannelMask */
    for(u8i=BDB_CHANNEL_MIN; u8i<=BDB_CHANNEL_MAX; u8i++)
    {
        if(u32ChannelMask & (1<<u8i))
        {
            au8ChanAr[u8ChanArSize++] = u8i;
        }
    }
#if RAND_CHANNEL
    return au8ChanAr[(uint8)RND_u32GetRand(0, u8ChanArSize-1)];
#else
    /* TestEvent - Return first channel from u32bdbPrimaryChannelSet */
    for(u8i=BDB_CHANNEL_MIN;u8i<BDB_CHANNEL_MAX;u8i++)
    {
        if(sBDB.sAttrib.u32bdbPrimaryChannelSet & (1<<u8i))
        {
            return u8i;
        }
    }
    /* Return first channel from BDBC_TL_PRIMARY_CHANNEL_SET */
    return  au8ChanAr[0];
#endif
}

/****************************************************************************
 *
 * NAME: BDB_vRejoinCycle
 *
 * DESCRIPTION: BDB Spec Section 7.1-4
 *  Rejoin attempts are performed for maximum of BDBC_IMP_MAX_REJOIN_CYCLES
 *  times. BDBC_IMP_MAX_REJOIN_CYCLES macro should be set in application
 *  (bdb_options.h file). Each rejoin attempt consists of below.
 *      a. Rejoin without discovery (direct rejoin)
 *      b. Rejoin with discovery on primary channel set
 *      c. Rejoin with discovery on secondary channel set
 *  If any of the above rejoin attempt was successful then
 *  BDB_EVENT_REJOIN_SUCCESS event is generated via APP_vBdbCallback.
 *
 *  After all the above rejoin attempts fail and if bApsUseInsecureJoin is 
 *  TRUE, then below is attempted.
 *      a. If ApsUseExtendedPanid is set to Non Zero (which will be the
 *         ideal case); association join on the same network is attempted.
 *      b. If application has set ApsUseExtendedPanid to Zero before 
 *         calling BDB_vStart(); association join is attempted on any 
 *         available open network.
 *  BDB performs above association join by calling BDB_eNsStartNwkSteering().
 *
 *  If the network rejoin attempt fails, and the value of the
 *  apsUseInsecureJoin attribute of the AIB has a value of FALSE,
 *  then BDB_EVENT_REJOIN_FAILURE event is generated via APP_vBdbCallback.
 *
 * PARAMETERS:      Name            RW  Usage
 *
 * RETURNS:
 *  void
 * 
 * EVENT:
 *  BDB_EVENT_REJOIN_SUCCESS
 *  BDB_EVENT_REJOIN_FAILURE
 *  In case of association join, events from BDB_eNsStartNwkSteering()
 *
 * NOTES:
 *
 ****************************************************************************/
PUBLIC void BDB_vRejoinCycle(bool_t bSkipDirectJoin)
{
    static teRejoinType eRejoinType = E_REJOIN_WITHOUT_DISC;
    BDB_tsBdbEvent sBdbEvent;
    ZPS_teStatus eStatus;

    if(prbCancelRejoinAction != NULL)
    {
        if ( prbCancelRejoinAction() )
        {
            /* Abort rejoin cycle as Tl is started */
            u8RejoinCycles = 0xFF;
            eInitState = E_INIT_IDLE;
            eFR_State = E_FR_IDLE;
            return;
        }
    }



    if(u8RejoinCycles > BDBC_IMP_MAX_REJOIN_CYCLES)
    {
        /* If previous rejoin cycles were all failures then start fresh */
        u8RejoinCycles = 1;
        eRejoinType = E_REJION_WITH_DISC_ON_PRIMARY_CH;
    }
    else if(u8RejoinCycles == 0)
    {
        /* Avoid "rejoin without discovery" in the first cycle when bSkipDirectJoin is set */

        u8RejoinCycles++;

        if(bSkipDirectJoin)
        {
            eRejoinType = E_REJION_WITH_DISC_ON_PRIMARY_CH;
        }
        else
        {
            eRejoinType = E_REJOIN_WITHOUT_DISC;
        }
    }

    /* Avoid "rejoin without discovery" in subsequent cycles */
    if(eRejoinType >= E_REJOIN_ATTEMPT_OVER)
    {
        u8RejoinCycles++;
        eRejoinType = E_REJION_WITH_DISC_ON_PRIMARY_CH;
    }

    if(u8RejoinCycles < BDBC_IMP_MAX_REJOIN_CYCLES)
    {
        for(;eRejoinType < E_REJOIN_ATTEMPT_OVER; eRejoinType++)
        {
            DBG_vPrintf(TRACE_BDB,"BDB: Rejoin Cycle %d-", u8RejoinCycles);
            switch(eRejoinType)
            {
                case E_REJOIN_WITHOUT_DISC:
                    /* Backup ApsChannelMask and restore after NwkForm confirmation */
                    u32BackUpApsChannelMask = ZPS_psAplAibGetAib()->pau32ApsChannelMask[0];
                    ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle())->sPersist.u64ExtPanId = ZPS_psAplAibGetAib()->u64ApsUseExtendedPanid;
                    DBG_vPrintf(TRACE_BDB,"A without Disc           ");
                    break;
                case E_REJION_WITH_DISC_ON_PRIMARY_CH:
                    ZPS_eAplAibSetApsChannelMask(sBDB.sAttrib.u32bdbPrimaryChannelSet);
                    DBG_vPrintf(TRACE_BDB,"B with Disc on Primary   ");
                    break;
                case E_REJOIN_WITH_DISC_ON_SECONDARY_CH:
                    ZPS_eAplAibSetApsChannelMask(sBDB.sAttrib.u32bdbSecondaryChannelSet);
                    DBG_vPrintf(TRACE_BDB,"C with Disc on Secondary ");
                    break;
                default:
                    break;
            }
            DBG_vPrintf(TRACE_BDB,"- on Ch Mask 0x%08x\n",ZPS_psAplAibGetAib()->pau32ApsChannelMask[0]);


            if(ZPS_psAplAibGetAib()->pau32ApsChannelMask[0])
            {
                BDB_vSetRejoinFilter();
                eStatus = ZPS_eAplZdoRejoinNetwork(eRejoinType);

                if(eStatus == ZPS_E_SUCCESS)
                {
                    eRejoinType++;
                    return;
                }
                else
                {
                    DBG_vPrintf(TRACE_BDB,"BDB: ZPS_eAplZdoRejoinNetwork returned %02x \n",eStatus);
                }
            }
        }
    }

    /* Restore the backup channel mask */
    ZPS_eAplAibSetApsChannelMask(u32BackUpApsChannelMask);

    /* Before doing the next rejoin attempt; let's break the context. */
    if(u8RejoinCycles < BDBC_IMP_MAX_REJOIN_CYCLES)
    {
        ZTIMER_eStart(u8TimerBdbRejoin, ZTIMER_TIME_MSEC(1));
        return;
    }

    u8RejoinCycles = 0xFF;

    eInitState = E_INIT_IDLE;
    eFR_State = E_FR_IDLE;

    /* r21-If the network rejoin attempt fails, and the value of the 
     * apsUseInsecureJoin attribute of the AIB has a value of TRUE, then the
     * device should follow the procedure outlined in section 3.6.1.4.1 for
     * joining a network ... */
    if(TRUE == ZPS_psAplAibGetAib()->bApsUseInsecureJoin)
    {
        /* If u64ApsUseExtendedPanid is set to 0 by application then clearing the
         * Trust Center link key link key to be successful during TCLK procedure
         * if accidental same network reappears */
        if(ZPS_psAplAibGetAib()->u64ApsUseExtendedPanid == 0)
        {
            ZPS_teStatus eStatus;
            DBG_vPrintf(TRACE_BDB,"BDB: Rejoin attempts failed; attempting Join with association \n");
            eStatus = ZPS_eAplZdoRemoveLinkKey(ZPS_psAplAibGetAib()->u64ApsTrustCenterAddress);
            if(eStatus != ZPS_E_SUCCESS)
            {
                DBG_vPrintf(TRACE_BDB,"BDB: Could not remove the trust center link key !\n");
            }
        }

        bAssociationJoin = TRUE;

        sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_NO_NETWORK;
        BDB_eNsStartNwkSteering();
    }
    else
    {
        sBDB.eState = E_STATE_BASE_FAIL;

        sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_NO_NETWORK;
        sBdbEvent.eEventType = BDB_EVENT_REJOIN_FAILURE;
        APP_vBdbCallback(&sBdbEvent);
    }
}

/****************************************************************************
 *
 * NAME: BDB_vRejoinSuccess
 *
 * DESCRIPTION:
 *  Generates rejoin success events and adjusts the state machines.
 *
 * PARAMETERS:      Name            RW  Usage
 *
 * RETURNS:
 * void
 *
 * EVENTS:
 *  BDB_EVENT_REJOIN_SUCCESS
 *
 * NOTES:
 *
 ****************************************************************************/
PUBLIC void BDB_vRejoinSuccess()
{
    BDB_tsBdbEvent sBdbEvent;

    sBDB.eState = E_STATE_BASE_ACTIVE;

    eInitState = E_INIT_IDLE;
    eFR_State = E_FR_IDLE;

    u8RejoinCycles = 0;
    /* Restore the backup channel mask */
    ZPS_eAplAibSetApsChannelMask(u32BackUpApsChannelMask);

    /* Store EPID for future rejoin */
    ZPS_eAplAibSetApsUseExtendedPanId(ZPS_u64NwkNibGetEpid( ZPS_pvAplZdoGetNwkHandle()));

    sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_SUCCESS;
    sBdbEvent.eEventType = BDB_EVENT_REJOIN_SUCCESS;
    APP_vBdbCallback(&sBdbEvent);
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: vInitAttribs
 *
 * DESCRIPTION:
 * Initialize the BDB attributes to their default values as per spec.
 * If attribute macros are defined already in application (e.g. via bdb_options.h
 * file) then those attributes are not initialized.
 *
 * These attributes can be updated by application via global shared structure
 * sBDB.sAttrib
 *
 * PARAMETERS:      Name            RW  Usage
 *
 *
 * RETURNS:
 * void
 *
 * NOTES:
 * 1.   u8bdbNodeCommissioningCapability is constant and set based on defines in Makefile
 *       BDB_SUPPORT_NWK_STEERING
 *       BDB_SUPPORT_NWK_FORMATION
 *       BDB_SUPPORT_TOUCHLINK
 *       BDB_SUPPORT_FIND_AND_BIND_INITIATOR
 *       BDB_SUPPORT_FIND_AND_BIND_TARGET
 * 2.   It's application's responsibility to set bbdbNodeIsOnANetwork from
 *      persistent storage before calling any BDB APIs.
 *
 ****************************************************************************/
PRIVATE void vInitAttribs()
{
    uint8 u8bdbNodeCommissioningCapability = 0;

    #ifdef BDB_COMMISSIONING_GROUP_ID
        sBDB.sAttrib.u16bdbCommissioningGroupID = BDB_COMMISSIONING_GROUP_ID;
    #else
        sBDB.sAttrib.u16bdbCommissioningGroupID = 0xFFFF; /* Default Value */
    #endif

    #ifdef BDB_COMMISSIONING_MODE
        sBDB.sAttrib.u8bdbCommissioningMode = BDB_COMMISSIONING_MODE;
    #else
        sBDB.sAttrib.u8bdbCommissioningMode = 0x00; /* Default Value */
    #endif

    #ifdef BDB_COMMISSIONING_STATUS
        sBDB.sAttrib.ebdbCommissioningStatus = BDB_COMMISSIONING_STATUS;
    #else
        sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_SUCCESS; /* Default Value */
    #endif

    #ifdef BDB_JOINING_NODE_EUI64
        sBDB.sAttrib.u64bdbJoiningNodeEui64 = BDB_JOINING_NODE_EUI64;
    #else
        sBDB.sAttrib.u64bdbJoiningNodeEui64 = 0x0000000000000000; /* Default Value */
    #endif

    memset(&sBDB.sAttrib.au8bdbJoiningNodeNewTCLinkKey[0],0,16); /* Default Value */

    #ifdef BDB_JOIN_USES_INSTALL_CODE_KEY
	{
	    ZPS_tsAplAib *psAib = ZPS_psAplAibGetAib();
        sBDB.sAttrib.bbdbJoinUsesInstallCodeKey = psAib->bUseInstallCode = (BDB_JOIN_USES_INSTALL_CODE_KEY) ?   TRUE  :  FALSE;
	}
    #else
        sBDB.sAttrib.bbdbJoinUsesInstallCodeKey = FALSE; /* Default Value */
    #endif

    /* sBDB.sAttrib.u8bdbNodeCommissioningCapability based on defined in makefile */
    #if (defined BDB_SUPPORT_NWK_STEERING)
        u8bdbNodeCommissioningCapability |= 1<<BDB_NODE_COMMISSIONING_CAPABILITY_NETWORK_STEERING;
    #endif
    #if (defined BDB_SUPPORT_NWK_FORMATION)
        u8bdbNodeCommissioningCapability |= 1<<BDB_NODE_COMMISSIONING_CAPABILITY_NETWORK_FORMATION;
    #endif
    #if (defined BDB_SUPPORT_TOUCHLINK)
        u8bdbNodeCommissioningCapability |= 1<<BDB_NODE_COMMISSIONING_CAPABILITY_TOUCHLINK;
    #endif
    #if (defined BDB_SUPPORT_FIND_AND_BIND_INITIATOR || defined BDB_SUPPORT_FIND_AND_BIND_TARGET)
        u8bdbNodeCommissioningCapability |= 1<<BDB_NODE_COMMISSIONING_CAPABILITY_FINDING_N_BINDING;
    #endif
    *(uint8 *)&sBDB.sAttrib.u8bdbNodeCommissioningCapability = u8bdbNodeCommissioningCapability;

    #ifdef BDB_NODE_JOIN_LINK_KEY_TYPE
        sBDB.sAttrib.u8bdbNodeJoinLinkKeyType = BDB_NODE_JOIN_LINK_KEY_TYPE;
    #else
        sBDB.sAttrib.u8bdbNodeJoinLinkKeyType = DEFAULT_GLOBAL_TRUST_CENTER_LINK_KEY; /* Default Value */
    #endif

    #ifdef BDB_PRIMARY_CHANNEL_SET
        sBDB.sAttrib.u32bdbPrimaryChannelSet = BDB_PRIMARY_CHANNEL_SET;
    #else
        sBDB.sAttrib.u32bdbPrimaryChannelSet = 0x02108800; /* Default Value */
    #endif

    #ifdef BDB_SCAN_DURATION
        sBDB.sAttrib.u8bdbScanDuration = BDB_SCAN_DURATION;
    #else
          /* Value from app.zpscfg->Device->Misc->”Scan Duration Time” takes priority */
        sBDB.sAttrib.u8bdbScanDuration = 0x04; /* Default Value */
    #endif

    #ifdef BDB_SECONDARY_CHANNEL_SET
        sBDB.sAttrib.u32bdbSecondaryChannelSet = BDB_SECONDARY_CHANNEL_SET;
    #else
        sBDB.sAttrib.u32bdbSecondaryChannelSet = 0x07FFF800 ^ sBDB.sAttrib.u32bdbPrimaryChannelSet; /* Default Value */
    #endif

    #ifdef BDB_TC_LINK_KEY_EXCHANGE_ATTEMPTS
        sBDB.sAttrib.u8bdbTCLinkKeyExchangeAttempts = BDB_TC_LINK_KEY_EXCHANGE_ATTEMPTS;
    #else
        sBDB.sAttrib.u8bdbTCLinkKeyExchangeAttempts = 0x00; /* Default Value */
    #endif

    #ifdef BDB_TC_LINK_KEY_EXCHANGE_ATTEMPTS_MAX
        sBDB.sAttrib.u8bdbTCLinkKeyExchangeAttemptsMax = BDB_TC_LINK_KEY_EXCHANGE_ATTEMPTS_MAX;
    #else
        sBDB.sAttrib.u8bdbTCLinkKeyExchangeAttemptsMax = 0x03; /* Default Value */
    #endif

    #ifdef BDB_TC_LINK_KEY_EXCHANGE_METHOD
        sBDB.sAttrib.u8bdbTCLinkKeyExchangeMethod = BDB_TC_LINK_KEY_EXCHANGE_METHOD;
    #else
        sBDB.sAttrib.u8bdbTCLinkKeyExchangeMethod = 0x00; /* Default Value */
    #endif

    #ifdef BDB_TRUST_CENTER_NODE_JOIN_TIMEOUT
        sBDB.sAttrib.u8bdbTrustCenterNodeJoinTimeout = BDB_TRUST_CENTER_NODE_JOIN_TIMEOUT;
    #else
        /* Value from app.zpscfg->Device->""APS Security Time out period" takes priority */
        sBDB.sAttrib.u8bdbTrustCenterNodeJoinTimeout = 0x0F; /* Default Value */  
    #endif

    #ifdef BDB_TRUST_CENTER_REQUIRE_KEYEXCHANGE
        sBDB.sAttrib.bbdbTrustCenterRequireKeyExchange = BDB_TRUST_CENTER_REQUIRE_KEYEXCHANGE;
    #else
        sBDB.sAttrib.bbdbTrustCenterRequireKeyExchange = TRUE; /* Default Value */
    #endif
}

/****************************************************************************
 *
 * NAME: BDB_vRejoinTimerCb
 *
 * DESCRIPTION: To break the context inside BDB_vRejoinCycle
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void BDB_vRejoinTimerCb(void *pvParam)
{
    BDB_vRejoinCycle(TRUE);
}


/****************************************************************************
 *
 * NAME: BDB_vSetRejoinFilter
 *
 * DESCRIPTION:
 *  Sets Rejoin filters
 *      a. sets up a whitelist containing a single EPID corresponding to the
 *         network that the node is to rejoin.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void BDB_vSetRejoinFilter()
{
    uint64 u64Epid;

    sBeaconFilter.u16FilterMap = BF_BITMAP_LQI;
    sBeaconFilter.u8ListSize = 0;
    sBeaconFilter.pu64ExtendPanIdList = NULL;

    u64Epid = ZPS_psAplAibGetAib()->u64ApsUseExtendedPanid;
    if ((u64Epid > 0) &&
        (u64Epid < 0xffffffffffffffffUL))
    {
        /* Apply a beacon filter */
        au64ExtPanListForBeaconFilter[0] = u64Epid;
        sBeaconFilter.pu64ExtendPanIdList = &au64ExtPanListForBeaconFilter[0];
        sBeaconFilter.u8ListSize = BEACON_FILTER_EXT_PAN_LIST_SIZE;
        sBeaconFilter.u16FilterMap |= BF_BITMAP_WHITELIST;
    }
}

/****************************************************************************
 *
 * NAME: vSetAssociationFilter
 *
 * DESCRIPTION:
 *  Sets Association filters
 *      a. Sets up an LQI filter to reject distant beacons
 *      b. filter on the Permit Join status to only find potential parents and
 *         networks that are accepting association requests
 *      d. filter on Router/End Device Capacity
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void BDB_vSetAssociationFilter()
{
    sBeaconFilter.pu64ExtendPanIdList = NULL;
    sBeaconFilter.u8ListSize = 0;
    sBeaconFilter.u8Lqi = BEACON_FILTER_LQI_VALUE;

    switch(ZPS_eAplZdoGetDeviceType())
    {
        case ZPS_ZDO_DEVICE_ENDDEVICE:
            sBeaconFilter.u16FilterMap = BF_BITMAP_CAP_ENDDEVICE | \
                                        BF_BITMAP_PERMIT_JOIN |\
                                        BF_BITMAP_LQI;
            break;
        case ZPS_ZDO_DEVICE_ROUTER:
            sBeaconFilter.u16FilterMap = BF_BITMAP_CAP_ROUTER | \
                                        BF_BITMAP_PERMIT_JOIN |\
                                        BF_BITMAP_LQI;
            break;
        default:
            //impossible!
            break;
    }

    ZPS_bAppAddBeaconFilter(&sBeaconFilter);
#ifdef ENABLE_SUBG_IF
	void *pvNwk = ZPS_pvAplZdoGetNwkHandle();
	ZPS_vNwkNibSetMacEnhancedMode(pvNwk, TRUE);
#endif
}

/****************************************************************************
 *
 * NAME:       ZPS_vAplZdoRegisterZdoLeaveActionCallback
 */
/**
 * @ingroup
 *
 * @param
 *
 * @return
 *
 * @note
 *
 ****************************************************************************/
PUBLIC void BDB_vRegisterRejoinControl(void* fnPtr)
{
    prbCancelRejoinAction = (bool_t (*)(void))fnPtr;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
