/****************************************************************************
 *
 * MODULE:             MAC
 *
 * DESCRIPTION:
 * MAC - The 802.15.4 Media Access Controller
 * Private header file to be included by module files only
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
 * Copyright NXP B.V. 2012. All rights reserved
 *
 ***************************************************************************/

/**
 * @defgroup g_mac_pib MAC PIB
 *
 * Parameters associated with setting or getting PIB values.
 * PIB direct access allows getting and setting of PIB attributes directly by obtaining
 * the PIB handle and dereferencing it appropriately.
 * All PIB attributes can be read (got) in this manner
 * Most PIB attributes can be written (set) in this manner, however some PIB attributes
 * require an auxiliary function call to be called instead. This is because they also require
 * hardware register values to change as well.
 * The functions listed in this group indicate which PIB attributes need to be set in this manner
 * @ingroup g_mac
 */

#ifndef _mac_pib_minimacshim_h_
#define _mac_pib_minimacshim_h_

#ifdef __cplusplus
extern "C" {
#endif

/***********************/
/**** INCLUDE FILES ****/
/***********************/

#include "jendefs.h"
#include "MiniMac.h"
#include "mac_sap.h"

/* Map old structures onto new ones: differences are too great for MiniMac
   shim layer to fix them */
typedef tsMiniMacPib MAC_Pib_s;

/* Define old structures. Code will use these and MiniMac shim layer will
   translate to new structures as required */
/************************/
/**** MACROS/DEFINES ****/
/************************/

/*************/
/**** PIB ****/
/*************/

/**
 * @name PIB defaults, maxima and minima
 * @ingroup g_mac_pib
 * All the default values, minima and maxima for the MAC PIB
 * @{
 */

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

/** ACL entry descriptor set size minimum */
#define MAC_PIB_ACL_ENTRY_DESCRIPTOR_SET_SIZE_MIN   ((uint8)0)
/** ACL entry descriptor set size default */
#define MAC_PIB_ACL_ENTRY_DESCRIPTOR_SET_SIZE_DEF   ((uint8)0)
/** ACL entry descriptor set size maximum */
#define MAC_PIB_ACL_ENTRY_DESCRIPTOR_SET_SIZE_MAX   ((uint8)MAC_MAX_ACL_ENTRIES)

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

/**************************/
/**** TYPE DEFINITIONS ****/
/**************************/

/**
 * MAC PIB Ack wait duration
 * Enumeration of PIB Ack wait duration attribute
 * @ingroup g_mac_pib
 */
typedef enum
{
    MAC_PIB_ACK_WAIT_DURATION_HI = 54,
    MAC_PIB_ACK_WAIT_DURATION_LO = 120,
    NUM_MAC_PIB_ACK_WAIT_DURATION
} MAC_PibAckWaitDuration_e;

#define MAC_PIB_ACK_WAIT_DURATION_DEF       MAC_PIB_ACK_WAIT_DURATION_HI

/**
 * MAC PIB Battery life extension periods
 * Enumeration of PIB Battery life extension periods attribute
 * @ingroup g_mac_pib
 */
typedef enum
{
    MAC_PIB_BATT_LIFE_EXT_PERIODS_HI = 6,
    MAC_PIB_BATT_LIFE_EXT_PERIODS_LO = 8,
    NUM_MAC_PIB_BATT_LIFE_EXT_PERIODS
} MAC_PibBattLifeExtPeriods_e;

#define MAC_PIB_BATT_LIFE_EXT_PERIODS_DEF   MAC_PIB_BATT_LIFE_EXT_PERIODS_HI

/****************************/
/**** EXPORTED VARIABLES ****/

/****************************/

/****************************/
/**** EXPORTED FUNCTIONS ****/
/****************************/
PUBLIC MAC_Pib_s *
MAC_psPibGetHandle(void *pvMac);

PUBLIC void
MAC_vPibSetShortAddr(void *pvMac,
                     uint16 u16ShortAddr);

PUBLIC void
MAC_vPibSetPanId(void *pvMac,
                 uint16 u16PanId);

PUBLIC void
MAC_vPibSetMinBe(void *pvMac,
                 uint8 u8MinBe);

PUBLIC void
MAC_vPibSetMaxCsmaBackoffs(void *pvMac,
                           uint8 u8MaxCsmaBackoffs);

PUBLIC void
MAC_vPibSetRxOnWhenIdle(void *pvMac,
                        bool_t bNewState,
                        bool_t bInReset);

PUBLIC void
MAC_vSetExtAddr(void *pvMac,
                MAC_ExtAddr_s *psExtAddr);

#ifdef __cplusplus
};
#endif

#endif /* _mac_pib_minimacshim_h_ */

/* End of file $RCSfile: mac_pib.h,v $ *******************************************/
