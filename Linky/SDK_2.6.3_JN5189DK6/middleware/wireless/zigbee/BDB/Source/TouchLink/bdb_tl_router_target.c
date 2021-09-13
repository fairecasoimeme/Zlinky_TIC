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
# COMPONENT:   bdb_tl_router_target.c
#
# DESCRIPTION: BDB Touchlink implementation of Router as Target
#              
#
###############################################################################*/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
#include "zps_gen.h"
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
#include "zps_nwk_nib.h"
#include "ZTimer.h"



#include "app_common.h"


#include "app_main.h"
#include "app_zlo_light_node.h"

#include "app_zcl_light_task.h"
#include "app_events.h"
#include "zcl_customcommand.h"

#include "zps_nwk_nib.h"

#include <string.h>
#include <rnd_pub.h>
#include <stdlib.h>

#include "PDM_IDs.h"


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define ADJUST_POWER        TRUE
#define TL_SCAN_LQI_MIN    (100)

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

#ifndef DEBUG_TL_NEGATIVE
#define TRACE_TL_NEGATIVE       FALSE
#else
#define TRACE_TL_NEGATIVE       TRUE
#endif

#define PERMIT_JOIN       FALSE
#define PERMIT_JOIN_TIME  255

#if (defined DR1173) || (defined DR1175)
#define RSSI_CORRECTION     0x0d       // value for dk4 boards
#else
#define RSSI_CORRECTION     0x20       // max allowed value, for SSL bulbs
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef enum {
    E_IDLE,
    E_ACTIVE,
    E_WAIT_DISCOVERY,
    E_SKIP_DISCOVERY,
    E_WAIT_LEAVE,
    E_WAIT_LEAVE_RESET,
    E_START_ROUTER
} teState;

typedef struct {
    ZPS_tsInterPanAddress sSrcAddr;
    tsCLD_ZllCommission_ScanRspCommandPayload sScanRspPayload;
} tsZllScanTable;

typedef struct {
    uint32 u8Count;
    tsZllScanTable sScanTable[2];
} tsZllScanResults;

typedef struct {
    teState eState;
    uint32 u32TransactionId;
    uint32 u32ResponseId;
    uint8 u8Flags;
    bool_t bProfileInterOp;
} tsCommissionData;


/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE teZCL_Status BDB_eTlSendScanResponse(ZPS_tsNwkNib *psNib,
                               ZPS_tsInterPanAddress       *psDstAddr,
                               uint32 u32TransactionId,
                               uint32 u32ResponseId);
PRIVATE bool_t BDB_bTlSearchDiscNt(ZPS_tsNwkNib *psNib, uint64 u64EpId, uint16 u16PanId);



/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern tsZllState sZllState;
PUBLIC uint8 u8TimerBdbTl;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
tsStartParams sStartParams;
tsCommissionData sCommission;

ZPS_tsInterPanAddress sDstAddr;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
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
    sCommission.u32ResponseId = 0;
    sCommission.u32TransactionId = 0;
    sCommission.u8Flags = 0;
    sCommission.bProfileInterOp = FALSE;
}

/****************************************************************************
 *
 * NAME: BDB_bTlTouchLinkInProgress
 *
 * DESCRIPTION: Informs host application if touch link is in preogress
 *
 * PARAMETERS:      Name            RW  Usage
 *
 * RETURNS: true if TL in progress, False otherwwize
 *
 * NOTES:
 *
 ****************************************************************************/
PUBLIC bool_t BDB_bTlTouchLinkInProgress( void)
{
    return ( sCommission.eState > E_IDLE) ? TRUE: FALSE;
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
    return (sCommission.eState > E_IDLE) ? TRUE: FALSE;
}

