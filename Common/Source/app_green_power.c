/*****************************************************************************
 *
 * MODULE:             JN-AN-1244
 *
 * COMPONENT:          app_light_interpolation.c
 *
 * DESCRIPTION:        Light Bulb application - Linear Interpolation - Implementation
 *
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
 * Copyright NXP B.V. 2016-2018. All rights reserved
 *
 ***************************************************************************/
#ifdef CLD_GREENPOWER

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include "zps_gen.h"
#include "dbg.h"
#include "app_green_power.h"
#include "LevelControl.h"
#include "OnOff.h"
#include "PDM.h"
#include  "string.h"
#include "PDM_IDs.h"
#include "app_common.h"
#include "bdb_api.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifdef DEBUG_APP_GP
    #define TRACE_APP_GP                       TRUE
#else
    #define TRACE_APP_GP                       FALSE
#endif

#define GP_PDM_DATA_VALID                      0x0B
#define ANY_INDEX                             0xFF
#define GP_NUMBER_OF_CONFIGURABLE_CMD_INFO    0x01
#define GP_SINK_TABLE_COMM_MODE_MASK       (0x0F)
#define ZPG_GP_ENDPOIND_ID                    242
#define DEVICE_ID_GP_PROXY_BASIC            0x61
#define PDM_VALID_BITS                      0x000F
#define PDM_TRANS_POINTER_INFO_INDEX        0x04
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct
{
    /* first nibble indicates whether PDM is valid, the other bits are used to know where the translation table pointer are pointed
     * Bits 0 - 3 : If set to GP_PDM_DATA_VALID, indicates that PDM data is valid
     * Bits 4 till GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES if set to 1 indicates that psGpToZclCmdInfo pointer in
     * asGpTranslationTable points to asGpToZclCmdInfoUpdate
     * rather than default asGpToZclLevelControlCmdInfo            -
     */

    uint32 u32RestoreGPPDMInfo;
#ifdef GP_COMBO_BASIC_DEVICE
    tsGP_TranslationTableEntry asGpTranslationTable[GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES];
    /*  the command info to be used on receiving  GP translation table update command */
    tsGP_GpToZclCommandInfo    asGpToZclCmdInfoUpdate[GP_NUMBER_OF_CONFIGURABLE_CMD_INFO];
#endif
}tsGP_PDM_Data;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef GP_COMBO_BASIC_DEVICE
PRIVATE tsGP_TranslationTableEntry* psApp_GPGetTranslationTable(
        uint8                u8AppId,
        uint8                *pu8TranslationTableIndex,
        tuGP_ZgpdDeviceAddr *uSrcAddr);
#endif
teGP_GreenPowerStatus eApp_UpdateTranslationTable(tsGP_ZgpsTranslationTableUpdate *psUpdateCmd);
extern PUBLIC uint8 au8DefaultTCLinkKey[16];
void vAPP_GP_SetCommModeLtWt(void);

bool  bAppAddTransTableEntries(
         tuGP_ZgpdDeviceAddr         uRcvdGPDAddr,
         zbmap8                      b8Options
        );

void vApp_UpdateTranslationTableOnPairingCfg(tsGP_ZgpsPairingConfigCmdRcvd *psPairingConfigCmdRcvd);
#ifdef GP_COMBO_BASIC_DEVICE
void vInitTranslationTablePointers(void);
#endif
/****************************************************************************/
/*          Exported Variables                                              */
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

tsGP_PDM_Data sGP_PDM_Data;

tsGP_GreenPowerDevice               sGPDeviceInfo;

#if 0
/* ZCL command info for default Level control device . The same is used for on/off device,Generic 1-state Switch, Advanced Generic 1-state Switch*/
tsGP_GpToZclCommandInfo asGpToZclLevelControlCmdInfo[] = {
    {E_GP_ZGP_LEVEL_CONTROL_SWITCH, E_GP_IDENTIFY, E_CLD_IDENTIFY_CMD_IDENTIFY, 1, GENERAL_CLUSTER_ID_IDENTIFY, 0x00, {0}}, /* On/Off Switch, GP Identify Cmd Id, ZB Cmd Id, EP, Cluster ID, ZB Payload Length, NULL data */
    {E_GP_ZGP_LEVEL_CONTROL_SWITCH, E_GP_OFF, 0x00, 1, GENERAL_CLUSTER_ID_ONOFF, 0x00, {0}}, /* On/Off Switch, GP Off Cmd Id, ZB Cmd Id, EP, Cluster ID, ZB Payload Length, NULL data */
    {E_GP_ZGP_LEVEL_CONTROL_SWITCH, E_GP_ON, 0x01, 1, GENERAL_CLUSTER_ID_ONOFF, 0x00, {0}}, /* On/Off Switch, GP On Cmd Id, ZB Cmd Id, EP, Cluster ID, ZB Payload Length, NULL data */
    {E_GP_ZGP_LEVEL_CONTROL_SWITCH, E_GP_TOGGLE, 0x02, 1, GENERAL_CLUSTER_ID_ONOFF, 0x00, {0}}, /* On/Off Switch, GP Toggle Cmd Id, ZB Cmd Id, EP, Cluster ID, ZB Payload Length, NULL data */
    {E_GP_ZGP_LEVEL_CONTROL_SWITCH, E_GP_LEVEL_CONTROL_STOP, E_CLD_LEVELCONTROL_CMD_STOP, 1, GENERAL_CLUSTER_ID_LEVEL_CONTROL, 0x00, {0}}, /* Level control switch , Level control stop, ZB Cmd Id, EP, Cluster ID, ZB Payload Length, NULL data */
    {E_GP_ZGP_LEVEL_CONTROL_SWITCH, E_GP_MOVE_UP_WITH_ON_OFF, E_CLD_LEVELCONTROL_CMD_MOVE_WITH_ON_OFF, 1, GENERAL_CLUSTER_ID_LEVEL_CONTROL, 0x02, {0,10}}, /* Level control switch,Move Up, ZB Cmd Id, EP, Cluster ID, ZB Payload Length, NULL data */
    {E_GP_ZGP_LEVEL_CONTROL_SWITCH, E_GP_MOVE_DOWN_WITH_ON_OFF, E_CLD_LEVELCONTROL_CMD_MOVE_WITH_ON_OFF, 1, GENERAL_CLUSTER_ID_LEVEL_CONTROL, 0x02, {1,10}}, /* Level control switch, Move down, ZB Cmd Id, EP, Cluster ID, ZB Payload Length, NULL data */
};
#endif

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 * NAME: vApp_GP_RegisterDevice
 *
 * DESCRIPTION:
 * Registers device as combo basic
 ****************************************************************************/

