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
 * COMPONENT:          GreenPower.h
 *
 * DESCRIPTION:        The API for the Green Power Cluster
 *
 *****************************************************************************/

#ifndef  GREENPOWER_H_INCLUDED
#define  GREENPOWER_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"
#include "dlist.h"
#include "zcl.h"
#include "zcl_options.h"
#include "zcl_customcommand.h"
#include "aessw_ccm.h"
/****************************************************************************/
/***        Macro Definitions that may be overridden in zcl_options.h     ***/
/****************************************************************************/
#ifdef CLD_GREENPOWER
#if !defined(GP_COMBO_BASIC_DEVICE) && !defined(GP_PROXY_BASIC_DEVICE)
#error Either GP_COMBO_BASIC_DEVICE or GP_PROXY_BASIC_DEVICE should be defined
#endif

#if defined(GP_COMBO_BASIC_DEVICE) && defined(GP_PROXY_BASIC_DEVICE )
#error Either GP_COMBO_BASIC_DEVICE or GP_PROXY_BASIC_DEVICE should be defined not both
#endif
#endif


#ifdef GP_COMBO_BASIC_DEVICE

#ifndef GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES
#define GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES                                (5)
#endif

#endif


#ifndef GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES
#define GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES                                 (5)
#endif


#ifndef GP_MAX_SINK_GROUP_LIST
#define GP_MAX_SINK_GROUP_LIST                                          (2)
#endif

#ifndef GP_MAX_DUPLICATE_TABLE_ENTIRES
#define GP_MAX_DUPLICATE_TABLE_ENTIRES                                  (5)
#endif

#ifndef GP_COMMISSION_WINDOW_DURATION
#define GP_COMMISSION_WINDOW_DURATION                                   (180)
#endif

#ifndef GP_MAX_ZB_CMD_PAYLOAD_LENGTH
#define GP_MAX_ZB_CMD_PAYLOAD_LENGTH                                    (65)
#endif

#ifndef GP_MAX_ZB_CMD_PAYLOAD_LENGTH_APP_ID_2
#define GP_MAX_ZB_CMD_PAYLOAD_LENGTH_APP_ID_2                           (60)
#endif


#ifndef GP_MAX_ZB_COMM_CMD_PAYLOAD_LENGTH
#define GP_MAX_ZB_COMM_CMD_PAYLOAD_LENGTH                               (65)
#endif
#ifndef GP_MAX_TRANS_ZB_CMD_PAYLOAD_LENGTH
#define GP_MAX_TRANS_ZB_CMD_PAYLOAD_LENGTH                              GP_MAX_ZB_COMM_CMD_PAYLOAD_LENGTH
#endif

#ifndef GP_MAX_ZB_COMM_CMD_PAYLOAD_LENGTH_APP_ID_2
#define GP_MAX_ZB_COMM_CMD_PAYLOAD_LENGTH_APP_ID_2                      (60)
#endif
#ifndef GP_MAX_UNICAST_SINK
#define GP_MAX_UNICAST_SINK                      						(2)
#endif

#ifndef GP_ZGP_DUPLICATE_TIMEOUT
#define GP_ZGP_DUPLICATE_TIMEOUT                                        (2)
#endif

#ifndef GP_GROUPCAST_RADIUS
#define GP_GROUPCAST_RADIUS                                             (0x0F)
#endif

#ifndef GP_MAX_PAIRED_ENDPOINTS
#define GP_MAX_PAIRED_ENDPOINTS                                         (5)
#endif

#ifndef GP_MAX_TRANSLATION_RESPONSE_ENTRY
#define GP_MAX_TRANSLATION_RESPONSE_ENTRY                               (10)
#endif

#ifndef MAX_SINK_TABLE_ENTRIES_LENGTH
#define MAX_SINK_TABLE_ENTRIES_LENGTH									(70)
#endif
#ifndef MAX_PROXY_TABLE_ENTRIES_LENGTH
#define MAX_PROXY_TABLE_ENTRIES_LENGTH									(70)
#endif

#ifndef GP_MAX_TRANSLATION_UPDATE_ENTRY
#define GP_MAX_TRANSLATION_UPDATE_ENTRY                                  7
#endif

#ifndef GP_COMM_MAX_COUNT_COMMAND_ID
#define GP_COMM_MAX_COUNT_COMMAND_ID                                     4
#endif

#ifndef GP_COMM_MAX_COUNT_CLUSTER
#define GP_COMM_MAX_COUNT_CLUSTER      									 4
#endif


#define GREENPOWER_CLUSTER_ID                                           (0x0021)

#define GREENPOWER_PROFILE_ID                                           (0xA1E0)

#define GP_MAX_NUMBER_BUFFERED_RECORDS                                  (0x04)
/* ZGP Client features */
#define GP_FEATURE_ZGPP_FEATURE                                          (1 << 0)   //ZGP Feature
#define GP_FEATURE_ZGPP_DIRECT_COMM                                      (1 << 1)   //Direct communication (reception of GPDF via ZGP stub)
#define GP_FEATURE_ZGPP_DERIVED_GC_COMM                                      (1 << 2)   // Derived groupcast communication
#define GP_FEATURE_ZGPP_COMMISSION_GC_COMM                                   (1 << 3)   //Pre-commissioned groupcast communication
#define GP_FEATURE_ZGPP_FULL_UNICAST_COMM                                    (1 << 4)   //Unicast communication
#define GP_FEATURE_ZGPP_LIGHTWEIGHT_UNICAST_COMM                             (1 << 5)   //Lightweight unicast communication
#define GP_FEATURE_ZGPP_BIDIRECTIONAL_OPERN                                  (1 << 7)   //Single-hop (in sink’s range) bidirectional operation
#define GP_FEATURE_ZGPP_PROXY_TABLE_MAINTENANCE                              (1 << 8)   //Proxy Table maintenance (active and passive, for ZGPD mobility and ZGPP robustness)
#define GP_FEATURE_ZGPP_GP_COMMISSIONING                                     (1 << 10)   //Single-hop (in sink’s range) commissioning (unidirectional and bidirectional)
#define GP_FEATURE_ZGPP_CT_BASED_CMSNG                                       (1 << 11)  //CT-based commissioning
#define GP_FEATURE_ZGPP_MAINTENANCE_OF_ZGPD                                  (1 << 12)  //Maintenance of ZGPD (deliver channel/key during operation)
#define GP_FEATURE_ZGPP_ZGPD_SEC_LVL_0B00                                    (1 << 13)  //zgpdSecurityLevel = 0b00
#define GP_FEATURE_ZGPP_ZGPD_SEC_LVL_0B10                                    (1 << 15)  //zgpdSecurityLevel = 0b10
#define GP_FEATURE_ZGPP_ZGPD_SEC_LVL_0B11                                    (1 << 16)  //zgpdSecurityLevel = 0b11
#define GP_FEATURE_ZGPP_ZGPD_IEEE_ADDR                                       (1 << 19)  //ZGPD IEEE address

