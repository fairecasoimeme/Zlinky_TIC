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

#ifndef _minimac_h_
#define _minimac_h_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
#include "MMAC.h"
#include "MiniMac_Pib.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* For use with u8TxOptions */
#define MAC_TX_OPTION_ACK      (1)             /**< Acknowledge required */
#define MAC_TX_OPTION_GTS      (2)             /**< Transmit in GTS */
#define MAC_TX_OPTION_INDIRECT (4)             /**< Transmit indirectly */
#define MAC_TX_OPTION_SECURITY (8)             /**< Use security */
#define MAC_TX_OPTION_TIMESTAMP (16)           /**< Transmit Timestamp */

/* Security levels supported */
#define MAC_SECURITY_KEYID_MODE_1       (1)

/* Helpers for building FCF */
#define FCF_PUT_FRAME_TYPE(A)    (A)
#define FCF_GET_FRAME_TYPE(A)    ((A) & 7)

#define FCF_PUT_SECURITY(A)      ((A) << 3)
#define FCF_GET_SECURITY(A)      (((A) >> 3) & 1)

#define FCF_PUT_PENDING(A)       ((A) << 4)
#define FCF_GET_PENDING(A)       (((A) >> 4) & 1)

#define FCF_PUT_ACK_REQ(A)       ((A) << 5)
#define FCF_GET_ACK_REQ(A)       (((A) >> 5) & 1)

#define FCF_PUT_INTRA_PAN(A)     ((A) << 6)
#define FCF_GET_INTRA_PAN(A)     (((A) >> 6) & 1)

#define FCF_PUT_DST_ADDR_MODE(A) ((A) << 10)
#define FCF_GET_DST_ADDR_MODE(A) (((A) >> 10) & 3)

#define FCF_PUT_VERSION(A)       ((A) << 12)
#define FCF_GET_VERSION(A)       (((A) >> 12) & 3)

#define FCF_PUT_SRC_ADDR_MODE(A) ((A) << 14)
#define FCF_GET_SRC_ADDR_MODE(A) (((A) >> 14) & 3)

#define FCF_TYPE_BEACON       (0U)
#define FCF_TYPE_MAC_DATA     (1U)
#define FCF_TYPE_ACKNOWLEDGE  (2U)
#define FCF_TYPE_MAC_COMMAND  (3U)

#define FCF_ADDR_MODE_SHORT   (2U)
#define FCF_ADDR_MODE_EXT     (3U)

#define FCF_VERSION_2003      (0U)
#define FCF_VERSION_2006      (1U)

#define FCF_ON                (1U)

/* General defines for MAC layer for 2.4G is 16 but for Sub Gig this needs to be extended*/
#define MAC_MAX_SCAN_CHANNELS (27)