/****************************************************************************
 *
 * NAME: APP_Commission_Task
 *
 * DESCRIPTION:
 * Task that handles touch link related events
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void BDB_vTlStateMachine( tsBDB_ZCLEvent *psEvent)
{


    tsZllPayloads sZllCommand;
    ZPS_tsInterPanAddress       sSrcAddr;
    uint8 u8Lqi = 0;

#define RCVD_DEST_ADDR ( ((tsCLD_ZllCommissionCustomDataStructure*)psEvent-> psCallBackEvent->psClusterInstance->pvEndPointCustomStructPtr)->sRxInterPanAddr.sDstAddr )


    uint8 u8Seq;
    ZPS_tsNwkNib *psNib;

    tsCLD_ZllCommissionCallBackMessage *psZllMessage = NULL;
    if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG)
    {
        /* get deatil of inter pan message */

        psZllMessage = &((tsCLD_ZllCommissionCustomDataStructure*)psEvent-> psCallBackEvent->psClusterInstance->pvEndPointCustomStructPtr)->sCallBackMessage;
        sSrcAddr = ((tsCLD_ZllCommissionCustomDataStructure*)psEvent-> psCallBackEvent->psClusterInstance->pvEndPointCustomStructPtr)->sRxInterPanAddr.sSrcAddr;
        u8Lqi = psEvent->psCallBackEvent->pZPSevent->uEvent.sApsInterPanDataIndEvent.u8LinkQuality;

    /*    DBG_vPrintf(1, "\r\r\nIP Cmd %d From %016llx\r\n",
                psZllMessage->u8CommandId,
                sSrcAddr.uAddress.u64Addr);
*/
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

    memset( &sZllCommand, 0, sizeof(tsZllPayloads));


    //DBG_vPrintf(TRACE_COMMISSION, "\r\nCOM: Got event %d in state %d\r\n", psEvent->eType, sCommission.eState);


    psNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    switch (sCommission.eState)
    {
        case E_IDLE:
            if (psEvent->eType == BDB_E_ZCL_EVENT_TL_IPAN_MSG)
            {
                if (psZllMessage->u8CommandId == E_CLD_COMMISSION_CMD_SCAN_REQ)
                {
                    if ( u8Lqi > TL_SCAN_LQI_MIN)
                    {
                        DBG_vPrintf(TRACE_COMMISSION, "\r\nScan Req LQI %d\r\n", u8Lqi);
                        if ( (psZllMessage->uMessage.psScanReqPayload->u8ZllInfo & TL_LINK_INIT) == 0 )
                        {
                            DBG_vPrintf(TRACE_TL_NEGATIVE, "Inistiator bit not set in req\r\n");
                            return;
                        }


                        /* Turn down Tx power */
#if ADJUST_POWER
                        eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, TX_POWER_LOW);
#endif
                        sCommission.u8Flags = 0;
                        if ((psZllMessage->uMessage.psScanReqPayload->u8ZigbeeInfo & TL_TYPE_MASK) != TL_ZED)
                        {
                            // Not a ZED requester, set FFD bit in flags
                            sCommission.u8Flags |= 0x02;
                        }
                        if (psZllMessage->uMessage.psScanReqPayload->u8ZigbeeInfo
                                & TL_RXON_IDLE)
                        {
                            // RxOnWhenIdle, so set RXON and power source bits in the flags
                            sCommission.u8Flags |= 0x0c;
                        }

                        sCommission.bProfileInterOp = ( (psZllMessage->uMessage.psScanReqPayload->u8ZllInfo & TL_PROFILE_INTEROP) == TL_PROFILE_INTEROP);

                        sDstAddr = sSrcAddr;
                        DBG_vPrintf(TRACE_JOIN, "Back to %016llx Mode %d\r\n", sDstAddr.uAddress.u64Addr, sDstAddr.eMode);
                        sDstAddr.u16PanId = 0xffff;
                        sCommission.u32TransactionId = psZllMessage->uMessage.psScanReqPayload->u32TransactionId;
                        sCommission.u32ResponseId = RND_u32GetRand(1, 0xffffffff);
                        if ( 0 == BDB_eTlSendScanResponse( psNib, &sDstAddr, sCommission.u32TransactionId, sCommission.u32ResponseId)) {
                            sCommission.eState = E_ACTIVE;
                            /* Timer to end inter pan */
                            ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(TL_INTERPAN_LIFE_TIME_SEC));
                        }
                    }
                    else
                    {
                        /* LQI too low */
                        DBG_vPrintf(TRACE_COMMISSION, "\r\nDrop Scan LQI %d\r\n", u8Lqi);
                    }
                } // end scan req
            }
            break;

        case E_ACTIVE:
            switch (psEvent->eType)
            {
                case BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED:
                    DBG_vPrintf(TRACE_COMMISSION, "Inter Pan Timed Out\r\n");
                    sCommission.eState = E_IDLE;
                    sCommission.u32TransactionId = 0;
                    sCommission.u32ResponseId = 0;
                    sCommission.bProfileInterOp = FALSE;
                    /* Turn up Tx power */
#if ADJUST_POWER
                    eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, TX_POWER_NORMAL);
