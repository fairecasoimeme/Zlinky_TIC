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
 * MODULE:          Zigbee Protocol Stack Application Layer
 *
 * COMPONENT:       zps_apl_af.h
 *
 * DESCRIPTION:     Provides a user friendly API to the application object
 *                  for making data requests
 *
 *****************************************************************************/

#ifndef ZPS_APL_AF_H_
#define ZPS_APL_AF_H_

#include <jendefs.h>
#include <mac_vs_sap.h>
#include <zps_nwk_sap.h>
#include <pdum_apl.h>
#include <zps_apl.h>
#include <zps_apl_aps.h>
#include <zps_apl_zdo.h>
#include <zps_apl_zdp.h>
#include <zps_apl_aib.h>
#include "aessw_ccm.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifndef ZPS_APL_INLINE
#define ZPS_APL_INLINE  INLINE
#endif


#ifndef ZPS_APL_ALWAYS_INLINE
#define ZPS_APL_ALWAYS_INLINE  ALWAYS_INLINE
#endif

#define ZPS_E_APL_AF_ALL_ENDPOINTS          (0xff)
/* Update Device Status */
#define ZPS_APL_SEC_STD_SEC_REJOIN          0
#define ZPS_APL_SEC_STD_UNSEC_JOIN          1
#define ZPS_APL_SEC_DEVICE_LEFT             2
#define ZPS_APL_SEC_STD_UNSEC_REJOIN        3
#define ZPS_APL_SEC_INT_LEAVE_REJION        4

#define ZPS_APL_FILTER_NONE                 0
#define ZPS_APL_FILTER_BCAST                1
#define ZPS_APL_FILTER_ALL                  2

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef struct {
    void (*prCallback)(void *, void *);
    void *pvContext;
    void *pvParam;
} zps_tsTimeEvent;

/* Security modes for data requests */
typedef enum {
    ZPS_E_APL_AF_UNSECURE = 0,
    ZPS_E_APL_AF_SECURE     = 0x01,
    ZPS_E_APL_AF_SECURE_NWK = 0x02,
    ZPS_E_APL_AF_EXT_NONCE  = 0x10,
    ZPS_E_APL_AF_WILD_PROFILE  = 0x20,
} ZPS_teAplAfSecurityMode;


/* Zigbee Greenpower status codes */
typedef enum {
    ZPS_E_APL_AF_GP_SECURITY_SUCCESS = ZPS_E_SUCCESS,
    ZPS_E_APL_AF_GP_NO_SECURITY,
    ZPS_E_APL_AF_GP_COUNTER_FAILURE,
    ZPS_E_APL_AF_GP_AUTH_FAILURE,
    ZPS_E_APL_AF_GP_UNPROCESSED,
} ZPS_teAplAfGpDataIndStatus;

/* Broadcast modes for data requests */
typedef enum {
    ZPS_E_APL_AF_BROADCAST_ALL, /* broadcast to all nodes, including all end devices */
    ZPS_E_APL_AF_BROADCAST_RX_ON, /* broadcast to all nodes with their receivers enabled when idle */
    ZPS_E_APL_AF_BROADCAST_ZC_ZR /* broadcast only to coordinator and routers */
} ZPS_teAplAfBroadcastMode;

/* Node descriptor */
typedef struct {
    uint32                   : 8; /* padding */
    uint32 eLogicalType      : 3;
    uint32 bComplexDescAvail : 1;
    uint32 bUserDescAvail    : 1;
    uint32 eReserved         : 3; /* reserved */
    uint32 eFrequencyBand    : 5;
    uint32 eApsFlags         : 3;
    uint32 u8MacFlags        : 8;
    uint16 u16ManufacturerCode;
    uint8 u8MaxBufferSize;
    uint16 u16MaxRxSize;
    uint16 u16ServerMask;
    uint16 u16MaxTxSize;
    uint8 u8DescriptorCapability;
} ZPS_tsAplAfNodeDescriptor;

#ifdef LITTLE_ENDIAN_PROCESSOR
/* Node power descriptor */
typedef struct {
    uint32 eCurrentPowerSourceLevel    : 4;
    uint32 eCurrentPowerSource         : 4;
    uint32 eAvailablePowerSources      : 4;
    uint32 eCurrentPowerMode           : 4;
} ZPS_tsAplAfNodePowerDescriptor;
#else
/* Node power descriptor */
typedef struct {
    uint32 eCurrentPowerMode        : 4;
    uint32 eAvailablePowerSources   : 4;
    uint32 eCurrentPowerSource      : 4;
    uint32 eCurrentPowerSourceLevel : 4;
} ZPS_tsAplAfNodePowerDescriptor;
#endif

/* Endpoint simple descriptor */
typedef struct {
    uint16 u16ApplicationProfileId;
    uint16 u16DeviceId;
    uint8  u8DeviceVersion;
    uint8  u8Endpoint;
    uint8  u8InClusterCount;
    uint8  u8OutClusterCount;
    const uint16 *pu16InClusterList;
    const uint16 *pu16OutClusterList;
    uint8 *au8InDiscoveryEnabledFlags;
    uint8 *au8OutDiscoveryEnabledFlags;
} ZPS_tsAplAfSimpleDescriptor;

/* Node's user descriptor */
typedef struct {
    char szUserDescriptor[16];
} ZPS_tsAplAfUserDescriptor;

typedef enum {
    ZPS_EVENT_NONE,                                     /*  0, 0x00 */
    ZPS_EVENT_APS_DATA_INDICATION,                      /*  1, 0x01 */
    ZPS_EVENT_APS_DATA_CONFIRM,                         /*  2, 0x02 */
    ZPS_EVENT_APS_DATA_ACK,                             /*  3, 0x03 */
    ZPS_EVENT_NWK_STARTED,                              /*  4, 0x04 */
    ZPS_EVENT_NWK_JOINED_AS_ROUTER,                     /*  5, 0x05 */
    ZPS_EVENT_NWK_JOINED_AS_ENDDEVICE,                  /*  6, 0x06 */
    ZPS_EVENT_NWK_FAILED_TO_START,                      /*  7, 0x07 */
    ZPS_EVENT_NWK_FAILED_TO_JOIN,                       /*  8, 0x08 */
    ZPS_EVENT_NWK_NEW_NODE_HAS_JOINED,                  /*  9, 0x09 */
    ZPS_EVENT_NWK_DISCOVERY_COMPLETE,                   /* 10, 0x0a */
    ZPS_EVENT_NWK_LEAVE_INDICATION,                     /* 11, 0x0b */
    ZPS_EVENT_NWK_LEAVE_CONFIRM,                        /* 12, 0x0c */
    ZPS_EVENT_NWK_STATUS_INDICATION,                    /* 13, 0x0d */
    ZPS_EVENT_NWK_ROUTE_DISCOVERY_CONFIRM,              /* 14, 0x0e */
    ZPS_EVENT_NWK_POLL_CONFIRM,                         /* 15, 0x0f */
    ZPS_EVENT_NWK_ED_SCAN,                              /* 16, 0x10 */
    ZPS_EVENT_ZDO_BIND,                                 /* 17, 0x11 */
    ZPS_EVENT_ZDO_UNBIND,                               /* 18, 0x12 */
    ZPS_EVENT_ZDO_LINK_KEY,                             /* 19, 0x13 */
    ZPS_EVENT_BIND_REQUEST_SERVER,                      /* 20, 0x14.*/
    ZPS_EVENT_ERROR,                                    /* 21, 0x15 */
    ZPS_EVENT_APS_INTERPAN_DATA_INDICATION,             /* 22, 0x16 */
    ZPS_EVENT_APS_INTERPAN_DATA_CONFIRM,                /* 23, 0x17 */
    ZPS_EVENT_APS_ZGP_DATA_INDICATION,                  /* 24, 0x18 */
    ZPS_EVENT_APS_ZGP_DATA_CONFIRM,                     /* 25, 0x19 */
    ZPS_EVENT_TC_STATUS,                                /* 26, 0X1A */
    ZPS_EVENT_NWK_DUTYCYCLE_INDICATION,                 /* 27, 0x1B */
    ZPS_EVENT_NWK_FAILED_TO_SELECT_AUX_CHANNEL,         /* 28, 0x1C */
    ZPS_EVENT_NWK_ROUTE_RECORD_INDICATION,              /* 29, 0x1D */
    ZPS_EVENT_NWK_DISCOVERY_ACTIVE_COMPLETE,            /* 30, 0x1F */
    ZPS_ZCP_EVENT_FAILURE
} ZPS_teAfEventType;