#define MAC_BROADCAST_PAN_ID     (0xFFFFU)
#define MAC_BROADCAST_SHORT_ADDR (0xFFFFU)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef enum PACK
{
    MAC_ENUM_SUCCESS = 0,             /**< Success (0x00) */
    MAC_ENUM_COUNTER_ERROR = 0xDB,
    MAC_ENUM_IMPROPER_KEY_TYPE,
    MAC_ENUM_IMPROPER_SECURITY_LEVEL,
    MAC_ENUM_UNSUPPORTED_LEGACY,
    MAC_ENUM_UNSUPPORTED_SECURITY,    /**< The security parameters passed as part of the primitive produced an invalid security level*/
    MAC_ENUM_BEACON_LOSS = 0xE0,      /**< Beacon loss after synchronisation request (0xE0) */
    MAC_ENUM_CHANNEL_ACCESS_FAILURE,  /**< CSMA/CA channel access failure (0xE1) */
    MAC_ENUM_DENIED,                  /**< GTS request denied (0xE2) */
    MAC_ENUM_DISABLE_TRX_FAILURE,     /**< Could not disable transmit or receive (0xE3) */
    MAC_ENUM_FAILED_SECURITY_CHECK,   /**< Incoming frame failed security check (0xE4) */
    MAC_ENUM_FRAME_TOO_LONG,          /**< Frame too long after security processing to be sent (0xE5) */
    MAC_ENUM_INVALID_GTS,             /**< GTS transmission failed (0xE6) */
    MAC_ENUM_INVALID_HANDLE,          /**< Purge request failed to find entry in queue (0xE7) */
    MAC_ENUM_INVALID_PARAMETER,       /**< Out-of-range parameter in primitive (0xE8) */
    MAC_ENUM_NO_ACK,                  /**< No acknowledgement received when expected (0xE9) */
    MAC_ENUM_NO_BEACON,               /**< Scan failed to find any beacons (0xEA) */
    MAC_ENUM_NO_DATA,                 /**< No response data after a data request (0xEB) */
    MAC_ENUM_NO_SHORT_ADDRESS,        /**< No allocated short address for operation (0xEC) */
    MAC_ENUM_OUT_OF_CAP,              /**< Receiver enable request could not be executed as CAP finished (0xED) */
    MAC_ENUM_PAN_ID_CONFLICT,         /**< PAN ID conflict has been detected (0xEE) */
    MAC_ENUM_REALIGNMENT,             /**< Coordinator realignment has been received (0xEF) */
    MAC_ENUM_TRANSACTION_EXPIRED,     /**< Pending transaction has expired and data discarded (0xF0) */
    MAC_ENUM_TRANSACTION_OVERFLOW,    /**< No capacity to store transaction (0xF1) */
    MAC_ENUM_TX_ACTIVE,               /**< Receiver enable request could not be executed as in transmit state (0xF2) */
    MAC_ENUM_UNAVAILABLE_KEY,         /**< Appropriate key is not available in ACL (0xF3) */
    MAC_ENUM_UNSUPPORTED_ATTRIBUTE,   /**< PIB Set/Get on unsupported attribute (0xF4) */
    MAC_ENUM_SCAN_IN_PROGRESS,        /**< Scan already in progress */
    MAC_ENUM_UNSUPPORTED_IN_MULTIMAC  /**< Functionality not supported by Multimac */

} teMacStatus;

typedef enum PACK
{
    MAC_MLME_DCFM_SCAN,                 /**< Use with tagMAC_MlmeCfmScan_s */
    MAC_MLME_DCFM_GTS,                  /**< Use with tagMAC_MlmeCfmGts_s */
    MAC_MLME_DCFM_ASSOCIATE,            /**< Use with tagMAC_MlmeCfmAssociate_s */
    MAC_MLME_DCFM_DISASSOCIATE,         /**< Use with tagMAC_MlmeCfmDisassociate_s */
    MAC_MLME_DCFM_POLL,                 /**< Use with tagMAC_MlmeCfmPoll_s */
    MAC_MLME_DCFM_RX_ENABLE,            /**< Use with tagMAC_MlmeCfmRxEnable_s */
    MAC_MLME_IND_ASSOCIATE,             /**< Use with tagMAC_MlmeIndAssociate_s */
    MAC_MLME_IND_DISASSOCIATE,          /**< Use with tagMAC_MlmeIndDisassociate_s */
    MAC_MLME_IND_SYNC_LOSS,             /**< Use with tagMAC_MlmeIndSyncLoss_s */
    MAC_MLME_IND_GTS,                   /**< Use with tagMAC_MlmeIndGts_s */
    MAC_MLME_IND_BEACON_NOTIFY,         /**< Use with tagMAC_MlmeIndBeacon_s */
    MAC_MLME_IND_COMM_STATUS,           /**< Use with tagMAC_MlmeIndCommStatus_s */
    MAC_MLME_IND_ORPHAN,                /**< Use with tagMAC_MlmeIndOrphan_s */

    MAC_MLME_IND_DUTY_CYCLE_MODE,       /**< Use with tagMAC_MlmeIndDutyCycleMode_s */
	MAC_MLME_IND_UNRECN_DATA_REQ,       /**< Use with tagMAC_MlmeIndUnRecnDataReq_s */

    MAC_MLME_DCFM_TOFPOLL,
    MAC_MLME_DCFM_TOFPRIME,
    MAC_MLME_DCFM_TOFDATAPOLL,
    MAC_MLME_DCFM_TOFDATA,
    MAC_MLME_IND_TOFPOLL,
    MAC_MLME_IND_TOFPRIME,
    MAC_MLME_IND_TOFDATAPOLL,
    MAC_MLME_IND_TOFDATA,
    NUM_MAC_MLME_IND,
    MAC_MLME_REJECT  = 0xFE,
    MAC_MLME_INVALID = 0xFF
} MAC_MlmeDcfmIndType_e;

