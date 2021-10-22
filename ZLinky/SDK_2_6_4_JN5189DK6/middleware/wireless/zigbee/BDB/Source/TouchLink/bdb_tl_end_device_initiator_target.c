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
# COMPONENT:   bdb_tl_end_device_initiator_target.c
#
# DESCRIPTION: BDB Touchlink implementation of End Device as Initiator/Target
#              
#
###############################################################################*/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
#include "bdb_api.h"
#include "bdb_tl.h"

#include <string.h>
#include <stdlib.h>
#include <AppApi.h>
#include "pdum_apl.h"
#include "pdum_gen.h"
#include "PDM.h"
#include "dbg.h"
#include "pwrm.h"
#include "zps_apl_af.h"
#include "zps_apl_zdo.h"
#include "zps_apl_aib.h"
#include "zps_apl_zdp.h"
#include "zll_commission.h"



#include "app_main.h"

#include "app_led_control.h"
#include "zlo_controller_node.h"

#include "app_events.h"
#include "zcl_customcommand.h"
#include "mac_sap.h"
#include "ZTimer.h"
#include <rnd_pub.h>
#include <mac_pib.h>
#include <string.h>
#include <stdlib.h>

// todo deferal rules for 2 initiators


#include "PDM_IDs.h"

#ifndef DEBUG_SCAN
#define TRACE_SCAN            FALSE
#else
#define TRACE_SCAN            TRUE
#endif

#ifndef DEBUG_JOIN
#define TRACE_JOIN            FALSE
#else
#define TRACE_JOIN            TRUE
#endif

#ifndef DEBUG_COMMISSION
#define TRACE_COMMISSION      FALSE
#else
#define TRACE_COMMISSION      TRUE
#endif

#ifndef DEBUG_SCAN_REQ
#define TRACE_SCAN_REQ        FALSE
#else
TRACE_SCAN_REQ                TRUE
#endif

#ifndef DEBUG_TL_NEGATIVE
#define TRACE_TL_NEGATIVE   FALSE
#else
#define TRACE_TL_NEGATIVE   TRUEE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define ADJUST_POWER        TRUE
#define TL_SCAN_LQI_MIN    (110)
#define RSSI_CORRECTION    (10)


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
    E_SCAN_WAIT_RESET_SENT,
    E_WAIT_START_RSP,
    E_WAIT_JOIN_RTR_RSP,
    E_WAIT_JOIN_ZED_RSP,
    E_WAIT_LEAVE,
    E_WAIT_LEAVE_RESET,
    E_WAIT_START_UP,
    E_ACTIVE
}eState;

typedef struct {
    eState eState;
    uint8 u8Count;
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
PRIVATE void vTlSendScanResponse( ZPS_tsNwkNib *psNib,
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

PRIVATE void vTlSendStartRequest( void *pvNwk,
                                ZPS_tsNwkNib *psNib,
                                tsZllScanTarget *psScanTarget,
                                uint32 u32TransactionId);
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
PRIVATE void vTlHandleNwkUpdateRequest( void * pvNwk,
                                      ZPS_tsNwkNib *psNib,
                                      tsCLD_ZllCommission_NetworkUpdateReqCommandPayload *psNwkUpdateReqPayload);

PRIVATE void vTlHandleDeviceInfoRequest( tsCLD_ZllCommission_DeviceInfoReqCommandPayload *psDeviceInfoReq,
                                              ZPS_tsInterPanAddress       *psSrcAddr);
PRIVATE void vTlHandleEndDeviceJoinRequest( void * pvNwk,
                                                 ZPS_tsNwkNib *psNib,
                                                 tsCLD_ZllCommission_NetworkJoinEndDeviceReqCommandPayload   *psNwkJoinEndDeviceReq,
                                                 ZPS_tsInterPanAddress       *psSrcAddr,
                                                 tsCommissionData *psCommission);
PRIVATE void vTlHandleNwkStartResponse( void * pvNwk,
                                             tsCLD_ZllCommission_NetworkStartRspCommandPayload *psNwkStartRsp,
                                             tsCommissionData *psCommission,
                                             ZPS_tsInterPanAddress       *psSrcAddr);
PRIVATE void vTlSendIdentifyRequest(uint64 u64Addr, uint32 u32TransactionId, uint8 u8Time);
PRIVATE void vTlEndCommissioning( void* pvNwk, eState eState, uint16 u16TimeMS);

PRIVATE void vTlSaveTempAddresses( void);
PRIVATE void vTlRecoverTempAddresses( void);
PRIVATE void vTlSendDeviceInfoReq(uint64 u64Addr, uint32 u32TransactionId, uint8 u8Index);
PRIVATE void vTlSetUpParent( ZPS_tsNwkNib *psNib, uint16 u16Addr);


/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/


extern tsZllRemoteState sZllState;
extern tsCLD_ZllDeviceTable sDeviceTable;
extern bool_t bFailedToJoin;

APP_tsEventTouchLink        sTarget;
ZPS_tsInterPanAddress       sDstAddr;

uint64 au64IgnoreList[3];
tsZllScanTarget sScanTarget;

uint8 au8TempKeyStore[16];
PUBLIC uint8 u8TimerBdbTl;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
tsCommissionData sCommission;

/* to hold address ranges over touchlink in case they need recovering */
uint16 u16TempAddrLow;
uint16 u16TempAddrHigh;
uint16 u16TempGroupLow;
uint16 u16TempGroupHigh;
bool_t bDoRejoin = FALSE;
bool_t bWithDiscovery = FALSE;
//PUBLIC bool_t bTLinkInProgress = FALSE;
PUBLIC bool_t bSendFactoryResetOverAir=FALSE;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
extern void vRemoveLight(uint16 u16Addr);;
/****************************************************************************
 *
 * NAME: 
 *
 * DESCRIPTION:
 *
 * PARAMETERS:      Name            RW  Usage
 *
 * RETURNS:
 *
 * NOTES:
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
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
#define RCVD_DEST_ADDR ( ((tsCLD_ZllCommissionCustomDataStructure*)psEvent-> psCallBackEvent->psClusterInstance->pvEndPointCustomStructPtr)->sRxInterPanAddr.sDstAddr )

PUBLIC void BDB_vTlStateMachine( tsBDB_ZCLEvent *psEvent)
{
    APP_tsEvent                   sAppEvent;
     ZPS_tsInterPanAddress       sSrcAddr;
     uint32 u32LogicalChannel;
     uint8 u8Lqi = 0;

     //DBG_vPrintf(1, "TL SM event %d state %d\r\n", psEvent->eType, sCommission.eState );

    tsCLD_ZllCommissionCallBackMessage *psZllMessage = NULL;
    if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG)
    {
        /* get deatil of inter pan message */
        psZllMessage = &((tsCLD_ZllCommissionCustomDataStructure*)psEvent-> psCallBackEvent->psClusterInstance->pvEndPointCustomStructPtr)->sCallBackMessage;
        sSrcAddr = ((tsCLD_ZllCommissionCustomDataStructure*)psEvent-> psCallBackEvent->psClusterInstance->pvEndPointCustomStructPtr)->sRxInterPanAddr.sSrcAddr;
        u8Lqi = psEvent->psCallBackEvent->pZPSevent->uEvent.sApsInterPanDataIndEvent.u8LinkQuality;

        if (psZllMessage->uMessage.psScanReqPayload->u32TransactionId == 0)
        {
            /* illegal transaction id */
            DBG_vPrintf(TRACE_TL_NEGATIVE, "REJECT: transaction Id 0\r\n");
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
                    DBG_vPrintf(TRACE_TL_NEGATIVE, "Drop IP on broadcast\r\n");
                    return;
                }
                break;

        default:
            // unknown command id, dro it
            return;
        }; // end of sweitch
    }


    DBG_vPrintf(TRACE_COMMISSION, "Commission task state %d type %d\r\n", sCommission.eState, psEvent->eType);
    void *pvNwk = ZPS_pvAplZdoGetNwkHandle();
    ZPS_tsNwkNib *psNib = ZPS_psNwkNibGetHandle( pvNwk);
    switch (sCommission.eState)
    {
        case E_IDLE:
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_START)
            {
                //bTLinkInProgress = TRUE;
                au64IgnoreList[0] = 0;
                au64IgnoreList[1] = 0;
                au64IgnoreList[2] = 0;
                sCommission.u8Count = 0;
                sScanTarget.u16LQI = 0;
                ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
                sCommission.eState = E_SCANNING;
                sCommission.u32TransactionId = RND_u32GetRand(1, 0xffffffff);
                DBG_vPrintf(TRACE_COMMISSION, "\r\nRxIdle True");

                MAC_vPibSetRxOnWhenIdle( NULL, TRUE, FALSE);

                sCommission.bResponded = FALSE;
               /* if device has been scanning to rejoin, due to parent removed, don't update channel */
               if(sBDB.sAttrib.ebdbCommissioningStatus != E_BDB_COMMISSIONING_STATUS_IN_PROGRESS)
               {
                   eAppApiPlmeGet(PHY_PIB_ATTR_CURRENT_CHANNEL, &u32LogicalChannel);
                   if (u32LogicalChannel != sZllState.u8MyChannel)
                   {
                       sZllState.u8MyChannel = u32LogicalChannel;
                       PDM_eSaveRecordData(PDM_ID_APP_ZLO_CONTROLLER, &sZllState,sizeof(tsZllRemoteState));
                       DBG_vPrintf(1, "Update Channel to %d\r\n", sZllState.u8MyChannel);
                   }
                }

                sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_IN_PROGRESS;
                //sBDB.sAttrib.u32bdbPrimaryChannelSet = BDBC_TL_PRIMARY_CHANNEL_SET;
                //sBDB.sAttrib.u32bdbSecondaryChannelSet = BDBC_TL_SECONDARY_CHANNEL_SET;
                sCommission.u32ScanChannels = BDBC_TL_PRIMARY_CHANNEL_SET;    // sBDB.sAttrib.u32bdbPrimaryChannelSet;
                sCommission.bIsFirstChannel = TRUE;
                sCommission.bDoPrimaryScan = TRUE;
                DBG_vPrintf(TRACE_SCAN_REQ, "Start scan Prim %08x Second %08x\r\n",BDBC_TL_PRIMARY_CHANNEL_SET ,
                        BDBC_TL_SECONDARY_CHANNEL_SET );


                /* Turn down Tx power */
#if ADJUST_POWER
                eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, TX_POWER_LOW);