/* ZGP Server features */
#define GP_FEATURE_ZGPS_FEATURE                                              (1 << 0)   //ZGP Feature
#define GP_FEATURE_ZGPS_DIRECT_COMM                                          (1 << 1)   //Direct communication (reception of GPDF via ZGP stub)
#define GP_FEATURE_ZGPS_DERIVED_GC_COMM                                      (1 << 2)   // Derived groupcast communication
#define GP_FEATURE_ZGPS_COMMISSION_GC_COMM                                   (1 << 3)   //Pre-commissioned groupcast communication
#define GP_FEATURE_ZGPS_UNICAST_COMM                                         (1 << 4)   //Unicast communication
#define GP_FEATURE_ZGPS_LIGHTWEIGHT_UNICAST_COMM                             (1 << 5)   //Lightweight unicast communication
#define GP_FEATURE_ZGPS_PROXIMITY_BIDIRECTIONAL_OPERN                        (1 << 6)   //Single-hop (in sink’s range) bidirectional operation
#define GP_FEATURE_ZGPS_MULTI_HOP_BIDIRECTIONAL                              (1 << 7)   //Multi-hop (Proxy-based) bidirectional operation
#define GP_FEATURE_ZGPS_PROXY_TABLE_MAINTENANCE                              (1 << 8)   //Proxy Table maintenance (active and passive, for ZGPD mobility and ZGPP robustness)
#define GP_FEATURE_ZGPS_PROXIMITY_CMSNG                                      (1 << 9)   //Single-hop (in sink’s range) commissioning (unidirectional and bidirectional)
#define GP_FEATURE_ZGPS_MULTI_HOP_CMSNG                                      (1 << 10)  //Multi-hop (Proxy-based) commissioning (unidirectional and bidirectional)
#define GP_FEATURE_ZGPS_CT_BASED_CMSNG                                       (1 << 11)  //CT-based commissioning
#define GP_FEATURE_ZGPS_MAINTENANCE_OF_ZGPD                                  (1 << 12)  //Maintenance of ZGPD (deliver channel/key during operation)
#define GP_FEATURE_ZGPS_ZGPD_SEC_LVL_0B00                                    (1 << 13)  //zgpdSecurityLevel = 0b00
#define GP_FEATURE_ZGPS_ZGPD_SEC_LVL_0B10                                    (1 << 15)  //zgpdSecurityLevel = 0b10
#define GP_FEATURE_ZGPS_ZGPD_SEC_LVL_0B11                                    (1 << 16)  //zgpdSecurityLevel = 0b11
#define GP_FEATURE_ZGPS_SINK_TABLE_BASED_GC_FORWARDING                       (1 << 17)  //Sink Table-based groupcast forwarding
#define GP_FEATURE_ZGPS_TRANSLATION_TABLE                                    (1 << 18)  //Translation Table
#define GP_FEATURE_ZGPS_ZGPD_IEEE_ADDR                                       (1 << 19)  //ZGPD IEEE address


/* Commissioning Exit mode */
#define GP_CMSNG_EXIT_MODE_ON_WINDOW_EXPIRE_MASK                        (0x01)            // On Commissioning Window expiration
#define GP_CMSNG_EXIT_MODE_ON_PAIRING_SUCCESS_MASK                      (0x02)            // On first Pairing success
#define GP_CMSNG_EXIT_MODE_ON_PROXY_MODE_MASK                           (0x04)            // On ZGP Proxy Commissioning Mode (exit)

/* Attribute IDs for Green Power Cluster */
typedef enum 
{
    /* Server Attribute IDs */
    E_CLD_GP_ATTR_ZGPS_MAX_SINK_TABLE_ENTRIES = 0x0000,
    E_CLD_GP_ATTR_ZGPS_SINK_TABLE,
    E_CLD_GP_ATTR_ZGPS_COMMUNICATION_MODE,
    E_CLD_GP_ATTR_ZGPS_COMMISSIONING_EXIT_MODE,
    E_CLD_GP_ATTR_ZGPS_COMMISSIONING_WINDOW,
    E_CLD_GP_ATTR_ZGPS_SECURITY_LEVEL,
    E_CLD_GP_ATTR_ZGPS_FUNCTIONALITY,
    E_CLD_GP_ATTR_ZGPS_ACTIVE_FUNCTIONALITY,

    /* Client Attribute IDs */
    E_CLD_GP_ATTR_ZGPP_MAX_PROXY_TABLE_ENTRIES = 0x0010,
    E_CLD_GP_ATTR_ZGPP_PROXY_TABLE,
    E_CLD_GP_ATTR_ZGPP_NOTIFICATION_RETRY_NUMBER,
    E_CLD_GP_ATTR_ZGPP_NOTIFICATION_RETRY_TIMER,
    E_CLD_GP_ATTR_ZGPP_MAX_SEARCH_COUNTER,
    E_CLD_GP_ATTR_ZGPP_BLOCKED_ZGPD_ID,
    E_CLD_GP_ATTR_ZGPP_FUNCTIONALITY,
    E_CLD_GP_ATTR_ZGPP_ACTIVE_FUNCTIONALITY,

    /* Shared Attributes b/w server and client */
    E_CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE = 0x0020,
    E_CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY,
    E_CLD_GP_ATTR_ZGP_LINK_KEY
}teGP_GreenPowerClusterAttrIds;

/* Cluster Command Ids */
typedef enum 
{
    /* Server Received Commands and Client Generated Commands */
    E_GP_ZGP_NOTIFICATION = 0x00,
    E_GP_ZGP_PAIRING_SEARCH,
    E_GP_ZGP_TUNNELING_STOP = 0x03,
    E_GP_ZGP_COMMOSSIONING_NOTIFICATION,
    E_GP_ZGP_TRANSLATION_TABLE_UPDATE = 0x07,
    E_GP_ZGP_TRANSLATION_TABLE_REQUEST,
    E_GP_ZGP_PAIRING_CONFIGURATION,
    E_GP_ZGP_SINK_TABLE_REQUEST,
    E_GP_ZGP_PROXY_TABLE_RESPONSE,
    /* Server Generated Commands  and Client Received Commands */
    E_GP_ZGP_NOTIFICATION_RESPONSE = 0x00,
    E_GP_ZGP_PAIRING,
    E_GP_ZGP_PROXY_COMMOSSIONING_MODE,
    E_GP_ZGP_RESPONSE = 0x06,
    E_GP_ZGP_TRANSLATION_TABLE_RESPONSE = 0x08,
    E_GP_ZGP_SINK_TABLE_RESPONSE  = 0x0a,
    E_GP_ZGP_PROXY_TABLE_REQUEST,

}teGP_GreenPowerClusterCmdIds;

