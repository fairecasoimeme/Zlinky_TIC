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
 * MODULE:             ZPS NWK
 *
 * COMPONENT:          zps_nwk_nib.h
 *
 * DESCRIPTION:        ZPS NWK Network Information Base header
 *
 *****************************************************************************/

/**
 * @defgroup g_zps_nwk_nib ZPS NWK Network Information Base
 *
 * Parameters associated with setting or getting NIB values.
 * NIB direct access allows getting and setting of NIB attributes directly by obtaining
 * the NIB handle and dereferencing it appropriately.
 * All NIB attributes can be read (got) in this manner
 * Most NIB attributes can be written (set) in this manner, however some NIB attributes
 * require an auxiliary function call to be called instead. This is because they also require
 * hardware register values to change as well.
 * The functions listed in this group indicate which NIB attributes need to be set in this manner
 * @ingroup g_zps_nwk
 */

#ifndef _zps_nwk_nib_h_
#define _zps_nwk_nib_h_

#ifdef __cplusplus
extern "C" {
#endif

/***********************/
/**** INCLUDE FILES ****/
/***********************/

#include "jendefs.h"
#include "zps_nwk_config.h"
#include "zps_nwk_sap.h"
#include "zps_nwk_slist.h"
#include "zps_tsv.h"

/************************/
/**** MACROS/DEFINES ****/
/************************/
/**
 * @name NWK constants
 * @ingroup g_zps_nwk_nib
 * All the network constants
 * @{
 */


#ifndef ZPS_NWK_INLINE
#define ZPS_NWK_INLINE  INLINE
#endif

typedef enum {
    ZED_TIMEOUT_10_SEC,                // 0 -> 10 seconds
    ZED_TIMEOUT_2_MIN,          // 1 -> 2 minutes
    ZED_TIMEOUT_4_MIN,          // 2 -> 4 minutes
    ZED_TIMEOUT_8_MIN,          // 3 -> 8 minutes
    ZED_TIMEOUT_16_MIN,         // 4 -> 16 minutes
    ZED_TIMEOUT_32_MIN,         // 5 -> 32 minutes
    ZED_TIMEOUT_64_MIN,         // 6 -> 64 minutes
    ZED_TIMEOUT_128_MIN,        // 7 -> 128 minutes
    ZED_TIMEOUT_256_MIN,        // 8 -> 256 minutes (4 hrs 16 min)
    ZED_TIMEOUT_512_MIN,        // 9 -> 512 minutes (8 hrs 32 min)
    ZED_TIMEOUT_1024_MIN,       // 10 -> 1024 minutes (17 hrs 4 min)
    ZED_TIMEOUT_2048_MIN,       // 11 -> 2048 minutes (1 day 10 hrs 8 min)
    ZED_TIMEOUT_4096_MIN,       // 12 -> 4096 minutes (2 days 20 hrs 16 min)
    ZED_TIMEOUT_8192_MIN,       // 13 -> 8192 minutes (5 days 16 hrs 32 min)
    ZED_TIMEOUT_16384_MIN,       // 14 -> 16384 minutes (11 days 9 hrs 4 min)
    ZED_TIMEOUT_INDEX_MAX,
    ZED_TIMEOUT_UNDEFINED = 0xff
}teZedTimeout;

#define CHILD_KEEP_ALIVE_UNDEFINED      		0
#define CHILD_KEEP_ALIVE_POLL           		0x01
#define CHILD_KEEP_ALIVE_TO_REQ         		0x02
#define CHILD_POWER_NEGOTIATION_SUPPORTED       0x04
#define PARENT_KEEP_ALIVE_CAPABILITY   ( CHILD_KEEP_ALIVE_POLL | CHILD_KEEP_ALIVE_TO_REQ | CHILD_POWER_NEGOTIATION_SUPPORTED)
#define KEEP_ALIVE_RCVD                (1<<0)
#define KEEP_ALIVE_RECENTLY_RCVD       (1<<1)
#define ZED_TIMEOUT_INDEX_DEFAULT    ZED_TIMEOUT_256_MIN
extern const uint32 au32EndDeviceTimeoutConstants[ZED_TIMEOUT_INDEX_MAX];


// Trac #502 8ms Btt tick
/** Route request retry interval = 0xfe milliseconds = 0x20 8ms periods */
#define ZPS_NWK_CONST_RREQ_RETRY_INTERVAL              ((uint8)0x20)

#define ZPS_NWK_CONST_INIT_RREQ_RETRIES                ((uint8)3)
#define ZPS_NWK_CONST_RREQ_RETRIES                     ((uint8)2)

/**
 * @name NIB defaults, maxima and minima
 * @ingroup g_zps_nwk_nib
 * All the default values, minima and maxima for the ZPS NWK NIB
 * @{
 */

/** Max. depth default - depends on stack profile */
#define ZPS_NWK_NIB_MAX_DEPTH_DEF                       ((uint8)ZPS_NWK_OPT_FS_MAX_DEPTH)


/** Broadcast delivery time default - in half seconds */
#define ZPS_NWK_NIB_BROADCAST_DELIVERY_TIME_DEF         ((uint8)18)


/** Unique address default - depends on feature set */
#define ZPS_NWK_NIB_UNIQUE_ADDR_DEF                     FALSE

/** Address alloc default - depends on feature set */
typedef enum
{
    ZPS_NWK_ADDR_ALC_DISTRIBUTED = 0,
    ZPS_NWK_ADDR_ALC_RESERVED = 1,
    ZPS_NWK_ADDR_ALC_STOCHASTIC = 2,
    ZPS_NWK_ADDR_ALC_VS_FIXED = 3    /* Vendor specific for testing */
} ZPS_teNwkAddrAlloc;

#define ZPS_NWK_NIB_ADDR_ALC_DEF                        ZPS_NWK_ADDR_ALC_STOCHASTIC


/* @} */

/** Security defines */
#define ZPS_NWK_KEY_LENGTH                              16

/* @} */

#ifndef BIT_W_1
#define BIT_W_1     0x1
#define BIT_W_2     0x3
#define BIT_W_3     0x7
#define BIT_W_4     0xF
#define BIT_W_5     0x1F
#define BIT_W_6     0x3F
#define BIT_W_7     0x7F
#define BIT_W_8     0xFF
#endif

/**** Mask operations ****/

#define ZPS_NWK_NT_TST(x, m)               ((x) & (m))
#define ZPS_NWK_NT_SET(x, m)               ((x) |= (m))
#define ZPS_NWK_NT_CLR(x, m)               ((x) &= ~(m))
#define ZPS_NWK_NT_MOD(x, m, y)            ((x) = (((x) & ~(m)) | (y)))

/**** Superframe specification ****/

/* Bits 0-3 - Beacon order - not interested */
/* Bits 4-7 - Superframe order - not interested */
/* Bits 8-11 - Final CAP slot - not interested */
/* Bit 12 - Battery life extension - not interested */
/* Bit 13 - Reserved - not interested */
/* Bit 14 - PAN Coordinator: Corresponds to ZigBee Coordinator in NT */
/* Bit 15 - Association permit: Corresponds to Join permit in NT */



/* Capability fields */

#define ZPS_NWK_CAP_DEVICE_TYPE_BIT             1
#define ZPS_NWK_CAP_DEVICE_TYPE_MASK            ((uint8)(BIT_W_1 << ZPS_NWK_CAP_DEVICE_TYPE_BIT))
#define ZPS_NWK_CAP_DEVICE_TYPE(x)              (((x) & ZPS_NWK_CAP_DEVICE_TYPE_MASK) >> ZPS_NWK_CAP_DEVICE_TYPE_BIT)
#define ZPS_NWK_CAP_MOD_DEVICE_TYPE(x, d)       ZPS_NWK_NT_MOD(x, ZPS_NWK_CAP_DEVICE_TYPE_MASK, ((d) << ZPS_NWK_CAP_DEVICE_TYPE_BIT))
#define ZPS_NWK_CAP_TST_DEVICE_TYPE(x)          ZPS_NWK_NT_TST(x, ZPS_NWK_CAP_DEVICE_TYPE_MASK)
#define ZPS_NWK_CAP_SET_DEVICE_TYPE(x)          ZPS_NWK_NT_SET(x, ZPS_NWK_CAP_DEVICE_TYPE_MASK)
#define ZPS_NWK_CAP_CLR_DEVICE_TYPE(x)          ZPS_NWK_NT_CLR(x, ZPS_NWK_CAP_DEVICE_TYPE_MASK)

#define ZPS_NWK_CAP_PWR_SRC_BIT                 2
#define ZPS_NWK_CAP_PWR_SRC_MASK                ((uint8)(BIT_W_1 << ZPS_NWK_CAP_PWR_SRC_BIT))
#define ZPS_NWK_CAP_PWR_SRC(x)                  (((x) & ZPS_NWK_CAP_PWR_SRC_MASK) >> ZPS_NWK_CAP_PWR_SRC_BIT)
#define ZPS_NWK_CAP_MOD_PWR_SRC(x, d)           ZPS_NWK_NT_MOD(x, ZPS_NWK_CAP_PWR_SRC_MASK, ((d) << ZPS_NWK_CAP_PWR_SRC_BIT))
#define ZPS_NWK_CAP_TST_PWR_SRC(x)              ZPS_NWK_NT_TST(x, ZPS_NWK_CAP_PWR_SRC_MASK)
#define ZPS_NWK_CAP_SET_PWR_SRC(x)              ZPS_NWK_NT_SET(x, ZPS_NWK_CAP_PWR_SRC_MASK)
#define ZPS_NWK_CAP_CLR_PWR_SRC(x)              ZPS_NWK_NT_CLR(x, ZPS_NWK_CAP_PWR_SRC_MASK)

#define ZPS_NWK_CAP_RX_ON_WHEN_IDLE_BIT         3
#define ZPS_NWK_CAP_RX_ON_WHEN_IDLE_MASK        ((uint8)(BIT_W_1 << ZPS_NWK_CAP_RX_ON_WHEN_IDLE_BIT))
#define ZPS_NWK_CAP_RX_ON_WHEN_IDLE(x)          (((x) & ZPS_NWK_CAP_RX_ON_WHEN_IDLE_MASK) >> ZPS_NWK_CAP_RX_ON_WHEN_IDLE_BIT)
#define ZPS_NWK_CAP_MOD_RX_ON_WHEN_IDLE(x, d)   ZPS_NWK_NT_MOD(x, ZPS_NWK_CAP_RX_ON_WHEN_IDLE_MASK, ((d) << ZPS_NWK_CAP_RX_ON_WHEN_IDLE_BIT))
#define ZPS_NWK_CAP_TST_RX_ON_WHEN_IDLE(x)      ZPS_NWK_NT_TST(x, ZPS_NWK_CAP_RX_ON_WHEN_IDLE_MASK)
#define ZPS_NWK_CAP_SET_RX_ON_WHEN_IDLE(x)      ZPS_NWK_NT_SET(x, ZPS_NWK_CAP_RX_ON_WHEN_IDLE_MASK)
#define ZPS_NWK_CAP_CLR_RX_ON_WHEN_IDLE(x)      ZPS_NWK_NT_CLR(x, ZPS_NWK_CAP_RX_ON_WHEN_IDLE_MASK)

#define ZPS_NWK_CAP_SECURITY_MODE_BIT           6
#define ZPS_NWK_CAP_SECURITY_MODE_MASK          ((uint8)(BIT_W_1 << ZPS_NWK_CAP_SECURITY_MODE_BIT))
#define ZPS_NWK_CAP_SECURITY_MODE(x)            (((x) & ZPS_NWK_CAP_SECURITY_MODE_MASK) >> ZPS_NWK_CAP_SECURITY_MODE_BIT)
#define ZPS_NWK_CAP_MOD_SECURITY_MODE(x, d)     ZPS_NWK_NT_MOD(x, ZPS_NWK_CAP_SECURITY_MODE_MASK, ((d) << ZPS_NWK_CAP_SECURITY_MODE_BIT))
#define ZPS_NWK_CAP_TST_SECURITY_MODE(x)        ZPS_NWK_NT_TST(x, ZPS_NWK_CAP_SECURITY_MODE_MASK)
#define ZPS_NWK_CAP_SET_SECURITY_MODE(x)        ZPS_NWK_NT_SET(x, ZPS_NWK_CAP_SECURITY_MODE_MASK)
#define ZPS_NWK_CAP_CLR_SECURITY_MODE(x)        ZPS_NWK_NT_CLR(x, ZPS_NWK_CAP_SECURITY_MODE_MASK)

#define ZPS_NWK_CAP_ALLOC_ADDR_BIT              7
#define ZPS_NWK_CAP_ALLOC_ADDR_MASK             ((uint8)(BIT_W_1 << ZPS_NWK_CAP_ALLOC_ADDR_BIT))
#define ZPS_NWK_CAP_ALLOC_ADDR(x)               (((x) & ZPS_NWK_CAP_ALLOC_ADDR_MASK) >> ZPS_NWK_CAP_ALLOC_ADDR_BIT)
#define ZPS_NWK_CAP_MOD_ALLOC_ADDR(x, d)        ZPS_NWK_NT_MOD(x, ZPS_NWK_CAP_ALLOC_ADDR_MASK, ((d) << ZPS_NWK_CAP_ALLOC_ADDR_BIT))
#define ZPS_NWK_CAP_TST_ALLOC_ADDR(x)           ZPS_NWK_NT_TST(x, ZPS_NWK_CAP_ALLOC_ADDR_MASK)
#define ZPS_NWK_CAP_SET_ALLOC_ADDR(x)           ZPS_NWK_NT_SET(x, ZPS_NWK_CAP_ALLOC_ADDR_MASK)
#define ZPS_NWK_CAP_CLR_ALLOC_ADDR(x)           ZPS_NWK_NT_CLR(x, ZPS_NWK_CAP_ALLOC_ADDR_MASK)


#define ZPS_NWK_CAP_BUILD(x)                    (((x).u1DeviceType << ZPS_NWK_CAP_DEVICE_TYPE_BIT) | \
                                                 ((x).u1PowerSource << ZPS_NWK_CAP_PWR_SRC_BIT) | \
                                                 ((x).u1RxOnWhenIdle << ZPS_NWK_CAP_RX_ON_WHEN_IDLE_BIT) | \
                                                 ((x).u1SecurityMode << ZPS_NWK_CAP_SECURITY_MODE_BIT))

