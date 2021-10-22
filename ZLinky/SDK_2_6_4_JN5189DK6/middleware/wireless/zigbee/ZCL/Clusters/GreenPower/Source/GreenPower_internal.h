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
 * COMPONENT:          GreenPower_internal.h
 *
 * DESCRIPTION:        The Internal API for the Green Power Cluster
 *
 *****************************************************************************/

#ifndef  GP_INTERNAL_H_INCLUDED
#define  GP_INTERNAL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"
#include "zcl.h"
#include "GreenPower.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define GP_GENERAL_CLUSTER_ID_IDENTIFY                                  (0x0003)

#define GP_GENERAL_CLUSTER_ID_ONOFF                                     (0x0006)

#define GP_GENERAL_CLUSTER_ID_LEVEL_CONTROL                             (0x0008)

#define GP_GENERAL_CLUSTER_ID_TEMP_MEASUREMENT                          (0x0402)
#define GP_MAX_COMMISSION_REPLY_PAYLOAD_LENGTH                          27

#define GP_NWK_MIN_BROADCAST_ADDRESS                                    (0xFFF8)
#define GP_ATTRIB_SEC_BITS												0x03 // First two bits
#define GP_ATTRIB_INVOLVE_TC_BITS										0x08
    /* GP Cluster Field MASKS */
#define GP_APPLICATION_ID_MASK                                          (0x0007)

#define GP_NOTIFICATION_APPOINT_TEMP_MASTER_MASK                        (0x0800)
#define GP_NOTIFICATION_SECURITY_LEVEL_MASK                             (0x00C0)
#define GP_NOTIFICATION_SECURITY_KEY_TYPE_MASK                          (0x0700)
#define GP_NOTIFICATION_TX_QUEUE_FULL                                   (0x1000)
#define GP_NOTIFICATION_ALSO_UNICAST_MASK                               (0x0008)
#define GP_NOTIFICATION_ALSO_DERIVED_GROUP_MASK                         (0x0010)
#define GP_NOTIFICATION_ALSO_COMMISSION_GROUP_MASK                      (0x0020)

#define GP_CMSNG_NOTIFICATION_RX_AFTER_TX_MASK                          (0x0008)
#define GP_CMSNG_NOTIFICATION_SECURITY_LEVEL_MASK                       (0x0030)
#define GP_CMSNG_NOTIFICATION_SECURITY_KEY_TYPE_MASK                    (0x01C0)
#define GP_CMSNG_NOTIFICATION_SEC_PROCESS_FAIL_MASK                     (0x0200)
#define GP_CMSNG_NOTIFICATION_PROXY_INFO_MASK                           (0x0800)
#define GP_NOTIFICATION_PROXY_INFO_PRESENT                              (0x4000)

#define GP_PROXY_CMSNG_MODE_ACTION_FIELD_MASK                           (0x01)
#define GP_PROXY_CMSNG_MODE_EXIT_MODE_ON_WINDOW_EXPIRE_MASK             (0x02)
#define GP_PROXY_CMSNG_MODE_EXIT_MODE_ON_PAIRING_SUCCESS_MASK           (0x04)
#define GP_PROXY_CMSNG_MODE_EXIT_MODE_ON_PROXY_MODE_CMD_MASK            (0x08)
#define GP_PROXY_CMSNG_MODE_CHANNEL_PRESENT_MASK                        (0x10)
#define GP_PROXY_CMSNG_MODE_UNICAST_COMMUN_MASK                         (0x20)

#define GP_PAIRING_ADD_SINK_MASK                                        (0x000008)
#define GP_PAIRING_REMOVE_GPD_MASK                                      (0x000010)
#define GP_PAIRING_COMM_MODE_MASK                                       (0x000060)
#define GP_PAIRING_GPD_FIXED_MASK                                       (0x000080)
#define GP_PAIRING_SEQ_NUM_CAP_MASK                                     (0x000100)
#define GP_PAIRING_SEC_LEVEL_MASK                                       (0x000600)
#define GP_PAIRING_SEC_KEY_TYPE_MASK                                    (0x003800)
#define GP_PAIRING_SECURITY_COUNTER_PRESENT_MASK                        (0x004000)
#define GP_PAIRING_SECURITY_KEY_PRESENT_MASK                            (0x008000)
#define GP_PAIRING_ASSIGNED_ALIAS_PRESENT_MASK                          (0x010000)
#define GP_PAIRING_FORWARDING_RADIUS_PRESENT_MASK                       (0x020000)


#define GP_ZGP_ACTIVE_FEATURES_ATTR_DEFAULT_VALUE                      (0x00FFFFFF)


/* sink table options field masks */