/* ZGPS Communication Modes */
typedef enum 
{
    E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_BOTH = 0x00,
    E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID,
    E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID,
    E_GP_UNI_FORWARD_ZGP_NOTIFICATION_BY_PROXIES_LIGHTWEIGHT
}teGP_GreenPowerCommunicationMode;

/* GPD Key Types */
typedef enum
{
    E_GPD_SHARED_KEY= 0x00,
    E_GPD_INDIVIDUAL_KEY,
}teGP_GreenPowerGPDKeyType;

/* ZGP Security Status */
typedef enum 
{
    E_GP_SEC_SUCCESS = 0x00,
    E_GP_SEC_NO_SECURITY = ZPS_E_APL_AF_GP_NO_SECURITY,
    E_GP_SEC_COUNTER_FAILURE,
    E_GP_SEC_AUTH_FAILURE,
    E_GP_SEC_UPROCESSED
}teGP_GreenPowerSecStatus;

/* ZGP Security Levels */
typedef enum 
{
    E_GP_NO_SECURITY = 0x00,
    E_GP_FULL_FC_FULL_MIC = 0x02,
    E_GP_ENC_FULL_FC_FULL_MIC
}teGP_GreenPowerSecLevel;

/* ZGP Security Key Type */
typedef enum 
{
    E_GP_NO_KEY = 0x00,
    E_GP_ZIGBEE_NWK_KEY,
    E_GP_ZGPD_GROUP_KEY,
    E_GP_NWK_KEY_DERIVED_ZGPD_GROUP_KEY,
    E_GP_OUT_OF_THE_BOX_ZGPD_KEY,
    E_GP_DERIVED_INDIVIDUAL_ZGPD_KEY = 0x07
}teGP_GreenPowerSecKeyType;

/* ZGPD Device Ids */
typedef enum 
{
    E_GP_ZGP_SIMPLE_GENERIC_ONE_STATE_SWITCH = 0x00,
    E_GP_ZGP_SIMPLE_GENERIC_TWO_STATE_SWITCH,
    E_GP_ZGP_ON_OFF_SWITCH,
    E_GP_ZGP_LEVEL_CONTROL_SWITCH,
    E_GP_ZGP_SIMPLE_SENSOR,
    E_GP_ZGP_ADVANCED_GENERIC_ONE_STATE_SWITCH,
    E_GP_ZGP_ADVANCED_GENERIC_TWO_STATE_SWITCH
    /* Remaining Device Ids will be updated when required */
}teGP_ZgpdDeviceId;

/* ZGPD Command Ids */
typedef enum 
{
    E_GP_IDENTIFY = 0x00,
    E_GP_OFF = 0x20,
    E_GP_ON,
    E_GP_TOGGLE,
    E_GP_MOVE_UP  = 0x30,
    E_GP_MOVE_DOWN,
    E_GP_STEP_UP,
    E_GP_STEP_DOWN,
    E_GP_LEVEL_CONTROL_STOP = 0x34,
    E_GP_MOVE_UP_WITH_ON_OFF,
    E_GP_MOVE_DOWN_WITH_ON_OFF,
    E_GP_STEP_UP_WITH_ON_OFF,
    E_GP_STEP_DOWN_WITH_ON_OFF,
    E_GP_COMMISSIONING = 0xE0,
    E_GP_DECOMMISSIONING,
    E_GP_SUCCESS,
    E_GP_CHANNEL_REQUEST,
    E_GP_COMMISSIONING_RANGE_END = 0xEF,
    E_GP_COMMISSIONING_REPLY = 0xF0,
    E_GP_CHANNEL_CONFIGURATION = 0xF3,
    E_GP_ZGPD_CMD_ID_ENUM_END
}teGP_ZgpdCommandId;

/* ZGPD Commission Command Type */
typedef enum 
{
    E_GP_COMM_CMD = 0x00,           /* Direct Commission GPDF */
    E_GP_DATA_CMD_AUTO_COMM,        /* Direct Data GPDF with Auto-comm flag to 1*/
}teGP_CommandType;

/* Green Power Device Types */
typedef enum
{
    E_GP_ZGP_PROXY_BASIC_DEVICE   = 0x00,
    E_GP_ZGP_PROXY_DEVICE,
    E_GP_ZGP_TARGET_PLUS_DEVICE,
    E_GP_ZGP_TARGET_DEVICE,
    E_GP_ZGP_COMM_TOOL_DEVICE,
    E_GP_ZGP_COMBO_DEVICE,
    E_GP_ZGP_COMBO_BASIC_DEVICE
}teGP_GreenPowerDeviceType;

/* Green Power Modes */
typedef enum
{
    E_GP_OPERATING_MODE = 0x00,
    E_GP_PAIRING_COMMISSION_MODE,
    E_GP_REMOTE_COMMISSION_MODE,
}teGP_GreenPowerDeviceMode;

/* Proxy Commission Modes */
typedef enum 
{
    E_GP_PROXY_COMMISSION_EXIT = 0x00,
    E_GP_PROXY_COMMISSION_ENTER,
}teGP_GreenPowerProxyCommissionMode;

/* Green Power Status Values */
typedef enum
{
    E_GP_TRANSLATION_UPDATE_SUCCESS ,
    E_GP_TRANSLATION_UPDATE_FAIL,
}teGP_GreenPowerStatus;

