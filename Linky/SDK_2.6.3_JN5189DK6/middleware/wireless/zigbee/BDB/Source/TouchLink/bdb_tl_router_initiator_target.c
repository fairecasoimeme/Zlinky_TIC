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
# COMPONENT:   bdb_tl_router_initiator_target.c
#
# DESCRIPTION: BDB Touchlink implementation of Router as Initiator/Target
#              
#
###############################################################################*/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include "bdb_api.h"
#include <AppApi.h>
#include "pdum_apl.h"
#include "pdum_gen.h"
#include "PDM.h"
#include "dbg.h"
//#include "pwrm.h"
#include "zps_apl_af.h"
#include "zps_apl_zdo.h"
#include "zps_apl_aib.h"
#include "zps_apl_zdp.h"
#include "mac_sap.h"
#include "zll_commission.h"
#include <rnd_pub.h>
#include "ZTimer.h"
#include "bdb_tl.h"
#include "app_events.h"
#include "app_common.h"
#include "Log.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef DEBUG_SCAN
#define TRACE_SCAN            TRUE
#else
#define TRACE_SCAN            TRUE
#endif

#ifndef DEBUG_JOIN
#define TRACE_JOIN            TRUE
#else
#define TRACE_JOIN            TRUE
#endif

#ifndef DEBUG_COMMISSION
#define TRACE_COMMISSION      TRUE
#else
#define TRACE_COMMISSION      TRUE
#endif

#ifndef DEBUG_SCAN_REQ
#define TRACE_SCAN_REQ        FALSE
#else
#define TRACE_SCAN_REQ        TRUE
#endif

#ifndef DEBUG_TL_NEGATIVE
#define TRACE_TL_NEGATIVE     FALSE
#else
#define TRACE_TL_NEGATIVE     TRUE
#endif



#define USE_LOG






#define LED1  (1 << 1)
#define LED2  (1)




#define TL_SCAN_LQI_MIN    (110)


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef enum {
    E_IDLE,
    E_INFORM_APP,
    E_SCANNING,
    E_SCAN_DONE,
    E_SCAN_WAIT_ID,
    E_SCAN_WAIT_INFO,
    E_WAIT_START_RSP,
    E_WAIT_JOIN_RTR_RSP,
    E_WAIT_JOIN_ZED_RSP,
    E_WAIT_DISCOVERY,
    E_SCAN_WAIT_RESET_SENT,
    E_SKIP_DISCOVERY,
    E_START_ROUTER,
    E_WAIT_LEAVE,
    E_WAIT_LEAVE_RESET,
    E_WAIT_START_UP,
    E_WAIT_NWK_START,
    E_ACTIVE
}eState;

typedef struct {
    eState eState;
    uint8 u8Count;
    uint8 u8Flags;
    bool_t bSecondScan;
    bool_t bResponded;
    uint32 u32TransactionId;
    uint32 u32ResponseId;
    uint32 u32TheirTransactionId;
    uint32 u32TheirResponseId;
    bool_t bDoPrimaryScan;
    bool_t bIsFirstChannel;
    uint32 u32ScanChannels;
    bool_t bProfileInterOp;
} tsCommissionData;

typedef struct {
    ZPS_tsInterPanAddress       sSrcAddr;
    tsCLD_ZllCommission_ScanRspCommandPayload                   sScanRspPayload;
}tsZllScanData;

typedef struct {
    tsZllScanData sScanDetails;
    uint16 u16LQI;
}tsZllScanTarget;


/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void vTlEndCommissioning( void* pvNwk, eState eState, uint16 u16TimeMS);
PRIVATE void vTlSendScanResponse(ZPS_tsNwkNib *psNib,
                               ZPS_tsInterPanAddress       *psDstAddr,
                               uint32 u32TransactionId,
                               uint32 u32ResponseId);
PRIVATE void vTlSendScanRequest(void *pvNwk, tsCommissionData *psCommission);
PRIVATE void vTlHandleScanResponse(void *pvNwk,
                                 ZPS_tsNwkNib *psNib,
                                 tsZllScanTarget *psScanTarget,
                                 tsCLD_ZllCommission_ScanRspCommandPayload                   *psScanRsp,
                                 ZPS_tsInterPanAddress       *psSrcAddr,
                                 uint8 u8Lqi);
PRIVATE void vTlHandleDeviceInfoRequest( tsCLD_ZllCommission_DeviceInfoReqCommandPayload *psDeviceInfoReq,
                                              ZPS_tsInterPanAddress       *psSrcAddr);
PRIVATE void vTlSendIdentifyRequest(uint64 u64Addr, uint32 u32TransactionId, uint8 u8Time);

PRIVATE void vTlSendDeviceInfoReq(uint64 u64Addr, uint32 u32TransactionId, uint8 u8Index);
#if 0
PRIVATE void vTlSendStartRequest( void *pvNwk,
                                ZPS_tsNwkNib *psNib,
                                tsZllScanTarget *psScanTarget,
                                uint32 u32TransactionId);
#endif
PRIVATE void vTlSendFactoryResetRequest( void *pvNwk,
                                       ZPS_tsNwkNib *psNib,
                                       tsZllScanTarget *psScanTarget,
                                       uint32 u32TransactionId);
PRIVATE void vTlSendRouterJoinRequest( void * pvNwk,
                                     ZPS_tsNwkNib *psNib,
                                     tsZllScanTarget *psScanTarget,
                                     uint32 u32TransactionId);
PRIVATE void vTlSendEndDeviceJoinRequest( void *pvNwk,
                                        ZPS_tsNwkNib *psNib,
                                        tsZllScanTarget *psScanTarget,
                                        uint32 u32TransactionId);

PRIVATE void vTlHandleNwkStartResponse( void * pvNwk,
                                             tsCLD_ZllCommission_NetworkStartRspCommandPayload *psNwkStartRsp,
                                             tsCommissionData *psCommission,
                                             ZPS_tsInterPanAddress       *psSrcAddr);
PRIVATE void vTlHandleNwkStartRequest( ZPS_tsInterPanAddress  *psDstAddr,
                                            tsCLD_ZllCommission_NetworkStartReqCommandPayload          *psNwkStartReq,
                                            tsCommissionData *psCommission,
                                            ZPS_tsInterPanAddress       *psSrcAddr);
PRIVATE void vTlHandleRouterJoinRequest( void * pvNwk,
                                       ZPS_tsNwkNib *psNib,
                                       tsCLD_ZllCommission_NetworkJoinRouterReqCommandPayload   *psRouterJoinReq,
                                       tsCommissionData *psCommission,
                                       ZPS_tsInterPanAddress       *psSrcAddr);
PRIVATE void vTlHandleNwkUpdateRequest(void * pvNwk,
                                     ZPS_tsNwkNib *psNib,
                                     tsCLD_ZllCommission_NetworkUpdateReqCommandPayload *psNwkUpdateReqPayload);



PUBLIC void BDB_vTlSetGroupAddress(uint16 u16GroupStart, uint8 u8NumGroups);
PRIVATE bool_t bSearchDiscNt(ZPS_tsNwkNib *psNib, uint64 u64EpId, uint16 u16PanId);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern tsZllEndpointInfoTable sEndpointTable;
extern tsZllGroupInfoTable sGroupTable;

extern bool_t bResetIssued;
extern tsCLD_ZllDeviceTable sDeviceTable;
//extern tsDeviceDesc           sDeviceDesc;
extern void ZPS_vTCSetCallback(void*);

extern tsZllState sZllState;

PUBLIC bool_t bSendFactoryResetOverAir=FALSE;
PUBLIC uint8 u8TimerBdbTl;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsCommissionData sCommission;
APP_tsEventTouchLink        sTarget;
ZPS_tsInterPanAddress       sDstAddr;
tsZllScanTarget sScanTarget;
PRIVATE uint8 au8TempKeyStore[16];
static tsStartParams sStartParams;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: BDB_vTlTimerCb
 *
 * DESCRIPTION: handles commissioning timer expiry events
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void BDB_vTlTimerCb(void *pvParam)
{
    tsBDB_ZCLEvent sEvent;
    sEvent.eType = BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED;
    BDB_vTlStateMachine( &sEvent);
}

/****************************************************************************
 *
 * NAME: BDB_vTlInit
 *
 * DESCRIPTION:
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void BDB_vTlInit(void)
{
    sCommission.eState = E_IDLE;
}

/****************************************************************************
 *
 * NAME:
 *
 * DESCRIPTION:
 *
 * RETURNS: void
 * void
 *
 ****************************************************************************/
PUBLIC bool_t BDB_bTlTouchLinkInProgress( void)
{
    return (sCommission.eState > E_INFORM_APP) ? TRUE: FALSE;
}

/****************************************************************************
 *
 * NAME:
 *
 * DESCRIPTION:
 *
 * RETURNS: void
 * void
 *
 ****************************************************************************/
PUBLIC bool_t bIsTlStarted(void)
{
    return (sCommission.eState > E_INFORM_APP) ? TRUE: FALSE;
}

/****************************************************************************
 *
 * NAME:
 *
 * DESCRIPTION:
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
#define RCVD_DEST_ADDR ( ((tsCLD_ZllCommissionCustomDataStructure*)psEvent-> psCallBackEvent->psClusterInstance->pvEndPointCustomStructPtr)->sRxInterPanAddr.sDstAddr )

PUBLIC void BDB_vTlStateMachine( tsBDB_ZCLEvent *psEvent)
{
     ZPS_tsInterPanAddress       sSrcAddr;
     uint32 u32LogicalChannel;
     uint8 u8Lqi = 0;
     BDB_tsBdbEvent sBDBEvent = {0};
     APP_tsEvent                   sAppEvent;
     /* If coordinator read the commissioning event off the queue but do nothing */
     if(sZllState.u8DeviceType == 0)
     {
         DBG_vPrintf(TRACE_COMMISSION, "sZllState.u8DeviceType == 0\n");
         return;
     }


    tsCLD_ZllCommissionCallBackMessage *psZllMessage = NULL;
    if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG)
    {
        /* get detail of inter pan message */
        psZllMessage = &((tsCLD_ZllCommissionCustomDataStructure*)psEvent-> psCallBackEvent->psClusterInstance->pvEndPointCustomStructPtr)->sCallBackMessage;
        sSrcAddr = ((tsCLD_ZllCommissionCustomDataStructure*)psEvent-> psCallBackEvent->psClusterInstance->pvEndPointCustomStructPtr)->sRxInterPanAddr.sSrcAddr;
        u8Lqi = psEvent->psCallBackEvent->pZPSevent->uEvent.sApsInterPanDataIndEvent.u8LinkQuality;

        if (psZllMessage->uMessage.psScanReqPayload->u32TransactionId == 0)
        {
            /* illegal transaction id */
            DBG_vPrintf(TRACE_TL_NEGATIVE, "REJECT: transaction Id 0\n");
            return;
        }
        switch (psZllMessage->u8CommandId)
        {
            case E_CLD_COMMISSION_CMD_SCAN_REQ:
            case E_CLD_COMMISSION_CMD_SCAN_RSP:
            case E_CLD_COMMISSION_CMD_DEVICE_INFO_REQ:
            case E_CLD_COMMISSION_CMD_IDENTIFY_REQ:
                // allowed to receive by on broadcasts
                break;
            case E_CLD_COMMISSION_CMD_DEVICE_INFO_RSP:
            case E_CLD_COMMISSION_CMD_FACTORY_RESET_REQ:
            case E_CLD_COMMISSION_CMD_NETWORK_START_REQ:
            case E_CLD_COMMISSION_CMD_NETWORK_START_RSP:
            case E_CLD_COMMISSION_CMD_NETWORK_JOIN_ROUTER_REQ:
            case E_CLD_COMMISSION_CMD_NETWORK_JOIN_ROUTER_RSP:
            case E_CLD_COMMISSION_CMD_NETWORK_JOIN_END_DEVICE_REQ:
            case E_CLD_COMMISSION_CMD_NETWORK_JOIN_END_DEVICE_RSP:
            case E_CLD_COMMISSION_CMD_NETWORK_UPDATE_REQ:
                if (  (( RCVD_DEST_ADDR.eMode == ZPS_E_AM_INTERPAN_SHORT ) && ( RCVD_DEST_ADDR.uAddress.u16Addr == 0xFFFF ) )
                        || (RCVD_DEST_ADDR.eMode == ZPS_E_AM_INTERPAN_GROUP)
                   )
                {
                    DBG_vPrintf(TRACE_TL_NEGATIVE, "Drop IP on broadcast\n");
                    return;
                }
                break;

        default:
            // unknown command id, drop it
            DBG_vPrintf(TRACE_COMMISSION, "unknown command id, drop it\n");
            return;
        }; // end of switch
    }

#ifdef USE_LOG
    vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Commission task state %d type %d MyChannel %d\n", sCommission.eState, psEvent->eType, sZllState.u8MyChannel);
