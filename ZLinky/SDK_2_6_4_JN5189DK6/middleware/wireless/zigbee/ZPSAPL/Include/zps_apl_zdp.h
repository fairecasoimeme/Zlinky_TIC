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
 * MODULE:      Zigbee Protocol Stack Application Layer
 *
 * COMPONENT:   zps_apl_zdp.h
 *
 * DESCRIPTION: Zigbee Device Profile (ZDP) interface
 *
 *****************************************************************************/

#ifndef ZPS_APL_ZDP_H_
#define ZPS_APL_ZDP_H_

#include <zps_apl.h>
#include <pdum_apl.h>
#include <zps_apl_zdo.h>

#ifndef ZPS_APL_INLINE
#define ZPS_APL_INLINE  INLINE
#endif

#ifndef ZPS_ZDP_ALWAYS_INLINE
#define ZPS_ZDP_ALWAYS_INLINE  ALWAYS_INLINE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/****************************************************************************/
/* ZDP Requests                                                             */

#define ZPS_ZDP_NWK_ADDR_REQ_CLUSTER_ID                     (0x0000)    /* [ISP001377_sfr 98] */
#define ZPS_ZDP_IEEE_ADDR_REQ_CLUSTER_ID                    (0x0001)    /* [ISP001377_sfr 99] */
#define ZPS_ZDP_NODE_DESC_REQ_CLUSTER_ID                    (0x0002)    /* [ISP001377_sfr 100] */
#define ZPS_ZDP_POWER_DESC_REQ_CLUSTER_ID                   (0x0003)    /* [ISP001377_sfr 101] */
#define ZPS_ZDP_SIMPLE_DESC_REQ_CLUSTER_ID                  (0x0004)    /* [ISP001377_sfr 102] */
#define ZPS_ZDP_ACTIVE_EP_REQ_CLUSTER_ID                    (0x0005)    /* [ISP001377_sfr 103] */
#define ZPS_ZDP_MATCH_DESC_REQ_CLUSTER_ID                   (0x0006)    /* [ISP001377_sfr 104] */
#define ZPS_ZDP_COMPLEX_DESC_REQ_CLUSTER_ID                 (0x0010)    /* [ISP001377_sfr 105] */
#define ZPS_ZDP_USER_DESC_REQ_CLUSTER_ID                    (0x0011)    /* [ISP001377_sfr 106] */
#define ZPS_ZDP_DISCOVERY_CACHE_REQ_CLUSTER_ID              (0x0012)    /* [ISP001377_sfr 107] */
#define ZPS_ZDP_DEVICE_ANNCE_REQ_CLUSTER_ID                 (0x0013)    /* [ISP001377_sfr 108] */
#define ZPS_ZDP_USER_DESC_SET_REQ_CLUSTER_ID                (0x0014)    /* [ISP001377_sfr 109] */
#define ZPS_ZDP_SYSTEM_SERVER_DISCOVERY_REQ_CLUSTER_ID      (0x0015)    /* [ISP001377_sfr 110] */
#define ZPS_ZDP_DISCOVERY_STORE_REQ_CLUSTER_ID              (0x0016)    /* [ISP001377_sfr 111] */
#define ZPS_ZDP_NODE_DESC_STORE_REQ_CLUSTER_ID              (0x0017)    /* [ISP001377_sfr 112] */
#define ZPS_ZDP_POWER_DESC_STORE_REQ_CLUSTER_ID             (0x0018)    /* [ISP001377_sfr 113] */
#define ZPS_ZDP_ACTIVE_EP_STORE_REQ_CLUSTER_ID              (0x0019)    /* [ISP001377_sfr 114] */
#define ZPS_ZDP_SIMPLE_DESC_STORE_REQ_CLUSTER_ID            (0x001a)    /* [ISP001377_sfr 115] */
#define ZPS_ZDP_REMOVE_NODE_CACHE_REQ_CLUSTER_ID            (0x001b)    /* [ISP001377_sfr 116] */
#define ZPS_ZDP_FIND_NODE_CACHE_REQ_CLUSTER_ID              (0x001c)    /* [ISP001377_sfr 117] */
#define ZPS_ZDP_EXTENDED_SIMPLE_DESC_REQ_CLUSTER_ID         (0x001d)    /* [ISP001377_sfr 118] */
#define ZPS_ZDP_EXTENDED_ACTIVE_EP_REQ_CLUSTER_ID           (0x001e)    /* [ISP001377_sfr 119] */
#define ZPS_ZDP_END_DEVICE_BIND_REQ_CLUSTER_ID              (0x0020)    /* [ISP001377_sfr 120] */
#define ZPS_ZDP_BIND_REQ_CLUSTER_ID                         (0x0021)    /* [ISP001377_sfr 121] */
#define ZPS_ZDP_UNBIND_REQ_CLUSTER_ID                       (0x0022)    /* [ISP001377_sfr 122] */
#define ZPS_ZDP_BIND_REGISTER_REQ_CLUSTER_ID                (0x0023)    /* [ISP001377_sfr 123] */
#define ZPS_ZDP_REPLACE_DEVICE_REQ_CLUSTER_ID               (0x0024)    /* [ISP001377_sfr 124] */
#define ZPS_ZDP_STORE_BKUP_BIND_ENTRY_REQ_CLUSTER_ID        (0x0025)    /* [ISP001377_sfr 125] */
#define ZPS_ZDP_REMOVE_BKUP_BIND_ENTRY_REQ_CLUSTER_ID       (0x0026)    /* [ISP001377_sfr 126] */
#define ZPS_ZDP_BACKUP_BIND_TABLE_REQ_CLUSTER_ID            (0x0027)    /* [ISP001377_sfr 127] */
#define ZPS_ZDP_RECOVER_BIND_TABLE_REQ_CLUSTER_ID           (0x0028)    /* [ISP001377_sfr 128] */
#define ZPS_ZDP_BACKUP_SOURCE_BIND_REQ_CLUSTER_ID           (0x0029)    /* [ISP001377_sfr 129] */
#define ZPS_ZDP_RECOVER_SOURCE_BIND_REQ_CLUSTER_ID          (0x002a)    /* [ISP001377_sfr 130] */
#define ZPS_ZDP_MGMT_NWK_DISC_REQ_CLUSTER_ID                (0x0030)    /* [ISP001377_sfr 131] */
#define ZPS_ZDP_MGMT_LQI_REQ_CLUSTER_ID                     (0x0031)    /* [ISP001377_sfr 132] */
#define ZPS_ZDP_MGMT_RTG_REQ_CLUSTER_ID                     (0x0032)    /* [ISP001377_sfr 133] */
#define ZPS_ZDP_MGMT_BIND_REQ_CLUSTER_ID                    (0x0033)    /* [ISP001377_sfr 134] */
#define ZPS_ZDP_MGMT_LEAVE_REQ_CLUSTER_ID                   (0x0034)    /* [ISP001377_sfr 135] */
#define ZPS_ZDP_MGMT_DIRECT_JOIN_REQ_CLUSTER_ID             (0x0035)    /* [ISP001377_sfr 136] */
#define ZPS_ZDP_MGMT_PERMIT_JOINING_REQ_CLUSTER_ID          (0x0036)    /* [ISP001377_sfr 137] */
#define ZPS_ZDP_MGMT_CACHE_REQ_CLUSTER_ID                   (0x0037)    /* [ISP001377_sfr 138] */
#define ZPS_ZDP_MGMT_NWK_UPDATE_REQ_CLUSTER_ID              (0x0038)    /* [ISP001377_sfr 139] */

#define ZPS_ZDP_PARENT_ANNCE_CLUSTER_ID                     (0x001F)
#define ZPS_ZDP_MAX_CHILD_IN_PARENT_ANNCE                   (10)
#define ZPS_ZDP_MGMT_NWK_ENHANCE_UPDATE_REQ_CLUSTER_ID      (0x0039)    /* [ISP001377_sfr 139] */ /* TO BE UPDATED ONCE ID IS DEFINED */
#define ZPS_ZDP_MGMT_MIB_IEEE_REQ_CLUSTER_ID				(0x003A)

/****************************************************************************/
/* ZDP Responses                                                            */

