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
 * MODULE:              ZLL Commissoning Cluster
 *
 * COMPONENT:          zll_commission.h
 *
 * DESCRIPTION:        Header for  Cluster
 *
 *****************************************************************************/

#ifndef ZLL_COMMISSION_H
#define ZLL_COMMISSION_H

#include <jendefs.h>
#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"
#include "zll_utility.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */
#define COMMISSIONING_CLUSTER_ID_TL_COMMISSIONING   0x1000

#define Z_COMMISSION_SERVER_DEVICE          0x1000
#define Z_COMMISSION_CLIENT_SERVER_DEVICE   0x1010
#define Z_COMMISSION_CLIENT_DEVICE          0x1020


#define TL_MAX_DEVICE_RECORDS          5
#define TL_MAX_GROUP_RECORDS          21
#define ZLL_MAX_ENDPOINT_RECORDS       8

#define TL_TEST_KEY_INDEX          0
#define TL_MASTER_KEY_INDEX        4
#define TL_CERTIFICATION_KEY_INDEX  15

#define TL_TEST_KEY_MASK            (1<<TL_TEST_KEY_INDEX)
#define TL_MASTER_KEY_MASK        (1<<TL_MASTER_KEY_INDEX)
#define TL_CERTIFICATION_KEY_MASK  (1<<TL_CERTIFICATION_KEY_INDEX)

#ifndef TL_SUPPORTED_KEYS
#define TL_SUPPORTED_KEYS (TL_TEST_KEY_MASK | TL_CERTIFICATION_KEY_MASK  )
#endif

/*  Zigbee Information field bits */
#define TL_COORDINATOR         0x00
#define TL_ROUTER              0x01
#define TL_ZED                 0x02
#define TL_TYPE_MASK           0x03
#define TL_RXON_IDLE           (1<<2)
#define TL_ZB_INFO_RESERVED    (0xF8)


/* TL Information field bit masks */
#define TL_FACTORY_NEW         (1<<0)
#define TL_ADDR_ASSIGN         (1<<1)
#define TL_LINK_INIT           (1<<4)
#define TL_TIME_WINDOW         (1<<5)            // response only (don't care in request)
#define TL_PROFILE_INTEROP     (1<<7)

/* Identify duration field */
#define TL_IDENTIFY_STOP           0
#define TL_IDENTIFY_TIME_MS(a)     (a)
#define TL_IDENTIFY_DEFAULT        0x0010

#ifndef ZLO_NUMBER_DEVICES
    #define ZLO_NUMBER_DEVICES      1
#endif

/* Touchlink address and Group Id ranges */
#define TL_MIN_ADDR     0x0001
#define TL_MAX_ADDR     0xfff7
#define TL_MIN_GROUP    0x0001
#define TL_MAX_GROUP    0xfeff

#define aplcInterPANTransIdLifetime        8            // Seconds
#define aplcMinStartupDelayTime            2            // Seconds
#define aplcRxWindowDuration               5            // Seconds
#define aplcScanTimeBaseDuration           250          // milli seconds

#define TL_SCAN_RX_TIME_MS                  aplcScanTimeBaseDuration
#define TL_INTERPAN_LIFE_TIME_SEC           aplcInterPANTransIdLifetime
#define TL_RX_WINDOW_TIME_SEC               aplcRxWindowDuration
#define TL_START_UP_TIME_SEC                aplcMinStartupDelayTime

#define TL_SUCCESS     0
#define TL_ERROR       1

#ifndef CLD_ZLL_COMMISSION_CLUSTER_REVISION
#define CLD_ZLL_COMMISSION_CLUSTER_REVISION 1
#endif

#ifndef CLD_ZLL_COMMISSION_FEATURE_MAP
#define CLD_ZLL_COMMISSION_FEATURE_MAP 0
#endif
/****************************************************************************/
/*              Cluster - Optional Attributes                */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes.                               */
/****************************************************************************/

/* There are no attributes */

/* End of optional attributes */

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#if 1
/* Attribute ID's */
typedef enum 
{
    E_CLD_HUH = 0x00,
    // there are no attributes
} teCLD_ZllCommission_ClusterID;