typedef struct
{
    ZPS_tuAddress uDstAddress;
    ZPS_tuAddress uSrcAddress;
    PDUM_thAPduInstance hAPduInst;
    uint32 u32RxTime;
    uint16 u16ProfileId;
    uint16 u16ClusterId;
    uint8  u8DstAddrMode;
    uint8  u8DstEndpoint;
    uint8  u8SrcAddrMode;
    uint8  u8SrcEndpoint;
    uint8 eStatus;
    uint8 eSecurityStatus;
    uint8 u8LinkQuality;
    bool_t bFramePending;
} ZPS_tsAfDataIndEvent;

typedef struct {
    ZPS_tuAddress uDstAddr;
    uint8 u8Status;
    uint8 u8SrcEndpoint;
    uint8 u8DstEndpoint;
    uint8 u8DstAddrMode;
    uint8   u8SequenceNum;
} ZPS_tsAfDataConfEvent;

typedef struct {
    uint16  u16DstAddr;
    uint16  u16ProfileId;
    uint16  u16ClusterId;
    uint8   u8Status;
    uint8   u8SrcEndpoint;
    uint8   u8DstEndpoint;
    uint8   u8DstAddrMode;
    uint8   u8SequenceNum;
} ZPS_tsAfDataAckEvent;

typedef struct {
    uint8 u8Status;
} ZPS_tsAfNwkFormationEvent;

typedef struct
{
    uint16 u16Addr;
    bool_t bRejoin;
    bool_t bSecuredRejoin;
} ZPS_tsAfNwkJoinedEvent;

typedef struct
{
    uint8 u8Status;
    bool_t bRejoin;
} ZPS_tsAfNwkJoinFailedEvent;

typedef struct
{
    ZPS_tsNwkNetworkDescr * psNwkDescriptors;
    uint32 u32UnscannedChannels;
    uint8 eStatus;
    uint8 u8NetworkCount;
    uint8 u8SelectedNetwork;
} ZPS_tsAfNwkDiscoveryEvent;

typedef struct
{
    uint64 u64ExtAddr;     /**< Device's IEEE address */
    uint16 u16NwkAddr;     /**< Extended address of device wishing to associate */
    uint8  u8Capability;   /**< Device capabilities */
    uint8  u8Rejoin;       /**< The nature of the join or rejoin */
    uint8  u8SecureRejoin; /**< Indicates if the rejoin was a secure rejoin */
} ZPS_tsAfNwkJoinIndEvent;

typedef struct {
    uint64 u64ExtAddr;     /**< Device's IEEE address */
    uint8  u8Rejoin;
} ZPS_tsAfNwkLeaveIndEvent;

typedef struct {
    uint64 u64ExtAddr;
    uint8  eStatus;
    bool_t bRejoin;
} ZPS_tsAfNwkLeaveConfEvent;

typedef struct {
    uint16  u16NwkAddr;
    uint8   u8Status;
} ZPS_tsAfNwkStatusIndEvent;

typedef struct {
    uint16 u16DstAddress;
    uint8   u8Status;
    uint8   u8NwkStatus;
} ZPS_tsAfNwkRouteDiscoveryConfEvent;

typedef struct {
    uint8   u8Status;
} ZPS_tsAfPollConfEvent;

typedef struct {
    ZPS_tuAddress uDstAddr;
    uint8 u8DstAddrMode;
    uint8 u8SrcEp;
    uint8 u8DstEp;
} ZPS_tsAfZdoBindEvent;

typedef ZPS_tsAfZdoBindEvent ZPS_tsAfZdoUnbindEvent;

typedef struct {
    uint8 u8KeyType;
    uint64 u64IeeeLinkAddr;
} ZPS_tsAfZdoLinkKeyEvent;

typedef ZPS_tsNwkNlmeCfmEdScan ZPS_tsAfNwkEdScanConfEvent;

typedef struct {
    enum {
        ZPS_ERROR_APDU_TOO_SMALL,
        ZPS_ERROR_APDU_INSTANCES_EXHAUSTED,
        ZPS_ERROR_NO_APDU_CONFIGURED,
        ZPS_ERROR_OS_MESSAGE_QUEUE_OVERRUN,
        ZPS_ERROR_APS_SECURITY_FAIL,
        ZPS_ERROR_ZDO_LINKSTATUS_FAIL,
    } eError;

    union {
        struct {
            uint16 u16ProfileId;
            uint16 u16ClusterId;
            union {
                uint16 u16SrcAddr;
                uint64 u64SrcAddr;
            }uAddr;
            uint16 u16DataSize;
            PDUM_thAPdu hAPdu;
            uint8  u8SrcEndpoint;
            uint8  u8DstEndpoint;
            uint8  u8SrcAddrMode;
        }sAfErrorApdu;

        struct {
            void* hMessage;
        } sAfErrorOsMessageOverrun;

        uint64 u64Value;
    } uErrorData;
} ZPS_tsAfErrorEvent;


typedef struct
{
    uint32 u32FailureCount;
    uint8 u8Status;
    uint8 u8SrcEndpoint;
}ZPS_tsAfBindRequestServerEvent;


typedef struct
{
    ZPS_tsInterPanAddress sDstAddr;
    PDUM_thAPduInstance hAPduInst;
    uint64 u64SrcAddress;
    uint16 u16SrcPan;
    uint16 u16ProfileId;
    uint16 u16ClusterId;
    uint8  u8SrcAddrMode;
    uint8 eStatus;
    uint8 u8DstEndpoint;
    uint8 u8LinkQuality;
} ZPS_tsAfInterPanDataIndEvent;

typedef struct {
    uint8 u8Status;
    uint8 u8Handle;
} ZPS_tsAfInterPanDataConfEvent;

typedef union {
    ZPS_tsAplApsKeyDescriptorEntry *pKeyDesc;
    uint64 u64ExtendedAddress;
} ZPS_tuTcStatusData;
typedef struct
{
    ZPS_tuTcStatusData uTcData;
    uint8 u8Status;
}ZPS_tsAfTCstatusEvent;

typedef union {
    uint64 u64Addr;
    uint32 u32SrcId;
    uint16 u16Addr;
} ZPS_tuGpAddress;