#define ZPS_ZDP_NWK_ADDR_RSP_CLUSTER_ID                     (0x8000)    /* [ISP001377_sfr 140] */
#define ZPS_ZDP_IEEE_ADDR_RSP_CLUSTER_ID                    (0x8001)    /* [ISP001377_sfr 141] */
#define ZPS_ZDP_NODE_DESC_RSP_CLUSTER_ID                    (0x8002)    /* [ISP001377_sfr 142] */
#define ZPS_ZDP_POWER_DESC_RSP_CLUSTER_ID                   (0x8003)    /* [ISP001377_sfr 143] */
#define ZPS_ZDP_SIMPLE_DESC_RSP_CLUSTER_ID                  (0x8004)    /* [ISP001377_sfr 144] */
#define ZPS_ZDP_ACTIVE_EP_RSP_CLUSTER_ID                    (0x8005)    /* [ISP001377_sfr 145] */
#define ZPS_ZDP_MATCH_DESC_RSP_CLUSTER_ID                   (0x8006)    /* [ISP001377_sfr 146] */
#define ZPS_ZDP_COMPLEX_DESC_RSP_CLUSTER_ID                 (0x8010)    /* [ISP001377_sfr 147] */
#define ZPS_ZDP_USER_DESC_RSP_CLUSTER_ID                    (0x8011)    /* [ISP001377_sfr 148] */
#define ZPS_ZDP_DISCOVERY_CACHE_RSP_CLUSTER_ID              (0x8012)    /* [ISP001377_sfr 151] */
#define ZPS_ZDP_USER_DESC_CONF_RSP_CLUSTER_ID               (0x8014)    /* [ISP001377_sfr 150] */
#define ZPS_ZDP_SYSTEM_SERVER_DISCOVERY_RSP_CLUSTER_ID      (0x8015)    /* [ISP001377_sfr 149] */
#define ZPS_ZDP_DISCOVERY_STORE_RSP_CLUSTER_ID              (0x8016)    /* [ISP001377_sfr 152] */
#define ZPS_ZDP_NODE_DESC_STORE_RSP_CLUSTER_ID              (0x8017)    /* [ISP001377_sfr 153] */
#define ZPS_ZDP_POWER_DESC_STORE_RSP_CLUSTER_ID             (0x8018)    /* [ISP001377_sfr 154] */
#define ZPS_ZDP_ACTIVE_EP_STORE_RSP_CLUSTER_ID              (0x8019)    /* [ISP001377_sfr 155] */
#define ZPS_ZDP_SIMPLE_DESC_STORE_RSP_CLUSTER_ID            (0x801a)    /* [ISP001377_sfr 273] */
#define ZPS_ZDP_REMOVE_NODE_CACHE_RSP_CLUSTER_ID            (0x801b)    /* [ISP001377_sfr 156] */
#define ZPS_ZDP_FIND_NODE_CACHE_RSP_CLUSTER_ID              (0x801c)    /* [ISP001377_sfr 157] */
#define ZPS_ZDP_EXTENDED_SIMPLE_DESC_RSP_CLUSTER_ID         (0x801d)    /* [ISP001377_sfr 158] */
#define ZPS_ZDP_EXTENDED_ACTIVE_EP_RSP_CLUSTER_ID           (0x801e)    /* [ISP001377_sfr 159] */
#define ZPS_ZDP_END_DEVICE_BIND_RSP_CLUSTER_ID              (0x8020)    /* [ISP001377_sfr 160] */
#define ZPS_ZDP_BIND_RSP_CLUSTER_ID                         (0x8021)    /* [ISP001377_sfr 161] */
#define ZPS_ZDP_UNBIND_RSP_CLUSTER_ID                       (0x8022)    /* [ISP001377_sfr 162] */
#define ZPS_ZDP_BIND_REGISTER_RSP_CLUSTER_ID                (0x8023)    /* [ISP001377_sfr 163] */
#define ZPS_ZDP_REPLACE_DEVICE_RSP_CLUSTER_ID               (0x8024)    /* [ISP001377_sfr 164] */
#define ZPS_ZDP_STORE_BKUP_BIND_RSP_CLUSTER_ID              (0x8025)    /* [ISP001377_sfr 165] */
#define ZPS_ZDP_REMOVE_BKUP_BIND_RSP_CLUSTER_ID             (0x8026)    /* [ISP001377_sfr 166] */
#define ZPS_ZDP_BKUP_BIND_TABLE_RSP_CLUSTER_ID              (0x8027)    /* [ISP001377_sfr 167] */
#define ZPS_ZDP_RECOVER_BIND_TABLE_RSP_CLUSTER_ID           (0x8028)    /* [ISP001377_sfr 168] */
#define ZPS_ZDP_BKUP_SOURCE_BIND_RSP_CLUSTER_ID             (0x8029)    /* [ISP001377_sfr 169] */
#define ZPS_ZDP_RECOVER_SOURCE_BIND_RSP_CLUSTER_ID          (0x802a)    /* [ISP001377_sfr 170] */
#define ZPS_ZDP_MGMT_NWK_DISC_RSP_CLUSTER_ID                (0x8030)    /* [ISP001377_sfr 171] */
#define ZPS_ZDP_MGMT_LQI_RSP_CLUSTER_ID                     (0x8031)    /* [ISP001377_sfr 172] */
#define ZPS_ZDP_MGMT_RTG_RSP_CLUSTER_ID                     (0x8032)    /* [ISP001377_sfr 173] */
#define ZPS_ZDP_MGMT_BIND_RSP_CLUSTER_ID                    (0x8033)    /* [ISP001377_sfr 174] */
#define ZPS_ZDP_MGMT_LEAVE_RSP_CLUSTER_ID                   (0x8034)    /* [ISP001377_sfr 175] */
#define ZPS_ZDP_MGMT_DIRECT_JOIN_RSP_CLUSTER_ID             (0x8035)    /* [ISP001377_sfr 176] */
#define ZPS_ZDP_MGMT_PERMIT_JOINING_RSP_CLUSTER_ID          (0x8036)    /* [ISP001377_sfr 177] */
#define ZPS_ZDP_MGMT_CACHE_RSP_CLUSTER_ID                   (0x8037)    /* [ISP001377_sfr 178] */
#define ZPS_ZDP_MGMT_NWK_UPDATE_NOTIFY_CLUSTER_ID           (0x8038)    /* [ISP001377_sfr 179] */

#define ZPS_ZDP_PARENT_ANNCE_RSP_CLUSTER_ID                 (0x801F)

#define ZPS_ZDP_MGMT_NWK_ENHANCE_UPDATE_NOTIFY_CLUSTER_ID   (0x8039)    /* [ISP001377_sfr 179] */
#define ZPS_ZDP_MGMT_MIB_IEEE_RSP_CLUSTER_ID				(0x803A)
#define ZPS_ZDP_MGMT_NWK_UNSOLICITED_ENHANCE_UPDATE_NOTIFY_CLUSTER_ID   (0x803B)

/* Maximum payload size possible for a ZDP message */
#define ZPS_APL_ZDP_MAX_FRAME_SIZE      	50

/* Maximum size of a user descriptor */
#define ZPS_ZDP_LENGTH_OF_USER_DESC         16

/* route table entry size is 5 bytes (see Table 2.128 in spec) */
#define ZPS_APL_ZDP_MAX_NUM_MGMT_RTG_RSP_ROUTE_TABLE_ENTRIES 7

#define ZPS_APL_ZDP_MAX_MIB_IEEE_LIST		9		/* 64 Bytes + seq number + 5 bytes Rsp = 70 bytes */

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum {
    ZPS_APL_ZDP_E_INV_REQUESTTYPE     			= 0x80,
    ZPS_APL_ZDP_E_DEVICE_NOT_FOUND    			= 0x81,
    ZPS_APL_ZDP_E_INVALID_EP          			= 0x82,
    ZPS_APL_ZDP_E_NOT_ACTIVE          			= 0x83,
    ZPS_APL_ZDP_E_NOT_SUPPORTED       			= 0x84,
    ZPS_APL_ZDP_E_TIMEOUT             			= 0x85,
    ZPS_APL_ZDP_E_NO_MATCH            			= 0x86,
    ZPS_APL_ZDP_E_NO_ENTRY            			= 0x88,
    ZPS_APL_ZDP_E_NO_DESCRIPTOR       			= 0x89,
    ZPS_APL_ZDP_E_INSUFFICIENT_SPACE  			= 0x8a,
    ZPS_APL_ZDP_E_NOT_PERMITTED       			= 0x8b,
    ZPS_APL_ZDP_E_TABLE_FULL          			= 0x8c,
    ZPS_APL_ZDP_E_NOT_AUTHORIZED      			= 0x8d,
    ZPS_APL_ZDP_E_DEVICE_BINDING_TABLE_FULL     = 0x8e,
    ZPS_APL_ZDP_E_INVALID_INDEX       			= 0x8f,
} ZPS_teAplZdpStatus;

/****************************************************************************/
/* ZDP Requests                                                             */
/****************************************************************************/

