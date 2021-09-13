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
# COMPONENT:   bdb_nf.c
#
# DESCRIPTION: BDB Network Formation implementation
#              
#
###############################################################################*/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
#include "bdb_api.h"
#include "bdb_start.h"
#include "bdb_nf.h"
#include "dbg.h"
#include "rnd_pub.h"
#include <string.h>
#include <stdlib.h>
#include "zps_apl_af.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define BDB_TC_STATUS_STD_SEC_REJOIN          0
#define BDB_TC_STATUS_STD_UNSEC_JOIN          1
#define BDB_TC_STATUS_DEVICE_LEFT             2
#define BDB_TC_STATUS_STD_UNSEC_REJOIN        3
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void vNfDiscoverNwk(void);
PRIVATE void vNfFormDistributedNwk(void);
PRIVATE void vNfRetryNwkFormation(void);
PRIVATE bool_t bNfSearchDiscNt(uint64 u64EpId, uint16 u16PanId);
#if (BDB_SET_DEFAULT_TC_POLICY == TRUE) 
PRIVATE bool_t vNfTcCallback (uint16 u16ShortAddress,
                             uint64 u64DeviceAddress,
                             uint64 u64ParentAddress,
                             uint8 u8Status,
                             uint16 u16MacId );
#endif
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
PUBLIC teNF_State eNF_State = E_NF_IDLE;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
static tsStartParams sStartParams;
static bool_t bDoPrimaryScan;
static uint32 u32ScanChannels;
static uint32 u32BackUpApsChannelMask;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: BDB_eNfStartNwkFormation
 *
 * DESCRIPTION: BDB specification section 8.4
 *  Forms centralized secure network if device is ZigBee Coordinator.
 *  Forms distributed secure network if device is ZigBee Router
 *  
 *  Above network formation are tried on primary channel set first. If nwk
 *  formation failed on primary channel set then it's tried on secondary 
 *  channel set. Channels are actively scanned to choose unique pan id.
 * 
 *  ZPS_eAplZdoStartStack is used to form centralized network.
 *  
 *  For distributed nwk formation
 *      a. Channel selection is random if RAND_CHANNEL is set to TRUE
 *      b. RAND_DISTRIBUTED_NWK_KEY can be set to TRUE, during application
 *          development it's set to FALSE in order to sniff all over the air
 *          packets.
 *      c. Random and unique panId and ExtPanId are choosen.
 *      d. Network address is randomly selected.
 *
 * PARAMETERS:      Name            RW  Usage
 *
 * RETURNS:
 * BDB_E_ERROR_INVALID_PARAMETER,   if ZED tries to for the nwk
 * BDB_E_ERROR_NODE_IS_ON_A_NWK,    if node is already on the nwk
 * BDB_E_SUCCESS, if network formation process in progress.
 *                                  In this case application must watch for
 *                                  below async events to come.
 * BDB_E_ERROR_COMMISSIONING_IN_PROGRESS
 *
 * EVENTS:
 * BDB_EVENT_NWK_FORMATION_SUCCESS, if centralized or distributed network
 *                                  formation was successful based on local
 *                                  device type
 * BDB_EVENT_NWK_FORMATION_FAILURE, if all network formation attempts failed
 *
 ****************************************************************************/
PUBLIC BDB_teStatus BDB_eNfStartNwkFormation(void)
{
    if(!(sBDB.sAttrib.u8bdbCommissioningMode & BDB_COMMISSIONING_MODE_NWK_FORMATION) || \
        (ZPS_ZDO_DEVICE_ENDDEVICE == ZPS_eAplZdoGetDeviceType()))
    {
        DBG_vPrintf(TRACE_BDB,"BDB: Param Check Trap \n");
        return BDB_E_ERROR_INVALID_PARAMETER;
    }
    if(sBDB.sAttrib.bbdbNodeIsOnANetwork)
    {
        DBG_vPrintf(TRACE_BDB,"BDB: Node is already on the network \n");
        return BDB_E_ERROR_NODE_IS_ON_A_NWK;
    }

    if(sBDB.sAttrib.ebdbCommissioningStatus == E_BDB_COMMISSIONING_STATUS_IN_PROGRESS)
    {
       return BDB_E_ERROR_COMMISSIONING_IN_PROGRESS;
    }

    sBDB.eState = E_STATE_BASE_SLAVE;

    /* Network formation procedure - BDB specification section 8.4 */
    sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_IN_PROGRESS;
    if(sBDB.sAttrib.u32bdbPrimaryChannelSet)
    {
        bDoPrimaryScan = TRUE;
        u32ScanChannels = sBDB.sAttrib.u32bdbPrimaryChannelSet;
    }
    else
    {
        bDoPrimaryScan = FALSE;
        u32ScanChannels = sBDB.sAttrib.u32bdbSecondaryChannelSet;
    }
#ifdef ENABLE_SUBG_IF
    u32ScanChannels |= SUBG_PAGE_28;
#endif
    /* Backup and restore after NwkForm confirmation */
    u32BackUpApsChannelMask = ZPS_psAplAibGetAib()->pau32ApsChannelMask[0];
    ZPS_psAplAibGetAib()->pau32ApsChannelMask[0] = u32ScanChannels;

    if(ZPS_ZDO_DEVICE_COORD == ZPS_eAplZdoGetDeviceType())
    {
        DBG_vPrintf(TRACE_BDB,"BDB: Forming Centralized Nwk \n");
        eNF_State = E_NF_WAIT_FORM_CENTRALIZED;
        BDB_vNfFormCentralizedNwk();
    }
    else //(ZPS_ZDO_DEVICE_ROUTER == ZPS_eAplZdoGetDeviceType())
    {
        DBG_vPrintf(TRACE_BDB,"BDB: Discovery and Forming Distributed Nwk \n");
        /* For Distributed nwk formation - Perform Discovery first and then formation */
        eNF_State = E_NF_WAIT_DISCOVERY;
        vNfDiscoverNwk();
    }
    return BDB_E_SUCCESS;
}