typedef struct
{
    ZPS_tuGpAddress uGpAddress;
    PDUM_thAPduInstance hAPduInst;
    uint32   u8Status               :8;
    uint32   u2ApplicationId        :2;
    uint32   u2SecurityLevel        :2;
    uint32   u2SecurityKeyType      :2;
    uint32   u8LinkQuality          :8;
    uint32   bAutoCommissioning     :1;
    uint32   bRxAfterTx             :1;
    uint32   u8CommandId            :8;
    uint32   u32Mic;
    uint32  u32SecFrameCounter;
    uint16  u16SrcPanId;
    uint8   u8Rssi;
    uint8   u8SrcAddrMode;
    uint8   u8SeqNum;
    uint8   u8FrameType;
    uint8   u8Endpoint;
} ZPS_tsAfZgpDataIndEvent;

typedef struct {
    uint8 u8Status;
    uint8 u8Handle;
} ZPS_tsAfZgpDataConfEvent;


typedef struct {
    uint8 u8Mode;
} ZPS_tsAfNwkDutyCycleIndEvent;

typedef struct {
	uint16    u16DstAddress;
	uint8     u8Status;
} ZPS_tsAfNwkRouteRecordIndEvent;



typedef union
{
    ZPS_tsAfDataIndEvent                sApsDataIndEvent;
    ZPS_tsAfDataConfEvent               sApsDataConfirmEvent;
    ZPS_tsAfDataAckEvent                sApsDataAckEvent;
    ZPS_tsAfNwkFormationEvent           sNwkFormationEvent;
    ZPS_tsAfNwkJoinedEvent              sNwkJoinedEvent;
    ZPS_tsAfNwkJoinFailedEvent          sNwkJoinFailedEvent;
    ZPS_tsAfNwkDiscoveryEvent           sNwkDiscoveryEvent;
    ZPS_tsAfNwkJoinIndEvent             sNwkJoinIndicationEvent;
    ZPS_tsAfNwkLeaveIndEvent            sNwkLeaveIndicationEvent;
    ZPS_tsAfNwkLeaveConfEvent           sNwkLeaveConfirmEvent;
    ZPS_tsAfNwkStatusIndEvent           sNwkStatusIndicationEvent;
    ZPS_tsAfNwkRouteDiscoveryConfEvent  sNwkRouteDiscoveryConfirmEvent;
    ZPS_tsAfPollConfEvent               sNwkPollConfirmEvent;
    ZPS_tsAfNwkEdScanConfEvent          sNwkEdScanConfirmEvent;
    ZPS_tsAfErrorEvent                  sAfErrorEvent;
    ZPS_tsAfZdoBindEvent                sZdoBindEvent;
    ZPS_tsAfZdoUnbindEvent              sZdoUnbindEvent;
    ZPS_tsAfZdoLinkKeyEvent             sZdoLinkKeyEvent;
    ZPS_tsAfBindRequestServerEvent      sBindRequestServerEvent;
    ZPS_tsAfInterPanDataIndEvent        sApsInterPanDataIndEvent;
    ZPS_tsAfInterPanDataConfEvent       sApsInterPanDataConfirmEvent;
    ZPS_tsAfZgpDataIndEvent             sApsZgpDataIndEvent;
    ZPS_tsAfZgpDataConfEvent            sApsZgpDataConfirmEvent;
    ZPS_tsAfTCstatusEvent               sApsTcEvent;
    ZPS_tsAfNwkDutyCycleIndEvent        sNwkDutyCycleIndicationEvent;
    ZPS_tsAfNwkRouteRecordIndEvent      sNwkRouteRecordIndEvent;
} ZPS_tuAfEventData;

/* event messages from the stack to the application */
typedef struct {
    ZPS_teAfEventType eType;
    ZPS_tuAfEventData uEvent;
} ZPS_tsAfEvent;

typedef struct {
    ZPS_tuAddress uDstAddr;
    uint16 u16ClusterId;
    uint16 u16ProfileId;
    uint8 u8SrcEp;
    ZPS_teAplApsdeAddressMode eDstAddrMode;
    uint8 u8DstEp;
    ZPS_teAplAfSecurityMode eSecurityMode;
    uint8 u8Radius;
}ZPS_tsAfProfileDataReq;

typedef enum
{
  ZPS_LEAVE_ORIGIN_NLME = 0,                  /* LEAVE_FROM_NLME */
  ZPS_LEAVE_ORIGIN_MGMT_LEAVE,                /* LEAVE_FROM_MGMT_LEAVE */
  ZPS_LEAVE_ORIGIN_REMOVE_DEVICE              /* LEAVE_FROM_REMOVE_DEVICE */
}ZPS_teAfLeaveActions;

typedef enum
{
  ZPS_E_ENTRY_ADDED = 0,                  /* LEAVE_FROM_NLME */
  ZPS_E_ENTRY_REPLACED,                /* LEAVE_FROM_MGMT_LEAVE */
  ZPS_E_ENTRY_NO_QUEUE,
  ZPS_E_ENTRY_REMOVED,
  ZPS_E_INVALID_ENTRY,
  ZPS_E_QUEUE_FULL              /* LEAVE_FROM_REMOVE_DEVICE */
}ZPS_teAfZgpQueueActions;


typedef enum
{
  ZPS_E_ZGP_ZIGBEE_NWK_KEY = 1,
  ZPS_E_ZGP_GROUP_KEY = 2 ,
  ZPS_E_ZGP_NWK_KEY_DERIVED_GRP_KEY = 3,
  ZPS_E_ZGP_INDIVIDUAL_KEY = 4,
  ZPS_E_ZGP_DERIVED_INDIVIDUAL_KEY = 7,
  ZPS_E_ZGP_OAP_TC_LK = 8
}ZPS_teAfZgpKeyType;

typedef enum
{
    ZPS_E_MATCH = 0,
    ZPS_E_DROP_FRAME,
    ZPS_E_TX_THEN_DROP,
    ZPS_E_PASS_UNPROCESSED
}ZPS_teAfSecActions;

typedef enum
{
    ZPS_ZDO_ST_INACTIVE,
    ZPS_ZDO_ST_NWK_FORMATION,
    ZPS_ZDO_ST_NWK_DISCOVERY,
    ZPS_ZDO_ST_NWK_DISCOVERY_COMPLETE,
    ZPS_ZDO_ST_NWK_JOINING,
    ZPS_ZDO_ST_NWK_REJOIN,
    ZPS_ZDO_ST_ACTIVE,
    ZPS_ZDO_PENDING_AUTHENTICATION,
    ZPS_ZDO_PENDING_LEAVE,
    ZPS_ZDO_PENDING_LEAVE_WITH_REJOIN,
    ZPS_NUM_ZDO_STATES
} ZPS_teZdoNetworkState;

typedef union
{
    uint64 u64Address;
    uint32 u32SrcId;
}ZPS_tuAfZgpGreenPowerId;

typedef struct
{
    ZPS_tuAfZgpGreenPowerId uGpId;
    uint16 u16Panid;
    uint16 u16DstAddr;
    uint16 u16TxQueueEntryLifetime;
    uint8 u8Handle;
    uint8 u8ApplicationId;
    uint8 u8SeqNum;
    uint8 u8TxOptions;
    uint8 u8Endpoint;
   // bool_t bDataFrame;
}ZPS_tsAfZgpGreenPowerReq;

typedef struct
{
    ZPS_tsAfZgpGreenPowerReq sReq;
    PDUM_thNPdu hNPdu;
    bool_t bValid;
}ZPS_tsAfZgpTxGpQueueEntry;

/* Security table structure */
typedef struct
{
    AESSW_Block_u uSecurityKey;
    ZPS_tuAfZgpGreenPowerId uGpId;
    uint32        u32Counter;
    uint8         u8SecurityLevel;
    uint8         u8KeyType;
    bool_t        bValid;
} ZPS_tsAfZgpGpstEntry;