/*
 * Any bits overlaid must be 0
 * 01001110
 * ||||||||
 * |||||||+- Alternate PAN coordinator: overlaid with Used
 * ||||||+-- Device type
 * |||||+--- Power source
 * ||||+---- Rx on when idle
 * ||++----- Reserved: Overlaid with relationship
 * |+------- Security capability (= mode)
 * +-------- Allocate address: Overlaid with authenticated
 */
#define ZPS_NWK_CAP_FIELD_MASK                  0x4e

/*
 * Any bits overlaid must be 0
 * 00110011
 * ||||||||
 * |||||||+- Used
 * ||||||+-- Device type
 * |||||+--- Power source
 * ||||+---- Rx on when idle
 * ||++----- Relationship
 * |+------- Security capability (= mode)
 * +-------- Authenticated
 */

#define ZPS_NWK_NT_ACTV_USED_RELATIONSHIP_MASK 0x33
#define ZPS_NWK_NT_ACTV_USED_RELATIONSHIP_CHILD 0x11

/* Enumerations */
#define ZPS_NWK_NT_AP_RELATIONSHIP_PARENT          (0)
#define ZPS_NWK_NT_AP_RELATIONSHIP_CHILD           (1)
#define ZPS_NWK_NT_AP_RELATIONSHIP_SIBLING         (2)
#define ZPS_NWK_NT_AP_RELATIONSHIP_UNKNOWN         (3)