#define GP_SINK_TABLE_COMM_MODE_MASK                                    (0x0F)
#define GP_SINK_TABLE_RX_ON_MASK                                        (0x10)
#define GP_SECURITY_LEVEL_MASK                                          (0x03)
#define GP_SECURITY_KEY_TYPE_MASK                                       (0x1C)

/* proxy table options field masks */
#define GP_PROXY_TABLE_ENTRY_ACTIVE_MASK                                (0x0008)
#define GP_PROXY_TABLE_ENTRY_VALID_MASK                                 (0x0010)
#define GP_PROXY_TABLE_SEQ_NUM_CAP_MASK                                 (0x0020)
#define GP_PROXY_TABLE_UNICAST_GPS_MASK                                 (0x0040)
#define GP_PROXY_TABLE_DERIVED_GROUP_GPS_MASK                           (0x0080)
#define GP_PROXY_TABLE_COMMISSION_GROUP_GPS_MASK                        (0x0100)
#define GP_PROXY_TABLE_FIRST_TO_FWD_MASK                                (0x0200)
#define GP_PROXY_TABLE_IN_RANGE_MASK                                    (0x0400)
#define GP_PROXY_TABLE_GPD_FIXED_MASK                                   (0x0800)
#define GP_PROXY_TABLE_HAS_ALL_UNICAST_ROUTES_MASK                      (0x1000)
#define GP_PROXY_TABLE_ASSIGNED_ALIAS_MASK                              (0x2000)
#define GP_PROXY_TABLE_SECURITY_USE_MASK                                (0x4000)

/* GP Translation table update command options field mask */
#define GP_TRANS_UPDATE_ACTION_MASK                                     (0x0018)
#define GP_TRANS_UPDATE_NUM_OF_TRANSLATIONS_MASK                        (0x00E0)

/* GP Pairing search command options field mask */
#define GP_PAIRING_SEARCH_RQST_UNICAST_SINK_MASK                        (0x0008)
#define GP_PAIRING_SEARCH_RQST_DERIVED_GROUPCAST_SINK_MASK              (0x0010)
#define GP_PAIRING_SEARCH_RQST_COMMISSION_GROUPCAST_SINK_MASK           (0x0020)
#define GP_PAIRING_SEARCH_RQST_SECURITY_COUNTER_MASK                    (0x0040)
#define GP_PAIRING_SEARCH_RQST_SECURITY_KEY_MASK                        (0x0080)

/* GP Pairing Configuration command actions field mask */
#define GP_PAIRING_CONFIG_ACTION_MASK                                   (0x07)
#define GP_PAIRING_CONFIG_SEND_GP_PAIRING_MASK                          (0x08)
#define GP_PAIRING_CONFIG_COMM_MODE_MASK                                (0x0018)
#define GP_PAIRING_CONFIG_ASSIGNED_ADDR_MASK                            (0x0100)
#define GP_PAIRING_CONFIG_SECURITY_USE_MASK                             (0x0200)

/* Commission Cmd Options Fields Mask */
#define GP_COMM_CMD_MAC_SEQ_NUM_CAP_MASK                                (0x01)
#define GP_COMM_CMD_RX_ON_CAP_MASK                                      (0x02)
#define GP_COMM_CMD_APP_INFO_PRESENT                                    (0x04)
#define GP_COMM_CMD_PANID_REQ_MASK                                      (0x10)
#define GP_COMM_CMD_ZGP_SEC_KEY_REQ_MASK                                (0x20)
#define GP_COMM_CMD_FIXED_LOCATION_MASK                                 (0x40)
#define GP_COMM_CMD_EXT_OPT_FIELD_MASK                                  (0x80)

/* Commission Cmd Extended Options Fields Mask */
#define GP_COMM_CMD_EXT_OPT_SEC_LEVEL_MASK                              (0x03)
#define GP_COMM_CMD_EXT_OPT_KEY_TYPE_MASK                               (0x1C)
#define GP_COMM_CMD_EXT_OPT_ZGPD_KEY_PRESENT_MASK                       (0x20)
#define GP_COMM_CMD_EXT_OPT_ZGPD_KEY_ENC_MASK                           (0x40)
#define GP_COMM_CMD_EXT_OPT_ZGPD_OUT_GOING_COUNTER_MASK                 (0x80)

/* application info field masks*/
#define GP_COMM_CMD_APP_INFO_MANU_ID                                    (0x01)
#define GP_COMM_CMD_APP_INFO_MODEL_ID                                   (0x02)
#define GP_COMM_CMD_APP_INFO_CMD_ID                                    (0x04)
#define GP_COMM_CMD_APP_INFO_CLUSTER_ID                                 (0x08)

