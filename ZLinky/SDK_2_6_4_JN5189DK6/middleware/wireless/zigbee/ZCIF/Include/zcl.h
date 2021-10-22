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
 * MODULE:             Smart Energy
 *
 * COMPONENT:          zcl.h
 *
 * DESCRIPTION:       The API for the Jennic Zigbee ZCL layer
 *
 ****************************************************************************/

#ifndef  ZCL_H_INCLUDED
#define  ZCL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"
#include "pdum_apl.h"
#include "zps_apl_af.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Smart Lighting Profile ID's */
#define ZLL_PROFILE_ID                  0xC05E

#ifndef HA_PROFILE_ID
    #define HA_PROFILE_ID                   0x0104
#endif

// This is the NXP manufacturer code.
// If creating new a manufacturer specific command: apply to the Zigbee alliance for an Id for your company
// Also update the manufacturer code in .zpscfg: Node Descriptor->misc
#ifndef ZCL_MANUFACTURER_CODE
    #define ZCL_MANUFACTURER_CODE           0x1037
#endif

/* Number of ZCL endpoints */
#ifndef ZCL_NUMBER_OF_ENDPOINTS
    #define ZCL_NUMBER_OF_ENDPOINTS         1
#endif

/* By default disable default responses */
#ifndef ZCL_DISABLE_DEFAULT_RESPONSES
    #define ZCL_DISABLE_DEFAULT_RESPONSES   1
#endif

#ifndef ZCL_DISABLE_APS_ACK
    #define ZCL_DISABLE_APS_ACK             0
#endif

/* ZCL has all cooperative task */
#define COOPERATIVE

/* Global Attribute Types*/
typedef enum 
{
    /* Global attribute ID's (2.3.6.1) */
    E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP                          = 0xFFFC,
    E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION                     = 0xFFFD, /* Mandatory */
    E_CLD_GLOBAL_ATTR_ID_ATTRIBUTE_REPORTING_STATUS           = 0xFFFE
} teCLD_Global_AttrID;

/* Attribute types (from ZCL spec - Table 2.15) */
typedef enum 
{
    /* Null */
    E_ZCL_NULL            = 0x00,

    /* General Data */
    E_ZCL_GINT8           = 0x08,                // General 8 bit - not specified if signed or not
    E_ZCL_GINT16,
    E_ZCL_GINT24,
    E_ZCL_GINT32,
    E_ZCL_GINT40,
    E_ZCL_GINT48,
    E_ZCL_GINT56,
    E_ZCL_GINT64,

    /* Logical */
    E_ZCL_BOOL            = 0x10,

    /* Bitmap */
    E_ZCL_BMAP8            = 0x18,                // 8 bit bitmap
    E_ZCL_BMAP16,
    E_ZCL_BMAP24,
    E_ZCL_BMAP32,
    E_ZCL_BMAP40,
    E_ZCL_BMAP48,
    E_ZCL_BMAP56,
    E_ZCL_BMAP64,

    /* Unsigned Integer */
    E_ZCL_UINT8           = 0x20,                // Unsigned 8 bit
    E_ZCL_UINT16,
    E_ZCL_UINT24,
    E_ZCL_UINT32,
    E_ZCL_UINT40,
    E_ZCL_UINT48,
    E_ZCL_UINT56,
    E_ZCL_UINT64,

    /* Signed Integer */
    E_ZCL_INT8            = 0x28,                // Signed 8 bit
    E_ZCL_INT16,
    E_ZCL_INT24,
    E_ZCL_INT32,
    E_ZCL_INT40,
    E_ZCL_INT48,
    E_ZCL_INT56,
    E_ZCL_INT64,

    /* Enumeration */
    E_ZCL_ENUM8            = 0x30,                // 8 Bit enumeration
    E_ZCL_ENUM16,

    /* Floating Point */
    E_ZCL_FLOAT_SEMI    = 0x38,                // Semi precision
    E_ZCL_FLOAT_SINGLE,                        // Single precision
    E_ZCL_FLOAT_DOUBLE,                        // Double precision

    /* String */
    E_ZCL_OSTRING        = 0x41,                // Octet string
    E_ZCL_CSTRING,                            // Character string
    E_ZCL_LOSTRING,                            // Long octet string
    E_ZCL_LCSTRING,                            // Long character string

    /* Ordered Sequence */
    E_ZCL_ARRAY          = 0x48,
    E_ZCL_STRUCT         = 0x4c,

    E_ZCL_SET            = 0x50,
    E_ZCL_BAG            = 0x51,

    /* Time */
    E_ZCL_TOD            = 0xe0,                // Time of day
    E_ZCL_DATE,                                // Date
    E_ZCL_UTCT,                                // UTC Time

    /* Identifier */
    E_ZCL_CLUSTER_ID    = 0xe8,                // Cluster ID
    E_ZCL_ATTRIBUTE_ID,                        // Attribute ID
    E_ZCL_BACNET_OID,                        // BACnet OID

    /* Miscellaneous */
    E_ZCL_IEEE_ADDR        = 0xf0,                // 64 Bit IEEE Address
    E_ZCL_KEY_128,                            // 128 Bit security key, currently not supported as it would add to code space in u16ZCL_WriteTypeNBO and add extra 8 bytes to report config record for each reportable attribute

    /* NOTE:
     * 0xfe is a reserved value, however we are using it to denote a message signature.
     * This may have to change some time if ZigBee ever allocate this value to a data type
     */
    E_ZCL_SIGNATURE     = 0xfe,             // ECDSA Signature (42 bytes)

    /* Unknown */
    E_ZCL_UNKNOWN        = 0xff

} teZCL_ZCLAttributeType;

/* Size of an ECDSA Signature's R & S components concatenated */
#define E_ZCL_SIGNATURE_SIZE    (42)
#define E_ZCL_KEY_128_SIZE      (16)

#ifndef MAX_ATTRIBUTES_IN_ANY_CLUSTER
#define MAX_ATTRIBUTES_IN_ANY_CLUSTER 50
#endif

/* Attribute flags */
#define E_ZCL_AF_RD         (1 << 0)            // Readable
#define E_ZCL_AF_WR         (1 << 1)            // Writable
#define E_ZCL_AF_RP         (1 << 2)            // Report attribute
#define E_ZCL_AF_MS         (1 << 3)            // Manufacturer specific
#define E_ZCL_AF_CA         (1 << 4)            // Client attribute
#define E_ZCL_AF_SE         (1 << 5)            // Scenes extension
#define E_ZCL_AF_GA         (1 << 6)            // Global attribute

#define E_ZCL_CF_RX         (1 << 0)            // Received
#define E_ZCL_CF_TX         (1 << 1)            // Trannsmitted
#define E_ZCL_CF_MS         (1 << 3)            // Manufacturer specific

