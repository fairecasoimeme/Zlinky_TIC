/****************************************************************************
 *
 * MODULE:             Dynamic switching support
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5148, JN5142, JN5139].
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
 * Copyright NXP B.V. 2020. All rights reserved
 *
 ***************************************************************************/

#ifndef _mac_dynamic_h_
#define _mac_dynamic_h_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "jendefs.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef enum
{
    E_DYN_SLAVE,            /* 15.4 */
    E_DYN_MASTER,           /* BLE */
    E_DYN_PROTOCOL_COUNT
} teDynProtocol;

typedef enum
{
    E_DYN_OK,
    E_DYN_REFUSED
} teDynStatus;

typedef enum
{
    E_DYN_STATE_OFF,            /* disabled */
    E_DYN_STATE_INACTIVE,       /* waiting for radio ownership */
    E_DYN_STATE_PAUSING,        /* interrupted by switch timer */
    E_DYN_STATE_ACTIVE          /* current radio owner */
} teDynState;

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern PUBLIC teDynState aeDynState[E_DYN_PROTOCOL_COUNT];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/* To be called as last function in pre sleep callback */
extern PUBLIC void vDynStopAll(void);

/* To be called as last function in wake up callback if BLE expects radio ownership */
extern PUBLIC void vDynForceBLE(void);

extern PUBLIC void vDynRequestState(teDynProtocol eProtocol, teDynState eStateRequest);

extern PUBLIC teDynState eDynGetProtocolState(teDynProtocol eProtocol);
extern PUBLIC void vMac_DynRadioAvailable(uint32 u32TimeAvailable_us);
extern PUBLIC teDynStatus eMac_DynActivityAdded(uint32 u32TimeLeft_us);

#ifdef __cplusplus
};
#endif

#endif /* _mac_dynamic_h_ */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