#endif



            } else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG ) {
                if (psZllMessage->u8CommandId == E_CLD_COMMISSION_CMD_SCAN_REQ )
                {
                    if ((u8Lqi < TL_SCAN_LQI_MIN) || ( (psZllMessage->uMessage.psScanReqPayload->u8ZllInfo & TL_LINK_INIT) == 0 ))
                    {
                        DBG_vPrintf(TRACE_COMMISSION, "\r\nIgnore scan reqXX");
                        return;

                    }

jumpHere:
                    DBG_vPrintf(TRACE_SCAN, "Got Scan Req\r\n");
                    /* Turn down Tx power */
#if ADJUST_POWER
                    eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, TX_POWER_LOW);
#endif


                    sDstAddr = sSrcAddr;
                    sDstAddr.u16PanId = 0xFFFF;

                    sCommission.u32ResponseId = RND_u32GetRand(1, 0xffffffff);
                    sCommission.u32TransactionId = psZllMessage->uMessage.psScanReqPayload->u32TransactionId;
                    sCommission.bProfileInterOp =
                            (psZllMessage->uMessage.psScanReqPayload->u8ZllInfo & TL_PROFILE_INTEROP) ? TRUE : FALSE;

                    vTlSendScanResponse( psNib,
                                       &sDstAddr,
                                       sCommission.u32TransactionId,
                                       sCommission.u32ResponseId);

                    sCommission.eState = E_ACTIVE;
                    DBG_vPrintf(1, "GO ACTIVE\r\n");
                    /* Timer to end inter pan */
                    ZTIMER_eStop(u8TimerBdbTl);
                    ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(TL_INTERPAN_LIFE_TIME_SEC));
                }
            }
            break;

        case E_SCANNING:
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED) {
                /*
                 * Send the next scan request
                 */
                vTlSendScanRequest(pvNwk, &sCommission);
            } else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG) {
                switch (psZllMessage->u8CommandId )
                {
                case E_CLD_COMMISSION_CMD_SCAN_RSP:

                    if (psZllMessage->uMessage.psScanRspPayload->u32TransactionId == sCommission.u32TransactionId)
                    {
                        vTlHandleScanResponse( pvNwk,
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
                    if ((u8Lqi < TL_SCAN_LQI_MIN) || ( (psZllMessage->uMessage.psScanReqPayload->u8ZllInfo & TL_LINK_INIT) == 0 ))
                    {
                       DBG_vPrintf(TRACE_COMMISSION, "\r\nIgnore scan req");
                       return;
                    }
                    DBG_vPrintf(TRACE_COMMISSION, "Scan Req1\r\n");
                    if ( (sZllState.eState == FACTORY_NEW_REMOTE) &&
                         ( ((psZllMessage->uMessage.psScanReqPayload->u8ZllInfo & TL_FACTORY_NEW) == 0) &&
                        (   ((psZllMessage->uMessage.psScanReqPayload->u8ZigbeeInfo & TL_TYPE_MASK) == TL_ZED) ||
                           ((psZllMessage->uMessage.psScanReqPayload->u8ZigbeeInfo & TL_TYPE_MASK) == TL_ROUTER)  )
                         )
                       )
                    {

                        DBG_vPrintf(TRACE_COMMISSION|1, "Scan Req while scanning\r\n");
                        ZTIMER_eStop(u8TimerBdbTl);
                        goto jumpHere;
                    }
#if 0
// if not going to be a target dont respond
                    else
                    {
                        if ( !sCommission.bResponded) {
                            /*
                             * Only respond once
                             */
                            sCommission.bResponded = TRUE;

                            sDstAddr = sSrcAddr;
                            sDstAddr.u16PanId = 0xFFFF;

                            sCommission.u32TheirResponseId = RND_u32GetRand(1, 0xffffffff);
                            sCommission.u32TheirTransactionId = psZllMessage->uMessage.psScanReqPayload->u32TransactionId;

                            sCommission.bProfileInterOp =
                                            (psZllMessage->uMessage.psScanReqPayload->u8ZllInfo & TL_PROFILE_INTEROP) ? TRUE : FALSE;

                            vTlSendScanResponse(psNib,
                                              &sDstAddr,
                                              sCommission.u32TheirTransactionId,
                                              sCommission.u32TheirResponseId);

                        }
                    }
#endif
                    break;

                case E_CLD_COMMISSION_CMD_DEVICE_INFO_REQ:
                    if (psZllMessage->uMessage.psDeviceInfoReqPayload->u32TransactionId == sCommission.u32TheirTransactionId) {
                        vTlHandleDeviceInfoRequest( psZllMessage->uMessage.psDeviceInfoReqPayload,
                                                         &sSrcAddr);
                    }
                    break;

                case E_CLD_COMMISSION_CMD_IDENTIFY_REQ:
                    if (psZllMessage->uMessage.psIdentifyReqPayload->u32TransactionId == sCommission.u32TheirTransactionId) {
                        vHandleIdentifyRequest(psZllMessage->uMessage.psIdentifyReqPayload->u16Duration);
                    }
                    break;

                default:
                    DBG_vPrintf(TRACE_COMMISSION, "Unhandled during scan %02x\r\n", psZllMessage->u8CommandId );
                    break;
                }

            }

            break;

        case E_SCAN_DONE:
            DBG_vPrintf(TRACE_SCAN, "Scan done target lqi %d\r\n", sScanTarget.u16LQI);
            if (sScanTarget.u16LQI )
            {
                /* scan done and we have a target */
                eAppApiPlmeSet(PHY_PIB_ATTR_CURRENT_CHANNEL, sScanTarget.sScanDetails.sScanRspPayload.u8LogicalChannel);
                if ( (sZllState.eState == NOT_FACTORY_NEW_REMOTE) &&
                        ( (sScanTarget.sScanDetails.sScanRspPayload.u8ZllInfo & TL_FACTORY_NEW) == 0) &&
                        ( (sScanTarget.sScanDetails.sScanRspPayload.u8ZigbeeInfo & TL_TYPE_MASK) == TL_ZED ) &&
                        (sScanTarget.sScanDetails.sScanRspPayload.u64ExtPanId == psNib->sPersist.u64ExtPanId)
                        ) 
               {
                    /* On our network, just gather endpoint details */
                    DBG_vPrintf(TRACE_SCAN, "NFN -> NFN ZED On Our nwk\r\n");
                    /* tell the app about the target we just touched with */
                    sTarget.u16NwkAddr = sScanTarget.sScanDetails.sScanRspPayload.u16NwkAddr;
                    sTarget.u8Endpoint = sScanTarget.sScanDetails.sScanRspPayload.u8Endpoint;
                    sTarget.u16ProfileID = sScanTarget.sScanDetails.sScanRspPayload.u16ProfileId;
                    sTarget.u16DeviceID = sScanTarget.sScanDetails.sScanRspPayload.u16DeviceId;
                    sTarget.u8Version = sScanTarget.sScanDetails.sScanRspPayload.u8Version;

                    vTlEndCommissioning(pvNwk, E_INFORM_APP, 6500 );
                    return;
                }
                DBG_vPrintf(TRACE_SCAN, "Send an Id Req to %016llx Id %08x\r\n", sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr, sCommission.u32TransactionId);
                vTlSendIdentifyRequest(sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr, sCommission.u32TransactionId, 3);
                ZTIMER_eStop(u8TimerBdbTl);
                ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(1));
                sCommission.eState = E_SCAN_WAIT_ID;
            }
            else
            {
                DBG_vPrintf(TRACE_SCAN, "scan No results\r\n");
                if ((sCommission.bDoPrimaryScan == FALSE) ||
                        (BDBC_TL_SECONDARY_CHANNEL_SET /*sBDB.sAttrib.u32bdbSecondaryChannelSet*/ == 0))
                {
                    sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_NO_NETWORK;  // todo no scan response
                    vTlEndCommissioning(pvNwk, E_IDLE, 0);
                    DBG_vPrintf(TRACE_SCAN_REQ|1, "ALL No results\r\n");
                    return;
                }
                sCommission.bDoPrimaryScan = FALSE;
                sCommission.u32ScanChannels = BDBC_TL_SECONDARY_CHANNEL_SET /*sBDB.sAttrib.u32bdbSecondaryChannelSet*/;
                sCommission.u8Count = 0;
                sCommission.bIsFirstChannel = FALSE;


                DBG_vPrintf(TRACE_SCAN|TRACE_SCAN_REQ,  "Second scan %08x\r\n", sCommission.u32ScanChannels);
                sCommission.eState = E_SCANNING;
                ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
            }
            break;

        case E_SCAN_WAIT_ID:
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED)
            {
                DBG_vPrintf(TRACE_SCAN, "Wait id time time out\r\n");

                /* send a device info req, start at index 0 */
                vTlSendDeviceInfoReq(sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr, sCommission.u32TransactionId, 0);

                ZTIMER_eStop(u8TimerBdbTl);
                ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(10));
                sCommission.eState = E_SCAN_WAIT_INFO;
            }
            else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_START)
            {
                DBG_vPrintf(TRACE_SCAN, "Got Key during wait id\r\n");
                if ( au64IgnoreList[0] == 0 )
                {
                    DBG_vPrintf(TRACE_SCAN, "1st black list %016llx\r\n", sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr);
                    au64IgnoreList[0] = sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr;
                }
                else if ( au64IgnoreList[1] == 0 )
                {
                    DBG_vPrintf(TRACE_SCAN, "2nd black list %016llx\r\n", sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr);
                    au64IgnoreList[1] = sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr;
                }
                else if ( au64IgnoreList[2] == 0 )
                {
                    DBG_vPrintf(TRACE_SCAN, "3rd black list %016llx\r\n", sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr);
                    au64IgnoreList[2] = sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr;
                }
                else
                {
                    DBG_vPrintf(TRACE_SCAN, "Run out give up\r\n");
                    vTlEndCommissioning(pvNwk, E_IDLE, 0);
                    return;
                }
                /* stop that one identifying */
                vTlSendIdentifyRequest(sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr, sCommission.u32TransactionId, 0);
                ZTIMER_eStop(u8TimerBdbTl);
                ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
                /* restart scanning */
                sCommission.eState = E_SCANNING;

                sCommission.u8Count = 0;
                sScanTarget.u16LQI = 0;
                sCommission.u32TransactionId = RND_u32GetRand(1, 0xffffffff);
                sCommission.bResponded = FALSE;


                //bTLinkInProgress = TRUE;
            }
            break;

        case E_SCAN_WAIT_INFO:

            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED)
            {
                DBG_vPrintf(TRACE_SCAN, "Wait Info time out\r\n");
                vTlEndCommissioning(pvNwk, E_IDLE, 0);
                return;
            }
            else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG)
            {
                DBG_vPrintf(TRACE_SCAN, "Wasit info com msg Cmd=%d  %d\r\n", psZllMessage->u8CommandId, E_CLD_COMMISSION_CMD_DEVICE_INFO_RSP);
                if ( ( psZllMessage->u8CommandId == E_CLD_COMMISSION_CMD_DEVICE_INFO_RSP ) &&
                     ( psZllMessage->uMessage.psDeviceInfoRspPayload->u32TransactionId == sCommission.u32TransactionId  ) )
                {
                    DBG_vPrintf(TRACE_SCAN, "Got Device Info Rsp\r\n");
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

                        sTarget.u16ProfileID = sScanTarget.sScanDetails.sScanRspPayload.u16ProfileId;
                        sTarget.u16DeviceID = sScanTarget.sScanDetails.sScanRspPayload.u16DeviceId;
                        sTarget.u8Endpoint = sScanTarget.sScanDetails.sScanRspPayload.u8Endpoint;
                        sTarget.u8Version = sScanTarget.sScanDetails.sScanRspPayload.u8Version;
                        // nwk addr will be assigned in a bit

                        sDstAddr.eMode = 3;
                        sDstAddr.u16PanId = 0xffff;
                        sDstAddr.uAddress.u64Addr = sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr;

                        DBG_vPrintf(TRACE_SCAN, "Back to %016llx Mode %d\r\n", sDstAddr.uAddress.u64Addr, sDstAddr.eMode);

                        if(bSendFactoryResetOverAir)
                        {
                            bSendFactoryResetOverAir=FALSE;
                            ZTIMER_eStop(u8TimerBdbTl);
                            vTlSendFactoryResetRequest( pvNwk,
                                                             psNib,
                                                             &sScanTarget,
                                                             sCommission.u32TransactionId);
                            sCommission.eState = E_SCAN_WAIT_RESET_SENT;
                            /* wait a bit for message to go, then finish up */
                            ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
                            DBG_vPrintf(TRACE_COMMISSION, "\r\nvSendFactoryResetRequest and going to E_IDLE\r\n");
                        }
                        else if (sZllState.eState == FACTORY_NEW_REMOTE)
                        {
                            // we are FN ZED, target must be a FN or NFN router
                            vTlSendStartRequest(pvNwk,psNib,&sScanTarget,sCommission.u32TransactionId);
                            sCommission.eState = E_WAIT_START_RSP;
                            DBG_vPrintf(TRACE_JOIN, "Sent start req\r\n");
                            ZTIMER_eStop(u8TimerBdbTl);
                            ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(5));
                        }
                        else
                        {
                            DBG_vPrintf(TRACE_SCAN, "NFN -> ??? %02x\r\n", sScanTarget.sScanDetails.sScanRspPayload.u8ZllInfo);
                            if (sScanTarget.sScanDetails.sScanRspPayload.u8ZllInfo & TL_FACTORY_NEW)
                            {
                                /* FN target */
                                if ((sScanTarget.sScanDetails.sScanRspPayload.u8ZigbeeInfo & TL_TYPE_MASK) != TL_ZED)
                                {
                                    DBG_vPrintf(TRACE_SCAN, "NFN -> FN ZR\r\n");
                                    /* router join */
                                    vTlSendRouterJoinRequest(pvNwk, psNib, &sScanTarget, sCommission.u32TransactionId);
                                    sCommission.eState = E_WAIT_JOIN_RTR_RSP;
                                    ZTIMER_eStop(u8TimerBdbTl);
                                    ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(4));

                                }
                                else
                                {
                                    DBG_vPrintf(TRACE_SCAN, "NFN -> FN ZED\r\n");
                                    /* end device join */
                                    vTlSendEndDeviceJoinRequest( pvNwk, psNib, &sScanTarget, sCommission.u32TransactionId);
                                    sCommission.eState = E_WAIT_JOIN_ZED_RSP;
                                    ZTIMER_eStop(u8TimerBdbTl);
                                    ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(2));
                                }
                            }
                            else
                            {
                                /* NFN target, must be ZR on different nwk */

                                DBG_vPrintf(TRACE_COMMISSION|1, "From Pan %016llx\r\n", sScanTarget.sScanDetails.sScanRspPayload.u64ExtPanId);
                                DBG_vPrintf(TRACE_COMMISSION|1, "NFN -> NFN ZR Our Pan %016llx\r\n", psNib->sPersist.u64ExtPanId);
                                if (sScanTarget.sScanDetails.sScanRspPayload.u64ExtPanId == psNib->sPersist.u64ExtPanId)
                                {
                                    DBG_vPrintf(TRACE_SCAN, "On our nwk\r\n"  );
                                    /* On our network, just gather endpoint details */
                                    DBG_vPrintf(TRACE_SCAN, "NFN -> NFN ZR On Our nwk\r\n");
                                    /* tell the app about the target we just touched with */
                                    sTarget.u16NwkAddr = sScanTarget.sScanDetails.sScanRspPayload.u16NwkAddr;
                                    sTarget.u8Endpoint = sScanTarget.sScanDetails.sScanRspPayload.u8Endpoint;
                                    sTarget.u16ProfileID = sScanTarget.sScanDetails.sScanRspPayload.u16ProfileId;
                                    sTarget.u16DeviceID = sScanTarget.sScanDetails.sScanRspPayload.u16DeviceId;
                                    sTarget.u8Version = sScanTarget.sScanDetails.sScanRspPayload.u8Version;

                                    vTlEndCommissioning(pvNwk, E_INFORM_APP, 1500);
                                    return;
                                }
                                if ( ((sScanTarget.sScanDetails.sScanRspPayload.u8ZigbeeInfo & TL_TYPE_MASK) != TL_ZED) )
                                {
                                    DBG_vPrintf(TRACE_SCAN, "NFN -> NFN ZR other nwk\r\n");

                                    vTlSendRouterJoinRequest( pvNwk, psNib, &sScanTarget, sCommission.u32TransactionId);
                                    sCommission.eState = E_WAIT_JOIN_RTR_RSP;
                                }
                                else
                                {
                                    DBG_vPrintf(TRACE_SCAN, "NFN -> NFN ZED other nwk\r\n");

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

        case E_SCAN_WAIT_RESET_SENT:
            /* Factory reset command has been sent,
             * end commissioning
             */
            vTlEndCommissioning(pvNwk, E_IDLE, 0);
            break;

        case E_WAIT_START_RSP:
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG) {
                DBG_vPrintf(TRACE_JOIN, "Start Rsp %d\r\n", psZllMessage->u8CommandId);
                if (psZllMessage->u8CommandId == E_CLD_COMMISSION_CMD_NETWORK_START_RSP) {
                    vTlHandleNwkStartResponse(pvNwk,
                                                   psZllMessage->uMessage.psNwkStartRspPayload,
                                                   &sCommission,
                                                   &sSrcAddr);
                }
            }
            else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED)  {
                DBG_vPrintf(TRACE_COMMISSION, "wait startrsp time out\r\n");

                vTlEndCommissioning(pvNwk, E_IDLE, 0);
            }
             break;

        case E_WAIT_JOIN_RTR_RSP:
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG) {
                if (psZllMessage->u8CommandId == E_CLD_COMMISSION_CMD_NETWORK_JOIN_ROUTER_RSP) {
                    if ( (psZllMessage->uMessage.psNwkJoinRouterRspPayload->u8Status == TL_SUCCESS) &&
                         (sCommission.u32TransactionId == psZllMessage->uMessage.psNwkJoinRouterRspPayload->u32TransactionId) &&
                         (sSrcAddr.uAddress.u64Addr == sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr))
                    {
                        DBG_vPrintf(TRACE_JOIN, "Got Router join Rsp %d\r\n", psZllMessage->uMessage.psNwkJoinRouterRspPayload->u8Status);
                        ZTIMER_eStop(u8TimerBdbTl);
                        ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(TL_START_UP_TIME_SEC));
                        bDoRejoin = TRUE;
                        bWithDiscovery = FALSE;
                        sCommission.eState = E_WAIT_START_UP;
                    } else {
                        /* recover assigned addresses */
                        vTlRecoverTempAddresses();
                        vTlEndCommissioning(pvNwk, E_IDLE, 0);
                    }
                }
            }
            else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED)
            {
                DBG_vPrintf(TRACE_COMMISSION, "zr join time out\r\n");

                vTlEndCommissioning(pvNwk, E_IDLE, 0);
            }
            break;

        case E_WAIT_JOIN_ZED_RSP:
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG) {
                if (psZllMessage->u8CommandId == E_CLD_COMMISSION_CMD_NETWORK_JOIN_END_DEVICE_RSP) {
                    if ((psZllMessage->uMessage.psNwkJoinEndDeviceRspPayload->u8Status == TL_SUCCESS) &&
                        (sCommission.u32TransactionId == psZllMessage->uMessage.psNwkJoinEndDeviceRspPayload->u32TransactionId) &&
                        (sSrcAddr.uAddress.u64Addr == sScanTarget.sScanDetails.sSrcAddr.uAddress.u64Addr))
                    {
                        DBG_vPrintf(TRACE_JOIN, "Got Zed join Rsp %d\r\n", psZllMessage->uMessage.psNwkJoinEndDeviceRspPayload->u8Status);
                        ZTIMER_eStop(u8TimerBdbTl);
                        ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(TL_START_UP_TIME_SEC));
                        bDoRejoin = FALSE;
                        sCommission.eState = E_WAIT_START_UP;
                    } else {
                        /* recover assigned addresses */
                        vTlRecoverTempAddresses();
                        vTlEndCommissioning(pvNwk, E_IDLE, 0);
                    }
                }
            }
            else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED)
            {
                DBG_vPrintf(TRACE_COMMISSION, "ed join time out\r\n");

                vTlEndCommissioning(pvNwk, E_IDLE, 0);
            }
            break;

        /*
         * We are the target of a scan
         */
        case E_ACTIVE:
            /* handle process after scan rsp */
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED)
            {
                DBG_vPrintf(TRACE_COMMISSION, "Active time out\r\n");
                vTlEndCommissioning(pvNwk, E_IDLE, 0);
            }
            else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG)
            {
                DBG_vPrintf(TRACE_COMMISSION, "Zll cmd %d in active\r\n", psZllMessage->u8CommandId);
                /* Set up return address */
                sDstAddr = sSrcAddr;
                sDstAddr.u16PanId = 0xffff;
                switch(psZllMessage->u8CommandId)
                {
                    /* if we receive the factory reset clear the persisted data and do a reset.
                     */
                    case E_CLD_COMMISSION_CMD_FACTORY_RESET_REQ:
                        if ((sZllState.eState == NOT_FACTORY_NEW_REMOTE) &&
                            (psZllMessage->uMessage.psFactoryResetPayload->u32TransactionId ==  sCommission.u32TransactionId))
                        {
                            sCommission.eState = E_WAIT_LEAVE_RESET;
                            /* leave req */
                            ZPS_eAplZdoLeaveNetwork(0, FALSE,FALSE);
                            sBDB.sAttrib.bLeaveRequested = TRUE;
                        }
                        break;
                    case E_CLD_COMMISSION_CMD_NETWORK_UPDATE_REQ:
                        if (psZllMessage->uMessage.psNwkUpdateReqPayload->u32TransactionId ==  sCommission.u32TransactionId)
                        {
                            vTlHandleNwkUpdateRequest( pvNwk,
                                                            psNib,
                                                            psZllMessage->uMessage.psNwkUpdateReqPayload);
                        }
                        break;

                    case E_CLD_COMMISSION_CMD_IDENTIFY_REQ:
                        if (psZllMessage->uMessage.psIdentifyReqPayload->u32TransactionId ==  sCommission.u32TransactionId)
                        {
                            vHandleIdentifyRequest(psZllMessage->uMessage.psIdentifyReqPayload->u16Duration);
                        }
                        break;

                    case E_CLD_COMMISSION_CMD_DEVICE_INFO_REQ:
                        if (psZllMessage->uMessage.psDeviceInfoReqPayload->u32TransactionId ==  sCommission.u32TransactionId)
                        {
                            vTlHandleDeviceInfoRequest(    psZllMessage->uMessage.psDeviceInfoReqPayload,
                                                                &sSrcAddr);
                        }
                        break;

                    case E_CLD_COMMISSION_CMD_NETWORK_JOIN_ROUTER_REQ:
                        /* we zre noit a zr, drop silently zb3.0 comment resolutiion */
                        break;

                    case E_CLD_COMMISSION_CMD_NETWORK_START_REQ:
                        /* we zre noit a zr, drop silently zb3.0 comment resolutiion */
                        break;

                    case E_CLD_COMMISSION_CMD_NETWORK_JOIN_END_DEVICE_REQ:
                        vTlHandleEndDeviceJoinRequest( pvNwk,
                                                     psNib,
                                                     psZllMessage->uMessage.psNwkJoinEndDeviceReqPayload,
                                                     &sSrcAddr,
                                                     &sCommission);
                        break;

                    default:
                        break;

                }               /* endof switch(psZllMessage->u8CommandId) */

            }               /* endof if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG) */

            break;

        case E_WAIT_LEAVE:
            break;

        case E_WAIT_LEAVE_RESET:
            if ((psEvent->eType == BDB_E_ZCL_EVENT_LEAVE_CFM) ||
                 (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED )) {
                DBG_vPrintf(TRACE_JOIN,"WARNING: Received Factory reset \r\n");
                APP_vFactoryResetRecords();
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
                vAHI_SwReset();
#else
                RESET_SystemReset();
#endif
            }
            break;

        case E_WAIT_START_UP:
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED)
            {
                #if ADJUST_POWER
                eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, TX_POWER_NORMAL);
                #endif

                DBG_vPrintf(TRACE_COMMISSION, "\r\nRxIdle False");

                MAC_vPibSetRxOnWhenIdle(NULL, FALSE, FALSE);

                if ( sZllState.eState == FACTORY_NEW_REMOTE ) {
                    ZPS_eAplAibSetApsTrustCenterAddress( 0xffffffffffffffffULL );
                    sZllState.eState = NOT_FACTORY_NEW_REMOTE;
                } else {
                    DBG_vPrintf(TRACE_JOIN, "Back on Ch %d\r\n", sZllState.u8MyChannel);
                    eAppApiPlmeSet(PHY_PIB_ATTR_CURRENT_CHANNEL, sZllState.u8MyChannel);

                    DBG_vPrintf(TRACE_JOIN, "restart poll timer\r\n");

                }

                DBG_vPrintf(TRACE_JOIN, "Free addr %04x to %04x\r\n", sZllState.u16FreeAddrLow, sZllState.u16FreeAddrHigh);
                DBG_vPrintf(TRACE_JOIN, "Free Gp  %04x to %04x\r\n", sZllState.u16FreeGroupLow, sZllState.u16FreeGroupHigh);

                sCommission.eState = E_INFORM_APP;
                PDM_eSaveRecordData(PDM_ID_APP_ZLO_CONTROLLER, &sZllState,sizeof(tsZllRemoteState));
                sCommission.u32TransactionId = 0;
                sCommission.bResponded = FALSE;
                if (bDoRejoin == TRUE)
                {
                    DBG_vPrintf(TRACE_JOIN|TRUE, "TLINK: Req rejoin\r\n");
                    if (bWithDiscovery == FALSE)
                    {
                        vTlSetUpParent( psNib, sTarget.u16NwkAddr);
                    }
                    bFailedToJoin = FALSE;
                    BDB_vSetRejoinFilter();
                    sBDB.sAttrib.bbdbNodeIsOnANetwork = TRUE;
                    ZPS_eAplZdoRejoinNetwork(bWithDiscovery);

                    ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(3000));
                    DBG_vPrintf(TRACE_JOIN, "Wait inform 3000\r\n");
                }
                else
                {
                    ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(2500));
                    DBG_vPrintf(TRACE_JOIN, "Wait inform 1500\r\n");
                    DBG_vPrintf(TRACE_JOIN, "restart poll timer\r\n");
                    vStartPolling();
                }

            }
            break;

        case E_INFORM_APP:
            /* tell the ap about the target we just joined with */
            if (sTarget.u16NwkAddr > 0) {
                sAppEvent.eType = APP_E_EVENT_TOUCH_LINK;
                sAppEvent.uEvent.sTouchLink = sTarget;
                DBG_vPrintf(TRACE_COMMISSION, "\r\nInform on %04x", sTarget.u16NwkAddr);
                ZQ_bQueueSend(&APP_msgAppEvents, &sAppEvent);
            }
            sCommission.eState = E_IDLE;
            //bTLinkInProgress = FALSE;
            DBG_vPrintf(TRACE_COMMISSION, "\r\nInform App, stop");
            break;

        default:
            DBG_vPrintf(TRACE_COMMISSION, "Unhandled event %d\r\n", psEvent->eType);
            break;
    }
}

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

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: vEndCommissioning
 *
 * DESCRIPTION: cleans up after a touch link session
 *
 * RETURNS: void
 * void
 *
 ****************************************************************************/