#endif
                    if (sZllState.eState == NOT_FACTORY_NEW)
                    {
                        sBDB.sAttrib.bTLStealNotAllowed = TRUE;
                    }
                    break;

                case BDB_E_ZCL_EVENT_TL_IPAN_MSG:
                    sDstAddr = sSrcAddr;
                    sDstAddr.u16PanId = 0xFFFF;
                    DBG_vPrintf(TRACE_JOIN, "IP cmd 0x%02x\r\n", psZllMessage->u8CommandId);

                    switch (psZllMessage->u8CommandId)
                    {

                            case E_CLD_COMMISSION_CMD_SCAN_REQ:
                                if (psZllMessage->uMessage.psScanReqPayload->u32TransactionId != sCommission.u32TransactionId)
                                {
                                    /*
                                     * New scan request
                                     */
                                    if (u8Lqi > TL_SCAN_LQI_MIN)
                                    {
                                        sCommission.u8Flags = 0;
                                        if ((psZllMessage->uMessage.psScanReqPayload->u8ZigbeeInfo & TL_TYPE_MASK) != TL_ZED)
                                        {
                                            // Not a ZED requester, set FFD bit in flags
                                            sCommission.u8Flags |= 0x02;
                                        }
                                        if (psZllMessage->uMessage.psScanReqPayload->u8ZigbeeInfo
                                                & TL_RXON_IDLE)
                                        {
                                            // RxOnWhenIdle, so set RXON and power source bits in the flags
                                            sCommission.u8Flags |= 0x0c;
                                        }

                                        sCommission.bProfileInterOp = ( (psZllMessage->uMessage.psScanReqPayload->u8ZllInfo & TL_PROFILE_INTEROP) == TL_PROFILE_INTEROP);

                                        sDstAddr = sSrcAddr;
                                        DBG_vPrintf(TRACE_JOIN, "New scan Back to %016llx Mode %d\r\n", sDstAddr.uAddress.u64Addr, sDstAddr.eMode);
                                        sDstAddr.u16PanId = 0xffff;
                                        sCommission.u32TransactionId = psZllMessage->uMessage.psScanReqPayload->u32TransactionId;
                                        sCommission.u32ResponseId = RND_u32GetRand(1, 0xffffffff);
                                        if ( 0 == BDB_eTlSendScanResponse( psNib, &sDstAddr, sCommission.u32TransactionId, sCommission.u32ResponseId))
                                        {
                                            /* Timer to end inter pan */
                                            ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_SEC(TL_INTERPAN_LIFE_TIME_SEC));
                                        }
                                    }
                                }

                            break;
                        case E_CLD_COMMISSION_CMD_FACTORY_RESET_REQ:
                            if ( (sZllState.eState == NOT_FACTORY_NEW) &&
                                (psZllMessage->uMessage.psFactoryResetPayload->u32TransactionId == sCommission.u32TransactionId))
                            {
                                sCommission.eState = E_WAIT_LEAVE_RESET;
                                ZPS_eAplZdoLeaveNetwork(0, FALSE, FALSE);
                                sBDB.sAttrib.bLeaveRequested = TRUE;
                            }
                            break;

                        case E_CLD_COMMISSION_CMD_NETWORK_UPDATE_REQ:
                            DBG_vPrintf(TRACE_COMMISSION, "Got nwk up req\r\n");
                            if ( (psZllMessage->uMessage.psNwkUpdateReqPayload->u32TransactionId == sCommission.u32TransactionId) &&
                                 (psZllMessage->uMessage.psNwkUpdateReqPayload->u64ExtPanId == psNib->sPersist.u64ExtPanId) &&
                                 (psZllMessage->uMessage.psNwkUpdateReqPayload->u16PanId == psNib->sPersist.u16VsPanId) &&
                                    (psNib->sPersist.u8UpdateId != BDB_u8TlNewUpdateID( psNib->sPersist.u8UpdateId,
                                                                                     psZllMessage->uMessage.psNwkUpdateReqPayload->u8NwkUpdateId)))
                            {
                                /* Update the UpdateId, Nwk addr and Channel */
                                void * pvNwk = ZPS_pvAplZdoGetNwkHandle();
                                ZPS_vNwkNibSetUpdateId( pvNwk, psZllMessage->uMessage.psNwkUpdateReqPayload->u8NwkUpdateId);
                                    ZPS_vNwkNibSetNwkAddr( pvNwk, psZllMessage->uMessage.psNwkUpdateReqPayload->u16NwkAddr);
                                ZPS_vNwkNibSetChannel( pvNwk, psZllMessage->uMessage.psNwkUpdateReqPayload->u8LogicalChannel);
                                DBG_vPrintf(TRACE_COMMISSION, "Nwk update to %d\r\n", psZllMessage->uMessage.psNwkUpdateReqPayload->u8LogicalChannel);
                            }
                            break;

                        case E_CLD_COMMISSION_CMD_IDENTIFY_REQ:
                            if (psZllMessage->uMessage.psIdentifyReqPayload->u32TransactionId == sCommission.u32TransactionId)
                            {
                                if (psZllMessage->uMessage.psIdentifyReqPayload->u16Duration == 0xFFFF)
                                {
                                    DBG_vPrintf(TRACE_COMMISSION, "Default Id time\r\n");
                                    psZllMessage->uMessage.psIdentifyReqPayload->u16Duration = 3;
                                }

                                /** Identfiy time goes at 1Hz **/
                                APP_ZCL_vSetIdentifyTime( psZllMessage->uMessage.psIdentifyReqPayload->u16Duration);
                                APP_vHandleIdentify(  psZllMessage->uMessage.psIdentifyReqPayload->u16Duration);
                            }
                            break;

                        case E_CLD_COMMISSION_CMD_DEVICE_INFO_REQ:
                            if (psZllMessage->uMessage.psDeviceInfoReqPayload->u32TransactionId == sCommission.u32TransactionId)
                            {
                                DBG_vPrintf(TRACE_JOIN, "Device Info request with Idx %d\r\n", psZllMessage->uMessage.psDeviceInfoReqPayload->u8StartIndex);

                                sZllCommand.uPayload.sDeviceInfoRspPayload.u32TransactionId = psZllMessage->uMessage.psDeviceInfoReqPayload->u32TransactionId;
                                sZllCommand.uPayload.sDeviceInfoRspPayload.u8NumberSubDevices = ZLO_NUMBER_DEVICES;
                                sZllCommand.uPayload.sDeviceInfoRspPayload.u8StartIndex = psZllMessage->uMessage.psDeviceInfoReqPayload->u8StartIndex;
                                sZllCommand.uPayload.sDeviceInfoRspPayload.u8DeviceInfoRecordCount = 0;
                                // copy from table sDeviceTable
                                if (psZllMessage->uMessage.psDeviceInfoReqPayload->u8StartIndex <= ZLO_NUMBER_DEVICES)
                                {

                                    int i,j;
                                    // todo test start index against no of devices first
                                    j = psZllMessage->uMessage.psDeviceInfoReqPayload->u8StartIndex;
                                    for (i = 0; i < TL_MAX_DEVICE_RECORDS && j < ZLO_NUMBER_DEVICES; i++, j++)
                                    {
                                        sZllCommand.uPayload.sDeviceInfoRspPayload.asDeviceRecords[i] = sDeviceTable.asDeviceRecords[j];
                                        sZllCommand.uPayload.sDeviceInfoRspPayload.u8DeviceInfoRecordCount++;
                                        if (sCommission.bProfileInterOp == FALSE)
                                        {
                                            sZllCommand.uPayload.sDeviceInfoRspPayload.asDeviceRecords[i].u16DeviceId = TL_LEGACY_DEVICE_ID;
                                            sZllCommand.uPayload.sDeviceInfoRspPayload.asDeviceRecords[i].u16ProfileId = TL_LEGACY_PROFILE_ID;
                                            sZllCommand.uPayload.sDeviceInfoRspPayload.asDeviceRecords[i].u8Version = TL_LEGACY_VERSION_ID;
                                        }

                                    }

                                }
                                else
                                {
                                    // start index > No of sub Devices
                                    DBG_vPrintf(TRACE_TL_NEGATIVE, "Start Index out of bounds\r\n");
                                }

                                eCLD_ZllCommissionCommandDeviceInfoRspCommandSend( &sDstAddr,
                                                                                   &u8Seq,
                                                                                   (tsCLD_ZllCommission_DeviceInfoRspCommandPayload*) &sZllCommand.uPayload);


                            }
                            break;

                        case E_CLD_COMMISSION_CMD_NETWORK_JOIN_END_DEVICE_REQ:
                        	/* zb3.0 comment resolution, drop silently */
                            break;

                        case E_CLD_COMMISSION_CMD_NETWORK_START_REQ:
                            DBG_vPrintf(TRACE_COMMISSION, "start request\r\n");
                            if ( ( psZllMessage->uMessage.psNwkStartReqPayload->u32TransactionId != sCommission.u32TransactionId) ||
                               ( psZllMessage->uMessage.psNwkStartReqPayload->u64InitiatorIEEEAddr != sSrcAddr.uAddress.u64Addr) ||
                                 (psZllMessage->uMessage.psNwkStartReqPayload->u16NwkAddr == 0) ||
                                 (psZllMessage->uMessage.psNwkStartReqPayload->u16NwkAddr > 0xfff6 ) ||
                                 ((psZllMessage->uMessage.psNwkStartReqPayload->u8LogicalChannel > 0) && (psZllMessage->uMessage.psNwkStartReqPayload->u8LogicalChannel < 11)) ||
                                 (psZllMessage->uMessage.psNwkStartReqPayload->u8LogicalChannel > 26) ||
                                 ( FALSE == BDB_bTlIsKeySupported( psZllMessage->uMessage.psNwkStartReqPayload->u8KeyIndex))

                               )
                            {
                               /* bad params, drop silently */
                                return;
                            }
                            else if (( sBDB.sAttrib.bTLStealNotAllowed == TRUE) && (sZllState.eState == NOT_FACTORY_NEW))
                            {
                                /* send start rsp */
                                DBG_vPrintf(TRACE_TL_NEGATIVE, "Start Req wrong Trans Id\r\n");
                                sZllCommand.uPayload.sNwkStartRspPayload.u32TransactionId = sCommission.u32TransactionId;
                                sZllCommand.uPayload.sNwkStartRspPayload.u8Status = TL_ERROR;
                                eCLD_ZllCommissionCommandNetworkStartRspCommandSend( &sDstAddr,
                                                                                     &u8Seq,
                                                                                     (tsCLD_ZllCommission_NetworkStartRspCommandPayload*) &sZllCommand.uPayload);
                            }
                            else
                            {
                                sStartParams.sNwkParams.u64ExtPanId = psZllMessage->uMessage.psNwkStartReqPayload->u64ExtPanId;
                                sStartParams.sNwkParams.u8KeyIndex = psZllMessage->uMessage.psNwkStartReqPayload->u8KeyIndex;
                                sStartParams.sNwkParams.u8LogicalChannel = psZllMessage->uMessage.psNwkStartReqPayload->u8LogicalChannel;
                                sStartParams.sNwkParams.u16PanId = psZllMessage->uMessage.psNwkStartReqPayload->u16PanId;
                                sStartParams.sNwkParams.u16NwkAddr  = psZllMessage->uMessage.psNwkStartReqPayload->u16NwkAddr;
                                sStartParams.u16GroupIdBegin = psZllMessage->uMessage.psNwkStartReqPayload->u16GroupIdBegin;
                                sStartParams.u16GroupIdEnd = psZllMessage->uMessage.psNwkStartReqPayload->u16GroupIdEnd;
                                sStartParams.u16FreeGroupIdBegin = psZllMessage->uMessage.psNwkStartReqPayload->u16FreeGroupIdBegin;
                                sStartParams.u16FreeGroupIdEnd = psZllMessage->uMessage.psNwkStartReqPayload->u16FreeGroupIdEnd;
                                sStartParams.u16FreeNwkAddrBegin = psZllMessage->uMessage.psNwkStartReqPayload->u16FreeNwkAddrBegin;
                                sStartParams.u16FreeNwkAddrEnd  = psZllMessage->uMessage.psNwkStartReqPayload->u16FreeNwkAddrEnd;
                                sStartParams.u64InitiatorIEEEAddr = psZllMessage->uMessage.psNwkStartReqPayload->u64InitiatorIEEEAddr;
                                sStartParams.u16InitiatorNwkAddr = psZllMessage->uMessage.psNwkStartReqPayload->u16InitiatorNwkAddr;
                                sStartParams.sNwkParams.u8NwkupdateId = 1;
                                sStartParams.sNwkParams.pu8NwkKey = sStartParams.au8NwkKey;
                                memcpy( sStartParams.au8NwkKey,
                                        psZllMessage->uMessage.psNwkStartReqPayload->au8NwkKey,
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
                                        DBG_vPrintf(TRACE_COMMISSION, "Gen Epid\r\n");
                                    }
                                    if (sStartParams.sNwkParams.u16PanId == 0)
                                    {
                                        sStartParams.sNwkParams.u16PanId = RND_u32GetRand( 1, 0xfffe);
                                        DBG_vPrintf(TRACE_COMMISSION, "Gen pan\r\n");
                                    }
                                    DBG_vPrintf(TRACE_COMMISSION, "Do discovery\r\n");
                                    ZPS_eAplZdoDiscoverNetworks(  BDBC_TL_PRIMARY_CHANNEL_SET);
                                    sCommission.eState = E_WAIT_DISCOVERY;
                                    ZTIMER_eStop(u8TimerBdbTl);
                                    ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(650));
                                }
                                else
                                {
                                    sCommission.eState = E_SKIP_DISCOVERY;
                                    DBG_vPrintf(TRACE_COMMISSION, "Skip discovery\r\n");
                                    ZTIMER_eStop(u8TimerBdbTl);
                                    ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
                                }
                            }
                            break;

                        case E_CLD_COMMISSION_CMD_NETWORK_JOIN_ROUTER_REQ:
                            DBG_vPrintf(TRACE_JOIN, "Active join router req\r\n");
                            sZllCommand.uPayload.sNwkJoinRouterRspPayload.u32TransactionId = sCommission.u32TransactionId;

                            sZllCommand.uPayload.sNwkJoinRouterRspPayload.u8Status  = TL_SUCCESS;
                            if ( (psZllMessage->uMessage.psNwkJoinRouterReqPayload->u32TransactionId != sCommission.u32TransactionId) ||
                                 (psZllMessage->uMessage.psNwkJoinRouterReqPayload->u64ExtPanId == 0) ||
                                 (psZllMessage->uMessage.psNwkJoinRouterReqPayload->u64ExtPanId == 0xffffffffff) ||
                                 (psZllMessage->uMessage.psNwkJoinRouterReqPayload->u8LogicalChannel < 11) ||
                                 (psZllMessage->uMessage.psNwkJoinRouterReqPayload->u8LogicalChannel > 26) ||
                                 ( FALSE == BDB_bTlIsKeySupported(psZllMessage->uMessage.psNwkJoinRouterReqPayload->u8KeyIndex))
                               )
                            {
                                /* bad params drop silently */
                                return;
                            }
                            else if (( sBDB.sAttrib.bTLStealNotAllowed == TRUE) && (sZllState.eState == NOT_FACTORY_NEW))
                            {
                                DBG_vPrintf( TRACE_COMMISSION, "Invalid start params reject\r\n");

                                sZllCommand.uPayload.sNwkJoinRouterRspPayload.u8Status  = TL_ERROR;
                                eCLD_ZllCommissionCommandNetworkJoinRouterRspCommandSend( &sDstAddr,
                                                                                          &u8Seq,
                                                                                          (tsCLD_ZllCommission_NetworkJoinRouterRspCommandPayload*) &sZllCommand.uPayload);
                            }
                            else
                            {

                                /* Turn up Tx power */
#if ADJUST_POWER
                                eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, TX_POWER_NORMAL);