/* Green Power Event Types */
typedef enum 
{
    E_GP_COMMISSION_DATA_INDICATION = 0x00,
    E_GP_COMMISSION_MODE_ENTER, //1
    E_GP_COMMISSION_MODE_EXIT, //2
    E_GP_CMD_UNSUPPORTED_PAYLOAD_LENGTH,//3
    E_GP_SINK_PROXY_TABLE_ENTRY_ADDED, //4
    E_GP_SINK_PROXY_TABLE_FULL, //5
    E_GP_ZGPD_COMMAND_RCVD, //6
    E_GP_ZGPD_CMD_RCVD_WO_TRANS_ENTRY, //7
    E_GP_ADDING_GROUP_TABLE_FAIL, //8
    E_GP_RECEIVED_CHANNEL_REQUEST, //9
    E_GP_TRANSLATION_TABLE_RESPONSE_RCVD, //10
    E_GP_TRANSLATION_TABLE_UPDATE, //11
    E_GP_SECURITY_LEVEL_MISMATCH, //12
    E_GP_SECURITY_PROCESSING_FAILED, //13
    E_GP_REMOVING_GROUP_TABLE_FAIL, //14
    E_GP_PAIRING_CONFIGURATION_CMD_RCVD, //15
    E_GP_PERSIST_SINK_PROXY_TABLE, //16
    E_GP_SUCCESS_CMD_RCVD,//17
    E_GP_DECOMM_CMD_RCVD,//18
    E_GP_SHARED_SECURITY_KEY_TYPE_IS_NOT_ENABLED,//19
    E_GP_SHARED_SECURITY_KEY_IS_NOT_ENABLED,//20
    E_GP_LINK_KEY_IS_NOT_ENABLED,//21
    E_GP_ZGPD_SINK_TABLE_RESPONSE_RCVD,//22
    E_GP_ZGPD_PROXY_TABLE_RESPONSE_RCVD,//23
    E_GP_NOTIFICATION_RCVD,//24
    E_GP_COMM_NOTIFICATION_RCVD,//25
    E_GP_RESPONSE_RCVD,//26
    E_GP_PAIRING_CMD_RCVD,//27
    E_GP_PAIRING_CONFIG_CMD_RCVD,
    E_GP_ADDED_TX_QUEUE,
    E_GP_UPDATE_SINK_TABLE_GROUP_INFO,
    E_GP_CBET_ENUM_END
}teGP_GreenPowerCallBackEventType;

/* Green Power Sink Table Priorities */
typedef enum 
{
    E_GP_SINK_TABLE_P_1 = 0x01,         /* Entry found in translation table */
    E_GP_SINK_TABLE_P_2,                /* Entry not found in translation table but direct received cmd */
    E_GP_SINK_TABLE_P_3                 /* Entry not found in translation table but tunnel received cmd */
}teGP_GreenPowerSinkTablePriority;



/* action subfield of GP pairing config command */
typedef enum 
{
    E_GP_PAIRING_CONFIG_NO_ACTION,
    E_GP_PAIRING_CONFIG_EXTEND_SINK_TABLE_ENTRY,
    E_GP_PAIRING_CONFIG_REPLACE_SINK_TABLE_ENTRY,
    E_GP_PAIRING_CONFIG_REMOVE_SINK_TABLE_ENTRY,
    E_GP_PAIRING_CONFIG_REMOVE_GPD
}teGP_GreenPowerPairingConfigAction;

typedef enum 
{
    E_GP_PAIRING_CONFIG_TRANSLATION_TABLE_ADD_ENTRY = 0x00,
    E_GP_PAIRING_CONFIG_TRANSLATION_TABLE_REMOVE_ENTRY,
    E_GP_PAIRING_CONFIG_TRANSLATION_TABLE_EXTEND_ENTRY,
    E_GP_PAIRING_CONFIG_TRANSLATION_TABLE_NO_ACTION
} teGP_PairingConfigTranslationTableAction;

/* action subfield of GP Translation update command */
typedef enum 
{
    E_GP_TRANSLATION_TABLE_ADD_ENTRY = 0x00,
    E_GP_TRANSLATION_TABLE_REPLACE_ENTRY,
    E_GP_TRANSLATION_TABLE_REMOVE_ENTRY
} teGP_TranslationTableUpdateAction;


typedef enum
{
	E_GP_DEFAULT_ATTRIBUTE_VALUE			=  0x01,
	E_GP_DEFAULT_PROXY_SINK_TABLE_VALUE			=  0x02,
}teGP_ResetToDefaultConfig;

// Pre-Definition of sink and translation table structures

typedef struct tsGP_TranslationTableEntry       tsGP_TranslationTableEntry;
typedef struct tsGP_GpToZclCommandInfo          tsGP_GpToZclCommandInfo;
/* ZGPD Device Id */
typedef struct
{
	 uint8   u8EndPoint;
	 uint64  u64ZgpdIEEEAddr;        /* 8 Bytes IEEE Addr */
}tsGP_ZgpdDeviceAddrAppId2;
typedef union
{
    uint32  u32ZgpdSrcId;           /* 4 Bytes Source Id */
    tsGP_ZgpdDeviceAddrAppId2 sZgpdDeviceAddrAppId2;
}tuGP_ZgpdDeviceAddr;

typedef struct
{
#ifdef GP_COMBO_BASIC_DEVICE

    /* Server Attributes */
    uint8                   u8ZgpsMaxSinkTableEntries;
    tsZCL_LongOctetString   sSinkTable;
    zbmap8                  b8ZgpsCommunicationMode;
    zbmap8                  b8ZgpsCommissioningExitMode;

#ifdef  CLD_GP_ATTR_ZGPS_COMMISSIONING_WINDOW
    uint16                  u16ZgpsCommissioningWindow;
#endif

    zbmap8                  b8ZgpsSecLevel;
    zbmap24                 b24ZgpsFunctionality;
    zbmap24                 b24ZgpsActiveFunctionality;
#endif


    /* Client Attributes */
    uint8                   u8ZgppMaxProxyTableEntries;
    tsZCL_LongOctetString   sProxyTable;
#ifdef GP_PROXY_BASIC_DEVICE
#ifdef  CLD_GP_ATTR_ZGPP_NOTIFICATION_RETRY_NUMBER
    uint8                   u8ZgppNotificationRetryNumber;
#endif

#ifdef  CLD_GP_ATTR_ZGPP_NOTIFICATION_RETRY_TIMER
    uint8                   u8ZgppNotificationRetryTimer;
#endif

#ifdef  CLD_GP_ATTR_ZGPP_MAX_SEARCH_COUNTER
    uint8                   u8ZgppMaxSearchCounter;
#endif

#ifdef  CLD_GP_ATTR_ZGPP_BLOCKED_GPD_ID
    tsZCL_LongOctetString   sZgppBlockedGpdID;
#endif

    zbmap24                 b24ZgppFunctionality;
    zbmap24                 b24ZgppActiveFunctionality;
#endif

    /* Shared Attributes b/w server and client */

#ifdef  CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE
    zbmap8                  b8ZgpSharedSecKeyType;
#endif

#ifdef  CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY
    tsZCL_Key               sZgpSharedSecKey;
#endif

#ifdef  CLD_GP_ATTR_ZGP_LINK_KEY
    tsZCL_Key               sZgpLinkKey;
#endif
    zbmap32       u32FeatureMap;
    
    uint16        u16ClusterRevision;
}tsCLD_GreenPower;


/* Structure Sink Group List */
typedef struct
{
    uint16                  u16SinkGroup;
    uint16                  u16Alias;
}tsGP_ZgpsGroupList;

