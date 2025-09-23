/*****************************************************************************
 *
 * MODULE:             JN-AN-1243
 *
 * COMPONENT:          app_router_node.c
 *
 * DESCRIPTION:        Base Device Demo: Router application
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5169,
 * JN5179, JN5189].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2017-2019. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>
#include "dbg.h"
#include "pdum_apl.h"
#include "pdum_nwk.h"
#include "pdum_gen.h"

#ifdef APP_LOW_POWER_API
#include "PWR_interface.h"
#else
#include "pwrm.h"
#endif

#include "PDM.h"
#include "zps_gen.h"
#include "zps_apl.h"
#include "zps_apl_af.h"
#include "zps_apl_zdo.h"
#include "zps_apl_zdp.h"
#include "zps_apl_aib.h"
#include "zps_apl_aps.h"
#include "zcl_options.h"
#include "app_common.h"
#include "app_main.h"
#include "app.h"
#include "app_leds.h"
#include "app_buttons.h"
#include "ZTimer.h"
#include "app_events.h"
#include <rnd_pub.h>
#include "app_zcl_task.h"
#include "app_router_node.h"
#include "zps_nwk_nib.h"
#include "PDM_IDs.h"
#include "zcl.h"
#include "app_reporting.h"
#ifdef JN517x
#include "AHI_ModuleConfiguration.h"
#endif
#ifdef APP_NTAG_NWK
#include "app_ntag.h"
#include "nfc_nwk.h"
#endif
#if (JENNIC_CHIP_FAMILY == JN518x)
#include "fsl_reset.h"
#endif
#ifdef CLD_OTA
    #include "OTA.h"
    #include "app_ota_client.h"
#endif
#ifdef CLD_GREENPOWER
#include "app_power_on_counter.h"
#include "app_green_power.h"
#define GREEN_POWER_ENDPOINT                   242
#endif
#include "MicroSpecific.h"
#ifdef RF_COMMANDS
#include "RFCommands.h"
#endif
#if (defined DUAL_BOOT)
#include "rom_api.h"
#include "rom_psector.h"
#endif

#include "app_blink_led.h"
#include "App_Linky.h"
#include "TuyaSpecific.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifdef DEBUG_APP
    #define TRACE_APP   TRUE
#else
    #define TRACE_APP   FALSE
#endif

#ifdef DEBUG_APP_EVENT
    #define TRACE_APP_EVENT   TRUE
#else
    #define TRACE_APP_EVENT   FALSE
#endif

#ifdef DEBUG_APP_BDB
    #define TRACE_APP_BDB     TRUE
#else
    #define TRACE_APP_BDB     FALSE
#endif

#define ASSOCIATION_ATTEMPTS   (5)

#define IMG_TYPE_BLE_FULL_APP 0x02

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void vAppHandleAfEvent( BDB_tsZpsAfEvent *psZpsAfEvent);
PRIVATE void vAppHandleZdoEvents( BDB_tsZpsAfEvent *psZpsAfEvent);
PRIVATE void APP_vBdbInit(void);
PRIVATE void vDeletePDMOnButtonPress(uint8 u8ButtonDIO);
PRIVATE void vPrintAPSTable(void);
#if (defined DUAL_BOOT)
PRIVATE void vImageSwitch(void);
#endif

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
PUBLIC tsDeviceDesc sDeviceDesc;
extern PUBLIC tsLinkyParams sLinkyParams;

#ifdef RF_COMMANDS
extern PUBLIC tsMacFilteringDataType tsMacFilteringData;
#endif
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
uint32 u32OldFrameCtr;
uint8 u8AssociatuionAttempts;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

#ifdef PDM_EEPROM
    extern uint8 u8PDM_CalculateFileSystemCapacity();
    extern uint8 u8PDM_GetFileSystemOccupancy();
#endif

extern void zps_vNwkSecClearMatSet(void *psNwk);

#ifdef K32WMCM_APP_BUILD
/* Must be called before zps_eAplAfInit() */
void APP_SetHighTxPowerMode();

/* Must be called after zps_eAplAfInit() */
void APP_SetMaxTxPower();

#undef HIGH_TX_PWR_LIMIT
#define HIGH_TX_PWR_LIMIT 15	/* dBm */
/* High Tx power */
void APP_SetHighTxPowerMode()
{
	if (CHIP_IS_HITXPOWER_CAPABLE())
		vMMAC_SetTxPowerMode(TRUE);
}

