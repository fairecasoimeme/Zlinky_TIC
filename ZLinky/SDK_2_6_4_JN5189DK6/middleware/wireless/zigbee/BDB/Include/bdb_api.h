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
# COMPONENT:   bdb_api.h
#
# DESCRIPTION: BDB(Base Device Behaviour) Interface
#              
#
################################################################################*/

#ifndef BDB_API_INCLUDED
#define BDB_API_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
#include "bdb_options.h"
#include "zps_apl_af.h"
#include "appZdpExtraction.h"
#include "appZpsBeaconHandler.h"
#include "zcl.h"
#include "ZQueue.h"
#include "ZTimer.h"
#include "zps_apl_zdo.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifdef DEBUG_BDB
    #define TRACE_BDB TRUE
#else
    #define TRACE_BDB FALSE
#endif

/* BDB Attribute bdbCommissioningMode fields */
#define BDB_COMMISSIONING_MODE_TOUCHLINK            (1 << 0)
#define BDB_COMMISSIONING_MODE_NWK_STEERING         (1 << 1)
#define BDB_COMMISSIONING_MODE_NWK_FORMATION        (1 << 2)
#define BDB_COMMISSIONING_MODE_FINDING_N_BINDING    (1 << 3)

/* BDB Attribute bdbNodeCommissioningCapability fields */
#define BDB_NODE_COMMISSIONING_CAPABILITY_NETWORK_STEERING      (0)
#define BDB_NODE_COMMISSIONING_CAPABILITY_NETWORK_FORMATION     (1)
#define BDB_NODE_COMMISSIONING_CAPABILITY_FINDING_N_BINDING     (2)
#define BDB_NODE_COMMISSIONING_CAPABILITY_TOUCHLINK             (3)

/* BDB Constants used by all nodes */
#ifndef BDBC_MAX_SAME_NETWORK_RETRY_ATTEMPTS
#define BDBC_MAX_SAME_NETWORK_RETRY_ATTEMPTS   (10)     /* bdbcMaxSameNetworkRetryAttempts */
#endif
#ifndef BDBC_MIN_COMMISSIONING_TIME
#define BDBC_MIN_COMMISSIONING_TIME            (180)    /* bdbcMinCommissioningTime */
#endif
#ifndef BDBC_REC_SAME_NETWORK_RETRY_ATTEMPTS
#define BDBC_REC_SAME_NETWORK_RETRY_ATTEMPTS   (3)      /* bdbcRecSameNetworkRetryAttempts */
#endif
#ifndef BDBC_TC_LINK_KEY_EXCHANGE_TIMEOUT
#define BDBC_TC_LINK_KEY_EXCHANGE_TIMEOUT      (5)      /* bdbcTCLinkKeyExchangeTimeout */
#endif

/* BDB Constants used by nodes supporting touchlink */
#ifndef BDBC_TL_INTERPAN_TRANS_ID_LIFETIME
#define BDBC_TL_INTERPAN_TRANS_ID_LIFETIME      (8)     /* bdbcTLInterPANTransIdLifetime */
#endif
#ifndef BDBC_TL_MIN_STARTUP_DELAY_TIME
#define BDBC_TL_MIN_STARTUP_DELAY_TIME          (2)     /* bdbcTLMinStartupDelayTime */
#endif
#ifndef BDBC_TL_PRIMARY_CHANNEL_SET
#define BDBC_TL_PRIMARY_CHANNEL_SET             (0x02108800)    /* bdbcTLPrimaryChannelSet */
#endif
#ifndef BDBC_TL_RX_WINDOW_DURATION
#define BDBC_TL_RX_WINDOW_DURATION              (5)     /* bdbcTLRxWindowDuration */
#endif
#ifndef BDBC_TL_SCAN_TIME_BASE_DURATION_MS
#define BDBC_TL_SCAN_TIME_BASE_DURATION_MS      (250)   /* bdbcTLScanTimeBaseDuration */
#endif
#ifndef BDBC_TL_SECONDARY_CHANNEL_SET
#define BDBC_TL_SECONDARY_CHANNEL_SET           (0x07fff800 ^ BDBC_TL_PRIMARY_CHANNEL_SET) /* bdbcTLSecondaryChannelSet */
#endif