/* Structure for ZGP Notification Command */
typedef struct
{
    teGP_ZgpdCommandId      eZgpdCmdId;
    uint8                   u8GPP_GPD_Link;
    uint16                  u16ZgppShortAddr;
    zbmap16                 b16Options;
    uint32                  u32ZgpdSecFrameCounter;
    tuGP_ZgpdDeviceAddr     uZgpdDeviceAddr;
    tsZCL_OctetString       sZgpdCommandPayload;
}tsGP_ZgpNotificationCmdPayload;

/* Structure for Sink Table Request  Command */
typedef struct
{
	zbmap8			      	b8Options;
	tuGP_ZgpdDeviceAddr     uZgpdDeviceAddr;
    uint8					u8Index;
}tsGP_ZgpSinkTableRequestCmdPayload;

/* Structure for Proxy Table Request  Command */
typedef struct
{
	zbmap8			      	b8Options;
	tuGP_ZgpdDeviceAddr     uZgpdDeviceAddr;
    uint8					u8Index;
}tsGP_ZgpProxyTableRequestCmdPayload;


/* Structure for ZGP Commissioning Notification Command */
typedef struct
{
    uint8                   u8ZgpdCmdId;
    uint8                   u8GPP_GPD_Link;
    uint16                  u16ZgppShortAddr;
    zbmap16                 b16Options;
    uint32                  u32ZgpdSecFrameCounter;
    uint32                  u32Mic;
    tuGP_ZgpdDeviceAddr     uZgpdDeviceAddr;
    tsZCL_OctetString       sZgpdCommandPayload;
}tsGP_ZgpCommissioningNotificationCmdPayload;

/* Structure for ZGP Proxy Commissioning Mode Command */
typedef struct
{
    zbmap8                  b8Options;
    uint16                  u16CommissioningWindow;
    uint8                   u8Channel;
}tsGP_ZgpProxyCommissioningModeCmdPayload;

/* Structure for ZGP Response Command */
typedef struct
{
    zbmap8                  b8Options;
    zbmap8                  b8TempMasterTxChannel;
    teGP_ZgpdCommandId      eZgpdCmdId;
    uint16                  u16TempMasterShortAddr;
    tuGP_ZgpdDeviceAddr     uZgpdDeviceAddr;
    tsZCL_OctetString       sZgpdCommandPayload;
}tsGP_ZgpResponseCmdPayload;

/* Structure for GPDF data cmd payload */
typedef struct
{
    teGP_ZgpdCommandId      eZgpdCmdId;
    uint8                   u8ZgpdCmdPayloadLength;
    uint8                   *pu8ZgpdCmdPayload;
}tsGP_ZgpDataCmdWithAutoCommPayload;


/* Structure for GPDF commissioning cmd payload */
typedef struct
{
	uint8                   u8ApplicationInfo;
	uint8                   u8NoOfGPDCommands;
	uint8                   u8NoOfClusters;
    teGP_ZgpdDeviceId       eZgpdDeviceId;
    zbmap8                  b8Options;
    zbmap8                  b8ExtendedOptions;
    uint16                  u16ManufID;
    uint16                  u16ModelID;
    uint32                  u32ZgpdKeyMic;
    uint32                  u32ZgpdOutgoingCounter;
    uint8			        u8CommandList[GP_COMM_MAX_COUNT_COMMAND_ID];
    uint16			        u16ClusterList[GP_COMM_MAX_COUNT_CLUSTER];
    tsZCL_Key               sZgpdKey;
}tsGP_ZgpCommissionCmdPayload;

/* Structure for ZGP pairing search cmd payload */
typedef struct
{
    zbmap16                 b16Options;
    tuGP_ZgpdDeviceAddr     uZgpdDeviceAddr;
}tsGP_ZgpPairingSearchCmdPayload;

/* Structure for ZGP pairing cmd payload */
typedef struct
{
    uint8                   u8DeviceId;
    uint8                   u8ForwardingRadius;
    uint16                  u16SinkGroupID;
    uint16                  u16AssignedAlias;
    zbmap24                 b24Options;
    uint32                  u32ZgpdSecFrameCounter;
    uint16                  u16SinkNwkAddress;
    uint64                  u64SinkIEEEAddress;
    tuGP_ZgpdDeviceAddr     uZgpdDeviceAddr;
    tsZCL_Key               sZgpdKey;
}tsGP_ZgpPairingCmdPayload;

/* Structure for ZGP pairing config cmd payload */
typedef struct
{
    uint8                               u8Actions;
    teGP_ZgpdDeviceId                   eZgpdDeviceId;
    uint8                               u8SinkGroupListEntries;
    uint8                               u8ForwardingRadius;
    zbmap8                              b8SecOptions;
    uint8                               u8NumberOfPairedEndpoints;
    uint8                               au8PairedEndpoints[GP_MAX_PAIRED_ENDPOINTS];
    zbmap16                             b16Options;
    uint16                              u16ZgpdAssignedAlias;
    uint32                              u32ZgpdSecFrameCounter;
    tuGP_ZgpdDeviceAddr                 uZgpdDeviceAddr;
    tsGP_ZgpsGroupList                  asSinkGroupList[GP_MAX_SINK_GROUP_LIST];
    tsZCL_Key                           sZgpdKey;
}tsGP_ZgpPairingConfigCmdPayload;

/* Structure for ZGP pairing config cmd received callback */
typedef struct
{
    teGP_GreenPowerPairingConfigAction               eAction;
    teGP_PairingConfigTranslationTableAction         eTranslationTableAction;
    teGP_ZgpdDeviceId                                eZgpdDeviceId;
    teGP_GreenPowerCommunicationMode                 eCommunicationMode;
    uint8                                            u8ApplicationId;
    tuGP_ZgpdDeviceAddr                              uZgpdDeviceAddr;
    uint8                                            u8NumberOfPairedEndpoints;
    uint8                                            au8PairedEndpointList[GP_MAX_PAIRED_ENDPOINTS];
    uint8                                            u8SinkGroupListEntries;
    tsGP_ZgpsGroupList                               asSinkGroupList[GP_MAX_SINK_GROUP_LIST];
} tsGP_ZgpsPairingConfigCmdRcvd;


/* Commission Indication Structure */
typedef struct
{
    teGP_CommandType                eCmdType;
    teZCL_Status           			eStatus;
    tsGP_GpToZclCommandInfo         *psGpToZclCommandInfo;
    bool_t							 bIsTunneledCmd;
    zbmap8                           b8AppId;
    tuGP_ZgpdDeviceAddr              uZgpdDeviceAddr;
    union
    {
        tsGP_ZgpCommissionCmdPayload                 sZgpCommissionCmd;
        tsGP_ZgpDataCmdWithAutoCommPayload           sZgpDataCmd;
    }uCommands;
}tsGP_ZgpCommissionIndication;