/* [I SP001377_sfr 15]  */
typedef struct {
    uint64 u64IeeeAddr;
    uint8  u8RequestType;
    uint8  u8StartIndex;
} ZPS_tsAplZdpNwkAddrReq;

/* [I SP001377_sfr 16]  */
typedef struct {
    uint16 u16NwkAddrOfInterest;
    uint8  u8RequestType;
    uint8  u8StartIndex;
} ZPS_tsAplZdpIeeeAddrReq;

/* [I SP001377_sfr 17]  */
typedef struct {
    uint16 u16NwkAddrOfInterest;
} ZPS_tsAplZdpNodeDescReq;

/* [I SP001377_sfr 18]  */
typedef struct {
    uint16 u16NwkAddrOfInterest;
} ZPS_tsAplZdpPowerDescReq;

/* [I SP001377_sfr 19]  */
typedef struct {
    uint16 u16NwkAddrOfInterest;
    uint8  u8EndPoint;
} ZPS_tsAplZdpSimpleDescReq;

/* [I SP001377_sfr 20]  */
typedef struct {
    uint16 u16NwkAddrOfInterest;
} ZPS_tsAplZdpActiveEpReq;

/* [I SP001377_sfr 21]  */
typedef struct {
    uint16 u16NwkAddrOfInterest;
    uint16 u16ProfileId;
    /* rest of message is variable length */
    uint8  u8NumInClusters;
    uint16* pu16InClusterList;
    uint8  u8NumOutClusters;
    uint16* pu16OutClusterList;
} ZPS_tsAplZdpMatchDescReq;

/* [I SP001377_sfr 22]  */
typedef struct {
    uint16 u16NwkAddrOfInterest;
} ZPS_tsAplZdpComplexDescReq;

/* [I SP001377_sfr 23]  */
typedef struct {
    uint16 u16NwkAddrOfInterest;
} ZPS_tsAplZdpUserDescReq;

/* [I SP001377_sfr 24]  */
typedef struct {
    uint16 u16NwkAddr;
    uint64 u64IeeeAddr;
} ZPS_tsAplZdpDiscoveryCacheReq;

/* [I SP001377_sfr 25]  */
typedef struct {
    uint16 u16NwkAddr;
    uint64 u64IeeeAddr;
    uint8  u8Capability;
} ZPS_tsAplZdpDeviceAnnceReq;


/* [I SP001377_sfr 26]  */
typedef struct {
    uint16 u16NwkAddrOfInterest;
    uint8  u8Length;
    char szUserDescriptor[ZPS_ZDP_LENGTH_OF_USER_DESC];
} ZPS_tsAplZdpUserDescSet;

/* [I SP001377_sfr 27]  */
typedef struct {
    uint16 u16ServerMask;
} ZPS_tsAplZdpSystemServerDiscoveryReq;

/* [I SP001377_sfr 28]  */
typedef struct {
    uint16 u16NwkAddr;
    uint64 u64IeeeAddr;
    uint8  u8NodeDescSize;
    uint8  u8PowerDescSize;
    uint8  u8ActiveEpSize;
    uint8  u8SimpleDescCount;
    /* rest of message is variable length */
    uint8* pu8SimpleDescSizeList;
} ZPS_tsAplZdpDiscoveryStoreReq;

#ifndef LITTLE_ENDIAN_PROCESSOR
typedef struct {
    /* ZPS_teZdoDeviceType */
    unsigned eFrequencyBand    : 5;
    unsigned eApsFlags         : 3;
    unsigned eReserved         : 3; /* reserved */
    unsigned bUserDescAvail    : 1;
    unsigned bComplexDescAvail : 1;
    unsigned eLogicalType      : 3;
}ZPS_tsAplZdpNodeDescBitFields;
#else
typedef struct {
    /* ZPS_teZdoDeviceType */
    unsigned eLogicalType      : 3;
    unsigned bComplexDescAvail : 1;
    unsigned bUserDescAvail    : 1;
    unsigned eReserved         : 3; /* reserved */
    unsigned eApsFlags         : 3;
    unsigned eFrequencyBand    : 5;
}ZPS_tsAplZdpNodeDescBitFields;
#endif
typedef struct {
    union
    {
        ZPS_tsAplZdpNodeDescBitFields sBitFields;
        uint16 u16Value;
    } uBitUnion;
    uint8 u8MacFlags;
    uint16 u16ManufacturerCode;
    uint8 u8MaxBufferSize;
    uint16 u16MaxRxSize;
    uint16 u16ServerMask;
    uint16 u16MaxTxSize;
    uint8 u8DescriptorCapability;
} ZPS_tsAplZdpNodeDescriptor;

/* [I SP001377_sfr 29]  */
typedef struct {
    uint16 u16NwkAddr;
    uint64 u64IeeeAddr;
    /* rest of message is variable length */
    ZPS_tsAplZdpNodeDescriptor sNodeDescriptor;
} ZPS_tsAplZdpNodeDescStoreReq;

#ifdef LITTLE_ENDIAN_PROCESSOR
typedef struct {
    unsigned eCurrentPowerMode          : 4;
    unsigned eAvailablePowerSource      : 4;
    unsigned eCurrentPowerSource        : 4;
    unsigned eCurrentPowerSourceLevel   : 4;
}ZPS_tsAplZdpPowerDescBitFields;
#else
typedef struct {
    unsigned eCurrentPowerSourceLevel   : 4;
    unsigned eCurrentPowerSource        : 4;
    unsigned eAvailablePowerSource      : 4;
    unsigned eCurrentPowerMode          : 4;
}ZPS_tsAplZdpPowerDescBitFields;
#endif

typedef struct {
    union
    {
        ZPS_tsAplZdpPowerDescBitFields sBitFields;
        uint16 u16Value;
    }uBitUnion;
} ZPS_tsAplZdpNodePowerDescriptor;

/* [I SP001377_sfr 30]  */
typedef struct {
    uint16 u16NwkAddr;
    uint64 u64IeeeAddr;
    /* rest of message is variable length */
    ZPS_tsAplZdpNodePowerDescriptor sPowerDescriptor;
} ZPS_tsAplZdpPowerDescStoreReq;

/* [I SP001377_sfr 31]  */
typedef struct {
    uint16 u16NwkAddr;
    uint64 u64IeeeAddr;
    uint8  u8ActiveEPCount;
    /* rest of message is variable length */
    uint8* pu8ActiveEpList;
} ZPS_tsAplZdpActiveEpStoreReq;


typedef struct
{
    unsigned eDeviceVersion :4;
    unsigned eReserved      :4;
}ZPS_tsAplZdpSimpleDescBitFields;

typedef struct {

    uint8  u8Endpoint;
    uint16 u16ApplicationProfileId;
    uint16 u16DeviceId;
    union
    {
        ZPS_tsAplZdpSimpleDescBitFields sBitFields;
        uint8 u8Value;
    }uBitUnion;
    uint8  u8InClusterCount;
    uint16* pu16InClusterList;
    uint8  u8OutClusterCount;
    uint16* pu16OutClusterList;
}ZPS_tsAplZdpSimpleDescType;

/* [I SP001377_sfr 32]  */
typedef struct {
    uint16 u16NwkAddr;
    uint64 u64IeeeAddr;
    uint8  u8Length;
    /* rest of message is variable length */
    ZPS_tsAplZdpSimpleDescType sSimpleDescriptor;
} ZPS_tsAplZdpSimpleDescStoreReq;

/* [I SP001377_sfr 33]  */
typedef struct {
    uint16 u16NwkAddr;
    uint64 u64IeeeAddr;
} ZPS_tsAplZdpRemoveNodeCacheReq;

/* [I SP001377_sfr 34]  */
typedef struct {
    uint16 u16NwkAddr;
    uint64 u64IeeeAddr;
} ZPS_tsAplZdpFindNodeCacheReq;

/* [I SP001377_sfr 35]  */
typedef struct {
    uint16 u16NwkAddr;
    uint8  u8EndPoint;
    uint8  u8StartIndex;
} ZPS_tsAplZdpExtendedSimpleDescReq;

/* [I SP001377_sfr 36]  */
typedef struct {
    uint16 u16NwkAddr;
    uint8  u8StartIndex;
} ZPS_tsAplZdpExtendedActiveEpReq;

/* [I SP001377_sfr 37]  */
typedef struct {
    uint16 u16BindingTarget;
    uint64 u64SrcIeeeAddress;
    uint8  u8SrcEndpoint;
    uint16 u16ProfileId;
    /* rest of message is variable length */
    uint8 u8NumInClusters;
    uint16 *pu16InClusterList;
    uint8 u8NumOutClusters;
    uint16 *pu16OutClusterList;
} ZPS_tsAplZdpEndDeviceBindReq;