#define GP_APPL_ID_4_BYTE                                               (0x00)
#define GP_APPL_ID_8_BYTE                                               (0x02)

#define BIT_MAP_REQUEST_TYPE                                             (0x18)

#define GP_SINK_TABLE_MINIMUM_LENGTH                                     8
#define GP_PROXY_TABLE_MINIMUM_LENGTH                                    7
#define GP_PROTECT_WITH_LINK_KEY_BIT								     4

#define LQI_POOR             											0
#define LQI_MODERATE            										1
#define LQI_HIGH             											2
#define LQI_EXCELLENT             										3

#define GP_SEC_ATTRIB_ENCRYPTION										4
#define GP_CHANNEL_CONFIG_BASIC_BIT										0x10
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum
{
	E_GP_NOT_PAIRED,
	E_GP_PAIRED,
	E_GP_PAIRING_NOT_COMPLETED,
	E_GP_DECOMMISSIONED
}E_GPD_STATUS;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
#ifdef GP_AT_TESTING
		extern uint8 u8gpATTestAvoidGPResponse;
#endif
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/* Server Transmission Cmd APIs */
PUBLIC teZCL_Status eGP_ProxyCommissioningModeSend(
                    uint8                                       u8SourceEndPointId,
                    uint8                                       u8DestinationEndPointId,
                    tsZCL_Address                               *psDestinationAddress,
                    uint8                                       *pu8TransactionSequenceNumber,
                    tsGP_ZgpProxyCommissioningModeCmdPayload    *psZgpProxyCommissioningModeCmdPayload);

/* Client Transmission Cmd APIs */
PUBLIC teZCL_Status eGP_ZgpNotificationSend(
                    uint8                                       u8SourceEndPointId,
                    uint8                                       u8DestinationEndPointId,
                    tsZCL_Address                               *psDestinationAddress,
                    uint8                                       *pu8TransactionSequenceNumber,
                    tsGP_ZgpNotificationCmdPayload              *psZgpNotificationPayload);

PUBLIC teZCL_Status eGP_ZgpCommissioningNotificationSend(
                    uint8                                       u8SourceEndPointId,
                    uint8                                       u8DestinationEndPointId,
                    tsZCL_Address                               *psDestinationAddress,
                    uint8                                       *pu8TransactionSequenceNumber,
                    tsGP_ZgpCommissioningNotificationCmdPayload *psZgpCmsngNotificationPayload);

/* Client Received Cmd APIs */
PUBLIC teZCL_Status eGP_ProxyCommissioningModeReceive(
                    ZPS_tsAfEvent                               *pZPSevent,
                    uint16                                      u16Offset,
                    tsGP_ZgpProxyCommissioningModeCmdPayload    *psZgpProxyCommissioningModeCmdPayload);

/* Server Received Cmd APIs */
PUBLIC teZCL_Status eGP_ZgpNotificationReceive(
                    ZPS_tsAfEvent                               *pZPSevent,
                    uint16                                      u16Offset,
                    tsGP_ZgpNotificationCmdPayload              *psZgpNotificationPayload);

PUBLIC teZCL_Status eGP_ZgpCommissioningNotificationReceive(
                    ZPS_tsAfEvent                               *pZPSevent,
uint16                                      u16Offset,
                    tsGP_ZgpCommissioningNotificationCmdPayload *psZgpCmsngNotificationPayload);

/* Green Power Command Handler APIs */
PUBLIC teZCL_Status eGP_HandleProxyCommissioningMode(
                    ZPS_tsAfEvent                               *pZPSevent,
                    tsZCL_EndPointDefinition                    *psEndPointDefinition,
                    tsZCL_ClusterInstance                       *psClusterInstance,
                    uint16                                      u16Offset);

PUBLIC teZCL_Status eGP_HandleZgpNotification(
                    ZPS_tsAfEvent                               *pZPSevent,
                    tsZCL_EndPointDefinition                    *psEndPointDefinition,
                    tsZCL_ClusterInstance                       *psClusterInstance,
                    uint16                                      u16Offset);

PUBLIC teZCL_Status eGP_HandleZgpCommissioningNotification(
                    ZPS_tsAfEvent                               *pZPSevent,
                    tsZCL_EndPointDefinition                    *psEndPointDefinition,
                    tsZCL_ClusterInstance                       *psClusterInstance,
                    uint16                                      u16Offset);

/* Green Power Cluster command Handler */
PUBLIC teZCL_Status eGP_GreenPowerCommandHandler(
                    ZPS_tsAfEvent                               *pZPSevent,
                    tsZCL_EndPointDefinition                    *psEndPointDefinition,
                    tsZCL_ClusterInstance                       *psClusterInstance);