/* Attribute Control Bits */
#define E_ZCL_ACF_RS        (1 << 0)          /* Attribute Request Status */
#define E_ZCL_ACF_RP        (1 << 1)          /* Is attribute reportable */
#define E_ZCL_ACF_MR        (1 << 2)          /* Is attribute value is updated at mirror by end device */

// ZCL command types (from ZCL spec) - implemented as #defines since custom
// commands require access
#define E_ZCL_READ_ATTRIBUTES                           (0x00)
#define E_ZCL_READ_ATTRIBUTES_RESPONSE                  (0x01)
#define E_ZCL_WRITE_ATTRIBUTES                          (0x02)
#define E_ZCL_WRITE_ATTRIBUTES_UNDIVIDED                (0x03)
#define E_ZCL_WRITE_ATTRIBUTES_RESPONSE                 (0x04)
#define E_ZCL_WRITE_ATTRIBUTES_NO_RESPONSE              (0x05)
#define E_ZCL_CONFIGURE_REPORTING                       (0x06)
#define E_ZCL_CONFIGURE_REPORTING_RESPONSE              (0x07)
#define E_ZCL_READ_REPORTING_CONFIGURATION              (0x08)
#define E_ZCL_READ_REPORTING_CONFIGURATION_RESPONSE     (0x09)
#define E_ZCL_REPORT_ATTRIBUTES                         (0x0a)
#define E_ZCL_DEFAULT_RESPONSE                          (0x0b)
#define E_ZCL_DISCOVER_ATTRIBUTES                       (0x0c)
#define E_ZCL_DISCOVER_ATTRIBUTES_RESPONSE              (0x0d)
#define E_ZCL_READ_ATTRIBUTES_STRUCTURED                (0x0e)
#define E_ZCL_WRITE_ATTRIBUTES_STRUCTURED               (0x0f)
#define E_ZCL_WRITE_ATTRIBUTES_STRUCTURED_RESPONSE      (0x10)
#define E_ZCL_DISCOVER_COMMANDS_RECEIVED                (0x11)
#define E_ZCL_DISCOVER_COMMANDS_RECEIVED_RESPONSE       (0x12)
#define E_ZCL_DISCOVER_COMMANDS_GENERATED               (0x13)
#define E_ZCL_DISCOVER_COMMANDS_GENERATED_RESPONSE      (0x14)
#define E_ZCL_DISCOVER_ATTRIBUTES_EXTENDED              (0x15)
#define E_ZCL_DISCOVER_ATTRIBUTES_EXTENDED_RESPONSE     (0x16)


/* Flags used for custom commands */
#define E_ZCL_ACCEPT_EXACT                              (0x00)
#define E_ZCL_ACCEPT_LESS                               (0x01)
#define E_ZCL_DISABLE_DEFAULT_RESPONSE                  (0x02)  // Cluster will send a response so don't send SUCCESS default response
#define E_ZCL_DISABLE_DEFAULT_RESPONSE_ERROR            (0x03)  // Cluster will send a response under error conditions, e.g. KEC Terminate when message corrupted
                                                                // Must Or the flags if don't want success or error, e.g. E_ZCL_DISABLE_DEFAULT_RESPONSE | E_ZCL_DISABLE_DEFAULT_RESPONSE_ERROR

#define REPORTING_MAXIMUM_PERIODIC_TURNED_OFF           (0x0000)
#define REPORTING_MAXIMUM_TURNED_OFF                    (0xffff)
#define REPORTING_MINIMUM_LIMIT_NONE                    (0x0000)
#define REPORTING_MINIMUM_NOT_SET                       (0xffff)
#define REPORTS_OF_ATTRIBUTE_NOT_SUBJECT_TO_TIMEOUT     (0x0000)

#define SEND_SECURITY_MASK             0x0F
#define CLUSTER_MIRROR_BIT             0x10
#define CLUSTER_APS_ACK_ENABLE_MASK    0x20             

#define ZCL_GP_PROXY_ENDPOINT_ID   0xF2
// Minimum security level required to send a message(from ZCL spec)
typedef enum 
{
    E_ZCL_SECURITY_NETWORK = 0x00,
    E_ZCL_SECURITY_APPLINK,
    E_ZCL_SECURITY_TEMP_APPLINK,
    E_ZCL_SECURITY_ENUM_END
} teZCL_ZCLSendSecurity;

// ZCL command address modes (from ZCL spec)
typedef enum 
{
    E_ZCL_AM_BOUND,
    E_ZCL_AM_GROUP,
    E_ZCL_AM_SHORT,
    E_ZCL_AM_IEEE,
    E_ZCL_AM_BROADCAST,
    E_ZCL_AM_NO_TRANSMIT,
    E_ZCL_AM_BOUND_NO_ACK,
    E_ZCL_AM_SHORT_NO_ACK,
    E_ZCL_AM_IEEE_NO_ACK,
    E_ZCL_AM_BOUND_NON_BLOCKING,
    E_ZCL_AM_BOUND_NON_BLOCKING_NO_ACK,
    E_ZCL_AM_ENUM_END /* enum End */
} teZCL_AddressMode;

// ZCL Command status (from ZCL spec)
typedef enum 
{
    E_ZCL_CMDS_SUCCESS =0x00,
    E_ZCL_CMDS_FAILURE,
    E_ZCL_CMDS_NOT_AUTHORIZED =0x7e,
    E_ZCL_CMDS_RESERVED_FIELD_NOT_ZERO,
    E_ZCL_CMDS_MALFORMED_COMMAND =0x80,
    E_ZCL_CMDS_UNSUP_CLUSTER_COMMAND,
    E_ZCL_CMDS_UNSUP_GENERAL_COMMAND,
    E_ZCL_CMDS_UNSUP_MANUF_CLUSTER_COMMAND,
    E_ZCL_CMDS_UNSUP_MANUF_GENERAL_COMMAND,
    E_ZCL_CMDS_INVALID_FIELD,
    E_ZCL_CMDS_UNSUPPORTED_ATTRIBUTE,
    E_ZCL_CMDS_INVALID_VALUE,
    E_ZCL_CMDS_READ_ONLY,
    E_ZCL_CMDS_INSUFFICIENT_SPACE,
    E_ZCL_CMDS_DUPLICATE_EXISTS,
    E_ZCL_CMDS_NOT_FOUND,
    E_ZCL_CMDS_UNREPORTABLE_ATTRIBUTE,
    E_ZCL_CMDS_INVALID_DATA_TYPE,
    E_ZCL_CMDS_INVALID_SELECTOR,
    E_ZCL_CMDS_WRITE_ONLY,
    E_ZCL_CMDS_INCONSISTENT_STARTUP_STATE,
    E_ZCL_CMDS_DEFINED_OUT_OF_BAND,
    E_ZCL_CMDS_INCONSISTENT,
    E_ZCL_CMDS_ACTION_DENIED,
    E_ZCL_CMDS_TIMEOUT,
    E_ZCL_CMDS_HARDWARE_FAILURE =0xc0,
    E_ZCL_CMDS_SOFTWARE_FAILURE,
    E_ZCL_CMDS_CALIBRATION_ERROR,
    E_ZCL_CMDS_UNSUPPORTED_CLUSTER,
    E_ZCL_CMDS_ENUM_END
} teZCL_CommandStatus;

