/****************************************************************************
 *
 * MODULE:             MAC PIB
 *
 * DESCRIPTION:
 * Definitions used for accessing the PIB by higher layers
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
 * Copyright NXP B.V. 2013. All rights reserved
 *
 ***************************************************************************/

#ifndef _minimac_nativeshim_h_
#define _minimac_nativeshim_h_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
#include "MMAC.h"
#include "MiniMac.h"
#include "MiniMac_Pib.h"
#include "mac_sap.h"
#include "mac_pib.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define bAppApi_CurrentlyScanning       bMiniMac_CurrentlyScanning
#define vAppApiSaveMacSettings          vMiniMac_SaveSettings
#define pvAppApiGetMacHandle(A)         NULL
#define pvAppApiGetMacAddrLocation      pvAppApiDefGetMacAddrLocation

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
/* High Power Module Enable */
#define APP_API_MODULE_STD   (0)
#define APP_API_MODULE_HPM05 (1)
#define APP_API_MODULE_HPM06 (2)
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC void *pvAppApiGetMacAddrLocation(void);

#ifdef __cplusplus
};
#endif

#endif /* _minimac_nativeshim_h_ */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