PRIVATE void vTlEndCommissioning( void* pvNwk, eState eState, uint16 u16TimeMS){
    sCommission.eState = eState;
    eAppApiPlmeSet(PHY_PIB_ATTR_CURRENT_CHANNEL, sZllState.u8MyChannel);

#if ADJUST_POWER
    eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, TX_POWER_NORMAL);
#endif
    sCommission.u32TransactionId = 0;
    sCommission.bResponded = FALSE;
    sCommission.bDoPrimaryScan = FALSE;
    sCommission.bProfileInterOp = FALSE;
    ZTIMER_eStop(u8TimerBdbTl);
    if (u16TimeMS > 0) {
        ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(u16TimeMS));
    }
    DBG_vPrintf(TRACE_COMMISSION, "\r\nRxIdle False");
    MAC_vPibSetRxOnWhenIdle(NULL, FALSE, FALSE);
    if (sZllState.eState == NOT_FACTORY_NEW_REMOTE) {
        vStartPolling();
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
        DBG_vPrintf(TRACE_SCAN_REQ, "Scan Set complete\r\n");
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
        DBG_vPrintf(TRACE_SCAN_REQ, "TL scan use ch %d\r\n", u32ChannelNumber);
    }

    eAppApiPlmeSet(PHY_PIB_ATTR_CURRENT_CHANNEL, u32ChannelNumber);
    sDstAddr.eMode = ZPS_E_AM_INTERPAN_SHORT;
    sDstAddr.u16PanId = 0xffff;
    sDstAddr.uAddress.u16Addr = 0xffff;
    sScanReqPayload.u32TransactionId = psCommission->u32TransactionId;
    sScanReqPayload.u8ZigbeeInfo =  TL_ZED;              // Rxon idle router

    sScanReqPayload.u8ZllInfo =
        (sZllState.eState == FACTORY_NEW_REMOTE) ?
                (TL_PROFILE_INTEROP|TL_FACTORY_NEW|TL_LINK_INIT|TL_ADDR_ASSIGN) :
                (TL_PROFILE_INTEROP|TL_LINK_INIT|TL_ADDR_ASSIGN);                  // factory New Addr assign Initiating

    //DBG_vPrintf(TRACE_COMMISSION, "Scan Req ret %02x\r\n",  );
    eCLD_ZllCommissionCommandScanReqCommandSend(
                                &sDstAddr,
                                 &u8Seq,
                                 &sScanReqPayload);

    ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(TL_SCAN_RX_TIME_MS));

    if (psCommission->u8Count)
    {
        psCommission->u8Count--;
        DBG_vPrintf(TRACE_SCAN_REQ, "Scan count now %d Mask %08x\r\n", psCommission->u8Count, psCommission->u32ScanChannels);
    }
    else
    {
        // clear channle as done
        psCommission->u32ScanChannels &= ~u32ChannelMask;
        DBG_vPrintf(TRACE_SCAN_REQ, "Scan Mask now %08x\r\n", psCommission->u32ScanChannels);
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


    sScanRsp.u8RSSICorrection = RSSI_CORRECTION;
    sScanRsp.u8ZigbeeInfo = TL_ZED;
    sScanRsp.u16KeyMask = TL_SUPPORTED_KEYS;
    if (sZllState.eState == FACTORY_NEW_REMOTE) {
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
 * NAME: vHandleScanResponse
 *
 * DESCRIPTION: handles scan responses, decodes if the responder
 * should become the target for the touch link
 *
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

    //DBG_vPrintf(TRACE_COMMISSION, "Scan Rsp\r\n");
    u16AdjustedLqi = u8Lqi + psScanRsp->u8RSSICorrection;
    if (u16AdjustedLqi < TL_SCAN_LQI_MIN )
    {
        DBG_vPrintf(TRACE_SCAN, "Reject LQI %d\r\n", u8Lqi);
        return;
    }

    DBG_vPrintf(TRACE_COMMISSION, "Their %016llx Mine %016llx TC %016llx\r\n",
            psScanRsp->u64ExtPanId,
            psNib->sPersist.u64ExtPanId,
            ZPS_eAplAibGetApsTrustCenterAddress()
            );
    if ( (sZllState.eState == NOT_FACTORY_NEW_REMOTE) &&
            (psScanRsp->u64ExtPanId != psNib->sPersist.u64ExtPanId) &&
            ( ZPS_eAplAibGetApsTrustCenterAddress() != 0xffffffffffffffffULL )
            )
    {
        DBG_vPrintf(TRACE_COMMISSION, "Non Zll nwk, target not my nwk\r\n");
        return;
    }

    if ( (TL_SUPPORTED_KEYS & psScanRsp->u16KeyMask) == 0)
    {
        // No common supported key index
        if ( (sZllState.eState == FACTORY_NEW_REMOTE) ||
            ( (sZllState.eState == NOT_FACTORY_NEW_REMOTE) && (psScanRsp->u64ExtPanId != psNib->sPersist.u64ExtPanId) )
                )
        {
            /* Either we are factory new or
             * we are not factory new and the target is on another pan
             * there is no common key index, to exchange keys so drop the scan response without further action
             */
            DBG_vPrintf(TRACE_COMMISSION, "No common security level, target not my nwk\r\n");
            return;
        }
    }

    /*
     * Check for Nwk Update Ids
     */
    if ( (sZllState.eState == NOT_FACTORY_NEW_REMOTE) &&
         (psScanRsp->u64ExtPanId == psNib->sPersist.u64ExtPanId) &&
         (psScanRsp->u16PanId == psNib->sPersist.u16VsPanId))
    {
        DBG_vPrintf(TRACE_COMMISSION, "Our Nwk Id %d theirs %d\r\n", psNib->sPersist.u8UpdateId, psScanRsp->u8NwkUpdateId );
        if (psNib->sPersist.u8UpdateId != psScanRsp->u8NwkUpdateId ) {
            if ( psNib->sPersist.u8UpdateId == BDB_u8TlNewUpdateID(psNib->sPersist.u8UpdateId, psScanRsp->u8NwkUpdateId) ) {
                DBG_vPrintf(TRACE_COMMISSION, "Use ours on %d\r\n", sZllState.u8MyChannel);
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
                //DBG_vPrintf(TRACE_COMMISSION, "Use theirs\r\n");
                /*
                 * Part of a network and our Update Id is out of date
                 */
                ZPS_vNwkNibSetUpdateId( pvNwk, psScanRsp->u8NwkUpdateId);
                sZllState.u8MyChannel = psScanRsp->u8LogicalChannel;

                vTlEndCommissioning(pvNwk, E_IDLE, 0);

                DBG_vPrintf(TRACE_SCAN, "Update Id and rejoin\r\n");
                BDB_vSetRejoinFilter();
                sBDB.sAttrib.bbdbNodeIsOnANetwork = TRUE;
                ZPS_eAplZdoRejoinNetwork(FALSE);
                return;
            }
        }
    }

    if  ((sZllState.eState == FACTORY_NEW_REMOTE) &&
              ((psScanRsp->u8ZigbeeInfo & TL_TYPE_MASK) == TL_ZED))
    {
        DBG_vPrintf(TRACE_SCAN, "Drop scan result 2 FN ZED\r\n");
    }
    else
    {
        if (u16AdjustedLqi > psScanTarget->u16LQI) {
            if (  ( au64IgnoreList[0] != psSrcAddr->uAddress.u64Addr  ) &&
                    (au64IgnoreList[1] != psSrcAddr->uAddress.u64Addr ) &&
                    (au64IgnoreList[2] != psSrcAddr->uAddress.u64Addr )
              )
            {
                psScanTarget->u16LQI = u16AdjustedLqi;
                /*
                 *  joining scenarios we are a ZED
                 * 1) Any router,
                 * 2) we are NFN then FN device
                 * 3) we are NFN then any NFN ED that is not on our Pan
                 */
                DBG_vPrintf(TRACE_SCAN, "Accept %d from %016llx\r\n", psScanTarget->u16LQI, psSrcAddr->uAddress.u64Addr );
                psScanTarget->sScanDetails.sSrcAddr = *psSrcAddr;
                psScanTarget->sScanDetails.sScanRspPayload = *psScanRsp;

                /* If the number of sub devices was not 1, the device and ep details will be missing
                 * these must be got from the device info request response
                 */
            }
            else
            {
                DBG_vPrintf(TRACE_SCAN, "%016llx is on the black list\r\n", psSrcAddr->uAddress.u64Addr);
            }

        }/* else {DBG_vPrintf(TRACE_COMMISSION, "Drop lower lqi Old %d Rej%d\r\n", psScanTarget->u16LQI, u16AdjustedLqi);}*/
    }
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
PRIVATE void vTlSendDeviceInfoReq(uint64 u64Addr, uint32 u32TransactionId, uint8 u8Index) {
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


/****************************************************************************
 *
 * NAME: vSendStartRequest
 *
 * DESCRIPTION: send a network start request to touch link target
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlSendStartRequest(void *pvNwk,
                               ZPS_tsNwkNib *psNib,
                               tsZllScanTarget *psScanTarget,
                               uint32 u32TransactionId )
{
    ZPS_tsInterPanAddress       sDstAddr;
    tsCLD_ZllCommission_NetworkStartReqCommandPayload           sNwkStartReqPayload;
    uint8 u8Seq;
    uint16 u16KeyMask;
    int i;

    if (psScanTarget->sScanDetails.sScanRspPayload.u8ZllInfo & TL_FACTORY_NEW) {
        DBG_vPrintf(TRACE_SCAN, "FN -> FN Router target\r\n");
        /* nwk start */
    } else {
        DBG_vPrintf(TRACE_SCAN, "FN -> NFN Router target\r\n");
        /* nwk start */
    }
    memset(&sNwkStartReqPayload, 0, sizeof(tsCLD_ZllCommission_NetworkStartReqCommandPayload));
    sNwkStartReqPayload.u32TransactionId = sCommission.u32TransactionId;
    // ext pan 0, target decides
#ifdef FIXED_CHANNEL
    sNwkStartReqPayload.u8LogicalChannel = FIXED_CHANNEL;
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
        au8TempKeyStore[i] = 0x09;
#endif
    }

    BDB_u8TlEncryptKey( au8TempKeyStore,
                        sNwkStartReqPayload.au8NwkKey,
                        u32TransactionId,
                        psScanTarget->sScanDetails.sScanRspPayload.u32ResponseId,
                        sNwkStartReqPayload.u8KeyIndex);

        /* temp save addreess and group ranges */
    vTlSaveTempAddresses();

    // take our group address requirement
    vSetGroupAddress(sZllState.u16FreeGroupLow, GROUPS_REQUIRED);
    sZllState.u16FreeGroupLow += GROUPS_REQUIRED;

    sNwkStartReqPayload.u16InitiatorNwkAddr = sZllState.u16FreeAddrLow;
    sZllState.u16MyAddr = sZllState.u16FreeAddrLow++;
    sTarget.u16NwkAddr = sZllState.u16FreeAddrLow;
    sNwkStartReqPayload.u16NwkAddr = sZllState.u16FreeAddrLow++;
    DBG_vPrintf(TRACE_COMMISSION, "Give it addr %04x\r\n", sNwkStartReqPayload.u16NwkAddr);
    if (psScanTarget->sScanDetails.sScanRspPayload.u8ZllInfo & TL_ADDR_ASSIGN) {
        DBG_vPrintf(TRACE_JOIN, "Assign addresses\r\n");

        sNwkStartReqPayload.u16FreeNwkAddrBegin = ((sZllState.u16FreeAddrLow+sZllState.u16FreeAddrHigh-1) >> 1);
        sNwkStartReqPayload.u16FreeNwkAddrEnd = sZllState.u16FreeAddrHigh;
        sZllState.u16FreeAddrHigh = sNwkStartReqPayload.u16FreeNwkAddrBegin - 1;

        sNwkStartReqPayload.u16FreeGroupIdBegin = ((sZllState.u16FreeGroupLow+sZllState.u16FreeGroupHigh-1) >> 1);
        sNwkStartReqPayload.u16FreeGroupIdEnd = sZllState.u16FreeGroupHigh;
        sZllState.u16FreeGroupHigh = sNwkStartReqPayload.u16FreeGroupIdBegin - 1;
    }
    if (psScanTarget->sScanDetails.sScanRspPayload.u8GroupIdCount) {
        /* allocate count group ids */
        sNwkStartReqPayload.u16GroupIdBegin = sZllState.u16FreeGroupLow;
        sNwkStartReqPayload.u16GroupIdEnd = sZllState.u16FreeGroupLow + psScanTarget->sScanDetails.sScanRspPayload.u8GroupIdCount-1;
        sZllState.u16FreeGroupLow += psScanTarget->sScanDetails.sScanRspPayload.u8GroupIdCount;
    }

    sNwkStartReqPayload.u64InitiatorIEEEAddr = sDeviceTable.asDeviceRecords[0].u64IEEEAddr;

    sDstAddr.eMode = 3;
    sDstAddr.u16PanId = 0xffff;
    sDstAddr.uAddress.u64Addr = psScanTarget->sScanDetails.sSrcAddr.uAddress.u64Addr;

    eAppApiPlmeSet(PHY_PIB_ATTR_CURRENT_CHANNEL, psScanTarget->sScanDetails.sScanRspPayload.u8LogicalChannel);

    //DBG_vPrintf(TRACE_COMMISSION, "Ch %d", psScanTarget->sScanDetails.sScanRspPayload.u8LogicalChannel);
    eCLD_ZllCommissionCommandNetworkStartReqCommandSend( &sDstAddr,
                                                         &u8Seq,
                                                         &sNwkStartReqPayload  );
}