/* BDB Attribute values */
#ifndef BDB_COMMISSIONING_GROUP_ID
#define BDB_COMMISSIONING_GROUP_ID              (0xFFFF)                /* bdbCommissioningGroupID */
#endif
#ifndef BDB_COMMISSIONING_MODE
#define BDB_COMMISSIONING_MODE                  (BDB_COMMISSIONING_MODE_TOUCHLINK |\
                                                 BDB_COMMISSIONING_MODE_NWK_STEERING|\
                                                 BDB_COMMISSIONING_MODE_NWK_FORMATION |\
                                                 BDB_COMMISSIONING_MODE_FINDING_N_BINDING)  /* bdbCommissioningMode */
#endif
#ifndef BDB_COMMISSIONING_STATUS
#define BDB_COMMISSIONING_STATUS                (0x00)                  /* bdbCommissioningStatus */
#endif
#ifndef BDB_JOINING_NODE_EUI64
#define BDB_JOINING_NODE_EUI64                  (0x0000000000000000)    /* bdbJoiningNodeEui64 */
#endif
                                                                        /* bdbJoiningNodeNewTCLinkKey */
#ifndef BDB_JOIN_USES_INSTALL_CODE_KEY
#define BDB_JOIN_USES_INSTALL_CODE_KEY          (FALSE)                 /* bdbJoinUsesInstallCodeKey */
#endif
                                                                        /* bdbNodeCommissioningCapability */
                                                                        /* bdbNodeIsOnANetwork */
#ifndef BDB_NODE_JOIN_LINK_KEY_TYPE
#define BDB_NODE_JOIN_LINK_KEY_TYPE             (0x00)                  /* bdbNodeJoinLinkKeyType */
#endif
#ifndef BDB_PRIMARY_CHANNEL_SET
#ifndef ENABLE_SUBG_IF
	#define BDB_PRIMARY_CHANNEL_SET                 (0x02108800)            /* bdbPrimaryChannelSet */
#else
	#define BDB_PRIMARY_CHANNEL_SET                 (0x07FFFFFF)            /* bdbPrimaryChannelSet */
#endif /* ENABLE_SUBG_IF */
#endif /* BDB_PRIMARY_CHANNEL_SET */
#ifndef BDB_SCAN_DURATION
#define BDB_SCAN_DURATION                       (0x04)                  /* bdbScanDuration */
#endif
#ifndef BDB_SECONDARY_CHANNEL_SET
#ifndef ENABLE_SUBG_IF
	#define BDB_SECONDARY_CHANNEL_SET               (0x07FFF800 ^ BDB_PRIMARY_CHANNEL_SET)  /* bdbSecondaryChannelSet */
#else
	#define BDB_SECONDARY_CHANNEL_SET               (0)  /* bdbSecondaryChannelSet */
#endif
#endif
#ifndef BDB_TC_LINK_KEY_EXCHANGE_ATTEMPTS
#define BDB_TC_LINK_KEY_EXCHANGE_ATTEMPTS       (0x00)                  /* bdbTCLinkKeyExchangeAttempts */
#endif
#ifndef BDB_TC_LINK_KEY_EXCHANGE_ATTEMPTS_MAX
#define BDB_TC_LINK_KEY_EXCHANGE_ATTEMPTS_MAX   (0x03)                  /* bdbTCLinkKeyExchangeAttemptsMax */
#endif
#ifndef BDB_TC_LINK_KEY_EXCHANGE_METHOD
#define BDB_TC_LINK_KEY_EXCHANGE_METHOD         (0x00)                  /* bdbTCLinkKeyExchangeMethod */
#endif
#ifndef BDB_TRUST_CENTER_NODE_JOIN_TIMEOUT
#define BDB_TRUST_CENTER_NODE_JOIN_TIMEOUT      (0x0F)                  /* bdbTrustCenterNodeJoinTimeout */
#endif
#ifndef BDB_TRUST_CENTER_REQUIRE_KEYEXCHANGE
#define BDB_TRUST_CENTER_REQUIRE_KEYEXCHANGE    (TRUE)                  /* bdbTrustCenterRequireKeyExchange */
#endif


/* BDB Attribute bdbNodeJoinLinkKeyType values */
#define DEFAULT_GLOBAL_TRUST_CENTER_LINK_KEY            (0x00)
#define DISTRIBUTED_SECURITY_GLOBAL_LINK_KEY            (0x01)
#define INSTALL_CODE_DERIVED_PRECONFIGURED_LINK_KEY     (0x02)
#define TOUCHLINK_PRECONFIGURED_LINK_KEY                (0x03)