/* Green Power Cluster Direct Command Handler */
PUBLIC teZCL_Status eGP_GreenPowerDirectCommandHandler(
                    ZPS_tsAfEvent                               *pZPSevent,
                    tsZCL_EndPointDefinition                    *psEndPointDefinition,
                    tsZCL_ClusterInstance                       *psClusterInstance);

PUBLIC uint16 u16GP_DeriveAliasSrcAddr(
                    uint8                                       u8ApplicationId,
                    tuGP_ZgpdDeviceAddr                         uZgpdID);

PUBLIC uint8 u8GP_DeriveAliasSeqNum(
                    uint8                                       u8GpdfSeqNum,
                    teGP_GreenPowerClusterCmdIds                eGpCmdId,
                    teGP_GreenPowerCommunicationMode            eCommunicationMode);

PUBLIC uint32 u32GP_CRC32(
                    uint8                                       *pu8Data,
                    uint32                                      u32Len);

PUBLIC bool_t bGP_IsDuplicatePkt(
		            uint8								   u8TimeOutInSec,
                    uint8                                  u8SourceEndPointId,
                    bool_t                                   bIsServer,
                    uint8                                  u8ApplicationID,
                    tuGP_ZgpdDeviceAddr                    uZgpdDeviceAddr,
                    uint8                                  u8SecLevel,
                    uint32                                 u32SeqNoOrCounter,
                    uint8                                  u8CommandId,
                    tsGP_GreenPowerCustomData              *psGpCustomDataStructure);

PUBLIC bool_t bGP_IsFreshPkt(
                    uint8                                       u8EndPointId,
                    uint8                                       u8ApplicationId,
                    tuGP_ZgpdDeviceAddr                         *puZgpdAddress,
                    tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
                    tsGP_ZgppProxySinkTable                          *psTableEntry,
                    uint32                                      u32SeqNoOrCounter,
                    uint8                                       u8SecurityLevel );

PUBLIC teZCL_Status eGP_GreenPowerAutoCommissionCmdHandler(
                    ZPS_tsAfEvent                               *pZPSevent,
                    tsZCL_EndPointDefinition                    *psEndPointDefinition,
                    tsZCL_ClusterInstance                       *psClusterInstance);

PUBLIC teZCL_Status eGP_GPDFHandler(
                    ZPS_tsAfEvent                               *pZPSevent,
                    tsZCL_EndPointDefinition                    *psEndPointDefinition,
                    tsGP_GreenPowerCustomData                   *psGpCustomDataStructure,
                    uint8                                       u8ApplicationId,
                    tuGP_ZgpdDeviceAddr                         *puZgpdAddress,
                    teGP_ZgpdCommandId                          eZgpdCommandId,
                    uint8                                       u8GpdCommandPayloadLength,
                    uint8                                       *pu8GpdCommandPayload);

PUBLIC teZCL_Status u16GP_APduInstanceReadWriteLongString(
                    uint8                                       u8EndPoint,
                    uint16                                      u16AttributeId,
                    bool_t                                      IsClientAttribute,
                    bool_t                                      bWriteAttributeValueIntoBuffer,
                    PDUM_thAPduInstance                         hAPduInst,
                    uint16                                      u16Pos,
                    uint16                                      *pu16NoOfBytes);


PUBLIC uint16 u16GP_ReadFromSinkTableToString(
                    uint8                                       u8GreenPowerEndPointId,
                    uint8                                       *pu8Data,
                    uint16                                      u16StringSize);

PUBLIC uint16 u16GP_GetStringSizeOfSinkTable(
                    uint8                                       u8GreenPowerEndPointId,
                    uint8										*pu8NoOfSinkTableEntries,
                    tsGP_GreenPowerCustomData                   *psGpCustomData);


PUBLIC uint16 u16GP_ReadProxyTableToString(
                    uint8                                       u8GreenPowerEndPointId,
                    uint8                                       *pu8Data,
                    uint16                                      u16StringSize);

PUBLIC uint16 u16GP_GetStringSizeOfProxyTable(
                    uint8                                       u8GreenPowerEndPointId,
                    uint8										*pu8NoOfSinkTableEntries,
                  	tsGP_GreenPowerCustomData                   *psGpCustomData);

PUBLIC void vGp_TransmissionTimerCallback(
                    uint8                                       u8SrcEndPointId,
                    tsZCL_EndPointDefinition                    *psEndPointDefinition,
                    tsGP_GreenPowerCustomData                   *psGpCustomDataStructure);