#endif
    void *pvNwk = ZPS_pvAplZdoGetNwkHandle();
    ZPS_tsNwkNib *psNib = ZPS_psNwkNibGetHandle( pvNwk);
    switch (sCommission.eState)
    {
        case E_IDLE:
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_START)
            {
                sCommission.u8Count = 0;
                sScanTarget.u16LQI = 0;
                ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
                sCommission.eState = E_SCANNING;
                sCommission.u32TransactionId = RND_u32GetRand(1, 0xffffffff);
                DBG_vPrintf(TRACE_COMMISSION, "RxIdle True\n");

                sCommission.bResponded = FALSE;

                sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_IN_PROGRESS;
                //sBDB.sAttrib.u32bdbPrimaryChannelSet = BDBC_TL_PRIMARY_CHANNEL_SET;
                //sBDB.sAttrib.u32bdbSecondaryChannelSet = BDBC_TL_SECONDARY_CHANNEL_SET;
                sCommission.u32ScanChannels = BDBC_TL_PRIMARY_CHANNEL_SET;    // sBDB.sAttrib.u32bdbPrimaryChannelSet;
                sCommission.bIsFirstChannel = TRUE;
                sCommission.bDoPrimaryScan = TRUE;
                DBG_vPrintf(TRACE_SCAN_REQ, "Start scan Prim %08x Second %08x\n",BDBC_TL_PRIMARY_CHANNEL_SET ,
                        BDBC_TL_SECONDARY_CHANNEL_SET );

                eAppApiPlmeGet(PHY_PIB_ATTR_CURRENT_CHANNEL, &u32LogicalChannel);
                if (u32LogicalChannel != sZllState.u8MyChannel)
                {
                    sZllState.u8MyChannel = u32LogicalChannel;
//                    DBG_vPrintf(TRUE, "Set MyChannel to %d at line %d\n", sZllState.u8MyChannel, __LINE__);
                    PDM_eSaveRecordData(PDM_ID_APP_ZLL_CMSSION, &sZllState,sizeof(tsZllState));
                    DBG_vPrintf(1, "Update Channel to %d\n", sZllState.u8MyChannel);
                }

                /* Turn down Tx power */
#if ADJUST_POWER
                eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, TX_POWER_LOW);
#endif
                sCommission.bSecondScan = FALSE;


            }
            else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG )
            {
                if (psZllMessage->u8CommandId == E_CLD_COMMISSION_CMD_SCAN_REQ )
                {
                    if ((u8Lqi < TL_SCAN_LQI_MIN) || ( (psZllMessage->uMessage.psScanReqPayload->u8ZllInfo & TL_LINK_INIT) == 0 ))
                    {
#ifdef USE_LOG
                        vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Ignore scan reqXX\n");
#endif
                        return;

                    }

jumpHere:
#ifdef USE_LOG
                    vLog_Printf((TRACE_SCAN|TRACE_COMMISSION),LOG_DEBUG, "Got Scan Req\n");
#endif
                    /* Turn down Tx power */
#if ADJUST_POWER
                    eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, TX_POWER_LOW);
#endif


                    sDstAddr = sSrcAddr;
                    sDstAddr.u16PanId = 0xFFFF;

                    sCommission.u32ResponseId = RND_u32GetRand(1, 0xffffffff);
                    sCommission.u32TransactionId = psZllMessage->uMessage.psScanReqPayload->u32TransactionId;

                    vTlSendScanResponse( psNib,
                                              &sDstAddr,
                                              sCommission.u32TransactionId,
                                              sCommission.u32ResponseId);

                    sCommission.eState = E_ACTIVE;
                    /* Timer to end inter pan */
                    ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(TL_INTERPAN_LIFE_TIME_SEC) );
                }
            }
            break;


        case E_SCANNING:
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED) {
                /*
                 * Send the next scan request
                 */
                vTlSendScanRequest(pvNwk, &sCommission);
            }
            else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG)
            {
                switch (psZllMessage->u8CommandId )
                {
                case E_CLD_COMMISSION_CMD_SCAN_RSP:
                    if (psZllMessage->uMessage.psScanRspPayload->u32TransactionId == sCommission.u32TransactionId)
                    {
                        vTlHandleScanResponse(pvNwk,
                                              psNib,
                                              &sScanTarget,
                                              psZllMessage->uMessage.psScanRspPayload,
                                              &sSrcAddr,
                                              u8Lqi);
                    }
                    break;

                case E_CLD_COMMISSION_CMD_SCAN_REQ:
                    /* If we are FN and get a scan req from a NFN remote then
                     * give up our scan and respond to theirs
                     */
                    if ((u8Lqi < TL_SCAN_LQI_MIN)   || ( (psZllMessage->uMessage.psScanReqPayload->u8ZllInfo & TL_LINK_INIT) == 0 ))
                    {
#ifdef USE_LOG
                       vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Ignore scan req\n");
#endif
                       return;
                    }
#ifdef USE_LOG
                    vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Scan Req1\n");
#endif
                    if ((sZllState.eState == FACTORY_NEW) &&
                        ( (psZllMessage->uMessage.psScanReqPayload->u8ZllInfo & TL_FACTORY_NEW) == 0 )) {

#ifdef USE_LOG
                        vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Scan Req while scanning\n");
#endif
                        ZTIMER_eStop(u8TimerBdbTl);
                        goto jumpHere;
                    }
                    else
                    {
                        if ( !sCommission.bResponded)
                        {
                            /*
                             * Only respond once
                             */
                            sCommission.bResponded = TRUE;

                            sDstAddr = sSrcAddr;
                            sDstAddr.u16PanId = 0xFFFF;

                            sCommission.u32TheirResponseId = RND_u32GetRand(1, 0xffffffff);
                            sCommission.u32TheirTransactionId = psZllMessage->uMessage.psScanReqPayload->u32TransactionId;

                            sCommission.u8Flags = 0;
                            if ((psZllMessage->uMessage.psScanReqPayload->u8ZigbeeInfo & TL_TYPE_MASK) != TL_ZED)
                            {
                                // Not a ZED requester, set FFD bit in flags
                                sCommission.u8Flags |= 0x02;
                            }
                            if (psZllMessage->uMessage.psScanReqPayload->u8ZigbeeInfo & TL_RXON_IDLE)
                            {
                                // RxOnWhenIdle, so set RXON and power source bits in the flags
                                sCommission.u8Flags |= 0x0c;
                            }

                            vTlSendScanResponse(psNib,
                                              &sDstAddr,
                                              sCommission.u32TheirTransactionId,
                                              sCommission.u32TheirResponseId);

                        }
                    }
                    break;

                case E_CLD_COMMISSION_CMD_DEVICE_INFO_REQ:
                    if (psZllMessage->uMessage.psDeviceInfoReqPayload->u32TransactionId == sCommission.u32TheirTransactionId)
                    {
                        vTlHandleDeviceInfoRequest( psZllMessage->uMessage.psDeviceInfoReqPayload,
                                                         &sSrcAddr);
                    }
                    break;

                case E_CLD_COMMISSION_CMD_IDENTIFY_REQ:
#ifdef USE_LOG
                    vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Identify request %d:%d\n", psZllMessage->uMessage.psIdentifyReqPayload->u32TransactionId, sCommission.u32TheirTransactionId );
#endif

                    if (psZllMessage->uMessage.psIdentifyReqPayload->u32TransactionId == sCommission.u32TheirTransactionId) {
                        vHandleIdentifyRequest(psZllMessage->uMessage.psIdentifyReqPayload->u16Duration);
                    }
                    break;

                default:
#ifdef USE_LOG
                    vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Unhandled during scan %02x\n", psZllMessage->u8CommandId );
#endif
                    break;
                }

            }

            break;

        case E_SCAN_WAIT_RESET_SENT:
            /* Factory reset command has been sent,
             * end commissioning
             */
            vTlEndCommissioning(pvNwk, E_IDLE, 0);
            break;

        case E_SCAN_DONE:
            if ( sScanTarget.u16LQI )
            {
#ifdef TEST_RANGE
                    if (( (sZllState.u16FreeAddrHigh - sZllState.u16FreeAddrLow) < 5) &&
                       ( (sZllState.u16FreeGroupHigh - sZllState.u16FreeGroupLow) < 5))
                    {
#ifdef USE_LOG
                        vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Out of Addr H%04x L%04x ", sZllState.u16FreeAddrHigh, sZllState.u16FreeAddrLow);
                        vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "GH%04x GL%04x\n", sZllState.u16FreeGroupHigh, sZllState.u16FreeGroupLow);
#endif
                        vTlEndCommissioning(pvNwk);
                        return;
                    }
#endif
                    /* scan done and we have a target */
                    eAppApiPlmeSet(PHY_PIB_ATTR_CURRENT_CHANNEL, sScanTarget.sScanDetails.sScanRspPayload.u8LogicalChannel);
                    if ( (sZllState.eState == NOT_FACTORY_NEW) &&
                            ( (sScanTarget.sScanDetails.sScanRspPayload.u8ZllInfo & TL_FACTORY_NEW) == 0) &&
                            ( (sScanTarget.sScanDetails.sScanRspPayload.u8ZigbeeInfo & TL_TYPE_MASK) == TL_ZED ) &&
                            (sScanTarget.sScanDetails.sScanRspPayload.u64ExtPanId == psNib->sPersist.u64ExtPanId)
                            ) {
                        /* On our network, just gather endpoint details */
#ifdef USE_LOG
                        vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "NFN -> NFN ZED On Our nwk\n");
#endif
                        /* tell the app about the target we just touched with */
                        sTarget.u16NwkAddr = sScanTarget.sScanDetails.sScanRspPayload.u16NwkAddr;
                        sTarget.u8Endpoint = sScanTarget.sScanDetails.sScanRspPayload.u8Endpoint;
                        sTarget.u16ProfileId = sScanTarget.sScanDetails.sScanRspPayload.u16ProfileId;
                        sTarget.u16DeviceId = sScanTarget.sScanDetails.sScanRspPayload.u16DeviceId;
                        sTarget.u8Version = sScanTarget.sScanDetails.sScanRspPayload.u8Version;

                        vTlEndCommissioning(pvNwk, E_INFORM_APP, 6500 );
                        return;
                    }
#ifdef USE_LOG
                    vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Send an Id Req to %016llx Id %08x\n", sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr, sCommission.u32TransactionId);
#endif
                    vTlSendIdentifyRequest(sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr, sCommission.u32TransactionId, 3);
                    ZTIMER_eStop(u8TimerBdbTl);
                    ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
                    sCommission.eState = E_SCAN_WAIT_ID;

            }
            else
            {
#ifdef USE_LOG
                vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "scan No results\n");
