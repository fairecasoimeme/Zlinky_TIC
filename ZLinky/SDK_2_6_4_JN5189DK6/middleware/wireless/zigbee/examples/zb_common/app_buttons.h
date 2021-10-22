/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef APP_BUTTONS_H
#define APP_BUTTONS_H

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <EmbeddedTypes.h>
#include "app.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Number of real GPIO lines */
#define APP_NUMBER_OF_GPIO 22
/* Dummy GPIO bit to use for internal NTAG FD line */
/* Use 22 as it has special purpose of waking from sleep when masks below are provided to PWRM */
#define APP_INTERNAL_NTAG_FD_BIT (22)
/* PIO pin to use for FD line */
/* 0xff = disabled, <=21 = GPIO, 22 = JN5189T Internal NTAG FD */
#define APP_BUTTONS_NFC_FD (APP_INTERNAL_NTAG_FD_BIT)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#define APP_BUTTONS_NUM_OM15082        (0UL)
#define APP_BUTTONS_DIO_MASK_OM15082   (0UL)

typedef enum {
		APP_E_BUTTONS_BUTTON_1 = 0,
		APP_E_BUTTONS_BUTTON_SW1,
		APP_E_BUTTONS_BUTTON_SW2,
		APP_E_BUTTONS_BUTTON_SW3,
		APP_E_BUTTONS_BUTTON_SW4,
	} APP_teButtons;

#define APP_BUTTONS_NUM                     (1UL + APP_BUTTONS_NUM_OM15082)
#define APP_BUTTONS_DIO_MASK                ((1 << APP_BOARD_SW0_PIN)|APP_BUTTONS_DIO_MASK_OM15082)
#define APP_BUTTONS_DIO_MASK_FOR_DEEP_SLEEP (APP_BUTTONS_DIO_MASK_OM15082)
#define APP_BUTTON_INVERT_MASK               0
#define DEMO_GINT0_POL_MASK                 ((APP_BUTTON_INVERT_MASK) & 0x3FFFFF)
#define DEMO_GINT0_ENA_MASK                 ((APP_BUTTONS_DIO_MASK)   & 0x3FFFFF)

typedef enum {
    E_INTERRUPT_UNKNOWN,
    E_INTERRUPT_BUTTON,
    E_INTERRUPT_WAKE_TIMER_EXPIRY
} teInterruptType;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
bool_t APP_bButtonInitialise(void);
void APP_cbTimerButtonScan(void *pvParam);
void APP_cbTimerButtonDelay(void *pvParam);
uint32_t APP_u32GetSwitchIOState(void);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#endif /*APP_BUTTONS_H*/