#endif

                                DBG_vPrintf( TRACE_COMMISSION, "Set channel %d\r\n", psZllMessage->uMessage.psNwkJoinRouterReqPayload->u8LogicalChannel);

                                eCLD_ZllCommissionCommandNetworkJoinRouterRspCommandSend( &sDstAddr,
                                                                                          &u8Seq,
                                                                                          (tsCLD_ZllCommission_NetworkJoinRouterRspCommandPayload*) &sZllCommand.uPayload);

                                sStartParams.sNwkParams.u64ExtPanId  = psZllMessage->uMessage.psNwkJoinRouterReqPayload->u64ExtPanId;
                                sStartParams.sNwkParams.u8KeyIndex = psZllMessage->uMessage.psNwkJoinRouterReqPayload->u8KeyIndex;
                                sStartParams.sNwkParams.u8LogicalChannel = psZllMessage->uMessage.psNwkJoinRouterReqPayload->u8LogicalChannel;
                                sStartParams.sNwkParams.u16PanId  = psZllMessage->uMessage.psNwkJoinRouterReqPayload->u16PanId;
                                sStartParams.sNwkParams.u16NwkAddr = psZllMessage->uMessage.psNwkJoinRouterReqPayload->u16NwkAddr;
                                sStartParams.u16GroupIdBegin = psZllMessage->uMessage.psNwkJoinRouterReqPayload->u16GroupIdBegin;
                                sStartParams.u16GroupIdEnd = psZllMessage->uMessage.psNwkJoinRouterReqPayload->u16GroupIdEnd;
                                sStartParams.u16FreeGroupIdBegin = psZllMessage->uMessage.psNwkJoinRouterReqPayload->u16FreeGroupIdBegin;
                                sStartParams.u16FreeGroupIdEnd = psZllMessage->uMessage.psNwkJoinRouterReqPayload->u16FreeGroupIdEnd;
                                sStartParams.u16FreeNwkAddrBegin = psZllMessage->uMessage.psNwkJoinRouterReqPayload->u16FreeNwkAddrBegin;
                                sStartParams.u16FreeNwkAddrEnd = psZllMessage->uMessage.psNwkJoinRouterReqPayload->u16FreeNwkAddrEnd;
                                sStartParams.u64InitiatorIEEEAddr = 0;
                                sStartParams.u16InitiatorNwkAddr = 0;
                                sStartParams.sNwkParams.u8NwkupdateId = psZllMessage->uMessage.psNwkJoinRouterReqPayload->u8NwkUpdateId;
                                sStartParams.sNwkParams.pu8NwkKey = sStartParams.au8NwkKey;

                                memcpy( sStartParams.au8NwkKey,
                                        psZllMessage->uMessage.psNwkJoinRouterReqPayload->au8NwkKey,
                                        16);

                                if (sZllState.eState == FACTORY_NEW)
                                {
                                    sCommission.eState = E_START_ROUTER;
                                }
                                else
                                {
                                    sCommission.eState = E_WAIT_LEAVE;
                                    /* leave req */
                                    ZPS_eAplZdoLeaveNetwork(0, FALSE, FALSE);
                                    sBDB.sAttrib.bLeaveRequested = TRUE;
                                }
                                ZTIMER_eStop(u8TimerBdbTl);
                                ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
                            }
                            break;

                        default:
                            if (psZllMessage->u8CommandId != 0)
                            {
                                DBG_vPrintf(TRACE_JOIN, "Active unhandled Cmd %02x\r\n", psZllMessage->u8CommandId);
                            }
                            break;

                    }
                    break;

                default:
                    break;
            }
            break;

        case E_WAIT_DISCOVERY:
            if (psEvent->eType == BDB_E_ZCL_EVENT_DISCOVERY_DONE)
            {
                DBG_vPrintf(TRACE_COMMISSION, "discovery in commissioning\r\n");
                /* get unique set of pans */
                while (!BDB_bTlSearchDiscNt(psNib, sStartParams.sNwkParams.u64ExtPanId,
                        sStartParams.sNwkParams.u16PanId))
                {
                    sStartParams.sNwkParams.u16PanId = RND_u32GetRand(1, 0xfffe);
                    sStartParams.sNwkParams.u64ExtPanId = RND_u32GetRand(1, 0xffffffff);
                    sStartParams.sNwkParams.u64ExtPanId <<= 32;
                    sStartParams.sNwkParams.u64ExtPanId |= RND_u32GetRand(0, 0xffffffff);
                };
                //DBG_vPrintf(TRACE_JOIN, "New Epid %016llx Pan %04x\r\n", sStartParams.u64ExtPanId, sStartParams.u16PanId);
            }
            else if (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED)
            {
                DBG_vPrintf(TRACE_COMMISSION, "discovery timed out\r\n");

            }
            // Deliberate fall through

        case E_SKIP_DISCOVERY:
            DBG_vPrintf(TRACE_JOIN, "New Epid %016llx Pan %04x\r\n",
                    sStartParams.sNwkParams.u64ExtPanId, sStartParams.sNwkParams.u16PanId);
            DBG_vPrintf(TRACE_COMMISSION, "e_skip-discovery Ch %d\r\n", sStartParams.sNwkParams.u8LogicalChannel);
            if (sStartParams.sNwkParams.u8LogicalChannel == 0)
            {
                sStartParams.sNwkParams.u8LogicalChannel = BDB_u8TlGetRandomPrimary();
                DBG_vPrintf(TRACE_JOIN, "Picked Ch %d\r\n", sStartParams.sNwkParams.u8LogicalChannel);

            }

            /* send start rsp */
            sZllCommand.uPayload.sNwkStartRspPayload.u32TransactionId = sCommission.u32TransactionId;
            sZllCommand.uPayload.sNwkStartRspPayload.u8Status = TL_SUCCESS;
            sZllCommand.uPayload.sNwkStartRspPayload.u64ExtPanId = sStartParams.sNwkParams.u64ExtPanId;
            sZllCommand.uPayload.sNwkStartRspPayload.u8NwkUpdateId = sStartParams.sNwkParams.u8NwkupdateId;
            sZllCommand.uPayload.sNwkStartRspPayload.u8LogicalChannel = sStartParams.sNwkParams.u8LogicalChannel;
            sZllCommand.uPayload.sNwkStartRspPayload.u16PanId = sStartParams.sNwkParams.u16PanId;

            eCLD_ZllCommissionCommandNetworkStartRspCommandSend( &sDstAddr,
                                                                 &u8Seq,
                                                                 (tsCLD_ZllCommission_NetworkStartRspCommandPayload*) &sZllCommand.uPayload);

            if (sZllState.eState == FACTORY_NEW)
            {
                sCommission.eState = E_START_ROUTER;
                /* give message time to go */
                ZTIMER_eStop(u8TimerBdbTl);
                ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
            }
            else
            {
                sCommission.eState = E_WAIT_LEAVE;
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
                DBG_vPrintf(TRACE_JOIN, "send leave outFC=%08x\r\n", psNib->sTbl.u32OutFC);
#else
                DBG_vPrintf(TRACE_JOIN, "send leave outFC=%08x\r\n", psNib->sPersist.u32OutFC);
#endif
                ZPS_eAplZdoLeaveNetwork(0, FALSE, FALSE);
                sBDB.sAttrib.bLeaveRequested = TRUE;
            }
            break;

        case E_WAIT_LEAVE:
            DBG_vPrintf(TRACE_JOIN, "Wait leave\r\n");
            if (psEvent->eType == BDB_E_ZCL_EVENT_LEAVE_CFM)
            {
                sCommission.eState = E_START_ROUTER;
                ZTIMER_eStop(u8TimerBdbTl);
                APP_vRemoveAllGroupsAndScenes();
                ZPS_vSetKeys();
                ZTIMER_eStart(u8TimerBdbTl, ZTIMER_TIME_MSEC(10));
                sBDB.sAttrib.bLeaveRequested = FALSE;
            }
            break;

        case E_WAIT_LEAVE_RESET:
            if ((psEvent->eType == BDB_E_ZCL_EVENT_LEAVE_CFM) || (psEvent->eType == BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED))
            {
                DBG_vPrintf(TRACE_JOIN, "WARNING: Received Factory reset \r\n");

                APP_vRemoveAllGroupsAndScenes();
                APP_vFactoryResetRecords();
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
                vAHI_SwReset();
#else
                RESET_SystemReset();
#endif
            }
            break;

        case E_START_ROUTER:
            DBG_vPrintf(TRACE_COMMISSION, "\r\n>Start router<\r\n");

            BDB_eTlDecryptKey( sStartParams.au8NwkKey,
                                sStartParams.au8NwkKey,
                                sCommission.u32TransactionId,
                                sCommission.u32ResponseId,
                                sStartParams.sNwkParams.u8KeyIndex);

            /* This sets up the device, starts as router and sends device ance */
            ZPS_eAplFormDistributedNetworkRouter(&sStartParams.sNwkParams, TRUE);


            if (sStartParams.u16InitiatorNwkAddr != 0)
            {
                ZPS_eAplZdoDirectJoinNetwork(sStartParams.u64InitiatorIEEEAddr, sStartParams.u16InitiatorNwkAddr, sCommission.u8Flags );
                DBG_vPrintf(TRACE_JOIN, "Direct join %02x\r\n", sCommission.u8Flags);
            }

            sZllState.eState = NOT_FACTORY_NEW;
            sZllState.eNodeState = E_RUNNING;
            PDM_eSaveRecordData(PDM_ID_APP_TL_ROUTER,&sZllState,sizeof(tsZllState));

            /* This is an application specific decision not to allow steering
             * this limit may be reversed or temporarily changed
             * under application control */
            sBDB.sAttrib.bbdbNodeIsOnANetwork = TRUE;
            sBDB.sAttrib.bTLStealNotAllowed = TRUE;