/* Identify Cluster */
typedef struct
{
    zbmap32 u32FeatureMap;
    zuint16 u16ClusterRevision;
} tsCLD_ZllCommission;


#endif

/* Command codes */
typedef enum 
{
    E_CLD_COMMISSION_CMD_SCAN_REQ = 0x00,               /* Mandatory (received by server) 0x00 */
    E_CLD_COMMISSION_CMD_SCAN_RSP,                      /* Mandatory (received by client) 0x01 */
    E_CLD_COMMISSION_CMD_DEVICE_INFO_REQ,               /* Mandatory (received by server) 0x02 */
    E_CLD_COMMISSION_CMD_DEVICE_INFO_RSP,               /* Mandatory (received by client) 0x03 */
    E_CLD_COMMISSION_CMD_IDENTIFY_REQ = 0x06,            /* Mandatory (received by server) 0x06 */
    E_CLD_COMMISSION_CMD_FACTORY_RESET_REQ,             /* Mandatory (received by server) 0x07 */
    E_CLD_COMMISSION_CMD_NETWORK_START_REQ = 0x10,      /* Mandatory (received by server) 0x10 */
    E_CLD_COMMISSION_CMD_NETWORK_START_RSP,             /* Mandatory (received by client) 0x11 */
    E_CLD_COMMISSION_CMD_NETWORK_JOIN_ROUTER_REQ,       /* Mandatory (received by server) 0x12 */
    E_CLD_COMMISSION_CMD_NETWORK_JOIN_ROUTER_RSP,       /* Mandatory (received by client) 0x13 */
    E_CLD_COMMISSION_CMD_NETWORK_JOIN_END_DEVICE_REQ,   /* Mandatory (received by server) 0x14 */
    E_CLD_COMMISSION_CMD_NETWORK_JOIN_END_DEVICE_RSP,   /* Mandatory (received by client) 0x15 */
    E_CLD_COMMISSION_CMD_NETWORK_UPDATE_REQ,            /* Mandatory (received by server) 0x16 */
} teCLD_ZllCommission_Command;

/* ZLL Device Record and Table structure */
typedef struct
{
    uint64  u64IEEEAddr;
    uint16  u16ProfileId;
    uint16 u16DeviceId;
    uint8   u8Endpoint;
    uint8 u8Version;
    uint8 u8NumberGroupIds;
    uint8 u8Sort;
} tsCLD_ZllDeviceRecord;

typedef struct
{
    uint8   u8NumberDevices;
    tsCLD_ZllDeviceRecord asDeviceRecords[ZLO_NUMBER_DEVICES];
} tsCLD_ZllDeviceTable;

/* Scan Request command payload */
typedef struct
{
    uint32 u32TransactionId;
    uint8 u8ZigbeeInfo;
    uint8 u8ZllInfo;
} tsCLD_ZllCommission_ScanReqCommandPayload;

/* Scan Response command payload */
typedef struct
{
    uint32  u32TransactionId;
    uint8   u8RSSICorrection;
    uint8   u8ZigbeeInfo;
    uint8   u8ZllInfo;
    uint16  u16KeyMask;
    uint32  u32ResponseId;
    uint64  u64ExtPanId;
    uint8   u8NwkUpdateId;
    uint8   u8LogicalChannel;
    uint16  u16PanId;
    uint16  u16NwkAddr;
    uint8   u8NumberSubDevices;
    uint8   u8TotalGroupIds;
    uint8   u8Endpoint;
    uint16  u16ProfileId;
    uint16  u16DeviceId;
    uint8   u8Version;
    uint8   u8GroupIdCount;
} tsCLD_ZllCommission_ScanRspCommandPayload;


/* Device information Request command payload */
typedef struct
{
    uint32 u32TransactionId;
    uint8 u8StartIndex;
} tsCLD_ZllCommission_DeviceInfoReqCommandPayload;

/* Device Information Response command payload */
typedef struct
{
    uint32 u32TransactionId;
    uint8   u8NumberSubDevices;
    uint8   u8StartIndex;
    uint8   u8DeviceInfoRecordCount;
    tsCLD_ZllDeviceRecord asDeviceRecords[TL_MAX_DEVICE_RECORDS];
} tsCLD_ZllCommission_DeviceInfoRspCommandPayload;


