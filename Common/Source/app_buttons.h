/*****************************************************************************
 *
 * MODULE:             JN-AN-1243
 *
 * COMPONENT:          app_buttons.h
 *
 * DESCRIPTION:        DR1199/DR1175 Button Press detection (Interface)
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
 * Copyright NXP B.V. 2017-2019. All rights reserved
 *
 ***************************************************************************/
#ifndef APP_BUTTONS_H
#define APP_BUTTONS_H

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
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
#if (defined BUTTON_MAP_OM15082)
    #define APP_BUTTONS_NUM_OM15082        (4UL)
    #define APP_BUTTONS_DIO_MASK_OM15082   ((1<<APP_BOARD_SW1_PIN)|(1<<APP_BOARD_SW2_PIN)|(1<<APP_BOARD_SW3_PIN)|(1<<APP_BOARD_SW4_PIN))
#else
    #define APP_BUTTONS_NUM_OM15082        (0UL)
    #define APP_BUTTONS_DIO_MASK_OM15082   (0UL)
#endif

#if ((defined APP_NTAG_NWK) && (APP_BUTTONS_NFC_FD != (0xff)))
    #define APP_BUTTONS_NUM_NTAG_FD        (1UL)
    #define APP_BUTTONS_DIO_NTAG_FD        (1<<APP_BUTTONS_NFC_FD)
#else
    #define APP_BUTTONS_NUM_NTAG_FD        (0UL)
    #define APP_BUTTONS_DIO_NTAG_FD        (0UL)
#endif

#if (defined DUAL_BOOT)
    #define APP_BUTTONS_NUM_DUAL_BOOT      (1UL)
    #define APP_BUTTONS_DIO_MASK_DUAL_BOOT (1<<APP_BOARD_SWISP_PIN)
#else
    #define APP_BUTTONS_NUM_DUAL_BOOT      (0UL)
    #define APP_BUTTONS_DIO_MASK_DUAL_BOOT (0UL)
#endif

typedef enum {
    APP_E_BUTTONS_BUTTON_1 = 0
#if (defined BUTTON_MAP_OM15082)||(defined OM5578)||(defined DONGLE) /* Used as pseudo buttons on OM5578 */
    , APP_E_BUTTONS_BUTTON_SW1
    , APP_E_BUTTONS_BUTTON_SW2
    , APP_E_BUTTONS_BUTTON_SW3
    , APP_E_BUTTONS_BUTTON_SW4
#endif
#if ((defined APP_NTAG_NWK) && (APP_BUTTONS_NFC_FD != (0xff)))
    , APP_E_BUTTONS_NFC_FD
#endif
#if (defined DUAL_BOOT)
    , APP_E_BUTTONS_SWISP
#endif
} APP_teButtons;

#define APP_BUTTONS_NUM                     (1UL + APP_BUTTONS_NUM_OM15082 + APP_BUTTONS_NUM_NTAG_FD + APP_BUTTONS_NUM_DUAL_BOOT)
#define APP_BUTTONS_DIO_MASK                ((1 << APP_BOARD_SW0_PIN)|APP_BUTTONS_DIO_MASK_OM15082|APP_BUTTONS_DIO_NTAG_FD|APP_BUTTONS_DIO_MASK_DUAL_BOOT)
#define APP_BUTTONS_DIO_MASK_FOR_DEEP_SLEEP (APP_BUTTONS_DIO_MASK_OM15082|APP_BUTTONS_DIO_NTAG_FD)
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
PUBLIC bool_t APP_bButtonInitialise(void);
PUBLIC void APP_cbTimerButtonScan(void *pvParam);
PUBLIC void APP_cbTimerButtonDelay(void *pvParam);
PUBLIC uint32 APP_u32GetSwitchIOState(void);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#endif /*APP_BUTTONS_H*/