void APP_SetMaxTxPower()
{
	if (CHIP_IS_HITXPOWER_CAPABLE())
		ZPS_eMacPlmeSet(PHY_PIB_ATTR_TX_POWER, HIGH_TX_PWR_LIMIT);
}
#endif
/****************************************************************************
 *
 * NAME: APP_vInitialiseRouter
 *
 * DESCRIPTION:
 * Initialises the application related functions
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vInitialiseRouter(void)
{
    uint16 u16ByteRead;
    PDM_teStatus eStatusReportReload;

#ifdef CLD_OTA
    vLoadOTAPersistedData();
#endif
#ifdef CLD_GREENPOWER
    vManagePowerOnCountLoadRecord();
    vAPP_GP_LoadPDMData();
#endif

    /* Stay awake */
#ifdef APP_LOW_POWER_API
    PWR_DisallowDeviceToSleep();
#else
    PWRM_eStartActivity();
#endif

    sDeviceDesc.eNodeState = E_STARTUP;
    sDeviceDesc.networkState = 0;
    PDM_eReadDataFromRecord(PDM_ID_APP_ROUTER,
                            &sDeviceDesc,
                            sizeof(tsDeviceDesc),
                            &u16ByteRead);

   /* Restore any report data that is previously saved to flash */
   eStatusReportReload = eRestoreReports();

    ZPS_u32MacSetTxBuffers (4);
#ifdef JN517x
    /* Default module configuration: change E_MODULE_DEFAULT as appropriate */
      vAHI_ModuleConfigure(E_MODULE_DEFAULT);
#endif

    ZPS_psAplAibGetAib()->bUseInstallCode = BDB_JOIN_USES_INSTALL_CODE_KEY;

#ifdef K32WMCM_APP_BUILD
    APP_SetHighTxPowerMode();
#endif

    /* Initialise ZBPro stack */
    ZPS_eAplAfInit();

#ifdef K32WMCM_APP_BUILD
    APP_SetMaxTxPower();
#endif

    /* Initialise ZCL */
    APP_ZCL_vInitialise();

    /* Initialise other software modules
     * HERE
     */
    APP_vBdbInit();

    /* Always initialise any peripherals used by the application
     * HERE
     */
#ifdef RF_COMMANDS
    vMacFilteringInitialisation();
#endif
    /* Initialise LEDs and buttons */
    APP_vLedInitialise();
    GPIO_PinWrite(GPIO, 0, 10, 1);

    APP_bButtonInitialise();
    /* Delete PDM if required */
#ifndef DONGLE
    vDeletePDMOnButtonPress(APP_BOARD_SW0_PIN);
#endif
    /* Reset association attempts */
    u8AssociatuionAttempts = ASSOCIATION_ATTEMPTS;

    /* The functions u8PDM_CalculateFileSystemCapacity and u8PDM_GetFileSystemOccupancy
     * may be called at any time to monitor space available in  the eeprom  */
  // TODO don't know if they exist DBG_vPrintf(TRACE_APP, "PDM: Capacity %d\r\n", u8PDM_CalculateFileSystemCapacity() );
  // TODO don't know if they exist  DBG_vPrintf(TRACE_APP, "PDM: Occupancy %d\r\n", u8PDM_GetFileSystemOccupancy() );

    DBG_vPrintf(TRACE_APP, "Start Up State %d On Network %d\r\n",
            sDeviceDesc.eNodeState,
            sBDB.sAttrib.bbdbNodeIsOnANetwork);

    /*Load the reports from the PDM or the default ones depending on the PDM load record status*/
    if(eStatusReportReload !=PDM_E_STATUS_OK )
    {
        /*Load Defaults if the data was not correct*/
        vLoadDefaultConfigForReportable();
    }
    /*Make the reportable attributes */
    vMakeSupportedAttributesReportable();

    //Bind table
    vDisplayBindingTable();

    //LinkyParams
    LoadLinkyParams();

    APP_vZCL_DeviceSpecific_Init();



#if 0
// TODO Remove once sdk catches up
 uint8 au8ByteCopy[16] = { 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff };
    ZPS_vAplSecSetInitialSecurityState(ZPS_ZDO_PRCONFIGURED_INSTALLATION_CODE, au8ByteCopy, 16, ZPS_APS_GLOBAL_LINK_KEY);
#endif

    vPrintAPSTable();
    vAPP_LinkySensorSample();

    //vAPP_TuyaAllReport();

#ifdef CLD_GREENPOWER
    vManagePowerOnCountInit();
#endif
}

