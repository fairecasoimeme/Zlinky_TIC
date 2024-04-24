/*****************************************************************************
 *
 * MODULE:             JN-AN-1243
 *
 * COMPONENT:          app_main.h
 *
 * DESCRIPTION:
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

#ifndef APP_MAIN_H
#define APP_MAIN_H

#include "ZQueue.h"
#include "tsv_pub.h"
#include "bdb_api.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC void APP_vInitResources(void);
PUBLIC void APP_vMainLoop(void);
PUBLIC void APP_vSetUpHardware(void);
PUBLIC void APP_vRadioTempUpdate(bool_t bLoadCalibration);

extern PUBLIC void APP_cbTimerPowerCount( void *pvParam);
extern PUBLIC void APP_cbTimerGPZclTick(void *pvParam);


/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern PUBLIC uint8 u8TimerButtonScan;
extern PUBLIC uint8 u8TimerZCL;
extern PUBLIC uint8 u8TimerLinky;
extern PUBLIC uint8 u8TimerBlink;
extern PUBLIC uint8 u8TimerLedON;
extern PUBLIC uint8 u8TimerAwake;


#if (defined APP_NTAG_NWK)
extern PUBLIC uint8 u8TimerNtag;
#endif

extern PUBLIC uint8 u8TimerPowerOn;

extern PUBLIC tszQueue zps_msgMlmeDcfmInd;
extern PUBLIC tszQueue zps_msgMcpsDcfmInd;
extern PUBLIC tszQueue zps_msgMcpsDcfm;
extern PUBLIC tszQueue zps_TimeEvents;

//extern PUBLIC tszQueue APP_msgZpsEvents;
extern PUBLIC tszQueue APP_msgBdbEvents;
extern PUBLIC tszQueue APP_msgAppEvents;

extern PUBLIC uint8 u8ModeLinky;
extern PUBLIC uint8 u8OldStatusLinky;
extern PUBLIC zuint48 u48StartTuyaTotalConsumption;
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
extern PUBLIC uint8 u8TimerPowerOn;

#ifdef CLD_GREENPOWER
extern PUBLIC uint8 u8GPTimerTick;
#endif

#ifdef CLD_GREENPOWER
extern PUBLIC tszQueue APP_msgGPZCLTimerEvents;
extern uint8 au8GPZCLEvent[];
extern uint8 u8GPZCLTimerEvent;
#endif

#endif /* APP_MAIN_H */