// ZCL callback structure enums
typedef enum 
{
   E_ZCL_CBET_LOCK_MUTEX = 0x0,             // get system mutex
   E_ZCL_CBET_UNLOCK_MUTEX,                 // release system mutex
   E_ZCL_CBET_UNHANDLED_EVENT,              // unhandled event call (like Data Confirm)
   E_ZCL_CBET_READ_INDIVIDUAL_ATTRIBUTE_RESPONSE,     // read response indication received
   E_ZCL_CBET_READ_ATTRIBUTES_RESPONSE,     // read response indication received
   E_ZCL_CBET_READ_REQUEST,                 // advance read warning to device to update attributes.  See also E_ZCL_CBET_REPORT_REQUEST
   E_ZCL_CBET_DEFAULT_RESPONSE,             // default response message handler
   E_ZCL_CBET_ERROR,                        // error callback
   E_ZCL_CBET_TIMER,                        // timer event
   E_ZCL_CBET_ZIGBEE_EVENT,                 // zigbee event
   E_ZCL_CBET_CLUSTER_CUSTOM,               // message defined in a cluster
   E_ZCL_CBET_WRITE_INDIVIDUAL_ATTRIBUTE,   // write received, called for each attribute
   E_ZCL_CBET_WRITE_ATTRIBUTES,              // write received, called once for each message after all atts updated
   E_ZCL_CBET_WRITE_INDIVIDUAL_ATTRIBUTE_RESPONSE, // Write attributes response received - only called for atts with write error
   E_ZCL_CBET_WRITE_ATTRIBUTES_RESPONSE,      // Write attributes response received - called once per message
   E_ZCL_CBET_CHECK_ATTRIBUTE_RANGE,          // Write attributes requests the application to check the range of the attribute
   E_ZCL_CBET_REPORT_TIMEOUT,
   E_ZCL_CBET_REPORT_INDIVIDUAL_ATTRIBUTE,
   E_ZCL_CBET_REPORT_ATTRIBUTES,
   E_ZCL_CBET_REPORT_INDIVIDUAL_ATTRIBUTES_CONFIGURE_RESPONSE,
   E_ZCL_CBET_REPORT_ATTRIBUTES_CONFIGURE,
   E_ZCL_CBET_REPORT_INDIVIDUAL_ATTRIBUTES_CONFIGURE,
   E_ZCL_CBET_REPORT_ATTRIBUTES_CONFIGURE_RESPONSE,
   E_ZCL_CBET_REPORT_READ_INDIVIDUAL_ATTRIBUTE_CONFIGURATION_RESPONSE,
   E_ZCL_CBET_REPORT_READ_ATTRIBUTE_CONFIGURATION_RESPONSE,
   E_ZCL_CBET_DISCOVER_INDIVIDUAL_ATTRIBUTE_RESPONSE,
   E_ZCL_CBET_DISCOVER_ATTRIBUTES_RESPONSE,
   E_ZCL_CBET_CLUSTER_UPDATE,               // Called when something on the local node causes any attribute to be changed
   E_ZCL_CBET_ATTRIBUTE_REPORT_MIRROR,
   E_ZCL_CBET_REPORT_REQUEST,                 // advance read warning to device to update attributes because a report is about to be generated
   E_ZCL_CBET_ENABLE_MS_TIMER,
   E_ZCL_CBET_DISABLE_MS_TIMER,
   E_ZCL_CBET_TIMER_MS,                        // Mili second timer event
   E_ZCL_CBET_ZGP_DATA_IND_ERROR,
   E_ZCL_CBET_DISCOVER_INDIVIDUAL_COMMAND_RECEIVED_RESPONSE,
   E_ZCL_CBET_DISCOVER_COMMAND_RECEIVED_RESPONSE,
   E_ZCL_CBET_DISCOVER_INDIVIDUAL_COMMAND_GENERATED_RESPONSE,
   E_ZCL_CBET_DISCOVER_COMMAND_GENERATED_RESPONSE,   
   E_ZCL_CBET_DISCOVER_INDIVIDUAL_ATTRIBUTE_EXTENDED_RESPONSE,
   E_ZCL_CBET_DISCOVER_ATTRIBUTES_EXTENDED_RESPONSE,
   E_ZCL_CBET_ENUM_END
} teZCL_CallBackEventType;