/****************************************************************************
 *
 * NAME: APP_vBdbCallback
 *
 * DESCRIPTION:
 * Callback from the BDB
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vBdbCallback(BDB_tsBdbEvent *psBdbEvent)
{
    BDB_teStatus eStatus;

    switch(psBdbEvent->eEventType)
    {
        case BDB_EVENT_NONE:
            break;

        case BDB_EVENT_ZPSAF:                // Use with BDB_tsZpsAfEvent
            vAppHandleAfEvent(&psBdbEvent->uEventData.sZpsAfEvent);
            break;

        case BDB_EVENT_INIT_SUCCESS:

            DBG_vPrintf(TRACE_APP,"APP: BDB_EVENT_INIT_SUCCESS\r\n");
            if (sDeviceDesc.eNodeState == E_STARTUP)
            {
                eStatus = BDB_eNsStartNwkSteering();
                DBG_vPrintf(TRACE_APP, "BDB Try Steering status %d\r\n",eStatus);
            }
            else
            {
                DBG_vPrintf(TRACE_APP, "BDB Init go Running");
                sDeviceDesc.eNodeState = E_RUNNING;
                PDM_eSaveRecordData(PDM_ID_APP_ROUTER,&sDeviceDesc,sizeof(tsDeviceDesc));
            }

            break;
        case BDB_EVENT_REJOIN_FAILURE: // only for ZED
            DBG_vPrintf(TRACE_APP, "\r\nNODE: APP_vBdbCallback(REJOIN_FAILURE)");
            break;

	   case BDB_EVENT_REJOIN_SUCCESS: // only for ZED
		   DBG_vPrintf(TRACE_APP, "\r\nNODE: APP_vBdbCallback(REJOIN_SUCCESS)");

		   break;
        case BDB_EVENT_NWK_FORMATION_SUCCESS:
            DBG_vPrintf(TRACE_APP,"APP: NwkFormation Success \r\n");
            sDeviceDesc.eNodeState = E_RUNNING;
            PDM_eSaveRecordData(PDM_ID_APP_ROUTER,
                                &sDeviceDesc,
                                sizeof(tsDeviceDesc));
            break;

        case BDB_EVENT_NO_NETWORK:
            DBG_vPrintf(TRACE_APP,"APP: BDB No Networks\r\n");
            if (sDeviceDesc.eNodeState == E_STARTUP)
            {
                //if (u8AssociatuionAttempts > 0)
                //{
                //    u8AssociatuionAttempts--;
                    eStatus = BDB_eNsStartNwkSteering();
                    DBG_vPrintf(TRACE_APP, "BDB retry Steering status %d\r\n",eStatus);
              //  }
            }
            break;

        case BDB_EVENT_NWK_STEERING_SUCCESS:
            DBG_vPrintf(TRACE_APP,"APP: NwkSteering Success \r\n");
            sDeviceDesc.eNodeState = E_RUNNING;
            PDM_eSaveRecordData(PDM_ID_APP_ROUTER,&sDeviceDesc,sizeof(tsDeviceDesc));
            break;

        default:
            break;
    }
}

/****************************************************************************
 *
 * NAME: APP_taskRouter
 *
 * DESCRIPTION:
 * Task that handles application related functions
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_taskRouter(void)
{
    APP_tsEvent sAppEvent;
    sAppEvent.eType = APP_E_EVENT_NONE;

    if (ZQ_bQueueReceive(&APP_msgAppEvents, &sAppEvent) == TRUE)
    {
        DBG_vPrintf(TRACE_APP, "ZPR: App event %d, NodeState=%d\r\n", sAppEvent.eType, sDeviceDesc.eNodeState);

        if(sAppEvent.eType == APP_E_EVENT_BUTTON_DOWN)
        {
            switch(sAppEvent.uEvent.sButton.u8Button)
            {
                BDB_teStatus eStatus;
                case APP_E_BUTTONS_BUTTON_1:
                    if (sDeviceDesc.eNodeState == E_RUNNING)
                    {
                        DBG_vPrintf(TRACE_APP_EVENT, "APP_EVENT: Network steering and F&B as Target\r\n");
                        eStatus = BDB_eNsStartNwkSteering();
                        if (eStatus != 0)
                        {
                            DBG_vPrintf(TRACE_APP_EVENT, "APP_EVENT: Network Steering %02x\r\n", eStatus);
                        }
                        eStatus = BDB_eFbTriggerAsTarget(ZLINKY_APPLICATION_ENDPOINT);
                        if (eStatus != 0 && eStatus != 9)
                        {
                            DBG_vPrintf(TRACE_APP_EVENT, "APP_EVENT: Fiind and Bind Failed %02x\r\n", eStatus);
                        }
                    }
                    else
                    {
                        DBG_vPrintf(TRACE_APP_EVENT, "APP_EVENT: Try Network Formation\r\n");
                        eStatus = BDB_eNfStartNwkFormation();
                        if (eStatus != 0 && eStatus != 7)
                        {
                            DBG_vPrintf(TRACE_APP_EVENT, "APP_EVENT: Formation Failed %02x\r\n", eStatus);
                        }
                        else
                        {DBG_vPrintf(TRACE_APP_EVENT, "APP-ZDO: Network started Channel = %d\r\n", ZPS_u8AplZdoGetRadioChannel() );

                        }
                    }
                    break;

#if ((defined APP_NTAG_NWK) && (APP_BUTTONS_NFC_FD != (0xff)))
                case APP_E_BUTTONS_NFC_FD:
                    DBG_vPrintf(TRACE_APP_EVENT, "APP_EVENT: NFC_FD DOWN\r\n");
                    APP_vNtagStart(ZLINKY_APPLICATION_ENDPOINT);
                    break;
#endif
                default:
                    break;
            }
        }
        else if (sAppEvent.eType == APP_E_EVENT_BUTTON_UP)
        {
            switch(sAppEvent.uEvent.sButton.u8Button)
            {
#if ((defined APP_NTAG_NWK) && (APP_BUTTONS_NFC_FD != (0xff)))
                case APP_E_BUTTONS_NFC_FD:
                {
                    DBG_vPrintf(TRACE_APP_EVENT, "\r\nAPP_EVENT: NFC_FD UP");
                    APP_vNtagStart(ENDDEVICE_APPLICATION_ENDPOINT);
                }
                break;
#endif
#if (defined DUAL_BOOT)
                case APP_E_BUTTONS_SWISP:
                {
                    /* Image switch; no return from this */
                    DBG_vPrintf(TRACE_APP_EVENT, "\r\nAPP_EVENT: SWISP UP Switching to BLE image\r\n");
                    vImageSwitch();
                }
                break;