typedef struct
{
  uint16    u16SectorSize;
  uint16    u16CredNodesCount;
  uint8     u8SectorSet;
  bool_t    bUseNextSectorForOverflow;
} ZPS_tsAfFlashInfoSet;

typedef struct
{
    ZPS_tsAfZgpGpstEntry* psGpSecTable;
    uint8 u8Size;
}ZPS_tsAfZgpGpst;

typedef struct
{
    ZPS_tsAfZgpTxGpQueueEntry* psTxQTable;
    uint8 u8Size;
}ZPS_tsAfZgpTxGpQueue;

typedef struct
{
    ZPS_tsAfZgpGpst  *psGpst;
    ZPS_tsAfZgpTxGpQueue *psTxQueue;
    ZPS_tsTsvTimer *psTxAgingTimer;
    TSV_Timer_s *psTxBiDirTimer;
    uint16 u16MsecInterval;
    uint8 u8TxPoint;
}ZPS_tsAfZgpGreenPowerContext;

typedef struct
{
    ZPS_tuAfZgpGreenPowerId uGpId;
    uint32 u32FrameCounter;
    uint32 u32Mic;
    uint8 u8SecurityLevel;
    uint8 u8KeyType;
    uint8 u8ApplicationId;
    uint8 u8Endpoint;
}ZPS_tsAfZgpSecReq;

typedef uint8 (*pfnAfZgpType) (ZPS_tsAfZgpSecReq* psSec,uint8 *pu8SecIndex);


struct pdum_tsAPduInstance_tag {
    uint8 *au8Storage;
    uint16 u16Size;
    uint16 u16NextAPduInstIdx;
    uint16 u16APduIdx;
};

typedef struct
{
    uint64 u64Address;
    uint16 u16NwkAddress;
    uint8  u8Flags;
} zps_tsPersistNeighboursTable;

typedef struct
{
    uint16 u16ShortAddress;
    uint64 u64ExtendedAddress;
} zps_tsPersistAddressMap;

typedef struct
{
    uint64 u64ExtAddr;
    uint32 u32OutgoingFrameCounter;
    uint8  au8LinkKey[ZPS_SEC_KEY_LENGTH];
} zps_tsPersistKeyTableMap;

typedef struct
{

    ZPS_tsNWkNibPersist sPersist;
    uint64 u64TcAddress;
    zps_tsPersistNeighboursTable *psActvNtEntry;
    ZPS_tsNwkSecMaterialSet *psSecMatSet;
    zps_tsPersistKeyTableMap *psAplApsKeyDescriptorEntry;
    zps_tsPersistKeyTableMap *psAplDefaultApsKeyDescriptorEntry;
    zps_tsPersistAddressMap *pRwAddressMap;
    uint32  u32OutgoingFrameCounter;
    uint16  u16NtTable;
    uint16  u16AddressLkmp;
    uint16  u16KeyDescTableSize;
    bool_t  bEndDeviceOnly;
    bool_t  bRecoverJoiner;
    uint8   u8KeyType;

} ZPS_tsAfRestorePointStruct;


typedef struct {
    uint64 u64ExtPanId;
    uint8 *pu8NwkKey;
    uint16 u16PanId;
    uint16 u16NwkAddr;
    uint8 u8KeyIndex;
    uint8 u8LogicalChannel;
    uint8 u8NwkupdateId;
} ZPS_tsAftsStartParamsDistributed;


typedef struct {
    uint64 u64UnauthenticatedChild;
    uint64 u64AuthenticatedParent;
    uint16 u16ShortAddressOfChild;
} ZPS_tsAftsAppAuthenticationTableType;
typedef struct pdum_tsAPduInstance_tag pdum_tsAPduInstance;

#define APDU_BUF(HANDLE, LOCATION)  ((pdum_tsAPduInstance*)HANDLE)->au8Storage[LOCATION]
#define APDU_BUF_INC(HANDLE, LOCATION)  ((pdum_tsAPduInstance*)HANDLE)->au8Storage[LOCATION++]

#define APDU_BUF_WRITE16(VAL, HANDLE, LOCATION) vWrite16Nbo(VAL, &APDU_BUF(HANDLE, LOCATION))
#define APDU_BUF_WRITE16_INC(VAL, HANDLE, LOCATION) vWrite16Nbo(VAL, &APDU_BUF(HANDLE, LOCATION)); LOCATION += 2

#define APDU_BUF_READ16(VAR, HANDLE, LOCATION) VAR = u16Read16Nbo( &APDU_BUF(HANDLE, LOCATION) )
#define APDU_BUF_READ16_INC(VAR, HANDLE, LOCATION) VAR = u16Read16Nbo( &APDU_BUF(HANDLE, LOCATION) ); LOCATION += 2


typedef bool (*pfTransportKeyDecider)(uint16 u16ShortAddress,
                                      uint64 u64DeviceAddress,
                                      uint64 u64ParentAddress,
                                      uint8 u8Status,
                                      uint16 u16Interface);


typedef void (*pfCallbackHashNotification)(uint64 u64DeviceAddress);
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Private Functions (must not be called by ZPS user)     */
/****************************************************************************/
PUBLIC void zps_taskZPS(void);
PUBLIC void zps_pvAesGetKeyFromInstallCode(uint8 *pu8installCode,
                    uint16 u16installCodeLength,
                    AESSW_Block_u *puresult);
PUBLIC uint32 zps_u32GetAplVersion(void);
PUBLIC ZPS_teStatus zps_eAplAfInit (void* pvApl, bool_t bColdInit );
PUBLIC ZPS_teStatus zps_eAplAfSetEndpointState(void *pvApl, uint8 u8Endpoint, bool bEnabled);
PUBLIC ZPS_teStatus zps_eAplAfGetEndpointState(void *pvApl, uint8 u8Endpoint, bool *pbEnabled );
PUBLIC ZPS_teStatus zps_eAplAfSetEndpointDiscovery(void *pvApl, uint8 u8Endpoint, uint16 u16ClusterId, bool bOutput, bool bDiscoverable);
PUBLIC ZPS_teStatus zps_eAplAfGetEndpointDiscovery(void *pvApl, uint8 u8Endpoint, uint16 u16ClusterId, bool bOutput, bool_t *pbDiscoverable);
PUBLIC ZPS_teStatus zps_eAplAfGetNodeDescriptor(void *pvApl, ZPS_tsAplAfNodeDescriptor *psDesc);
PUBLIC ZPS_teStatus zps_eAplAfGetNodePowerDescriptor(void *pvApl, ZPS_tsAplAfNodePowerDescriptor *psDesc);
PUBLIC ZPS_teStatus zps_eAplAfGetSimpleDescriptor(void *pvApl, uint8 u8Endpoint, ZPS_tsAplAfSimpleDescriptor *psDesc);
PUBLIC ZPS_teStatus zps_eAplAfUnicastDataReq(void *pvApl, PDUM_thAPduInstance hAPduInst, uint32 u32ClId_DstEp_SrcEp, uint16 u16DestAddr,
                                             uint16 u16SecMd_Radius, uint8 *pu8SeqNum);
PUBLIC ZPS_teStatus zps_eAplAfUnicastIeeeDataReq(void *pvApl, PDUM_thAPduInstance hAPduInst, uint32 u32ClId_DstEp_SrcEp, uint64 *pu64DestAddr,
                                                 uint16 u16SecMd_Radius, uint8 *pu8SeqNum);
PUBLIC ZPS_teStatus zps_eAplAfUnicastAckDataReq(void *pvApl, PDUM_thAPduInstance hAPduInst, uint32 u32ClId_DstEp_SrcEp, uint16 u16DestAddr,
                                                uint16 u16SecMd_Radius, uint8 *pu8SeqNum);