/* [I SP001377_sfr 38,39]  */
typedef struct {
    uint64 u64SrcAddress;
    uint8  u8SrcEndpoint;
    uint16 u16ClusterId;
    uint8  u8DstAddrMode;
    union {
        struct {
            uint16 u16DstAddress;
        } sShort;
        struct {
            uint64 u64DstAddress;
            uint8  u8DstEndPoint;
        } sExtended;
    } uAddressField;
} ZPS_tsAplZdpBindUnbindReq;

/* [I SP001377_sfr 40]  */
typedef struct {
    uint64 u64NodeAddress;
} ZPS_tsAplZdpBindRegisterReq;

/* [I SP001377_sfr 41]  */
typedef struct {
    uint64 u64OldAddress;
    uint8  u8OldEndPoint;
    uint64 u64NewAddress;
    uint8  u8NewEndPoint;
} ZPS_tsAplZdpReplaceDeviceReq;

/* [I SP001377_sfr 42]  */
typedef struct {
    uint64 u64SrcAddress;
    uint8  u8SrcEndPoint;
    uint16 u16ClusterId;
    uint8  u8DstAddrMode;
    union {
        struct {
            uint16 u16DstAddress;
        } sShort;
        struct {
            uint64 u64DstAddress;
            uint8  u8DstEndPoint;
        } sExtended;
    };
} ZPS_tsAplZdpStoreBkupBindEntryReq;

/* [I SP001377_sfr 43]  */
typedef struct {
    uint64 u64SrcAddress;
    uint8  u8SrcEndPoint;
    uint16 u16ClusterId;
    uint8  u8DstAddrMode;
    union {
        struct {
            uint16 u16DstAddress;
        } sShort;
        struct {
            uint64 u64DstAddress;
            uint8  u8DstEndPoint;
        } sExtended;
    };
} ZPS_tsAplZdpRemoveBkupBindEntryReq;

typedef struct
{
    ZPS_tuAddress    uDstAddress;
    uint16           u16ClusterId;
    uint8            u8DstAddrMode;
    uint8            u8SourceEndpoint;
    uint8            u8DstEndPoint;
}ZPS_tsAplZdpBindingTableEntry;

typedef struct
{
   uint64  u64SourceAddress;
   ZPS_tsAplZdpBindingTableEntry* psBindingTableEntryForSpSrcAddr;
}ZPS_tsAplZdpBindingTable;

/* [I SP001377_sfr 44]  */
typedef struct {
    uint16 u16BindingTableEntries;
    uint16 u16StartIndex;
    uint16 u16BindingTableListCount;
    /* rest of message is variable length */
    ZPS_tsAplZdpBindingTable sBindingTable;
} ZPS_tsAplZdpBackupBindTableReq;

/* [I SP001377_sfr 45]  */
typedef struct {
    uint16 u16StartIndex;
} ZPS_tsAplZdpRecoverBindTableReq;

/* [I SP001377_sfr 46]  */
typedef struct {
    uint16 u16SourceTableEntries;
    uint16 u16StartIndex;
    uint16 u16SourceTableListCount;
    /* rest of message is variable length */
    uint64* pu64SourceAddress;
} ZPS_tsAplZdpBackupSourceBindReq;

/* [I SP001377_sfr 47]  */
typedef struct {
    uint16 u16StartIndex;
} ZPS_tsAplZdpRecoverSourceBindReq;

/* [I SP001377_sfr 48]  */
typedef struct {
    uint32 u32ScanChannels;
    uint8  u8ScanDuration;
    uint8  u8StartIndex;
} ZPS_tsAplZdpMgmtNwkDiscReq;

/* [I SP001377_sfr 49]  */
typedef struct {
    uint8  u8StartIndex;
} ZPS_tsAplZdpMgmtLqiReq;

/* [I SP001377_sfr 50]  */
typedef struct {
    uint8  u8StartIndex;
} ZPS_tsAplZdpMgmtRtgReq;

/* [I SP001377_sfr 51]  */
typedef struct {
    uint8  u8StartIndex;
} ZPS_tsAplZdpMgmtBindReq;

/* [I SP001377_sfr 52]  */
typedef struct {
    uint64 u64DeviceAddress;
    uint8  u8Flags;
} ZPS_tsAplZdpMgmtLeaveReq;

/* [I SP001377_sfr 53]  */
typedef struct {
    uint64 u64DeviceAddress;
    uint8  u8Capability;
} ZPS_tsAplZdpMgmtDirectJoinReq;

/* [I SP001377_sfr 54]  */
typedef struct {
    uint8 u8PermitDuration;
    bool_t bTcSignificance;
} ZPS_tsAplZdpMgmtPermitJoiningReq;

/* [I SP001377_sfr 55]  */
typedef struct {
    uint8 u8StartIndex;
} ZPS_tsAplZdpMgmtCacheReq;

/* [I SP001377_sfr 56]  */
typedef struct
{
	uint32 u32ScanChannels;
    uint8  u8ScanDuration;
    uint8  u8ScanCount;
    uint8  u8NwkUpdateId;
    uint16 u16NwkManagerAddr;

} ZPS_tsAplZdpMgmtNwkUpdateReq;


/* NWK Enhance Update Req */
#define MAX_ZDP_CHANNEL_PAGES 5
typedef struct
{
	uint8	u8ChannelPageCount;								/* Number of channel fields to follow */
	uint32	pu32ChannelField[MAX_ZDP_CHANNEL_PAGES];		/* Array of channel fields */

} ZPS_tsAplZdpScanChannelList;

typedef struct {
	ZPS_tsAplZdpScanChannelList sZdpScanChannelList;
    uint8  						u8ScanDuration;
    uint8  						u8ScanCount;
    uint8  						u8NwkUpdateId;
    uint16 						u16NwkManagerAddr;
}ZPS_tsAplZdpMgmtNwkEnhanceUpdateReq;


typedef struct {
    uint8 u8NumberOfChildren;
    uint64* pu64ChildList;
} ZPS_tsAplZdpParentAnnceReq;



typedef struct {
    uint8  u8StartIndex;
} ZPS_tsAplZdpMgmtMibIeeeReq;

/****************************************************************************/
/* ZDP Responses                                                            */

/* [I SP001377_sfr 57]  */
typedef struct {
    uint8  u8Status;
    uint64 u64IeeeAddrRemoteDev;
    uint16 u16NwkAddrRemoteDev;
    uint8  u8NumAssocDev;
    uint8  u8StartIndex;
    /* rest of the message is variable Length */
    uint16* pu16NwkAddrAssocDevList;
} ZPS_tsAplZdpNwkAddrRsp;

/* [I SP001377_sfr 58]  */
typedef struct {
    uint8  u8Status;
    uint64 u64IeeeAddrRemoteDev;
    uint16 u16NwkAddrRemoteDev;
    uint8  u8NumAssocDev;
    uint8  u8StartIndex;

    /* rest of the message is variable Length */
    uint16* pu16NwkAddrAssocDevList;
} ZPS_tsAplZdpIeeeAddrRsp;

/* [I SP001377_sfr 59]  */
typedef struct {
    uint8  u8Status;
    uint16 u16NwkAddrOfInterest;
    /* rest of the message is variable length */
    ZPS_tsAplZdpNodeDescriptor sNodeDescriptor;
} ZPS_tsAplZdpNodeDescRsp;

/* [I SP001377_sfr 60]  */
typedef struct {
    uint8  u8Status;
    uint16 u16NwkAddrOfInterest;
    /* rest of the message is variable length */
    ZPS_tsAplZdpNodePowerDescriptor sPowerDescriptor;
} ZPS_tsAplZdpPowerDescRsp;

/* [I SP001377_sfr 61]  */
typedef struct {
    uint8  u8Status;
    uint16 u16NwkAddrOfInterest;
    uint8  u8Length;
    /* rest of the message is variable length */
    ZPS_tsAplZdpSimpleDescType sSimpleDescriptor;
} ZPS_tsAplZdpSimpleDescRsp;

/* [I SP001377_sfr 62]  */
typedef struct {
    uint8  u8Status;
    uint16 u16NwkAddrOfInterest;
    uint8  u8ActiveEpCount;
    /* rest of the message is variable */
    uint8* pu8ActiveEpList;
} ZPS_tsAplZdpActiveEpRsp;

/* [I SP001377_sfr 63]  */
typedef struct {
    uint8  u8Status;
    uint16 u16NwkAddrOfInterest;
    uint8  u8MatchLength;
    /* rest of message is variable length */
    uint8* pu8MatchList;
} ZPS_tsAplZdpMatchDescRsp;

