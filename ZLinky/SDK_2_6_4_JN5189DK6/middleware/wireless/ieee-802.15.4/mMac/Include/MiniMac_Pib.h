/****************************************************************************
 *
 * MODULE:             MAC PIB
 *
 * DESCRIPTION:
 * Definitions used for accessing the PIB by higher layers
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5148, JN5142, JN5139].
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
 * Copyright NXP B.V. 2013. All rights reserved
 *
 ***************************************************************************/

#ifndef _minimac_pib_h_
#define _minimac_pib_h_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
//#include "mac_config.h"
//#include "mac_sap.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/** Maximum PHY packet (PDU) size */
#define MAC_MAX_PHY_PKT_SIZE            127
/** Minimum Data Frame overhead */
#define MAC_MIN_DATA_FRM_OVERHEAD       9
/** Maximum Data Frame overhead */
//#define MAC_MAX_DATA_FRM_OVERHEAD       25
/** Minimum Beacon Frame overhead */
#define MAC_MIN_BEACON_FRM_OVERHEAD     9
/** Maximum Beacon Frame overhead */
//#define MAC_MAX_BEACON_FRM_OVERHEAD     15
/** Maximum Data Frame payload */
#define MAC_MAX_DATA_PAYLOAD_LEN      (MAC_MAX_PHY_PKT_SIZE - MAC_MIN_DATA_FRM_OVERHEAD)
/** Maximum Beacon Frame payload */
#define MAC_MAX_BEACON_PAYLOAD_LEN    (MAC_MAX_PHY_PKT_SIZE - MAC_MIN_BEACON_FRM_OVERHEAD)

/** Association permit default */
#define MAC_PIB_ASSOCIATION_PERMIT_DEF              ((bool_t)FALSE)

/** Auto request default */
#define MAC_PIB_AUTO_REQUEST_DEF                    ((bool_t)TRUE)

/** Battery life extension default */
#define MAC_PIB_BATT_LIFE_EXT_DEF                   ((bool_t)FALSE)

/** Beacon order minimum */
#define MAC_PIB_BEACON_ORDER_MIN                    ((uint8)0)
/** Beacon order default */
#define MAC_PIB_BEACON_ORDER_DEF                    ((uint8)15)
/** Beacon order maximum */
#define MAC_PIB_BEACON_ORDER_MAX                    ((uint8)15)

/** Beacon transmit time minimum */
#define MAC_PIB_BEACON_TX_TIME_MIN                  ((uint32)0x000000)
/** Beacon transmit time default */
#define MAC_PIB_BEACON_TX_TIME_DEF                  ((uint32)0x000000)
/** Beacon transmit time maximum */
#define MAC_PIB_BEACON_TX_TIME_MAX                  ((uint32)0xFFFFFF)

/** Coordinator short address minimum */
#define MAC_PIB_COORD_SHORT_ADDRESS_MIN             ((uint16)0x0000)
/** Coordinator short address default */
#define MAC_PIB_COORD_SHORT_ADDRESS_DEF             ((uint16)0xFFFF)
/** Coordinator short address maximum */
#define MAC_PIB_COORD_SHORT_ADDRESS_MAX             ((uint16)0xFFFF)

/** GTS permit default */
#define MAC_PIB_GTS_PERMIT_DEF                      ((bool_t)TRUE)

/** Maximum CSMA backoffs minimum */
#define MAC_PIB_MAX_CSMA_BACKOFFS_MIN               ((uint8)0)
/** Maximum CSMA backoffs default */
#define MAC_PIB_MAX_CSMA_BACKOFFS_DEF               ((uint8)4)
/** Maximum CSMA backoffs maximum */
#define MAC_PIB_MAX_CSMA_BACKOFFS_MAX               ((uint8)5)

/** Default Maximum Frame Retries */
#define MAC_PIB_MAX_FRAME_RETRIES_DEF               ((uint8)3)

/** Minimum backoff exponent minimum */
#define MAC_PIB_MIN_BE_MIN                          ((uint8)0)
/** Minimum backoff exponent default */
#define MAC_PIB_MIN_BE_DEF                          ((uint8)3)
/** Minimum backoff exponent maximum */
#define MAC_PIB_MIN_BE_MAX                          ((uint8)3)

/** PAN ID minimum */
#define MAC_PIB_PAN_ID_MIN                          ((uint16)0x0000)
/** PAN ID default */
#define MAC_PIB_PAN_ID_DEF                          ((uint16)0xFFFF)
/** PAN ID maximum */
#define MAC_PIB_PAN_ID_MAX                          ((uint16)0xFFFF)

/** Promiscuous mode default */
#define MAC_PIB_PROMISCUOUS_MODE_DEF                ((bool_t)FALSE)