void vApp_GP_RegisterDevice(tfpZCL_ZCLCallBackFunction fptrEPCallBack)
{
    teZCL_Status eZCL_Status;
    teGP_ResetToDefaultConfig eResetToDefault = E_GP_DEFAULT_ATTRIBUTE_VALUE |E_GP_DEFAULT_PROXY_SINK_TABLE_VALUE;
    /* Register GP End Point */
#ifdef GP_COMBO_BASIC_DEVICE
    eZCL_Status = eGP_RegisterComboBasicEndPoint(
            GREENPOWER_END_POINT_ID,
            fptrEPCallBack,
            &sGPDeviceInfo,
            HA_PROFILE_ID,
            sGP_PDM_Data.asGpTranslationTable);
    /* Check GP end point registration status */
    if (eZCL_Status != E_ZCL_SUCCESS)
    {
            DBG_vPrintf(TRACE_APP_GP, "Error: eGP_RegisterComboBasicEndPoint:%d\r\n", eZCL_Status);
    }
#endif

#ifdef GP_PROXY_BASIC_DEVICE

    eZCL_Status = eGP_RegisterProxyBasicEndPoint(
            GREENPOWER_END_POINT_ID,
            fptrEPCallBack,
            &sGPDeviceInfo);
    /* Check GP end point registration status */
    if (eZCL_Status != E_ZCL_SUCCESS)
    {
            DBG_vPrintf(TRACE_APP_GP, "Error: eGP_RegisterProxyBasicEndPoint:%d\r\n", eZCL_Status);
    }
#endif

    DBG_vPrintf(TRACE_APP_GP, "\n before  vGP_RestorePersistedData Addr= 0x%8x %d Address = 0x%x\n",sGPDeviceInfo.sGreenPowerCustomDataStruct.asZgpsSinkProxyTable[0].uZgpdDeviceAddr.u32ZgpdSrcId,
            sGPDeviceInfo.sGreenPowerCustomDataStruct.asZgpsSinkProxyTable[0].bProxyTableEntryOccupied,
            &sGPDeviceInfo.sGreenPowerCustomDataStruct.asZgpsSinkProxyTable[0].uZgpdDeviceAddr);
     if(GP_PDM_DATA_VALID == (sGP_PDM_Data.u32RestoreGPPDMInfo & PDM_VALID_BITS))
     {

        DBG_vPrintf(TRACE_APP_GP, "sGP_PDM_Data.u32RestoreGPPDMInfo = %d \n", sGP_PDM_Data.u32RestoreGPPDMInfo);
        /* DO not reset sink table to default. Sink table is persisted */
        eResetToDefault = E_GP_DEFAULT_ATTRIBUTE_VALUE ;
        vGP_RestorePersistedData(sGPDeviceInfo.sGreenPowerCustomDataStruct.asZgpsSinkProxyTable,eResetToDefault);
#ifdef GP_COMBO_BASIC_DEVICE
        vInitTranslationTablePointers();
#endif

     }
     else
     {

        DBG_vPrintf(TRACE_APP_GP, "sGP_PDM_Data.u32RestoreGPPDMInfo = %d should not be %d \n", sGP_PDM_Data.u32RestoreGPPDMInfo, GP_PDM_DATA_VALID);
        vGP_RestorePersistedData(NULL,eResetToDefault);
     }
#ifdef GP_COMBO_BASIC_DEVICE
     sGPDeviceInfo.sServerGreenPowerCluster.b8ZgpsCommissioningExitMode =
            GP_CMSNG_EXIT_MODE_ON_PAIRING_SUCCESS_MASK | GP_CMSNG_EXIT_MODE_ON_WINDOW_EXPIRE_MASK ;

     sGPDeviceInfo.sServerGreenPowerCluster.b8ZgpsSecLevel = E_GP_NO_SECURITY;
     vAPP_GP_SetCommModeLtWt();


#ifdef  CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE
         sGPDeviceInfo.sServerGreenPowerCluster.b8ZgpSharedSecKeyType = GP_KEYTPE;
         if(E_GP_OUT_OF_THE_BOX_ZGPD_KEY != GP_KEYTPE)
         {
             uint8 key[] = GP_SHARED_KEY;
             memcpy(&sGPDeviceInfo.sServerGreenPowerCluster.sZgpSharedSecKey,key, 16 );
         }
#endif
#ifdef CLD_GP_ATTR_ZGP_LINK_KEY
             ZPS_tsAplAib *psAib = ZPS_psAplAibGetAib();
             memcpy((sGPDeviceInfo.sServerGreenPowerCluster.sZgpLinkKey.au8Key),psAib->psAplDefaultTCAPSLinkKey->au8LinkKey, 16 );
             DBG_vPrintf(TRACE_APP_GP, "Link key : ");
             {
                 uint8 i;
                 for(i= 0; i < 16; i++)
                 {
                     DBG_vPrintf(TRACE_APP_GP, "%x, ",sGPDeviceInfo.sServerGreenPowerCluster.sZgpLinkKey.au8Key[i] );
                 }
                 DBG_vPrintf(TRACE_APP_GP, "\n");
             }
#endif
#else
#ifdef  CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE
         sGPDeviceInfo.sClientGreenPowerCluster.b8ZgpSharedSecKeyType = GP_KEYTPE;
         if(E_GP_OUT_OF_THE_BOX_ZGPD_KEY != GP_KEYTPE)
         {
             uint8 key[] = GP_SHARED_KEY;
             memcpy(&sGPDeviceInfo.sClientGreenPowerCluster.sZgpSharedSecKey,key, 16 );
         }
#endif
#ifdef CLD_GP_ATTR_ZGP_LINK_KEY
             ZPS_tsAplAib *psAib = ZPS_psAplAibGetAib();
             memcpy((sGPDeviceInfo.sClientGreenPowerCluster.sZgpLinkKey.au8Key),psAib->psAplDefaultTCAPSLinkKey->au8LinkKey, 16 );
             DBG_vPrintf(TRACE_APP_GP, "Link key : ");
             {
                 uint8 i;
                 for(i= 0; i < 16; i++)
                 {
                     DBG_vPrintf(TRACE_APP_GP, "%x, ",sGPDeviceInfo.sClientGreenPowerCluster.sZgpLinkKey.au8Key[i] );
                 }
                 DBG_vPrintf(TRACE_APP_GP, "\n");
             }
#endif
             DBG_vPrintf(TRACE_APP_GP, "\n vSink table address = 0x%8x %d\n",sGPDeviceInfo.sGreenPowerCustomDataStruct.asZgpsSinkProxyTable[0].uZgpdDeviceAddr.u32ZgpdSrcId,
                     sGPDeviceInfo.sGreenPowerCustomDataStruct.asZgpsSinkProxyTable[0].bProxyTableEntryOccupied);
#endif

}
/****************************************************************************
 * NAME: vApp_GP_EnterCommissioningMode
 *
 * DESCRIPTION:
 * Combo device enters to commission mode
 ****************************************************************************/