typedef struct {
    uint8 u8XMLTag;
    uint8 u8FieldCount;
    uint8 *pu8CdData;
} ZPS_tsAplZdpComplexDescElement;

/* [I SP001377_sfr 64]  */
typedef struct {
    uint8  u8Status;
    uint16 u16NwkAddrOfInterest;
    uint8  u8Length;
    /* rest of the message is variable Length */
    ZPS_tsAplZdpComplexDescElement sComplexDescriptor;
} ZPS_tsAplZdpComplexDescRsp;

/* [I SP001377_sfr 65]  */
typedef struct {
    uint8  u8Status;
    uint16 u16NwkAddrOfInterest;
    uint8  u8Length;
    /* rest of the message is variable Length */
    char szUserDescriptor[ZPS_ZDP_LENGTH_OF_USER_DESC];
} ZPS_tsAplZdpUserDescRsp;

/* [I SP001377_sfr 68]  */
typedef struct {
    uint8  u8Status;
} ZPS_tsAplZdpDiscoveryCacheRsp;

/* [I SP001377_sfr 67]  */
typedef struct {
    uint8  u8Status;
    uint16 u16NwkAddrOfInterest;
} ZPS_tsAplZdpUserDescConf;

/* [I SP001377_sfr 66]  */
typedef struct {
    uint8  u8Status;
    uint16 u16ServerMask;
} ZPS_tsAplZdpSystemServerDiscoveryRsp;

/* [I SP001377_sfr 69]  */
typedef struct {
    uint8   u8Status;
} ZPS_tsAplZdpDiscoveryStoreRsp;

/* [I SP001377_sfr 70]  */
typedef struct {
    uint8  u8Status;
} ZPS_tsAplZdpNodeDescStoreRsp;

/* [I SP001377_sfr 71]  */
typedef struct {
    uint8  u8Status;
    uint64 u64IeeeAddr;
    /* rest of message is variable length */
    ZPS_tsAplZdpNodePowerDescriptor sPowerDescriptor;
} ZPS_tsAplZdpPowerDescStoreRsp;

/* [I SP001377_sfr 72]  */
typedef struct {
    uint8   u8Status;
} ZPS_tsAplZdpActiveEpStoreRsp;

/* [I SP001377_sfr 73]  */
typedef struct {
    uint8  u8Status;
} ZPS_tsAplZdpSimpleDescStoreRsp;

/* [I SP001377_sfr 74]  */
typedef struct {
    uint8  u8Status;
} ZPS_tsAplZdpRemoveNodeCacheRsp;

/* [I SP001377_sfr 75]  */
typedef struct {
    uint16 u16CacheNwkAddr;
    uint16 u16NwkAddr;
    uint64 u64IeeeAddr;
} ZPS_tsAplZdpFindNodeCacheRsp;

/* [I SP001377_sfr 76]  */
typedef struct {
    uint8  u8Status;
    uint16 u16NwkAddr;
    uint8  u8EndPoint;
    uint8  u8AppInputClusterCount;
    uint8  u8AppOutputClusterCount;
    uint8  u8StartIndex;
    /* rest of the message is variable length */
    uint16* pu16AppClusterList;
} ZPS_tsAplZdpExtendedSimpleDescRsp;

/* [I SP001377_sfr 77]  */
typedef struct {
    uint8  u8Status;
    uint16 u16NwkAddr;
    uint8  u8ActiveEpCount;
    uint8  u8StartIndex;
    /* rest of the message is variable Length */
    uint8* pu8ActiveEpList;
} ZPS_tsAplZdpExtendedActiveEpRsp;

/* [I SP001377_sfr 78]  */
typedef struct {
    uint8 u8Status;
} ZPS_tsAplZdpEndDeviceBindRsp;

/* [I SP001377_sfr 79]  */
typedef struct {
    uint8 u8Status;
} ZPS_tsAplZdpBindRsp;

/* [I SP001377_sfr 80]  */
typedef struct {
    uint8 u8Status;
} ZPS_tsAplZdpUnbindRsp;


/* [I SP001377_sfr 81]  */
typedef struct {
    uint8  u8Status;
    uint16 u16BindingTableEntries;
    uint16 u16BindingTableListCount;

    /* rest of the message is variable Length */
    ZPS_tsAplZdpBindingTable sBindingTableList;
} ZPS_tsAplZdpBindRegisterRsp;

/* [I SP001377_sfr 82]  */
typedef struct {
    uint8 u8Status;
} ZPS_tsAplZdpReplaceDeviceRsp;

/* [I SP001377_sfr 83]  */
typedef struct {
    uint8   u8Status;
} ZPS_tsAplZdpStoreBkupBindEntryRsp;

/* [I SP001377_sfr 84]  */
typedef struct {
    uint8   u8Status;
} ZPS_tsAplZdpRemoveBkupBindEntryRsp;

/* [I SP001377_sfr 85]  */
typedef struct {
    uint8 u8Status;
    uint16  u16EntryCount;
} ZPS_tsAplZdpBackupBindTableRsp;

/* [I SP001377_sfr 86]  */
typedef struct {
    uint8   u8Status;
    uint16 u16StartIndex;
    uint16 u16BindingTableEntries;
    uint16 u16BindingTableListCount;
    /* rest of the message is variable Length */
    ZPS_tsAplZdpBindingTable sBindingTableList;
} ZPS_tsAplZdpRecoverBindTableRsp;

/* [I SP001377_sfr 87]  */
typedef struct {
    uint8   u8Status;
} ZPS_tsAplZdpBackupSourceBindRsp;

/* [I SP001377_sfr 88]  */
typedef struct {
    uint8   u8Status;
    uint16 u16StartIndex;
    uint16 u16SourceTableEntries;
    uint16 u16SourceTableListCount;
    /* rest of the message is variable Length */
    uint64* pu64RcSourceTableList;
} ZPS_tsAplZdpRecoverSourceBindRsp;

typedef struct
{
    uint64 u64ExtPanId;         /**< Extended PAN ID */
    uint8  u8LogicalChan;       /**< Logical channel */
    uint8  u8StackProfile;      /**< Stack profile */
    uint8  u8ZigBeeVersion;     /**< ZigBee version */
    uint8  u8PermitJoining;     /**< Permit Joining */
    uint8  u8RouterCapacity;    /**< Router capacity */
    uint8  u8EndDeviceCapacity; /**< End device capacity */
} ZPS_tsAplZdpNetworkDescr;

/* [I SP001377_sfr 89]  */
typedef struct {
    uint8 u8Status;
    uint8 u8NetworkCount;
    uint8 u8StartIndex;
    uint8 u8NetworkListCount;
    /* rest of the message is variable Length */
    ZPS_tsAplZdpNetworkDescr* psZdpNetworkDescrList;
} ZPS_tsAplZdpMgmtNwkDiscRsp;

typedef struct
{
    uint64 u64ExtPanId;         /**< Extended PAN ID */
    uint64 u64ExtendedAddress;
    uint16 u16NwkAddr;          /**< Network address */
    uint8  u8LinkQuality;       /**< Link quality */
    uint8  u8Depth;
    /*
     * Bitfields are used for syntactic neatness and space saving. May need to assess whether
     * these are suitable for embedded environment and may need to watch endianness on u8Assignment
     */
    union
    {
#ifndef LITTLE_ENDIAN_PROCESSOR
        struct
        {
            unsigned u1Reserved1:1;
            unsigned u2Relationship:3;
            unsigned u2RxOnWhenIdle:2;
            unsigned u2DeviceType:2;
            unsigned u6Reserved2:6;
            unsigned u2PermitJoining:2;
        } ;
#else
        struct
        {
            unsigned u2DeviceType:2;
            unsigned u2RxOnWhenIdle:2;
            unsigned u2Relationship:3;
            unsigned u1Reserved1:1;
            unsigned u2PermitJoining:2;
            unsigned u6Reserved2:6;
        } ;
#endif
        uint8 au8Field[2];
    } uAncAttrs;
} ZPS_tsAplZdpNtListEntry;

/* [I SP001377_sfr 90]  */
typedef struct {
    uint8  u8Status;
    uint8  u8NeighborTableEntries;
    uint8  u8StartIndex;
    uint8  u8NeighborTableListCount;
        /* rest of the message is variable Length */
    ZPS_tsAplZdpNtListEntry* psNetworkTableList;
} ZPS_tsAplZdpMgmtLqiRsp;

#pragma GCC diagnostic ignored "-Wattributes"