// ZCL return status codes returned to the user
typedef enum 
{
   // General
   E_ZCL_SUCCESS = 0x0,
   E_ZCL_FAIL,                  // 01
   E_ZCL_ERR_PARAMETER_NULL,    // 02
   E_ZCL_ERR_PARAMETER_RANGE,   // 03
   E_ZCL_ERR_HEAP_FAIL,         // 04
   // Specific ZCL status codes
   E_ZCL_ERR_EP_RANGE,          // 05
   E_ZCL_ERR_EP_UNKNOWN,        // 06
   E_ZCL_ERR_SECURITY_RANGE,    // 07
   E_ZCL_ERR_CLUSTER_0,         // 08
   E_ZCL_ERR_CLUSTER_NULL,      // 09
   E_ZCL_ERR_CLUSTER_NOT_FOUND, // 10
   E_ZCL_ERR_CLUSTER_ID_RANGE,  // 11
   E_ZCL_ERR_ATTRIBUTES_NULL,   // 12
   E_ZCL_ERR_ATTRIBUTES_0,      // 13
   E_ZCL_ERR_ATTRIBUTE_WO,      // 14
   E_ZCL_ERR_ATTRIBUTE_RO,      // 15
   E_ZCL_ERR_ATTRIBUTES_ACCESS, // 16
   E_ZCL_ERR_ATTRIBUTE_TYPE_UNSUPPORTED, // 17
   E_ZCL_ERR_ATTRIBUTE_NOT_FOUND,   // 18
   E_ZCL_ERR_CALLBACK_NULL,         // 19
   E_ZCL_ERR_ZBUFFER_FAIL,          // 20
   E_ZCL_ERR_ZTRANSMIT_FAIL,        // 21
   E_ZCL_ERR_CLIENT_SERVER_STATUS,  // 22
   E_ZCL_ERR_TIMER_RESOURCE,        // 23
   E_ZCL_ERR_ATTRIBUTE_IS_CLIENT,   // 24
   E_ZCL_ERR_ATTRIBUTE_IS_SERVER,   // 25
   E_ZCL_ERR_ATTRIBUTE_RANGE,       // 26
   E_ZCL_ERR_ATTRIBUTE_MISMATCH,    // 27
   E_ZCL_ERR_KEY_ESTABLISHMENT_MORE_THAN_ONE_CLUSTER, //28
   E_ZCL_ERR_INSUFFICIENT_SPACE,    // 29
   E_ZCL_ERR_NO_REPORTABLE_CHANGE,  // 30
   E_ZCL_ERR_NO_REPORT_ENTRIES,     // 31
   E_ZCL_ERR_ATTRIBUTE_NOT_REPORTABLE, //32
   E_ZCL_ERR_ATTRIBUTE_ID_ORDER,    // 33
   E_ZCL_ERR_MALFORMED_MESSAGE,     // 34
   E_ZCL_ERR_MANUFACTURER_SPECIFIC, // 35
   E_ZCL_ERR_PROFILE_ID,            // 36
   E_ZCL_ERR_INVALID_VALUE,         // 37
   E_ZCL_ERR_CERT_NOT_FOUND,        // 38
   E_ZCL_ERR_CUSTOM_DATA_NULL,      // 39
   E_ZCL_ERR_TIME_NOT_SYNCHRONISED, // 40
   E_ZCL_ERR_SIGNATURE_VERIFY_FAILED, //41
   E_ZCL_ERR_ZRECEIVE_FAIL,          // 42
   E_ZCL_ERR_KEY_ESTABLISHMENT_END_POINT_NOT_FOUND, // 43
   E_ZCL_ERR_KEY_ESTABLISHMENT_CLUSTER_ENTRY_NOT_FOUND, // 44
   E_ZCL_ERR_KEY_ESTABLISHMENT_CALLBACK_ERROR, // 45
   E_ZCL_ERR_SECURITY_INSUFFICIENT_FOR_CLUSTER, // 46
   E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR, // 47
   E_ZCL_ERR_INVALID_IMAGE_SIZE,         // 48
   E_ZCL_ERR_INVALID_IMAGE_VERSION,         // 49
   E_ZCL_READ_ATTR_REQ_NOT_FINISHED,         // 50
   E_ZCL_DENY_ATTRIBUTE_ACCESS,             // 51
   E_ZCL_ERR_SECURITY_FAIL,                 // 52
   E_ZCL_ERR_CLUSTER_COMMAND_NOT_FOUND,
   E_ZCL_ERR_SCENE_NOT_FOUND,
   E_ZCL_RESTORE_DEFAULT_REPORT_CONFIGURATION, 
   E_ZCL_ERR_ENUM_END
} teZCL_Status;

typedef enum 
{
    E_ZCL_TIMER_STOP = 0x0,
    E_ZCL_TIMER_CLICK,
    E_ZCL_TIMER_MOD,
    E_ZCL_TIMER_VALUE,
    E_ZCL_TIMER_INTERVAL,
    E_ZCL_TIMER_CLICK_MS,
    // always last
    E_ZCL_TIMER_ENUM_END
} teZCL_TimerMode;

/* Events passed to scene event handlers in clusters */
typedef enum 
{
    E_ZCL_SCENE_EVENT_SAVE = 0x0,
    E_ZCL_SCENE_EVENT_RECALL
} teZCL_SceneEvent;

/*Holding status for ZCL report attribute command*/
typedef enum 
{
    E_ZCL_ATTR_REPORT_OK = 0x00,
    E_ZCL_ATTR_REPORT_EP_MISMATCH,
    E_ZCL_ATTR_REPORT_ADDR_MISMATCH,
    E_ZCL_ATTR_REPORT_ERR
} teZCL_ReportAttributeStatus;

/* Bit wise processing Macros */
#define ZCL_OFFSET(strct, member)           (uint32)&(((strct *)0)->member)
#define ZCL_BIT(type, n)                    ( (type)1 << n )
#define ZCL_BIT_CLEAR(type, reg, mask)      ( reg &= ~((type)mask) )
#define ZCL_BIT_SET(type, reg, mask)        ( reg |= (type)mask )
/* returns 1, if bit is set */
#define ZCL_IS_BIT_SET(type, reg, mask)     ( (reg & (type)mask) != 0 )
/* returns 1, if bit is clear */
#define ZCL_IS_BIT_CLEAR(type, reg, mask)   ( (reg & (type)mask) == 0 )
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/******************/
/* ZCL Data Types */
/******************/

/* General data */
typedef uint8     zgint8;
typedef uint16    zgint16;
typedef uint32    zgint24;
typedef uint32    zgint32;
typedef uint64    zgint40;
typedef uint64    zgint48;
typedef uint64    zgint56;
typedef uint64    zgint64;

/* Logical */
typedef bool_t    zbool;

/* Bitmaps */
typedef uint8     zbmap8;
typedef uint16    zbmap16;
typedef uint32    zbmap24;
typedef uint32    zbmap32;
typedef uint64    zbmap40;
typedef uint64    zbmap48;
typedef uint64    zbmap56;
typedef uint64    zbmap64;

/* Unsigned Integer */
typedef uint8     zuint8;
typedef uint16    zuint16;
typedef uint32    zuint24;
typedef uint32    zuint32;
typedef uint64    zuint40;
typedef uint64    zuint48;
typedef uint64    zuint56;
typedef uint64    zuint64;

/* Signed Integer */
typedef int8     zint8;
typedef int16    zint16;
typedef int32    zint24;
typedef int32    zint32;
typedef int64    zint40;
typedef int64    zint48;
typedef int64    zint56;
typedef int64    zint64;

/* Enumeration */
typedef uint8     zenum8;
typedef uint16    zenum16;

/* Floating Point */
typedef uint16    zsemi;
typedef float     zsingle;
typedef double    zdouble;

/* String */
typedef struct
{
    uint8    u8MaxLength;
    uint8    u8Length;
    uint8   *pu8Data;
} tsZCL_OctetString;

typedef struct
{
    uint8    u8MaxLength;
    uint8    u8Length;
    uint8   *pu8Data;
} tsZCL_CharacterString;


typedef struct
{
    uint16    u16MaxLength;
    uint16    u16Length;
    uint8    *pu8Data;
  
} tsZCL_LongOctetString;

typedef struct
{
    uint16    u16MaxLength;
    uint16    u16Length;
    uint8    *pu8Data;
  
} tsZCL_LongCharacterString;


/* Ordered Sequence */
typedef struct
{
    uint16    u16MaxLength;
    uint16    u16Length;
    uint8    *pu8Data;
} tsZCL_Array;

typedef struct
{
    uint16    u16MaxLength;
    uint16    u16Length;
    uint8    *pu8Data;
} tsZCL_Structure;


/* Time */
typedef uint32    ztime;
typedef uint32    zdate;
typedef uint32    zutctime;

/* Identifier */
typedef uint16    zclusterid;
typedef uint16    zattributeid;
typedef uint32    zbacnetid;

/* Miscellaneous */
typedef uint64    zieeeaddress;

