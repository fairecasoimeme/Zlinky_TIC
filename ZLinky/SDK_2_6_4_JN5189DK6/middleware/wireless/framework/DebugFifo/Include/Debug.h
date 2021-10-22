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

#ifndef _debug_h_
#define _debug_h_

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
#ifndef DEBUG_ENABLE
 #define vDebugInit();
 #define vDebug(A);
 #define vDebugVal(A,B);
 #define vDebugHex(A,B);
 #define vDebugNonInt(A);
 #define vDebugValNonInt(A,B);
 #define vDebugHexNonInt(A,B);
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
#ifdef DEBUG_ENABLE
 extern PUBLIC void vDebugInit(void);
 extern PUBLIC void vDebug(char *pcMessage);
 extern PUBLIC void vDebugVal(char *pcMessage, uint32 u32Value);
 extern PUBLIC void vDebugHex(uint32 u32Data, int iSize);
 extern PUBLIC void vDebugNonInt(char *pcMessage);
 extern PUBLIC void vDebugValNonInt(char *pcMessage, uint32 u32Value);
 extern PUBLIC void vDebugHexNonInt(uint32 u32Data, int iSize);
#endif

#ifdef __cplusplus
};
#endif

#endif /* _debug_h_ */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
