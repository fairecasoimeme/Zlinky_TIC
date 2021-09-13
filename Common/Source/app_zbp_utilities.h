/*****************************************************************************
 *
 * MODULE:             JN-AN-1220 ZLO Sensor Demo
 *
 * COMPONENT:          app_zpb_utilities.h
 *
 * DESCRIPTION:        Stack Development Utilities (Interface)
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
 * Copyright NXP B.V. 2016-2018. All rights reserved
 *
 ***************************************************************************/

#ifndef APP_ZBP_UTILITIES_H_
#define APP_ZBP_UTILITIES_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "zps_apl_af.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC void vClearDiscNT(void);
PUBLIC void vRemoveCoordParents(void);
PUBLIC void vDisplayDiscNT(void);
PUBLIC bool bInRoutingTable(uint16 u16ShortAddress);
PUBLIC void vDisplayRoutingTable(void);
PUBLIC void vDisplayTableSizes(void);
PUBLIC void vDisplayAddressMapTable(void);
PUBLIC void vDisplayNT(void);
PUBLIC void vClearNTEntry( uint64 u64AddressToRemove );
PUBLIC void vClearRoutingTable(void);
PUBLIC void vDisplayRouteRecordTable(void);
PUBLIC void vClearRouteRecordTable(void);PUBLIC void vDisplayAPSTable(void);
PUBLIC void vDisplayNWKTransmitTable(void);
PUBLIC void vDisplayStackEvent( ZPS_tsAfEvent sStackEvent );
PUBLIC void vStartStopTimer(uint8 u8Timer, uint32 u32Ticks,uint8* pu8State, uint8 eNextState);
PUBLIC void vEnablePermitJoin(uint8 u8Time);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        In line  Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#endif /*APP_ZBP_UTILITIES_H_*/