typedef enum PACK
{
    MAC_MCPS_DCFM_DATA,
    MAC_MCPS_DCFM_PURGE,
    MAC_MCPS_IND_DATA,
    NUM_MAC_MCPS_IND,
    MAC_MCPS_REJECT  = 0xFE,
    MAC_MCPS_INVALID = 0xFF
} MAC_McpsDcfmIndType_e;

/* For full reset use:
     E_MAC_RESET_PIB | E_MAC_RESET_INTERRUPT | E_MAC_RESET_STACK
   For ZigBee full reset use:
     E_MAC_RESET_PIB | E_MAC_RESET_STACK | E_MAC_RESET_NOT_TSV
   For warm start use:
     E_MAC_RESET_INTERRUPT
   For ZigBee warm start use:
     0
 */
typedef enum PACK
{
    E_MAC_RESET_PIB = 1,       /* Resets PIB */
    E_MAC_RESET_INTERRUPT = 2, /* Initialises interrupts and handler */
    E_MAC_RESET_STACK = 4,     /* Resets MAC internal code */
    E_MAC_RESET_NOT_TSV = 8,   /* Don't reset timer server */
	E_MAC_RESET_RESTART = 16   /* Internal use: indicates if restarting */
} teInitOptions;

typedef enum PACK
{
    E_MODULE_STD   = 0,
    E_MODULE_HPM05 = 1,
    E_MODULE_HPM06 = 2
} teModuleType;

typedef enum PACK
{
    E_RETURN_BUFFER = 1,
    E_START_NEXT_TRANSACTION = 2
} teTxDoneAction;

typedef enum PACK
{
    E_RETURN_BUFFER_TO_POOL_ON_FAIL,
    E_CALLER_KEEPS_BUFFER_ON_FAIL
} teTxFailBufferHandling;

typedef enum PACK
{
    E_FRAME_TX_PENDING,
    E_FRAME_TX_PURGED,
    E_FRAME_TX_EXPIRED,
    E_FRAME_TX_PROCESSING
} teTxStage;

typedef enum PACK
{
    MAC_MLME_SCAN_TYPE_ENERGY_DETECT,
    MAC_MLME_SCAN_TYPE_ACTIVE,
    MAC_MLME_SCAN_TYPE_PASSIVE,
    MAC_MLME_SCAN_TYPE_ORPHAN
} teScanType;

typedef enum PACK
{
    E_FIP_NONE = 0,
    E_FIP_RX,
    E_FIP_TX,
    E_FIP_POLL,
} teFrameInProgress;

struct tsTxFrameFormat_tag;
typedef teTxDoneAction (*tpreTxCallback)(struct tsTxFrameFormat_tag *psFrame, uint32 u32Status);

typedef struct tsTxFrameFormat_tag
{
    tsMacFrame     sFrameBody;

    struct tsTxFrameFormat_tag *psNext;
    tsSecurity     sSecurityData;
    tpreTxCallback preTxCallback;
    uint32         u32TimerCounter;
    uint32         u32CsmaContext;
    uint8          u8TxOptions;
    uint8          u8TrafficClass;
    uint8          u8Channel;
    uint8          u8TriesLeft;
    uint8          u8Handle;
    teTxStage      eTxStatus;
    uint8          u8RxInCcaOptions;
    bool_t         bTransmitReady;
#if 0 /* Possible IPN feature */
    uint8          u8Priority;
#endif
} tsTxFrameFormat;

typedef struct
{
    uint16 u16PanId;
    uint8  u8AddrMode;
    tuAddr uAddr;
} tsFullAddr;

typedef struct
{
    uint8      u8Type;
    uint8      u8ParamLength;
} tsMlmeGeneric;

typedef struct
{
    uint8      u8Type;
    uint8      u8ParamLength;
    uint8      u8Status;
    tsFullAddr sSrcAddr;
    tsFullAddr sDstAddr;
    tsSecurity sSecurityData;
} tsMlmeCommStatusInd;

typedef struct
{
    uint8      u8Type;
    uint8      u8ParamLength;
    uint8      u8Capability;
    tsExtAddr  sDeviceAddr;
} tsMlmeAssocInd;