/* BDB Attribute bdbTCLinkKeyExchangeMethod values */
#define APS_REQUEST_KEY                                 (0x00)
#define CERTIFICATE_BASED_KEY_EXCHANGE                  (0x01)

#ifndef RAND_CHANNEL
#define RAND_CHANNEL                                    (FALSE)
#endif

#ifndef MAX_IC_KEY_ADDR_PAIRS
#define MAX_IC_KEY_ADDR_PAIRS                           (2)
#endif

#ifndef RAND_DISTRIBUTED_NWK_KEY
#define RAND_DISTRIBUTED_NWK_KEY                        (TRUE)
#endif

/* BDB_ZTIMER_STORAGE - Application must reserve extra 
 * BDB_ZTIMER_STORAGE during ZTIMER_eInit function.
 */
#ifdef BDB_SUPPORT_NWK_STEERING
    #define BDB_INCLUDE_NS_TIMER        (1)
#else
    #define BDB_INCLUDE_NS_TIMER        (0)
#endif

#ifdef BDB_SUPPORT_FIND_AND_BIND_INITIATOR
    #define BDB_INCLUDE_FB_TIMER        (1)
#else
    #define BDB_INCLUDE_FB_TIMER        (0)
#endif

#ifdef BDB_SUPPORT_TOUCHLINK
    #define BDB_INCLUDE_TL_TIMER        (1)
#else
    #define BDB_INCLUDE_TL_TIMER        (0)
#endif

#define BDB_ZTIMER_STORAGE              (1 + BDB_INCLUDE_NS_TIMER + BDB_INCLUDE_FB_TIMER + BDB_INCLUDE_TL_TIMER)

#ifdef ENABLE_SUBG_IF
#define BDB_CHANNEL_MIN (0)
#define BDB_CHANNEL_MAX (26)

#define SUBG_PAGE_28	0xE0000000

#else
#define BDB_CHANNEL_MIN (11)
#define BDB_CHANNEL_MAX (26)
#endif


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Application specific Base Device States */
typedef enum
{
    E_STATE_BASE_INIT   = 0x00, /* Processes no stack events */
    E_STATE_BASE_SLAVE  = 0x01, /* Commissioinnig as triggerd by Application Framework */
    E_STATE_BASE_FAIL   = 0x02, /* Previous operation failed */
    E_STATE_BASE_ACTIVE = 0x03, /* Commissioining successful */
    E_NUM_STATES                /* Always keep it the last item */
} BDB_teState;

/* BDB Attribute bdbCommissioningStatus fields */
typedef enum
{
    E_BDB_COMMISSIONING_STATUS_SUCCESS,
    E_BDB_COMMISSIONING_STATUS_IN_PROGRESS,
    E_BDB_COMMISSIONING_STATUS_NOT_AA_CAPABLE,
    E_BDB_COMMISSIONING_STATUS_NO_NETWORK,
    E_BDB_COMMISSIONING_STATUS_FORMATION_FAILURE,
    E_BDB_COMMISSIONING_STATUS_NO_IDENTIFY_QUERY_RESPONSE,
    E_BDB_COMMISSIONING_STATUS_BINDING_TABLE_FULL,
    E_BDB_COMMISSIONING_STATUS_NO_SCAN_RESPONSE,
    E_BDB_COMMISSIONING_STATUS_NOT_PERMITTED,
    E_BDB_COMMISSIONING_STATUS_TCLK_EX_FAILURE
}BDB_teCommissioningStatus;