PUBLIC teZCL_Status eGp_BufferTransmissionPacket(
                    tsGP_ZgpDataIndication                      *psZgpDataIndication,
                    teGP_GreenPowerBufferedCommands             eGreenPowerBufferedCommand,
                    tsGP_GreenPowerCustomData                   *psGreenPowerCustomData);


PUBLIC teZCL_Status eGP_ZgpPairingReceive(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint16                                 u16Offset,
                    tsGP_ZgpPairingCmdPayload              *psZgpPairingPayload);


PUBLIC teZCL_Status eGP_ZgpPairingConfigReceive(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint16                                 u16Offset,
                    tsGP_ZgpPairingConfigCmdPayload        *psZgpPairingConfigPayload);

PUBLIC teZCL_Status eGP_ZgpPairingSearchSend(
                    uint8                                  u8SourceEndPointId,
                    uint8                                  u8DestinationEndPointId,
                    tsZCL_Address                          *psDestinationAddress,
                    uint8                                  *pu8TransactionSequenceNumber,
                    tsGP_ZgpPairingSearchCmdPayload        *psZgpPairingSearchPayload);

PUBLIC teZCL_Status eGP_ZgpPairingSearchReceive(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint16                                 u16Offset,
                    tsGP_ZgpPairingSearchCmdPayload        *psZgpPairingSearchPayload);



PUBLIC teZCL_Status eGP_ZgpTranslationTableRequestReceive(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint8                                  *pu8StartIndex);

PUBLIC teZCL_Status eGP_ZgpTranslationTableResponseSend(
                    uint8                                  u8SourceEndPointId,
                    uint8                                  u8DestinationEndPointId,
                    tsZCL_Address                          *psDestinationAddress,
                    uint8                                  *pu8TransactionSequenceNumber,
                    tsGP_ZgpTransTableResponseCmdPayload   *psZgpTransTableResponsePayload);

PUBLIC teZCL_Status eGP_ZgpTranslationTableResponseReceive(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint16                                 u16Offset,
                    tsGP_ZgpTransTableResponseCmdPayload   *psZgpTransTableResponsePayload);

PUBLIC teZCL_Status eGP_ZgpTranslationTableUpdateReceive(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint16                                 u16Offset,
                    tsGP_ZgpTranslationUpdateCmdPayload    *psZgpTransTableUpdatePayload);

PUBLIC teZCL_Status eGP_HandleZgpPairing(
                    ZPS_tsAfEvent                          *pZPSevent,
                    tsZCL_EndPointDefinition               *psEndPointDefinition,
                    tsZCL_ClusterInstance                  *psClusterInstance,
                    uint16                                 u16Offset);

PUBLIC teZCL_Status eGP_HandleZgpTranslationRequest(
                    ZPS_tsAfEvent                          *pZPSevent,
                    tsZCL_EndPointDefinition               *psEndPointDefinition,
                    tsZCL_ClusterInstance                  *psClusterInstance,
                    uint16                                 u16Offset);

PUBLIC teZCL_Status eGP_HandleZgpTranslationResponse(
                    ZPS_tsAfEvent                          *pZPSevent,
                    tsZCL_EndPointDefinition               *psEndPointDefinition,
                    tsZCL_ClusterInstance                  *psClusterInstance,
                    uint16                                 u16Offset);

PUBLIC teZCL_Status eGP_HandleZgpTranslationTableUpdate(
                    ZPS_tsAfEvent                          *pZPSevent,
                    tsZCL_EndPointDefinition               *psEndPointDefinition,
                    tsZCL_ClusterInstance                  *psClusterInstance,
                    uint16                                 u16Offset);

PUBLIC teZCL_Status eGP_HandleZgpPairingSearch(
                    ZPS_tsAfEvent                          *pZPSevent,
                    tsZCL_EndPointDefinition               *psEndPointDefinition,
                    tsZCL_ClusterInstance                  *psClusterInstance,
                    uint16                                 u16Offset);

PUBLIC teZCL_Status eGP_HandleZgpPairingConfig(
                    ZPS_tsAfEvent                          *pZPSevent,
                    tsZCL_EndPointDefinition               *psEndPointDefinition,
                    tsZCL_ClusterInstance                  *psClusterInstance,
                    uint16                                 u16Offset);


PUBLIC bool_t bGP_IsZgpdCommandSupported(
                    teGP_ZgpdCommandId                     eZgpdCommandId,
                    uint8                                  u8ZbCommandId,
                    uint16                                 u16ZbClusterId);

PUBLIC teZCL_Status eGP_GreenPowerChannelReqCmdHandler(
                    ZPS_tsAfEvent                               *pZPSevent,
                    tsZCL_EndPointDefinition                    *psEndPointDefinition,
                    tsZCL_ClusterInstance                       *psClusterInstance);