/* Identify Request command payload */
typedef struct
{
    uint32  u32TransactionId;
    uint16   u16Duration;
} tsCLD_ZllCommission_IdentifyReqCommandPayload;

/* Factory Reset Request command payload */
typedef struct
{
    uint32 u32TransactionId;
} tsCLD_ZllCommission_FactoryResetReqCommandPayload;


/* Network Start Request command payload */
typedef struct
{
    uint32 u32TransactionId;
    uint64  u64ExtPanId;
    uint8   u8KeyIndex;
    uint8   au8NwkKey[16];
    uint8   u8LogicalChannel;
    uint16  u16PanId;
    uint16  u16NwkAddr;
    uint16  u16GroupIdBegin;
    uint16  u16GroupIdEnd;
    uint16  u16FreeNwkAddrBegin;
    uint16  u16FreeNwkAddrEnd;
    uint16  u16FreeGroupIdBegin;
    uint16  u16FreeGroupIdEnd;
    uint64  u64InitiatorIEEEAddr;
    uint16  u16InitiatorNwkAddr;
} tsCLD_ZllCommission_NetworkStartReqCommandPayload;


/* Network start Response command payload */
typedef struct
{
    uint32 u32TransactionId;
    uint8   u8Status;
    uint64  u64ExtPanId;
    uint8   u8NwkUpdateId;
    uint8   u8LogicalChannel;
    uint16  u16PanId;
} tsCLD_ZllCommission_NetworkStartRspCommandPayload;

/* Network Join Router Request command payload */
typedef struct
{
    uint32 u32TransactionId;
    uint64  u64ExtPanId;
    uint8   u8KeyIndex;
    uint8   au8NwkKey[16];
    uint8   u8NwkUpdateId;
    uint8   u8LogicalChannel;
    uint16  u16PanId;
    uint16  u16NwkAddr;
    uint16  u16GroupIdBegin;
    uint16  u16GroupIdEnd;
    uint16  u16FreeNwkAddrBegin;
    uint16  u16FreeNwkAddrEnd;
    uint16  u16FreeGroupIdBegin;
    uint16  u16FreeGroupIdEnd;
} tsCLD_ZllCommission_NetworkJoinRouterReqCommandPayload;


/* Network Join Router Response command payload */
typedef struct
{
    uint32  u32TransactionId;
    uint8   u8Status;
} tsCLD_ZllCommission_NetworkJoinRouterRspCommandPayload;


/*  command payload */
typedef struct
{
    uint32 u32TransactionId;
    uint64  u64ExtPanId;
    uint8   u8KeyIndex;
    uint8   au8NwkKey[16];
    uint8   u8NwkUpdateId;
    uint8   u8LogicalChannel;
    uint16  u16PanId;
    uint16  u16NwkAddr;
    uint16  u16GroupIdBegin;
    uint16  u16GroupIdEnd;
    uint16  u16FreeNwkAddrBegin;
    uint16  u16FreeNwkAddrEnd;
    uint16  u16FreeGroupIdBegin;
    uint16  u16FreeGroupIdEnd;
} tsCLD_ZllCommission_NetworkJoinEndDeviceReqCommandPayload;


/* Network join end device response command payload */
typedef struct
{
    uint32 u32TransactionId;
    uint8   u8Status;
} tsCLD_ZllCommission_NetworkJoinEndDeviceRspCommandPayload;


/* Network Update Request command payload */
typedef struct
{
    uint32 u32TransactionId;
    uint64  u64ExtPanId;
    uint8   u8NwkUpdateId;
    uint8   u8LogicalChannel;
    uint16  u16PanId;
    uint16  u16NwkAddr;
} tsCLD_ZllCommission_NetworkUpdateReqCommandPayload;

#if 0
/* Endpoint Information command payload */
typedef struct
{
    uint32 u32TransactionId;
    uint64  u64IEEEAddr;
    uint16  u16NwkAddr;
    uint8   u8Endpoint;
    uint16  u16ProfileID;
    uint16  u16DeviceID;
    uint8   u8Version;
} tsCLD_ZllCommission_EndpointInformationCommandPayload;
#endif