/* Device type */
#define ZPS_NWK_NT_AP_DEVICE_TYPE_ZED              (0)
#define ZPS_NWK_NT_AP_DEVICE_TYPE_ZR_ZC            (1)

/* Power source */
#define ZPS_NWK_NT_AP_PWR_SRC_BATT                 (0)
#define ZPS_NWK_NT_AP_PWR_SRC_MAINS                (1)

/*
 * Initial parent bitmap
 * 00001111
 * ||||||||
 * |||||||+- Used: True
 * ||||||+-- Device type: ZC/ZR
 * |||||+--- Power source: Mains (TODO)
 * ||||+---- Rx on when idle: True (TODO?)
 * ||++----- Device type: Parent (0x00)
 * |+------- Security capability (= mode): 0 (don't know yet)
 * +-------- Authenticated: 0 (don't know yet)
 */
#define ZPS_NWK_NT_AP_INITIAL_PARENT              0x0f

/*
 * Initial sibling bitmap
 * 00101111
 * ||||||||
 * |||||||+- Used: True
 * ||||||+-- Device type: ZC/ZR
 * |||||+--- Power source: Mains (TODO)
 * ||||+---- Rx on when idle: True (TODO?)
 * ||++----- Device type: Sibling (0x02)
 * |+------- Security capability (= mode): 0 (don't know yet)
 * +-------- Authenticated: 0 (don't know yet)
 */
#define ZPS_NWK_NT_AP_INITIAL_SIBLING             0x2f

/**
 * Active neighbour table search masks
 */
#define ZPS_NWK_NT_ACTV_MATCH_NWK_ADDR 0x01 /**< Match network address */
#define ZPS_NWK_NT_ACTV_MATCH_EXT_ADDR 0x02 /**< Match extended address */
#define ZPS_NWK_NT_ACTV_MATCH_CHILD    0x04 /**< Match child */
#define ZPS_NWK_NT_ACTV_MATCH_ZED      0x08 /**< Match end device */

#define ZPS_NWK_NT_ACTV_MATCH_EXT_ADDR_CHILD     (ZPS_NWK_NT_ACTV_MATCH_EXT_ADDR | ZPS_NWK_NT_ACTV_MATCH_CHILD)
#define ZPS_NWK_NT_ACTV_MATCH_EXT_ADDR_ZED       (ZPS_NWK_NT_ACTV_MATCH_EXT_ADDR | ZPS_NWK_NT_ACTV_MATCH_ZED)
#define ZPS_NWK_NT_ACTV_MATCH_NWK_ADDR_CHILD     (ZPS_NWK_NT_ACTV_MATCH_NWK_ADDR | ZPS_NWK_NT_ACTV_MATCH_CHILD)
#define ZPS_NWK_NT_ACTV_MATCH_NWK_ADDR_ZED_CHILD (ZPS_NWK_NT_ACTV_MATCH_NWK_ADDR | ZPS_NWK_NT_ACTV_MATCH_CHILD | ZPS_NWK_NT_ACTV_MATCH_ZED)

/**************************/
/**** TYPE DEFINITIONS ****/
/**************************/

/**
 * ZPS NWK Routing Table entry status
 * The status of a Routing Table entry
 * @ingroup g_zps_nwk_nib
 */
typedef enum
{
    ZPS_NWK_RT_ACTIVE = 0,
    ZPS_NWK_RT_DISC_UNDERWAY = 1,
    ZPS_NWK_RT_DISC_FAILED = 2,
    ZPS_NWK_RT_INACTIVE = 3,
    ZPS_NWK_RT_VALDN_UNDERWAY = 4
} ZPS_teNwkRtStatus;

/**
 * ZPS NWK Broadcast Transaction Table entry state
 * The state a Broadcast Transaction Table entry can be in
 * @ingroup g_zps_nwk_nib
 */
typedef enum
{
    ZPS_NWK_BTR_INACTIVE,
    ZPS_NWK_BTR_JITTER_PASSIVE_ACK,         /**< Timing with passive ack for standard broadcast - jitter phase */
    ZPS_NWK_BTR_JITTER_PASSIVE_ACK_RECD,    /**< Timing with passive ack for standard broadcast - jitter phase, passive ack recd. */
    ZPS_NWK_BTR_PASSIVE_ACK,                /**< Timing with passive ack for standard broadcast */
    ZPS_NWK_BTR_NO_PASSIVE_ACK,             /**< Timing with no passive ack for member mode multicast */
    ZPS_NWK_BTR_RREQ_RETRY_INTERVAL,        /**< Timing route request retry interval */
    ZPS_NWK_BTR_BCAST_DELIVERY_TIME,        /**< Timing broadcast delivery time for standard broadcast */
} ZPS_teNwkBtrState;