#endif
                DBG_vPrintf(TRACE_SCAN, "scan No results\n");
                if ((sCommission.bDoPrimaryScan == FALSE) ||
                        (BDBC_TL_SECONDARY_CHANNEL_SET /*sBDB.sAttrib.u32bdbSecondaryChannelSet*/ == 0))
                {
                    sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_NO_NETWORK;  // todo no scan response
                    vTlEndCommissioning(pvNwk, E_IDLE, 0);
                    DBG_vPrintf(TRACE_SCAN_REQ, "ALL No results\n");
                    return;
                }
                sCommission.bDoPrimaryScan = FALSE;
                sCommission.u32ScanChannels = BDBC_TL_SECONDARY_CHANNEL_SET /*sBDB.sAttrib.u32bdbSecondaryChannelSet*/;
                sCommission.u8Count = 0;
                sCommission.bIsFirstChannel = FALSE;


                DBG_vPrintf(TRACE_SCAN|TRACE_SCAN_REQ,  "Second scan %08x\n", sCommission.u32ScanChannels);
                sCommission.eState = E_SCANNING;
                ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
            }
            break;

        case E_SCAN_WAIT_ID:
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED)
            {
#ifdef USE_LOG
                vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Wait id time time out\n");
#endif

                /* send a device info req */
                vTlSendDeviceInfoReq(sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr, sCommission.u32TransactionId, 0);

                ZTIMER_eStop(u8TimerBdbTl);
                ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(2));
                sCommission.eState = E_SCAN_WAIT_INFO;
            }
            break;

        case E_SCAN_WAIT_INFO:

            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED)
            {
#ifdef USE_LOG
                vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Wait Info time out\n");
#endif
                vTlEndCommissioning(pvNwk, E_IDLE, 0);
                return;
            }
            else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG)
            {
#ifdef USE_LOG
                vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Wait info com msg Cmd=%d  %d\n", psZllMessage->u8CommandId, E_CLD_COMMISSION_CMD_DEVICE_INFO_RSP);
#endif
                if ( ( psZllMessage->u8CommandId == E_CLD_COMMISSION_CMD_DEVICE_INFO_RSP ) &&
                     ( psZllMessage->uMessage.psDeviceInfoRspPayload->u32TransactionId == sCommission.u32TransactionId ) )
                {
#ifdef USE_LOG
                    vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Got Device Info Rsp\n");
#endif
                    if ( (psZllMessage->uMessage.psDeviceInfoRspPayload->u8StartIndex == 0) &&
                                             (psZllMessage->uMessage.psDeviceInfoRspPayload->u8DeviceInfoRecordCount > 0))
                    {
                        /* catch the first device */
                        sScanTarget.sScanDetails.sScanRspPayload.u16ProfileId = psZllMessage->uMessage.psDeviceInfoRspPayload->asDeviceRecords[0].u16ProfileId;
                        sScanTarget.sScanDetails.sScanRspPayload.u16DeviceId = psZllMessage->uMessage.psDeviceInfoRspPayload->asDeviceRecords[0].u16DeviceId;
                        sScanTarget.sScanDetails.sScanRspPayload.u8Endpoint = psZllMessage->uMessage.psDeviceInfoRspPayload->asDeviceRecords[0].u8Endpoint;
                        sScanTarget.sScanDetails.sScanRspPayload.u8Version = psZllMessage->uMessage.psDeviceInfoRspPayload->asDeviceRecords[0].u8Version;
                        sScanTarget.sScanDetails.sScanRspPayload.u8GroupIdCount = psZllMessage->uMessage.psDeviceInfoRspPayload->asDeviceRecords[0].u8NumberGroupIds;
                    }

                    if ( (psZllMessage->uMessage.psDeviceInfoRspPayload->u8DeviceInfoRecordCount +
                            psZllMessage->uMessage.psDeviceInfoRspPayload->u8StartIndex)
                            < psZllMessage->uMessage.psDeviceInfoRspPayload->u8NumberSubDevices)
                    {
                        uint8 u8Index = psZllMessage->uMessage.psDeviceInfoRspPayload->u8StartIndex + psZllMessage->uMessage.psDeviceInfoRspPayload->u8DeviceInfoRecordCount;
                        vTlSendDeviceInfoReq(sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr, sCommission.u32TransactionId, u8Index);
                    }
                    else
                    {
                        sTarget.u16ProfileId = sScanTarget.sScanDetails.sScanRspPayload.u16ProfileId;
                        sTarget.u16DeviceId = sScanTarget.sScanDetails.sScanRspPayload.u16DeviceId;
                        sTarget.u8Endpoint = sScanTarget.sScanDetails.sScanRspPayload.u8Endpoint;
                        sTarget.u8Version = sScanTarget.sScanDetails.sScanRspPayload.u8Version;
                        // nwk addr will be assigned in a bit

                        sDstAddr.eMode = 3;
                        sDstAddr.u16PanId = 0xffff;
                        sDstAddr.uAddress.u64Addr = sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr;

    #ifdef USE_LOG
                        vLog_Printf(TRACE_SCAN,LOG_DEBUG, "Back to %016llx Mode %d\n", sDstAddr.uAddress.u64Addr, sDstAddr.eMode);
    #endif
                        if(bSendFactoryResetOverAir)
                        {
                            bSendFactoryResetOverAir=FALSE;
                            ZTIMER_eStop(u8TimerBdbTl);
                            vTlSendFactoryResetRequest( pvNwk,
                                                             psNib,
                                                             &sScanTarget,
                                                             sCommission.u32TransactionId);
                            sCommission.eState = E_SCAN_WAIT_RESET_SENT;
                            /* wait a bit for message to go, then finsh up */
                            ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
    #ifdef USE_LOG
                            vLog_Printf(TRACE_SCAN,LOG_DEBUG, "\nvSendFactoryResetRequest and going to E_IDLE\n");
    #endif
                        }
                        else if (sZllState.eState == FACTORY_NEW)
                        {
                            // we are FN CB, target must be a FN or NFN
#ifdef USE_LOG
                            vLog_Printf(TRACE_SCAN,LOG_DEBUG, "I'm a FN Router\n");
#endif

#if ADJUST_POWER
                            /* Turn up Tx power */
                            eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, TX_POWER_NORMAL);
#endif

                            //Kick off discovery and then form our own network
#ifdef USE_LOG
                            vLog_Printf(TRACE_SCAN,LOG_DEBUG, "Do discovery\n");
#endif

                            /* set nwkAddr as 0x0001 */
                            sStartParams.sNwkParams.u16NwkAddr = sZllState.u16FreeAddrLow++;

                            sCommission.eState = E_WAIT_DISCOVERY;
                            ZPS_eAplZdoDiscoverNetworks( BDBC_TL_PRIMARY_CHANNEL_SET );

                            ZTIMER_eStop(u8TimerBdbTl);
                            ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(650));
                        }
                        else
                        {
#ifdef USE_LOG
                            vLog_Printf(TRACE_SCAN,LOG_DEBUG, "NFN -> ??? %02x\n", sScanTarget.sScanDetails.sScanRspPayload.u8ZllInfo);
#endif
                            if (sScanTarget.sScanDetails.sScanRspPayload.u8ZllInfo & TL_FACTORY_NEW)
                            {
                                /* FN target */
                                if ((sScanTarget.sScanDetails.sScanRspPayload.u8ZigbeeInfo & TL_TYPE_MASK) != TL_ZED)
                                {
#ifdef USE_LOG
                                    vLog_Printf(TRACE_SCAN,LOG_DEBUG, "NFN -> FN ZR\n");
#endif
                                    /* router join */
                                    vTlSendRouterJoinRequest(pvNwk, psNib, &sScanTarget, sCommission.u32TransactionId);
                                    sCommission.eState = E_WAIT_JOIN_RTR_RSP;
                                    ZTIMER_eStop(u8TimerBdbTl);
                                    ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(2));

                                }
                                else
                                {
#ifdef USE_LOG
                                    vLog_Printf(TRACE_SCAN,LOG_DEBUG,"NFN -> FN ZED\n");
#endif
                                    /* end device join */
                                    vTlSendEndDeviceJoinRequest( pvNwk, psNib, &sScanTarget, sCommission.u32TransactionId);
                                    sCommission.eState = E_WAIT_JOIN_ZED_RSP;
                                    ZTIMER_eStop(u8TimerBdbTl);
                                    ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(2));
                                }
                            }
                            else
                            {
                                /* NFN target, must be on a different nwk */
                                if (sScanTarget.sScanDetails.sScanRspPayload.u64ExtPanId == psNib->sPersist.u64ExtPanId) {
    #ifdef USE_LOG
                                    vLog_Printf(TRACE_SCAN,LOG_DEBUG,"On our nwk\n"  );
                                    /* On our network, just gather endpoint details */
                                    vLog_Printf(TRACE_SCAN,LOG_DEBUG, "NFN -> NFN ZR On Our nwk\n");
    #endif
                                    /* tell the app about the target we just touched with */
                                    sTarget.u16NwkAddr = sScanTarget.sScanDetails.sScanRspPayload.u16NwkAddr;
                                    sTarget.u8Endpoint = sScanTarget.sScanDetails.sScanRspPayload.u8Endpoint;
                                    sTarget.u16ProfileId = sScanTarget.sScanDetails.sScanRspPayload.u16ProfileId;
                                    sTarget.u16DeviceId = sScanTarget.sScanDetails.sScanRspPayload.u16DeviceId;
                                    sTarget.u8Version = sScanTarget.sScanDetails.sScanRspPayload.u8Version;

                                    vTlEndCommissioning(pvNwk, E_INFORM_APP, 1500);
                                    return;
                                }
#ifdef USE_LOG
                                vLog_Printf(TRACE_SCAN,LOG_DEBUG, "NFN -> NFN ZR other nwk\n");
#endif


                                if ((sScanTarget.sScanDetails.sScanRspPayload.u8ZigbeeInfo & TL_TYPE_MASK) != TL_ZED)
                                {
#ifdef USE_LOG
                                    vLog_Printf(TRACE_SCAN,LOG_DEBUG, "NFN -> NFN ZR\n");
#endif
                                    /* router join */
                                    vTlSendRouterJoinRequest(pvNwk, psNib, &sScanTarget, sCommission.u32TransactionId);
                                    sCommission.eState = E_WAIT_JOIN_RTR_RSP;

                                }
                                else
                                {
#ifdef USE_LOG
                                    vLog_Printf(TRACE_SCAN,LOG_DEBUG,"NFN -> NFN ZED\n");
#endif
                                    /* end device join */
                                    vTlSendEndDeviceJoinRequest( pvNwk, psNib, &sScanTarget, sCommission.u32TransactionId);
                                    sCommission.eState = E_WAIT_JOIN_ZED_RSP;
                                }
                                ZTIMER_eStop(u8TimerBdbTl);
                                ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(2));

                            }
                        }
                    }
                }
            }
            break;

        case E_WAIT_START_RSP:
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG)
            {
#ifdef USE_LOG
                vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Start Rsp %d\n", psZllMessage->u8CommandId);
#endif
                if (psZllMessage->u8CommandId == E_CLD_COMMISSION_CMD_NETWORK_START_RSP)
                {
                    vTlHandleNwkStartResponse( pvNwk,
                                                    psZllMessage->uMessage.psNwkStartRspPayload,
                                                    &sCommission,
                                                    &sSrcAddr);
                }
            }
            else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED)
            {
#ifdef USE_LOG
                vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "wait startrsp time out\n");
#endif
                vTlEndCommissioning(pvNwk, E_IDLE, 0);
            }
             break;

        case E_WAIT_JOIN_RTR_RSP:
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG)
            {
                if (psZllMessage->u8CommandId == E_CLD_COMMISSION_CMD_NETWORK_JOIN_ROUTER_RSP)
                {
#ifdef USE_LOG
                    vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Got Router join Rsp %d\n", psZllMessage->uMessage.psNwkJoinRouterRspPayload->u8Status);
#endif
                    if ( (psZllMessage->uMessage.psNwkJoinRouterRspPayload->u8Status == TL_SUCCESS) &&
                         (sCommission.u32TransactionId == psZllMessage->uMessage.psNwkJoinRouterRspPayload->u32TransactionId) &&
                         (sSrcAddr.uAddress.u64Addr == sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr))
                    {
                        ZTIMER_eStop(u8TimerBdbTl);
                        ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(TL_START_UP_TIME_SEC));
                        sCommission.eState = E_WAIT_START_UP;
                    }
                }
            }
            else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED)
            {
#ifdef USE_LOG
                vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "zr join time out\n");
#endif
                vTlEndCommissioning(pvNwk, E_IDLE, 0);
            }
            break;

        case E_WAIT_JOIN_ZED_RSP:
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG)
            {
                if (psZllMessage->u8CommandId == E_CLD_COMMISSION_CMD_NETWORK_JOIN_END_DEVICE_RSP)
                {
#ifdef USE_LOG
                    vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Got Zed join Rsp %d\n", psZllMessage->uMessage.psNwkJoinEndDeviceRspPayload->u8Status);
#endif
                    if ((psZllMessage->uMessage.psNwkJoinEndDeviceRspPayload->u8Status == TL_SUCCESS) &&
                        (sCommission.u32TransactionId == psZllMessage->uMessage.psNwkJoinEndDeviceRspPayload->u32TransactionId) &&
                        (sSrcAddr.uAddress.u64Addr == sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr))
                    {
                        ZTIMER_eStop(u8TimerBdbTl);
                        ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(1 /*TL_START_UP_TIME_SEC*/));
                        sCommission.eState = E_WAIT_START_UP;
                    }
                }
            }
            else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED)
            {
#ifdef USE_LOG
                vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "ed join time out\n");
