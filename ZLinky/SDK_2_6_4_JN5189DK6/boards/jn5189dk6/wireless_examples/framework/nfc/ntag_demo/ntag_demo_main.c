/*
* Copyright 2020 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

/* Framework include */
#include "fsl_os_abstraction.h"
#include "board.h"
#include "Panic.h"
#include "fsl_debug_console.h"
#include "MemManager.h"
#include "Messaging.h"
#include "PWR_Interface.h"
#include "PWR_Configuration.h"

#include "EmbeddedTypes.h"

#include "app_ntag.h"

/************************************************************************************
*************************************************************************************
* Local macros
*************************************************************************************
************************************************************************************/
#define gIdleTaskPriority 8
#define gIdleTaskStackSize 1024
#define gAppEvntMsgFromNtag  1
/************************************************************************************
*************************************************************************************
* Private definitions
*************************************************************************************
************************************************************************************/
static uint8_t platformInitialized = 0;
static osaEventId_t  mAppEvent;
static anchor_t mNtagInputQueue;
/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static void Idle_Task(osaTaskParam_t param);
OSA_TASK_DEFINE( Idle_Task, gIdleTaskPriority, 1, gIdleTaskStackSize, FALSE );
static void App_Thread (uint32_t param);

#if defined(cPWR_UsePowerDownMode) && (cPWR_UsePowerDownMode)
static void app_preSleepCallBack(void);
static void app_wakeupCallBack(void);
#endif
/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  This is the first task created by the OS. This task will initialize
*         the system
*
* \param[in]  param
*
********************************************************************************** */
void main_task(uint32_t param)
{
    if (!platformInitialized)
    {
        platformInitialized = 1;
        MEM_Init();
        /* Create application event */
        mAppEvent = OSA_EventCreate(TRUE);
        if( NULL == mAppEvent )
        {
            panic(0,0,0,0);
            return;
        }
        MSG_InitQueue(&mNtagInputQueue);
        /* Create the idle task */
        if (OSA_TaskCreate(OSA_TASK(Idle_Task), NULL) == NULL)
        {
            panic( 0, (uint32_t)Idle_Task, 0, 0 );
        }

        if (!app_ntag_init())
        {
            PRINTF("NTAG not supported\n");
            panic( 0, (uint32_t)app_ntag_init, 0, 0 );
        }
        PRINTF("===> APP STARTING\r\n");
#if defined(cPWR_UsePowerDownMode) && (cPWR_UsePowerDownMode)
        PWR_Init();
        (void)PWR_ChangeDeepSleepMode(cPWR_DeepSleepMode);
        PWR_RegisterLowPowerExitCallback(app_wakeupCallBack);
        PWR_RegisterLowPowerEnterCallback(app_preSleepCallBack);
        PWR_vWakeUpConfig(PWR_NTAG_FD_WAKEUP);
        //PWR_DisallowDeviceToSleep();
#endif
    }

    /* Call application task */
    App_Thread( param );
}

void app_queue_ntag_msg(sAppNtagMsg_t *pMsg)
{
    (void)MSG_Queue(&mNtagInputQueue, pMsg);
    /* Signal app Ntag event */
    (void)OSA_EventSet(mAppEvent, gAppEvntMsgFromNtag);
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  This function represents the Application task. 
*         This task reuses the stack allocated for the MainThread.
*         This function is called to process all events for the task. Events 
*         include timers, messages and any other user defined events.
* \param[in]  argument
*
********************************************************************************** */
static void App_Thread (uint32_t param)
{
    while(1)
    {
        osaEventFlags_t event = 0U;
        (void)OSA_EventWait(mAppEvent, osaEventFlagsAll_c, FALSE, osaWaitForever_c , &event);
        if (MSG_Pending(&mNtagInputQueue))
        {
            sAppNtagMsg_t *pMsgIn = MSG_DeQueue(&mNtagInputQueue);
            if (pMsgIn != NULL)
            {
                pMsgIn->processMsg(pMsgIn->pMsgParm);
                (void)MEM_BufferFree(pMsgIn);
            }
        }
        if (MSG_Pending(&mNtagInputQueue))
            (void)OSA_EventSet(mAppEvent, gAppEvntMsgFromNtag);
        /* For BareMetal break the while(1) after 1 run */
        if( gUseRtos_c == 0 )
        {
            break;
        }
    }
}

static void Idle_Task(osaTaskParam_t param)
{
     while(1)
    {
        if (PWR_CheckIfDeviceCanGoToSleep())
        {
            PWR_EnterLowPower();
        }

        app_ntag_runStateMachine();
        
        /* For BareMetal break the while(1) after 1 run */
        if( gUseRtos_c == 0 )
        {
            break;
        }
    }   
}

#if defined(cPWR_UsePowerDownMode) && (cPWR_UsePowerDownMode)
static void app_preSleepCallBack(void)
{
    PRINTF("Sleeping\n");
    /* Deinitialize debug console */
    BOARD_DeinitDebugConsole();
    /* configure pins for power down mode */
    BOARD_SetPinsForPowerMode();
}

static void app_wakeupCallBack(void)
{
    PRINTF("WakeUp\n");
    /* Get the wakeup cause */
    if (PMC->WAKEIOCAUSE & PWR_NTAG_FD_WAKEUP)
    {
        PRINTF("Wake-Up from Field detect\n");
    }
    NVIC_EnableIRQ(NFCTag_IRQn);
}
#endif
