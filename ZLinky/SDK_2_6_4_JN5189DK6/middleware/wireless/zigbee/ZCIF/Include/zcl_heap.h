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
 * COMPONENT:          zcl_heap.h
 *
 * DESCRIPTION:        Manages the heap used to allocate memory to components. Simple design:
 * doesn't allow the memory to be returned.
 *
 ****************************************************************************/
#ifndef  ZCL_HEAP_H_INCLUDED
#define  ZCL_HEAP_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* ZCL_INTERNAL_TIMERS is 1 because the ZCL needs a timer of its own for reporting.
   u8NumberOfTimers in the heap size macro is the number of timers required outside of the ZCL.*/
#define ZCL_INTERNAL_TIMERS (1)

#ifndef ZCL_ATTRIBUTE_REPORT_STRING_MAXIMUM_SIZE
    #define ZCL_ATTRIBUTE_REPORT_STRING_MAXIMUM_SIZE        32
#endif    

typedef struct
{
    bool_t                    bRegistered;
    tsZCL_EndPointDefinition *psEndPointDefinition;
} tsZCL_EndPointRecord;

typedef struct {
    DNODE                       dllTimeNode;
    teZCL_TimerMode             eTimerMode;
    uint32                      u32UTCTime;
    tfpZCL_ZCLCallBackFunction  pfZCLCallBackFunction;
} tsZCL_TimerRecord;

typedef struct
{
    DNODE                       dllReportNode;
    tsZCL_EndPointDefinition    *psEndPointDefinition;
    tsZCL_ClusterInstance       *psClusterInstance;
    tsZCL_AttributeDefinition   *psAttributeDefinition;
    tuZCL_AttributeStorage       uAttributeStorage;
    uint32                       u32LastFiredUTCTime;
    tsZCL_AttributeReportingConfigurationRecord sAttributeReportingConfigurationRecord;
} tsZCL_ReportRecord;

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
// ZCL_INTERNAL_TIMERS is 1 because the ZCL now needs a timer of its own for reporting.  u8NumberOfTimers in the heap size macro is the number of timers required outside of the ZCL.
#define ZCL_HEAP_SIZE(u8NumberOfEndpoints, u8NumberOfTimers, u8NumberOfReports , u8NumberOfStringReports) (ENDPOINT_SIZE_WORDS(u8NumberOfEndpoints) + TIMER_SIZE_WORDS((u8NumberOfTimers+ZCL_INTERNAL_TIMERS)) + REPORT_SIZE_WORDS(u8NumberOfReports) + STRING_REPORT_SIZE_WORDS(u8NumberOfStringReports))

#define ENDPOINT_SIZE(u8NumberOfEndpoints)                ((sizeof(tsZCL_EndPointRecord)+ 4) * u8NumberOfEndpoints)
#define ENDPOINT_SIZE_WORDS(u8NumberOfEndpoints)          ((ENDPOINT_SIZE(u8NumberOfEndpoints) + 4)/4)

#define TIMER_SIZE(u8NumberOfTimers)                      ((sizeof(tsZCL_TimerRecord)+ 4) * u8NumberOfTimers)
#define TIMER_SIZE_WORDS(u8NumberOfTimers)                ((TIMER_SIZE(u8NumberOfTimers) + 4)/4)

#define REPORT_SIZE(u8NumberOfReports)                    ((sizeof(tsZCL_ReportRecord)+ 4) * u8NumberOfReports)
#define REPORT_SIZE_WORDS(u8NumberOfReports)              ((REPORT_SIZE(u8NumberOfReports) + 4)/4)

#define STRING_REPORT_SIZE(u8NumberOfStringReports)       ((ZCL_ATTRIBUTE_REPORT_STRING_MAXIMUM_SIZE + 4) * u8NumberOfStringReports)
#define STRING_REPORT_SIZE_WORDS(u8NumberOfStringReports) ((STRING_REPORT_SIZE(u8NumberOfStringReports) + 4)/4)
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Imported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* ZCL_HEAP_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