typedef struct
{
    uint8                       u8ApplicationId;
    tuGP_ZgpdDeviceAddr         uZgpdDeviceAddr;
}tsGP_ZgpDecommissionIndication;

/* Structure for Translation Table Entry */
struct tsGP_GpToZclCommandInfo
{
    teGP_ZgpdDeviceId       eZgpdDeviceId;
    teGP_ZgpdCommandId      eZgpdCommandId;
    uint8                   u8ZbCommandId ;
    uint8                   u8EndpointId;
    uint16                  u16ZbClusterId ;
    uint8                   u8ZbCmdLength ;
    uint8                   au8ZbCmdPayload[GP_MAX_TRANS_ZB_CMD_PAYLOAD_LENGTH];
};

/* Structure for Translation Table */
struct tsGP_TranslationTableEntry
{
    zbmap8                      b8Options;
    uint8						u8NoOfCmdInfo;
    tsGP_GpToZclCommandInfo     *psGpToZclCmdInfo;
    tuGP_ZgpdDeviceAddr         uZgpdDeviceAddr;

};

/* structure for sink table response command */
typedef struct tsGP_SinkTableResposneCmdPayload
{
    uint8                       u8Status;
    uint8                       u8TotalNoOfEntries;
    uint8                       u8StartIndex;
    uint8                       u8EntriesCount;
    uint16						u16SizeOfSinkTableEntries;
    uint8						*puSinkTableEntries;
}tsGP_SinkTableRespCmdPayload;

/* structure for proxy table response command */
typedef struct tsGP_ProxyTableRespCmdPayload
{
    uint8                       u8Status;
    uint8                       u8TotalNoOfEntries;
    uint8                       u8StartIndex;
    uint8                       u8EntriesCount;
    uint16						u16SizeOfProxyTableEntries;
    uint8						*puProxyTableEntries;
}tsGP_ProxyTableRespCmdPayload;


/* structure for Lightweight sink address list*/
typedef struct
{
	uint16 u16SinkNWKAddress;
	uint64	u64SinkIEEEAddress;
}tsGP_ZgpsSinkAddrList;

/* Structure for Sink/Proxy Table */
typedef struct
{
#ifdef GP_COMBO_BASIC_DEVICE
	/* the entries exclusive to Sink table */
	uint8								u8GPDPaired;
    teGP_GreenPowerSinkTablePriority    eGreenPowerSinkTablePriority;
    teGP_ZgpdDeviceId                   eZgpdDeviceId;
    zbmap8                                b8SinkOptions;
#endif
    bool_t                              bProxyTableEntryOccupied;
    uint8                               u8SinkGroupListEntries;
    uint8                               u8GroupCastRadius;
    uint8                               u8SearchCounter;
    uint8								u8NoOfUnicastSink;
    zbmap8                              b8SecOptions;
    zbmap16                             b16Options;
    uint16                              u16ZgpdAssignedAlias;
    uint16 								u16SinkUnicastNWKAddress[GP_MAX_UNICAST_SINK];
    uint32                              u32ZgpdSecFrameCounter;
    uint64								u64SinkUnicastIEEEAddres[GP_MAX_UNICAST_SINK];
    tsGP_ZgpsGroupList                  asSinkGroupList[GP_MAX_SINK_GROUP_LIST];
    tsZCL_Key                           sZgpdKey;
    tuGP_ZgpdDeviceAddr                 uZgpdDeviceAddr;

  //  tsGP_ZgpsSinkAddrList               sUnicastSinkAddr[GP_MAX_UNICAST_SINK];
}tsGP_ZgppProxySinkTable;


/* structure for the ZGP command cluster table */
typedef struct {
    teGP_ZgpdCommandId                  eZgpdCommandId;
    uint16                              u16ClusterId;
}tsGP_ZgpCommandClusterTable;

/* Structure for ZGP Translation update cmd translation entry */
typedef struct
{
    uint8                   u8Index;
    teGP_ZgpdCommandId      eZgpdCommandId;
    uint8                   u8EndpointId;
    uint16                  u16ProfileID;
    uint16                  u16ZbClusterId;
    uint8                   u8ZbCommandId;
    uint8                   u8ZbCmdLength ;
    uint8                   au8ZbCmdPayload[GP_MAX_TRANS_ZB_CMD_PAYLOAD_LENGTH];
} tsGP_ZgpsTranslationUpdateEntry;


/* Structure for ZGP Translation update cmd payload */
typedef struct
{
    zbmap16                         b16Options;
    tuGP_ZgpdDeviceAddr             uZgpdDeviceAddr;
    tsGP_ZgpsTranslationUpdateEntry asTranslationUpdateEntry[GP_MAX_TRANSLATION_UPDATE_ENTRY];
}tsGP_ZgpTranslationUpdateCmdPayload;

typedef struct
{
    teGP_GreenPowerStatus                       eStatus;
    teGP_TranslationTableUpdateAction           eAction;
    uint8                                       u8ApplicationId;
    tuGP_ZgpdDeviceAddr                         uZgpdDeviceAddr;
    tsGP_ZgpsTranslationUpdateEntry             *psTranslationUpdateEntry;
} tsGP_ZgpsTranslationTableUpdate;

/* Structure for ZGP Translation response cmd translation entry */
typedef struct
{

    teGP_ZgpdCommandId      eZgpdCommandId;
    uint8                   u8ZbCommandId ;
    uint8                   u8EndpointId;
    uint16                  u16ProfileID;
    uint16                  u16ZbClusterId ;
    uint8                   u8ZbCmdLength ;
    uint8                   au8ZbCmdPayload[GP_MAX_TRANS_ZB_CMD_PAYLOAD_LENGTH];
    tuGP_ZgpdDeviceAddr     uZgpdDeviceAddr;
} tsGP_ZgpsTransTblRspEntry;

/* Structure for ZGP Translation response cmd payload */
typedef struct
{
    uint8                           u8Status;
    zbmap8                          b8Options;
    uint8                           u8TotalNumOfEntries;
    uint8                           u8StartIndex;
    uint8                           u8EntriesCount;
    tsGP_ZgpsTransTblRspEntry       asTransTblRspEntry[GP_MAX_TRANSLATION_RESPONSE_ENTRY];
}tsGP_ZgpTransTableResponseCmdPayload;

typedef struct
{
	uint8     u8Status;
	uint8     u8Handle;
	uint8     u8CmdId;
}tsGP_TxQueueMsg;