/* Definition of Call back Event Structure */
typedef struct
{
    uint8   u8CommandId;
    union
    {
        tsCLD_ZllCommission_ScanReqCommandPayload                   *psScanReqPayload;
        tsCLD_ZllCommission_ScanRspCommandPayload                   *psScanRspPayload;
        tsCLD_ZllCommission_IdentifyReqCommandPayload               *psIdentifyReqPayload;
        tsCLD_ZllCommission_DeviceInfoReqCommandPayload             *psDeviceInfoReqPayload;
        tsCLD_ZllCommission_DeviceInfoRspCommandPayload             *psDeviceInfoRspPayload;
        tsCLD_ZllCommission_FactoryResetReqCommandPayload           *psFactoryResetPayload;
        tsCLD_ZllCommission_NetworkStartReqCommandPayload           *psNwkStartReqPayload;
        tsCLD_ZllCommission_NetworkStartRspCommandPayload           *psNwkStartRspPayload;
        tsCLD_ZllCommission_NetworkJoinRouterReqCommandPayload      *psNwkJoinRouterReqPayload;
        tsCLD_ZllCommission_NetworkJoinRouterRspCommandPayload      *psNwkJoinRouterRspPayload;
        tsCLD_ZllCommission_NetworkJoinEndDeviceReqCommandPayload   *psNwkJoinEndDeviceReqPayload;
        tsCLD_ZllCommission_NetworkJoinEndDeviceRspCommandPayload   *psNwkJoinEndDeviceRspPayload;
        tsCLD_ZllCommission_NetworkUpdateReqCommandPayload          *psNwkUpdateReqPayload;

        tsCLD_ZllUtility_EndpointInformationCommandPayload       *psEndpointInfoPayload;
        tsCLD_ZllUtility_GetGroupIdReqCommandPayload             *psGetGroupIdReqPayload;
        tsCLD_ZllUtility_GetGroupIdRspCommandPayload             *psGetGroupIdRspPayload;
        tsCLD_ZllUtility_GetEndpointListReqCommandPayload        *psGetEndpointlistReqPayload;
        tsCLD_ZllUtility_GetEndpointListRspCommandPayload        *psGetEndpointListRspPayload;
    } uMessage;
} tsCLD_ZllCommissionCallBackMessage;


/* Custom data structure */
typedef struct
{
    union {
    tsZCL_ReceiveEventAddressInterPan        sRxInterPanAddr;
    tsZCL_ReceiveEventAddress                sRxAddr;
    };
    tsZCL_CallBackEvent                      sCustomCallBackEvent;
    tsCLD_ZllCommissionCallBackMessage       sCallBackMessage;
} tsCLD_ZllCommissionCustomDataStructure;

typedef struct
{
    teCLD_ZllCommission_Command eCommand;
    ZPS_tsInterPanAddress       sSrcAddr;
    union
    {
        tsCLD_ZllCommission_ScanReqCommandPayload                   sScanReqPayload;
        tsCLD_ZllCommission_ScanRspCommandPayload                   sScanRspPayload;
        tsCLD_ZllCommission_IdentifyReqCommandPayload               sIdentifyReqPayload;
        tsCLD_ZllCommission_DeviceInfoReqCommandPayload             sDeviceInfoReqPayload;
        tsCLD_ZllCommission_DeviceInfoRspCommandPayload             sDeviceInfoRspPayload;
        tsCLD_ZllCommission_FactoryResetReqCommandPayload           sFactoryResetPayload;
        tsCLD_ZllCommission_NetworkStartReqCommandPayload           sNwkStartReqPayload;
        tsCLD_ZllCommission_NetworkStartRspCommandPayload           sNwkStartRspPayload;
        tsCLD_ZllCommission_NetworkJoinRouterReqCommandPayload      sNwkJoinRouterReqPayload;
        tsCLD_ZllCommission_NetworkJoinRouterRspCommandPayload      sNwkJoinRouterRspPayload;
        tsCLD_ZllCommission_NetworkJoinEndDeviceReqCommandPayload   sNwkJoinEndDeviceReqPayload;
        tsCLD_ZllCommission_NetworkJoinEndDeviceRspCommandPayload   sNwkJoinEndDeviceRspPayload;
        tsCLD_ZllCommission_NetworkUpdateReqCommandPayload          sNwkUpdateReqPayload;
    }uPayload;
} tsZllMessage;