/****************************************************************************
 *
 * NAME: BDB_vNfStateMachine
 *
 * DESCRIPTION:
 *  BDB Network formation state machine; this is utilized internally by BDB.
 *
 * PARAMETERS:      Name            RW  Usage
 *                  BDB_tsZpsAfEvent    Zps event
 * RETURNS:
 *  void
 *
 ****************************************************************************/
PUBLIC void BDB_vNfStateMachine(BDB_tsZpsAfEvent *psZpsAfEvent)
{
    BDB_tsBdbEvent sBdbEvent;

    switch(eNF_State)
    {
        case E_NF_IDLE:
            break;

        case E_NF_WAIT_FORM_CENTRALIZED:
            switch(psZpsAfEvent->sStackEvent.eType)
            {
                /* For Centralized network */
                case ZPS_EVENT_NWK_STARTED:
                    ZPS_vSetTCLockDownOverride (ZPS_pvAplZdoGetAplHandle(), //ZPS_pvAplZdoGetNwkHandle(),
                                                FALSE,       //u8RemoteOverride
                                                FALSE);     // bDisableAuthentications
                    sBDB.sAttrib.bbdbNodeIsOnANetwork = TRUE;
                    sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_SUCCESS;

                    sBDB.eState = E_STATE_BASE_ACTIVE;
                    eNF_State = E_NF_IDLE;
                    sBdbEvent.eEventType = BDB_EVENT_NWK_FORMATION_SUCCESS;
                    APP_vBdbCallback(&sBdbEvent);
                    break;
                case ZPS_EVENT_NWK_FAILED_TO_START:
                    vNfRetryNwkFormation();
                    break;
                default:
                    break;
            }
            break;

        case E_NF_WAIT_DISCOVERY:
            switch(psZpsAfEvent->sStackEvent.eType)
            {
                /* For Distributed network */
                case ZPS_EVENT_NWK_DISCOVERY_COMPLETE:
                    eNF_State = E_NF_WAIT_FORM_DISTRIBUTED;
                    vNfFormDistributedNwk();
                    break;
                default:
                    break;
            }
            break;

        case E_NF_WAIT_FORM_DISTRIBUTED:
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
 * NAME: BDB_vNfFormCentralizedNwk
 *
 * DESCRIPTION:
 *  Forms a centralized network
 *
 * PARAMETERS:      Name            RW  Usage
 * 
 * RETURNS:
 *  void
 *
 ****************************************************************************/
PUBLIC void BDB_vNfFormCentralizedNwk(void)
{
    #if (BDB_SET_DEFAULT_TC_POLICY == TRUE)
        ZPS_vTCSetCallback(vNfTcCallback);
    #endif

    ZPS_eAplZdoStartStack();
}

/****************************************************************************
 *
 * NAME: vNfDiscoverNwk
 *
 * DESCRIPTION:
 *  Discovers network in order to form distributed network.
 *
 * PARAMETERS:      Name            RW  Usage
 * 
 * RETURNS:
 *  void
 *
 ****************************************************************************/
PRIVATE void vNfDiscoverNwk()
{
    ZPS_teStatus eStatus;
    /* Set the start up parameters - To be used later while forming - ZPS_EVENT_NWK_DISCOVERY_COMPLETE */
    sStartParams.sNwkParams.u8LogicalChannel = BDB_u8PickChannel(u32ScanChannels);
    sStartParams.sNwkParams.u16NwkAddr = RND_u32GetRand(1, 0xfffe);

    if (sStartParams.sNwkParams.u64ExtPanId == 0)
    {
        sStartParams.sNwkParams.u64ExtPanId = RND_u32GetRand(1, 0xffffffff);
        sStartParams.sNwkParams.u64ExtPanId <<= 32;
        sStartParams.sNwkParams.u64ExtPanId |= RND_u32GetRand(0, 0xffffffff);
    }
    if (sStartParams.sNwkParams.u16PanId == 0)
    {
        sStartParams.sNwkParams.u16PanId = RND_u32GetRand( 1, 0xfffe);
    }

    DBG_vPrintf(TRACE_BDB, "BDB: Disc to Form 0x%08x \n",ZPS_psAplAibGetAib()->pau32ApsChannelMask[0]);
    ZPS_vNwkNibClearDiscoveryNT(ZPS_pvAplZdoGetNwkHandle());

    eStatus = ZPS_eAplZdoDiscoverNetworks(ZPS_psAplAibGetAib()->pau32ApsChannelMask[0]);

    if(ZPS_E_SUCCESS != eStatus)
    {
        DBG_vPrintf(TRACE_BDB,"BDB: ZPS_eAplZdoDiscoverNetworks failed %04x !\n", eStatus);
        vNfRetryNwkFormation();
    }
}

/****************************************************************************
 *
 * NAME: vNfFormDistributedNwk
 *
 * DESCRIPTION:
 *  Distributed network formation
 *
 * PARAMETERS:      Name            RW  Usage
 * 
 * RETURNS:
 *  void
 *
 ****************************************************************************/
PRIVATE void vNfFormDistributedNwk(void)
{
    BDB_tsBdbEvent sBdbEvent;
    ZPS_teStatus eStatus;
    #if !(RAND_DISTRIBUTED_NWK_KEY)
        uint8 au8NwkKey[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                               0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    #endif

    /* get unique set of pans */
    while (!bNfSearchDiscNt(sStartParams.sNwkParams.u64ExtPanId, sStartParams.sNwkParams.u16PanId))
    {
        sStartParams.sNwkParams.u16PanId = RND_u32GetRand(1, 0xfffe);
        sStartParams.sNwkParams.u64ExtPanId = RND_u32GetRand(1, 0xffffffff);
        sStartParams.sNwkParams.u64ExtPanId <<= 32;
        sStartParams.sNwkParams.u64ExtPanId |= RND_u32GetRand(0, 0xffffffff);
    };

    #if RAND_DISTRIBUTED_NWK_KEY
        sStartParams.sNwkParams.pu8NwkKey = NULL;
    #else
        sStartParams.sNwkParams.pu8NwkKey = au8NwkKey;
    #endif
    sStartParams.sNwkParams.u8KeyIndex = 0;
    sStartParams.sNwkParams.u8NwkupdateId = 1;

#ifdef ENABLE_SUBG_IF
    void *pvNwk = ZPS_pvAplZdoGetNwkHandle();
	ZPS_vNwkNibSetMacEnhancedMode(pvNwk, TRUE);

    ZPS_u8MacMibIeeeSetPolicy(FALSE);
#endif

    /* This sets up the device, starts as router and sends device ance */
    eStatus = ZPS_eAplFormDistributedNetworkRouter(&sStartParams.sNwkParams, FALSE);

    if(ZPS_E_SUCCESS == eStatus)
    {
        sBDB.sAttrib.bbdbNodeIsOnANetwork = TRUE;
        sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_SUCCESS;

        sBDB.eState = E_STATE_BASE_ACTIVE;
        eNF_State = E_NF_IDLE;
        sBdbEvent.eEventType = BDB_EVENT_NWK_FORMATION_SUCCESS;
        APP_vBdbCallback(&sBdbEvent);
    }
    else
    {
        DBG_vPrintf(TRACE_BDB, "BDB: Couldn't Form Distributed Nwk %d !\n", eStatus);
    }
}

/****************************************************************************
 *
 * NAME: vNfRetryNwkFormation
 *
 * DESCRIPTION:
 *  
 *
 * PARAMETERS:      Name            RW  Usage
 * 
 * RETURNS:
 *  void
 *
 ****************************************************************************/
PRIVATE void vNfRetryNwkFormation(void)
{
    BDB_tsBdbEvent sBdbEvent;

    if(bDoPrimaryScan == FALSE)
    {
        sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_FORMATION_FAILURE;

        sBDB.eState = E_STATE_BASE_FAIL;
        eNF_State = E_NF_IDLE;
        sBdbEvent.eEventType = BDB_EVENT_NWK_FORMATION_FAILURE;
        ZPS_psAplAibGetAib()->pau32ApsChannelMask[0] = u32BackUpApsChannelMask;
        APP_vBdbCallback(&sBdbEvent);

    }
    else
    {
        bDoPrimaryScan = FALSE;
        u32ScanChannels = sBDB.sAttrib.u32bdbSecondaryChannelSet;
        ZPS_psAplAibGetAib()->pau32ApsChannelMask[0] = u32ScanChannels;
        if(ZPS_ZDO_DEVICE_COORD == ZPS_eAplZdoGetDeviceType())
        {
            DBG_vPrintf(TRACE_BDB,"BDB: Forming Centralized Nwk \n");
            eNF_State = E_NF_WAIT_FORM_CENTRALIZED;
            BDB_vNfFormCentralizedNwk();
        }
        else //(ZPS_ZDO_DEVICE_ROUTER == ZPS_eAplZdoGetDeviceType())
        {
            DBG_vPrintf(TRACE_BDB,"BDB: Discovery and Forming Distributed Nwk \n");
            /* For Distributed nwk formation - Perform Discovery first and then formation */
            eNF_State = E_NF_WAIT_DISCOVERY;
            vNfDiscoverNwk();
        }
    }
}

/****************************************************************************
 *
 * NAME: bNfSearchDiscNt
 *
 * DESCRIPTION:
 *
 * PARAMETERS:      Name            RW  Usage
 * 
 * RETURNS:
 *
 ****************************************************************************/
PRIVATE bool_t bNfSearchDiscNt(uint64 u64EpId, uint16 u16PanId)
{
    int i;
    ZPS_tsNwkNib *psNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());
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

#if (BDB_SET_DEFAULT_TC_POLICY == TRUE)
/****************************************************************************
 *
 * NAME: vNfTcCallback
 *
 * DESCRIPTION:
 * This function is called from the stack when the TC receives an update
 * and needs to transport a key. This defines the TC policy.
 * default TC policy is to allow devices to fresh join with a default key
 * This is strictly a BDB default TC policy. This policy can be overwritten.
 *
 * RETURNS:
 * bool_t
 *
 ****************************************************************************/
PRIVATE bool_t vNfTcCallback (uint16 u16ShortAddress,
                             uint64 u64DeviceAddress,
                             uint64 u64ParentAddress,
                             uint8 u8Status,
                             uint16 u16MacId )
{
    /*If we are joining with default key we only need to remove the existing key */
    ZPS_tsAplAib *psAib                          = zps_psAplAibGetAib( ZPS_pvAplZdoGetAplHandle( ) );
    uint32 u32KeyTblSize                         = psAib->psAplDeviceKeyPairTable->u16SizeOfKeyDescriptorTable;
    ZPS_tsAplApsKeyDescriptorEntry *psKeyTbl     = psAib->psAplDeviceKeyPairTable->psAplApsKeyDescriptorEntry;
    uint32 i;
#if (BDB_JOIN_USES_INSTALL_CODE_KEY == TRUE)
    ZPS_teDevicePermissions    eDevicePermissions;
    ZPS_teStatus    eStatus;
#endif
    /* we are only interested in insecure join , all other joins we should
     * use the normal key which is the TC key if negotiated else it is
     * the global or install code.
     */
    if(u8Status == BDB_TC_STATUS_STD_UNSEC_JOIN)
    {
        for (i = 0; i < u32KeyTblSize; i++)
        {
            if (u64DeviceAddress == ZPS_u64NwkNibGetMappedIeeeAddr( ZPS_pvAplZdoGetNwkHandle(), psKeyTbl[i].u16ExtAddrLkup ) )
            {
#if (BDB_JOIN_USES_INSTALL_CODE_KEY == FALSE)
                psKeyTbl[i].u16ExtAddrLkup          = 0xFFFF;
                psAib->pu32IncomingFrameCounter[i]  = 0;
                psKeyTbl[i].u32OutgoingFrameCounter   = 0;
                memset(psKeyTbl[i].au8LinkKey, 0, ZPS_SEC_KEY_LENGTH);
#else
                eStatus =  ZPS_bAplZdoTrustCenterGetDevicePermissions ( u64DeviceAddress,
                                                                &eDevicePermissions );
                if( eStatus == ZPS_E_SUCCESS )
                { 
                    if ( eDevicePermissions != ZPS_DEVICE_PERMISSIONS_ALL_PERMITED )
                    {
                        ZPS_bAplZdoTrustCenterSetDevicePermissions(u64DeviceAddress,
                                                           ZPS_DEVICE_PERMISSIONS_ALL_PERMITED);
                    }
                }
#endif
                ZPS_vSaveAllZpsRecords();
            }
        }
    }
    /* Always return TRUE otherwise the transport key won't go out
     * if permissions table is used and if you don't want authenticate devices
     * it is mandatory to send FALSE*/
     return TRUE;
}

#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