#endif
                vTlEndCommissioning(pvNwk, E_IDLE, 0);
            }
            break;

        /*
         * We are the target of a scan
         */

        case E_ACTIVE:
            /* handle process after scan rsp */
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_START)
            {
                if (sZllState.eState == NOT_FACTORY_NEW)
                {
#ifdef USE_LOG
                    vLog_Printf(TRACE_SCAN,LOG_DEBUG,"Abandon scan\n");
#endif
                     ZTIMER_eStop(u8TimerBdbTl);
                    sCommission.u8Count = 0;
                    sScanTarget.u16LQI = 0;
                    ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
                    sCommission.eState = E_SCANNING;
                    sCommission.u32TransactionId = RND_u32GetRand(1, 0xffffffff);
                    sCommission.bResponded = FALSE;
                    /* Turn down Tx power */
#if ADJUST_POWER
                    eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, TX_POWER_LOW);
#endif
                    sCommission.bSecondScan = FALSE;
                    sCommission.u32ScanChannels = BDBC_TL_PRIMARY_CHANNEL_SET;    // sBDB.sAttrib.u32bdbPrimaryChannelSet;
                    sCommission.bIsFirstChannel = TRUE;
                    sCommission.bDoPrimaryScan = TRUE;
                    return;
                }

            }
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED)
            {
#ifdef USE_LOG
                vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Active time out\n");
#endif
                vTlEndCommissioning(pvNwk, E_IDLE, 0);
            }
            else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG)
            {
#ifdef USE_LOG
                vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Zll cmd %d in active\n", psZllMessage->u8CommandId);
#endif

                /* Set up return address */
                sDstAddr = sSrcAddr;
                sDstAddr.u16PanId = 0xffff;
                switch(psZllMessage->u8CommandId)
                {
                    case E_CLD_COMMISSION_CMD_FACTORY_RESET_REQ:
                        if ((sZllState.eState == NOT_FACTORY_NEW) &&
                            (psZllMessage->uMessage.psScanReqPayload->u32TransactionId ==  sCommission.u32TransactionId))
                        {
                            sCommission.eState = E_WAIT_LEAVE_RESET;
                            /* leave req */
                            ZPS_eAplZdoLeaveNetwork(0, FALSE,FALSE);
                            sBDB.sAttrib.bLeaveRequested = TRUE;
                        }
                        break;
                    case E_CLD_COMMISSION_CMD_NETWORK_UPDATE_REQ:
                        if (psZllMessage->uMessage.psScanReqPayload->u32TransactionId ==  sCommission.u32TransactionId)
                        {
                            vTlHandleNwkUpdateRequest( pvNwk,
                                                       psNib,
                                                       psZllMessage->uMessage.psNwkUpdateReqPayload);
                        }
                        break;

                    case E_CLD_COMMISSION_CMD_IDENTIFY_REQ:
                        if (psZllMessage->uMessage.psScanReqPayload->u32TransactionId ==  sCommission.u32TransactionId)
                        {
#ifdef USE_LOG
                            vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Identify: %d\n", psZllMessage->uMessage.psIdentifyReqPayload->u16Duration);
#endif
                            vHandleIdentifyRequest(psZllMessage->uMessage.psIdentifyReqPayload->u16Duration);
                        }
                        break;

                    case E_CLD_COMMISSION_CMD_DEVICE_INFO_REQ:
                        if (psZllMessage->uMessage.psScanReqPayload->u32TransactionId ==  sCommission.u32TransactionId)
                        {
                            vTlHandleDeviceInfoRequest( psZllMessage->uMessage.psDeviceInfoReqPayload,
                                                             &sSrcAddr);
                        }
                        break;

                    case E_CLD_COMMISSION_CMD_NETWORK_JOIN_ROUTER_REQ:
                        vTlHandleRouterJoinRequest( pvNwk,
                                                        psNib,
                                                        psZllMessage->uMessage.psNwkJoinRouterReqPayload,
                                                        &sCommission,
                                                        &sSrcAddr);
                        break;

                    case E_CLD_COMMISSION_CMD_NETWORK_START_REQ:
                        vTlHandleNwkStartRequest( &sDstAddr,
                                                       psZllMessage->uMessage.psNwkStartReqPayload,
                                                       &sCommission,
                                                       &sSrcAddr);
                        break;

                    case E_CLD_COMMISSION_CMD_NETWORK_JOIN_END_DEVICE_REQ:
                       /* we are not a zed, ignore silently */
                        break;

                    default:
                        break;

                }               /* endof switch(psZllMessage->u8CommandId) */
            }               /* endof if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG) */

            break;

        case E_WAIT_DISCOVERY:
           if (psEvent->eType != BDB_E_ZCL_EVENT_DISCOVERY_DONE) {

               return;
           }
           else
           {
               ZPS_tsAplAib *psAib = ZPS_psAplAibGetAib();
#ifdef USE_LOG
               vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "discovery in commissioning\n");
#endif
               /* get unique set of pans */
               while (!bSearchDiscNt(psNib, sStartParams.sNwkParams.u64ExtPanId,
                       sStartParams.sNwkParams.u16PanId))
               {
                   sStartParams.sNwkParams.u16PanId = RND_u32GetRand(1, 0xfffe);
                   if(psAib->u64ApsUseExtendedPanid == 0)
                   {
                       sStartParams.sNwkParams.u64ExtPanId = RND_u32GetRand(1, 0xffffffff);
                       sStartParams.sNwkParams.u64ExtPanId <<= 32;
                       sStartParams.sNwkParams.u64ExtPanId |= RND_u32GetRand(0, 0xffffffff);
                   }
                   else
                   {
                       sStartParams.sNwkParams.u64ExtPanId = psAib->u64ApsUseExtendedPanid;
                   }
               };
//LJM
               /* If we initiated the touchlink and we're a FN router */
               if(sBDB.sAttrib.ebdbCommissioningStatus == E_BDB_COMMISSIONING_STATUS_IN_PROGRESS)
               {
#ifdef FIXED_CHANNEL
                    sStartParams.sNwkParams.u8LogicalChannel = DEFAULT_CHANNEL;
#else
                    // pick a random channel
                    sStartParams.sNwkParams.u8LogicalChannel = BDB_u8TlGetRandomPrimary();
#endif
                    sZllState.u8MyChannel = sStartParams.sNwkParams.u8LogicalChannel;
//                    DBG_vPrintf(TRUE, "Set MyChannel to %d at line %d\n", sZllState.u8MyChannel, __LINE__);

                    /* For FN device, start with nwkUpdateId of 1 */
                    if(sZllState.eState == FACTORY_NEW)
                    {
                        sStartParams.sNwkParams.u8NwkupdateId = 1;
                    }
#ifdef USE_LOG
                    vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Starting a new network on ch %d\n", sStartParams.sNwkParams.u8LogicalChannel);
#endif
                    /* This sets up the device, starts as router and sends device announce */
                    ZPS_eAplFormDistributedNetworkRouter(&sStartParams.sNwkParams, TRUE);
                    sCommission.eState = E_WAIT_NWK_START;

                    /* Delay to allow the network to form and the device announce messages to be broadcast */
                    ZTIMER_eStop(u8TimerBdbTl);
                    ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(2100));

                    return;
               }

           }
           // Deliberate fall through

       case E_SKIP_DISCOVERY:
#ifdef USE_LOG
           vLog_Printf(TRACE_JOIN,LOG_DEBUG, "New Epid %016llx Pan %04x\n",
                   sStartParams.sNwkParams.u64ExtPanId, sStartParams.sNwkParams.u16PanId);
           vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "e_skip-discovery\n");
#endif
           if (sStartParams.sNwkParams.u8LogicalChannel == 0)
           {
               // pick random

               sStartParams.sNwkParams.u8LogicalChannel = BDB_u8TlGetRandomPrimary();
           }

           /* send start rsp */
           tsCLD_ZllCommission_NetworkStartRspCommandPayload sNwkStartRsp;
           uint8 u8Seq;

           sNwkStartRsp.u32TransactionId = sCommission.u32TransactionId;
           sNwkStartRsp.u8Status = TL_SUCCESS;
           sNwkStartRsp.u64ExtPanId = sStartParams.sNwkParams.u64ExtPanId;
           sNwkStartRsp.u8NwkUpdateId = sStartParams.sNwkParams.u8NwkupdateId;
           sNwkStartRsp.u8LogicalChannel = sStartParams.sNwkParams.u8LogicalChannel;
           sNwkStartRsp.u16PanId = sStartParams.sNwkParams.u16PanId;
#ifdef USE_LOG
           vLog_Printf(TRACE_COMMISSION ,LOG_DEBUG, "send Start Resp\n");
#endif
           eCLD_ZllCommissionCommandNetworkStartRspCommandSend( &sDstAddr,
                                                                &u8Seq,
                                                                &sNwkStartRsp);

           /* Gateway can't be stolen so factory new test has already been done
            * to have got here we must be factory new so no need to initiate a
            * Leave before starting as a router
            */
           sCommission.eState = E_START_ROUTER;

           /* give message time to go */
           ZTIMER_eStop(u8TimerBdbTl);
           ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
           break;

       case E_WAIT_NWK_START:
           if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED )
           {
#ifdef USE_LOG
               vLog_Printf(TRACE_JOIN,LOG_DEBUG,"NWK should have started now...\n");
#endif
               /* We just started our own network so we're now NOT factory new */
               sZllState.eState = NOT_FACTORY_NEW;

               if ((sScanTarget.sScanDetails.sScanRspPayload.u8ZigbeeInfo & TL_TYPE_MASK) != TL_ZED)
               {
#ifdef USE_LOG
                   vLog_Printf(TRACE_SCAN,LOG_DEBUG, "NFN -> FN ZR\n");
#endif
                   /* router join */
                   vTlSendRouterJoinRequest(pvNwk, psNib, &sScanTarget, sCommission.u32TransactionId);
                   sCommission.eState = E_WAIT_JOIN_RTR_RSP;
                   ZTIMER_eStop(u8TimerBdbTl);
                   ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(2));

               }
               else
               {
#ifdef USE_LOG
                   vLog_Printf(TRACE_SCAN,LOG_DEBUG,"NFN -> FN ZED\n");
#endif
                   /* end device join */
                   vTlSendEndDeviceJoinRequest( pvNwk, psNib, &sScanTarget, sCommission.u32TransactionId);
                   sCommission.eState = E_WAIT_JOIN_ZED_RSP;
                   ZTIMER_eStop(u8TimerBdbTl);
                   ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(2));
               }
           }
           break;

       case E_WAIT_LEAVE:
           sBDB.sAttrib.bLeaveRequested = FALSE;
           // todo needs handling
           break;

       case E_WAIT_LEAVE_RESET:
           if ((psEvent->eType == BDB_E_ZCL_EVENT_LEAVE_CFM) || (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED )) {
#ifdef USE_LOG
                vLog_Printf(TRACE_JOIN,LOG_DEBUG,"WARNING: Received Factory reset \n");
#endif
                PDM_vDeleteAllDataRecords();
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
                vAHI_SwReset();
#else
                RESET_SystemReset();
#endif
           }
           break;

        case E_START_ROUTER:
#ifdef USE_LOG
            vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "\nStart router\n");
#endif
            /* set zll params */
            sZllState.u16MyAddr  = sStartParams.sNwkParams.u16NwkAddr;
            sZllState.u16FreeAddrLow = sStartParams.u16FreeNwkAddrBegin;
            sZllState.u16FreeAddrHigh = sStartParams.u16FreeNwkAddrEnd;
            sZllState.u16FreeGroupLow = sStartParams.u16FreeGroupIdBegin;
            sZllState.u16FreeGroupHigh = sStartParams.u16FreeGroupIdEnd;
            sZllState.u8MyChannel = sStartParams.sNwkParams.u8LogicalChannel;
//            DBG_vPrintf(TRUE, "Set MyChannel to %d at line %d\n", sZllState.u8MyChannel, __LINE__);
            sZllState.eState = NOT_FACTORY_NEW;
            BDB_vTlSetGroupAddress( sStartParams.u16GroupIdBegin, GROUPS_REQUIRED);
            sZllState.eState = NOT_FACTORY_NEW;
            PDM_eSaveRecordData( PDM_ID_APP_ZLL_CMSSION,&sZllState,sizeof(tsZllState));

            /* Set nwk params */
            BDB_eTlDecryptKey( sStartParams.au8NwkKey,
                                sStartParams.au8NwkKey,
                                sCommission.u32TransactionId,
                                sCommission.u32ResponseId,
                                sStartParams.sNwkParams.u8KeyIndex);

            /* This sets up the device, starts as router and sends device ance */
            ZPS_eAplFormDistributedNetworkRouter(&sStartParams.sNwkParams, TRUE);


            if (sStartParams.u16InitiatorNwkAddr != 0)
            {
                ZPS_eAplZdoDirectJoinNetwork(sStartParams.u64InitiatorIEEEAddr,  sStartParams.u16InitiatorNwkAddr, sCommission.u8Flags /*MAC_FLAGS*/);
#ifdef USE_LOG
                vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Direct join %02x\n", sCommission.u8Flags);
#endif
            }


            sZllState.eNodeState = E_RUNNING;
            PDM_eSaveRecordData( PDM_ID_APP_ZLL_CMSSION,&sZllState,sizeof(sZllState));


            ZPS_eAplAibSetApsTrustCenterAddress(0xffffffffffffffffULL);
#if PERMIT_JOIN
            ZPS_eAplZdoPermitJoining( PERMIT_JOIN_TIME);
#endif

#ifdef USE_LOG
            vLog_Printf(TRACE_JOIN,LOG_DEBUG, "My Address %04x\n", sZllState.u16MyAddr);
            vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Free Addr low %04x\n", sZllState.u16FreeAddrLow);
            vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Free Addr High %04x\n", sZllState.u16FreeAddrHigh);
            vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Free Group low %04x\n", sZllState.u16FreeGroupLow);
            vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Free Group high%04x\n", sZllState.u16FreeGroupHigh);
            vLog_Printf(TRACE_JOIN,LOG_DEBUG, "My Channel %d\n", sZllState.u8MyChannel);
#endif

            sCommission.eState = E_IDLE;
            sCommission.u32TransactionId = 0;
            sCommission.u32ResponseId = 0;
            ZTIMER_eStop(u8TimerBdbTl);
#ifdef USE_LOG
            vLog_Printf(TRACE_JOIN,LOG_DEBUG, "All done\n");
#endif
            break;

        case E_WAIT_START_UP:
        {
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED) {
#if ADJUST_POWER
                eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, TX_POWER_NORMAL);
#endif
                if ( sZllState.eState == FACTORY_NEW ) {

#ifdef USE_LOG
                    vLog_Printf(TRACE_JOIN,LOG_DEBUG, "start up on the channel\n");
#endif

                    sZllState.eState = NOT_FACTORY_NEW;
                } else {
#ifdef USE_LOG
                    vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Back on Ch %d\n", sZllState.u8MyChannel);
#endif
                    ZPS_vNwkNibSetChannel( ZPS_pvAplZdoGetNwkHandle(), sZllState.u8MyChannel);

                }

#ifdef USE_LOG
                vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Free addr %04x to %04x\n", sZllState.u16FreeAddrLow, sZllState.u16FreeAddrHigh);
                vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Free Gp  %04x to %04x\n", sZllState.u16FreeGroupLow, sZllState.u16FreeGroupHigh);