void vApp_GP_EnterCommissioningMode(void)
{
    uint8 u8Status;
    tsZCL_Address                          sDestinationAddress;
    sDestinationAddress.eAddressMode = E_ZCL_AM_BROADCAST;
    sDestinationAddress.uAddress.eBroadcastMode = ZPS_E_APL_AF_BROADCAST_RX_ON;
    /* Toggle action: enter commissioning mode if the device is in operating mode,
     * otherwise exit commissioning mode */
    if(sGPDeviceInfo.sGreenPowerCustomDataStruct.eGreenPowerDeviceMode == E_GP_OPERATING_MODE)
    {
         u8Status = eGP_ProxyCommissioningMode(
                GREENPOWER_END_POINT_ID,
                        242,
                        sDestinationAddress,
                        E_GP_PROXY_COMMISSION_ENTER);
    }
    else
    {
        u8Status = eGP_ProxyCommissioningMode(
                GREENPOWER_END_POINT_ID,
                        242,
                        sDestinationAddress,
                        E_GP_PROXY_COMMISSION_EXIT);
        //vStartEffect(E_CLD_IDENTIFY_EFFECT_STOP_EFFECT);
    }
    DBG_vPrintf(TRACE_APP_GP, "eGP_ProxyCommissioningMode returned status 0x%x", u8Status);

}
/****************************************************************************
 * NAME: vAPP_GP_LoadPDMData
 *
 * DESCRIPTION:
 * Loads GP data from PDM
 ****************************************************************************/
void vAPP_GP_LoadPDMData(void)
{
    uint8 u8Status;
    uint16 u16ByteRead;
    u8Status = PDM_eReadDataFromRecord(PDM_ID_APP_CLD_GP_TRANS_TABLE,
                                               &sGP_PDM_Data,
                                               sizeof(sGP_PDM_Data),
                                               &u16ByteRead);
    DBG_vPrintf(TRACE_APP_GP, "\n vAPP_GP_LoadPDMData PDM_ID_APP_CLD_GP_TRANS_TABLE u8Status = %d u16ByteRead = %d  %d\n",u8Status, u16ByteRead,
            sizeof(sGP_PDM_Data));
    u8Status = PDM_eReadDataFromRecord(PDM_ID_APP_CLD_GP_SINK_PROXY_TABLE,
                                                  &sGPDeviceInfo.sGreenPowerCustomDataStruct.asZgpsSinkProxyTable,
                                                  sizeof(sGPDeviceInfo.sGreenPowerCustomDataStruct.asZgpsSinkProxyTable),
                                                &u16ByteRead);
    DBG_vPrintf(TRACE_APP_GP, "\n vAPP_GP_LoadPDMData PDM_ID_APP_CLD_GP_SINK_PROXY_TABLE u8Status = %d u16ByteRead = %d %d address = 0x%8x %d\n",u8Status, u16ByteRead,
            sizeof(sGPDeviceInfo.sGreenPowerCustomDataStruct.asZgpsSinkProxyTable),sGPDeviceInfo.sGreenPowerCustomDataStruct.asZgpsSinkProxyTable[0].uZgpdDeviceAddr.u32ZgpdSrcId,
            sGPDeviceInfo.sGreenPowerCustomDataStruct.asZgpsSinkProxyTable[0].bProxyTableEntryOccupied);
}