typedef struct
{
    uint8    au8Key[E_ZCL_KEY_128_SIZE];
} tsZCL_Key;

/* Used for building custom commands */
typedef struct
{
    uint16                  u16Quantity;
    teZCL_ZCLAttributeType  eType;
    void                    *pvData;
} tsZCL_TxPayloadItem;


/* Used for receiving custom commands */
typedef struct
{
    uint16                  u16MaxQuantity;
    uint16                  *pu16ActualQuantity;
    teZCL_ZCLAttributeType  eType;
    void                    *pvDestination;
} tsZCL_RxPayloadItem;


typedef enum PACKED
{
    eFRAME_TYPE_COMMAND_ACTS_ACCROSS_ENTIRE_PROFILE = 0x00,
    eFRAME_TYPE_COMMAND_IS_SPECIFIC_TO_A_CLUSTER
} eFrameTypeSubfield;

typedef struct
{
    eFrameTypeSubfield  eFrameType;
    bool_t              bManufacturerSpecific;
    uint16              u16ManufacturerCode;
    bool_t              bDirection;
    bool_t              bDisableDefaultResponse;
    uint8               u8TransactionSequenceNumber;
    uint8               u8CommandIdentifier;
} tsZCL_HeaderParams;

typedef struct
{
  teZCL_AddressMode eAddressMode;
  union {
        uint16 u16GroupAddress;
        uint16 u16DestinationAddress;
        uint64 u64DestinationAddress;
        ZPS_teAplAfBroadcastMode eBroadcastMode;
    }uAddress;
} tsZCL_Address;

// Definition of Read / Write Individual Attribute Response Message Structure
typedef struct {
    uint16                      u16AttributeEnum;
    teZCL_ZCLAttributeType      eAttributeDataType;
    teZCL_CommandStatus         eAttributeStatus;
    void                       *pvAttributeData;
} tsZCL_IndividualAttributesResponse;

// Definition of Default Response Message Structure
typedef struct {
     uint8  u8CommandId;        // The ID of the command that caused the default response message.
     uint8  u8StatusCode;
} tsZCL_DefaultResponse;

// Definition of Timer Message Structure
typedef struct {
    teZCL_TimerMode            eTimerMode;
    uint32                     u32UTCTime;
} tsZCL_TimerMessage;


// Definition of a Cluster Custom Message Structure
typedef struct {
     uint16                     u16ClusterId;
     void                       *pvCustomData;
} tsZCL_ClusterCustomMessage;

typedef union
{
/* Unsigned Integer */
    zuint8      zuint8ReportableChange;
    zuint16     zuint16ReportableChange;
    zuint24     zuint24ReportableChange;
    zuint32     zuint32ReportableChange;
    zuint40     zuint40ReportableChange;
    zuint48     zuint48ReportableChange;
    zuint56     zuint56ReportableChange;
    zuint64     zuint64ReportableChange;

/* Signed Integer */
    zint8       zint8ReportableChange;
    zint16      zint16ReportableChange;
    zint24      zint24ReportableChange;
    zint32      zint32ReportableChange;
    zint40      zint40ReportableChange;
    zint48      zint48ReportableChange;
    zint56      zint56ReportableChange;
    zint64      zint64ReportableChange;

/* Floating Point */
    zsemi       zsemiReportableChange;
    zsingle     zsingleReportableChange;
    zdouble     zdoubleReportableChange;

/* Time */
    ztime       ztimeReportableChange;
    zdate       zdateReportableChange;
    zutctime    zutctimeReportableChange;
} tuZCL_AttributeReportable;

// Pre-Definition of cluster Instance and Endpoint structures
typedef struct tsZCL_ClusterInstance     tsZCL_ClusterInstance;
typedef struct tsZCL_EndPointDefinition  tsZCL_EndPointDefinition;
typedef struct tsZCL_AttributeDefinition tsZCL_AttributeDefinition;
typedef struct tsZCL_CommandDefinition   tsZCL_CommandDefinition;
typedef struct
{
    uint8                           u8DirectionIsReceived;
    teZCL_ZCLAttributeType          eAttributeDataType;
    uint16                          u16AttributeEnum;                   // attribute Id
    uint16                          u16MinimumReportingInterval;
    uint16                          u16MaximumReportingInterval;
    uint16                          u16TimeoutPeriodField;
    tuZCL_AttributeReportable       uAttributeReportableChange;

} tsZCL_AttributeReportingConfigurationRecord;

typedef union
{
    /* entries used to store present values for types which aren't differenced */
    uint64                       u64AttributeValue;
    int64                        i64AttributeValue;

    zsemi                        zsemiValue;
    zsingle                      zsingleValue;
    zdouble                      zdoubleValue;

    tsZCL_Key                    sKey;

    tsZCL_OctetString            sOctetString;
    tsZCL_LongOctetString        sLongOctetString;
    tsZCL_CharacterString        sCharacterString;
    tsZCL_LongCharacterString    sLongCharacterString;
    tuZCL_AttributeReportable    uAttributeReportable;
} tuZCL_AttributeStorage;

typedef struct
{
    uint8                       u8DirectionIsReceived;
    uint16                      u16AttributeEnum;                   // attribute Id
} tsZCL_AttributeReadReportingConfigurationRecord;

typedef struct
{
    uint8                       u8Status;
    uint8                       u8DirectionIsReceived;
    uint16                      u16AttributeEnum;
} tsZCL_AttributeStatusRecord;

typedef struct
{
    uint16                      u16AttributeEnum;
    teZCL_ZCLAttributeType      eAttributeDataType;                 // Zigbee data type
} tsZCL_AttributeReportRecord;

typedef struct
{
    uint8                       u8DestinationEndPoint;
    uint16                      u16ClusterId;
    uint64                      u64RemoteIeeeAddress;
    teZCL_ReportAttributeStatus eStatus;
}tsZCL_ReportAttributeMirror;

typedef struct
{
    bool_t                      bDiscoveryComplete;
    uint16                      u16AttributeEnum;                   // attribute Id
    teZCL_ZCLAttributeType      eAttributeDataType;
} tsZCL_AttributeDiscoveryResponse;

typedef struct
{
    bool_t                      bDiscoveryComplete;
    uint16                      u16AttributeEnum;                   // attribute Id
    teZCL_ZCLAttributeType      eAttributeDataType;
    uint8                       u8AttributeFlags;
}tsZCL_AttributeDiscoveryExtendedResponse;

typedef struct
{
    uint8                       u8CommandEnum;                   // CommandId
    uint8                       u8CommandIndex;
} tsZCL_CommandDiscoveryIndividualResponse;

typedef struct
{
    bool_t                      bDiscoveryComplete;              // CommandId
    uint8                       u8NumberOfCommands;
} tsZCL_CommandDiscoveryResponse;

