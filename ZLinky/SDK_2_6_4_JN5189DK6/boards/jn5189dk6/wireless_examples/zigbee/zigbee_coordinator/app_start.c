/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include "EmbeddedTypes.h"
#include "app_main.h"
#include "zigbee_config.h"
#include "PDM.h"
#include "app_uart.h"
#include "app_coordinator.h"
#include "bdb_api.h"
#include "app_leds.h"
#ifdef ENABLE_SUBG_IF
#include "MDI_ReadFirmVer.h"
#include "MDI_Programmer.h"
#endif
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/


/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
static void APP_vInitialise(void);
#ifdef ENABLE_SUBG_IF
static void APP_vCheckMtgState(void);
#endif
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/


/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
extern uint8* ZPS_pu8AplZdoGetVsOUI(void);
/****************************************************************************
 *
 * NAME: vAppMain
 *
 * DESCRIPTION:
 * Entry point for application from a cold start.
 *
 * RETURNS:
 * Never returns.
 *
 ****************************************************************************/
void vAppMain(void)
{
	APP_vLedInitialise();
	APP_vInitResources();
    APP_vInitZigbeeResources();
    APP_vInitialise();
    BDB_vStart();
}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: APP_vInitialise
 *
 * DESCRIPTION:
 * Initialises Zigbee stack, hardware and application.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
static void APP_vInitialise(void)
{
    /* Initialise the Persistent Data Manager */
    PDM_eInitialise(1200, 63, NULL);

#ifdef ENABLE_SUBG_IF
    APP_vCheckMtgState();
#endif
    UART_vInit();
    UART_vRtsStartFlow();
    /* Initialise application */
    APP_vInitialiseCoordinator();
}

#ifdef ENABLE_SUBG_IF
/****************************************************************************
 *
 * NAME: APP_vCheckMtgState
 *
 * DESCRIPTION:
 * Initialises MTG comms
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void APP_vCheckMtgState(void)
{
	vMDI_RecoverDioSettings();
    vMDI_PrintIOConfig();

    /* Reset the Radio, don't care */
   	vMDI_Reset868MtGRadio();

   	/* Send SYNC request, wait for SYNC resp */
   	vMDI_SyncWithMtG();
}
#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