/****************************************************************************
 *
 * NAME: vSendRouterJoinRequest
 *
 * DESCRIPTION: sends router join request to touch link target
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlSendRouterJoinRequest(void *pvNwk,
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

    if (u16KeyMask & TL_MASTER_KEY_MASK) {
        sNwkJoinRouterReqPayload.u8KeyIndex = TL_MASTER_KEY_INDEX;
    } else if (u16KeyMask & TL_CERTIFICATION_KEY_MASK) {
        sNwkJoinRouterReqPayload.u8KeyIndex = TL_CERTIFICATION_KEY_INDEX;
    } else {
        sNwkJoinRouterReqPayload.u8KeyIndex = TL_TEST_KEY_INDEX;
    }

    BDB_u8TlEncryptKey(psNib->sTbl.psSecMatSet[0].au8Key,
            sNwkJoinRouterReqPayload.au8NwkKey,
            u32TransactionId,
            psScanTarget->sScanDetails.sScanRspPayload.u32ResponseId,
            sNwkJoinRouterReqPayload.u8KeyIndex);



    sNwkJoinRouterReqPayload.u64ExtPanId = psNib->sPersist.u64ExtPanId;

    DBG_vPrintf(TRACE_COMMISSION, "RJoin nwk epid %016llx\r\n", psNib->sPersist.u64ExtPanId);
    DBG_vPrintf(TRACE_COMMISSION, "RJoin nwk epid %016llx\r\n",ZPS_psAplAibGetAib()->u64ApsUseExtendedPanid);

    sNwkJoinRouterReqPayload.u8NwkUpdateId = psNib->sPersist.u8UpdateId;
    sNwkJoinRouterReqPayload.u8LogicalChannel = sZllState.u8MyChannel;
    sNwkJoinRouterReqPayload.u16PanId = psNib->sPersist.u16VsPanId;

    /* temp save addreess and group ranges */
    vTlSaveTempAddresses();

    if (sZllState.u16FreeAddrLow == 0) {
        sTarget.u16NwkAddr = RND_u32GetRand(1, 0xfff6);
        sNwkJoinRouterReqPayload.u16NwkAddr = sTarget.u16NwkAddr;
    } else  {
        sTarget.u16NwkAddr = sZllState.u16FreeAddrLow;
        sNwkJoinRouterReqPayload.u16NwkAddr = sZllState.u16FreeAddrLow++;
        if (psScanTarget->sScanDetails.sScanRspPayload.u8ZllInfo & TL_ADDR_ASSIGN) {
            DBG_vPrintf(TRACE_JOIN, "Assign addresses\r\n");

            sNwkJoinRouterReqPayload.u16FreeNwkAddrBegin = ((sZllState.u16FreeAddrLow+sZllState.u16FreeAddrHigh-1) >> 1);
            sNwkJoinRouterReqPayload.u16FreeNwkAddrEnd = sZllState.u16FreeAddrHigh;
            sZllState.u16FreeAddrHigh = sNwkJoinRouterReqPayload.u16FreeNwkAddrBegin - 1;

            sNwkJoinRouterReqPayload.u16FreeGroupIdBegin = ((sZllState.u16FreeGroupLow+sZllState.u16FreeGroupHigh-1) >> 1);
            sNwkJoinRouterReqPayload.u16FreeGroupIdEnd = sZllState.u16FreeGroupHigh;
            sZllState.u16FreeGroupHigh = sNwkJoinRouterReqPayload.u16FreeGroupIdBegin - 1;
        }
    }
    if (psScanTarget->sScanDetails.sScanRspPayload.u8GroupIdCount) {
        /* allocate count group id's */
        sNwkJoinRouterReqPayload.u16GroupIdBegin = sZllState.u16FreeGroupLow;
        sNwkJoinRouterReqPayload.u16GroupIdEnd = sZllState.u16FreeGroupLow + psScanTarget->sScanDetails.sScanRspPayload.u8GroupIdCount-1;
        sZllState.u16FreeGroupLow += psScanTarget->sScanDetails.sScanRspPayload.u8GroupIdCount;
    }
    DBG_vPrintf(TRACE_JOIN, "Give it addr %04x\r\n", sNwkJoinRouterReqPayload.u16NwkAddr);
    /* rest of params zero */
    eAppApiPlmeSet(PHY_PIB_ATTR_CURRENT_CHANNEL, psScanTarget->sScanDetails.sScanRspPayload.u8LogicalChannel);

    sDstAddr.eMode = 3;
    sDstAddr.u16PanId = 0xffff;
    sDstAddr.uAddress.u64Addr = psScanTarget->sScanDetails.sSrcAddr.uAddress.u64Addr;

    DBG_vPrintf(TRACE_COMMISSION, "Send rtr join on Ch %d for ch %d",psScanTarget->sScanDetails.sScanRspPayload.u8LogicalChannel, sZllState.u8MyChannel);
    eCLD_ZllCommissionCommandNetworkJoinRouterReqCommandSend( &sDstAddr,
                                                             &u8Seq,
                                                             &sNwkJoinRouterReqPayload  );
}