/** Receive on when idle default */
#define MAC_PIB_RX_ON_WHEN_IDLE_DEF                 ((bool_t)FALSE)

/** Short address minimum */
#define MAC_PIB_SHORT_ADDRESS_MIN                   ((uint16)0x0000)
/** Short address default */
#define MAC_PIB_SHORT_ADDRESS_DEF                   ((uint16)0xFFFF)
/** Short address maximum */
#define MAC_PIB_SHORT_ADDRESS_MAX                   ((uint16)0xFFFF)

/** Superframe order minimum */
#define MAC_PIB_SUPERFRAME_ORDER_MIN                ((uint8)0)
/** Superframe order default */
#define MAC_PIB_SUPERFRAME_ORDER_DEF                ((uint8)15)
/** Superframe order maximum */
#define MAC_PIB_SUPERFRAME_ORDER_MAX                ((uint8)15)

/** Transaction persistence time minimum */
#define MAC_PIB_TRANSACTION_PERSISTENCE_TIME_MIN    ((uint16)0x0000)
/** Transaction persistence time default */
#define MAC_PIB_TRANSACTION_PERSISTENCE_TIME_DEF    ((uint16)0x01F4)
/** Transaction persistence time maximum */
#define MAC_PIB_TRANSACTION_PERSISTENCE_TIME_MAX    ((uint16)0xFFFF)

/** Total frame transmit time minimum */
#define MAC_PIB_MAX_TOTAL_FRAME_TX_TIME_MIN         ((uint16)143)
/** Total frame transmit time default */
#define MAC_PIB_MAX_TOTAL_FRAME_TX_TIME_DEF         ((uint16)1220)
/** Total frame transmit time maximum */
#define MAC_PIB_MAX_TOTAL_FRAME_TX_TIME_MAX         ((uint16)25776)

/** Response wait time minimum */
#define MAC_PIB_RESPONSE_WAIT_TIME_MIN              ((uint8)2)
/** Response wait time default */
#define MAC_PIB_RESPONSE_WAIT_TIME_DEF              ((uint8)32)
/** Response wait time maximum */
#define MAC_PIB_RESPONSE_WAIT_TIME_MAX              ((uint8)64)

/** Use default security default */
#define MAC_PIB_DEFAULT_SECURITY_DEF                ((bool_t)FALSE)

/** Default security length minimum */
#define MAC_PIB_ACL_DEFAULT_SECURITY_LEN_MIN        ((uint8)0)
/** Default security length default */
#define MAC_PIB_ACL_DEFAULT_SECURITY_LEN_DEF        ((uint8)0x15)
/** Default security length maximum */
#define MAC_PIB_ACL_DEFAULT_SECURITY_LEN_MAX        ((uint8)MAC_MAX_SECURITY_MATERIAL_LEN)

/** Security suite minimum */
#define MAC_PIB_DEFAULT_SECURITY_SUITE_MIN          ((uint8)0)
/** Security suite default */
#define MAC_PIB_DEFAULT_SECURITY_SUITE_DEF          ((uint8)0)
/** Security suite maximum */
#define MAC_PIB_DEFAULT_SECURITY_SUITE_MAX          ((uint8)7)

/** Security mode minimum */
#define MAC_PIB_SECURITY_MODE_MIN                   ((uint8)0)
/** Security mode default */
#define MAC_PIB_SECURITY_MODE_DEF                   ((uint8)0)
/** Security mode maximum */
#define MAC_PIB_SECURITY_MODE_MAX                   ((uint8)2)

/** Security material: Key length in words */
#define MAC_KEY_LEN_WORDS           4
/** Security material: Key length in octets */
#define MAC_KEY_LEN_OCTETS          16
/** Security material: Frame counter length in words */
#define MAC_FRAME_COUNTER_WORDS     1
/** Security material: Frame counter length in octets */
#define MAC_FRAME_COUNTER_OCTETS    4

/** Security mode: Unsecured */
#define MAC_SECURITY_MODE_UNSECURED 0
/** Security mode: ACL mode */
#define MAC_SECURITY_MODE_ACL       1
/** Security mode: Secured */
#define MAC_SECURITY_MODE_SECURED   2