typedef struct
{
    teZCL_CommandStatus                         eCommandStatus;
    tsZCL_AttributeReportingConfigurationRecord sAttributeReportingConfigurationRecord;
}tsZCL_AttributeReportingConfigurationResponse;

// Definition of Callback Event Structure
// Structure reordered for SE1.7
typedef struct
{
   teZCL_CallBackEventType                  eEventType;
   uint8                                    u8TransactionSequenceNumber;
   uint8                                    u8EndPoint;
   teZCL_Status                             eZCL_Status;

   union {
       tsZCL_IndividualAttributesResponse   sIndividualAttributeResponse;  // Read Att Response and Write Att per att CB.
       tsZCL_DefaultResponse                sDefaultResponse;
       tsZCL_TimerMessage                   sTimerMessage;
       tsZCL_ClusterCustomMessage           sClusterCustomMessage;
       tsZCL_AttributeReportingConfigurationRecord sAttributeReportingConfigurationRecord;
       tsZCL_AttributeReportingConfigurationResponse sAttributeReportingConfigurationResponse;
       tsZCL_AttributeDiscoveryResponse     sAttributeDiscoveryResponse;
       tsZCL_AttributeStatusRecord          sReportingConfigurationResponse;
       tsZCL_ReportAttributeMirror          sReportAttributeMirror;
       uint32                               u32TimerPeriodMs;

       tsZCL_CommandDiscoveryIndividualResponse     sCommandsReceivedDiscoveryIndividualResponse;
       tsZCL_CommandDiscoveryResponse               sCommandsReceivedDiscoveryResponse;
       tsZCL_CommandDiscoveryIndividualResponse     sCommandsGeneratedDiscoveryIndividualResponse;
       tsZCL_CommandDiscoveryResponse               sCommandsGeneratedDiscoveryResponse;
       tsZCL_AttributeDiscoveryExtendedResponse     sAttributeDiscoveryExtenedResponse;
       tsZCL_AttributeReportRecord                   sAttributeReportRecord;
   }uMessage ;
   ZPS_tsAfEvent                           *pZPSevent;
   tsZCL_ClusterInstance                   *psClusterInstance;
} tsZCL_CallBackEvent;

typedef void (* tfpZCL_ZCLCallBackFunction)(tsZCL_CallBackEvent *pCallBackEvent);

// Definition of a single attribute
struct tsZCL_AttributeDefinition
{
    uint16                      u16AttributeEnum;                   // attribute Id
    uint8                       u8AttributeFlags;                   // attribute flags
    teZCL_ZCLAttributeType      eAttributeDataType;                 // Zigbee data type
    uint16                      u16OffsetFromStructBase;            // attribute offset in cluster
    uint16                      u16AttributeArrayLength;            // length of consecutive attributes of same type
};

struct tsZCL_CommandDefinition
{
    uint8                       u8CommandEnum;                   // CommandId Id
    uint8                       u8CommandFlags;                   // Command flags
};

/* Function prototype for scene event handler */
typedef teZCL_Status (*tfpZCL_SceneEventHandler)(
        teZCL_SceneEvent            eEvent,
        tsZCL_EndPointDefinition   *psEndPointDefinition,
        tsZCL_ClusterInstance      *psClusterInstance);

typedef struct
{
    uint16                      u16Attribute;
    teZCL_ZCLAttributeType      eAttributeDataType;
} tsZCL_SceneExtensionAttribute;

// Scene extension table
typedef struct
{
    tfpZCL_SceneEventHandler    pSceneEventHandler;
    uint16                      u16NumberOfAttributes;
    tsZCL_SceneExtensionAttribute au16Attributes[];
} tsZCL_SceneExtensionTable;

// Definition of a cluster
typedef struct
{
    uint16                      u16ClusterEnum;                     // cluster Id
    bool_t                      bIsManufacturerSpecificCluster;     // Zigbee or proprietry indicator
    uint8                       u8ClusterControlFlags;                // Lower Nibble holds sercurity Info, Upper nibble is for ClusterType
    uint16                      u16NumberOfAttributes;              // number of attributes in cluster
    tsZCL_AttributeDefinition  *psAttributeDefinition;              // pointer to array of attribute definition structures
    tsZCL_SceneExtensionTable  *psSceneExtensionTable;              // pointer to scene extension table, if any
    #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED    
        uint8                       u8NumberOfCommands;
        tsZCL_CommandDefinition    *psCommandDefinition;
    #endif    
}  tsZCL_ClusterDefinition;

// Definition of custom command callback function
typedef teZCL_Status (* tfpZCL_ZCLCustomcallCallBackFunction)(
    ZPS_tsAfEvent              *pZPSEvent,
    tsZCL_EndPointDefinition   *psEndPointDefinition,
    tsZCL_ClusterInstance      *psClusterInstance);

// Definition of long octet read write callback function
typedef teZCL_Status (* tfpZCL_ZCLStringCallBackFunction)(
                                            uint8                       u8EndPoint,
                                            uint16                      u16AttributeId,
                                            bool_t                      IsClientAttribute,
                                            bool_t                      bWriteAttributeValueIntoBuffer,
                                            PDUM_thAPduInstance         hAPduInst,
                                            uint16                      u16offset,
                                            uint16                      *pu16NoOfBytes);

// Definition of a cluster Instance - indicates Client/Server status and stores data
struct tsZCL_ClusterInstance
{
    bool_t                      bIsServer;                          // denotes if cluster is server/client type
    tsZCL_ClusterDefinition    *psClusterDefinition;                // cluster definition structure
    void                       *pvEndPointSharedStructPtr;          // attribute data
    uint8                      *pu8AttributeControlBits;            // array of attribute control bits
    void                       *pvEndPointCustomStructPtr;          // custom data
    tfpZCL_ZCLCustomcallCallBackFunction pCustomcallCallBackFunction;   // custom commands callback
    tfpZCL_ZCLStringCallBackFunction     pStringCallBackFunction;    // callback function to read write long octet string
} __attribute__ ((aligned(4)));

// Definition of an Endpoint
struct tsZCL_EndPointDefinition
{
    uint8                       u8EndPointNumber;                   // endpoint number 1..N, 0 reserved
    uint16                      u16ManufacturerCode;                // manufacturer code (valid when bIsManufacturerSpecificProfile TRUE)
    uint16                      u16ProfileEnum;                     // Zigbee profile Id
    bool_t                      bIsManufacturerSpecificProfile;     // Zigbee or proprietry indicator
    uint16                      u16NumberOfClusters;                // number of clusters in EP
    tsZCL_ClusterInstance      *psClusterInstance;                  // pointer to array of cluster Instance structures
    bool_t                      bDisableDefaultResponse;            // disable default response flag
    tfpZCL_ZCLCallBackFunction  pCallBackFunctions;                 // EP callback function mapper
};