#endif
                default:
                {
                }
                break;
            }
        }else if (sAppEvent.eType == APP_E_EVENT_LEAVE_AND_RESET)
        {
            if (sDeviceDesc.eNodeState == E_RUNNING)
            {
                if (ZPS_eAplZdoLeaveNetwork( 0UL, FALSE, FALSE) != ZPS_E_SUCCESS )
                 {
                    APP_vFactoryResetRecords();
                    MICRO_DISABLE_INTERRUPTS();
                    RESET_SystemReset();
                }
            }
            else
            {
                APP_vFactoryResetRecords();
                MICRO_DISABLE_INTERRUPTS();
                RESET_SystemReset();
            }
            PDM_vDeleteAllDataRecords();
        }else if (sAppEvent.eType == APP_E_EVENT_PERIODIC_REPORT)
        {
        	vAPP_LinkySensorSample();


        }

#ifdef CLD_GREENPOWER
        else if(sAppEvent.eType == APP_EVENT_POR_RESET_GP_TABLES)
            vAPP_GP_ResetData();
#endif
    }
#ifdef CLD_GREENPOWER
    else if (ZQ_bQueueReceive(&APP_msgGPZCLTimerEvents, &u8GPZCLTimerEvent) == TRUE)
    {
        tsZCL_CallBackEvent sZCL_CallBackEvent;
        sZCL_CallBackEvent.eEventType =E_ZCL_CBET_TIMER_MS;
        vZCL_EventHandler(&sZCL_CallBackEvent);
    }
#endif
}


PUBLIC void APP_LeaveAndResetForTuya(void)
{
	if (sDeviceDesc.eNodeState == E_RUNNING)
	{
		if (ZPS_eAplZdoLeaveNetwork( 0UL, FALSE, FALSE) != ZPS_E_SUCCESS )
		 {
			APP_vFactoryResetRecords();
			MICRO_DISABLE_INTERRUPTS();
			RESET_SystemReset();
		}
	}
}

