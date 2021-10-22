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
# COMPONENT:   bdb_options.h
#
# DESCRIPTION: Options header file for Base Device Behaviour (BDB) module
#              
#
###############################################################################*/

#ifndef BDB_OPTIONS_INCLUDED
#define BDB_OPTIONS_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "jendefs.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* BDB Constants used by all nodes 
   Note - Must not change for final app */
#define BDBC_MAX_SAME_NETWORK_RETRY_ATTEMPTS   (10)     /* bdbcMaxSameNetworkRetryAttempts */
#define BDBC_MIN_COMMISSIONING_TIME            (180)    /* bdbcMinCommissioningTime */
#define BDBC_REC_SAME_NETWORK_RETRY_ATTEMPTS   (3)      /* bdbcRecSameNetworkRetryAttempts */
#define BDBC_TC_LINK_KEY_EXCHANGE_TIMEOUT      (5)      /* bdbcTCLinkKeyExchangeTimeout */

/* BDB Constants used by nodes supporting touchlink 
   Note - Must not change for final app */
#define BDBC_TL_INTERPAN_TRANS_ID_LIFETIME      (8)     /* bdbcTLInterPANTransIdLifetime */
#define BDBC_TL_MIN_STARTUP_DELAY_TIME          (2)     /* bdbcTLMinStartupDelayTime */
#define BDBC_TL_PRIMARY_CHANNEL_SET             (0x02108800)    /* bdbcTLPrimaryChannelSet */
#define BDBC_TL_RX_WINDOW_DURATION              (5)     /* bdbcTLRxWindowDuration */
#define BDBC_TL_SCAN_TIME_BASE_DURATION_MS      (250)   /* bdbcTLScanTimeBaseDuration */
#define BDBC_TL_SECONDARY_CHANNEL_SET           (0x07fff800 ^ BDBC_TL_PRIMARY_CHANNEL_SET) /* bdbcTLSecondaryChannelSet */

/* BDB Attribute values */
#define BDB_COMMISSIONING_GROUP_ID              (0xFFFF)                /* bdbCommissioningGroupID */
#define BDB_COMMISSIONING_MODE                  (0x0F)                  /* bdbCommissioningMode */
#define BDB_COMMISSIONING_STATUS                (0x00)                  /* bdbCommissioningStatus */
#define BDB_JOINING_NODE_EUI64                  (0x0000000000000000)    /* bdbJoiningNodeEui64 */
                                                                        /* bdbJoiningNodeNewTCLinkKey */
#define BDB_JOIN_USES_INSTALL_CODE_KEY          (FALSE)                 /* bdbJoinUsesInstallCodeKey */
                                                                        /* bdbNodeCommissioningCapability - Will be set automatically as per BDB features in Makefile */
                                                                        /* bdbNodeIsOnANetwork - Application is responsible to set this before BDB initialisation */
#define BDB_NODE_JOIN_LINK_KEY_TYPE             (0x00)                  /* bdbNodeJoinLinkKeyType */
#define BDB_PRIMARY_CHANNEL_SET                 (0x02108800)            /* bdbPrimaryChannelSet */
#define BDB_SCAN_DURATION                       (0x04)                  /* bdbScanDuration */
#define BDB_SECONDARY_CHANNEL_SET               (0x07FFF800 ^ BDB_PRIMARY_CHANNEL_SET)  /* bdbSecondaryChannelSet */
#define BDB_TC_LINK_KEY_EXCHANGE_ATTEMPTS       (0x00)                  /* bdbTCLinkKeyExchangeAttempts */
#define BDB_TC_LINK_KEY_EXCHANGE_ATTEMPTS_MAX   (0x03)                  /* bdbTCLinkKeyExchangeAttemptsMax */
#define BDB_TC_LINK_KEY_EXCHANGE_METHOD         (0x00)                  /* bdbTCLinkKeyExchangeMethod */
#define BDB_TRUST_CENTER_NODE_JOIN_TIMEOUT      (0x0F)                  /* bdbTrustCenterNodeJoinTimeout */
#define BDB_TRUST_CENTER_REQUIRE_KEYEXCHANGE    (TRUE)                  /* bdbTrustCenterRequireKeyExchange */

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* BDB_OPTIONS_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
