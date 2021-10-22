/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef BDB_OPTIONS_INCLUDED
#define BDB_OPTIONS_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "EmbeddedTypes.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define BDB_SUPPORT_NWK_FORMATION               (1)
#define BDB_SUPPORT_NWK_STEERING                (1)
#define BDB_SUPPORT_FIND_AND_BIND_INITIATOR     (1)
#define BDB_PRIMARY_CHANNEL_SET                 (1<<SINGLE_CHANNEL)     /* bdbPrimaryChannelSet e.g. (0x02108800) */
#define BDB_SECONDARY_CHANNEL_SET               (0)                     /* bdbSecondaryChannelSet e.g. (0x07FFF800 ^ BDB_PRIMARY_CHANNEL_SET) */
#define BDBC_TL_PRIMARY_CHANNEL_SET             (0x02108800)            /* bdbcTLPrimaryChannelSet */
#define BDBC_TL_SECONDARY_CHANNEL_SET           (0x07fff800 ^ BDBC_TL_PRIMARY_CHANNEL_SET) /* bdbcTLSecondaryChannelSet */

/* BDB Constants */
#define BDBC_MIN_COMMISSIONING_TIME            (180)    /* bdbcMinCommissioningTime */
#define BDBC_TC_LINK_KEY_EXCHANGE_TIMEOUT      (5)      /* bdbcTCLinkKeyExchangeTimeout */

/* BDB Attribute values */
#define BDB_COMMISSIONING_GROUP_ID              (0xFFFF)                /* bdbCommissioningGroupID */
#define BDB_COMMISSIONING_MODE                  (0x0F)                  /* bdbCommissioningMode */
#define BDB_COMMISSIONING_STATUS                (0x00)                  /* bdbCommissioningStatus */
#define BDB_JOINING_NODE_EUI64                  (0x0000000000000000)    /* bdbJoiningNodeEui64 */
#define BDB_JOIN_USES_INSTALL_CODE_KEY          (FALSE)                 /* bdbJoinUsesInstallCodeKey */

#define BDB_NODE_JOIN_LINK_KEY_TYPE             (0x01)                  /* bdbNodeJoinLinkKeyType */

#define BDB_SCAN_DURATION                       (0x04)                  /* bdbScanDuration */

#define BDB_TC_LINK_KEY_EXCHANGE_ATTEMPTS       (0x00)                  /* bdbTCLinkKeyExchangeAttempts */
#define BDB_TC_LINK_KEY_EXCHANGE_ATTEMPTS_MAX   (0x03)                  /* bdbTCLinkKeyExchangeAttemptsMax */
#define BDB_TC_LINK_KEY_EXCHANGE_METHOD         (0x00)                  /* bdbTCLinkKeyExchangeMethod */
#define BDB_TRUST_CENTER_NODE_JOIN_TIMEOUT      (0x0F)                  /* bdbTrustCenterNodeJoinTimeout */
#define BDB_TRUST_CENTER_REQUIRE_KEYEXCHANGE    (TRUE)                  /* bdbTrustCenterRequireKeyExchange */

#define BDB_SET_DEFAULT_TC_POLICY               (TRUE)
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