/* Structure for Green Power Call Back Message */
typedef struct
{
    teGP_GreenPowerCallBackEventType                    eEventType;

    union
    {
    	tsGP_TxQueueMsg                                 *psTxQueueMsg;
    	tsGP_ZgppProxySinkTable                         *psZgpsProxySinkTable;
        tsGP_ZgpCommissionIndication                    *psZgpCommissionIndication;
        ZPS_teStatus                                    eAddGroupTableStatus;
        ZPS_teStatus                                    eRemoveGroupTableStatus;
        bool_t                                          bIsActAsTempMaster;
        tsGP_ZgpTransTableResponseCmdPayload            *psZgpTransRspCmdPayload;
        tsGP_ZgpsTranslationTableUpdate                 *psTransationTableUpdate;
        tsGP_ZgpsPairingConfigCmdRcvd                   *psPairingConfigCmdRcvd;
        tsGP_ZgpDecommissionIndication                  *psZgpDecommissionIndication;
        tsGP_SinkTableRespCmdPayload                    *psZgpSinkTableRespCmdPayload;
        tsGP_ProxyTableRespCmdPayload                    *psZgpProxyTableRespCmdPayload;
        tsGP_ZgpResponseCmdPayload                       *psZgpResponseCmdPayload;
        tsGP_ZgpNotificationCmdPayload                   *psZgpNotificationCmdPayload ;
        tsGP_ZgpCommissioningNotificationCmdPayload      *psZgpCommissioningNotificationCmdPayload;
        tsGP_ZgpPairingCmdPayload                        *psZgpPairingCmdPayload;
        tsGP_ZgpPairingConfigCmdPayload                  *psZgpPairingConfigCmdPayload;
    }uMessage;
}tsGP_GreenPowerCallBackMessage;

/* Structure for Green Power Common */
typedef struct
{
    tsZCL_ReceiveEventAddress           sReceiveEventAddress;
    tsZCL_CallBackEvent                 sGPCustomCallBackEvent;
    tsGP_GreenPowerCallBackMessage      sGreenPowerCallBackMessage;
}tsGP_Common;

/* Structure for GP Custom Data */
/* Green Power Buffered Command Types  */
typedef enum
{
    E_GP_COMMISSION_NOTIFICATION = 0x00,
    E_GP_DEVICE_ANNOUNCE,
    E_GP_LOOP_BACK,
    E_GP_RESPONSE,
    E_GP_ADD_PAIRING,
    E_GP_PROXY_COMMISION,
    E_GP_COMMISSIONING_CMDS,
    E_GP_DATA,
    E_GP_NOTIFICATION,
    E_GP_REMOVE_PAIRING,
    E_GP_RESPONSE_AFTER_DELAY,
    E_GP_ADD_MSG_TO_TX_QUEUE,
    E_GP_ADD_MSG_TO_TX_QUEUE_AFTER_DELAY,
    E_GP_DEV_ANCE
}teGP_GreenPowerBufferedCommands;


/* Structure for ZGP Duplicate filter table */
typedef struct
{
    uint8                               u3SecLevel  :1;
    uint8                               u3ApplicationID   :3;
    uint16                              u16AgingTime;
    tuGP_ZgpdDeviceAddr                 uZgpdDeviceAddr;
    union {
        uint32 u32gpdCrc;
        uint32 u32SecFrameCounter;
    } uData;
}tsGP_ZgpDuplicateTable;



typedef struct
{
	uint8								u8RSSI;
    uint16                              u16Delay;
    uint8                               u8GPP_GPDLink;
    teGP_GreenPowerBufferedCommands     eGreenPowerBufferedCommand;
    uint8                               u8PDUSize;
    uint8                               u8SeqNum;
    uint8                               u8TranslationIndex;
    bool_t                              bTunneledPkt;
    bool_t								bFrameType;
    uint16                              u16NwkShortAddr;
    uint32                              u8Status                   :8;
    uint32                              u8LinkQuality              :8;
    uint32                              u8CommandId                :8;
    uint32                              u2ApplicationId            :2;
    uint32                              u2SecurityLevel            :2;
    uint32                              u2SecurityKeyType          :2;
    uint32                              bAutoCommissioning         :1;
    uint32                              bRxAfterTx                 :1;
    uint32                              u32SecFrameCounter;
    uint32                              u32Mic;
    tuGP_ZgpdDeviceAddr                 uZgpdDeviceAddr;
    uint8                               au8PDU[GP_MAX_ZB_CMD_PAYLOAD_LENGTH];
}tsGP_BufferedApduInfo;

typedef struct {
    DNODE                               dllGpNode;
    tsGP_BufferedApduInfo               sBufferedApduInfo;
}tsGP_ZgpBufferedApduRecord;

typedef struct {
    uint8                               u8PDUSize;
    uint8                               u8SeqNum;
    uint8                               u8TranslationIndex;
    uint8                               u8GPP_GPDLink;
    bool_t                              bTunneledPkt;
    bool_t								bFrameType;
    uint8								u8RSSI;
    uint16                              u16NwkShortAddr;
    uint32                              u8Status                   :8;
    uint32                              u8LinkQuality              :8;
    uint32                              u8CommandId                :8;
    uint32                              u2ApplicationId            :2;
    uint32                              u2SecurityLevel            :2;
    uint32                              u2SecurityKeyType          :2;
    uint32                              bAutoCommissioning         :1;
    uint32                              bRxAfterTx                 :1;
    uint32                              u32SecFrameCounter;
    uint8                               *pu8Pdu;
    uint32                              u32Mic;
    tuGP_ZgpdDeviceAddr                 uZgpdDeviceAddr;
}tsGP_ZgpDataIndication;

typedef struct
{
    teGP_GreenPowerDeviceMode           eGreenPowerDeviceMode;
    teGP_GreenPowerDeviceType           eGreenPowerDeviceType;
    uint8                               bProxyCommissionEnterCmdSent                 :1;
    uint8                               bCommissionExitModeOnFirstPairSuccess        :1;
    uint8                               bCommissionExitModeOnCommissionModeExitCmd   :1;
    uint8                               bIsCommissionReplySent                       :1;
    uint8                               bIsCommissionIndGiven                        :1;
    uint8                               u8GroupCastRadius;
    uint8                               u8OperationalChannel;
    uint8                               u8GPDataReqHandle;
    uint16                              u16ProfileId;
    uint16                              u16CommissionWindow;
    uint16                              u16TransmitChannelTimeout;
#ifdef GP_COMBO_BASIC_DEVICE
    tsGP_TranslationTableEntry          *psZgpsTranslationTableEntry;
#endif
    uint64                              u64CommissionSetAddress;
    bool_t	                            bCommissionUnicast;
    uint16	                            u16CommissionUnicastAddress;
    tsGP_ZgpDuplicateTable              asZgpDuplicateFilterTable[GP_MAX_DUPLICATE_TABLE_ENTIRES];
    tsGP_ZgppProxySinkTable             asZgpsSinkProxyTable[GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES];


    DLIST                               lGpAllocList;
    DLIST                               lGpDeAllocList;
    tsGP_ZgpBufferedApduRecord          asZgpBufferedApduRecord[GP_MAX_NUMBER_BUFFERED_RECORDS];
    tsGP_Common                         sGPCommon;
}tsGP_GreenPowerCustomData;