typedef union PACK {
    struct PACK
    {
    	uint8 u3Status:3;           /**< Status of route */
    	uint8 u1MemConst:1;         /**< Destination address is unable to store source routes */
    	uint8 u1ManyToOne:1;        /**< Destination address is concentrator that issued a many-to-one route request */
    	uint8 u1RouteRecordReqd:1;  /**< Route record is required */
    	uint8 u1Reserved:2;         /**<  reserved*/
    } bfBitfields;
    uint8 u8Flags;
} ZPS_tsAplZdpRtEntryFlags;

#pragma GCC diagnostic pop

typedef struct
{
    uint16 u16NwkDstAddr;     /**< Destination Network address */
    uint8  u8Flags;           /**< Routing table entry flags  - see ZPS_tsAplZdpRtEntryFlags */
    uint16 u16NwkNxtHopAddr;  /**< Next hop Network address */
} ZPS_tsAplZdpRtEntry;

/* [I SP001377_sfr 91]  */
typedef struct {
    uint8  u8Status;
    uint8  u8RoutingTableEntries;
    uint8  u8StartIndex;
    uint8  u8RoutingTableCount;
    ZPS_tsAplZdpRtEntry asRoutingTableList[ZPS_APL_ZDP_MAX_NUM_MGMT_RTG_RSP_ROUTE_TABLE_ENTRIES];
} ZPS_tsAplZdpMgmtRtgRsp;

/* [I SP001377_sfr 92]  */
typedef struct {
    uint8   u8Status;
    uint16 u16BindingTableEntries;
    uint16 u16StartIndex;
    uint16 u16BindingTableListCount;
    /* rest of the message is variable Length */
    ZPS_tsAplZdpBindingTable sBindingTableList;
} ZPS_tsAplZdpMgmtBindRsp;

/* [I SP001377_sfr 93]  */
typedef struct {
    uint8  u8Status;
} ZPS_tsAplZdpMgmtLeaveRsp;

/* [I SP001377_sfr 94]  */
typedef struct {
    uint8  u8Status;
} ZPS_tsAplZdpMgmtDirectJoinRsp;

/* [I SP001377_sfr 95]  */
typedef struct {
    uint8  u8Status;
} ZPS_tsAplZdpMgmtPermitJoiningRsp;


typedef struct {
    uint64 u64ExtendedAddress;
    uint16 u16NwkAddress;
} ZPS_tsAplDiscoveryCache;

/* [I SP001377_sfr 96]  */
typedef struct {
    uint8 u8Status;
    uint8 u8DiscoveryCacheEntries;
    uint8 u8StartIndex;
    uint8 u8DiscoveryCacheListCount;
    /* rest of the message is variable Length */
    ZPS_tsAplDiscoveryCache* psDiscoveryCacheList;
} ZPS_tsAplZdpMgmtCacheRsp;

/* [I SP001377_sfr 97]  */
typedef struct {
    uint8  u8Status;
    uint32 u32ScannedChannels;
    uint16 u16TotalTransmissions;
    uint16 u16TransmissionFailures;
    uint8  u8ScannedChannelListCount;
        /* rest of the message is variable Length */
    uint8*  pu8EnergyValuesList;
    bool_t	bEnhanced;
} ZPS_tsAplZdpMgmtNwkUpdateNotify;

typedef struct {
    uint64* pu64ChildList;
    uint8 u8NumberOfChildren;
	uint8 u8Status;
} ZPS_tsAplZdpParentAnnceRsp;



typedef struct {

	uint8 u8Status;
	uint8 u8IeeeJoiningListUpdateID;
    uint8 u8MibJoiningPolicy;
    uint8 IeeeJoiningListTotal;
    uint8 u8StartIndex;
    uint8 IeeeJoiningCount;
    uint64	*pu64IEEEAddr;

} ZPS_tsAplZdpMgmtMibIeeeRsp;

typedef struct {

    uint32 u32ChannelInUse;
    uint16 u16MACTxUnicastTotal;
    uint16 u16MACTxUnicastFailures;
    uint16 u16MACTxUnicastRetries;
    uint8  u8Status;
    uint8  u8PeriodOfTimeForResults;
} ZPS_tsAplZdpMgmtNwkUnSolictedUpdateNotify;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions Private Functions                          ***/
/****************************************************************************/

PUBLIC ZPS_teStatus zps_eAplZdpNwkAddrRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpNwkAddrReq *psZdpNwkAddrReq);

PUBLIC ZPS_teStatus zps_eAplZdpIeeeAddrRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpIeeeAddrReq *psZdpIeeeAddrReq);

PUBLIC ZPS_teStatus zps_eAplZdpNodeDescRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpNodeDescReq *psZdpNodeDescReq);

PUBLIC ZPS_teStatus zps_eAplZdpPowerDescRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpPowerDescReq *psZdpPowerDescReq);

PUBLIC ZPS_teStatus zps_eAplZdpSimpleDescRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpSimpleDescReq *psZdpSimpleDescReq);

PUBLIC ZPS_teStatus zps_eAplZdpActiveEpRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpActiveEpReq *psZdpActiveEpReq);

PUBLIC ZPS_teStatus zps_eAplZdpMatchDescRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMatchDescReq *psZdpMatchDescReq);

PUBLIC ZPS_teStatus zps_eAplZdpComplexDescRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpComplexDescReq *psZdpComplexDescReq);

PUBLIC ZPS_teStatus zps_eAplZdpUserDescRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpUserDescReq *psZdpUserDescReq);

PUBLIC ZPS_teStatus zps_eAplZdpDiscoveryCacheRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpDiscoveryCacheReq *psZdpDiscoveryCacheReq);

PUBLIC ZPS_teStatus zps_eAplZdpDeviceAnnceRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpDeviceAnnceReq *psZdpDeviceAnnceReq);

PUBLIC ZPS_teStatus zps_eAplZdpUserDescSetRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpUserDescSet *psZdpUserDescSetReq);

PUBLIC ZPS_teStatus zps_eAplZdpSystemServerDiscoveryRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpSystemServerDiscoveryReq *psZdpSystemServerDiscoveryReq);

PUBLIC ZPS_teStatus zps_eAplZdpDiscoveryStoreRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpDiscoveryStoreReq *psZdpDiscoveryStoreReq);

PUBLIC ZPS_teStatus zps_eAplZdpNodeDescStoreRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpNodeDescStoreReq *psZdpNodeDescStoreReq);

PUBLIC ZPS_teStatus zps_eAplZdpPowerDescStoreRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpPowerDescStoreReq *psZdpPowerDescStoreReq);

PUBLIC ZPS_teStatus zps_eAplZdpActiveEpStoreRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpActiveEpStoreReq *psZdpActiveEpStoreReq);

PUBLIC ZPS_teStatus zps_eAplZdpSimpleDescStoreRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpSimpleDescStoreReq *psZdpSimpleDescStoreReq);

PUBLIC ZPS_teStatus zps_eAplZdpRemoveNodeCacheRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpRemoveNodeCacheReq *psZdpRemoveNodeCacheReq);

PUBLIC ZPS_teStatus zps_eAplZdpFindNodeCacheRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpFindNodeCacheReq *psZdpFindNodeCacheReq);

PUBLIC ZPS_teStatus zps_eAplZdpExtendedSimpleDescRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpExtendedSimpleDescReq *psZdpExtendedSimpleDescReq);

PUBLIC ZPS_teStatus zps_eAplZdpExtendedActiveEpRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpExtendedActiveEpReq *psZdpExtendedActiveEpReq);

PUBLIC ZPS_teStatus zps_eAplZdpEndDeviceBindRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpEndDeviceBindReq *psZdpEndDeviceBindReq);

PUBLIC ZPS_teStatus zps_eAplZdpBindUnbindRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    bool bBindReq,
    ZPS_tsAplZdpBindUnbindReq *psZdpBindReq);

PUBLIC ZPS_teStatus zps_eAplZdpUnbindRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpBindUnbindReq *psZdpUnbindReq);

PUBLIC ZPS_teStatus zps_eAplZdpBindRegisterRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpBindRegisterReq *psZdpBindRegisterReq);

PUBLIC ZPS_teStatus zps_eAplZdpReplaceDeviceRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpReplaceDeviceReq *psZdpReplaceDeviceReq) ;

PUBLIC ZPS_teStatus zps_eAplZdpStoreBkupBindEntryRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpStoreBkupBindEntryReq *psZdpStoreBkupBindEntryReq);

PUBLIC ZPS_teStatus zps_eAplZdpRemoveBkupBindEntryRequest(
        void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpRemoveBkupBindEntryReq *psZdpRemoveBkupBindEntryReq);

PUBLIC ZPS_teStatus zps_eAplZdpBackupBindTableRequest(
        void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpBackupBindTableReq *psZdpBackupBindTableReq);