#endif



                sCommission.eState = E_INFORM_APP;
                sCommission.u32TransactionId = 0;
                sCommission.bResponded = FALSE;

                /* Call application */
                sBDBEvent.eEventType = BDB_EVENT_NWK_FORMATION_SUCCESS;
                sBDBEvent.uEventData.sZpsAfEvent.sStackEvent.uEvent.sNwkFormationEvent.u8Status = ZPS_E_SUCCESS;
                APP_vBdbCallback(&sBDBEvent);

                ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(1500) );
#ifdef USE_LOG
                vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Wait inform\n");
#endif

                sZllState.eNodeState = E_RUNNING;

            }
        }
            break;

        case E_INFORM_APP:
#ifdef USE_LOG
                vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "E_INFORM_APP\n");
#endif
            /* tell the ap about the target we just joined with */
            if (sTarget.u16NwkAddr > 0) {
                sAppEvent.eType = APP_E_EVENT_TOUCH_LINK;
                sAppEvent.uEvent.sTouchLink= sTarget;
                sAppEvent.uEvent.sTouchLink.u64Address = sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr;
                sAppEvent.uEvent.sTouchLink.u8MacCap = 0;
                if ((sScanTarget.sScanDetails.sScanRspPayload.u8ZigbeeInfo & TL_TYPE_MASK) != TL_ZED)
                {
                    // Not a ZED requester, set FFD bit in flags
                    sAppEvent.uEvent.sTouchLink.u8MacCap |= 0x02;
                }
                if (sScanTarget.sScanDetails.sScanRspPayload.u8ZigbeeInfo & TL_RXON_IDLE)
                {
                    // RxOnWhenIdle, so set RXON and power source bits in the flags
                    sAppEvent.uEvent.sTouchLink.u8MacCap |= 0x0c;
                }
#ifdef USE_LOG
                vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Inform on %04x\n", sTarget.u16NwkAddr);
#endif
                ZQ_bQueueSend(&APP_msgAppEvents, &sAppEvent);
            }
            sCommission.eState = E_IDLE;

#ifdef USE_LOG
            vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Inform App, stop\n");
#endif
            break;

        default:
#ifdef USE_LOG
            vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Unhandled event %d\n", psEvent->eType);
#endif
            break;
    }

}

/****************************************************************************
 *
 * NAME: BDB_vTlSetGroupAddress
 *
 * DESCRIPTION:
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void BDB_vTlSetGroupAddress(uint16 u16GroupStart, uint8 u8NumGroups)
{
    int i;

    /* This passes all the required group addresses for the device
     * if the are morethan one sub devices (endpoints) they need
     * sharing amoung the endpoints
     * In this case there is one the 1 Rc endpoint, so all group addresses
     * are for it
     */
    for (i=0; i<NUM_GROUP_RECORDS && i<u8NumGroups; i++) {
        sGroupTable.asGroupRecords[i].u16GroupId = u16GroupStart++;
        sGroupTable.asGroupRecords[i].u8GroupType = 0;
#ifdef USE_LOG
        vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Idx %d Grp %04x\n", i, sGroupTable.asGroupRecords[i].u16GroupId);
#endif
    }
    sGroupTable.u8NumRecords = u8NumGroups;

    PDM_eSaveRecordData( PDM_ID_APP_GROUP_TABLE,&sGroupTable,sizeof(tsZllGroupInfoTable));

}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: vTlEndCommissioning
 *
 * DESCRIPTION:

 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlEndCommissioning( void* pvNwk, eState eState, uint16 u16TimeMS)
{
    sCommission.eState = eState;
    ZPS_vNwkNibSetChannel( pvNwk, sZllState.u8MyChannel);

#ifdef USE_LOG
    vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Ending commissioning, going to state %d ch %d on line %d\n", eState, sZllState.u8MyChannel, __LINE__);
#endif

#if ADJUST_POWER
    eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, TX_POWER_NORMAL);
#endif
    sCommission.u32TransactionId = 0;
    sCommission.bResponded = FALSE;
    ZTIMER_eStop( u8TimerBdbTl);
    if (u16TimeMS > 0) {
        ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(u16TimeMS));
    }

}

/****************************************************************************
 *
 * NAME:
 *
 * DESCRIPTION:
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlSendScanRequest(void *pvNwk, tsCommissionData *psCommission)
{
    ZPS_tsInterPanAddress       sDstAddr;
    tsCLD_ZllCommission_ScanReqCommandPayload                   sScanReqPayload;
    uint8 u8Seq;
    uint32 u32ChannelNumber = 11;
    uint32 u32ChannelMask = (1<<11);

    if (psCommission->bIsFirstChannel)
    {
        psCommission->u8Count = 4;
        psCommission->bIsFirstChannel = FALSE;
    }
    if (psCommission->u32ScanChannels == 0)
    {
        // all done
        psCommission->eState = E_SCAN_DONE;
        DBG_vPrintf(TRACE_SCAN_REQ, "Scan Set complete\n");
        ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(5));
        return;
    }
    else
    {
        while ((psCommission->u32ScanChannels & u32ChannelMask) == 0)
        {
            u32ChannelMask <<= 1;
            u32ChannelNumber++;
        };
        DBG_vPrintf(TRACE_SCAN_REQ, "TL scan use ch %d\n", u32ChannelNumber);
    }

    eAppApiPlmeSet(PHY_PIB_ATTR_CURRENT_CHANNEL, u32ChannelNumber);
    sDstAddr.eMode = ZPS_E_AM_INTERPAN_SHORT;
    sDstAddr.u16PanId = 0xffff;
    sDstAddr.uAddress.u16Addr = 0xffff;
    sScanReqPayload.u32TransactionId = psCommission->u32TransactionId;
    sScanReqPayload.u8ZigbeeInfo =  TL_ROUTER | TL_RXON_IDLE;              // Rxon idle router

    sScanReqPayload.u8ZllInfo = (sZllState.eState == FACTORY_NEW) ?
            (TL_PROFILE_INTEROP|TL_FACTORY_NEW|TL_LINK_INIT|TL_ADDR_ASSIGN) :
            (TL_PROFILE_INTEROP|TL_LINK_INIT|TL_ADDR_ASSIGN);                  // factory New Addr assign Initiating

    //DBG_vPrintf(TRACE_COMMISSION, "Scan Req ret %02x\n",  );
    eCLD_ZllCommissionCommandScanReqCommandSend(
                                &sDstAddr,
                                 &u8Seq,
                                 &sScanReqPayload);

    ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(TL_SCAN_RX_TIME_MS));

    if (psCommission->u8Count)
    {
        psCommission->u8Count--;
        DBG_vPrintf(TRACE_SCAN_REQ, "Scan count now %d Mask %08x\n", psCommission->u8Count, psCommission->u32ScanChannels);
    }
    else
    {
        // clear channel as done
        psCommission->u32ScanChannels &= ~u32ChannelMask;
        DBG_vPrintf(TRACE_SCAN_REQ, "Scan Mask now %08x\n", psCommission->u32ScanChannels);
    }
}

/****************************************************************************
 *
 * NAME:
 *
 * DESCRIPTION: sends a scan response following a touch link scan request
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlSendScanResponse(ZPS_tsNwkNib *psNib,
                               ZPS_tsInterPanAddress       *psDstAddr,
                               uint32 u32TransactionId,
                               uint32 u32ResponseId)
{
    tsCLD_ZllCommission_ScanRspCommandPayload                   sScanRsp;
    uint8 u8Seq;
    uint8 i;


    memset(&sScanRsp, 0, sizeof(tsCLD_ZllCommission_ScanRspCommandPayload));

    sScanRsp.u32TransactionId = u32TransactionId;
    sScanRsp.u32ResponseId = u32ResponseId;


    sScanRsp.u8RSSICorrection = 0;
    sScanRsp.u8ZigbeeInfo = TL_ROUTER | TL_RXON_IDLE;
    sScanRsp.u16KeyMask = TL_SUPPORTED_KEYS;
    if (sZllState.eState == FACTORY_NEW) {
        sScanRsp.u8ZllInfo = TL_PROFILE_INTEROP|TL_FACTORY_NEW|TL_ADDR_ASSIGN;
        // Ext Pan 0
        // nwk update 0
        // logical channel zero
        // Pan Id 0
        sScanRsp.u16NwkAddr = 0xffff;
    } else {
        sScanRsp.u8ZllInfo = TL_PROFILE_INTEROP|TL_ADDR_ASSIGN;
        sScanRsp.u64ExtPanId = psNib->sPersist.u64ExtPanId;
        sScanRsp.u8NwkUpdateId = psNib->sPersist.u8UpdateId;
        sScanRsp.u16PanId = psNib->sPersist.u16VsPanId;
        sScanRsp.u16NwkAddr = psNib->sPersist.u16NwkAddr;
    }

    sScanRsp.u8LogicalChannel = psNib->sPersist.u8VsChannel;
    for (i=0; i<sDeviceTable.u8NumberDevices; i++) {
        sScanRsp.u8TotalGroupIds += sDeviceTable.asDeviceRecords[i].u8NumberGroupIds;
    }
    sScanRsp.u8NumberSubDevices = sDeviceTable.u8NumberDevices;
    if (sScanRsp.u8NumberSubDevices == 1)
    {
        if (sCommission.bProfileInterOp)
        {

            sScanRsp.u16ProfileId = sDeviceTable.asDeviceRecords[0].u16ProfileId;
            sScanRsp.u16DeviceId = sDeviceTable.asDeviceRecords[0].u16DeviceId;
            sScanRsp.u8Version = sDeviceTable.asDeviceRecords[0].u8Version;
        }
        else
        {
            sScanRsp.u16ProfileId = TL_LEGACY_PROFILE_ID;
            sScanRsp.u16DeviceId = TL_LEGACY_DEVICE_ID;
            sScanRsp.u8Version = TL_LEGACY_VERSION_ID;
        }


        sScanRsp.u8Endpoint = sDeviceTable.asDeviceRecords[0].u8Endpoint;
        sScanRsp.u8GroupIdCount = sDeviceTable.asDeviceRecords[0].u8NumberGroupIds;
    }

    eCLD_ZllCommissionCommandScanRspCommandSend( psDstAddr,
            &u8Seq,
            &sScanRsp);
}

/****************************************************************************
 *
 * NAME:  vTlHandleScanResponse
 *
 * DESCRIPTION: handles a scan response
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlHandleScanResponse(void *pvNwk,
                                 ZPS_tsNwkNib *psNib,
                                 tsZllScanTarget *psScanTarget,
                                 tsCLD_ZllCommission_ScanRspCommandPayload                   *psScanRsp,
                                 ZPS_tsInterPanAddress       *psSrcAddr,
                                 uint8 u8Lqi)
{
    uint16 u16AdjustedLqi;
    ZPS_tsInterPanAddress       sDstAddr;
    uint8 u8Seq;

    u16AdjustedLqi = u8Lqi + psScanRsp->u8RSSICorrection;

    if (u16AdjustedLqi < TL_SCAN_LQI_MIN ) {
#ifdef USE_LOG
        vLog_Printf(TRACE_SCAN,LOG_DEBUG, "Reject LQI %d\n", u8Lqi);
#endif
        return;
    }
#ifdef USE_LOG
    vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Their %016llx Mine %016llx TC %016llx\n",
            psScanRsp->u64ExtPanId,
            psNib->sPersist.u64ExtPanId,
            ZPS_eAplAibGetApsTrustCenterAddress() );
#endif
    if ( (sZllState.eState == NOT_FACTORY_NEW) &&
            (psScanRsp->u64ExtPanId != psNib->sPersist.u64ExtPanId) &&
            ( ZPS_eAplAibGetApsTrustCenterAddress() != 0xffffffffffffffffULL )
            )
    {
#ifdef USE_LOG
        vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Non Zll nwk, target not my nwk\n");
#endif
        return;
    }

    /*check for supported keys */
    if ( (TL_SUPPORTED_KEYS & psScanRsp->u16KeyMask) == 0)
    {
        // No common supported key index
        if ( (sZllState.eState == FACTORY_NEW) ||
            ( (sZllState.eState == NOT_FACTORY_NEW) && (psScanRsp->u64ExtPanId != psNib->sPersist.u64ExtPanId) )
                )
        {
            /* Either we are factory new or
             * we are not factory new and the target is on another pan
             * there is no common key index, to exchange keys so drop the scan response without further action
             */
#ifdef USE_LOG
            vLog_Printf(TRACE_SCAN,LOG_DEBUG, "No common security level, target not my nwk\n");
#endif
            return;
        }
    }
    /*
     * Check for Nwk Update Ids
     */
    if ( (sZllState.eState == NOT_FACTORY_NEW) &&
         (psScanRsp->u64ExtPanId == psNib->sPersist.u64ExtPanId) &&
         (psScanRsp->u16PanId == psNib->sPersist.u16VsPanId))
    {
#ifdef USE_LOG
        vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Our Nwk Id %d theirs %d\n", psNib->sPersist.u8UpdateId, psScanRsp->u8NwkUpdateId );
#endif
        if (psNib->sPersist.u8UpdateId != psScanRsp->u8NwkUpdateId ) {
            if ( psNib->sPersist.u8UpdateId == BDB_u8TlNewUpdateID(psNib->sPersist.u8UpdateId, psScanRsp->u8NwkUpdateId) )
            {
#ifdef USE_LOG
                vLog_Printf(TRACE_COMMISSION, LOG_DEBUG, "Use ours on %d\n", sZllState.u8MyChannel);
#endif
                /*
                 * Send them a network update request
                 */
                tsCLD_ZllCommission_NetworkUpdateReqCommandPayload          sNwkUpdateReqPayload;

                sDstAddr.eMode = ZPS_E_AM_INTERPAN_IEEE;
                sDstAddr.u16PanId = 0xffff;
                sDstAddr.uAddress.u64Addr = psSrcAddr->uAddress.u64Addr;

                sNwkUpdateReqPayload.u32TransactionId = sCommission.u32TransactionId;
                sNwkUpdateReqPayload.u16NwkAddr = psScanRsp->u16NwkAddr;
                sNwkUpdateReqPayload.u64ExtPanId = psNib->sPersist.u64ExtPanId;
                sNwkUpdateReqPayload.u8NwkUpdateId = psNib->sPersist.u8UpdateId;
                sNwkUpdateReqPayload.u8LogicalChannel = sZllState.u8MyChannel;

                sNwkUpdateReqPayload.u16PanId = psNib->sPersist.u16VsPanId;


                eCLD_ZllCommissionCommandNetworkUpdateReqCommandSend(
                                            &sDstAddr,
                                            &u8Seq,
                                            &sNwkUpdateReqPayload);

                return;

            } else {
                /*
                 * Part of a network and our Update Id is out of date
                 */
                ZPS_vNwkNibSetUpdateId( pvNwk, psScanRsp->u8NwkUpdateId);
                sZllState.u8MyChannel = psScanRsp->u8LogicalChannel;
//                DBG_vPrintf(TRUE, "Set MyChannel to %d at line %d\n", sZllState.u8MyChannel, __LINE__);
                vTlEndCommissioning(pvNwk, E_IDLE, 0);

#ifdef USE_LOG
                vLog_Printf(TRACE_SCAN,LOG_DEBUG, "Update Id = %04x and rejoin\n",psNib->sPersist.u8UpdateId);
#endif

                return;
            }
        }
    }

    if (u16AdjustedLqi > psScanTarget->u16LQI) {
        psScanTarget->u16LQI = u16AdjustedLqi;
        /*
         *  joining scenarios
         * 1) Any router,
         * 2) we are NFN then FN device
         * 3) we are NFN then any NFN ED that is not on our Pan
         */
#ifdef USE_LOG
        vLog_Printf(TRACE_SCAN,LOG_DEBUG, "Accept %d\n", psScanTarget->u16LQI);
#endif
        psScanTarget->sScanDetails.sSrcAddr = *psSrcAddr;
        psScanTarget->sScanDetails.sScanRspPayload = *psScanRsp;
    }

}