/****************************************************************************
 *
 * NAME: APP_vOobcSetRunning
 *
 * DESCRIPTION:
 * Set running state for OOBC
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vOobcSetRunning(void)
{
    sDeviceDesc.eNodeState = E_RUNNING;
    PDM_eSaveRecordData(PDM_ID_APP_ROUTER,&sDeviceDesc,sizeof(tsDeviceDesc));
}

/****************************************************************************
 *
 * NAME: vAppHandleAfEvent
 *
 * DESCRIPTION:
 * Application handler for stack events
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vAppHandleAfEvent( BDB_tsZpsAfEvent *psZpsAfEvent)
{
    if (psZpsAfEvent->u8EndPoint == ZLINKY_APPLICATION_ENDPOINT)
    {
//        DBG_vPrintf(TRACE_APP, "Pass to ZCL\r\n");
        if ((psZpsAfEvent->sStackEvent.eType == ZPS_EVENT_APS_DATA_INDICATION) ||
            (psZpsAfEvent->sStackEvent.eType == ZPS_EVENT_APS_INTERPAN_DATA_INDICATION))
        {
#ifdef RF_COMMANDS
            if(psZpsAfEvent->sStackEvent.uEvent.sApsDataIndEvent.u16ClusterId == RF_COMMAND_HANDLER_CLUSTER_ID)
            {
                DBG_vPrintf((bool)TRUE, "Pass to RFCommandHandler\n");
                RFCommandsHandler( &psZpsAfEvent->sStackEvent );
            }
            else
            {
                APP_ZCL_vEventHandler( &psZpsAfEvent->sStackEvent);
            }
#else
            APP_ZCL_vEventHandler( &psZpsAfEvent->sStackEvent);
#endif
        }
    }
    else if (psZpsAfEvent->u8EndPoint == 0)
    {
        vAppHandleZdoEvents( psZpsAfEvent);
    }
#ifdef CLD_GREENPOWER
    else if ((psZpsAfEvent->u8EndPoint == GREEN_POWER_ENDPOINT) ||  (psZpsAfEvent->sStackEvent.eType == ZPS_EVENT_APS_ZGP_DATA_CONFIRM))
    {
        if ((psZpsAfEvent->sStackEvent.eType == ZPS_EVENT_APS_DATA_INDICATION) ||
            (psZpsAfEvent->sStackEvent.eType == ZPS_EVENT_APS_ZGP_DATA_INDICATION)||
            (psZpsAfEvent->sStackEvent.eType == ZPS_EVENT_APS_ZGP_DATA_CONFIRM))
        {

            APP_ZCL_vEventHandler( &psZpsAfEvent->sStackEvent);
        }
    }
#endif

    /* Ensure Freeing of Apdus */
    if (psZpsAfEvent->sStackEvent.eType == ZPS_EVENT_APS_DATA_INDICATION)
    {
        PDUM_eAPduFreeAPduInstance(psZpsAfEvent->sStackEvent.uEvent.sApsDataIndEvent.hAPduInst);
    }
    else if ( psZpsAfEvent->sStackEvent.eType == ZPS_EVENT_APS_INTERPAN_DATA_INDICATION )
    {
        PDUM_eAPduFreeAPduInstance(psZpsAfEvent->sStackEvent.uEvent.sApsInterPanDataIndEvent.hAPduInst);
    }

}