/**
 * ZPS NWK Discovery Neighbor table entry
 * The neighbor table for devices within radio range during a discovery
 * @ingroup g_zps_nwk_nib
 * @note Only fields which can be gleaned from the PAN Descriptor and beacon payload
 * need to be considered here.
 */
typedef struct
{
    uint64 u64ExtPanId;         /**< Extended PAN ID */
    uint16 u16PanId;            /**< PAN ID */
    uint16 u16NwkAddr;          /**< Network address */
    uint8  u8LinkQuality;       /**< Link quality */
    uint8  u8LogicalChan;       /**< Logical channel */
    uint8  u8NwkUpdateId;       /**< Update ID */
    /*
     * Bitfields are used for syntactic neatness and space saving. May need to assess whether
     * these are suitable for embedded environment
     */
    union
    {
        struct
        {
            unsigned u1PanCoord:1;
            unsigned u1JoinPermit:1;
            unsigned u1ZrCapacity:1;
            unsigned u4Depth:4;
            unsigned u1ZedCapacity:1;
            unsigned u4StackProfile:4;      /**< Stack profile */
            unsigned u1PotentialParent:1;
            unsigned u1Used:1;
#ifdef WWAH_SUPPORT
            unsigned u2PriorityParent:2;
#endif
        } bfBitfields;
        uint8 au8Field[2];
    } uAncAttrs;
} ZPS_tsNwkDiscNtEntry;

/**
 * ZPS NWK Neighbor table entry
 * The neighbor table for devices within radio range. This is when the device is
 * active and will, for a tree, indicate one parent and all children. For a mesh,
 * may also include siblings
 * @ingroup g_zps_nwk_nib
 * @note
 * Only need one bit for u1DeviceType to show if it is a router or end device.
 * A ZC will be shown as router but always have u16NwkAddr as 0x0000.
 */
typedef struct
{
    zps_tsNwkSlistNode sNode;  /**< Single linked list node */
    uint16 u16Lookup;         /**< Extended address */
    uint16 u16NwkAddr;         /**< Network address */
    uint8  u8TxFailed;         /**< Transmit failed count */
    uint8  u8LinkQuality;      /**< Link Quality indication */
    uint8  u8Age;    /**< Router age (in link status periods) */
    uint8 u8ZedTimeoutindex;    /* index into the timeout const table */

    int8 	i8TXPower;			/** < TX Power in dBm last used for this device */
    uint8	u8MacID;			/** < Mac ID for this device */
    /*
     * Bitfields are used for syntactic neatness and space saving. May need to assess whether
     * these are suitable for embedded environment
     */
    union
    {
        struct
        {
            unsigned u1Used:1;           /* Overlays: Alternate PAN coordinator */
            unsigned u1DeviceType:1;
            unsigned u1PowerSource:1;
            unsigned u1RxOnWhenIdle:1;
            unsigned u2Relationship:2;   /* Overlays: Reserved */
            unsigned u1SecurityMode:1;
            unsigned u1Authenticated:1;  /* Overlays: Allocate address */
            unsigned u1LinkStatusDone:1; /**< Link status has been processed for this device */
            unsigned u3OutgoingCost:3;     /**< Outgoing cost for sym link = true */
            unsigned u3Reserve:3;
            unsigned u1ExpectAnnc:1;       /*** Set for newly joined children, cleared on hearing their annce **/
        } bfBitfields;
        uint8 au8Field[2];
    }  uAncAttrs;
} ZPS_tsNwkActvNtEntry;

#define ZPS_NWK_NT_ACTV_PARENT              0 /**< Entry 0 is the parent */
#define ZPS_NWK_NT_ACTV_FIRST_NON_PARENT    ZPS_NWK_NT_ACTV_PARENT+1 /**< Entry 0 is the parent */

/**
 * ZPS NWK NIB Initial values
 * This overlays the top of the NIB, making it easier to initialise the NIB in one go
 * @note Any changes to the NIB order MUST be reflected here
 * A union/substructure was not use for clarity in the NIB itself
 */
typedef struct
{
    uint32 u32VsOldRouteExpiryTime;                         /**< Vendor specific - Period of old route garbage collection */
    uint8  u8MaxRouters;                                    /**< nwkMaxRouters */
    uint8  u8MaxChildren;                                   /**< nwkMaxChildren */
    uint8  u8MaxDepth;                                      /**< nwkMaxDepth */
    uint8  u8PassiveAckTimeout;                             /**< nwkPassiveAckTimeout (in half-seconds) */ /* Potential const */
    uint8  u8MaxBroadcastRetries;                           /**< nwkMaxBroadcastRetries */ /* Potential const */
    uint8  u8MaxSourceRoute;                                /**< nwkMaxSourceRoute */
    uint8  u8NetworkBroadcastDeliveryTime;                  /**< nwkNetworkBroadcastDeliveryTime */ /* Potential const */
    uint8  u8UniqueAddr;                                    /**< nwkUniqueAddr */
    uint8  u8AddrAlloc;                                     /**< nwkAddrAlloc */
    uint8  u8UseTreeRouting;                                /**< nwkUseTreeRouting: bool */
    uint8  u8SymLink;                                       /**< nwkSymLink: bool */
    uint8  u8UseMulticast;                                  /**< nwkUseMulticast: bool */
    uint8  u8LinkStatusPeriod;                              /**< nwkLinkStatusPeriod */
    uint8  u8RouterAgeLimit;                                /**< nwkRouterAgeLimit */
    uint8  u8RouteDiscoveryRetriesPermitted;                /**< nwkRouteDiscoveryRetriesPermitted */
    uint8  u8VsFormEdThreshold;                             /**< Vendor specific - energy detect threshold for network forming */
    uint8  u8SecurityLevel;                                 /**< nwkSecurityLevel */ /* Potential const */
    uint8  u8AllFresh;                                      /**< nwkAllFresh */ /* Potential const */
    uint8  u8SecureAllFrames;                               /**< nwkSecureAllFrames */ /* Potential const */
    uint8  u8VsTxFailThreshold;                             /**< Vendor specific - transmit fail threshold when RT next hop gets cleared */
    uint8  u8VsMaxOutgoingCost;                             /**< Vendor specific - only uses links with cost >= than this */
    uint8  u8VsLeaveRejoin;
    uint8 u8ZedTimeout;
    uint8 u8ZedTimeoutDefault;                              /** nwkEndDeviceTimeoutDefault **/

    uint16 u16VerifyLinkCostTransmitRate; 					/** nwkVerifyLinkCostTransmitRate 0-65535 **/

} ZPS_tsNwkNibInitialValues;

/**
 * ZPS NWK Route discovery table entry
 * The route discovery table for mesh routing devices.
 * @ingroup g_zps_nwk
 * @note
 */
