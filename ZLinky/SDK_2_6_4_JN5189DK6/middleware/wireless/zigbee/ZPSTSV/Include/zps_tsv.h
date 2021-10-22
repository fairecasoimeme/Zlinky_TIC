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
 * MODULE:      ZPSTSV
 *
 * COMPONENT:   zps_tsv.h
 *
 * DESCRIPTION:
 *
 *****************************************************************************/

#ifndef ZPS_TSV_H_
#define ZPS_TSV_H_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"
#include "tsv_pub.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define ZPS_TSV_TIME_SEC(v) ((uint32)(v) * 62500UL)
#define ZPS_TSV_TIME_MSEC(v) ((uint32)(v) * 62UL)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef TSV_ResultCode_e ZPS_teTsvResultCode;
typedef void (*ZPS_prTsvExpireCb)(void *, void *);

typedef struct
{
    ZPS_prTsvExpireCb prCallback;
    ZPS_prTsvExpireCb pvContext;
    ZPS_prTsvExpireCb pvParam;
} ZPS_tsTsvTimerInfo;

typedef struct
{
    TSV_Timer_s sTsvTimer;
    ZPS_tsTsvTimerInfo sTimerInfo;
} ZPS_tsTsvTimer;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC uint32 zps_u32GetTsvVersion(void);
void ZPS_vTsvInit(void (*prExpireFn)(ZPS_tsTsvTimerInfo *));

PUBLIC ZPS_teTsvResultCode ZPS_eTsvOpen(
    ZPS_tsTsvTimer *psTimer,
    ZPS_prTsvExpireCb prExpireCb,
    void *pvContext,
    void *pvParam);

PUBLIC ZPS_teTsvResultCode ZPS_eTsvClose(
    ZPS_tsTsvTimer* psTimer,
    bool_t bInvokeCBIfRunning);

PUBLIC ZPS_teTsvResultCode ZPS_eTsvStart(
    ZPS_tsTsvTimer* psTimer,
    uint32 u32Value);

PUBLIC ZPS_teTsvResultCode ZPS_eTsvStop(ZPS_tsTsvTimer* psTimer);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#ifdef __cplusplus
};
#endif

#endif /* ZPS_TSV_H_ */