/****************************************************************************
 *
 * NAME:  vTlHandleDeviceInfoRequest
 *
 * DESCRIPTION: sends device info request to the target
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlHandleDeviceInfoRequest( tsCLD_ZllCommission_DeviceInfoReqCommandPayload *psDeviceInfoReq,
                                              ZPS_tsInterPanAddress       *psSrcAddr)
{
    tsCLD_ZllCommission_DeviceInfoRspCommandPayload sDeviceInfoRspPayload;
    ZPS_tsInterPanAddress       sDstAddr;
    int i, j;
    uint8 u8Seq;

#ifdef USE_LOG
    vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Device Info request\n");
#endif
    memset(&sDeviceInfoRspPayload, 0, sizeof(tsCLD_ZllCommission_DeviceInfoRspCommandPayload));
    sDeviceInfoRspPayload.u32TransactionId = psDeviceInfoReq->u32TransactionId;
    sDeviceInfoRspPayload.u8NumberSubDevices = ZLO_NUMBER_DEVICES;
    sDeviceInfoRspPayload.u8StartIndex = psDeviceInfoReq->u8StartIndex;
    sDeviceInfoRspPayload.u8DeviceInfoRecordCount = 0;

    // copy from table sDeviceTable
    if ( psDeviceInfoReq->u8StartIndex < ZLO_NUMBER_DEVICES)
    {
    // copy from table sDeviceTable

        j = psDeviceInfoReq->u8StartIndex;
        for (i=0; i<TL_MAX_DEVICE_RECORDS && j<ZLO_NUMBER_DEVICES; i++, j++)
        {
            sDeviceInfoRspPayload.asDeviceRecords[i] = sDeviceTable.asDeviceRecords[j];
            sDeviceInfoRspPayload.u8DeviceInfoRecordCount++;
            if (sCommission.bProfileInterOp == FALSE)
            {
                sDeviceInfoRspPayload.asDeviceRecords[i].u16DeviceId = TL_LEGACY_DEVICE_ID;
                sDeviceInfoRspPayload.asDeviceRecords[i].u16ProfileId = TL_LEGACY_PROFILE_ID;
                sDeviceInfoRspPayload.asDeviceRecords[i].u8Version = TL_LEGACY_VERSION_ID;
            }
        }
    }
    else
    {
        // start index > No of sub Devices
        DBG_vPrintf(TRACE_TL_NEGATIVE, "Start Index out of bounds\n");
    }

    sDstAddr = *psSrcAddr;
    sDstAddr.u16PanId = 0xffff;

    eCLD_ZllCommissionCommandDeviceInfoRspCommandSend( &sDstAddr,
                                                       &u8Seq,
                                                       &sDeviceInfoRspPayload);

}

/****************************************************************************
 *
 * NAME:  vTlSendIdentifyRequest
 *
 * DESCRIPTION: sends an identify request to the target
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlSendIdentifyRequest(uint64 u64Addr, uint32 u32TransactionId, uint8 u8Time)
{
    ZPS_tsInterPanAddress  sDstAddr;
    tsCLD_ZllCommission_IdentifyReqCommandPayload               sIdentifyReqPayload;
    uint8 u8Seq;

    sDstAddr.eMode = ZPS_E_AM_INTERPAN_IEEE;
    sDstAddr.u16PanId = 0xffff;
    sDstAddr.uAddress.u64Addr = u64Addr;
    sIdentifyReqPayload.u32TransactionId = sCommission.u32TransactionId;
    sIdentifyReqPayload.u16Duration =3;

    eCLD_ZllCommissionCommandDeviceIndentifyReqCommandSend(
                                        &sDstAddr,
                                        &u8Seq,
                                        &sIdentifyReqPayload);
}

/****************************************************************************
 *
 * NAME: vSendDeviceInfoReq
 *
 * DESCRIPTION: Sends the touch link device info request
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlSendDeviceInfoReq(uint64 u64Addr, uint32 u32TransactionId, uint8 u8Index)
{
    ZPS_tsInterPanAddress       sDstAddr;
    uint8 u8Seq;
    tsCLD_ZllCommission_DeviceInfoReqCommandPayload             sDeviceInfoReqPayload;

    sDstAddr.eMode = ZPS_E_AM_INTERPAN_IEEE;
    sDstAddr.u16PanId = 0xffff;
    sDstAddr.uAddress.u64Addr = u64Addr;

    sDeviceInfoReqPayload.u32TransactionId = u32TransactionId;
    sDeviceInfoReqPayload.u8StartIndex = u8Index;
    eCLD_ZllCommissionCommandDeviceInfoReqCommandSend( &sDstAddr,
                                                       &u8Seq,
                                                       &sDeviceInfoReqPayload );
}
#if 0
/****************************************************************************
 *
 * NAME: vTlSendStartRequest
 *
 * DESCRIPTION: send a network start request to the target
 *
 * RETURNS:
 *
 ****************************************************************************/
PRIVATE void vTlSendStartRequest( void *pvNwk,
                                ZPS_tsNwkNib *psNib,
                                tsZllScanTarget *psScanTarget,
                                uint32 u32TransactionId)
{
    ZPS_tsInterPanAddress       sDstAddr;
    tsCLD_ZllCommission_NetworkStartReqCommandPayload           sNwkStartReqPayload;
    uint8 u8Seq;
    uint16 u16KeyMask;
    int i;

    if (psScanTarget->sScanDetails.sScanRspPayload.u8ZllInfo & TL_FACTORY_NEW) {
#ifdef USE_LOG
        vLog_Printf(TRACE_SCAN,LOG_DEBUG, "FN -> FN Router target\n");
#endif
        /* nwk start */
    }
    else
    {
#ifdef USE_LOG
        vLog_Printf(TRACE_SCAN,LOG_DEBUG, "FN -> NFN Router target\n");
#endif
        /* nwk start */
    }

    memset(&sNwkStartReqPayload, 0, sizeof(tsCLD_ZllCommission_NetworkStartReqCommandPayload));
    sNwkStartReqPayload.u32TransactionId = sCommission.u32TransactionId;
    // ext pan 0, target decides
#ifdef FIXED_CHANNEL
    sNwkStartReqPayload.u8LogicalChannel = DEFAULT_CHANNEL;
#endif
    u16KeyMask = TL_SUPPORTED_KEYS & psScanTarget->sScanDetails.sScanRspPayload.u16KeyMask;

    if (u16KeyMask & TL_MASTER_KEY_MASK) {
        sNwkStartReqPayload.u8KeyIndex = TL_MASTER_KEY_INDEX;
    } else if (u16KeyMask & TL_CERTIFICATION_KEY_MASK) {
        sNwkStartReqPayload.u8KeyIndex = TL_CERTIFICATION_KEY_INDEX;
    } else {
        sNwkStartReqPayload.u8KeyIndex = TL_TEST_KEY_INDEX;
    }

    for (i=0; i<16; i++) {
#ifndef FIXED_NWK_KEY
        au8TempKeyStore[i] = (uint8)(RND_u32GetRand256() & 0xFF);
#else
        au8TempKeyStore[i] =  0xaa;
#endif
    }


    BDB_u8TlEncryptKey( au8TempKeyStore,
                        sNwkStartReqPayload.au8NwkKey,
                        u32TransactionId,
                        psScanTarget->sScanDetails.sScanRspPayload.u32ResponseId,
                        sNwkStartReqPayload.u8KeyIndex);

    // Pan Id 0 target decides

    // take our group address requirement
    BDB_vTlSetGroupAddress(sZllState.u16FreeGroupLow, GROUPS_REQUIRED);
    sZllState.u16FreeGroupLow += GROUPS_REQUIRED;

    /* todo this look wrong */
    /* todo should assign addresses and group ranges if router is address assign capable */
    sNwkStartReqPayload.u16InitiatorNwkAddr = sZllState.u16FreeAddrLow;
    sZllState.u16MyAddr = sZllState.u16FreeAddrLow++;

    sTarget.u16NwkAddr = sZllState.u16FreeAddrLow;
    sNwkStartReqPayload.u16NwkAddr = sZllState.u16FreeAddrLow++;

    sNwkStartReqPayload.u64InitiatorIEEEAddr = sDeviceTable.asDeviceRecords[0].u64IEEEAddr;

    sDstAddr.eMode = 3;
    sDstAddr.u16PanId = 0xffff;
    sDstAddr.uAddress.u64Addr = psScanTarget->sScanDetails.sSrcAddr.uAddress.u64Addr;

    ZPS_vNwkNibSetChannel( pvNwk, psScanTarget->sScanDetails.sScanRspPayload.u8LogicalChannel);

    eCLD_ZllCommissionCommandNetworkStartReqCommandSend( &sDstAddr,
                                                         &u8Seq,
                                                         &sNwkStartReqPayload  );
}
#endif
/****************************************************************************
 *
 * NAME: vTlSendFactoryResetRequest
 *
 * DESCRIPTION: send a factory reset request to the target
 *
 * RETURNS:
 *
 ****************************************************************************/
PRIVATE void vTlSendFactoryResetRequest( void *pvNwk,
                                       ZPS_tsNwkNib *psNib,
                                       tsZllScanTarget *psScanTarget,
                                       uint32 u32TransactionId)
{
    ZPS_tsInterPanAddress       sDstAddr;
    tsCLD_ZllCommission_FactoryResetReqCommandPayload sFacRstPayload;
    uint8 u8Seq;
    memset(&sFacRstPayload, 0, sizeof(tsCLD_ZllCommission_FactoryResetReqCommandPayload));
    sDstAddr.eMode = 3;
    sDstAddr.u16PanId = 0xffff;
    sDstAddr.uAddress.u64Addr = psScanTarget->sScanDetails.sSrcAddr.uAddress.u64Addr;

    ZPS_vNwkNibSetChannel( pvNwk, psScanTarget->sScanDetails.sScanRspPayload.u8LogicalChannel);
    sFacRstPayload.u32TransactionId= psScanTarget->sScanDetails.sScanRspPayload.u32TransactionId;
    eCLD_ZllCommissionCommandFactoryResetReqCommandSend( &sDstAddr,
                                                         &u8Seq,
                                                         &sFacRstPayload  );
}

/****************************************************************************
 *
 * NAME: vTlSendRouterJoinRequest
 *
 * DESCRIPTION: send a router join request to the target
 *
 * RETURNS:
 *
 ****************************************************************************/