typedef struct
{
    ZPS_tsTsvTimer sTimer;   /**< Route discovery timer */
    uint16 u16NwkSrcAddr;    /**< Network address of RREQ's initiator */
    uint16 u16NwkSenderAddr; /**< Network address of RREQ's corresponding to u8RreqId neighbour relayer with best link cost */
    uint16 u16RtEntryId;       /**< Associated Routing Table entry ID (its index) */
    uint8 u8RouteReqId;      /**< RREQ ID */
    uint8 u8ForwardCost;     /**< Accumulated path cost from RREQ src to this node */
    uint8 u8ResidualCost;    /**< Accumulated path cost this node to RREQ dst (symmetrical routing?) */
} ZPS_tsNwkRtDiscEntry;

/**
 * ZPS NWK Route table entry
 * The routing table for mesh routing devices.
 * @ingroup g_zps_nwk_nib
 * @note
 */
typedef struct
{
    uint16 u16NwkDstAddr;     /**< Destination Network address */
    uint16 u16NwkNxtHopAddr;  /**< Next hop Network address */
    uint8  u8ExpiryCount;
    /*
     * Bitfields are used for syntactic neatness and space saving. May need to assess whether
     * these are suitable for embedded environment
     */
    union
    {
        struct
        {
            unsigned u3Status:3;          /**< Status of route */
            unsigned u1NoRouteCache:1;    /**< Destination address is unable to store source routes */
            unsigned u1ManyToOne:1;       /**< Destination address is concentrator that issued a many-to-one route request */
            unsigned u1RouteRecordReqd:1; /**< Route record is required */
            unsigned u1GroupIdFlag:1;     /**< Indicates u16DstAddr is a group ID -- Now used as a ever used flag*/
            unsigned u1RecentlyUsed:1;    /**< Indicates route has recently been used */
            unsigned u8TxFailure:8;
        } bfBitfields;
        uint8 au8Field[2];
    } uAncAttrs;
} ZPS_tsNwkRtEntry;

/**
 * ZPS NWK Broadcast transaction record
 * The broadcast transaction record (BTR)
 * @ingroup g_zps_nwk_nib
 * @note
 * As we had to go into a second word, fill it up with info
 */
typedef struct
{
    uint16 u16NwkSrcAddr;     /**< Source Network address */
    uint8  u8SeqNum;          /**< Sequence number */
    uint8  u8ExpiryTime;      /**< Expiration time */
    uint8  u8State;           /**< State (use a uint8 to save space) */
    uint8  u8ExpiryPeriods;   /**< Used for retries and longer timings */
} ZPS_tsNwkBtr;

/**
 * ZPS NWK Route record table entry
 * A route record entry in the Route Record table
 * @ingroup g_zps_nwk_nib
 * @note
 */
typedef struct
{
    uint16 u16NwkDstAddr;                           /**< Destination address */
    uint8  u8RelayCount;                            /**< Relay count */
    uint16 au16Path[11];                            /**< The total number of hops is 11 because the tunnel command can only fit
                                                         that many hops */
} ZPS_tsNwkRctEntry;

/*****************************************************************************/
/* SECURITY ATTRIBUTES */
/*****************************************************************************/


/**
 * ZPS NWK Security Material Set
 * @ingroup g_zps_nwk_nib
 * @note
 */
typedef struct
{
	uint8               au8Key[ZPS_NWK_KEY_LENGTH] __attribute__ ((aligned (16)));         /**< The Key */
	uint8               u8KeySeqNum;                        /**< Key Sequence Number */
    uint8               u8KeyType;                          /**< Key Type */

} ZPS_tsNwkSecMaterialSet;

/**
 * ZPS NWK Initial Table sizes
 * Used for passing as a parameter on initialisation
 * @ingroup g_zps_nwk_nib
 */
typedef struct
{
    uint16 u16NtActv;     /**< nwkNeighborTable (active) size */
    uint16 u16Rt;         /**< nwkRouteTable size */
    uint16 u16Rct;        /**< nwkRouteRecordTable size */
    uint16 u16AddrMap;    /**< nwkAddressMap size */
    uint8 u8NtDisc;     /**< nwkNeighborTable (discovery) size */
    uint8 u8RtDisc;     /**< Route discovery table (not strictly NIB) */
    uint8 u8Btt;        /**< nwkBroadcastTransactionTable size */
    uint8 u8SecMatSet;  /**< nwkSecurityMaterialSet size */
    uint8 u8NibDefault; /** size of the nib default struct **/
    uint8 u8ChildTable;
    uint16 u16MacAddTableSize;
} ZPS_tsNwkNibTblSize;


/**
 * ZPS NWK Initial Table locations
 * Used for passing as a parameter on initialisation
 * @ingroup g_zps_nwk_nib
 */
typedef struct
{
    ZPS_tsNwkDiscNtEntry *psNtDisc;                                      /**< nwkNeighborTable - discovery */
    ZPS_tsNwkActvNtEntry *psNtActv;                                      /**< nwkNeighborTable - active */
    ZPS_tsNwkRtDiscEntry *psRtDisc;                                      /**< Route discovery table (not strictly NIB) */
    ZPS_tsNwkRtEntry *psRt;                                              /**< nwkRouteTable */
    ZPS_tsNwkBtr *psBtt;                                                 /**< nwkBroadcastTransactionTable */
    /* Group ID */                                                       /**< nwkGroupIDTable */
    ZPS_tsNwkRctEntry *psRct;                                            /**< nwkRouteRecordTable */
    ZPS_tsNwkSecMaterialSet *psSecMatSet;                                /**< nwkSecurityMaterialSet */     /* [I SP001379_sr 67] */
    ZPS_tsNwkNibInitialValues* psNibDefault;                             /** struct holding Nib defaults (NOT a table) **/
    uint16 *pu16AddrMapNwk;                                              /**< nwkAddressMap - network addresses */
    uint16 *pu16AddrLookup;                                              /**< lookup index in the MAC table */
    uint32              *pu32InFCSet;                                    /**< Incoming Frame Counter Set  */
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    uint32              u32OutFC;                                        /** < Vendor specific - single NWK outgoing frame counter for all security sets */
#endif
    uint64 *pu64AddrExtAddrMap;                                          /**< nwkAddressMap - extended addresses */
    uint32  *pu32ZedTimeoutCount;
    uint8   *pu8KeepAliveFlags;
    uint32 *pu32RxByteCount;
    uint32 *pu32TxByteCount;
} ZPS_tsNwkNibTbl;

/**
 * ZPS NWK NIB Persisted Data
 * This includes members of the NIB which must persist through reset etc.
 * @note This does not include any tables e.g neighbour table, which are persisted separately
 */