// Definition of message signature creation function
typedef teZCL_Status (* tfpZCL_ZCLMessageSignatureCreationFunction)(
    uint8                       u8SourceEndPointId,
    uint8                       *pu8MessageData,
    uint32                      u32MessageLength,
    uint8                       *pu8R,
    uint8                       *pu8S);

// Definition of message signature verification function
typedef teZCL_Status (* tfpZCL_ZCLMessageSignatureVerificationFunction)(
    uint8                       u8SourceEndPointId,
    uint64                      u64MACAddress,
    uint8                       *pu8MessageData,
    uint32                      u32MessageLength,
    uint8                       *pu8R,
    uint8                       *pu8S);

// Passed in to eZCL_CreateZCL
typedef struct
{
    uint8                        u8NumberOfEndpoints;
    uint8                        u8NumberOfTimers;
    uint8                        u8NumberOfReports;
    uint16                       u16SystemMinimumReportingInterval;
    uint16                       u16SystemMaximumReportingInterval;
    tfpZCL_ZCLCallBackFunction   pfZCLcallBackFunction;
    tfpZCL_ZCLMessageSignatureCreationFunction      pfZCLSignatureCreationFunction;
    tfpZCL_ZCLMessageSignatureVerificationFunction  pfZCLSignatureVerificationFunction;
    PDUM_thAPdu                  hAPdu;
    uint32                       *pu32ZCL_Heap;
    uint32                       u32ZCL_HeapSizeInWords;
} tsZCL_Config;

typedef struct
{
    teZCL_ZCLAttributeType      eAttributeDataType;
    uint16                      u16AttributeEnum;
    uint8                       *pu8AttributeData;
}tsZCL_WriteAttributeRecord;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eZCL_CreateZCL(
                   tsZCL_Config                *psZCL_Config);

PUBLIC teZCL_Status eZCL_Register(
                   tsZCL_EndPointDefinition    *psEndPointDefinition);

PUBLIC  teZCL_Status eZCL_HandleReadAttributesResponse(
                   tsZCL_CallBackEvent         *psEvent,
                   uint8                       *pu8TransactionSequenceNumber);

PUBLIC teZCL_Status eZCL_SendReadAttributesRequest(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8NumberOfAttributesInRequest,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    uint16                     *pu16AttributeRequestList);

PUBLIC void vZCL_EventHandler(
                    tsZCL_CallBackEvent        *psZCLCallBackEvent);

PUBLIC teZCL_Status eZCL_TimerRegister(
                    teZCL_TimerMode             eTimerMode,
                    uint32                      u32UTCTime,
                    tfpZCL_ZCLCallBackFunction  pfZCLCallBackFunction);

PUBLIC void vZCL_SetUTCTime(
                    uint32                      u32UTCTime);

PUBLIC uint32 u32ZCL_GetUTCTime(void);
PUBLIC teZCL_Status eZCL_UpdateMsTimer(
                    tsZCL_EndPointDefinition   *psEndPointDefinition,
                    bool_t                      bEnable,
                    uint32                      u32TimerPeriodMs);

PUBLIC uint8 u8ZCL_GetNumberOfEndpointsRegistered(void);
PUBLIC PDUM_thAPdu hZCL_GetBufferPoolHandle(void);

PUBLIC ZPS_teStatus eZCL_GetLastZpsError(void);

PUBLIC bool_t bZCL_GetTimeHasBeenSynchronised(void);
PUBLIC void vZCL_ClearTimeHasBeenSynchronised(void);

PUBLIC teZCL_Status eZCL_ReportAllAttributes(
                    tsZCL_Address              *psDestinationAddress,
                    uint16                      u16ClusterID,
                    uint8                       u8SrcEndPoint,
                    uint8                       u8DestEndPoint,
                    PDUM_thAPduInstance         hAPduInst);

PUBLIC teZCL_Status eZCL_SendDiscoverAttributesRequest(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint16                      u16AttributeId,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    uint8                       u8MaximumNumberOfIdentifiers);

PUBLIC INLINE teZCL_Status eZCL_SendWriteAttributesRequest(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8NumberOfAttributesInRequest,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    tsZCL_WriteAttributeRecord *pu16AttributeRequestList) ALWAYS_INLINE;


PUBLIC INLINE teZCL_Status eZCL_SendWriteAttributesUndividedRequest(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8NumberOfAttributesInRequest,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    tsZCL_WriteAttributeRecord  *pu16AttributeRequestList) ALWAYS_INLINE;

PUBLIC INLINE teZCL_Status eZCL_SendWriteAttributesNoResponseRequest(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8NumberOfAttributesInRequest,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    tsZCL_WriteAttributeRecord  *pu16AttributeRequestList)ALWAYS_INLINE;

PUBLIC teZCL_Status eZCL_SendConfigureReportingCommand(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8NumberOfAttributesInRequest,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    tsZCL_AttributeReportingConfigurationRecord     *psAttributeReportingConfigurationRecord);

PUBLIC teZCL_Status eZCL_CreateLocalReport(
                    uint8                       u8SourceEndPointId,
                    uint16                      u16ClusterId,
                    bool_t                      bManufacturerSpecific,
                    bool_t                      bIsServerAttribute,
                    tsZCL_AttributeReportingConfigurationRecord     *psAttributeReportingConfigurationRecord);



PUBLIC teZCL_Status eZCL_SendReadReportingConfigurationCommand(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8NumberOfAttributesInRequest,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    tsZCL_AttributeReadReportingConfigurationRecord     *psAttributeReadReportingConfigurationRecord);

PUBLIC teZCL_Status eZCL_WriteToAttributeReportableStructure(
                     void                      *pvReportableChange,
                     teZCL_ZCLAttributeType     eAttributeDataType,
                     tuZCL_AttributeReportable *puAttributeReportableChange);

PUBLIC ZPS_teStatus eZCL_WriteLocalAttributeValue(
                     uint8                      u8SrcEndpoint,
                     uint16                     u16ClusterId,
					 bool_t                       bIsServerClusterInstance,
					 bool_t                       bManufacturerSpecific,
                     bool_t                     bIsClientAttribute,
                     uint16                     u16AttributeId,
                     void                      *pvAttributeValue);

PUBLIC ZPS_teStatus eZCL_ReadLocalAttributeValue(
                     uint8                      u8SrcEndpoint,
                     uint16                     u16ClusterId,
					 bool_t                       bIsServerClusterInstance,
					 bool_t                       bManufacturerSpecific,
                     bool_t                     bIsClientAttribute,
                     uint16                     u16AttributeId,
                     void                      *pvAttributeValue);

PUBLIC teZCL_Status eZCL_SetSupportedSecurity(
                    teZCL_ZCLSendSecurity       eSecuritySupported);

PUBLIC teZCL_Status eZCL_SetReportableFlag(
                    uint8                       u8SrcEndPoint,
                    uint16                      u6ClusterID,
					bool_t                        bIsServerClusterInstance,
					bool_t                        bIsManufacturerSpecific,
                    uint16                      u16AttributeId);