#if PERMIT_JOIN
            ZPS_eAplZdoPermitJoining( PERMIT_JOIN_TIME);
#endif

            sCommission.eState = E_IDLE;
            sCommission.u32TransactionId = 0;
            sCommission.u32ResponseId = 0;
            sCommission.bProfileInterOp = FALSE;
            ZTIMER_eStop(u8TimerBdbTl);
            if (sZllState.eState == NOT_FACTORY_NEW)
            {
                sBDB.sAttrib.bTLStealNotAllowed = TRUE;
            }
            DBG_vPrintf(TRACE_JOIN, "All done\r\n");
            break;

        default:
            break;

    }

}

/****************************************************************************
 *
 * NAME: BDB_vTlTimerCb
 *
 * DESCRIPTION:
 * Handles commissioning timer expiry events
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void BDB_vTlTimerCb(void *pvParam) {
    tsBDB_ZCLEvent sEvent;
    sEvent.eType = BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED;
    BDB_vTlStateMachine( &sEvent);
}

/****************************************************************************
 *
 * NAME: eSendScanResponse
 *
 * DESCRIPTION:
 * sends the scan response message in response to a scan request
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE teZCL_Status BDB_eTlSendScanResponse(ZPS_tsNwkNib *psNib,
                               ZPS_tsInterPanAddress       *psDstAddr,
                               uint32 u32TransactionId,
                               uint32 u32ResponseId)
{
    tsCLD_ZllCommission_ScanRspCommandPayload sScanRsp;
    uint8 u8Seq;

    memset( &sScanRsp, 0, sizeof(tsCLD_ZllCommission_ScanRspCommandPayload));
    //u32TransactionId = psZllMessage->uPayload.sScanReqPayload.u32TransactionId;
    sScanRsp.u32TransactionId = u32TransactionId;

    //u32ResponseId = RND_u32GetRand(1, 0xffffffff);
    sScanRsp.u32ResponseId = u32ResponseId;
    sScanRsp.u8RSSICorrection = RSSI_CORRECTION;
    sScanRsp.u8ZigbeeInfo = TL_ROUTER | TL_RXON_IDLE;
    sScanRsp.u16KeyMask = TL_SUPPORTED_KEYS;
    sScanRsp.u8ZllInfo = (sZllState.eState == FACTORY_NEW) ? (TL_PROFILE_INTEROP|TL_FACTORY_NEW) : (TL_PROFILE_INTEROP);
    sScanRsp.u64ExtPanId  = psNib->sPersist.u64ExtPanId;
    sScanRsp.u8NwkUpdateId = psNib->sPersist.u8UpdateId;
    sScanRsp.u8LogicalChannel = psNib->sPersist.u8VsChannel;
    uint32 u32temp;
    eAppApiPlmeGet(PHY_PIB_ATTR_CURRENT_CHANNEL, &u32temp);
    DBG_vPrintf(TRACE_JOIN, "Send Scan Response %d Actual %d\r\n", sScanRsp.u8LogicalChannel, u32temp);
    if (sZllState.eState == TL_FACTORY_NEW)
    {
        sScanRsp.u16PanId = 0xFFFF;
    }else
    {
        sScanRsp.u16PanId = psNib->sPersist.u16VsPanId;
    }
    sScanRsp.u16NwkAddr = psNib->sPersist.u16NwkAddr;

    sScanRsp.u8TotalGroupIds = 0;
    sScanRsp.u8NumberSubDevices = sDeviceTable.u8NumberDevices;
    if (sScanRsp.u8NumberSubDevices  == 1)
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
        sScanRsp.u8GroupIdCount = 0;
    }
    //sDstAddr = sEvent.sZllMessage.sSrcAddr;
    DBG_vPrintf(TRACE_JOIN, "Back to %016llx Mode %d\r\n", sDstAddr.uAddress.u64Addr, sDstAddr.eMode);
    sDstAddr.u16PanId = 0xffff;
    return eCLD_ZllCommissionCommandScanRspCommandSend( psDstAddr /*&sEvent.sZllMessage.sSrcAddr*/,
                                                            &u8Seq,
                                                            &sScanRsp);

}


PRIVATE bool_t BDB_bTlSearchDiscNt(ZPS_tsNwkNib *psNib, uint64 u64EpId, uint16 u16PanId) {

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