typedef struct
{
    uint8  u8UpdateId;                                      /**< nwkUpdateId */
    uint8  u8ActiveKeySeqNumber;                            /**< nwkActiveKeySeqNumber */                    /* [I SP001379_sr 67] */
    uint8  u8VsDepth;                                       /**< Vendor specific - Depth this node is at */
    uint8  u8CapabilityInformation;                         /**< nwkCapabilityInformation */
    uint8  u8VsChannel;                                     /**< Vendor specific - Channel: In Phy also */
    uint8  u8ParentTimeoutMethod;                           /** nwkParentInformation **/
    uint8  u8VsAuxChannel;                                  /**< Multimac auxiliary active channel */
    uint8  u8MacEnhanced;                                   /**< MAC Enhanced Mode */
    uint16 u16VsPanId;                                      /**< Vendor specific - nwkPANId: In MAC PIB */
    uint16 u16NwkAddr;                                      /**< nwkNetworkAddress: In MAC PIB */
    uint16 u16VsParentAddr;                                 /**< Vendor specific - Parent network address: also in PIB */
#if ( JENNIC_CHIP_FAMILY != JN516x) && ( JENNIC_CHIP_FAMILY != JN517x)
    uint32  u32OutFC;
#endif
    uint64 u64ExtPanId;                                     /**< nwkExtendedPANID */
} ZPS_tsNWkNibPersist;

/**
 * ZPS NWK NIB
 * The NIB itself
 * @ingroup g_zps_nwk_nib
 * @note: TODO Would be an idea to sort out based on size etc. like MAC
 * @note: Some are potential constants given ZigBee and ZigBee PRO feature sets but
 * are still included here for flexibility:
 * nwkPassiveAckTimeout = 0.5s (ZigBee and PRO)
 * nwkBroadcastRetries = 2 (ZigBee and PRO)
 * nwkBroadcastDeliveryTime = 9 (ZigBee and PRO)
 * nwkAllFresh = TRUE
 * nwkSecureAllFrames = TRUE
 * nwkSecurityLevel = 5
 * @note: Some are constants given ZigBee and ZigBee PRO feature sets and will not be included:
 * nwkTimeStamp = FALSE
 * @note: Some are in the MAC PIB and will be accessed through the MAC PIB:
 * nwkTransactionPersistenceTime = macTransactionPersistenceTime
 * nwkShortAddress = macShortAddress
 * nwkPanId = macPANId
 */
typedef struct
{
    /**** Initial values described in struct above start here ****/
    uint32 u32VsOldRouteExpiryTime;                         /**< Vendor specific - Period of old route garbage collection */
    uint8  u8MaxRouters;                                    /**< nwkMaxRouters */
    uint8  u8MaxChildren;                                   /**< nwkMaxChildren */
    uint8  u8MaxDepth;                                      /**< nwkMaxDepth */
    uint8  u8PassiveAckTimeout;                             /**< nwkPassiveAckTimeout (in half-seconds) */ /* Potential const */
    uint8  u8MaxBroadcastRetries;                           /**< nwkMaxBroadcastRetries */ /* Potential const */
    uint8  u8MaxSourceRoute;                                /**< nwkMaxSourceRoute */
    uint8  u8NetworkBroadcastDeliveryTime;                  /**< nwkNetworkBroadcastDeliveryTime */ /* Potential const */
    uint8  u8UniqueAddr;                                    /**< nwkUniqueAddr */
    uint8  u8AddrAlloc;                                     /**< nwkAddrAlloc */
    uint8  u8UseTreeRouting;                                /**< nwkUseTreeRouting: bool */
    uint8  u8SymLink;                                       /**< nwkSymLink: bool */
    uint8  u8UseMulticast;                                  /**< nwkUseMulticast: bool */
    uint8  u8LinkStatusPeriod;                              /**< nwkLinkStatusPeriod */
    uint8  u8RouterAgeLimit;                                /**< nwkRouterAgeLimit */
    uint8  u8RouteDiscoveryRetriesPermitted;                /**< nwkRouteDiscoveryRetriesPermitted */
    uint8  u8VsFormEdThreshold;                             /**< Vendor specific - energy detect threshold for network forming */
    uint8  u8SecurityLevel;                                 /**< nwkSecurityLevel */ /* Potential const */   /* [I SP001379_sr 67] */
    uint8  u8AllFresh;                                      /**< nwkAllFresh */ /* Potential const */        /* [I SP001379_sr 67] */
    uint8  u8SecureAllFrames;                               /**< nwkSecureAllFrames */ /* Potential const */ /* [I SP001379_sr 67] */
    uint8  u8VsTxFailThreshold;                             /**< Vendor specific - transmit fail threshold when RT nxt hop gets cleared */
    uint8  u8VsMaxOutgoingCost;                             /**< Vendor specific - only uses links with cost >= than this */
    uint8  u8VsLeaveRejoin;
    uint8  u8ZedTimeout;
    uint8  u8ZedTimeoutDefault;                              /** nwkEndDeviceTimeoutDefault **/
    uint16 u16VerifyLinkCostTransmitRate;                   /** nwkVerifyLinkCostTransmitRate 0-65535 **/

    /**** Initial values described in struct above end here ****/
    uint8  u8ReportConstantCost;                            /**< nwkReportConstantCost */ /* Potential const */
    uint8  u8IsConcentrator;                                /**< nwkIsConcentrator: bool */
    uint8  u8ConcentratorRadius;                            /**< nwkConcentratorRadius */
    uint8  u8ConcentratorDiscoveryTime;                     /**< nwkConcentratorDiscoveryTime */
    uint8  u8SequenceNumber;                                /**< nwkSequenceNumber */
    uint16 u16ManagerAddr;                                  /**< nwkManagerAddr */
    uint16 u16VsFixedAlcAddr;                               /**< Vendor specific - fixed allocated address */
    uint64 u64VsLastPanIdConflict;                          /**< Vendor specific - EPID of other network in last Pan Id conflict detection */
    uint16 u16TxTotal;                                      /**< nwkTxTotal */
    uint16 u16BufferedSuccess;                              /**< Frames buffered and later sent successfully */
    uint16 u16RouteDiscInitiated;                           /**< Diagnostics - counter of initiated Route Disc and Route request identifier */
    uint16 u16NeighborAddedDisc;                            /**< Diagnostics - neighbor add operations in Discovery context */
    uint16 u16NeighborAdded;                                /**< Diagnostics - neighbor add operations */
    uint16 u16NeighborRemoved;                              /**< Diagnostics - neighbor remove operations */
    uint16 u16NeighborStale;                                /**< Diagnostics - neighbor stale detections */
    uint16 u16JoinIndication;                               /**< Diagnostics - join indication operations */
    uint16 u16RelayedUcast;                                 /**< Diagnostics - unicast packets relayed */
    uint16 u16FCFailure;                                    /**< Diagnostics - Frame Counter failures */
    uint16 u16DecryptFailure;                               /**< Diagnostics - frames failed to decrypt */
    uint16 u16PacketValidateDropCount;                      /**< Diagnostics - packets dropped as invalid */
    uint16 u16PacketBufferAllocateFailure;                  /**< Diagnostics - packet buffer allocation failures */
    uint16 u16LinkStatSuccess;                              /**< Diagnostics - Link Status sent successfully */
    uint16 u16LinkStatFailure;                              /**< Diagnostics - Link Status failure */
    uint8  u8LinkStatusLastError;                           /**< Last known status of a scheduled Link Status */
    uint32 u32LinkStatusLastTimestamp;                      /**< Last timestamp of a successful TXed Link Status */
    uint16 u16RReqRetry;                                    /**< TX Route Request broadcast retries */
    uint16 u16RReqProcDropped;                              /**< RX Route Request processing dropped */
    uint16 u16RReqProcCompleted;                            /**< RX Route Request processing completed */
    uint16 u16DfcmDropped;                                  /**< Timed deferred confirms not delivered due to overflow */

    /**** Tables ****/
    zps_tsNwkSlist       sActvSortedList;                   /**< Linked list of sorted NT entries */
    ZPS_tsNwkNibTblSize  sTblSize;                          /**< Table sizes */
    ZPS_tsNwkNibTbl      sTbl;                              /**< Tables */
    ZPS_tsNWkNibPersist  sPersist;                          /**< Members of NIB which require persisting */

    /* The network address map is done like this as an entry containing a uint16 and uint64 is very inefficient */
} ZPS_tsNwkNib;