PRIVATE void vTlSendRouterJoinRequest( void * pvNwk,
                                     ZPS_tsNwkNib *psNib,
                                     tsZllScanTarget *psScanTarget,
                                     uint32 u32TransactionId)
{
    ZPS_tsInterPanAddress       sDstAddr;
    tsCLD_ZllCommission_NetworkJoinRouterReqCommandPayload      sNwkJoinRouterReqPayload;
    uint16 u16KeyMask;
    uint8 u8Seq;


    memset(&sNwkJoinRouterReqPayload, 0, sizeof(tsCLD_ZllCommission_NetworkJoinRouterReqCommandPayload));

    sNwkJoinRouterReqPayload.u32TransactionId = u32TransactionId;
    // ext pan 0, target decides
    u16KeyMask = TL_SUPPORTED_KEYS & psScanTarget->sScanDetails.sScanRspPayload.u16KeyMask;

    if (u16KeyMask & TL_MASTER_KEY_MASK)
    {
        sNwkJoinRouterReqPayload.u8KeyIndex = TL_MASTER_KEY_INDEX;
    }
    else if (u16KeyMask & TL_CERTIFICATION_KEY_MASK)
    {
        sNwkJoinRouterReqPayload.u8KeyIndex = TL_CERTIFICATION_KEY_INDEX;
    }
    else
    {
        sNwkJoinRouterReqPayload.u8KeyIndex = TL_TEST_KEY_INDEX;
    }

    BDB_u8TlEncryptKey( psNib->sTbl.psSecMatSet[0].au8Key,
                        sNwkJoinRouterReqPayload.au8NwkKey,
                        u32TransactionId,
                        psScanTarget->sScanDetails.sScanRspPayload.u32ResponseId,
                        sNwkJoinRouterReqPayload.u8KeyIndex);



    sNwkJoinRouterReqPayload.u64ExtPanId = psNib->sPersist.u64ExtPanId;

#ifdef USE_LOG
    vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "RJoin nwk epid %016llx\n", psNib->sPersist.u64ExtPanId);
    vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "RJoin nwk epid %016llx\n",ZPS_psAplAibGetAib()->u64ApsUseExtendedPanid);
#endif

    sNwkJoinRouterReqPayload.u8NwkUpdateId = psNib->sPersist.u8UpdateId;
    sNwkJoinRouterReqPayload.u8LogicalChannel = sZllState.u8MyChannel;
    sNwkJoinRouterReqPayload.u16PanId = psNib->sPersist.u16VsPanId;

    if (sZllState.u16FreeAddrLow == 0)
    {
        sTarget.u16NwkAddr = RND_u32GetRand(1, 0xfff6);
        sNwkJoinRouterReqPayload.u16NwkAddr = sTarget.u16NwkAddr;
    }
    else
    {
        sTarget.u16NwkAddr = sZllState.u16FreeAddrLow;
        sNwkJoinRouterReqPayload.u16NwkAddr = sZllState.u16FreeAddrLow++;
        if (psScanTarget->sScanDetails.sScanRspPayload.u8ZllInfo & TL_ADDR_ASSIGN) {
#ifdef USE_LOG
            vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Assign addresses\n");
#endif

            sNwkJoinRouterReqPayload.u16FreeNwkAddrBegin = ((sZllState.u16FreeAddrLow+sZllState.u16FreeAddrHigh-1) >> 1);
            sNwkJoinRouterReqPayload.u16FreeNwkAddrEnd = sZllState.u16FreeAddrHigh;
            sZllState.u16FreeAddrHigh = sNwkJoinRouterReqPayload.u16FreeNwkAddrBegin - 1;

            sNwkJoinRouterReqPayload.u16FreeGroupIdBegin = ((sZllState.u16FreeGroupLow+sZllState.u16FreeGroupHigh-1) >> 1);
            sNwkJoinRouterReqPayload.u16FreeGroupIdEnd = sZllState.u16FreeGroupHigh;
            sZllState.u16FreeGroupHigh = sNwkJoinRouterReqPayload.u16FreeGroupIdBegin - 1;
        }
    }
    if (psScanTarget->sScanDetails.sScanRspPayload.u8GroupIdCount)
    {
        /* allocate count group ids */
        sNwkJoinRouterReqPayload.u16GroupIdBegin = sZllState.u16FreeGroupLow;
        sNwkJoinRouterReqPayload.u16GroupIdEnd = sZllState.u16FreeGroupLow + psScanTarget->sScanDetails.sScanRspPayload.u8GroupIdCount-1;
        sZllState.u16FreeGroupLow += psScanTarget->sScanDetails.sScanRspPayload.u8GroupIdCount;
    }

#ifdef USE_LOG
    vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Give it addr %04x\n", sNwkJoinRouterReqPayload.u16NwkAddr);
#endif
    /* rest of params zero */
    ZPS_vNwkNibSetChannel( pvNwk, psScanTarget->sScanDetails.sScanRspPayload.u8LogicalChannel);

    sDstAddr.eMode = 3;
    sDstAddr.u16PanId = 0xffff;
    sDstAddr.uAddress.u64Addr = psScanTarget->sScanDetails.sSrcAddr.uAddress.u64Addr;

#ifdef USE_LOG
    vLog_Printf(TRACE_COMMISSION,LOG_DEBUG, "Send rtr join on Ch %d for ch %d\n",psScanTarget->sScanDetails.sScanRspPayload.u8LogicalChannel, sZllState.u8MyChannel);
#endif
    eCLD_ZllCommissionCommandNetworkJoinRouterReqCommandSend( &sDstAddr,
                                                             &u8Seq,
                                                             &sNwkJoinRouterReqPayload  );
}

/****************************************************************************
 *
 * NAME:
 *
 * DESCRIPTION:
 *
 * RETURNS:
 *
 ****************************************************************************/
PRIVATE void vTlSendEndDeviceJoinRequest( void *pvNwk,
                                        ZPS_tsNwkNib *psNib,
                                        tsZllScanTarget *psScanTarget,
                                        uint32 u32TransactionId)
{
    ZPS_tsInterPanAddress       sDstAddr;
    tsCLD_ZllCommission_NetworkJoinEndDeviceReqCommandPayload      sNwkJoinEndDeviceReqPayload;
    uint16 u16KeyMask;
    uint8 u8Seq;

    memset(&sNwkJoinEndDeviceReqPayload, 0, sizeof(tsCLD_ZllCommission_NetworkJoinEndDeviceReqCommandPayload));
    sNwkJoinEndDeviceReqPayload.u32TransactionId = sCommission.u32TransactionId;

    u16KeyMask = TL_SUPPORTED_KEYS & psScanTarget->sScanDetails.sScanRspPayload.u16KeyMask;
    if (u16KeyMask & TL_MASTER_KEY_MASK)
    {
        sNwkJoinEndDeviceReqPayload.u8KeyIndex = TL_MASTER_KEY_INDEX;
    }
    else if (u16KeyMask & TL_CERTIFICATION_KEY_MASK)
    {
        sNwkJoinEndDeviceReqPayload.u8KeyIndex = TL_CERTIFICATION_KEY_INDEX;
    }
    else
    {
        sNwkJoinEndDeviceReqPayload.u8KeyIndex = TL_TEST_KEY_INDEX;
    }


    BDB_u8TlEncryptKey( psNib->sTbl.psSecMatSet[0].au8Key,
                        sNwkJoinEndDeviceReqPayload.au8NwkKey,
                        u32TransactionId,
                        psScanTarget->sScanDetails.sScanRspPayload.u32ResponseId,
                        sNwkJoinEndDeviceReqPayload.u8KeyIndex);



    sNwkJoinEndDeviceReqPayload.u64ExtPanId = psNib->sPersist.u64ExtPanId;
    sNwkJoinEndDeviceReqPayload.u8NwkUpdateId = psNib->sPersist.u8UpdateId;
    sNwkJoinEndDeviceReqPayload.u8LogicalChannel = sZllState.u8MyChannel;
    sNwkJoinEndDeviceReqPayload.u16PanId = psNib->sPersist.u16VsPanId;

    /* todo cant assign random addresses */
    if (sZllState.u16FreeAddrLow == 0) {
        sTarget.u16NwkAddr = RND_u32GetRand(1, 0xfff6);;
        sNwkJoinEndDeviceReqPayload.u16NwkAddr = sTarget.u16NwkAddr;
    } else  {
        sTarget.u16NwkAddr = sZllState.u16FreeAddrLow;
        sNwkJoinEndDeviceReqPayload.u16NwkAddr = sZllState.u16FreeAddrLow++;
        if (psScanTarget->sScanDetails.sScanRspPayload.u8ZllInfo & TL_ADDR_ASSIGN)
        {
#ifdef USE_LOG
            vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Assign addresses\n");
#endif

            sNwkJoinEndDeviceReqPayload.u16FreeNwkAddrBegin = ((sZllState.u16FreeAddrLow+sZllState.u16FreeAddrHigh-1) >> 1);
            sNwkJoinEndDeviceReqPayload.u16FreeNwkAddrEnd = sZllState.u16FreeAddrHigh;
            sZllState.u16FreeAddrHigh = sNwkJoinEndDeviceReqPayload.u16FreeNwkAddrBegin - 1;

            sNwkJoinEndDeviceReqPayload.u16FreeGroupIdBegin = ((sZllState.u16FreeGroupLow+sZllState.u16FreeGroupHigh-1) >> 1);
            sNwkJoinEndDeviceReqPayload.u16FreeGroupIdEnd = sZllState.u16FreeGroupHigh;
            sZllState.u16FreeGroupHigh = sNwkJoinEndDeviceReqPayload.u16FreeGroupIdBegin - 1;
        }
    }
#ifdef USE_LOG
    vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Give it addr %04x\n", sNwkJoinEndDeviceReqPayload.u16NwkAddr);
#endif
    if (psScanTarget->sScanDetails.sScanRspPayload.u8GroupIdCount)
    {
        /* allocate count group ids */
        sNwkJoinEndDeviceReqPayload.u16GroupIdBegin = sZllState.u16FreeGroupLow;
        sNwkJoinEndDeviceReqPayload.u16GroupIdEnd = sZllState.u16FreeGroupLow + psScanTarget->sScanDetails.sScanRspPayload.u8GroupIdCount-1;
        sZllState.u16FreeGroupLow += psScanTarget->sScanDetails.sScanRspPayload.u8GroupIdCount;
    }

    ZPS_vNwkNibSetChannel( pvNwk, psScanTarget->sScanDetails.sScanRspPayload.u8LogicalChannel);

    sDstAddr.eMode = 3;
    sDstAddr.u16PanId = 0xffff;
    sDstAddr.uAddress.u64Addr = psScanTarget->sScanDetails.sSrcAddr.uAddress.u64Addr;

#ifdef USE_LOG
    vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Send zed join on Ch %d for ch %d\n", psScanTarget->sScanDetails.sScanRspPayload.u8LogicalChannel,

                                                                    sZllState.u8MyChannel);
#endif
    eCLD_ZllCommissionCommandNetworkJoinEndDeviceReqCommandSend( &sDstAddr,
                                                                 &u8Seq,
                                                                 &sNwkJoinEndDeviceReqPayload  );
}

/****************************************************************************
 *
 * NAME:
 *
 * DESCRIPTION:
 *
 * RETURNS:
 *
 ****************************************************************************/
PRIVATE void vTlHandleNwkStartResponse( void * pvNwk,
                                             tsCLD_ZllCommission_NetworkStartRspCommandPayload *psNwkStartRsp,
                                             tsCommissionData *psCommission,
                                             ZPS_tsInterPanAddress       *psSrcAddr)
{
    ZPS_tsAftsStartParamsDistributed sNwkParams;
    uint8 u8Status;

    if ( (psNwkStartRsp->u8Status == TL_SUCCESS) &&
         (psCommission->u32TransactionId == psNwkStartRsp->u32TransactionId) &&
         (psSrcAddr->uAddress.u64Addr == sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr))
    {
        sNwkParams.u64ExtPanId = psNwkStartRsp->u64ExtPanId;
        sNwkParams.pu8NwkKey = au8TempKeyStore;
        sNwkParams.u16PanId = psNwkStartRsp->u16PanId;
        sNwkParams.u16NwkAddr = sZllState.u16MyAddr;
        sNwkParams.u8KeyIndex = 0;
        sNwkParams.u8LogicalChannel = psNwkStartRsp->u8LogicalChannel;
        sZllState.u8MyChannel = sNwkParams.u8LogicalChannel;
//        DBG_vPrintf(TRUE, "Set MyChannel to %d at line %d\n", sZllState.u8MyChannel, __LINE__);
        sNwkParams.u8NwkupdateId = psNwkStartRsp->u8NwkUpdateId;

        u8Status = ZPS_eAplFormDistributedNetworkRouter( &sNwkParams, TRUE);
        if (u8Status)
        {
            DBG_vPrintf(TRACE_JOIN, "Failed to start router %02x\n", u8Status);
        }

        sZllState.u8MyChannel = psNwkStartRsp->u8LogicalChannel;
//        DBG_vPrintf(TRUE, "Set MyChannel to %d at line %d\n", sZllState.u8MyChannel, __LINE__);
        /* Set channel maskl to primary channels */
        ZTIMER_eStop(u8TimerBdbTl);
        ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(TL_START_UP_TIME_SEC));
#ifdef USE_LOG
        vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Start on Ch %d\n", sZllState.u8MyChannel);
#endif
        psCommission->eState = E_WAIT_START_UP;
    }
}