PUBLIC uint8 u8GP_GetProxyDistance(
		uint8                              u8LinkQuality ,
		uint8							   u8RSSI);

PUBLIC uint8 u8GP_GetProxyDelay(uint8 u8LinkQuality);

PUBLIC teZCL_Status eGP_ZgpResponseReceive(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint16                                 u16Offset,
                    tsGP_ZgpResponseCmdPayload             *psZgpResponseCmdPayload);

PUBLIC teZCL_Status eGP_HandleZgpReponse(
                    ZPS_tsAfEvent                               *pZPSevent,
                    tsZCL_EndPointDefinition                    *psEndPointDefinition,
                    tsZCL_ClusterInstance                       *psClusterInstance,
                    uint16                                      u16Offset);

PUBLIC teZCL_Status eGP_GreenPowerCommissionCmdHandler(
                    ZPS_tsAfEvent                               *pZPSevent,
                    tsZCL_EndPointDefinition                    *psEndPointDefinition,
                    tsZCL_ClusterInstance                       *psClusterInstance);

PUBLIC teZCL_Status eGP_UpdateSinkTable(
                                        tsZCL_EndPointDefinition        *psEndPointDefinition,
                                        tsGP_GreenPowerCustomData       *psGpCustomDataStructure,
                                        tsGP_ZgppProxySinkTable         *psZgpsSinkTable,
                               		    tsGP_ZgpDataIndication          *psZgpDataIndication,
                               		    tsGP_ZgpCommissionIndication    *psZgpCommissionIndication);

PUBLIC teZCL_Status eGP_UpdateApsGroupTableAndDeviceAnnounce(
                    tsZCL_EndPointDefinition        *psEndPointDefinition,
                    tsGP_GreenPowerCustomData       *psGpCustomDataStructure,
                    tsGP_ZgpDataIndication          *psZgpDataIndication,
                    teZCL_Status                    eCommIndStatus);

PUBLIC void vGP_RemoveGPEPFromGroup(tsGP_GreenPowerCustomData   *psGpCustomDataStructure,
                                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                                    tsGP_ZgppProxySinkTable          *psSinkTableEntry);

PUBLIC void vGP_AddGPEPToGroup(     tsGP_GreenPowerCustomData   *psGpCustomDataStructure,
                                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                                    tsGP_ZgppProxySinkTable          *psSinkTableEntry);

PUBLIC teZCL_Status eGP_GreenPowerSuccessCmdHandler(
                    ZPS_tsAfEvent                               *pZPSevent,
                    tsZCL_EndPointDefinition                    *psEndPointDefinition,
                    tsZCL_ClusterInstance                       *psClusterInstance);

PUBLIC teZCL_Status eGP_GreenPowerDeCommissionCmdHandler(
                    ZPS_tsAfEvent                               *pZPSevent,
                    tsZCL_EndPointDefinition                    *psEndPointDefinition,
                    tsZCL_ClusterInstance                       *psClusterInstance);

PUBLIC uint8 u8GP_GetDataReqHandle(
                    tsGP_GreenPowerCustomData                   *psGpCustomDataStructure);


PUBLIC uint16 u16GP_GetClusterId(teGP_ZgpdCommandId              eZgpdCommandId);
void vGP_ExitCommMode(
		tsZCL_EndPointDefinition                                 *psEndPointDefinition,
		tsGP_GreenPowerCustomData                                *psGpCustomDataStructure);

bool_t bEncryptDecryptKey(
		bool_t											 bEncrypt,
		uint8                                             u8AppId,
		tuGP_ZgpdDeviceAddr                              *puGPDAddress,
		uint32                                           u32FrameCounterIfOutGoing,
		uint8											*pu8DataEncyptDecrypt,
		uint8											*pu8DataOut,
		uint8											*u8MIC,
		tsZCL_EndPointDefinition    					*psEndPointDefinition,
		tsGP_GreenPowerCustomData                       *psGpCustomDataStructure
		);

bool_t bValidatePacket(
		tsZCL_EndPointDefinition    						*psEndPointDefinition,
		tsGP_GreenPowerCustomData                       	*psGpCustomDataStructure,
	    bool_t     										    *pbIsEntryPresent,
	    uint8												u8AppId,
	    uint32												u32FrameCounter,
		tsGP_ZgppProxySinkTable                              **psZgpsSinkTable,
	   tuGP_ZgpdDeviceAddr                            *puZgpdAddress
);

PUBLIC bool_t bValidatePacketLength(
		tsZCL_EndPointDefinition    						*psEndPointDefinition,
		tsGP_ZgpDataIndication                              *psZgpDataIndication,
		tsGP_GreenPowerCustomData                       	*psGpCustomDataStructure );