typedef enum
{
    ZPS_NWK_NEIGHBOR = 0,
    ZPS_NWK_MAP = 1,
    ZPS_NWK_FULL = 2,
} ZPS_teNwkMacTableAccess;


/****************************/
/**** EXPORTED VARIABLES ****/
/****************************/

/****************************/
/**** EXPORTED FUNCTIONS ****/
/****************************/
PUBLIC uint32 zps_u32GetNwkVersion(void);
PUBLIC ZPS_tsNwkNib *
ZPS_psNwkNibGetHandle(void *pvNwk);

PUBLIC void
ZPS_vNwkNibClearTables(void *pvNwk);

PUBLIC void
ZPS_vNwkNibClearDiscoveryNT(void *pvNwk);

PUBLIC uint16
ZPS_u16NwkNibGetNwkAddr(void *pvNwk);

PUBLIC void
ZPS_vNwkNibSetNwkAddr(void *pvNwk, uint16 u16NwkAddr);

PUBLIC void
ZPS_vNwkNibSetNwkAddrNibOnly(void *pvNwk, uint16 u16NwkAddr);

PUBLIC uint64
ZPS_u64NwkNibGetExtAddr(void *pvNwk);

PUBLIC void
ZPS_vNwkNibSetExtPanId(void *pvNwk,
                       uint64 u64ExtPanId);

PUBLIC bool_t
ZPS_bNwkNibAddrMapAddEntry(void *pvNwk,
                           uint16 u16NwkAddr,
                           uint64 u64ExtAddr,bool_t bCheckNeighborTable);

PUBLIC bool_t
ZPS_bNwkNibNtActvMatch(void *pvNwk,
                       uint8 u8MatchMask,
                       uint16 u16NwkAddr,
                       uint64 u64ExtAddr,
                       ZPS_tsNwkActvNtEntry **ppsActvNtEntry);

PUBLIC bool_t
ZPS_bNwkNibNtActvUnusedChild (
	void *pvNwk,
	uint8 *i,
	uint8 j);

PUBLIC bool_t
ZPS_bNwkNibNtActvFindChild (
    void *pvNwk,
    bool_t bFindChildEntry,
    uint8 u8MatchMask,
    uint64 u16NwkAddr,
    uint64 u64ExtAddr,
    ZPS_tsNwkActvNtEntry **ppsActvNtEntry);

PUBLIC uint8
ZPS_u8NwkNibNtDiscMatch(void *pvNwk,
                        uint64 u64ExtPanId,
                        ZPS_teNwkRejoin eRejoin,
                        ZPS_tsNwkDiscNtEntry **ppsDiscNtEntry);

PUBLIC uint64
ZPS_u64NwkNibFindExtAddr(void *pvNwk,
                         uint16 u16NwkAddr);

PUBLIC uint16
ZPS_u16NwkNibFindNwkAddr(void *pvNwk,
                         uint64 u64ExtAddr);
PUBLIC void ZPS_vNwkNibNtDiscSetPParent(
        void *pvNwk,
        uint64 u64ExtPanId);

PUBLIC void
ZPS_vNwkNibCheckAndUpdateAddresses(void *pvNwk,
                                   uint16 u16NwkAddr,
                                   uint64 u64ExtAddr,bool_t bZgp);

PUBLIC void ZPS_vNwkNibCheckAndUpdateChildren(void *pvNwk,
                                   uint16 u16NwkAddr,
                                   uint64 u64ExtAddr,
                                   uint8  u8Capability);

PUBLIC void
ZPS_vNwkNibSetKeySeqNum(void *pvNwk,
                        uint8 u8ActiveKeySeqNumber);

PUBLIC void
ZPS_vNwkNibSetDepth(void *pvNwk,
                    uint8 u8Depth);

PUBLIC void
ZPS_vNwkNibIncSeqNum(void *pvNwk);

PUBLIC void
ZPS_vNwkNibSetUpdateId(void *pvNwk,
                       uint8 u8UpdateId);

PUBLIC void
ZPS_vNwkNibSetPanId(void *pvNwk,
                    uint16 u16PanId);

PUBLIC void
ZPS_vNwkNibSetCapabilityInfo(void *pvNwk,
                             uint8 u8CapabilityInfo);

PUBLIC void
ZPS_vNwkNibSetChannel(void *pvNwk,
                      uint8 u8Channel);

PUBLIC void
ZPS_vNwkNibSetAuxChannel(void *pvNwk,
	                     uint8 u8Channel);


PUBLIC void
ZPS_vNwkNibSetMacEnhancedMode(void *pvNwk,
	                          bool_t bEnhanced);


PUBLIC void
ZPS_vNwkNibSetParentAddr(void *pvNwk,
                         uint16 u16ParentAddr);

PUBLIC void
ZPS_vNwkSaveNt(void *pvNwk);

PUBLIC void
ZPS_vNwkSaveNib(void *pvNwk);

PUBLIC void ZPS_vNwkSaveSecMat(void *pvNwk);


PUBLIC void ZPS_vNwkIncOutFC(void *pvNwk);

PUBLIC uint64
ZPS_u64NwkNibGetEpid(void *pvNwk);

PUBLIC void
ZPS_vNtSetAuthStatus(void *pvNwk,
                     ZPS_tsNwkActvNtEntry *psActvNtEntry,
                     bool_t bStatus);

PUBLIC void
ZPS_vNtSetUsedStatus(void *pvNwk,
                     ZPS_tsNwkActvNtEntry *psActvNtEntry,
                     bool_t bStatus);

PUBLIC uint16 ZPS_u16NwkNibGetMacPanId(void *pvNwk);