/* BDB attributes */
typedef struct
{
    uint16 u16bdbCommissioningGroupID;          /* bdbCommissioningGroupID */
    uint8  u8bdbCommissioningMode;              /* bdbCommissioningMode */
    BDB_teCommissioningStatus ebdbCommissioningStatus; /* bdbCommissioningStatus */
    uint64 u64bdbJoiningNodeEui64;              /* bdbJoiningNodeEui64 */
    uint8  au8bdbJoiningNodeNewTCLinkKey[16];   /* bdbJoiningNodeNewTCLinkKey */
    bool_t bbdbJoinUsesInstallCodeKey;          /* bdbJoinUsesInstallCodeKey */
    const uint8  u8bdbNodeCommissioningCapability;    /* bdbNodeCommissioningCapability */
    bool_t bbdbNodeIsOnANetwork;                /* bdbNodeIsOnANetwork */
    uint8  u8bdbNodeJoinLinkKeyType;            /* bdbNodeJoinLinkKeyType */
    uint32 u32bdbPrimaryChannelSet;             /* bdbPrimaryChannelSet */
    uint8  u8bdbScanDuration;                   /* bdbScanDuration */  // This is directly taken from ZPSConfig - Scan Duration Time
    uint32 u32bdbSecondaryChannelSet;           /* bdbSecondaryChannelSet */
    uint8  u8bdbTCLinkKeyExchangeAttempts;      /* bdbTCLinkKeyExchangeAttempts */
    uint8  u8bdbTCLinkKeyExchangeAttemptsMax;   /* bdbTCLinkKeyExchangeAttemptsMax */
    uint8  u8bdbTCLinkKeyExchangeMethod;        /* bdbTCLinkKeyExchangeMethod */
    uint8  u8bdbTrustCenterNodeJoinTimeout;     /* bdbTrustCenterNodeJoinTimeout */
    bool_t bbdbTrustCenterRequireKeyExchange;   /* bdbTrustCenterRequireKeyExchange */
    bool_t bTLStealNotAllowed;
    bool_t bLeaveRequested;
    bool_t bOutBandRejoinInprogress;
}BDB_tsAttrib;

typedef struct
{
    BDB_teState     eNewState;
    ZPS_tsAfEvent   *psAfEvent;
} BDB_tsStateUpdate;

/* Find And Bind Event Structure */
typedef struct{
    //teFB_Events         eEventType;
    uint8               u8InitiatorEp;
    uint8               u8TargetEp;
    uint16              u16TargetAddress;
    uint16              u16ProfileId;
    uint16              u16DeviceId;
    uint8               u8DeviceVersion;
    union {
        uint16          u16ClusterId;
        uint16          u16GroupId;
    }uEvent;
    ZPS_tsAfZdpEvent    *psAfZdpEvent;
    bool_t              bAllowBindOrGroup;
    bool_t              bGroupCast;
}BDB_tsFindAndBindEvent;

typedef enum
{
    BDB_E_SUCCESS,
    BDB_E_FAILURE,
    BDB_E_ERROR_INVALID_PARAMETER,
    BDB_E_ERROR_INVALID_DEVICE,
    BDB_E_ERROR_NODE_IS_ON_A_NWK,
    BDB_E_ERROR_IMPROPER_COMMISSIONING_MODE,
    BDB_E_ERROR_COMMISSIONING_IN_PROGRESS,
}BDB_teStatus;

typedef enum {
    BDB_EVENT_NONE,
    BDB_EVENT_ZPSAF,                // Use with BDB_tsZpsAfEvent

    BDB_EVENT_INIT_SUCCESS,
    BDB_EVENT_REJOIN_SUCCESS,
    BDB_EVENT_REJOIN_FAILURE,

    BDB_EVENT_NWK_STEERING_SUCCESS,
    BDB_EVENT_NO_NETWORK,
    BDB_EVENT_NWK_JOIN_SUCCESS,
    BDB_EVENT_NWK_JOIN_FAILURE,
    BDB_EVENT_APP_START_POLLING,

    BDB_EVENT_NWK_FORMATION_SUCCESS,
    BDB_EVENT_NWK_FORMATION_FAILURE,

    BDB_EVENT_FB_HANDLE_SIMPLE_DESC_RESP_OF_TARGET,        // Use with BDB_tsFindAndBindEvent
    BDB_EVENT_FB_CHECK_BEFORE_BINDING_CLUSTER_FOR_TARGET,
    BDB_EVENT_FB_CLUSTER_BIND_CREATED_FOR_TARGET,
    BDB_EVENT_FB_BIND_CREATED_FOR_TARGET,
    BDB_EVENT_FB_GROUP_ADDED_TO_TARGET,
    BDB_EVENT_FB_ERR_BINDING_FAILED,
    BDB_EVENT_FB_ERR_BINDING_TABLE_FULL,
    BDB_EVENT_FB_ERR_GROUPING_FAILED,
    BDB_EVENT_FB_NO_QUERY_RESPONSE,
    BDB_EVENT_FB_TIMEOUT,
    BDB_EVENT_FB_OVER_AT_TARGET,

    BDB_EVENT_FAILURE_RECOVERY_FOR_REJOIN,
    BDB_EVENT_LEAVE_WITHOUT_REJOIN,

    BDB_EVENT_OOB_FORM_SUCCESS,
    BDB_EVENT_OOB_JOIN_SUCCESS,
    BDB_EVENT_OOB_FAIL
} BDB_teBdbEventType;

