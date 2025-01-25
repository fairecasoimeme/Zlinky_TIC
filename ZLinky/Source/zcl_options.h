/*****************************************************************************
 *
 * MODULE:             ZCL Options - Router
 *
 * COMPONENT:          zcl_options.h
 *
 * DESCRIPTION:        Options Header for ZigBee Cluster Library functions
 *                     [Base Device Template Application Router]
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
 * Copyright NXP B.V. 2016-2019. All rights reserved
 *
 ***************************************************************************/

#ifndef ZCL_OPTIONS_H
#define ZCL_OPTIONS_H

#include <jendefs.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/*                      ZCL Specific initialization                         */
/****************************************************************************/
/* This is the NXP manufacturer code.If creating new a manufacturer         */
/* specific command apply to the Zigbee alliance for an Id for your company */
/* Also update the manufacturer code in .zpscfg: Node Descriptor->misc      */
#define ZCL_MANUFACTURER_CODE                                0x1037

/* Number of endpoints supported by this device */
#define ZCL_NUMBER_OF_ENDPOINTS                              1

/* For GP proxy cluster*/
#define ZCL_NUMBER_OF_APPLICATION_TIMERS     1

/* ZCL has all cooperative task */
#define COOPERATIVE

/* Set this Tue to disable non error default responses from clusters */
#define ZCL_DISABLE_DEFAULT_RESPONSES       (TRUE)
#define ZCL_DISABLE_APS_ACK                 (TRUE)


/* Which Custom commands needs to be supported */
#define ZCL_ATTRIBUTE_READ_SERVER_SUPPORTED
#define ZCL_ATTRIBUTE_WRITE_SERVER_SUPPORTED

#define ZCL_ATTRIBUTE_REPORTING_SERVER_SUPPORTED
#define ZCL_CONFIGURE_ATTRIBUTE_REPORTING_SERVER_SUPPORTED
#define ZCL_READ_ATTRIBUTE_REPORTING_CONFIGURATION_SERVER_SUPPORTED

/* Reporting related configuration */
enum
{
 REPORT_ONOFF_SLOT = 0,
 NUMBER_OF_REPORTS
};

#define ZCL_NUMBER_OF_REPORTS     50
#define MIN_REPORT_INTERVAL       1
#define MAX_REPORT_INTERVAL       0x14
#define ZCL_SYSTEM_MAX_REPORT_INTERVAL 0x0014

/* Enable wild card profile */
#define ZCL_ALLOW_WILD_CARD_PROFILE

/****************************************************************************/
/*                             Enable Cluster                               */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to enable         */
/* cluster and their client or server instances                             */
/****************************************************************************/
#define CLD_BASIC
#define BASIC_SERVER

#define CLD_IDENTIFY
#define IDENTIFY_SERVER

#define CLD_SIMPLE_METERING
#define SM_SERVER

#define CLD_ELECTRICAL_MEASUREMENT
#define ELECTRICAL_MEASUREMENT_SERVER

#define CLD_METER_IDENTIFICATION
#define METER_IDENTIFICATION_SERVER

#define CLD_GROUPS
#define GROUPS_SERVER

#define CLD_POWER_CONFIGURATION
#define POWER_CONFIGURATION_SERVER

#define CLD_TIME
#define TIME_CLIENT

#define CLD_TUYASPECIFIC

//#ifdef BUILD_OTA
#define CLD_OTA
#define OTA_CLIENT
#define OTA_CLD_ATTR_CURRENT_FILE_VERSION
//#endif

/****************************************************************************/
/*             Basic Cluster - Optional Attributes                          */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the basic cluster.                                         */
/****************************************************************************/
#define   CLD_BAS_ATTR_APPLICATION_VERSION
#define   CLD_BAS_ATTR_STACK_VERSION
#define   CLD_BAS_ATTR_HARDWARE_VERSION
#define   CLD_BAS_ATTR_MANUFACTURER_NAME
#define   CLD_BAS_ATTR_MODEL_IDENTIFIER
#define   CLD_BAS_ATTR_DATE_CODE
#define   CLD_BAS_ATTR_SW_BUILD_ID
#define   CLD_BAS_ATTR_GENERIC_DEVICE_CLASS
#define   CLD_BAS_ATTR_GENERIC_DEVICE_TYPE
#define   CLD_BAS_ATTR_PRODUCT_CODE
#define   CLD_BAS_ATTR_PRODUCT_URL

/* Tuya */
#define CLD_BAS_ATTR_TUYA_ID