/****************************************************************************
 * NAME: vAPP_GP_ResetData
 *
 * DESCRIPTION:
 * Deletes data from PDM
 ****************************************************************************/
void vAPP_GP_ResetData(void)
{

    PDM_vDeleteDataRecord(PDM_ID_APP_CLD_GP_TRANS_TABLE);
    PDM_vDeleteDataRecord(PDM_ID_APP_CLD_GP_SINK_PROXY_TABLE);
    memset( &(sGPDeviceInfo.sGreenPowerCustomDataStruct.asZgpsSinkProxyTable),0,
               sizeof(sGPDeviceInfo.sGreenPowerCustomDataStruct.asZgpsSinkProxyTable));
    memset(&sGP_PDM_Data,0,
            sizeof(tsGP_PDM_Data));
}
/****************************************************************************
 * NAME: vAPP_GP_SetCommModeLtWt
 *
 * DESCRIPTION:
 *Sets communication mode to light weight unicast for testing
 ****************************************************************************/
void vAPP_GP_SetCommModeLtWt(void)
{

#ifdef GP_COMBO_BASIC_DEVICE
     sGPDeviceInfo.sServerGreenPowerCluster.b24ZgpsFunctionality |= GP_FEATURE_ZGPP_LIGHTWEIGHT_UNICAST_COMM;
     DBG_vPrintf(TRACE_APP_GP, "b8ZgpsCommunicationMode set to .E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_LIGHTWEIGHT \n");
#endif
}

/****************************************************************************
 * NAME: vHandleGreenPowerEvent
 *
 * DESCRIPTION:
 * GP Event Handler
 ****************************************************************************/


void vHandleGreenPowerEvent(tsGP_GreenPowerCallBackMessage *psGPMessage)
{
    DBG_vPrintf(TRACE_APP_GP, "Green Power Callback Message\n");
#ifdef GP_COMBO_BASIC_DEVICE
    uint8  u8Count;
    tsGP_TranslationTableEntry *psTranslationTableEntry = NULL;
    tsZCL_Address                          sDestinationAddress;
#endif
    switch(psGPMessage->eEventType)
    {
#ifdef GP_COMBO_BASIC_DEVICE
        case E_GP_COMMISSION_DATA_INDICATION:
        {

            tsGP_ZgpCommissionIndication *psZgpCommissionIndication;

            DBG_vPrintf(TRACE_APP_GP, "E_GP_COMMISSION_DATA_INDICATION \n");

            psZgpCommissionIndication = psGPMessage->uMessage.psZgpCommissionIndication;

            /* add device to translation table and map commands */
            if(  bAppAddTransTableEntries(
                    psZgpCommissionIndication->uZgpdDeviceAddr,
                    (uint8)(psZgpCommissionIndication->b8AppId)
                    ) == TRUE)
            {
                psZgpCommissionIndication->eStatus = E_ZCL_SUCCESS;
                psZgpCommissionIndication->psGpToZclCommandInfo = asGpToZclLevelControlCmdInfo;
            }
            else
            {
                psZgpCommissionIndication->eStatus = E_ZCL_FAIL;
            }

            break;
        }
#endif
        case E_GP_COMMISSION_MODE_ENTER:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_COMMISSION_MODE_ENTER \n");
            break;
        }

        case E_GP_COMMISSION_MODE_EXIT:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_COMMISSION_MODE_EXIT \n");

            //vStartEffect(E_CLD_IDENTIFY_EFFECT_STOP_EFFECT);
                break;
        }

        case E_GP_CMD_UNSUPPORTED_PAYLOAD_LENGTH:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_CMD_UNSUPPORTED_PAYLOAD_LENGTH \n");
            break;
        }
#ifdef GP_COMBO_BASIC_DEVICE
        case E_GP_UPDATE_SINK_TABLE_GROUP_INFO:
        {
            tsGP_ZgppProxySinkTable *psZgpSinkTable = psGPMessage->uMessage.psZgpsProxySinkTable;
                        if(((psZgpSinkTable->b8SinkOptions & GP_SINK_TABLE_COMM_MODE_MASK)
                             == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID))
            {
                uint8 i;

                ZPS_tsAplAib *tsAplAib  = ZPS_psAplAibGetAib();
                for(i = 0; ((psZgpSinkTable->u8SinkGroupListEntries < GP_MAX_SINK_GROUP_LIST) &&
                        (i <  tsAplAib->psAplApsmeGroupTable->u32SizeOfGroupTable)); i++)
                {
                     DBG_vPrintf(TRACE_APP_GP, "\n Adding group id at index =  %d \n",i);

                    if ((tsAplAib->psAplApsmeGroupTable->psAplApsmeGroupTableId[i].au8Endpoint[0] & (1 << 0)) != 0)
                    {

                        psZgpSinkTable->asSinkGroupList[psZgpSinkTable->u8SinkGroupListEntries].u16SinkGroup =
                            tsAplAib->psAplApsmeGroupTable->psAplApsmeGroupTableId[i].u16Groupid;
                        psZgpSinkTable->asSinkGroupList[psZgpSinkTable->u8SinkGroupListEntries].u16Alias =
                            0xFFFF;

                        DBG_vPrintf(TRACE_APP_GP, "Adding group Id = 0x%8x in sink table = 0x%8x \n",
                                tsAplAib->psAplApsmeGroupTable->psAplApsmeGroupTableId[i].u16Groupid ,
                                psZgpSinkTable->asSinkGroupList[psZgpSinkTable->u8SinkGroupListEntries].u16SinkGroup );
                        psZgpSinkTable->u8SinkGroupListEntries++;

                    }
                }
            }
        }

            break;
