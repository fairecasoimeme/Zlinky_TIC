/*****************************************************************************
 *
 * MODULE:             JN-AN-1220 ZLO Sensor Demo
 *
 * COMPONENT:          bdb_options.h
 *
 * DESCRIPTION:        Options header file for Base Device Behaviour (BDB) module
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5169,
 * JN5179, JN5189].
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
 * Copyright NXP B.V. 2016-2018. All rights reserved
 *
 ***************************************************************************/

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
/* Channel Definitions */
#if (SINGLE_CHANNEL < 11 || SINGLE_CHANNEL > 26)
#define BDB_PRIMARY_CHANNEL_SET                 (0x02108800)                 /* bdbPrimaryChannelSet e.g. (0x02108800) */
#define BDB_SECONDARY_CHANNEL_SET               (0x07FFF800 ^ BDB_PRIMARY_CHANNEL_SET)                     /* bdbSecondaryChannelSet e.g. (0x07FFF800 ^ BDB_PRIMARY_CHANNEL_SET) */
#define BDBC_TL_PRIMARY_CHANNEL_SET             (0x02108800)    /* bdbcTLPrimaryChannelSet */
#define BDBC_TL_SECONDARY_CHANNEL_SET           (0x07fff800 ^ BDBC_TL_PRIMARY_CHANNEL_SET) /* bdbcTLSecondaryChannelSet */
#else
#warning Single channel only!
#define BDB_PRIMARY_CHANNEL_SET                 (1<<SINGLE_CHANNEL)                 /* bdbPrimaryChannelSet e.g. (0x02108800) */
#define BDB_SECONDARY_CHANNEL_SET               (0)                     /* bdbSecondaryChannelSet e.g. (0x07FFF800 ^ BDB_PRIMARY_CHANNEL_SET) */
#define BDBC_TL_PRIMARY_CHANNEL_SET             (0x02108800)    /* bdbcTLPrimaryChannelSet */
#define BDBC_TL_SECONDARY_CHANNEL_SET           (0x07fff800 ^ BDBC_TL_PRIMARY_CHANNEL_SET) /* bdbcTLSecondaryChannelSet */
#endif

/* BDB Constants used by all nodes
   Note - Must not change for final app */
#define BDBC_MAX_SAME_NETWORK_RETRY_ATTEMPTS   (10)     /* bdbcMaxSameNetworkRetryAttempts */
#define BDBC_MIN_COMMISSIONING_TIME            (180)    /* bdbcMinCommissioningTime */
#define BDBC_REC_SAME_NETWORK_RETRY_ATTEMPTS   (3)      /* bdbcRecSameNetworkRetryAttempts */
#define BDBC_TC_LINK_KEY_EXCHANGE_TIMEOUT      (5)      /* bdbcTCLinkKeyExchangeTimeout */


/* BDB Attribute values */
#define BDB_COMMISSIONING_GROUP_ID              (0xFFFF)                /* bdbCommissioningGroupID */
#define BDB_COMMISSIONING_MODE                  (0x0F)                  /* bdbCommissioningMode */
#define BDB_COMMISSIONING_STATUS                (0x00)                  /* bdbCommissioningStatus */
#define BDB_JOINING_NODE_EUI64                  (0x0000000000000000)    /* bdbJoiningNodeEui64 */
#define BDB_JOIN_USES_INSTALL_CODE_KEY          (FALSE)                 /* bdbJoinUsesInstallCodeKey */
                                                                        /* bdbNodeCommissioningCapability - This will be set according to BDB features in Makefile */

#define BDB_NODE_JOIN_LINK_KEY_TYPE             (0x00)                  /* bdbNodeJoinLinkKeyType */
#define BDB_SCAN_DURATION                       (0x04)                  /* bdbScanDuration */
#define BDB_TC_LINK_KEY_EXCHANGE_ATTEMPTS       (0x00)                  /* bdbTCLinkKeyExchangeAttempts */
#define BDB_TC_LINK_KEY_EXCHANGE_ATTEMPTS_MAX   (0x03)                  /* bdbTCLinkKeyExchangeAttemptsMax */
#define BDB_TC_LINK_KEY_EXCHANGE_METHOD         (0x00)                  /* bdbTCLinkKeyExchangeMethod */
#define BDB_TRUST_CENTER_NODE_JOIN_TIMEOUT      (0x0F)                  /* bdbTrustCenterNodeJoinTimeout */
#define BDB_TRUST_CENTER_REQUIRE_KEYEXCHANGE    (TRUE)                  /* bdbTrustCenterRequireKeyExchange */
#define BDB_SET_DEFAULT_TC_POLICY               (FALSE)

#define BDBC_IMP_MAX_REJOIN_CYCLES              (3)

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