/****************************************************************************
 *
 * NAME: vSendEndDeviceJoinRequest
 *
 * DESCRIPTION: sends end device join request to touch link target
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlSendEndDeviceJoinRequest(void *pvNwk,
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
    if (u16KeyMask & TL_MASTER_KEY_MASK) {
        sNwkJoinEndDeviceReqPayload.u8KeyIndex = TL_MASTER_KEY_INDEX;
    } else if (u16KeyMask & TL_CERTIFICATION_KEY_MASK) {
        sNwkJoinEndDeviceReqPayload.u8KeyIndex = TL_CERTIFICATION_KEY_INDEX;
    } else {
        sNwkJoinEndDeviceReqPayload.u8KeyIndex = TL_TEST_KEY_INDEX;
    }


    BDB_u8TlEncryptKey(psNib->sTbl.psSecMatSet[0].au8Key,
                sNwkJoinEndDeviceReqPayload.au8NwkKey,
                u32TransactionId,
                psScanTarget->sScanDetails.sScanRspPayload.u32ResponseId,
                sNwkJoinEndDeviceReqPayload.u8KeyIndex);



    sNwkJoinEndDeviceReqPayload.u64ExtPanId = psNib->sPersist.u64ExtPanId;
    sNwkJoinEndDeviceReqPayload.u8NwkUpdateId = psNib->sPersist.u8UpdateId;
    sNwkJoinEndDeviceReqPayload.u8LogicalChannel = sZllState.u8MyChannel;
    sNwkJoinEndDeviceReqPayload.u16PanId = psNib->sPersist.u16VsPanId;

    /* temp save addreess and group ranges */
    vTlSaveTempAddresses();

    if (sZllState.u16FreeAddrLow == 0) {
        sTarget.u16NwkAddr = RND_u32GetRand(1, 0xfff6);;
        sNwkJoinEndDeviceReqPayload.u16NwkAddr = sTarget.u16NwkAddr;
    } else  {
        sTarget.u16NwkAddr = sZllState.u16FreeAddrLow;
        sNwkJoinEndDeviceReqPayload.u16NwkAddr = sZllState.u16FreeAddrLow++;
        if (psScanTarget->sScanDetails.sScanRspPayload.u8ZllInfo & TL_ADDR_ASSIGN) {
            DBG_vPrintf(TRACE_JOIN, "Assign addresses\r\n");

            sNwkJoinEndDeviceReqPayload.u16FreeNwkAddrBegin = ((sZllState.u16FreeAddrLow+sZllState.u16FreeAddrHigh-1) >> 1);
            sNwkJoinEndDeviceReqPayload.u16FreeNwkAddrEnd = sZllState.u16FreeAddrHigh;
            sZllState.u16FreeAddrHigh = sNwkJoinEndDeviceReqPayload.u16FreeNwkAddrBegin - 1;

            sNwkJoinEndDeviceReqPayload.u16FreeGroupIdBegin = ((sZllState.u16FreeGroupLow+sZllState.u16FreeGroupHigh-1) >> 1);
            sNwkJoinEndDeviceReqPayload.u16FreeGroupIdEnd = sZllState.u16FreeGroupHigh;
            sZllState.u16FreeGroupHigh = sNwkJoinEndDeviceReqPayload.u16FreeGroupIdBegin - 1;
        }
    }
    DBG_vPrintf(TRACE_JOIN, "Give it addr %04x\r\n", sNwkJoinEndDeviceReqPayload.u16NwkAddr);
    if (psScanTarget->sScanDetails.sScanRspPayload.u8GroupIdCount) {
        /* allocate count group ids */
        sNwkJoinEndDeviceReqPayload.u16GroupIdBegin = sZllState.u16FreeGroupLow;
        sNwkJoinEndDeviceReqPayload.u16GroupIdEnd = sZllState.u16FreeGroupLow + psScanTarget->sScanDetails.sScanRspPayload.u8GroupIdCount-1;
        sZllState.u16FreeGroupLow += psScanTarget->sScanDetails.sScanRspPayload.u8GroupIdCount;
    }

    eAppApiPlmeSet(PHY_PIB_ATTR_CURRENT_CHANNEL,psScanTarget->sScanDetails.sScanRspPayload.u8LogicalChannel);

    sDstAddr.eMode = 3;
    sDstAddr.u16PanId = 0xffff;
    sDstAddr.uAddress.u64Addr = psScanTarget->sScanDetails.sSrcAddr.uAddress.u64Addr;

    DBG_vPrintf(TRACE_JOIN, "Send zed join on Ch %d for ch %d\r\n", psScanTarget->sScanDetails.sScanRspPayload.u8LogicalChannel,
                                                                    sZllState.u8MyChannel);
    eCLD_ZllCommissionCommandNetworkJoinEndDeviceReqCommandSend( &sDstAddr,
                                                                 &u8Seq,
                                                                 &sNwkJoinEndDeviceReqPayload  );
}


