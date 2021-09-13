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
#include "board.h"
#include "Panic.h"
#include "fsl_os_abstraction.h"
#include "fsl_debug_console.h"
#include "MemManager.h"
#include "Messaging.h"

#include "app_nci.h"


/************************************************************************************
*************************************************************************************
* Local macros
*************************************************************************************
************************************************************************************/
#define gIdleTaskPriority 8
#define gIdleTaskStackSize 1024
#define gAppEvntMsgFromNci  1
/************************************************************************************
*************************************************************************************
* Private definitions
*************************************************************************************
************************************************************************************/
static uint8_t platformInitialized = 0;
static osaEventId_t  mAppEvent;
static anchor_t mNciInputQueue;

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static void Idle_Task(osaTaskParam_t param);
OSA_TASK_DEFINE( Idle_Task, gIdleTaskPriority, 1, gIdleTaskStackSize, FALSE );
static void App_Thread (uint32_t param);

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
        MSG_InitQueue(&mNciInputQueue);
        /* Create the idle task */
        if (OSA_TaskCreate(OSA_TASK(Idle_Task), NULL) == NULL)
        {
            panic( 0, (uint32_t)Idle_Task, 0, 0 );
        }
        app_nci_init();
        PRINTF("STARTING\n");
    }

    /* Call application task */
    App_Thread( param );
}

void app_queue_nci_msg(sAppNciMsg_t *pMsg)
{
    (void)MSG_Queue(&mNciInputQueue, pMsg);
    /* Signal app Nci event */
    (void)OSA_EventSet(mAppEvent, gAppEvntMsgFromNci);
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
        if (MSG_Pending(&mNciInputQueue))
        {
            sAppNciMsg_t *pMsgIn = MSG_DeQueue(&mNciInputQueue);
            if (pMsgIn != NULL)
            {
                pMsgIn->processMsg(pMsgIn->pMsgParm);
                (void)MEM_BufferFree(pMsgIn);
            }
        }
        if (MSG_Pending(&mNciInputQueue))
            (void)OSA_EventSet(mAppEvent, gAppEvntMsgFromNci);
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
        app_nci_runStateMachine();
        
        /* For BareMetal break the while(1) after 1 run */
        if( gUseRtos_c == 0 )
        {
            break;
        }
    }   
}