PUBLIC  void vGP_HandleGPDCommand(
		 tsGP_ZgpBufferedApduRecord                         *psZgpBufferedApduRecord,
		 tsZCL_EndPointDefinition                           *psEndPointDefinition,
		 tsGP_GreenPowerCustomData                          *psGpCustomDataStructure);


void vCreateDataIndFromBufferedCmd(
		tsGP_ZgpBufferedApduRecord                           *psZgpBufferedApduRecord,
		tsGP_ZgpDataIndication                               *psZgpDataIndication);

teZCL_Status  eGP_AddUpdateSinkTable(
		 tsGP_ZgpDataIndication               		           *psZgpDataIndication,
		 tsZCL_EndPointDefinition    		                   *psEndPointDefinition,
		 tsGP_GreenPowerCustomData                             *psGpCustomDataStructure,
		 tsGP_ZgpCommissionIndication                          *psZgpCommissionIndication);
PUBLIC  teGP_GreenPowerStatus eGP_AddOrUpdateProxyTable   (
		tsZCL_EndPointDefinition    						*psEndPointDefinition,
		tsGP_GreenPowerCustomData                           *psGpCustomDataStructure,
   	 tsGP_ZgpDataIndication               		           *psZgpDataIndication
   );

bool_t bGP_GPDFValidityCheck(
		 tsGP_ZgpDataIndication                         *psZgpDataIndication,
		 tsZCL_EndPointDefinition                      *psEndPointDefinition,
		 tsZCL_ClusterInstance                         *psClusterInstance,
		 teGP_GreenPowerBufferedCommands               *pBufferedCmdId);

bool_t bGP_ValidateCommandType(
		uint8                                          u8Status,
		teGP_GreenPowerBufferedCommands               *pBufferedCmdId,
		bool_t                                         bAutoCommissioning,
		uint8                                          u8ZgpdCmdId,
		tsGP_GreenPowerCustomData                      *psGpCustomDataStructure);
bool_t bGP_GiveCommIndAndUpdateTable(
		 tsGP_ZgpDataIndication               		            *psZgpDataIndication,
		 tsZCL_EndPointDefinition    							*psEndPointDefinition,
		 tsGP_GreenPowerCustomData                              *psGpCustomDataStructure);


PUBLIC  bool_t bGP_AddOrUpdateProxySinkTableEntries   (
		tsZCL_EndPointDefinition    						*psEndPointDefinition,
		tsGP_GreenPowerCustomData                           *psGpCustomDataStructure,
   	    tsGP_ZgpDataIndication               		        *psZgpDataIndication,
   	    tsGP_ZgpCommissionIndication                        *psZgpCommissionIndication
   );
uint16 u16GetSinkTableString(
		uint8                                                *pu8Data,
		tsGP_ZgppProxySinkTable                              *psZgppProxySinkTable);
PUBLIC teZCL_Status eGP_SinkTableRequestReceive(
		ZPS_tsAfEvent                               *pZPSevent,
		uint16                                      u16Offset,
		tsGP_ZgpSinkTableRequestCmdPayload          *psZgpSinkTableRequestCmdPayload);

PUBLIC teZCL_Status eGP_SinkTableResponseReceive(
                    ZPS_tsAfEvent                               *pZPSevent,
                    uint16										u16Offset,
                    tsGP_SinkTableRespCmdPayload                *psZgpSinkTableRespCmdPayload);
PUBLIC teZCL_Status eGP_SinkTableResponseSend(
                        uint8                               u8SourceEndPointId,
                        uint8                               u8DestEndPointId,
                        tsZCL_Address                       *psDestinationAddress,
                        tsGP_SinkTableRespCmdPayload  *psZgpSinkTableRespCmdPayload);

bool_t bGP_CheckGPDAddressMatch(
		uint8 						u8AppIdSrc,
		uint8                       u8AppIdDst,
		tuGP_ZgpdDeviceAddr          *sAddrSrc,
		tuGP_ZgpdDeviceAddr          *sAddrDst
		);

PUBLIC teZCL_Status eGP_HandleSinkTableResponse(
                    ZPS_tsAfEvent                  *pZPSevent,
                    tsZCL_EndPointDefinition       *psEndPointDefinition,
                    tsZCL_ClusterInstance          *psClusterInstance,
                    uint16										u16Offset);

PUBLIC teZCL_Status  eGP_HandleSinkTableRequest(
		ZPS_tsAfEvent                  *pZPSevent,
        tsZCL_EndPointDefinition       *psEndPointDefinition,
        tsZCL_ClusterInstance          *psClusterInstance,
        uint16                         u16Offset);