typedef struct
{
    union
    {
        tsCLD_ZllCommission_ScanReqCommandPayload                   sScanReqPayload;
        tsCLD_ZllCommission_ScanRspCommandPayload                   sScanRspPayload;
        tsCLD_ZllCommission_IdentifyReqCommandPayload               sIdentifyReqPayload;
        tsCLD_ZllCommission_DeviceInfoReqCommandPayload             sDeviceInfoReqPayload;
        tsCLD_ZllCommission_DeviceInfoRspCommandPayload             sDeviceInfoRspPayload;
        tsCLD_ZllCommission_FactoryResetReqCommandPayload           sFactoryResetPayload;
        tsCLD_ZllCommission_NetworkStartReqCommandPayload           sNwkStartReqPayload;
        tsCLD_ZllCommission_NetworkStartRspCommandPayload           sNwkStartRspPayload;
        tsCLD_ZllCommission_NetworkJoinRouterReqCommandPayload      sNwkJoinRouterReqPayload;
        tsCLD_ZllCommission_NetworkJoinRouterRspCommandPayload      sNwkJoinRouterRspPayload;
        tsCLD_ZllCommission_NetworkJoinEndDeviceReqCommandPayload   sNwkJoinEndDeviceReqPayload;
        tsCLD_ZllCommission_NetworkJoinEndDeviceRspCommandPayload   sNwkJoinEndDeviceRspPayload;
        tsCLD_ZllCommission_NetworkUpdateReqCommandPayload          sNwkUpdateReqPayload;
    }uPayload;
} tsZllPayloads;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_ZllCommissionCreateCommission(
                    tsZCL_ClusterInstance              *psClusterInstance,
                    bool_t                              bIsServer,
                    tsZCL_ClusterDefinition            *psClusterDefinition,
                    void                               *pvSharedStructPtr,
                    uint8                              *pu8AttributeControlBits,
                    tsCLD_ZllCommissionCustomDataStructure  *psCustomDataStructure);

PUBLIC teZCL_Status eCLD_ZllCommissionCommandScanReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_ScanReqCommandPayload  *psPayload);

PUBLIC teZCL_Status eCLD_ZllCommissionCommandScanRspCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_ScanRspCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ZllCommissionCommandDeviceInfoReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_DeviceInfoReqCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ZllCommissionCommandDeviceInfoRspCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_DeviceInfoRspCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ZllCommissionCommandDeviceIndentifyReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_IdentifyReqCommandPayload *psPayload);
/*
PUBLIC teZCL_Status eCLD_ZllCommissionCommandDeviceIdentifyRspCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_IdentifyRequestCommandPayload *psPayload);
*/
PUBLIC teZCL_Status eCLD_ZllCommissionCommandFactoryResetReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_FactoryResetReqCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ZllCommissionCommandNetworkStartReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_NetworkStartReqCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ZllCommissionCommandNetworkStartRspCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_NetworkStartRspCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ZllCommissionCommandNetworkJoinRouterReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_NetworkJoinRouterReqCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ZllCommissionCommandNetworkJoinRouterRspCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_NetworkJoinRouterRspCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ZllCommissionCommandNetworkJoinEndDeviceReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_NetworkJoinEndDeviceReqCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ZllCommissionCommandNetworkJoinEndDeviceRspCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_NetworkJoinEndDeviceRspCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ZllCommissionCommandNetworkUpdateReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_NetworkUpdateReqCommandPayload *psPayload);



PUBLIC teZCL_Status eCLD_ZllCommissionCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);


/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/


extern const tsZCL_AttributeDefinition asCLD_ZllCommissionClusterAttributeDefinitions[];
extern tsZCL_ClusterDefinition sCLD_ZllCommission;
extern uint8 au8ZllCommissionAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* ZLL_COMMISSION_H */