typedef struct
{
    uint8      u8Type;
    uint8      u8ParamLength;
    uint16     u16AssocShortAddr;
    uint8      u8Status;
} tsMlmeAssocDcfm;

typedef struct
{
    uint8      u8Type;
    uint8      u8ParamLength;
    uint8      u8Status;
    uint8      u8ScanType;
    uint32     u32UnscannedChannels;
    uint8      au8EnergyDetect[MAC_MAX_SCAN_CHANNELS];
    uint8      u8ResultListSize;
} tsMlmeScanDcfm;

typedef struct
{
    uint8      u8Type;
    uint8      u8ParamLength;
    uint8      u8Reason;
    tsExtAddr  sDeviceAddr;
} tsMlmeDisassocInd;

typedef struct
{
    uint8      u8Type;
    uint8      u8ParamLength;
    uint8      u8Status;
} tsMlmeDisassocDcfm;

typedef struct
{
    uint8      u8Type;
    uint8      u8ParamLength;
    uint8      u8Status;
} tsMlmePollDcfm;

typedef struct
{
    uint8      u8Type;
    uint8      u8ParamLength;
    tsExtAddr  sDeviceAddr;
} tsMlmeOrphanInd;

typedef struct
{
    uint8      u8Mode;
} tsMlmeDutyCycleModeInd;

typedef struct
{
    tuAddr     uCoordAddr;
    uint32     u32TimeStamp;
    uint16     u16CoordPanId;
    uint16     u16SuperframeSpec;
    uint8      u8CoordAddrMode;
    uint8      u8LogicalChan;
    uint8      u8GtsPermit;
    uint8      u8LinkQuality;
} tsPanDescriptor;

typedef struct
{
    uint8            u8Type;
    uint8            u8ParamLength;
    tsPanDescriptor  sPanDescriptor;
    tuAddr           auAddrList[7];
    uint8            au8SDU[MAC_MAX_BEACON_PAYLOAD_LEN];
    uint8            u8BSN;
    uint8            u8PendAddrSpec;
    uint8            u8SDUlength;
    uint8            u8BeaconType; /* Only populated if 802.15.4-2015 supported */
} tsMlmeBeaconNotifyInd;

typedef struct
{
    uint8      u8Type;
    uint8      u8ParamLength;
} tsMcpsGeneric;

typedef struct
{
    uint8            u8Type;
    uint8            u8ParamLength;
    uint8            u8Status;
    tsTxFrameFormat *psFrame;
    uint32           u32Timestamp;
} tsMcpsDataDcfm;

typedef struct
{
    uint8            u8Type;
    uint8            u8ParamLength;
    tsRxFrameFormat *psFrame;
} tsMcpsDataInd;

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
/* This device's MAC address */
extern PUBLIC tsExtAddr sThisDeviceMacAddr;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/* Initialisation functions */
PUBLIC void vMiniMac_Init(teInitOptions eInitOptions);
PUBLIC void vMiniMac_SetHighPowerMode(teModuleType eModuleType);
PUBLIC void vMiniMac_SaveSettings(void);
PUBLIC void vMiniMac_ConfigureDoze(uint32 u32Period, uint32 u32Duration);
PUBLIC void vMiniMac_SleepCompensation(uint32 u32SleepTimeInMs);

/* MLME API */
PUBLIC void vMiniMac_MLME_Reset(bool_t bResetPib);

PUBLIC teMacStatus eMiniMac_MLME_ScanReq(
    uint32            u32ScanChannels,
    teScanType        eScanType,
    uint8             u8ScanDuration,
    uint8             u8ScanBeaconLimit);

PUBLIC teMacStatus eMiniMac_MLME_StartReq(
    uint16            u16NewPanId,
    uint8             u8NewChannel,
    bool_t            bPanCoordinator,
    bool_t            bRealignment);

PUBLIC teMacStatus eMiniMac_MLME_AssociateReq(
    uint8             u8LogicalChannel,
    uint8             u8Capability,
    tsFullAddr       *psCoordAddr);

PUBLIC teMacStatus eMiniMac_MLME_AssociateResp(
    uint8             u8Status,
    uint16            u16AssocShortAddr,
    tsExtAddr        *psDeviceAddr);