uint16 u16GetProxyTableString(
		uint8                       *pu8Data,
		tsGP_ZgppProxySinkTable     *psZgppProxySinkTable);

uint16 u16GetProxyTableEntrySize( tsGP_ZgppProxySinkTable  *psZgppProxySinkTable);
PUBLIC teZCL_Status  eGP_HandleProxyTableRequest(
		ZPS_tsAfEvent                  *pZPSevent,
        tsZCL_EndPointDefinition       *psEndPointDefinition,
        tsZCL_ClusterInstance          *psClusterInstance,
        uint16                         u16Offset);
PUBLIC teZCL_Status eGP_HandleProxyTableResponse(
                    ZPS_tsAfEvent                  *pZPSevent,
                    tsZCL_EndPointDefinition       *psEndPointDefinition,
                    tsZCL_ClusterInstance          *psClusterInstance,
                    uint16							u16Offset);
PUBLIC teZCL_Status eGP_ProxyTableRequestReceive(
                    ZPS_tsAfEvent                               *pZPSevent,
                    uint16                                      u16Offset,
                    tsGP_ZgpProxyTableRequestCmdPayload          *psZgpProxyTableRequestCmdPayload);
PUBLIC teZCL_Status eGP_ProxyTableResponseSend(
                        uint8                               u8SourceEndPointId,
                        uint8                               u8DestEndPointId,
                        tsZCL_Address                       *psDestinationAddress,
                        tsGP_ProxyTableRespCmdPayload  *psZgpProxyTableRespCmdPayload);
PUBLIC teZCL_Status eGP_ProxyTableResponseReceive(
                    ZPS_tsAfEvent                               *pZPSevent,
                    uint16										u16Offset,
                    tsGP_ProxyTableRespCmdPayload                *psZgpProxyTableRespCmdPayload);
bool_t bGP_GetSecurityDetails(
		 uint8									      u8AppId,
		 uint8										  *pu8SecLevel,
		 uint8										  *pu8KeyType,
		 AESSW_Block_u                                *pu8Key,
		 tuGP_ZgpdDeviceAddr                          *puZgpdDeviceAddr,
		 tsZCL_EndPointDefinition                     *psEndPointDefinition,
		 tsGP_GreenPowerCustomData                    *psGpCustomDataStructure);

void vGP_SendDeviceAnnounce(uint16 u16AliasShortAddr, uint64 u64IeeeAddr);
bool_t  bGP_GetGPDKey(
		uint8                                u8AppID,
		bool_t                                 bIsTableEntryPresent,
		tuGP_ZgpdDeviceAddr                  *puZgpdDeviceAddr,
		tsGP_ZgppProxySinkTable              *psZgppProxySinkTable,
		teGP_GreenPowerSecKeyType            eZgpSecKeyType,
		AESSW_Block_u                        *puSecurityKey,
		tsZCL_EndPointDefinition             *psEndPointDefinition,
	    tsGP_GreenPowerCustomData            *psGpCustomDataStructure);

uint8 u8GetGPDFKeyMAP(uint8 u8EndPoint, uint8 u8GPDFKeyType);

bool_t bGP_ValidateGPDF_Fields(
		tsGP_ZgpDataIndication                         *psZgpDataIndication,
		tsGP_GreenPowerCustomData                      *psGpCustomDataStructure);
PUBLIC teZCL_Status eGP_ZgpResponseSend(
                    uint8                                  u8SourceEndPointId,
                    uint8                                  u8DestinationEndPointId,
                    tsZCL_Address                          *psDestinationAddress,
                    uint8                                  *pu8TransactionSequenceNumber,
                    tsGP_ZgpResponseCmdPayload             *psZgpResponseCmdPayload);
bool_t bGP_CheckGroupTable(uint16 u16GrpId );
bool_t bIsInvolveTC(uint8 u8EndPointNumber);
void vSendToTxQueue(		tsGP_ZgpBufferedApduRecord                  *psZgpBufferedApduRecord,
		tsGP_GreenPowerCustomData           *psGreenPowerCustomData);
void vBackUpTxQueueMsg(teGP_GreenPowerBufferedCommands            eGreenPowerBufferedCommand,
					tsGP_ZgpResponseCmdPayload             *psZgpResponseCmdPayload,
					ZPS_tsAfZgpGreenPowerReq                   *psZgpDataReq,
					tsGP_GreenPowerCustomData                   *psGpCustomDataStructure);
PUBLIC ZPS_teAfSecActions bGP_CheckFrameToBeProcessed(void);
void vGP_RemoveTemperorySinkTableEntries(uint8 u8EndpointID);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* GP_INTERNAL_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
