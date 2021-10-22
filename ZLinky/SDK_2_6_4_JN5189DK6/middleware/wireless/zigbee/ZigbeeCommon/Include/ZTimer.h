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
 * MODULE:             ZTimer
 *
 * COMPONENT:          ZTimer.h
 *
 * DESCRIPTION:        Zigbee Timer Module
 *
 ****************************************************************************/

#ifndef ZTIMER_H_
#define ZTIMER_H_

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define ZTIMER_TIME_SEC(v) ((uint32)(v) * 1000UL)
#define ZTIMER_TIME_MSEC(v) ((uint32)(v) * 1UL)

/* Flags for timer configuration */
#define ZTIMER_FLAG_ALLOW_SLEEP     0
#define ZTIMER_FLAG_PREVENT_SLEEP   (1 << 0)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef enum
{
    E_ZTIMER_STATE_CLOSED,
    E_ZTIMER_STATE_STOPPED,
    E_ZTIMER_STATE_RUNNING,
    E_ZTIMER_STATE_EXPIRED,    
} ZTIMER_teState;

typedef void (*ZTIMER_tpfCallback)(void *pvParam);

typedef struct
{
    uint8               u8Flags;
    ZTIMER_teState      eState;
    uint32                u32Time;
    void                *pvParameters;
    ZTIMER_tpfCallback    pfCallback;
} ZTIMER_tsTimer;

typedef enum
{
    E_ZTIMER_OK,
    E_ZTIMER_FAIL
} ZTIMER_teStatus;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC ZTIMER_teStatus ZTIMER_eInit(ZTIMER_tsTimer *psTimers, uint8 u8NumTimers);
#if (JENNIC_CHIP_FAMILY == JN516x)
PUBLIC void ISR_vTickTimer(void);
#else
PUBLIC void ZTIMER_vAhiCallback ( uint32 u32Device, uint32 u32ItemBitmap);
#endif
PUBLIC void ZTIMER_vSleep(void);
PUBLIC void ZTIMER_vWake(void);
PUBLIC void ZTIMER_vTask(void);
PUBLIC ZTIMER_teStatus ZTIMER_eOpen(uint8 *pu8TimerIndex, ZTIMER_tpfCallback pfCallback, void *pvParams, uint8 u8Flags);
PUBLIC ZTIMER_teStatus ZTIMER_eClose(uint8 u8TimerIndex);
PUBLIC ZTIMER_teStatus ZTIMER_eStart(uint8 u8TimerIndex, uint32 u32Time);
PUBLIC ZTIMER_teStatus ZTIMER_eStop(uint8 u8TimerIndex);
PUBLIC ZTIMER_teState ZTIMER_eGetState(uint8 u8TimerIndex);
PUBLIC void ZTIMER_vStopAllTimers(void);
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#endif /*ZTIMER_H_*/