/****************************************************************************
 *
 * NAME:
 *
 * DESCRIPTION:
 *
 * RETURNS:
 *
 ****************************************************************************/
PRIVATE void vTlHandleNwkStartRequest( ZPS_tsInterPanAddress  *psDstAddr,
                                            tsCLD_ZllCommission_NetworkStartReqCommandPayload          *psNwkStartReq,
                                            tsCommissionData *psCommission,
                                            ZPS_tsInterPanAddress       *psSrcAddr)
{


    if ( ( psNwkStartReq->u32TransactionId != sCommission.u32TransactionId) ||
       ( psNwkStartReq->u64InitiatorIEEEAddr != psSrcAddr->uAddress.u64Addr) ||
         (psNwkStartReq->u16NwkAddr == 0) ||
         (psNwkStartReq->u16NwkAddr > 0xfff6 ) ||
         ((psNwkStartReq->u8LogicalChannel > 0) && (psNwkStartReq->u8LogicalChannel < 11)) ||
         (psNwkStartReq->u8LogicalChannel > 26) ||
         ( FALSE == BDB_bTlIsKeySupported( psNwkStartReq->u8KeyIndex))
       )
    {
        DBG_vPrintf(TRACE_TL_NEGATIVE, "Start Req wrong Params\n");
        /* drop silently */
        return;
    } else if (( sBDB.sAttrib.bTLStealNotAllowed == TRUE) && (sZllState.eState == NOT_FACTORY_NEW))
    {
        /* send start rsp */
        tsCLD_ZllCommission_NetworkStartRspCommandPayload sNwkStartRsp;
        uint8 u8Seq;

        DBG_vPrintf(TRACE_TL_NEGATIVE, "Start Req No Stealing\n");
        sNwkStartRsp.u32TransactionId = sCommission.u32TransactionId;
        sNwkStartRsp.u8Status = TL_ERROR;
        sNwkStartRsp.u8NwkUpdateId = 0;
        sNwkStartRsp.u64ExtPanId = 0;
        sNwkStartRsp.u8LogicalChannel = 0;
        sNwkStartRsp.u16PanId = 0;
        eCLD_ZllCommissionCommandNetworkStartRspCommandSend( &sDstAddr,
                                                             &u8Seq,
                                                             &sNwkStartRsp);
    }
    else
    {
        sStartParams.sNwkParams.u64ExtPanId = psNwkStartReq->u64ExtPanId;
       sStartParams.sNwkParams.u8KeyIndex = psNwkStartReq->u8KeyIndex;
       sStartParams.sNwkParams.u8LogicalChannel = psNwkStartReq->u8LogicalChannel;
       sStartParams.sNwkParams.u16PanId = psNwkStartReq->u16PanId;
       sStartParams.sNwkParams.u16NwkAddr  = psNwkStartReq->u16NwkAddr;
       sStartParams.u16GroupIdBegin = psNwkStartReq->u16GroupIdBegin;
       sStartParams.u16GroupIdEnd = psNwkStartReq->u16GroupIdEnd;
       sStartParams.u16FreeGroupIdBegin = psNwkStartReq->u16FreeGroupIdBegin;
       sStartParams.u16FreeGroupIdEnd = psNwkStartReq->u16FreeGroupIdEnd;
       sStartParams.u16FreeNwkAddrBegin = psNwkStartReq->u16FreeNwkAddrBegin;
       sStartParams.u16FreeNwkAddrEnd  = psNwkStartReq->u16FreeNwkAddrEnd;
       sStartParams.u64InitiatorIEEEAddr = psNwkStartReq->u64InitiatorIEEEAddr;
       sStartParams.u16InitiatorNwkAddr = psNwkStartReq->u16InitiatorNwkAddr;
       sStartParams.sNwkParams.u8NwkupdateId = 1;
       sStartParams.sNwkParams.pu8NwkKey = sStartParams.au8NwkKey;
       memcpy( sStartParams.au8NwkKey,
               psNwkStartReq->au8NwkKey,
               16);

       /* Turn up Tx power */
#if ADJUST_POWER
       eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, TX_POWER_NORMAL);
#endif
       if ((sStartParams.sNwkParams.u64ExtPanId == 0) || (sStartParams.sNwkParams.u16PanId == 0))
       {
           if (sStartParams.sNwkParams.u64ExtPanId == 0)
           {
               sStartParams.sNwkParams.u64ExtPanId = RND_u32GetRand(1, 0xffffffff);
               sStartParams.sNwkParams.u64ExtPanId <<= 32;
               sStartParams.sNwkParams.u64ExtPanId |= RND_u32GetRand(0, 0xffffffff);
               DBG_vPrintf(TRACE_COMMISSION, "Gen Epid\n");
           }
           if (sStartParams.sNwkParams.u16PanId == 0)
           {
               sStartParams.sNwkParams.u16PanId = RND_u32GetRand( 1, 0xfffe);
               DBG_vPrintf(TRACE_COMMISSION, "Gen pan\n");
           }
           DBG_vPrintf(TRACE_COMMISSION, "Do discovery\n");
           ZPS_eAplZdoDiscoverNetworks( BDBC_TL_PRIMARY_CHANNEL_SET | BDBC_TL_SECONDARY_CHANNEL_SET);
           sCommission.eState = E_WAIT_DISCOVERY;
           ZTIMER_eStop(u8TimerBdbTl);
           ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(650));
       }
       else
       {
           sCommission.eState = E_SKIP_DISCOVERY;
           DBG_vPrintf(TRACE_COMMISSION, "Skip discovery\n");
           ZTIMER_eStop(u8TimerBdbTl);
           ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
       }
    }
}

/****************************************************************************
 *
 * NAME:
 *
 * DESCRIPTION:
 *
 * RETURNS:
 *
 ****************************************************************************/
PRIVATE void vTlHandleRouterJoinRequest( void * pvNwk,
        ZPS_tsNwkNib *psNib,
        tsCLD_ZllCommission_NetworkJoinRouterReqCommandPayload   *psRouterJoinReq,
        tsCommissionData *psCommission,
        ZPS_tsInterPanAddress       *psSrcAddr)
{
    tsCLD_ZllCommission_NetworkJoinRouterRspCommandPayload      sNwkJoinRouterRsp;
    ZPS_tsInterPanAddress       sDstAddr;
    uint8  u8Seq;

    sNwkJoinRouterRsp.u32TransactionId = sCommission.u32TransactionId;
    sNwkJoinRouterRsp.u8Status  = TL_SUCCESS;
    sDstAddr = *psSrcAddr;
    sDstAddr.u16PanId = 0xFFFF;
#if ADJUST_POWER
    eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, TX_POWER_NORMAL);
#endif

#ifdef USE_LOG
    vLog_Printf(TRACE_JOIN,LOG_DEBUG, "Network Join Router request ExtPanId=%016llx Ch=%d\n", psRouterJoinReq->u64ExtPanId, psRouterJoinReq->u8LogicalChannel);
#endif

    if ( (psRouterJoinReq->u32TransactionId != sCommission.u32TransactionId) ||
         (psRouterJoinReq->u64ExtPanId == 0) ||
         (psRouterJoinReq->u64ExtPanId == 0xffffffffff) ||
         (psRouterJoinReq->u8LogicalChannel < 11) ||
         (psRouterJoinReq->u8LogicalChannel > 26) ||
         ( FALSE == BDB_bTlIsKeySupported(psRouterJoinReq->u8KeyIndex))
        )
    {
        DBG_vPrintf( TRACE_COMMISSION, "Invalid start params reject\n");
        return;
    }
    else if (( sBDB.sAttrib.bTLStealNotAllowed == TRUE) && (sZllState.eState == NOT_FACTORY_NEW))
    {
        sNwkJoinRouterRsp.u8Status  = TL_ERROR;
    }
#ifdef USE_LOG
    vLog_Printf(TRACE_JOIN,LOG_DEBUG, "A\n");
#endif
    eCLD_ZllCommissionCommandNetworkJoinRouterRspCommandSend( &sDstAddr,
                                                                      &u8Seq,
                                                                      &sNwkJoinRouterRsp);
#ifdef USE_LOG
    vLog_Printf(TRACE_JOIN,LOG_DEBUG, "B\n");
#endif

    if (sNwkJoinRouterRsp.u8Status  == TL_SUCCESS)
    {

#ifdef USE_LOG
    vLog_Printf(TRACE_JOIN,LOG_DEBUG, "C\n");
#endif


        sStartParams.sNwkParams.u64ExtPanId  = psRouterJoinReq->u64ExtPanId;
        sStartParams.sNwkParams.u8KeyIndex = psRouterJoinReq->u8KeyIndex;
        sStartParams.sNwkParams.u8LogicalChannel = psRouterJoinReq->u8LogicalChannel;
        sStartParams.sNwkParams.u16PanId  = psRouterJoinReq->u16PanId;
        sStartParams.sNwkParams.u16NwkAddr = psRouterJoinReq->u16NwkAddr;
        sStartParams.u16GroupIdBegin = psRouterJoinReq->u16GroupIdBegin;
        sStartParams.u16GroupIdEnd = psRouterJoinReq->u16GroupIdEnd;
        sStartParams.u16FreeGroupIdBegin = psRouterJoinReq->u16FreeGroupIdBegin;
        sStartParams.u16FreeGroupIdEnd = psRouterJoinReq->u16FreeGroupIdEnd;
        sStartParams.u16FreeNwkAddrBegin = psRouterJoinReq->u16FreeNwkAddrBegin;
        sStartParams.u16FreeNwkAddrEnd = psRouterJoinReq->u16FreeNwkAddrEnd;
        sStartParams.u64InitiatorIEEEAddr = 0;
        sStartParams.u16InitiatorNwkAddr = 0;
        sStartParams.sNwkParams.u8NwkupdateId = psRouterJoinReq->u8NwkUpdateId;
        sStartParams.sNwkParams.pu8NwkKey = sStartParams.au8NwkKey;

        memcpy( sStartParams.au8NwkKey,
                psRouterJoinReq->au8NwkKey,
                16);

        if (sZllState.eState == FACTORY_NEW)
        {
#ifdef USE_LOG
    vLog_Printf(TRACE_JOIN,LOG_DEBUG, "D\n");
#endif
            sCommission.eState = E_START_ROUTER;
        }
        else
        {
#ifdef USE_LOG
    vLog_Printf(TRACE_JOIN,LOG_DEBUG, "E\n");
#endif
            sCommission.eState = E_WAIT_LEAVE;
            /* leave req */
            ZPS_eAplZdoLeaveNetwork(0, FALSE, FALSE);
            sBDB.sAttrib.bLeaveRequested = TRUE;
        }
        ZTIMER_eStop(u8TimerBdbTl);
        ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
    }
}

/****************************************************************************
 *
 * NAME:
 *
 * DESCRIPTION:
 *
 * RETURNS:
 *
 ****************************************************************************/
PRIVATE void vTlHandleNwkUpdateRequest(void * pvNwk,
                                     ZPS_tsNwkNib *psNib,
                                     tsCLD_ZllCommission_NetworkUpdateReqCommandPayload *psNwkUpdateReqPayload)
{

    if ( (psNwkUpdateReqPayload->u32TransactionId == sCommission.u32TransactionId) &&
         (psNwkUpdateReqPayload->u64ExtPanId == psNib->sPersist.u64ExtPanId) &&
         (psNwkUpdateReqPayload->u16PanId == psNib->sPersist.u16VsPanId) &&
         (psNib->sPersist.u8UpdateId != BDB_u8TlNewUpdateID( psNib->sPersist.u8UpdateId,
                                                              psNwkUpdateReqPayload->u8NwkUpdateId)))
    {
        /* Update the UpdateId, Nwk addr and Channel */
        void * pvNwk = ZPS_pvAplZdoGetNwkHandle();
        ZPS_vNwkNibSetUpdateId( pvNwk, psNwkUpdateReqPayload->u8NwkUpdateId);
            ZPS_vNwkNibSetNwkAddr( pvNwk, psNwkUpdateReqPayload->u16NwkAddr);
        ZPS_vNwkNibSetChannel( pvNwk, psNwkUpdateReqPayload->u8LogicalChannel);
        DBG_vPrintf(TRACE_COMMISSION, "Nwk update to %d\n", psNwkUpdateReqPayload->u8LogicalChannel);
    }
}

PRIVATE bool_t bSearchDiscNt(ZPS_tsNwkNib *psNib, uint64 u64EpId, uint16 u16PanId) {

    int i;
    for (i = 0; i < psNib->sTblSize.u8NtDisc; i++)
    {
        if ((psNib->sTbl.psNtDisc[i].u64ExtPanId == u64EpId)
                || (psNib->sTbl.psNtDisc[i].u16PanId == u16PanId))
        {
            return FALSE;
        }
    }
    return TRUE;
}


/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