PUBLIC ZPS_teStatus zps_eAplZdpRecoverBindTableRequest(
        void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpRecoverBindTableReq *psZdpRecoverBindTableReq);

PUBLIC ZPS_teStatus zps_eAplZdpBackupSourceBindRequest(
        void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpBackupSourceBindReq *psZdpBackupSourceBindReq);

PUBLIC ZPS_teStatus zps_eAplZdpRecoverSourceBindRequest(
        void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpRecoverSourceBindReq *psZdpRecoverSourceBindReq);

PUBLIC ZPS_teStatus zps_eAplZdpMgmtNwkDiscRequest(
        void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtNwkDiscReq *psZdpMgmtNwkDiscReq);

PUBLIC ZPS_teStatus zps_eAplZdpMgmtLqiRequest(
        void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtLqiReq *psZdpMgmtLqiReq);

PUBLIC ZPS_teStatus zps_eAplZdpMgmtRtgRequest(
        void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtRtgReq *psZdpMgmtRtgReq);

PUBLIC ZPS_teStatus zps_eAplZdpMgmtBindRequest(
        void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtBindReq *psZdpMgmtBindReq);

PUBLIC ZPS_teStatus zps_eAplZdpMgmtLeaveRequest(
        void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtLeaveReq *psZdpMgmtLeaveReq);

PUBLIC ZPS_teStatus zps_eAplZdpMgmtDirectJoinRequest(
        void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtDirectJoinReq *psZdpMgmtDirectJoinReq);

PUBLIC ZPS_teStatus zps_eAplZdpMgmtPermitJoiningRequest(
        void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtPermitJoiningReq *psZdpMgmtPermitJoiningReq);

PUBLIC ZPS_teStatus zps_eAplZdpMgmtCacheRequest(
        void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtCacheReq *psZdpMgmtCacheReq);

PUBLIC ZPS_teStatus zps_eAplZdpMgmtNwkUpdateRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtNwkUpdateReq *psZdpMgmtNwkUpdateReq);

PUBLIC ZPS_teStatus zps_eAplZdpMgmtNwkEnhanceUpdateRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtNwkEnhanceUpdateReq *psZdpMgmtNwkEnhanceUpdateReq);

PUBLIC ZPS_teStatus zps_eZdpMgmtNwkUpdateNotifyResponse(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 u8SeqNumber,
    ZPS_tsAplZdpMgmtNwkUpdateNotify *psRspStruct,
    uint8 *pu8SeqNum);

PUBLIC ZPS_teStatus zps_eAplZdpParentAnnceReq(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpParentAnnceReq *psZdpParentAnnceReq );


PUBLIC ZPS_teStatus zps_eAplZdpMgmtMibIeeeRequest(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtMibIeeeReq *psZdpMgmtMibIeeeReq);