/****************************************************************************
 *
 * NAME: vAppHandleZdoEvents
 *
 * DESCRIPTION:
 * Application handler for stack events for end point 0 (ZDO)
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vAppHandleZdoEvents( BDB_tsZpsAfEvent *psZpsAfEvent)
{
    ZPS_tsAfEvent *psAfEvent = &(psZpsAfEvent->sStackEvent);

    switch(psAfEvent->eType)
    {
        case ZPS_EVENT_APS_DATA_INDICATION:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Data Indication Status %02x from %04x Src Ep Dst %d Ep %d Profile %04x Cluster %04x\r\n",
                    psAfEvent->uEvent.sApsDataIndEvent.eStatus,
                    psAfEvent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr,
                    psAfEvent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                    psAfEvent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                    psAfEvent->uEvent.sApsDataIndEvent.u16ProfileId,
                    psAfEvent->uEvent.sApsDataIndEvent.u16ClusterId);

            #ifdef CLD_OTA
                if ((psZpsAfEvent->sStackEvent.uEvent.sApsDataIndEvent.eStatus == ZPS_E_SUCCESS) &&
                        (psZpsAfEvent->sStackEvent.uEvent.sApsDataIndEvent.u8DstEndpoint == 0))
                {
                    // Data Ind for ZDp Ep
                    if (ZPS_ZDP_MATCH_DESC_RSP_CLUSTER_ID == psZpsAfEvent->sStackEvent.uEvent.sApsDataIndEvent.u16ClusterId)
                    {
                        vHandleMatchDescriptor(&psZpsAfEvent->sStackEvent);
                    } else if (ZPS_ZDP_IEEE_ADDR_RSP_CLUSTER_ID == psZpsAfEvent->sStackEvent.uEvent.sApsDataIndEvent.u16ClusterId) {
                        vHandleIeeeAddressRsp(&psZpsAfEvent->sStackEvent);
                    }
                }
            #endif
            break;

        case ZPS_EVENT_APS_DATA_CONFIRM:
            break;

        case ZPS_EVENT_APS_DATA_ACK:
            break;
            break;

        case ZPS_EVENT_NWK_STARTED:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Network started\r\n");
            break;

        case ZPS_EVENT_NWK_JOINED_AS_ROUTER:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Joined Network Addr %04x Rejoin %d\r\n",
                    psAfEvent->uEvent.sNwkJoinedEvent.u16Addr,
                    psAfEvent->uEvent.sNwkJoinedEvent.bRejoin);

            /* Save to PDM */
            sDeviceDesc.eNodeState = E_RUNNING;
            sDeviceDesc.networkState = 1;
            PDM_eSaveRecordData(PDM_ID_APP_ROUTER,&sDeviceDesc,sizeof(tsDeviceDesc));

            ZPS_eAplAibSetApsUseExtendedPanId( ZPS_u64NwkNibGetEpid(ZPS_pvAplZdoGetNwkHandle()) );
            #ifdef APP_NTAG_NWK
            {
                /* Not a rejoin ? */
                if (FALSE == psAfEvent->uEvent.sNwkJoinedEvent.bRejoin)
                {
                    /* Write network data to tag */
                    APP_vNtagStart(ZLINKY_APPLICATION_ENDPOINT);
                }
            }
            #endif
            u8OldStatusLinky=99;
            u48StartTuyaTotalConsumption = 0;
            DBG_vPrintf(1, "\r\n ----------TUYA u48StartTuyaTotalConsumption : %d\r\n", u48StartTuyaTotalConsumption );
            break;
        case ZPS_EVENT_NWK_FAILED_TO_START:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Network Failed To start\r\n");
            break;

        case ZPS_EVENT_NWK_FAILED_TO_JOIN:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Failed To Join %02x Rejoin %d\r\n",
                    psAfEvent->uEvent.sNwkJoinFailedEvent.u8Status,
                    psAfEvent->uEvent.sNwkJoinFailedEvent.bRejoin);
            break;

        case ZPS_EVENT_NWK_NEW_NODE_HAS_JOINED:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: New Node %04x Has Joined\r\n",
                    psAfEvent->uEvent.sNwkJoinIndicationEvent.u16NwkAddr);
            sDeviceDesc.eNodeState = E_RUNNING;
            sDeviceDesc.networkState = 1;
            PDM_eSaveRecordData(PDM_ID_APP_ROUTER,&sDeviceDesc,sizeof(tsDeviceDesc));

            u8OldStatusLinky=99;
            u48StartTuyaTotalConsumption = 0;
            DBG_vPrintf(1, "\r\n ----------TUYA u48StartTuyaTotalConsumption : %d\r\n", u48StartTuyaTotalConsumption );
            break;

        case ZPS_EVENT_NWK_DISCOVERY_COMPLETE:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Discovery Complete %02x\r\n",
            psAfEvent->uEvent.sNwkDiscoveryEvent.eStatus);
            //vStartAwakeTimer(2);
            //vStartBlinkTimer(50);
           // vStartAwakeTimer(15);
            vStartBlinkTimer(200);
            //GPIO_PinWrite(GPIO, 0, 10, 1); //ON

            break;

        case ZPS_EVENT_NWK_LEAVE_INDICATION:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Leave Indication %016llx Rejoin %d\r\n",
                    psAfEvent->uEvent.sNwkLeaveIndicationEvent.u64ExtAddr,
                    psAfEvent->uEvent.sNwkLeaveIndicationEvent.u8Rejoin);
            if ( (psAfEvent->uEvent.sNwkLeaveIndicationEvent.u64ExtAddr == 0UL) &&
                 (psAfEvent->uEvent.sNwkLeaveIndicationEvent.u8Rejoin == 0) )
            {
                /* We sare asked to Leave without rejoin */
                DBG_vPrintf(TRACE_APP, "LEAVE IND -> For Us No Rejoin\r\n");
                APP_vFactoryResetRecords();
                MICRO_DISABLE_INTERRUPTS();
                RESET_SystemReset();
            }
            break;

        case ZPS_EVENT_NWK_LEAVE_CONFIRM:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Leave Confirm status %02x Addr %016llx\r\n",
                    psAfEvent->uEvent.sNwkLeaveConfirmEvent.eStatus,
                    psAfEvent->uEvent.sNwkLeaveConfirmEvent.u64ExtAddr);
            if ((psAfEvent->uEvent.sNwkLeaveConfirmEvent.eStatus == ZPS_E_SUCCESS) &&
                (psAfEvent->uEvent.sNwkLeaveConfirmEvent.u64ExtAddr == 0UL))
            {
                DBG_vPrintf(TRACE_APP, "Leave -> Reset Data Structures\r\n");
                APP_vFactoryResetRecords();
                MICRO_DISABLE_INTERRUPTS();
                RESET_SystemReset();
            }
            break;

        case ZPS_EVENT_NWK_STATUS_INDICATION:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Network status Indication %02x addr %04x\r\n",
                    psAfEvent->uEvent.sNwkStatusIndicationEvent.u8Status,
                    psAfEvent->uEvent.sNwkStatusIndicationEvent.u16NwkAddr);
            break;

        case ZPS_EVENT_NWK_ROUTE_DISCOVERY_CONFIRM:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Discovery Confirm\r\n");
            break;

        case ZPS_EVENT_NWK_ED_SCAN:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Energy Detect Scan %02x\r\n",
                    psAfEvent->uEvent.sNwkEdScanConfirmEvent.u8Status);
            break;
        case ZPS_EVENT_ZDO_BIND:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Zdo Bind event\r\n");
            break;

        case ZPS_EVENT_ZDO_UNBIND:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Zdo Unbiind Event\r\n");
            break;

        case ZPS_EVENT_ZDO_LINK_KEY:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Zdo Link Key Event Type %d Addr %016llx\r\n",
                        psAfEvent->uEvent.sZdoLinkKeyEvent.u8KeyType,
                        psAfEvent->uEvent.sZdoLinkKeyEvent.u64IeeeLinkAddr);
            break;

        case ZPS_EVENT_BIND_REQUEST_SERVER:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Bind Request Server Event\r\n");
            break;

        case ZPS_EVENT_ERROR:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: AF Error Event %d\r\n", psAfEvent->uEvent.sAfErrorEvent.eError);
            break;

        case ZPS_EVENT_TC_STATUS:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Trust Center Status %02x\r\n", psAfEvent->uEvent.sApsTcEvent.u8Status);
            break;

        default:
            DBG_vPrintf(TRACE_APP, "APP-ZDO: Unhandled Event %d\r\n", psAfEvent->eType);
            break;
    }
}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/


