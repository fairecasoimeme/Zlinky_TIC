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
 * COMPONENT:   zps_apl_aps.h
 *
 * DESCRIPTION: Application Support Sub-Layer public interface
 *
 *****************************************************************************/

#ifndef ZPS_APL_APS_H_
#define ZPS_APL_APS_H_

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define ZPS_E_SUCCESS               (0UL)

/* broadcast network addresses */
#define ZPS_E_BROADCAST_ALL_IEEE    (0xffffffffffffffffULL)
#define ZPS_E_BROADCAST_ALL         (0xffff)
#define ZPS_E_BROADCAST_RX_ON       (0xfffd)
#define ZPS_E_BROADCAST_ZC_ZR       (0xfffc)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum {
    /* [I SP001349_sfr 6] */
    ZPS_APL_APS_E_ASDU_TOO_LONG = 0xa0,
    ZPS_APL_APS_E_DEFRAG_DEFERRED,
    ZPS_APL_APS_E_DEFRAG_UNSUPPORTED,
    ZPS_APL_APS_E_ILLEGAL_REQUEST,
    ZPS_APL_APS_E_INVALID_BINDING,
    ZPS_APL_APS_E_INVALID_GROUP,
    ZPS_APL_APS_E_INVALID_PARAMETER,
    ZPS_APL_APS_E_NO_ACK,
    ZPS_APL_APS_E_NO_BOUND_DEVICE,
    ZPS_APL_APS_E_NO_SHORT_ADDRESS,
    ZPS_APL_APS_E_NOT_SUPPORTED,
    ZPS_APL_APS_E_SECURED_LINK_KEY,
    ZPS_APL_APS_E_SECURED_NWK_KEY,
    ZPS_APL_APS_E_SECURITY_FAIL,
    ZPS_APL_APS_E_TABLE_FULL,
    ZPS_APL_APS_E_UNSECURED,
    ZPS_APL_APS_E_UNSUPPORTED_ATTRIBUTE = 0xb0
} ZPS_teApsStatus;

typedef enum
{
    ZPS_E_ADDR_MODE_BOUND        = 0,
    ZPS_E_ADDR_MODE_GROUP        = 1,
    ZPS_E_ADDR_MODE_SHORT        = 2,
    ZPS_E_ADDR_MODE_IEEE         = 3
} ZPS_teAplApsdeAddressMode;

/* address type for short and long addresses */
typedef union {
    uint16 u16Addr;
    uint64 u64Addr;
} ZPS_tuAddress;

typedef struct
{
  enum {
      ZPS_E_AM_INTERPAN_GROUP = 0x01,
      ZPS_E_AM_INTERPAN_SHORT,
      ZPS_E_AM_INTERPAN_IEEE
  }eMode;
  uint16        u16PanId;
  ZPS_tuAddress uAddress;
} ZPS_tsInterPanAddress;

/**
 * @brief APS packet traffic statistics
 *
 * Counters for unicast and broadcast statistics in the APS layer
 */
typedef struct
{
    uint16 u16ApsRxBcast;                    /**< Diagnostics - APS layer RXed broadcast packets */
    uint16 u16ApsTxBcast;                    /**< Diagnostics - APS layer TXed broadcast packets */
    uint16 u16ApsRxUcast;                    /**< Diagnostics - APS layer RXed unicast packets */
    uint16 u16ApsTxUcastSuccess;             /**< Diagnostics - APS layer TXed successfully unicast */
    uint16 u16ApsTxUcastRetry;               /**< Diagnostics - APS layer TXed retried */
    uint16 u16ApsTxUcastFail;                /**< Diagnostics - APS layer TXed failed */
    uint16 u16ApsFCFailure;                  /**< Diagnostics - APS layer Frame Counter failures */
    uint16 u16ApsUnauthorizedKey;            /**< Diagnostics - APS layer frames with unauthorized key */
    uint16 u16ApsDecryptFailure;             /**< Diagnostics - APS layer frames failed to decrypt */
    uint16 u16PacketBufferAllocateFailure;   /**< Diagnostics - packet buffer allocation failures */
} ZPS_tsApsCounters;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC ZPS_tsApsCounters *ZPS_psApsGetCounters(void *pvApl);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Inlined Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#endif /* ZPS_APL_APS_H_ */