PUBLIC void ZPS_vNwkNibSetLeaveRejoin(void *pvNwk, bool_t bRejoin);
PUBLIC void ZPS_vNwkNibSetLeaveAllowed(void *pvNwk, bool_t bLeave);
PUBLIC void zps_vRemoveRoute(void * pvNwk, uint16 u16NwkAddr);
PUBLIC void zps_vRemoveNextHop(void * pvNwk, uint16 u16NwkAddr);
PUBLIC void zps_vStartRouter(void *pvNwk,
                             ZPS_tsNwkNlmeReqRsp *psNlmeReqRsp,
                             ZPS_tsNwkNlmeSyncCfm *psNlmeSyncCfm);

PUBLIC void ZPS_vNwkNibSetTables (
    void *pvNwk,
    ZPS_tsNwkNibTblSize *psTblSize,
    ZPS_tsNwkNibTbl *psTbl);
PUBLIC void zps_vBuildSortedActvNTList(void* pvNwk);
PUBLIC bool_t zps_bNwkFindAddIeeeAddr(void *pvNwk,uint64 u64MacAddress, uint16 *pu16Location, ZPS_teNwkMacTableAccess eTableAccess);
PUBLIC void ZPS_vRemoveEntryFromAddressMap(void *pvNwk, uint16 u16ShortAddress);
PUBLIC uint64 ZPS_u64NwkNibGetMappedIeeeAddr(void* pvNwk, uint16 u16Location);
PUBLIC void ZPS_vNwkTimerQueueOverflow(ZPS_tsTsvTimerInfo *psTimerInfo);
PUBLIC void ZPS_vNwkSendNwkStatusCommand(void *pvNwk, uint16 u16DstAddress, uint16 u16TargetAddress, uint8 u8CommandId, uint8 u8Radius);
PUBLIC uint8 ZPS_u8NwkSendNwkEdTimeoutsCommand(void *pvNwk);
PUBLIC bool_t ZPS_bNwkSetEnddeviceTimeout(void *pvNwk, uint8 u8Timeout);
PUBLIC uint8 ZPS_u8NwkGetEndDeviceTimeoutMethod(void *pvNwk);
PUBLIC bool_t ZPS_bNwkGetAliasStatus( void *pvNwk);
PUBLIC uint8 ZPS_bNwkGetAliasApsCounter( void *pvNwk);
PUBLIC void zps_vDefaultNwkLayer(void* pvNwk);
PUBLIC void ZPS_vNwkInitialiseRecords(void* pvNwk);
PUBLIC uint8 ZPS_u8ReturnNwkState(void *pvNwk);
PUBLIC void ZPS_vSetNwkStateIdle ( void    *pvNwk );
PUBLIC void ZPS_vSetNwkStateActive ( void    *pvNwk );
PUBLIC void ZPS_vNwkSetParentTimeoutMethod(void *pvNwk, uint8 u8Value);
PUBLIC bool_t zps_bIsDeviceMyNeighbour( void *pvNwk, uint16 u16ShortAddress );
PUBLIC void ZPS_vNwkPurgeMacAddressAndRelatedEntries ( void* pvNwk,uint64 u64MacAddress, bool_t bMacRemove);
PUBLIC void ZPS_vNwkSetLocalDeviceTimeout(void *pvNwk, uint16 u16Short, uint8 u8Value);
PUBLIC bool_t ZPS_bNibNwkGetTxCount ( void      *pvNwk,
                                      uint16    u16ShortAddress,
                                      uint32    *pu32Count );
PUBLIC bool_t ZPS_bNibNwkGetRxCount ( void      *pvNwk,
                                      uint16    u16ShortAddress,
                                      uint32    *pu32Count );
PUBLIC bool_t ZPS_bNibNwkClearTxCount ( void    *pvNwk,
                                        uint16 u16ShortAddress );
PUBLIC bool_t ZPS_bNibNwkClearRxCount ( void    *pvNwk,
		                                uint16 u16ShortAddress );
PUBLIC void ZPS_vNwkUpdateTxBytesForNeighbor ( void         *pvNwk,
                                               uint16       u16NwkAddress,
                                               uint32       u32ByteCount );

PUBLIC void ZPS_vNwkUpdateRxBytesForNeighbor ( void         *pvNwk,
                                               uint16       u16NwkAddress,
                                               uint32       u32ByteCount );


#define ZPS_MIB_IEEE_LIST_SIZE	16
typedef struct
{
	uint16  u16ShortAddress;
	int8    i8PowerDelta;

} ZPS_tsNwkLinkPowerListEntry;

typedef enum
{
    ZPS_NWK_MIB_IEEE_JOIN_POLICY_ALL = 0,
    ZPS_NWK_MIB_IEEE_JOIN_POLICY_LIST = 1,
    ZPS_NWK_MIB_IEEE_JOIN_POLICY_NONE = 2,

} ZPS_teNwkMibIeeeJoinPolicy;


/* TX Power functions */
MODULE bool_t ZPS_bNwkEndDeviceLinkPowerDeltaReq(void *pvNwk);
MODULE bool_t ZPS_bNwkEndDeviceLinkPowerDeltaNtfn(void *pvNwk);
MODULE bool_t ZPS_bNwkCoordLinkPowerDeltaNtfcn(void *pvNwk);

MODULE void   ZPS_vLinkPowerDeltaTimerExpiry(void *pvNwk);
MODULE void   ZPS_vNwkNibNTUpdateTxPower(void *pvNwk);
MODULE bool_t ZPS_bMACTxPowerTableInitialise(void *pvNwk);
PUBLIC void ZPS_vNwkMacEntriesClear ( void* pvNwk );

PUBLIC void ZPS_vPurgeJoinManagerEntry (void *pvNwk );
PUBLIC void ZPS_vNibRealignMac (void* pvNwk);
PUBLIC bool_t ZPS_bCheckMacMisaligned (void* pvNwk);
PUBLIC void ZPS_vNwkNibRestoreWarmRestartCounter(void* pvNwk);
PUBLIC void ZPS_vNwkLinkCostCallbackRegister ( void* pvFn );
PUBLIC void ZPS_vNwkSetAddrConflictCallback ( void* pvfn );
PUBLIC void ZPS_vNwkSetOverrideConflictBehaviour (bool_t bResolvedConflictAllowed,
                                                  bool_t bNwkManagerDetectConflict);
void ZPS_vNwkSinkAllDataForChildList(uint16* pu16DstList, uint16 u16Size);
#ifdef WWAH_SUPPORT
PUBLIC void ZPS_vAplSetResolutionPanId ( uint16 u16PanId );
PUBLIC uint16 ZPS_u16AplGetResolutionPanId ( void );
PUBLIC void ZPS_vNwkNibSetPriorityParent( uint8 u8BitMask );
PUBLIC void ZPS_vNwkNibClearPriorityParent( uint8 u8BitMask );
#endif

#ifdef __cplusplus
};
#endif

#endif /* _zps_nwk_nib_h_ */

/* End of file **************************************************************/