/****************************************************************************
 *
 * NAME: APP_vFactoryResetRecords
 *
 * DESCRIPTION:
 * Resets persisted data structures to factory new state
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vFactoryResetRecords(void)
{
    /* clear out the stack */
    ZPS_vDefaultStack();
    ZPS_eAplAibSetApsUseExtendedPanId(0);
    ZPS_vSetKeys();

#ifdef CLD_GREENPOWER
    vAPP_GP_ResetData();
#endif

    /* save everything */
    sDeviceDesc.eNodeState = E_STARTUP;
    sDeviceDesc.networkState = 0 ;

#ifdef CLD_OTA
    vResetOTADiscovery();
    vOTAResetPersist();
#endif

    PDM_eSaveRecordData(PDM_ID_APP_ROUTER,&sDeviceDesc,sizeof(tsDeviceDesc));
    ZPS_vSaveAllZpsRecords();
}

/****************************************************************************
 *
 * NAME: APP_vBdbInit
 *
 * DESCRIPTION:
 * Function to initialize BDB attributes and message queue
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void APP_vBdbInit(void)
{
    BDB_tsInitArgs sInitArgs;

    sBDB.sAttrib.bbdbNodeIsOnANetwork = ((sDeviceDesc.eNodeState >= E_RUNNING)?(TRUE):(FALSE));
    sInitArgs.hBdbEventsMsgQ = &APP_msgBdbEvents;
    BDB_vInit(&sInitArgs);
}

/****************************************************************************
 *
 * NAME: vDeletePDMOnButtonPress
 *
 * DESCRIPTION:
 * PDM context clearing on button press
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vDeletePDMOnButtonPress(uint8 u8ButtonDIO)
{
    bool_t bDeleteRecords = FALSE;
    uint8 u8Status;

    uint32 u32Buttons = APP_u32GetSwitchIOState() & (1 << u8ButtonDIO);

    if (u32Buttons == 0)
    {
        bDeleteRecords = TRUE;
    }
    else
    {
        bDeleteRecords = FALSE;
    }
    /* If required, at this point delete the network context from flash, perhaps upon some condition
     * For example, check if a button is being held down at reset, and if so request the Persistent
     * Data Manager to delete all its records:
     * e.g. bDeleteRecords = vCheckButtons();
     * Alternatively, always call PDM_vDeleteAllDataRecords() if context saving is not required.
     */
    if(bDeleteRecords)
    {
        /* wait for button release */
        while (u32Buttons == 0)
        {
            u32Buttons = APP_u32GetSwitchIOState() & (1 << u8ButtonDIO);
        }
        u8Status = ZPS_eAplZdoLeaveNetwork(0, FALSE,FALSE);
        if (ZPS_E_SUCCESS !=  u8Status )
        {
            /* Leave failed,so just reset everything */
            DBG_vPrintf(TRACE_APP,"Leave failed status %x Deleting the PDM\r\n", u8Status);
            APP_vFactoryResetRecords();
            MICRO_DISABLE_INTERRUPTS();
            RESET_SystemReset();
        } else { DBG_vPrintf(TRACE_APP, "RESET: Sent Leave\r\n"); }
    }
}