/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC teZCL_Status eGP_CreateGreenPower(
                    tsZCL_ClusterInstance                  *psClusterInstance,
                    bool_t                                 bIsServer,
                    tsZCL_ClusterDefinition                *psClusterDefinition,
                    void                                   *pvEndPointSharedStructPtr,
                    uint8                                  *pu8AttributeControlBits,
                    tsGP_GreenPowerCustomData              *psCustomDataStructure);

PUBLIC teZCL_Status eGP_Update20mS(
                    uint8                                  u8GreenPowerEndPointId);
PUBLIC teZCL_Status eGP_Update1mS(
                    uint8                                  u8GreenPowerEndPointId);

PUBLIC void vZCL_HandleZgpDataIndication(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint8                                  u8GpEndPoint);

PUBLIC void vZCL_HandleZgpDataConfirm(
                    ZPS_tsAfEvent                          *pZPSevent,
                    uint8                                  u8GpEndpoint);

PUBLIC teZCL_Status eGP_ProxyCommissioningMode(
                    uint8                                  u8SourceEndPointId,
                    uint8                                  u8DestEndPointId,
                    tsZCL_Address                          sDestinationAddress,
                    teGP_GreenPowerProxyCommissionMode     eGreenPowerProxyCommissionMode);
#ifdef GP_COMBO_BASIC_DEVICE
PUBLIC bool_t bGP_IsSinkTableEntryPresent(
                    uint8                                  u8GpEndPointId,
                    uint8                                  u8ApplicationId,
                    tuGP_ZgpdDeviceAddr                    *puZgpdAddress,
                    tsGP_ZgppProxySinkTable                **psSinkTableEntry,
                    teGP_GreenPowerCommunicationMode       eCommunicationMode);
#endif

PUBLIC bool_t bGP_AliasPresent(
                    uint16                                 u16Address,
                    uint64                                 u64IEEEAddr,
                    uint8                                  u8AliasSeq);

PUBLIC teZCL_Status eGP_FindGpCluster(
                    uint8                                  u8SourceEndPoint,
                    bool_t                                   bIsServer,
                    tsZCL_EndPointDefinition               **ppsEndPointDefinition,
                    tsZCL_ClusterInstance                  **ppsClusterInstance,
                    tsGP_GreenPowerCustomData              **ppsGpCustomDataStructure);

PUBLIC void vGP_CallbackForPersistData(void);

PUBLIC void vGP_RestorePersistedData(
		            tsGP_ZgppProxySinkTable                   *psZgpsProxySinkTable,
                    teGP_ResetToDefaultConfig                 eSetToDefault);

PUBLIC teZCL_Status eGP_ZgpTranslationTableUpdateSend(
                    uint8                                  u8SourceEndPointId,
                    uint8                                  u8DestinationEndPointId,
                    tsZCL_Address                          *psDestinationAddress,
                    uint8                                  *pu8TransactionSequenceNumber,
                    tsGP_ZgpTranslationUpdateCmdPayload    *psZgpTransTableUpdatePayload);

PUBLIC teZCL_Status eGP_ZgpTranslationTableRequestSend(
                    uint8                                  u8SourceEndPointId,
                    uint8                                  u8DestinationEndPointId,
                    tsZCL_Address                          *psDestinationAddress,
                    uint8                                  *pu8TransactionSequenceNumber,
                    uint8                                  *pu8StartIndex);

PUBLIC teZCL_Status eGP_ZgpPairingConfigSend(
                    uint8                                  u8SourceEndPointId,
                    uint8                                  u8DestinationEndPointId,
                    tsZCL_Address                          *psDestinationAddress,
                    uint8                                  *pu8TransactionSequenceNumber,
                    tsGP_ZgpPairingConfigCmdPayload        *psZgpPairingConfigPayload);

PUBLIC uint8 u8GP_SecurityProcessCallback(
					ZPS_tsAfZgpSecReq                        *psSec,
                    uint8                                  *pu8SecIndex);

PUBLIC ZPS_teAfSecActions eGP_AddDeviceSecurity(
					ZPS_tsAfZgpSecReq                        *psSec);


PUBLIC bool_t bGP_IsProxyTableEntryPresent(
                    uint8                                  u8GpEndPointId,
                    bool_t								   bIsServer,
                    uint8                                  u8ApplicationId,
                    tuGP_ZgpdDeviceAddr                    *puZgpdAddress,
                    tsGP_ZgppProxySinkTable                **psProxySinkTableEntry);
PUBLIC bool_t bGP_GetFreeProxySinkTableEntry(
                    uint8                                  u8GreenPowerEndPointId,
                    bool_t								   bIsServer,
                    tsGP_ZgppProxySinkTable                **psProxySinkTableEntry);

PUBLIC teZCL_Status eGP_SinkTableRequestSend(
					uint8                               u8SourceEndPointId,
					uint8                               u8DestEndPointId,
					tsZCL_Address                       sDestinationAddress,
					tsGP_ZgpSinkTableRequestCmdPayload  *psZgpSinkTableRequestCmdPayload);
PUBLIC teZCL_Status eGP_ProxyTableRequestSend(
                        uint8                               u8SourceEndPointId,
                        uint8                               u8DestEndPointId,
                        tsZCL_Address                       sDestinationAddress,
                        tsGP_ZgpProxyTableRequestCmdPayload  *psZgpProxyTableRequestCmdPayload);


bool_t  bGP_CheckGPDAddressMatch(
		uint8 						u8GPTableAppIdSrc,
		uint8                       u8AppIdDst,
		tuGP_ZgpdDeviceAddr          *sGPTableAddrSrc,
		tuGP_ZgpdDeviceAddr          *sAddrDst
);
PUBLIC teZCL_Status eGP_ZgpPairingSend(
                    uint8                                  u8SourceEndPointId,
                    uint8                                  u8DestinationEndPointId,
                    tsZCL_Address                          *psDestinationAddress,
                    uint8                                  *pu8TransactionSequenceNumber,
                    tsGP_ZgpPairingCmdPayload              *psZgpPairingPayload);
void vGP_RemoveGPDFromProxySinkTable( uint8                                u8EndPointNumber,
		         uint8                                u8AppID,
		         tuGP_ZgpdDeviceAddr                  *puZgpdDeviceAddr);
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern const tsGP_ZgpCommandClusterTable asZgpCommadClusterTable[];


/****************************************************************************/
#if defined __cplusplus
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
#endif /* GREENPOWER_H_INCLUDED */