#endif
        case E_GP_SINK_PROXY_TABLE_ENTRY_ADDED:
            break;

        case E_GP_SINK_PROXY_TABLE_FULL:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_SINK_TABLE_FULL \n");
            break;
        }

        case E_GP_ZGPD_COMMAND_RCVD:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_ZGPD_COMMAND_RCVD \n");
            break;
        }

        case E_GP_ZGPD_CMD_RCVD_WO_TRANS_ENTRY:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_ZGPD_CMD_RCVD_WO_TRANS_ENTRY \n");
            break;
        }

        case E_GP_ADDING_GROUP_TABLE_FAIL:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_ADDING_GROUP_TABLE_FAIL \n");
            break;
        }
        case E_GP_SHARED_SECURITY_KEY_TYPE_IS_NOT_ENABLED:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_SHARED_SECURITY_KEY_TYPE_IS_NOT_ENABLED \n");
            break;
        }
        case E_GP_SHARED_SECURITY_KEY_IS_NOT_ENABLED:
        {
              DBG_vPrintf(TRACE_APP_GP, "E_GP_SHARED_SECURITY_KEY_IS_NOT_ENABLED \n");
              break;
        }
        case E_GP_RECEIVED_CHANNEL_REQUEST:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_RECEIVED_CHANNEL_REQUEST \n");
            break;
        }
        case E_GP_SECURITY_LEVEL_MISMATCH:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_SECURITY_LEVEL_MISMATCH \n");
            break;
        }
        case E_GP_SECURITY_PROCESSING_FAILED:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_SECURITY_PROCESSING_FAILED \n");
            break;
        }
        case E_GP_REMOVING_GROUP_TABLE_FAIL:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_REMOVING_GROUP_TABLE_FAIL \n");
            break;
        }
        case E_GP_LINK_KEY_IS_NOT_ENABLED:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_LINK_KEY_IS_NOT_ENABLED \n");
            break;
        }
        case E_GP_SUCCESS_CMD_RCVD:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_SUCCESS_CMD_RCVD \n");

            break;
        }
        case E_GP_PERSIST_SINK_PROXY_TABLE:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_PERSIST_SINK_PROXY_TABLE \n");
            sGP_PDM_Data.u32RestoreGPPDMInfo &= ~PDM_VALID_BITS;
            sGP_PDM_Data.u32RestoreGPPDMInfo |= GP_PDM_DATA_VALID;

            PDM_eSaveRecordData(PDM_ID_APP_CLD_GP_TRANS_TABLE,
                                &sGP_PDM_Data,
                                sizeof(tsGP_PDM_Data));
            PDM_eSaveRecordData(PDM_ID_APP_CLD_GP_SINK_PROXY_TABLE,
                                   &(sGPDeviceInfo.sGreenPowerCustomDataStruct.asZgpsSinkProxyTable),
                                   sizeof(sGPDeviceInfo.sGreenPowerCustomDataStruct.asZgpsSinkProxyTable));
            break;
        }
#ifdef GP_COMBO_BASIC_DEVICE
        case E_GP_TRANSLATION_TABLE_UPDATE:
            psGPMessage->uMessage.psTransationTableUpdate->eStatus =
            eApp_UpdateTranslationTable((psGPMessage->uMessage.psTransationTableUpdate));

            break;
        case E_GP_PAIRING_CONFIGURATION_CMD_RCVD:
            vApp_UpdateTranslationTableOnPairingCfg(psGPMessage->uMessage.psPairingConfigCmdRcvd);
            sGP_PDM_Data.u32RestoreGPPDMInfo &= ~0xf0;
            sGP_PDM_Data.u32RestoreGPPDMInfo |= GP_PDM_DATA_VALID;

            break;
#endif
        case E_GP_DECOMM_CMD_RCVD:
            DBG_vPrintf(TRACE_APP_GP, "E_GP_DECOMM_CMD_RCVD ,0x%08x\n",
                    psGPMessage->uMessage.psZgpDecommissionIndication->uZgpdDeviceAddr.u32ZgpdSrcId);
#ifdef GP_COMBO_BASIC_DEVICE
            for(u8Count = 0; u8Count < GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES; u8Count++)
            {
                uint8                u8TranslationTableIndex;
                psTranslationTableEntry =
                    psApp_GPGetTranslationTable(psGPMessage->uMessage.psZgpDecommissionIndication->u8ApplicationId,
                            &u8TranslationTableIndex,
                            &(psGPMessage->uMessage.psZgpDecommissionIndication->uZgpdDeviceAddr));
                if(NULL != psTranslationTableEntry)
                {
                    memset(psTranslationTableEntry,0,sizeof(tsGP_TranslationTableEntry));
                    sGP_PDM_Data.u32RestoreGPPDMInfo &= ~(1 << (u8TranslationTableIndex + PDM_TRANS_POINTER_INFO_INDEX ));
                }
            }
            sDestinationAddress.eAddressMode = E_ZCL_AM_BROADCAST;
                    sDestinationAddress.uAddress.eBroadcastMode = ZPS_E_APL_AF_BROADCAST_RX_ON;

             eGP_ProxyCommissioningMode(
                            GREENPOWER_END_POINT_ID,
                            242,
                            sDestinationAddress,
                            E_GP_PROXY_COMMISSION_EXIT);
#endif
            break;

        default:
        {
            DBG_vPrintf(TRACE_APP_GP, "Unknown event generated = %d\n", psGPMessage->eEventType);
            break;
        }
    }

}
/****************************************************************************/
/***        Local    Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME: bAppAddTransTableEntries
 *
 * DESCRIPTION:
 * GP translation table addition during commissioning
 *
 ****************************************************************************/