PUBLIC ZPS_teStatus zps_eAplAfUnicastIeeeAckDataReq(void *pvApl, PDUM_thAPduInstance hAPduInst, uint32 u32ClId_DstEp_SrcEp, uint64 *pu64DestAddr,
                                                    uint16 u16SecMd_Radius, uint8 *pu8SeqNum);
PUBLIC ZPS_teStatus zps_eAplAfGroupDataReq(void *pvApl, PDUM_thAPduInstance hAPduInst, uint32 u32ClId_SrcEp, uint16 u16DstGroupAddr,
                                           uint16 u16SecMd_Radius, uint8 *pu8SeqNum);
PUBLIC ZPS_teStatus zps_eAplAfBroadcastDataReq(void *pvApl, PDUM_thAPduInstance hAPduInst, uint32 u32ClId_DstEp_SrcEp,
                                               ZPS_teAplAfBroadcastMode eBroadcastMode, uint16 u16SecMd_Radius, uint8 *pu8SeqNum);
PUBLIC ZPS_teStatus zps_eAplAfBoundDataReq(void *pvApl, PDUM_thAPduInstance hAPduInst, uint32 u32ClId_SrcEp, uint16 u16SecMd_Radius,
                                           uint8* pu8SeqNum);
PUBLIC ZPS_teStatus zps_eAplAfBoundAckDataReq(void *pvApl, PDUM_thAPduInstance hAPduInst, uint32 u32ClId_SrcEp, uint16 u16SecMd_Radius,
                                              uint8* pu8SeqNum);
PUBLIC ZPS_teStatus zps_eAplAfInterPanDataReq(void *pvApl,PDUM_thAPduInstance hAPduInst,uint32 u32ClId_ProfId,ZPS_tsInterPanAddress *psDstAddr,
                                              uint8 u8Handle);
PUBLIC ZPS_teStatus zps_eAplAfApsdeDataReq(void *pvApl, PDUM_thAPduInstance hAPduInst, ZPS_tsAfProfileDataReq* psProfileDataReq, uint8 *pu8SeqNum, uint8 eTxOptions);
PUBLIC void zps_vAfInterPanInit(void *pvApl);
PUBLIC void ZPS_vZgpInitGpTxQueue(void);
PUBLIC void ZPS_vZgpInitGpSecurityTable(void);
PUBLIC void zps_vRegisterGreenPower(void *pvApl);
PUBLIC void zps_vSetIgnoreProfileCheck(void);
PUBLIC ZPS_teStatus zps_ePurgeBindTable(void *pvApl);
PUBLIC void zps_vPurgeAddressMap(void* pvApl);
PUBLIC bool_t ZPS_sZgpAddDeviceSecurity(uint32 u32ApplicationId, ZPS_tsAfZgpGpstEntry *psEntry);
PUBLIC void ZPS_vRegisterSecRequestResponseCallback(void* pFn);
PUBLIC void ZPS_vZgpTransformKey(ZPS_teAfZgpKeyType eKeyType, uint8 u8Applicationid, uint32 u32Srcid, uint64 u64MacAddress, AESSW_Block_u *puInKey,
                             AESSW_Block_u *puOutKey);
PUBLIC ZPS_teStatus zps_vDStub_DataReq(ZPS_tsAfZgpGreenPowerReq *psDataReq,PDUM_thAPduInstance hAPduInst,bool_t bActions,uint8 u8CommandId);
PUBLIC ZPS_tsAfZgpGpstEntry* ZPS_psZgpFindGpstEntry(uint32 u32ApplicationId,ZPS_tuAfZgpGreenPowerId uGreenPowerId, uint8* pu8Index);

PUBLIC uint8 u8ZgpCCMStarEncrypt(uint8 *pu8Payload,
        uint8 u8PayloadLen,
        uint8 u8ApplicationId,
        uint16 u16NwkHdr,
        ZPS_tsAfZgpGpstEntry* psSec,
        uint16 u16Panid,
        uint16 u16DstAddr,
        uint8 u8Endpoint);

PUBLIC uint8 u8ZgpCCMStarDecrypt(
        AESSW_Block_u *puSecurityKey,
        uint8 *pu8Payload,
        uint8 u8PayloadLen,
        uint8 u8NwkHdr,
        uint8         u8ExtNwkHdr,
        uint16 u16Panid,
        uint16 u16DstAddr,
        ZPS_tsAfZgpSecReq* pZgpReq);

PUBLIC void zps_vSaveAllZpsRecords(void *pvApl);
PUBLIC bool_t ZPS_bIsLinkKeyPresent(uint64 u64IeeeAddress);
PUBLIC ZPS_teStatus zps_eAplAfSendKeepAlive(void* pvApl );
PUBLIC bool zps_bAplAfSetEndDeviceTimeout(void* pvApl, uint8 u8Timeout );

PUBLIC ZPS_tsAplApsKeyDescriptorEntry *
zps_psFindKeyDescr(void *pvApl,  uint64 u64DeviceAddr,  uint32* pu32Index);

PUBLIC void ZPS_vSetTCLockDownOverride (void* pvApl, bool_t u8RemoteOverride, bool_t bDisableAuthentications );

PUBLIC ZPS_teStatus zps_eAfConfirmKeyReqRsp (void* pvApl,  uint8 u8Status,  uint64 u64DstAddr,  uint8 u8KeyType);

PUBLIC ZPS_teStatus zps_eAfVerifyKeyReqRsp ( void* pvApl,  uint64 u64DstAddr,  uint8 u8KeyType);
PUBLIC void ZPS_vSecondTimerCallback (void);

PUBLIC void vWrite16Nbo(uint16 u16Word, uint8 *pu8Payload);
PUBLIC void vWrite32Nbo(uint32 u32Word, uint8 *pu8Payload);
PUBLIC void vWrite64Nbo(uint64 u64dWord, uint8 *pu8Payload);

PUBLIC uint16 u16Read16Nbo(uint8 *pu8Payload);

PUBLIC void ZPS_vAplZdoRegisterProfileCallback(void* fnPtr);
PUBLIC void ZPS_vAplZdoRegisterInterPanFilter(void* fnPtr);
PUBLIC void ZPS_vGetRestorePoint(ZPS_tsAfRestorePointStruct *psStruct);
PUBLIC void ZPS_vSetRestorePoint(ZPS_tsAfRestorePointStruct *psStruct);
PUBLIC void ZPS_vSetJoinKeyExchangeTimeoutPeriod(uint32 u32TimeoutInSeconds);
PUBLIC void ZPS_vSetFixedNwkKey(uint8 *pu8Key, uint8 u8KeySeqNumb);
PUBLIC void zps_vDefaultStack (void *pvApl);
PUBLIC ZPS_teStatus zps_eAplZdoStartRouter(void *pvApl, bool_t bDeviceAnnounce);
PUBLIC uint8 ZPS_u8AplGetMaxPayloadSize(void *pvApl , uint16 u16Addr);
PUBLIC bool_t ZPS_bAplDoesDeviceSupportFragmentation(void *pvApl);
PUBLIC ZPS_teStatus zps_eAplInitEndDeviceDistributed(
    void *pvApl ,
    ZPS_tsAftsStartParamsDistributed *psStartParms);
PUBLIC ZPS_teStatus zps_eAplFormDistributedNetworkRouter(
    void *pvApl ,
    ZPS_tsAftsStartParamsDistributed *psStartParms,
    bool_t bSendDevice);