/** ACL freshness support: Lowest numbered ACL suite supporting freshness */
#define ACL_SUITE_FRESHNESS_MIN     1
/** ACL freshness support: Highest numbered ACL suite supporting freshness */
#define ACL_SUITE_FRESHNESS_MAX     4
/** ACL freshness support:  Length of security material supporting freshness */
#define ACL_SECURITY_FRESHNESS_LEN  26
/* @} */

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Unsupported/limited PIB attributes:
    Attribute             | Comment
    ----------------------+------------------------------------------------------
    macAckWaitDuration    | Constant
    macAssociatedPANCoord | PAN ID conflict not supported so not relevant
    macAutoRequest        | No regular beacons; for scan, always acts as if false
    macBattLifeExt        | No regular beacons
    macBattLifeExtPeriods | No regular beacons
    macBeaconOrder        | No regular beacons
    macBeaconTxTime       | No regular beacons
    macGTSPermit          | No regular beacons, GTS not supported
    macMinLIFSPeriod      | Constant
    macMinSIFSPeriod      | Constant
    macPromiscuousMode    | Not supported
    bMacSecurityEnabled   | Assumed from build options
    macSuperframeOrder    | No regular beacons
    macSyncSymbolOffset   | Constant
    macTimestampSupported | Constant
 */
typedef struct
{
    /* Explicitly initialised attributes */
    uint16                      u16CoordShortAddr;   /**< macCoordShortAddress attribute */
    uint16                      u16MaxFrameTotalWaitTime;             /**< macMaxFrameTotalWaitTime */
    uint16                      u16TransactionPersistenceTime;      /**< macTransactionPersistenceTime attribute */
    uint8                       u8MaxFrameRetries; /**<macMaxFrameRetries */
    uint8                       u8ResponseWaitTime;  /**< macResponseWaitTime */

    /* Attributes that are set via functions */
    uint16                      u16PanId_ReadOnly;/**< macPANId attribute */
    uint16                      u16ShortAddr_ReadOnly;/**< macShortAddress attribute */
    uint8                       u8MinBe_ReadOnly;/**< macMinBE attribute */
    uint8                       u8MaxBe_ReadOnly;
    uint8                       u8MaxCsmaBackoffs_ReadOnly; /**< macMaxCSMABackoffs attribute */
    bool_t                      bRxOnWhenIdle_ReadOnly;       /**< macRxOnWhenIdle attribute */

    /* Attributes initialised to 0 */
    tsExtAddr                   sCoordExtAddr;/**< macCoordExtendedAddress attribute */
    uint32                      u32MacFrameCounter;
    bool_t                      bAssociationPermit;  /**< macAssociationPermit attribute */
    uint8                       u8BeaconPayloadLength;  /**< macBeaconPayloadLength attrbute */
    uint8                       au8BeaconPayload[MAC_MAX_BEACON_PAYLOAD_LEN];/**< macBeaconPayload attrbute */

    /* Randomly initialised attributes */
    uint8                       u8Bsn;/**< macBSN attribute */
    uint8                       u8Dsn;/**< macDSN attribute */

    /* Vendor-specific attributes (default to 0) */
    bool_t                      bPanCoordinator;
    uint8                       u8BeaconRequestLqiThreshold; /* Replaces bIgnoreBeaconRequests: if non-zero,
                                                                any received beacon requests with LQI less
                                                                than or equal to this value will be discarded */
    uint8                       u8MinBeDataResp; /* minBE to use for data request responses. Defaults to same
                                                    as normal minBE, and is updated to same value whenever minBE
                                                    is set by vMiniMac_MLME_SetReq_MinBe */
} tsMiniMacPib;

typedef struct
{
    int8         i8Power;
    uint8        u8Channel;
} tsMiniMacPhyPib;

/* 802.15.4-2003 security */
typedef struct
{
    uint32 au32SymmetricKey[4];
    uint32 u32FrameCounter;
    uint8  u8KeySeqCounter;
    bool_t bPassedSecurity;
} tsMiniMacSecurity2003Pib;

/* 802.15.4-2006 security */
typedef struct
{
   uint32                      au32SymmetricKey[4];
   uint16                      u16KeyIndexAndValid; /* Value > 255 means 'invalid key' */
} tsMiniMacKeyDescriptor;

typedef struct
{
   tsExtAddr sExtAddr;           /**< Extended address */
   uint32    u32FrameCounter;
} tsMiniMacDeviceDescriptor;

typedef struct
{
    tsMiniMacKeyDescriptor         *pasMacKeyTable;
    tsMiniMacDeviceDescriptor      *pasMacDeviceTable;
    uint8                           u8MacKeyTableEntries;
    uint8                           u8MacDeviceTableEntries;
    uint8                           u8MinSecurityLevel;
} tsMiniMacSecurityPib;

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern tsMiniMacPib             sMiniMacPib;
extern tsMiniMacPhyPib          sMiniMacPhyPib;
extern tsMiniMacSecurityPib     sMiniMacSecurityPib;
extern tsMiniMacSecurity2003Pib sMiniMacSecurity2003Pib;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

#ifdef __cplusplus
};
#endif

#endif /* _minimac_pib_h_ */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