ZPS_teStatus zps_eAplZdpMgmtUnsolicitedEnhancedUpdateNotify(
    void *pvApl,
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    ZPS_tsAplZdpMgmtNwkUnSolictedUpdateNotify *psRspStruct,
    uint8 *pu8SeqNum);
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Inlined Functions                                            ***/
/****************************************************************************/


ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpNwkAddrRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpNwkAddrReq *psZdpNwkAddrReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpNwkAddrRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpNwkAddrReq *psZdpNwkAddrReq)
{
    return zps_eAplZdpNwkAddrRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpNwkAddrReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpIeeeAddrRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpIeeeAddrReq *psZdpIeeeAddrReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpIeeeAddrRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpIeeeAddrReq *psZdpIeeeAddrReq)
{
    return zps_eAplZdpIeeeAddrRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpIeeeAddrReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpNodeDescRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpNodeDescReq *psZdpNodeDescReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpNodeDescRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpNodeDescReq *psZdpNodeDescReq)
{
    return zps_eAplZdpNodeDescRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpNodeDescReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpPowerDescRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpPowerDescReq *psZdpPowerDescReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpPowerDescRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpPowerDescReq *psZdpPowerDescReq)
{
    return zps_eAplZdpPowerDescRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpPowerDescReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpSimpleDescRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpSimpleDescReq *psZdpSimpleDescReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpSimpleDescRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpSimpleDescReq *psZdpSimpleDescReq)
{
    return zps_eAplZdpSimpleDescRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpSimpleDescReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpActiveEpRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpActiveEpReq *psZdpActiveEpReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpActiveEpRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpActiveEpReq *psZdpActiveEpReq)
{
    return zps_eAplZdpActiveEpRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpActiveEpReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMatchDescRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMatchDescReq *psZdpMatchDescReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMatchDescRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMatchDescReq *psZdpMatchDescReq)
{
    return zps_eAplZdpMatchDescRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpMatchDescReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpComplexDescRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpComplexDescReq *psZdpComplexDescReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpComplexDescRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpComplexDescReq *psZdpComplexDescReq)
{
    return zps_eAplZdpComplexDescRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpComplexDescReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpUserDescRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpUserDescReq *psZdpUserDescReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpUserDescRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpUserDescReq *psZdpUserDescReq)
{
    return zps_eAplZdpUserDescRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpUserDescReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpDiscoveryCacheRequest(
    PDUM_thAPduInstance hAPduInst,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpDiscoveryCacheReq *psZdpDiscoveryCacheReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpDiscoveryCacheRequest(
    PDUM_thAPduInstance hAPduInst,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpDiscoveryCacheReq *psZdpDiscoveryCacheReq)
{
    return zps_eAplZdpDiscoveryCacheRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, pu8SeqNumber, psZdpDiscoveryCacheReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpDeviceAnnceRequest(
    PDUM_thAPduInstance hAPduInst,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpDeviceAnnceReq *psZdpDeviceAnnceReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpDeviceAnnceRequest(
    PDUM_thAPduInstance hAPduInst,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpDeviceAnnceReq *psZdpDeviceAnnceReq)
{
    return zps_eAplZdpDeviceAnnceRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, pu8SeqNumber, psZdpDeviceAnnceReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpUserDescSetRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpUserDescSet *psZdpUserDescSetReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpUserDescSetRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpUserDescSet *psZdpUserDescSetReq)
{
    return zps_eAplZdpUserDescSetRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpUserDescSetReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpSystemServerDiscoveryRequest(
    PDUM_thAPduInstance hAPduInst,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpSystemServerDiscoveryReq *psZdpSystemServerDiscoveryReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpSystemServerDiscoveryRequest(
    PDUM_thAPduInstance hAPduInst,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpSystemServerDiscoveryReq *psZdpSystemServerDiscoveryReq)
{
    return zps_eAplZdpSystemServerDiscoveryRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, pu8SeqNumber, psZdpSystemServerDiscoveryReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpDiscoveryStoreRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpDiscoveryStoreReq *psZdpDiscoveryStoreReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpDiscoveryStoreRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpDiscoveryStoreReq *psZdpDiscoveryStoreReq)
{
    return zps_eAplZdpDiscoveryStoreRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpDiscoveryStoreReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpNodeDescStoreRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpNodeDescStoreReq *psZdpNodeDescStoreReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpNodeDescStoreRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpNodeDescStoreReq *psZdpNodeDescStoreReq)
{
    return zps_eAplZdpNodeDescStoreRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpNodeDescStoreReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpPowerDescStoreRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpPowerDescStoreReq *psZdpPowerDescStoreReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpPowerDescStoreRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpPowerDescStoreReq *psZdpPowerDescStoreReq)
{
    return zps_eAplZdpPowerDescStoreRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpPowerDescStoreReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpActiveEpStoreRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpActiveEpStoreReq *psZdpActiveEpStoreReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpActiveEpStoreRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpActiveEpStoreReq *psZdpActiveEpStoreReq)
{
    return zps_eAplZdpActiveEpStoreRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpActiveEpStoreReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpSimpleDescStoreRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpSimpleDescStoreReq *psZdpSimpleDescStoreReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpSimpleDescStoreRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpSimpleDescStoreReq *psZdpSimpleDescStoreReq)
{
    return zps_eAplZdpSimpleDescStoreRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpSimpleDescStoreReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpRemoveNodeCacheRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpRemoveNodeCacheReq *psZdpRemoveNodeCacheReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpRemoveNodeCacheRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpRemoveNodeCacheReq *psZdpRemoveNodeCacheReq)
{
    return zps_eAplZdpRemoveNodeCacheRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpRemoveNodeCacheReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpFindNodeCacheRequest(
    PDUM_thAPduInstance hAPduInst,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpFindNodeCacheReq *psZdpFindNodeCacheReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpFindNodeCacheRequest(
    PDUM_thAPduInstance hAPduInst,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpFindNodeCacheReq *psZdpFindNodeCacheReq)
{
    return zps_eAplZdpFindNodeCacheRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, pu8SeqNumber, psZdpFindNodeCacheReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpExtendedSimpleDescRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpExtendedSimpleDescReq *psZdpExtendedSimpleDescReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpExtendedSimpleDescRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpExtendedSimpleDescReq *psZdpExtendedSimpleDescReq)
{
    return zps_eAplZdpExtendedSimpleDescRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpExtendedSimpleDescReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpExtendedActiveEpRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpExtendedActiveEpReq *psZdpExtendedActiveEpReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpExtendedActiveEpRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpExtendedActiveEpReq *psZdpExtendedActiveEpReq)
{
    return zps_eAplZdpExtendedActiveEpRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpExtendedActiveEpReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpEndDeviceBindRequest(
    PDUM_thAPduInstance hAPduInst,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpEndDeviceBindReq *psZdpEndDeviceBindReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpEndDeviceBindRequest(
    PDUM_thAPduInstance hAPduInst,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpEndDeviceBindReq *psZdpEndDeviceBindReq)
{
    return zps_eAplZdpEndDeviceBindRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, pu8SeqNumber, psZdpEndDeviceBindReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpBindUnbindRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    bool bBindReq,
    ZPS_tsAplZdpBindUnbindReq *psZdpBindReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpBindUnbindRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    bool bBindReq,
    ZPS_tsAplZdpBindUnbindReq *psZdpBindReq)
{
    return zps_eAplZdpBindUnbindRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, bBindReq, psZdpBindReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpBindRegisterRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpBindRegisterReq *psZdpBindRegisterReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpBindRegisterRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpBindRegisterReq *psZdpBindRegisterReq)
{
    return zps_eAplZdpBindRegisterRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpBindRegisterReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpReplaceDeviceRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpReplaceDeviceReq *psZdpReplaceDeviceReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpReplaceDeviceRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpReplaceDeviceReq *psZdpReplaceDeviceReq)
{
    return zps_eAplZdpReplaceDeviceRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpReplaceDeviceReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpStoreBkupBindEntryRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpStoreBkupBindEntryReq *psZdpStoreBkupBindEntryReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpStoreBkupBindEntryRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpStoreBkupBindEntryReq *psZdpStoreBkupBindEntryReq)
{
    return zps_eAplZdpStoreBkupBindEntryRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpStoreBkupBindEntryReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpRemoveBkupBindEntryRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpRemoveBkupBindEntryReq *psZdpRemoveBkupBindEntryReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpRemoveBkupBindEntryRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpRemoveBkupBindEntryReq *psZdpRemoveBkupBindEntryReq)
{
    return zps_eAplZdpRemoveBkupBindEntryRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpRemoveBkupBindEntryReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpBackupBindTableRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpBackupBindTableReq *psZdpBackupBindTableReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpBackupBindTableRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpBackupBindTableReq *psZdpBackupBindTableReq)
{
    return zps_eAplZdpBackupBindTableRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpBackupBindTableReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpRecoverBindTableRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpRecoverBindTableReq *psZdpRecoverBindTableReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpRecoverBindTableRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpRecoverBindTableReq *psZdpRecoverBindTableReq)
{
    return zps_eAplZdpRecoverBindTableRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpRecoverBindTableReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpBackupSourceBindRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpBackupSourceBindReq *psZdpBackupSourceBindReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpBackupSourceBindRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpBackupSourceBindReq *psZdpBackupSourceBindReq)
{
    return zps_eAplZdpBackupSourceBindRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpBackupSourceBindReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpRecoverSourceBindRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpRecoverSourceBindReq *psZdpRecoverSourceBindReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpRecoverSourceBindRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpRecoverSourceBindReq *psZdpRecoverSourceBindReq)
{
    return zps_eAplZdpRecoverSourceBindRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpRecoverSourceBindReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtNwkDiscRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtNwkDiscReq *psZdpMgmtNwkDiscReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtNwkDiscRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtNwkDiscReq *psZdpMgmtNwkDiscReq)
{
    return zps_eAplZdpMgmtNwkDiscRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpMgmtNwkDiscReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtLqiRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtLqiReq *psZdpMgmtLqiReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtLqiRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtLqiReq *psZdpMgmtLqiReq)
{
    return zps_eAplZdpMgmtLqiRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpMgmtLqiReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtRtgRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtRtgReq *psZdpMgmtRtgReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtRtgRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtRtgReq *psZdpMgmtRtgReq)
{
    return zps_eAplZdpMgmtRtgRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpMgmtRtgReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtBindRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtBindReq *psZdpMgmtBindReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtBindRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtBindReq *psZdpMgmtBindReq)
{
    return zps_eAplZdpMgmtBindRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpMgmtBindReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtLeaveRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtLeaveReq *psZdpMgmtLeaveReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtLeaveRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtLeaveReq *psZdpMgmtLeaveReq)
{
    return zps_eAplZdpMgmtLeaveRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpMgmtLeaveReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtDirectJoinRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtDirectJoinReq *psZdpMgmtDirectJoinReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtDirectJoinRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtDirectJoinReq *psZdpMgmtDirectJoinReq)
{
    return zps_eAplZdpMgmtDirectJoinRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpMgmtDirectJoinReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtPermitJoiningRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtPermitJoiningReq *psZdpMgmtPermitJoiningReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtPermitJoiningRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtPermitJoiningReq *psZdpMgmtPermitJoiningReq)
{
    return zps_eAplZdpMgmtPermitJoiningRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpMgmtPermitJoiningReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtCacheRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtCacheReq *psZdpMgmtCacheReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtCacheRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtCacheReq *psZdpMgmtCacheReq)
{
    return zps_eAplZdpMgmtCacheRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpMgmtCacheReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtNwkUpdateRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtNwkUpdateReq *psZdpMgmtNwkUpdateReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtNwkUpdateRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtNwkUpdateReq *psZdpMgmtNwkUpdateReq)
{
    return zps_eAplZdpMgmtNwkUpdateRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpMgmtNwkUpdateReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtNwkEnhanceUpdateRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtNwkEnhanceUpdateReq *psZdpMgmtNwkEnhanceUpdateReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtNwkEnhanceUpdateRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtNwkEnhanceUpdateReq *psZdpMgmtNwkEnhanceUpdateReq)
{
    return zps_eAplZdpMgmtNwkEnhanceUpdateRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpMgmtNwkEnhanceUpdateReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpParentAnnceReq(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpParentAnnceReq *psZdpParentAnnceReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpParentAnnceReq(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpParentAnnceReq *psZdpParentAnnceReq)
{
    return zps_eAplZdpParentAnnceReq(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpParentAnnceReq);
}


ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtMibIeeeRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtMibIeeeReq *psZdpMgmtMibIeeeReq) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtMibIeeeRequest(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    uint8 *pu8SeqNumber,
    ZPS_tsAplZdpMgmtMibIeeeReq *psZdpMgmtMibIeeeReq)
{
    return zps_eAplZdpMgmtMibIeeeRequest(ZPS_pvAplZdoGetAplHandle(), hAPduInst, uDstAddr, bExtAddr, pu8SeqNumber, psZdpMgmtMibIeeeReq);
}

ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtUnsolicitedEnhancedUpdateNotify(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    ZPS_tsAplZdpMgmtNwkUnSolictedUpdateNotify *psAplZdpMgmtNwkUnSolictedUpdateNotify,
    uint8 *pu8SeqNumber ) ZPS_ZDP_ALWAYS_INLINE;
ZPS_APL_INLINE ZPS_teStatus ZPS_eAplZdpMgmtUnsolicitedEnhancedUpdateNotify(
    PDUM_thAPduInstance hAPduInst,
    ZPS_tuAddress uDstAddr,
    bool bExtAddr,
    ZPS_tsAplZdpMgmtNwkUnSolictedUpdateNotify *psAplZdpMgmtNwkUnSolictedUpdateNotify,
    uint8 *pu8SeqNumber)
{
    return zps_eAplZdpMgmtUnsolicitedEnhancedUpdateNotify( ZPS_pvAplZdoGetAplHandle(),
    		                                               hAPduInst,
    		                                               uDstAddr,
    		                                               bExtAddr,
    		                                               psAplZdpMgmtNwkUnSolictedUpdateNotify,
    		                                               pu8SeqNumber);
}



/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#endif /* ZPS_APL_ZDP_H_ */