PUBLIC  ZPS_tsNwkNetworkDescr* ZPS_psGetNetworkDescriptors (
                                           uint8 *pu8NumberOfNetworks);
PUBLIC void ZPS_vTCSetCallback(void*);
PUBLIC void ZPS_vSetOrphanUpdateDisable ( bool_t bEnableOverride );
PUBLIC void ZPS_vAplAfEnableMcpsFilter ( uint8 u8McpsFilterEnable, uint8 u8ZpsDefaultFilterValue);
PUBLIC void ZPS_vRunningCRC32 ( uint8    u8Byte, uint32 *pu32Result );
PUBLIC void ZPS_vFinalCrc32 (uint32 *pu32Result);
PUBLIC void zps_vAplAfSetLocalEndDeviceTimeout( void* pvApl, uint16 u16Short, uint8 u8Timeout);
PUBLIC bool_t zps_bZgpRemoveTxQueue(ZPS_tsAfZgpTxGpQueueEntry *psEntry);
PUBLIC void ZPS_vNMPurgeEntry (uint64 u64DeviceAddr);
PUBLIC void zps_vAplAfSetMacCapability( void* pvApl, uint8 u8MacCapability);
PUBLIC void ZPS_vTcInitFlash( ZPS_tsAfFlashInfoSet*       psFlashInfoSet ,
                              ZPS_TclkDescriptorEntry*    psTclkStruct );
PUBLIC uint64 ZPS_u64GetFlashMappedIeeeAddress ( uint16    u16Location );
PUBLIC  ZPS_teStatus zps_eAplAfBoundDataReqNonBlocking( void                  *pvApl,
                                               PDUM_thAPduInstance    hAPduInst,
                                               uint32                 u32ClId_SrcEp,
                                               uint16                 u16SecMd_Radius,
                                               bool                   bAckReq );
PUBLIC uint16 ZPS_u16crc(uint8* pu8data, uint16 u16length);
PUBLIC void ZPS_vSetKeys (void);
PUBLIC ZPS_tsAplAfNodeDescriptor * zps_psGetLocalNodeDescriptor (void *pvApl);
PUBLIC ZPS_tsAplAfSimpleDescriptor* zps_psGetSimpleDescriptor (void *pvApl, uint8 u8Index);
PUBLIC uint8 zps_u8GetLocalScanDuration (void *pvApl);
PUBLIC ZPS_tsAplAfNodePowerDescriptor* zps_psGetLocalPowerDescriptor (void *pvApl);
PUBLIC void zps_vSetLocalPollInterval (void *pvApl, uint16 u16PollInterval);
PUBLIC void zps_vSetLocalMaxNumPollFailures (void *pvApl, uint16 u8PollFailure);
PUBLIC void zps_vSetLocalRxOnWhenIdle (void *pvApl, bool bSet);
PUBLIC void zps_vSetLocalScanDuration (void *pvApl, uint8 u8ScanDuration);
PUBLIC bool_t zps_bIsFragmentationEngineActive (void *pvApl);
PUBLIC void ZPS_vApplicationAuthenticationEnable( ZPS_tsAftsAppAuthenticationTableType* pasAppAuthenticateTable,
                                                  uint8 u8TableSize );
PUBLIC void ZPS_vAllowJoinerNetworkAccess ( uint64 u64JoinersAddress );

#ifdef WWAH_SUPPORT
PUBLIC void ZPS_vAplSetResolutionChannel ( uint8 u8Channel );
PUBLIC uint8 ZPS_u8AplGetResolutionChannel ( void );
#endif