PUBLIC teZCL_Status eZCL_OverrideClusterControlFlags(
                    uint8                       u8SrcEndpoint,
                    uint16                      u16ClusterId,
					bool_t                        bIsServerClusterInstance,
                    uint8                       u8ClusterControlFlags);

PUBLIC  uint8 u8ZCL_GetClusterControlFlags(
                    uint8                       u8SrcEndpoint,
                    uint16                      u16ClusterId,
					bool_t                        bIsServerClusterInstance);
                    
PUBLIC  teZCL_Status eZCL_SetAPSACKClusterControlFlagsBit(
                    uint8                       u8SrcEndpoint,
                    uint16                      u16ClusterId,
					bool_t                        bIsServerClusterInstance,
					bool_t                        bSet);
                    
PUBLIC teZCL_Status  eZCL_SendDiscoverCommandReceivedRequest(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,                
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8CommandId,                    
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    uint8                       u8MaximumNumberOfIdentifiers);
                    
PUBLIC teZCL_Status  eZCL_SendDiscoverCommandGeneratedRequest(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,                
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8CommandId,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    uint8                       u8MaximumNumberOfIdentifiers);

PUBLIC teZCL_Status eZCL_SendDiscoverAttributesExtendedRequest(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint16                      u16AttributeId,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    uint8                       u8MaximumNumberOfIdentifiers);

PUBLIC void vReverseMemcpy(uint8 *pu8OutData, uint8 *pu8InData, uint8 u8NumOfBytes);

PUBLIC void vZCL_InitializeClusterInstance(
                tsZCL_ClusterInstance                              *psClusterInstance,
                bool_t                                             bIsServer,
                tsZCL_ClusterDefinition                            *psClusterDefinition,
                void                                               *pvEndPointSharedStructPtr,
                uint8                                              *pu8AttributeControlBits,
                void                                               *psCustomDataStructure,
                tfpZCL_ZCLCustomcallCallBackFunction               pCustomcallCallBackFunction) __attribute__((noinline)); 
                
PUBLIC teZCL_Status eZCL_Initialise(tfpZCL_ZCLCallBackFunction cbCallBack,
                                   PDUM_thAPdu hAPdu);

PUBLIC teZCL_Status eZCL_Update100mS(void);   

PUBLIC void vZCL_DisableAPSACK(
                                bool_t       bDisableAPSACK);
     
PUBLIC uint16 u16ZCL_GetTxPayloadSize(uint16 u16DestAddr);

PUBLIC teZCL_Status eZCL_ReportAttribute(
                    tsZCL_Address               *psDestinationAddress,
                    uint16                      u16ClusterID,
                    uint16                      u16AttributeID,
                    uint8                       u8SrcEndPoint,
                    uint8                       u8DestEndPoint,
                    PDUM_thAPduInstance         hAPduInst);
PUBLIC teZCL_Status eZCL_SearchForEPIndex(
                    uint8                       u8endpointId,
                    uint8                       *pu8EndpointIndex); 
PUBLIC uint8 u8ZCL_GetEPIdFromIndex(
                    uint8                       u8EndpointIndex);                   
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#ifndef PC_PLATFORM_BUILD
extern PUBLIC const uint32  gu32ZCLVersion;
#endif


/****************************************************************************/
/***        Implementations for Inline Functions                          ***/
/****************************************************************************/

PUBLIC teZCL_Status eZCL_SendWriteAttributesRequestInternal(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8NumberOfAttributesInRequest,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    tsZCL_WriteAttributeRecord  *pu16AttributeRequestList,
                    uint8                       u8CommandId);

PUBLIC INLINE teZCL_Status eZCL_SendWriteAttributesRequest(
                    uint8                                   u8SourceEndPointId,
                    uint8                                   u8DestinationEndPointId,
                    uint16                                  u16ClusterId,
                    bool_t                                  bDirectionIsServerToClient,
                    tsZCL_Address                           *psDestinationAddress,
                    uint8                                   *pu8TransactionSequenceNumber,
                    uint8                                   u8NumberOfAttributesInRequest,
                    bool_t                                  bIsManufacturerSpecific,
                    uint16                                  u16ManufacturerCode,
                    tsZCL_WriteAttributeRecord               *pu16AttributeRequestList)
{

    return eZCL_SendWriteAttributesRequestInternal(
                    u8SourceEndPointId,
                    u8DestinationEndPointId,
                    u16ClusterId,
                    bDirectionIsServerToClient,
                    psDestinationAddress,
                    pu8TransactionSequenceNumber,
                    u8NumberOfAttributesInRequest,
                    bIsManufacturerSpecific,
                    u16ManufacturerCode,
                    pu16AttributeRequestList,
                    E_ZCL_WRITE_ATTRIBUTES);
}

PUBLIC INLINE teZCL_Status eZCL_SendWriteAttributesUndividedRequest(
                    uint8                                   u8SourceEndPointId,
                    uint8                                   u8DestinationEndPointId,
                    uint16                                  u16ClusterId,
                    bool_t                                  bDirectionIsServerToClient,
                    tsZCL_Address                           *psDestinationAddress,
                    uint8                                   *pu8TransactionSequenceNumber,
                    uint8                                   u8NumberOfAttributesInRequest,
                    bool_t                                  bIsManufacturerSpecific,
                    uint16                                  u16ManufacturerCode,
                    tsZCL_WriteAttributeRecord             *pu16AttributeRequestList)
{

    return eZCL_SendWriteAttributesRequestInternal(
                    u8SourceEndPointId,
                    u8DestinationEndPointId,
                    u16ClusterId,
                    bDirectionIsServerToClient,
                    psDestinationAddress,
                    pu8TransactionSequenceNumber,
                    u8NumberOfAttributesInRequest,
                    bIsManufacturerSpecific,
                    u16ManufacturerCode,
                    pu16AttributeRequestList,
                    E_ZCL_WRITE_ATTRIBUTES_UNDIVIDED);
}

PUBLIC INLINE teZCL_Status eZCL_SendWriteAttributesNoResponseRequest(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8NumberOfAttributesInRequest,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    tsZCL_WriteAttributeRecord  *pu16AttributeRequestList)
{

    return eZCL_SendWriteAttributesRequestInternal(
                    u8SourceEndPointId,
                    u8DestinationEndPointId,
                    u16ClusterId,
                    bDirectionIsServerToClient,
                    psDestinationAddress,
                    pu8TransactionSequenceNumber,
                    u8NumberOfAttributesInRequest,
                    bIsManufacturerSpecific,
                    u16ManufacturerCode,
                    pu16AttributeRequestList,
                    E_ZCL_WRITE_ATTRIBUTES_NO_RESPONSE);
}


#if defined __cplusplus
}
#endif

#endif  /* ZCL_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