/****************************************************************************
 *
 * NAME: vHandleNwkUpdateRequest
 *
 * DESCRIPTION: handles network id update requests, updates nib
 * param if required
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlHandleNwkUpdateRequest(void * pvNwk,
                                     ZPS_tsNwkNib *psNib,
                                     tsCLD_ZllCommission_NetworkUpdateReqCommandPayload *psNwkUpdateReqPayload)
{
    if ((psNwkUpdateReqPayload->u64ExtPanId == psNib->sPersist.u64ExtPanId) &&
         (psNwkUpdateReqPayload->u16PanId == psNib->sPersist.u16VsPanId) &&
          (psNib->sPersist.u16VsPanId != BDB_u8TlNewUpdateID(psNib->sPersist.u16VsPanId, psNwkUpdateReqPayload->u16PanId) ))
    {
        /* Update the UpdateId, Nwk addr and Channel */
        ZPS_vNwkNibSetUpdateId( pvNwk, psNwkUpdateReqPayload->u8NwkUpdateId);
        ZPS_vNwkNibSetNwkAddr( pvNwk,  psNwkUpdateReqPayload->u16NwkAddr);
        ZPS_vNwkNibSetChannel( pvNwk,  psNwkUpdateReqPayload->u8LogicalChannel);
    }
}