extern bool_t bSetTclkFlashFeature;
/****************************************************************************/
/***        In-line Functions                                            ***/
/****************************************************************************/

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfInit(void) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfInit(void)
{
    return zps_eAplAfInit(ZPS_pvAplZdoGetAplHandle(),TRUE);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfReInit(void) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfReInit(void)
{
    return zps_eAplAfInit(ZPS_pvAplZdoGetAplHandle(),FALSE);
}

ZPS_APL_INLINE bool_t ZPS_bIsFragmentationEngineActive(void) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE bool_t ZPS_bIsFragmentationEngineActive(void)
{
    return zps_bIsFragmentationEngineActive(ZPS_pvAplZdoGetAplHandle());
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfApsdeDataReq(PDUM_thAPduInstance hAPduInst, ZPS_tsAfProfileDataReq* psProfileDataReq, uint8 *pu8SeqNum) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfApsdeDataReq(PDUM_thAPduInstance hAPduInst, ZPS_tsAfProfileDataReq* psProfileDataReq, uint8 *pu8SeqNum)
{
    return zps_eAplAfApsdeDataReq(ZPS_pvAplZdoGetAplHandle(), hAPduInst, psProfileDataReq, pu8SeqNum,0);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfFullApsdeDataReq(PDUM_thAPduInstance hAPduInst, ZPS_tsAfProfileDataReq* psProfileDataReq, uint8 *pu8SeqNum, uint8 u8TxOptions) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfFullApsdeDataReq(PDUM_thAPduInstance hAPduInst, ZPS_tsAfProfileDataReq* psProfileDataReq, uint8 *pu8SeqNum, uint8 u8TxOptions)
{
    return zps_eAplAfApsdeDataReq(ZPS_pvAplZdoGetAplHandle(), hAPduInst, psProfileDataReq, pu8SeqNum, u8TxOptions);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfSetEndpointState(uint8 u8Endpoint, bool bEnabled) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfSetEndpointState(uint8 u8Endpoint, bool bEnabled)
{
    return zps_eAplAfSetEndpointState(ZPS_pvAplZdoGetAplHandle(), u8Endpoint, bEnabled);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfGetEndpointState(uint8 u8Endpoint, bool *pbEnabled ) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfGetEndpointState(uint8 u8Endpoint, bool *pbEnabled )
{
    return zps_eAplAfGetEndpointState(ZPS_pvAplZdoGetAplHandle(), u8Endpoint, pbEnabled);
}

ZPS_APL_INLINE void ZPS_vAplAfSetMacCapability(uint8 u8MacCapability) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE void ZPS_vAplAfSetMacCapability(uint8 u8MacCapability)
{
    zps_vAplAfSetMacCapability(ZPS_pvAplZdoGetAplHandle(), u8MacCapability);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfSetEndpointDiscovery(uint8 u8Endpoint, uint16 u16ClusterId, bool bOutput, bool bDiscoverable) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfSetEndpointDiscovery(uint8 u8Endpoint, uint16 u16ClusterId, bool bOutput, bool bDiscoverable)
{
    return zps_eAplAfSetEndpointDiscovery(ZPS_pvAplZdoGetAplHandle(), u8Endpoint, u16ClusterId, bOutput, bDiscoverable);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfGetEndpointDiscovery(uint8 u8Endpoint, uint16 u16ClusterId, bool bOutput, bool_t *pbDiscoverable) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfGetEndpointDiscovery(uint8 u8Endpoint, uint16 u16ClusterId, bool bOutput, bool_t *pbDiscoverable)
{
    return zps_eAplAfGetEndpointDiscovery(ZPS_pvAplZdoGetAplHandle(), u8Endpoint, u16ClusterId, bOutput, pbDiscoverable);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfGetNodeDescriptor(ZPS_tsAplAfNodeDescriptor *psDesc) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfGetNodeDescriptor(ZPS_tsAplAfNodeDescriptor *psDesc)
{
    return zps_eAplAfGetNodeDescriptor(ZPS_pvAplZdoGetAplHandle(), psDesc);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfGetNodePowerDescriptor(ZPS_tsAplAfNodePowerDescriptor *psDesc) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfGetNodePowerDescriptor(ZPS_tsAplAfNodePowerDescriptor *psDesc)
{
    return zps_eAplAfGetNodePowerDescriptor(ZPS_pvAplZdoGetAplHandle(), psDesc);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfGetSimpleDescriptor(uint8 u8Endpoint, ZPS_tsAplAfSimpleDescriptor *psDesc) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfGetSimpleDescriptor(uint8 u8Endpoint, ZPS_tsAplAfSimpleDescriptor *psDesc)
{
    return zps_eAplAfGetSimpleDescriptor(ZPS_pvAplZdoGetAplHandle(), u8Endpoint, psDesc);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfUnicastDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, uint8 u8DstEndpoint, uint16 u16DestAddr, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8 *pu8SeqNum) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfUnicastDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, uint8 u8DstEndpoint, uint16 u16DestAddr, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8 *pu8SeqNum)
{
    return zps_eAplAfUnicastDataReq(ZPS_pvAplZdoGetAplHandle(), hAPduInst, (u16ClusterId << 16) | (u8DstEndpoint << 8) | u8SrcEndpoint, u16DestAddr, (eSecurityMode << 8) | u8Radius, pu8SeqNum);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfUnicastIeeeDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, uint8 u8DstEndpoint, uint64 u64DestAddr, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8 *pu8SeqNum) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfUnicastIeeeDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, uint8 u8DstEndpoint, uint64 u64DestAddr, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8 *pu8SeqNum)
{
    return zps_eAplAfUnicastIeeeDataReq(ZPS_pvAplZdoGetAplHandle(), hAPduInst, (u16ClusterId << 16) | (u8DstEndpoint << 8) | u8SrcEndpoint, &u64DestAddr,  (eSecurityMode << 8) | u8Radius, pu8SeqNum);
}

/* PR #236 http://trac/Zigbee-PRO/ticket/236 - corrected parameter order */
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfUnicastAckDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, uint8 u8DstEndpoint, uint16 u16DestAddr, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8 *pu8SeqNum) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfUnicastAckDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, uint8 u8DstEndpoint, uint16 u16DestAddr, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8 *pu8SeqNum)
{
    return zps_eAplAfUnicastAckDataReq(ZPS_pvAplZdoGetAplHandle(), hAPduInst, (u16ClusterId << 16) | (u8DstEndpoint << 8) | u8SrcEndpoint, u16DestAddr, (eSecurityMode << 8) | u8Radius, pu8SeqNum);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfUnicastIeeeAckDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, uint8 u8DstEndpoint, uint64 u64DestAddr, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8 *pu8SeqNum) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfUnicastIeeeAckDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, uint8 u8DstEndpoint, uint64 u64DestAddr, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8 *pu8SeqNum)
{
    return zps_eAplAfUnicastIeeeAckDataReq(ZPS_pvAplZdoGetAplHandle(), hAPduInst, (u16ClusterId << 16) | (u8DstEndpoint << 8) | u8SrcEndpoint, &u64DestAddr, (eSecurityMode << 8) | u8Radius, pu8SeqNum);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfGroupDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, uint16 u16DstGroupAddr, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8 *pu8SeqNum) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfGroupDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, uint16 u16DstGroupAddr, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8 *pu8SeqNum)
{
    return zps_eAplAfGroupDataReq(ZPS_pvAplZdoGetAplHandle(), hAPduInst, (u16ClusterId << 16) | u8SrcEndpoint,  u16DstGroupAddr, (eSecurityMode << 8) | u8Radius, pu8SeqNum);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfBroadcastDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, uint8 u8DstEndpoint, ZPS_teAplAfBroadcastMode eBroadcastMode, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8 *pu8SeqNum) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfBroadcastDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, uint8 u8DstEndpoint, ZPS_teAplAfBroadcastMode eBroadcastMode, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8 *pu8SeqNum)
{
    return zps_eAplAfBroadcastDataReq(ZPS_pvAplZdoGetAplHandle(), hAPduInst, (u16ClusterId << 16) | (u8DstEndpoint << 8) | u8SrcEndpoint, eBroadcastMode, (eSecurityMode << 8) | u8Radius, pu8SeqNum);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfBoundDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8* pu8SeqNum) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfBoundDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8* pu8SeqNum)
{
    return zps_eAplAfBoundDataReq(ZPS_pvAplZdoGetAplHandle(), hAPduInst, (u16ClusterId << 16) | u8SrcEndpoint, (eSecurityMode << 8) | u8Radius, pu8SeqNum);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfBoundAckDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8* pu8SeqNum) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfBoundAckDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, uint8* pu8SeqNum)
{
    return zps_eAplAfBoundAckDataReq(ZPS_pvAplZdoGetAplHandle(), hAPduInst, (u16ClusterId << 16) | u8SrcEndpoint, (eSecurityMode << 8) | u8Radius, pu8SeqNum);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfInterPanDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint16 u16ProfileId,
                                        ZPS_tsInterPanAddress *psDstAddr, uint8 u8Handle) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfInterPanDataReq(PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint16 u16ProfileId, ZPS_tsInterPanAddress *psDstAddr, uint8 u8Handle)
{
    return zps_eAplAfInterPanDataReq(ZPS_pvAplZdoGetAplHandle(),
                                     hAPduInst,
                                     (u16ClusterId << 16) | u16ProfileId,
                                     psDstAddr,
                                     u8Handle);
}

ZPS_APL_INLINE void ZPS_vAfInterPanInit(void) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE void ZPS_vAfInterPanInit(void)
{
    zps_vAfInterPanInit(ZPS_pvAplZdoGetAplHandle());
}


ZPS_APL_INLINE void ZPS_vAfSetZgpAlias(uint16 u16AliasShortAddress, uint8 u8AliasSeq, uint8 u8AliasApsCounter) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE void ZPS_vAfSetZgpAlias(uint16 u16AliasShortAddress, uint8 u8AliasSeq, uint8 u8AliasApsCounter)
{
    zps_vNwkSetZgpAlias(ZPS_pvAplZdoGetNwkHandle(),u16AliasShortAddress,u8AliasSeq, u8AliasApsCounter);
}


ZPS_APL_INLINE void ZPS_vAfZgpDeviceActive(uint16 u16Address) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE void ZPS_vAfZgpDeviceActive(uint16 u16Address)
{
    zps_vZgpDeviceActive(ZPS_pvAplZdoGetNwkHandle(),u16Address);
}

ZPS_APL_INLINE void ZPS_vRegisterGreenPower(void) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE void ZPS_vRegisterGreenPower(void)
{
    zps_vRegisterGreenPower(ZPS_pvAplZdoGetAplHandle());
}


ZPS_APL_INLINE void ZPS_vSetIgnoreProfileCheck(void) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE void ZPS_vSetIgnoreProfileCheck(void)
{
    zps_vSetIgnoreProfileCheck();
}
ZPS_APL_INLINE bool_t ZPS_bZgpRemoveTxQueue (ZPS_tsAfZgpTxGpQueueEntry *psEntry) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE bool_t ZPS_bZgpRemoveTxQueue (ZPS_tsAfZgpTxGpQueueEntry *psEntry)
{
    return zps_bZgpRemoveTxQueue(psEntry);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_ePurgeBindTable(void) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_ePurgeBindTable(void)
{
    return zps_ePurgeBindTable(ZPS_pvAplZdoGetAplHandle());
}

ZPS_APL_INLINE void ZPS_vPurgeAddressMap(void) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE void ZPS_vPurgeAddressMap(void)
{
    zps_vPurgeAddressMap(ZPS_pvAplZdoGetAplHandle());
}

ZPS_APL_INLINE void ZPS_vSaveAllZpsRecords(void) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE void ZPS_vSaveAllZpsRecords(void)
{
    zps_vSaveAllZpsRecords(ZPS_pvAplZdoGetAplHandle());
}


ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfSendKeepAlive(void) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfSendKeepAlive(void)
{
    return zps_eAplAfSendKeepAlive(ZPS_pvAplZdoGetAplHandle() );
}

ZPS_APL_INLINE bool ZPS_bAplAfSetEndDeviceTimeout(uint8 u8Timeout ) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE bool ZPS_bAplAfSetEndDeviceTimeout(uint8 u8Timeout )
{
    return zps_bAplAfSetEndDeviceTimeout( ZPS_pvAplZdoGetAplHandle(), u8Timeout);
}

ZPS_APL_INLINE void ZPS_vAplAfSetLocalEndDeviceTimeout(uint16 u16Short , uint8 u8Timeout ) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE void ZPS_vAplAfSetLocalEndDeviceTimeout(uint16 u16Short , uint8 u8Timeout )
{
    zps_vAplAfSetLocalEndDeviceTimeout( ZPS_pvAplZdoGetAplHandle(), u16Short, u8Timeout);
}
ZPS_APL_INLINE ZPS_tsAplApsKeyDescriptorEntry* ZPS_psGetActiveKey(uint64 u64IeeeAddress, uint32* pu32Index) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_tsAplApsKeyDescriptorEntry* ZPS_psGetActiveKey(uint64 u64IeeeAddress, uint32* pu32Index)
{
    return zps_psFindKeyDescr(ZPS_pvAplZdoGetAplHandle(), u64IeeeAddress, pu32Index);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAfConfirmKeyReqRsp( uint8 u8Status,  uint64 u64DstAddr,   uint8 u8KeyType) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAfConfirmKeyReqRsp( uint8 u8Status, uint64 u64DstAddr,   uint8 u8KeyType)
{
    return zps_eAfConfirmKeyReqRsp(ZPS_pvAplZdoGetAplHandle(), u8Status, u64DstAddr, u8KeyType);
}


ZPS_APL_INLINE ZPS_teStatus ZPS_eAfVerifyKeyReqRsp( uint64 u64DstAddr,   uint8 u8KeyType) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAfVerifyKeyReqRsp( uint64 u64DstAddr,   uint8 u8KeyType)
{
    return zps_eAfVerifyKeyReqRsp(ZPS_pvAplZdoGetAplHandle(),  u64DstAddr, u8KeyType);
}


ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdoStartRouter( bool_t bDeviceAnnounce ) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdoStartRouter( bool_t bDeviceAnnounce )
{
    return zps_eAplZdoStartRouter(ZPS_pvAplZdoGetAplHandle(),  bDeviceAnnounce);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplFormDistributedNetworkRouter( ZPS_tsAftsStartParamsDistributed *psStartParms , bool_t bSendDeviceAnnce) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplFormDistributedNetworkRouter( ZPS_tsAftsStartParamsDistributed *psStartParms , bool_t bSendDeviceAnnce)
{
    return zps_eAplFormDistributedNetworkRouter(ZPS_pvAplZdoGetAplHandle(),  psStartParms, bSendDeviceAnnce);
}


ZPS_APL_INLINE ZPS_teStatus ZPS_eAplInitEndDeviceDistributed( ZPS_tsAftsStartParamsDistributed *psStartParms ) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplInitEndDeviceDistributed( ZPS_tsAftsStartParamsDistributed *psStartParms )
{
    return zps_eAplInitEndDeviceDistributed(ZPS_pvAplZdoGetAplHandle(),  psStartParms);
}

ZPS_APL_INLINE void ZPS_vDefaultStack(void ) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE void ZPS_vDefaultStack( void )
{
    zps_vDefaultStack(ZPS_pvAplZdoGetAplHandle());
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfBoundDataReqNonBlocking (PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, bool bAck ) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplAfBoundDataReqNonBlocking (PDUM_thAPduInstance hAPduInst, uint16 u16ClusterId, uint8 u8SrcEndpoint, ZPS_teAplAfSecurityMode eSecurityMode, uint8 u8Radius, bool bAck )
{
    return zps_eAplAfBoundDataReqNonBlocking( ZPS_pvAplZdoGetAplHandle(),
                                     hAPduInst,
                                     (u16ClusterId << 16) | u8SrcEndpoint,
                                     (eSecurityMode << 8) | u8Radius,
                                     bAck );
}

ZPS_APL_INLINE ZPS_tsAplAfNodeDescriptor * ZPS_psGetLocalNodeDescriptor(void) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_tsAplAfNodeDescriptor * ZPS_psGetLocalNodeDescriptor(void)
{
    return zps_psGetLocalNodeDescriptor(ZPS_pvAplZdoGetAplHandle());
}

ZPS_APL_INLINE ZPS_tsAplAfSimpleDescriptor* ZPS_psGetSimpleDescriptor (uint8 u8Index) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_tsAplAfSimpleDescriptor* ZPS_psGetSimpleDescriptor (uint8 u8Index)
{
    return zps_psGetSimpleDescriptor(ZPS_pvAplZdoGetAplHandle(), u8Index);
}


ZPS_APL_INLINE uint8 ZPS_u8GetLocalScanDuration (void) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE uint8 ZPS_u8GetLocalScanDuration (void)
{
    return zps_u8GetLocalScanDuration(ZPS_pvAplZdoGetAplHandle());
}

ZPS_APL_INLINE ZPS_tsAplAfNodePowerDescriptor* ZPS_psGetLocalPowerDescriptor(void) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_tsAplAfNodePowerDescriptor* ZPS_psGetLocalPowerDescriptor(void)
{
    return zps_psGetLocalPowerDescriptor(ZPS_pvAplZdoGetAplHandle());
}


ZPS_APL_INLINE void ZPS_vSetLocalPollInterval(uint16 u16PollInterval) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE void ZPS_vSetLocalPollInterval(uint16 u16PollInterval)
{
    zps_vSetLocalPollInterval(ZPS_pvAplZdoGetAplHandle(), u16PollInterval);
}


ZPS_APL_INLINE void ZPS_vSetLocalMaxNumPollFailures(uint16 u8PollFailure) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE void ZPS_vSetLocalMaxNumPollFailures(uint16 u8PollFailure)
{
    zps_vSetLocalMaxNumPollFailures(ZPS_pvAplZdoGetAplHandle(), u8PollFailure);
}

ZPS_APL_INLINE void ZPS_vSetLocalRxOnWhenIdle(bool bSet) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE void ZPS_vSetLocalRxOnWhenIdle(bool bSet)
{
     zps_vSetLocalRxOnWhenIdle(ZPS_pvAplZdoGetAplHandle(),bSet);
}

ZPS_APL_INLINE void ZPS_vSetLocalScanDuration(uint8 u8ScanDuration) ZPS_APL_ALWAYS_INLINE;
ZPS_APL_INLINE void ZPS_vSetLocalScanDuration(uint8 u8ScanDuration)
{
    zps_vSetLocalScanDuration(ZPS_pvAplZdoGetAplHandle(),u8ScanDuration);
}

#ifdef ZCP
PUBLIC void ZPS_vRegisterCommandCallbackAPS(void* pvFnCallback);
PUBLIC void ZPS_vSetBufferCommandCallbackAPS(uint8 *pu8Buffer);
#endif

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#endif /*ZPS_APL_AF_H_*/