#ifdef GP_COMBO_BASIC_DEVICE
bool  bAppAddTransTableEntries(
         tuGP_ZgpdDeviceAddr         uRcvdGPDAddr,
         zbmap8                      b8Options
        )
{

    uint8 u8Count = 0,u8TranslationTableIndex;
    tsGP_TranslationTableEntry *psTranslationTableEntry;
    /* uDummydeviceAddr will be used to get a unused/free entry from translation table */
    tuGP_ZgpdDeviceAddr uDummydeviceAddr = { 0 };

    /* Check if previous entry with the same device exists, delete all such entries
     * if any  */
    for(u8Count = 0; u8Count < GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES; u8Count++)
    {
        psTranslationTableEntry =
            psApp_GPGetTranslationTable(b8Options,&u8TranslationTableIndex, &(uRcvdGPDAddr));
        if( psTranslationTableEntry == NULL )
        {
            break;
        }
        else
        {
            /* clear entry */
            memset(&psTranslationTableEntry->uZgpdDeviceAddr, 0, sizeof(tuGP_ZgpdDeviceAddr));
            DBG_vPrintf(TRACE_APP_GP, "cleared translation table  \n");
            sGP_PDM_Data.u32RestoreGPPDMInfo &= ~(1 << (u8TranslationTableIndex + PDM_TRANS_POINTER_INFO_INDEX ));
        }
    }

    /* Now add translation table entry for each command supported */

    DBG_vPrintf(TRACE_APP_GP, "adding commands \n");
    /* get free translation entry( entry with 0x00(uDummydeviceAddr) as src id )*/
    psTranslationTableEntry = psApp_GPGetTranslationTable(0, &u8TranslationTableIndex,&uDummydeviceAddr);
    /* check pointer */
    if(psTranslationTableEntry != NULL)
    {
        psTranslationTableEntry->b8Options =
                b8Options;
        psTranslationTableEntry->uZgpdDeviceAddr =
                uRcvdGPDAddr;

        psTranslationTableEntry->psGpToZclCmdInfo = asGpToZclLevelControlCmdInfo;
        psTranslationTableEntry->u8NoOfCmdInfo = sizeof(asGpToZclLevelControlCmdInfo)/sizeof(tsGP_GpToZclCommandInfo);
        DBG_vPrintf(TRACE_APP_GP, "ENTRY ADDED TO THE TABLE = 0x%x\n", psTranslationTableEntry->psGpToZclCmdInfo->eZgpdCommandId);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/****************************************************************************
 *
 * NAME: psApp_GPGetTranslationTable
 *
 * DESCRIPTION:
 * Gets a given transition Table entry. To get free entry, pass address as 0.
 *
 ****************************************************************************/
PRIVATE tsGP_TranslationTableEntry* psApp_GPGetTranslationTable(
        uint8                u8AppId,
        uint8                *pu8TranslationTableIndex,
        tuGP_ZgpdDeviceAddr *uSrcAddr)
{
    uint8 u8Count;

    for(u8Count = 0; u8Count < GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES; u8Count++)
    {
        /* if ZGPD Src ID is zero then it is free entry */
        if(bGP_CheckGPDAddressMatch(
                sGP_PDM_Data.asGpTranslationTable[u8Count].b8Options,
                u8AppId,
                &sGP_PDM_Data.asGpTranslationTable[u8Count].uZgpdDeviceAddr,
                uSrcAddr
        ))
        {
            DBG_vPrintf(TRACE_APP_GP, "psApp_GPGetTranslationTable Success\n");
            *pu8TranslationTableIndex =u8Count;
            return &(sGP_PDM_Data.asGpTranslationTable[u8Count]);
        }
    }

    return NULL;
}


/****************************************************************************
 *
 * NAME: eApp_UpdateTranslationTable
 *
 * DESCRIPTION:
 * GP translation table update when translation table update command is received
 *
 * RETURNS:
 *
 ****************************************************************************/
teGP_GreenPowerStatus eApp_UpdateTranslationTable(tsGP_ZgpsTranslationTableUpdate *psUpdateCmd)
{
    uint8 i, u8TranslationTableIndex;
    teGP_GreenPowerStatus eStatus =E_GP_TRANSLATION_UPDATE_FAIL;
    tsGP_TranslationTableEntry *psTranslationTableEntry = NULL;
    /* uDummydeviceAddr will be used to get a unused/free entry from translation table */
    tuGP_ZgpdDeviceAddr uDummydeviceAddr = { 0 };
    switch(psUpdateCmd->eAction)
    {
    case E_GP_TRANSLATION_TABLE_ADD_ENTRY:
        if(psUpdateCmd->psTranslationUpdateEntry->u8Index == ANY_INDEX)
        {
            /* get free translation table entry(translation table entry with 0x0000 as src id)*/
            psTranslationTableEntry = psApp_GPGetTranslationTable(0,&u8TranslationTableIndex, &uDummydeviceAddr);
        }
        else
        {
            if(psUpdateCmd->psTranslationUpdateEntry->u8Index < GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES)
            {
                psTranslationTableEntry =
                    &sGP_PDM_Data.asGpTranslationTable[psUpdateCmd->psTranslationUpdateEntry->u8Index];
                /* check if entry is empty*/
                if(psTranslationTableEntry->uZgpdDeviceAddr.u32ZgpdSrcId != 0)
                {
                    psTranslationTableEntry = NULL;
                }
            }

        }

        /*fall through */
    case E_GP_TRANSLATION_TABLE_REPLACE_ENTRY:
        if((psUpdateCmd->psTranslationUpdateEntry->u8Index < GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES) &&
                (E_GP_TRANSLATION_TABLE_REPLACE_ENTRY == psUpdateCmd->eAction))
        {
            psTranslationTableEntry = &sGP_PDM_Data.asGpTranslationTable[psUpdateCmd->psTranslationUpdateEntry->u8Index];
            for(i = 0 ; i < GP_NUMBER_OF_CONFIGURABLE_CMD_INFO; i++)
            {
                if(psTranslationTableEntry->psGpToZclCmdInfo == &sGP_PDM_Data.asGpToZclCmdInfoUpdate[i])
                {
                    /* clear the entry, we are going to overwrite*/
                    memset(&sGP_PDM_Data.asGpToZclCmdInfoUpdate[i],
                            0,
                            sizeof(tsGP_GpToZclCommandInfo));
                    sGP_PDM_Data.u32RestoreGPPDMInfo &= ~(1 << (psUpdateCmd->psTranslationUpdateEntry->u8Index + PDM_TRANS_POINTER_INFO_INDEX ));
                    u8TranslationTableIndex =psUpdateCmd->psTranslationUpdateEntry->u8Index;
                    break;
                }
            }
        }
        if(psTranslationTableEntry != NULL)
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_TRANSLATION_TABLE_ADD_ENTRY check\n");
            psTranslationTableEntry->psGpToZclCmdInfo = NULL;
            /* get a free entry to add command details*/
            for(i = 0 ; i < GP_NUMBER_OF_CONFIGURABLE_CMD_INFO; i++)
            {
                if(sGP_PDM_Data.asGpToZclCmdInfoUpdate[i].u8EndpointId == 0x00)
                {
                    /* clear the entry, we are going to overwrite*/
                    psTranslationTableEntry->psGpToZclCmdInfo =
                        &sGP_PDM_Data.asGpToZclCmdInfoUpdate[i];
                    sGP_PDM_Data.u32RestoreGPPDMInfo |= (1 << (u8TranslationTableIndex + PDM_TRANS_POINTER_INFO_INDEX ));
                    break;
                }
            }
            if(psTranslationTableEntry->psGpToZclCmdInfo == NULL)
            {
                break;
            }
            if(psUpdateCmd->psTranslationUpdateEntry->u8EndpointId < 0xF0 )
            {
                if(psUpdateCmd->psTranslationUpdateEntry->u8EndpointId != app_u8GetDeviceEndpoint() )
                {
                    break;
                }
            }

            if(psUpdateCmd->psTranslationUpdateEntry->u8EndpointId == 0xFF)
            {
                psTranslationTableEntry->psGpToZclCmdInfo->u8EndpointId = 0xFF;
            }
            else
            {
                psTranslationTableEntry->psGpToZclCmdInfo->u8EndpointId =
                    app_u8GetDeviceEndpoint();
            }

            psTranslationTableEntry->b8Options =
                            psUpdateCmd->u8ApplicationId;
                        psTranslationTableEntry->uZgpdDeviceAddr =
                            psUpdateCmd->uZgpdDeviceAddr;

            psTranslationTableEntry->psGpToZclCmdInfo->eZgpdCommandId =
                psUpdateCmd->psTranslationUpdateEntry->eZgpdCommandId;

            psTranslationTableEntry->psGpToZclCmdInfo->u8ZbCommandId =
                psUpdateCmd->psTranslationUpdateEntry->u8ZbCommandId;




            psTranslationTableEntry->psGpToZclCmdInfo->u16ZbClusterId =
                psUpdateCmd->psTranslationUpdateEntry->u16ZbClusterId;

            psTranslationTableEntry->psGpToZclCmdInfo->u8ZbCmdLength =
                psUpdateCmd->psTranslationUpdateEntry->u8ZbCmdLength;

            memcpy(&psTranslationTableEntry->psGpToZclCmdInfo->au8ZbCmdPayload[0],
                    &psUpdateCmd->psTranslationUpdateEntry->au8ZbCmdPayload[0],
                    psTranslationTableEntry->psGpToZclCmdInfo->u8ZbCmdLength);

            eStatus =E_GP_TRANSLATION_UPDATE_SUCCESS;

        }
        break;
    case E_GP_TRANSLATION_TABLE_REMOVE_ENTRY:
        if(psUpdateCmd->psTranslationUpdateEntry->u8Index < GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES)
        {
            psTranslationTableEntry =
                &sGP_PDM_Data.asGpTranslationTable[psUpdateCmd->psTranslationUpdateEntry->u8Index];
            memset(psTranslationTableEntry,0,sizeof(tsGP_TranslationTableEntry));
            eStatus = E_GP_TRANSLATION_UPDATE_SUCCESS;
            sGP_PDM_Data.u32RestoreGPPDMInfo &= ~(1 << (psUpdateCmd->psTranslationUpdateEntry->u8Index + PDM_TRANS_POINTER_INFO_INDEX ));
        }

        break;
    default:
        break;
    }
    return eStatus;
}

/****************************************************************************
 *
 * NAME: vApp_UpdateTranslationTableOnpairingCfg
 *
 * DESCRIPTION:
 * GP translation table update when pairing configuration command is received
 *
 *
 ****************************************************************************/

void vApp_UpdateTranslationTableOnPairingCfg(tsGP_ZgpsPairingConfigCmdRcvd *psPairingConfigCmdRcvd)
{

    tsGP_TranslationTableEntry *psTranslationTableEntry = NULL;
    bool u8Count;
    uint8 i = 0, j=0;
    bool bAddTranslationTable = FALSE;

    switch(psPairingConfigCmdRcvd->eTranslationTableAction)
    {
        case E_GP_PAIRING_CONFIG_TRANSLATION_TABLE_ADD_ENTRY:
        {
            DBG_vPrintf(TRACE_APP_GP, "E_GP_PAIRING_CONFIG_EXTEND_SINK_TABLE_ENTRY  \n");
            /* If the Number of paired endpoints field is set to 0xfe,
             * there paired endpoints are to be derived by the sink itself */
            if(psPairingConfigCmdRcvd->u8NumberOfPairedEndpoints == 0xFE)
            {
                /* In the current version of the specification, a device SHALL only send GP Pairing
                 * Configuration command with the Number of paired endpoints field set to 0xfe, if the CommunicationMode is equal to Pre-Commissioned Groupcast.*/
                if(psPairingConfigCmdRcvd->eCommunicationMode == E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID)
                {
                    /* check if any group added to application endpoint */
                    ZPS_tsAplAib * tsAplAib  = ZPS_psAplAibGetAib();
                    for(i = 0; ((i < psPairingConfigCmdRcvd->u8SinkGroupListEntries) &&
                                (bAddTranslationTable == FALSE)); i++)
                    {
                        for(j=0; j < tsAplAib->psAplApsmeGroupTable->u32SizeOfGroupTable;j++)
                        {
                            /*  If the group id a*/
                            if(tsAplAib->psAplApsmeGroupTable->psAplApsmeGroupTableId[j].u16Groupid ==
                                psPairingConfigCmdRcvd->asSinkGroupList[i].u16SinkGroup)
                            {
                                bAddTranslationTable = TRUE;
                                break;
                            }
                        }
                    }
                }
                else
                {
                     bAddTranslationTable = TRUE;
                }
            }
            else if ((psPairingConfigCmdRcvd->u8NumberOfPairedEndpoints > 0 ) &&
                    (psPairingConfigCmdRcvd->u8NumberOfPairedEndpoints < 0xFD))
            {
                /* check if we supports the endpoint provided */
                for(i = 0; (i < psPairingConfigCmdRcvd->u8NumberOfPairedEndpoints);i++)
                {
                    if(psPairingConfigCmdRcvd->au8PairedEndpointList[i] == app_u8GetDeviceEndpoint() )
                    {
                        bAddTranslationTable = TRUE;
                        break;
                    }
                }
            }
            if(bAddTranslationTable == FALSE)
            {
                return;
            }
            bAppAddTransTableEntries(
                 psPairingConfigCmdRcvd->uZgpdDeviceAddr,
                 psPairingConfigCmdRcvd->u8ApplicationId
                );
            DBG_vPrintf(TRACE_APP_GP, "psPairingConfigCmdRcvd->eZgpdDeviceId = %d \n", psPairingConfigCmdRcvd->eZgpdDeviceId);
        }
        break;
    case E_GP_PAIRING_CONFIG_TRANSLATION_TABLE_REMOVE_ENTRY:

        DBG_vPrintf(TRACE_APP_GP, "E_GP_PAIRING_CONFIG_REMOVE_SINK_TABLE_ENTRY  \n");
        for(u8Count = 0; u8Count < GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES; u8Count++)
        {
            uint8 u8TranslationTableIndex;
            psTranslationTableEntry = psApp_GPGetTranslationTable(
                    psPairingConfigCmdRcvd->u8ApplicationId,
                    &u8TranslationTableIndex,
                    &psPairingConfigCmdRcvd->uZgpdDeviceAddr);
            if(NULL != psTranslationTableEntry)
            {
                memset(psTranslationTableEntry,0,sizeof(tsGP_TranslationTableEntry));
                sGP_PDM_Data.u32RestoreGPPDMInfo &= ~(1 << (u8TranslationTableIndex + PDM_TRANS_POINTER_INFO_INDEX ));
            }
        }
        break;
    default:
        DBG_vPrintf(TRACE_APP_GP, "vApp_UpdateTranslationTableOnPairingCfg default   \n");
        break;
    }

}

/****************************************************************************
 *
 * NAME: vInitTranslationTablePointers
 *
 * DESCRIPTION:
 * Initialises the pointers of translation table when restored from PDM
 *
 *
 ****************************************************************************/
void vInitTranslationTablePointers(void)
{
    uint8 u8Count;
    bool bIsCmdInfoUpdated;
    tuGP_ZgpdDeviceAddr uDummydeviceAddr = { 0 };
    for(u8Count = 0; u8Count < GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES; u8Count++)
    {
        /* if ZGPD Src ID is zero then it is free entry */
        if((bGP_CheckGPDAddressMatch(
                sGP_PDM_Data.asGpTranslationTable[u8Count].b8Options,
                sGP_PDM_Data.asGpTranslationTable[u8Count].b8Options,
                &sGP_PDM_Data.asGpTranslationTable[u8Count].uZgpdDeviceAddr,
                &uDummydeviceAddr
        ) != E_ZCL_SUCCESS))
        {
            bIsCmdInfoUpdated = sGP_PDM_Data.u32RestoreGPPDMInfo & ( 1 << (u8Count + PDM_TRANS_POINTER_INFO_INDEX));
            if(bIsCmdInfoUpdated)
            {
                sGP_PDM_Data.asGpTranslationTable[u8Count].psGpToZclCmdInfo = sGP_PDM_Data.asGpToZclCmdInfoUpdate;
            }
            else
            {
                sGP_PDM_Data.asGpTranslationTable[u8Count].psGpToZclCmdInfo = asGpToZclLevelControlCmdInfo;
            }
        }
    }

}
#endif
#endif /*  CLD_GREENPOWER */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
