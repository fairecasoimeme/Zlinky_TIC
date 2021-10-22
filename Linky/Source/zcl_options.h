/*****************************************************************************
 *
 * MODULE:             JN-AN-1220 ZLO Sensor Demo
 *
 * COMPONENT:          zcl_options.h
 *
 * DESCRIPTION:        ZCL Options Header for ZLO Light Sensor
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

#ifndef ZCL_OPTIONS_H
#define ZCL_OPTIONS_H

#include <jendefs.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/*                      ZCL Specific initialization                         */
/****************************************************************************/
/* Disable APS acks */
#define ZCL_DISABLE_APS_ACK                                 1

/* This is the NXP manufacturer code. If creating new a manufacturer        */
/* specific command apply to the Zigbee alliance for an Id for your company */
/* Also update the manufacturer code in .zpscfg: Node Descriptor->misc      */
#define ZCL_MANUFACTURER_CODE                                0x1037

/*  Enabling various server and clients for command which act across entire  */
/*  profile                                                                  */
#define ZCL_ATTRIBUTE_READ_SERVER_SUPPORTED
#define ZCL_ATTRIBUTE_READ_CLIENT_SUPPORTED
#define ZCL_ATTRIBUTE_WRITE_SERVER_SUPPORTED
#define ZCL_ATTRIBUTE_REPORTING_SERVER_SUPPORTED
#define ZCL_CONFIGURE_ATTRIBUTE_REPORTING_SERVER_SUPPORTED
#define ZCL_READ_ATTRIBUTE_REPORTING_CONFIGURATION_SERVER_SUPPORTED


/* Only one reportable attribute : MeasuredValue */
#define ZCL_NUMBER_OF_REPORTS     20
#define ZLO_MIN_REPORT_INTERVAL   0x1e
#define ZLO_MAX_REPORT_INTERVAL   0x3d//0x3d

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

//#define CLD_SCENES
//#define SCENES_CLIENT

#define CLD_IDENTIFY
#define IDENTIFY_CLIENT
#define IDENTIFY_SERVER

#define CLD_ALARMS
#define ALARMS_SERVER

//#define CLD_GROUPS
//#define GROUPS_CLIENT

#define CLD_SIMPLE_METERING
#define SM_SERVER

#define CLD_ELECTRICAL_MEASUREMENT
#define ELECTRICAL_MEASUREMENT_SERVER

#define CLD_METER_IDENTIFICATION
#define METER_IDENTIFICATION_SERVER

#ifdef BUILD_OTA
#define CLD_OTA
#define OTA_CLIENT
#endif
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

#define CLD_BAS_APP_VERSION         (1)
#define CLD_BAS_STACK_VERSION       (2)
#define CLD_BAS_HARDWARE_VERSION    (1)
#define CLD_BAS_MANUF_NAME_SIZE     (5)
#define CLD_BAS_MODEL_ID_SIZE       (11)
#define CLD_BAS_DATE_SIZE           (8)
#define CLD_BAS_POWER_SOURCE        E_CLD_BAS_PS_BATTERY
#define CLD_BAS_SW_BUILD_SIZE       (9)
#define CLD_BAS_URL_SIZE            (12)
#define CLD_BAS_PCODE_SIZE          (4)


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
    #define CLD_OTA_MAX_BLOCK_PAGE_REQ_RETRIES      5      // count of block reqest failure befiore abandoning download
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
/*             Identify Cluster - Optional Commands                         */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* commands to the identify cluster.                                        */
/****************************************************************************/
#define   CLD_IDENTIFY_CMD_TRIGGER_EFFECT
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