PUBLIC teMacStatus eMiniMac_MLME_DisassociateReq(
    tsFullAddr       *psAddr,
    uint8             u8Reason);

PUBLIC teMacStatus eMiniMac_MLME_OrphanRsp(
    tsExtAddr        *psOrphanAddr,
    uint16            u16ShortAddr);

PUBLIC teMacStatus eMiniMac_MLME_DataReq(
    uint8             u8CoordAddrMode,
    tuAddr           *puCoordAddr);

PUBLIC teMacStatus eMiniMac_MLME_RxEnableReq(
    uint32            u32Duration);

/* MLME API: Specific MLME-Set.Requests */
PUBLIC void vMiniMac_MLME_SetReq_PanId(uint16 u16PanId);
PUBLIC void vMiniMac_MLME_SetReq_ShortAddr(uint16 u16ShortAddr);
PUBLIC void vMiniMac_MLME_SetReq_MinBe(uint8 u8MinBe);
PUBLIC void vMiniMac_MLME_SetReq_MaxBe(uint8 u8MaxBe);
PUBLIC void vMiniMac_MLME_SetReq_MaxCsmaBackoffs(uint8 u8MaxCsmaBackoffs);
PUBLIC void vMiniMac_MLME_SetReq_RxOnWhenIdle(bool_t bNewState);

/* PLME API */
PUBLIC void vMiniMac_PLME_SetReq_Channel(uint8 u8Channel);
PUBLIC void vMiniMac_PLME_SetReq_Power(uint8 u8Power);
PUBLIC void vMiniMac_PLME_SetReq_CcaMode(uint8 u8Mode);

/* MCPS API */
PUBLIC teMacStatus eMiniMac_MCPS_DataReq(
    tsTxFrameFormat        *psTxFrame,
    teTxFailBufferHandling  eFailBufferHandling);

PUBLIC teMacStatus eMiniMac_MCPS_DataRetransmitReq(
    tsTxFrameFormat        *psTxFrame,
    teTxFailBufferHandling  eFailBufferHandling);

PUBLIC teMacStatus eMiniMac_MCPS_PurgeReq(uint8 u8Handle,
                                          tsTxFrameFormat **ppsFrame);

/* Mandatory application callback functions */
PUBLIC void vMiniMac_MlmeCallback(tsMlmeGeneric *psMlmeDcfmInd);
PUBLIC void vMiniMac_McpsCallback(tsMcpsGeneric *psMcpsDcfmInd);
PUBLIC tsTxFrameFormat *psMiniMac_GetTxBufferCallback(void);
PUBLIC tsRxFrameFormat *psMiniMac_GetRxBufferCallback(void);
PUBLIC void vMiniMac_ReturnTxBufferCallback(tsTxFrameFormat *psBuffer);
PUBLIC void vMiniMac_ReturnRxBufferCallback(tsRxFrameFormat *psBuffer);
PUBLIC void vMiniMac_DataRequestCallback(tsRxFrameFormat *psFrame);
PUBLIC void vMiniMac_EmptyMcpsFrameWithFpSetCallback(void);
PUBLIC void vMiniMac_BeaconRequestReceived(void);

/* Optional application callback functions */
PUBLIC WEAK void vMiniMac_EnergyScanInterChannelCallback(void);

/* Helper functions */
PUBLIC tsTxFrameFormat *psGetTxFrameBuffer(void);
PUBLIC void vInsertFullSrcAddr(tsMacFrame *psFrame,
                               tsFullAddr *psAddr);
PUBLIC void vInsertFullDstAddr(tsMacFrame *psFrame,
                               tsFullAddr *psAddr);
PUBLIC void vExtractFullSrcAddr(tsMacFrame *psFrame,
                                tsFullAddr *psAddr);
PUBLIC void vExtractFullDstAddr(tsMacFrame *psFrame,
                                tsFullAddr *psAddr);

/* Status functions */
PUBLIC teFrameInProgress eMiniMac_FrameInProgress(void);
PUBLIC bool_t            bMiniMac_CurrentlyScanning(void);

/* Additional functionality */
PUBLIC void vMiniMac_PurgeAssocResp(tsExtAddr *psSearchAddr,
		                            bool_t bCallCallback);

#ifdef __cplusplus
};
#endif

#endif /* _minimac_h_ */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