#define CLD_BAS_APP_VERSION         (1)
#define CLD_BAS_STACK_VERSION       (2)
#define CLD_BAS_HARDWARE_VERSION    (1)
#define CLD_BAS_MANUF_NAME_SIZE     (16) //(5)
#define CLD_BAS_MODEL_ID_SIZE       (6) //(6)
#define CLD_BAS_DATE_SIZE           (8)
#define CLD_BAS_POWER_SOURCE        E_CLD_BAS_PS_SINGLE_PHASE_MAINS
#define CLD_BAS_SW_BUILD_SIZE       (9)
#define CLD_BAS_URL_SIZE            (12)
#define CLD_BAS_PCODE_SIZE          (4)

/****************************************************************************/
/*             Identify - Optional Attributes                               */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the identify cluster.                                      */
/****************************************************************************/
#define CLD_IDENTIFY_CMD_TRIGGER_EFFECT
#define CLD_IDENTIFY_10HZ_TICK

/****************************************************************************/
/*             Groups Cluster - Optional Attributes                         */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the scenes cluster.                                        */
/****************************************************************************/
#define CLD_GROUPS_MAX_NUMBER_OF_GROUPS                     16
#define CLD_GROUPS_DISABLE_NAME_SUPPORT

/****************************************************************************/
/*             OnOff Control Cluster - Optional Attributes                 */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the time cluster.                                          */
/****************************************************************************/
/****************************************************************************/
/*             Green Power Cluster - Optional Attributes                 */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the Green Power cluster.                                          */
/****************************************************************************/
#ifdef  CLD_GREENPOWER
    #define GP_IEEE_ADDR_SUPPORT
    #define GREENPOWER_END_POINT_ID                 2
    #define GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES  5
    #define GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES       5
    #define CLD_GP_ATTR_ZGPS_COMMISSIONING_WINDOW
    #define CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE
    #define CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY
    #define CLD_GP_ATTR_ZGP_LINK_KEY
    /* security level */
    #define GP_SECURITY_LEVEL E_GP_ENC_FULL_FC_FULL_MIC
    /* Possible values are  E_GP_NO_KEY, E_GP_ZIGBEE_NWK_KEY, E_GP_ZGPD_GROUP_KEY,
        E_GP_NWK_KEY_DERIVED_ZGPD_GROUP_KEY, E_GP_OUT_OF_THE_BOX_ZGPD_KEY, E_GP_DERIVED_INDIVIDUAL_ZGPD_KEY*/
    #define GP_KEYTPE              E_GP_ZGPD_GROUP_KEY
    #define GP_SHARED_KEY  { 0xC0 , 0xC1 , 0xC2 , 0xC3 , 0xC4 , 0xC5 , 0xC6 , 0xC7 , 0xC8 , 0xC9 , 0xCa , 0xCb , 0xCc , 0xCd , 0xCe , 0xCf}
#undef ZCL_NUMBER_OF_ENDPOINTS
#define ZCL_NUMBER_OF_ENDPOINTS                             2
#undef CLD_GROUPS_MAX_NUMBER_OF_GROUPS
#define CLD_GROUPS_MAX_NUMBER_OF_GROUPS                     4
#define GP_MAX_TRANS_ZB_CMD_PAYLOAD_LENGTH                   2
#endif

/****************************************************************************/
/*             OTA Cluster - Optional Attributes                 */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the OTA cluster.                                          */
/****************************************************************************/
#ifdef  CLD_OTA
    #define OTA_DEMO_TIMINGS                        // define this fior the fast timings for edemo purposes
    #define OTA_ACKS_ON                             FALSE
    #define OTA_MAX_CO_PROCESSOR_IMAGES             0
    #define OTA_CLD_ATTR_CURRENT_FILE_VERSION
    #define OTA_MAX_BLOCK_SIZE                      48      // in multiples of 16 (internal flash requirement)
#ifdef OTA_DEMO_TIMINGS
    #define OTA_TIME_INTERVAL_BETWEEN_RETRIES       5       // Valid only if OTA_TIME_INTERVAL_BETWEEN_REQUESTS not defined
    #define CLD_OTA_MAX_BLOCK_PAGE_REQ_RETRIES      10      // count of block reqest failure befiore abandoning download
#else
    #define OTA_TIME_INTERVAL_BETWEEN_REQUESTS      RND_u32GetRand(10,20)
    #define OTA_TIME_INTERVAL_BETWEEN_RETRIES       RND_u32GetRand(10,20)      // Valid only if OTA_TIME_INTERVAL_BETWEEN_REQUESTS not defined
    #define CLD_OTA_MAX_BLOCK_PAGE_REQ_RETRIES      240                        // count of block reqest failure befiore abandoning download
#endif

    #define OTA_STRING_COMPARE
    #define OTA_MAX_IMAGES_PER_ENDPOINT             1
#endif

/****************************************************************************/
/*             Basic Cluster - Optional Commands                            */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* commands to the basic cluster.                                           */
/****************************************************************************/
#define CLD_BAS_CMD_RESET_TO_FACTORY_DEFAULTS

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* ZCL_OPTIONS_H */