/****************************************************************************
 *
 * NAME: vHandleDeviceInfoRequest
 *
 * DESCRIPTION: handles touch link device info request, sends the reqponse
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

    DBG_vPrintf(TRACE_JOIN, "Device Info request\r\n");
    memset(&sDeviceInfoRspPayload, 0, sizeof(tsCLD_ZllCommission_DeviceInfoRspCommandPayload));
    sDeviceInfoRspPayload.u32TransactionId = psDeviceInfoReq->u32TransactionId;
    sDeviceInfoRspPayload.u8NumberSubDevices = ZLO_NUMBER_DEVICES;
    sDeviceInfoRspPayload.u8StartIndex = psDeviceInfoReq->u8StartIndex;
    sDeviceInfoRspPayload.u8DeviceInfoRecordCount = 0;

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
        DBG_vPrintf(TRACE_TL_NEGATIVE, "Start Index out of bounds\r\n");
    }

    sDstAddr = *psSrcAddr;
    sDstAddr.u16PanId = 0xffff;

    eCLD_ZllCommissionCommandDeviceInfoRspCommandSend( &sDstAddr,
                                                       &u8Seq,
                                                       &sDeviceInfoRspPayload);
}


/****************************************************************************
 *
 * NAME: vHandleEndDeviceJoinRequest
 *
 * DESCRIPTION: handles the response to a device info request
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlHandleEndDeviceJoinRequest( void * pvNwk,
                                          ZPS_tsNwkNib *psNib,
                                          tsCLD_ZllCommission_NetworkJoinEndDeviceReqCommandPayload   *psNwkJoinEndDeviceReq,
                                          ZPS_tsInterPanAddress       *psSrcAddr,
                                          tsCommissionData *psCommission)
{
    tsCLD_ZllCommission_NetworkJoinEndDeviceRspCommandPayload   sNwkJoinEndDeviceRspPayload;
    ZPS_tsAftsStartParamsDistributed sNwkParams;
    ZPS_tsInterPanAddress       sDstAddr;
    uint8 u8Seq;

    sDstAddr = *psSrcAddr;
    sDstAddr.u16PanId = 0xffff;

    DBG_vPrintf(TRACE_JOIN, "Join End Device Req\r\n");
    sNwkJoinEndDeviceRspPayload.u32TransactionId = psCommission->u32TransactionId;

    DBG_vPrintf(TRACE_JOIN, "Join End Device Req\r\n");

    if ( (psNwkJoinEndDeviceReq->u32TransactionId !=  psCommission->u32TransactionId) ||
         (psNwkJoinEndDeviceReq->u64ExtPanId == 0UL ) ||
         (psNwkJoinEndDeviceReq->u64ExtPanId == 0xFFFFFFFFFFFFFFFFUL ) ||
         (psNwkJoinEndDeviceReq->u8LogicalChannel < 11 ) ||
         (psNwkJoinEndDeviceReq->u8LogicalChannel > 26  ) ||
         (psNwkJoinEndDeviceReq->u16NwkAddr == 0x0000 ) ||
         (psNwkJoinEndDeviceReq->u16NwkAddr > 0xfff6  ) ||
         ( FALSE == BDB_bTlIsKeySupported(psNwkJoinEndDeviceReq->u8KeyIndex))
     )
    {
        /* drop bad params silently */
        return;
    }
    else if (sZllState.eState == NOT_FACTORY_NEW_REMOTE)
    {
        /* NFN Remote is not allowed to be stolen */
        sNwkJoinEndDeviceRspPayload.u8Status = TL_ERROR;
    }
    else
    {
        sNwkJoinEndDeviceRspPayload.u8Status = TL_SUCCESS;
    }


    eCLD_ZllCommissionCommandNetworkJoinEndDeviceRspCommandSend( &sDstAddr,
                                                                 &u8Seq,
                                                                 &sNwkJoinEndDeviceRspPayload);
    if (sNwkJoinEndDeviceRspPayload.u8Status == TL_SUCCESS)
    {
        sNwkParams.u64ExtPanId = psNwkJoinEndDeviceReq->u64ExtPanId;
        sNwkParams.pu8NwkKey = psNwkJoinEndDeviceReq->au8NwkKey;
        sNwkParams.u16PanId = psNwkJoinEndDeviceReq->u16PanId;
        sNwkParams.u16NwkAddr = psNwkJoinEndDeviceReq->u16NwkAddr;
        sNwkParams.u8KeyIndex = 0;
        sNwkParams.u8LogicalChannel = psNwkJoinEndDeviceReq->u8LogicalChannel;
        sNwkParams.u8NwkupdateId = psNwkJoinEndDeviceReq->u8NwkUpdateId;

        BDB_eTlDecryptKey( psNwkJoinEndDeviceReq->au8NwkKey,
                            psNwkJoinEndDeviceReq->au8NwkKey,
                            psCommission->u32TransactionId,
                            psCommission->u32ResponseId,
                            psNwkJoinEndDeviceReq->u8KeyIndex);

        sZllState.u8MyChannel = psNwkJoinEndDeviceReq->u8LogicalChannel;
       sZllState.u16MyAddr = psNwkJoinEndDeviceReq->u16NwkAddr;
       sZllState.u16FreeAddrLow = psNwkJoinEndDeviceReq->u16FreeNwkAddrBegin;
       sZllState.u16FreeAddrHigh = psNwkJoinEndDeviceReq->u16FreeNwkAddrEnd;
       sZllState.u16FreeGroupLow = psNwkJoinEndDeviceReq->u16FreeGroupIdBegin;
       sZllState.u16FreeGroupHigh = psNwkJoinEndDeviceReq->u16FreeGroupIdEnd;

       // take our group addr requirement
       vSetGroupAddress( psNwkJoinEndDeviceReq->u16GroupIdBegin, GROUPS_REQUIRED);


       DBG_vPrintf(TRACE_JOIN, "Start with pan %016llx\r\n", psNwkJoinEndDeviceReq->u64ExtPanId);

       ZPS_eAplInitEndDeviceDistributed( &sNwkParams);

       ZTIMER_eStop(u8TimerBdbTl);
      ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
      bDoRejoin = TRUE;
      bWithDiscovery = TRUE;
      psCommission->eState = E_WAIT_START_UP;
    }
}

