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
 * MODULE:             Smart Energy Profile
 *
 * COMPONENT:          se.c
 *
 * DESCRIPTION:        ZigBee Smart Energy profile functions
 *
 ****************************************************************************/
 
/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include "zps_apl.h"
#include "zcl_heap.h"
#include "zcl.h"
#include "zcl_options.h"
#include "Scenes.h"
#include "Identify.h"
#include "LevelControl.h"
#include "OnOff.h"
#ifdef CLD_COLOUR_CONTROL
    #include "ColourControl.h"
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifndef ZCL_NUMBER_OF_APPLICATION_TIMERS
#define ZCL_NUMBER_OF_APPLICATION_TIMERS                 (0)
#endif

#if (defined CLD_IDENTIFY) && (defined IDENTIFY_SERVER)
#define ZCL_NUMBER_OF_IDENTIFY_TIMERS                    (1)
#else
#define ZCL_NUMBER_OF_IDENTIFY_TIMERS                    (0)
#endif

#if (defined CLD_SCENES) && (defined SCENES_SERVER)
#define ZCL_NUMBER_OF_SCENES_TIMERS                      (1)
#else
#define ZCL_NUMBER_OF_SCENES_TIMERS                      (0)
#endif

#ifdef CLD_OTA
#define ZCL_NUMBER_OF_OTA_TIMERS    (1)
#else
#define ZCL_NUMBER_OF_OTA_TIMERS    (0)
#endif

#define ZCL_NUMBER_OF_TIMERS     (ZCL_NUMBER_OF_APPLICATION_TIMERS) + \
								 (ZCL_NUMBER_OF_IDENTIFY_TIMERS) +\
								 (ZCL_NUMBER_OF_SCENES_TIMERS) +\
								 (ZCL_NUMBER_OF_OTA_TIMERS)
                                 
#ifdef ZCL_ATTRIBUTE_REPORTING_SERVER_SUPPORTED
    /*If Number of reports not defined by app define it to 10*/
    #ifndef ZCL_NUMBER_OF_REPORTS
        #define ZCL_NUMBER_OF_REPORTS            (10)
    #endif

    #ifndef ZCL_NUMBER_OF_STRING_REPORTS
        #define ZCL_NUMBER_OF_STRING_REPORTS     (0)
    #endif

    /*If System Min time is not defined by app define it to 1*/
    #ifndef ZCL_SYSTEM_MIN_REPORT_INTERVAL
        #define ZCL_SYSTEM_MIN_REPORT_INTERVAL   (0x0001)
    #endif
    
    /*If system max time  not defined by app define it to 61 secs*/
    #ifndef ZCL_SYSTEM_MAX_REPORT_INTERVAL
        #define ZCL_SYSTEM_MAX_REPORT_INTERVAL   (0x003d)
    #endif
#else
    #ifndef ZCL_NUMBER_OF_REPORTS
        #define ZCL_NUMBER_OF_REPORTS            (0)
    #endif
    
    #ifndef ZCL_NUMBER_OF_STRING_REPORTS
        #define ZCL_NUMBER_OF_STRING_REPORTS     (0)
    #endif
    
    #ifndef ZCL_SYSTEM_MIN_REPORT_INTERVAL
        #define ZCL_SYSTEM_MIN_REPORT_INTERVAL   (0)
    #endif
    
    #ifndef ZCL_SYSTEM_MAX_REPORT_INTERVAL    
        #define ZCL_SYSTEM_MAX_REPORT_INTERVAL   (0)
    #endif
#endif
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

PRIVATE uint32 u32ZCL_Heap[ZCL_HEAP_SIZE(ZCL_NUMBER_OF_ENDPOINTS, ZCL_NUMBER_OF_TIMERS, ZCL_NUMBER_OF_REPORTS, ZCL_NUMBER_OF_STRING_REPORTS)];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
#if (defined IDENTIFY_SERVER) && (defined CLD_IDENTIFY_10HZ_TICK)
    extern PUBLIC void vIdEffectTick(uint8 u8Endpoint);
#endif

/****************************************************************************
 *
 * NAME: eZCL_Initialise
 *
 * DESCRIPTION:
 * Registers timer servers with the ZCL layer
 *
 * PARAMETERS:  Name                            Usage
 *              cbCallBack                      Pointer to general callback
 *              hAPdu                           pointer to pool of APDU's
 *
 * RETURNS:
 * teZCL_Status
 *
 ****************************************************************************/
PUBLIC teZCL_Status eZCL_Initialise(tfpZCL_ZCLCallBackFunction cbCallBack,
        PDUM_thAPdu hAPdu) {
    teZCL_Status eStatus;
    tsZCL_Config sConfig;

    sConfig.u8NumberOfEndpoints = ZCL_NUMBER_OF_ENDPOINTS;
    sConfig.u8NumberOfTimers = ZCL_NUMBER_OF_TIMERS;
    sConfig.u8NumberOfReports = ZCL_NUMBER_OF_REPORTS;
    sConfig.u16SystemMinimumReportingInterval = ZCL_SYSTEM_MIN_REPORT_INTERVAL;
    sConfig.u16SystemMaximumReportingInterval = ZCL_SYSTEM_MAX_REPORT_INTERVAL;
    sConfig.pfZCLcallBackFunction = cbCallBack;
    sConfig.hAPdu = hAPdu;
    sConfig.pfZCLSignatureCreationFunction = NULL;
    sConfig.pfZCLSignatureVerificationFunction = NULL;
    sConfig.u32ZCL_HeapSizeInWords = sizeof(u32ZCL_Heap) / sizeof(uint32);
    sConfig.pu32ZCL_Heap = &u32ZCL_Heap[0];
    eStatus = eZCL_CreateZCL(&sConfig);
    return eStatus;
}

/****************************************************************************
 *
 * NAME: eZCL_Update100mS
 *
 * DESCRIPTION:
 * Should be called by an application timer 10 times per second
 *
 * PARAMETERS:  Name                            Usage
 *
 * RETURNS:
 * teZCL_Status
 *
 ****************************************************************************/
PUBLIC teZCL_Status eZCL_Update100mS(void)
{

    /* Update clusters on each end point if any */
    #if ((defined LEVEL_CONTROL_SERVER) || (defined SCENES_SERVER) || \
         (defined COLOUR_CONTROL_SERVER)|| (defined ONOFF_SERVER) || \
         (defined IDENTIFY_SERVER) )
        {
            uint8 u8NumEndpoints;
            u8NumEndpoints = u8ZCL_GetNumberOfEndpointsRegistered();

            int i;
            for (i = 1; i < (u8NumEndpoints + 1); i++)
            {
                #if (defined LEVEL_CONTROL_SERVER)
                    eCLD_LevelControlUpdate(i);
                #endif

                #if (defined SCENES_SERVER)
                    eCLD_ScenesUpdate(i);
                #endif

                #if (defined COLOUR_CONTROL_SERVER)
                    eCLD_ColourControlUpdate(i);
                #endif
 
                #if (defined ONOFF_SERVER) && (defined CLD_ONOFF_CMD_ON_WITH_TIMED_OFF)
                    eCLD_OnOffUpdate(i);
                #endif

                #if (defined IDENTIFY_SERVER) && (defined CLD_IDENTIFY_10HZ_TICK)
                    vIdEffectTick(i);
                #endif
            }
        }
    #endif
    return E_ZCL_SUCCESS;
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