/****************************************************************************
 *
 * NAME: vPrintAPSTable
 *
 * DESCRIPTION:
 * Prints the content of APS table
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vPrintAPSTable(void)
{
    uint8 i;
    uint8 j;

    ZPS_tsAplAib * psAplAib;
    uint64 u64Addr;

    psAplAib = ZPS_psAplAibGetAib();

    for ( i = 0 ; i < (psAplAib->psAplDeviceKeyPairTable->u16SizeOfKeyDescriptorTable + 3) ; i++ )
    {
        u64Addr = ZPS_u64NwkNibGetMappedIeeeAddr(ZPS_pvAplZdoGetNwkHandle(), psAplAib->psAplDeviceKeyPairTable->psAplApsKeyDescriptorEntry[i].u16ExtAddrLkup);
        DBG_vPrintf(TRUE, "%d MAC: 0x%x %x Key: ", i, (uint32)(u64Addr>>32), (uint32)(u64Addr&0xffffffff) );
        for(j=0; j<16;j++)
        {
            DBG_vPrintf(TRUE, "%02x ", psAplAib->psAplDeviceKeyPairTable->psAplApsKeyDescriptorEntry[i].au8LinkKey[j]);
        }
        DBG_vPrintf(TRUE, "\r\n");
    }
}

/****************************************************************************
 *
 * NAME: app_u8GetDeviceEndpoint
 *
 * DESCRIPTION:
 * Return the application endpoint
 *
 * PARAMETER: void
 *
 * RETURNS: uint8
 *
 ****************************************************************************/
PUBLIC uint8 app_u8GetDeviceEndpoint( void)
{
    return ZLINKY_APPLICATION_ENDPOINT;
}

#ifdef CLD_OTA
/****************************************************************************
 *
 * NAME: eGetNodeState
 *
 * DESCRIPTION:
 * Returns the node state
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC teNodeState eGetNodeState(void)
{
    return sDeviceDesc.eNodeState;
}
/****************************************************************************
 *
 * NAME: sGetOTACallBackPersistdata
 *
 * DESCRIPTION:
 * returns persisted data
 *
 * RETURNS:
 * tsOTA_PersistedData
 *
 ****************************************************************************/

PUBLIC tsOTA_PersistedData sGetOTACallBackPersistdata(void)
{
    return sBaseDevice.sCLD_OTA_CustomDataStruct.sOTACallBackMessage.sPersistedData;
}

#endif

#if (defined DUAL_BOOT)
/*! *********************************************************************************
* \brief        image switch.
*
********************************************************************************** */
PRIVATE void vImageSwitch(void)
{
    psector_page_data_t page;
    psector_page_state_t page_state;

    /* Read protected sectors */
    page_state = psector_ReadData(PSECTOR_PAGE0_PART, 0, 0, sizeof(psector_page_t), &page);
    if (page_state < PAGE_STATE_DEGRADED)
    {
        DBG_vPrintf(TRACE_APP, "APP: vImageSwitch() PSECTOR Error\r\n");
        /* Problem with pSector, so just return */
        return;
    }

    DBG_vPrintf(TRACE_APP, "APP: vImageSwitch() to BLE\r\n");
    page.page0_v3.preferred_app_index = IMG_TYPE_BLE_FULL_APP;

    page.hdr.version++;
    page.hdr.checksum = psector_CalculateChecksum((psector_page_t *)&page);

    if(psector_WriteUpdatePage(PSECTOR_PAGE0_PART, (psector_page_t *)&page) != WRITE_OK)
    {
        DBG_vPrintf(TRACE_APP, "APP: vImageSwitch() WRITE Error\r\n");
    }
    else
    {
        DBG_vPrintf(TRACE_APP, "APP: vImageSwitch() WRITE Ok\r\n");
    }
    RESET_SystemReset();
    while(1);
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