/****************************************************************************
 *
 * NAME: vHandleNwkStartResponse
 *
 * DESCRIPTION: handles the start response, either sets up the network params
 *  ready to join, or recovers assigned addresses in a fail case
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlHandleNwkStartResponse( void * pvNwk,
                                             tsCLD_ZllCommission_NetworkStartRspCommandPayload *psNwkStartRsp,
                                             tsCommissionData *psCommission,
                                             ZPS_tsInterPanAddress       *psSrcAddr)
{
    ZPS_tsAftsStartParamsDistributed sNwkParams;


    if ((psNwkStartRsp->u8Status == TL_SUCCESS) &&
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
        sNwkParams.u8NwkupdateId = psNwkStartRsp->u8NwkUpdateId;

        ZPS_eAplInitEndDeviceDistributed( &sNwkParams);


        ZTIMER_eStop(u8TimerBdbTl);
        ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(TL_START_UP_TIME_SEC));
        DBG_vPrintf(TRACE_JOIN, "Start on Ch %d\r\n", sZllState.u8MyChannel);
        bDoRejoin = TRUE;
        bWithDiscovery = FALSE;
        psCommission->eState = E_WAIT_START_UP;

    } else {
        /* recover assigned addresses */
        /* temp save addreess and group ranges */
        DBG_vPrintf(TRACE_TL_NEGATIVE, "BAD Start Response\r\n");
        vTlRecoverTempAddresses();
    }
}

/****************************************************************************
 *
 * NAME: vSendIdentifyRequest
 *
 * DESCRIPTION: sends an identify request to a touch link target
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
    sIdentifyReqPayload.u16Duration = u8Time;

    eCLD_ZllCommissionCommandDeviceIndentifyReqCommandSend(
                                        &sDstAddr,
                                        &u8Seq,
                                        &sIdentifyReqPayload);
}

/****************************************************************************
 *
 * NAME: vSendFactoryResetRequest
 *
 * DESCRIPTION: send a factory reset touch link command to a target
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlSendFactoryResetRequest(void *pvNwk,
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

    eAppApiPlmeSet(PHY_PIB_ATTR_CURRENT_CHANNEL, psScanTarget->sScanDetails.sScanRspPayload.u8LogicalChannel);
    sFacRstPayload.u32TransactionId= psScanTarget->sScanDetails.sScanRspPayload.u32TransactionId;
    eCLD_ZllCommissionCommandFactoryResetReqCommandSend( &sDstAddr,
                                                         &u8Seq,
                                                         &sFacRstPayload  );

    vRemoveLight(psScanTarget->sScanDetails.sScanRspPayload.u16NwkAddr);
}

/****************************************************************************
 *
 * NAME: vSaveTempAddresses
 *
 * DESCRIPTION: temp save the address and group ranges in case the join or network start request fails
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlSaveTempAddresses( void) {
    /* save address ranges, in case needed to recover */
    u16TempAddrLow = sZllState.u16FreeAddrLow;
     u16TempAddrHigh = sZllState.u16FreeAddrHigh;
     u16TempGroupLow = sZllState.u16FreeGroupLow;
     u16TempGroupHigh = sZllState.u16FreeGroupHigh;
}

/****************************************************************************
 *
 * NAME: vRecoverTempAddresses
 *
 * DESCRIPTION: restore the address and group ranges after a join or network start failed
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlRecoverTempAddresses( void) {
    /* recover assigned addresses */
    sZllState.u16FreeAddrLow = u16TempAddrLow;
    sZllState.u16FreeAddrHigh = u16TempAddrHigh;
    sZllState.u16FreeGroupLow = u16TempGroupLow;
    sZllState.u16FreeGroupHigh = u16TempGroupHigh;
}

/****************************************************************************
 *
 * NAME: vSetUpParent
 *
 * DESCRIPTION: sets up the parent in the neighbor table before join
 * with out discovery
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vTlSetUpParent( ZPS_tsNwkNib *psNib, uint16 u16Addr)
{
    void *pvNwk = ZPS_pvAplZdoGetNwkHandle();
    psNib->sTbl.psNtActv[ZPS_NWK_NT_ACTV_PARENT].u16NwkAddr = u16Addr;
    psNib->sTbl.psNtActv[ZPS_NWK_NT_ACTV_PARENT].u16Lookup = 0xffff;
    psNib->sTbl.pu32InFCSet[0] = 0;
    ZPS_vNtSetUsedStatus(pvNwk, &psNib->sTbl.psNtActv[ZPS_NWK_NT_ACTV_PARENT], TRUE);
    psNib->sTbl.psNtActv[ZPS_NWK_NT_ACTV_PARENT].uAncAttrs.bfBitfields.u1DeviceType = TRUE; /* ZC/ZR */
    psNib->sTbl.psNtActv[ZPS_NWK_NT_ACTV_PARENT].uAncAttrs.bfBitfields.u1PowerSource = TRUE; /* Mains */
    psNib->sTbl.psNtActv[ZPS_NWK_NT_ACTV_PARENT].uAncAttrs.bfBitfields.u1RxOnWhenIdle = TRUE;
    psNib->sTbl.psNtActv[ZPS_NWK_NT_ACTV_PARENT].uAncAttrs.bfBitfields.u1Authenticated = TRUE;
    psNib->sTbl.psNtActv[ZPS_NWK_NT_ACTV_PARENT].uAncAttrs.bfBitfields.u2Relationship = ZPS_NWK_NT_AP_RELATIONSHIP_PARENT;
    psNib->sTbl.psNtActv[ZPS_NWK_NT_ACTV_PARENT].uAncAttrs.bfBitfields.u3OutgoingCost = 1;
    psNib->sTbl.psNtActv[ZPS_NWK_NT_ACTV_PARENT].u8Age = 0;
}




/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