typedef struct
{
    uint8 u8EndPoint;
    ZPS_tsAfEvent sStackEvent;
} BDB_tsZpsAfEvent;

typedef union
{
    BDB_tsZpsAfEvent            sZpsAfEvent;            // Use when BDB_EVENT_ZPSAF
    BDB_tsFindAndBindEvent      *psFindAndBindEvent;
} BDB_tuBdbEventData;

typedef struct
{
    BDB_teBdbEventType eEventType;
    BDB_tuBdbEventData uEventData;
} BDB_tsBdbEvent;

typedef void (* tfpBDB_ZpsCallBack)(uint8 u8Endpoint, ZPS_tsAfEvent *psStackEvent);
typedef void (* tfpBDB_BdbCallBack)(BDB_tsBdbEvent *psBdbEvent);

typedef struct
{
    tszQueue *hBdbEventsMsgQ;
} BDB_tsInitArgs;

/* BDB System-wide data */
typedef struct
{
    BDB_teState     eState;
    BDB_tsAttrib    sAttrib;

    uint8 *pu8DefaultTCLinkKey;     /* Default global Trust Center link key */
    uint8 *pu8DistributedLinkKey;   /* Distributed security global link key */
    uint8 *pu8PreConfgLinkKey;      /* Install code derived preconfigured link key */
    uint8 *pu8TouchLinkKey;         /* Touchlink preconfigured link key */

    tszQueue *hBdbEventsMsgQ;
} tsBDB;

typedef enum {
    BDB_E_ZCL_EVENT_NONE = 0,
    BDB_E_ZCL_EVENT_TL_START,
    BDB_E_ZCL_EVENT_TL_TIMER_EXPIRED,
    BDB_E_ZCL_EVENT_TL_IPAN_MSG,
    BDB_E_ZCL_EVENT_TL_ACK,
    BDB_E_ZCL_EVENT_TL_NOACK,
    BDB_E_ZCL_EVENT_DISCOVERY_DONE,
    BDB_E_ZCL_EVENT_LEAVE_CFM,
    BDB_E_ZCL_EVENT_IDENTIFY_QUERY,
    BDB_E_ZCL_EVENT_IDENTIFY
}teBDB_ZCLCallbackEventType;

typedef struct {
    teBDB_ZCLCallbackEventType eType;
    tsZCL_CallBackEvent *psCallBackEvent;
}tsBDB_ZCLEvent;

typedef void (*tprEventHandler)(void *);

typedef struct {
    ZPS_tsAftsStartParamsDistributed sNwkParams;
    uint8 au8NwkKey[16];
    uint16 u16GroupIdBegin;
    uint16 u16GroupIdEnd;
    uint16 u16FreeNwkAddrBegin;
    uint16 u16FreeNwkAddrEnd;
    uint16 u16FreeGroupIdBegin;
    uint16 u16FreeGroupIdEnd;
    uint64 u64InitiatorIEEEAddr;
    uint16 u16InitiatorNwkAddr;
} tsStartParams;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC void BDB_vInit(BDB_tsInitArgs *psInitArgs);
PUBLIC void BDB_vStart(void);
PUBLIC void BDB_vRestart(void);
PUBLIC BDB_teStatus BDB_eNfStartNwkFormation(void);
PUBLIC BDB_teStatus BDB_eNsStartNwkSteering(void);
PUBLIC BDB_teStatus BDB_eFbTriggerAsInitiator(uint8 u8SourceEndPointId);
PUBLIC void BDB_vFbExitAsInitiator();
PUBLIC BDB_teStatus BDB_eFbTriggerAsTarget(uint8 u8EndPointId);
PUBLIC void BDB_vFbExitAsTarget(uint8 u8SourceEndpoint);

PUBLIC bool_t BDB_bIsBaseIdle(void);
PUBLIC void BDB_vSetRejoinFilter(void);
PUBLIC void BDB_vSetAssociationFilter(void);


PUBLIC void BDB_vZclEventHandler(tsBDB_ZCLEvent *psEvent);
PUBLIC void APP_vBdbCallback(BDB_tsBdbEvent *psBdbEvent);
PUBLIC void bdb_taskBDB(void);
PUBLIC void BDB_vRegisterRejoinControl(void* fnPtr);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern PUBLIC tsBDB sBDB;


#if defined __cplusplus
}
#endif

#endif  /* BDB_API_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

