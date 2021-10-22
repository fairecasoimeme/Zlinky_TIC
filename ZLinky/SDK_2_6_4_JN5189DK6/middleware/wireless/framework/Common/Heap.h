/*****************************************************************************
 *
 * MODULE:              Heap manager
 *
*/
/****************************************************************************
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

* Copyright NXP B.V. 2012. All rights reserved
*
***************************************************************************/

#ifndef  HEAP_INCLUDED
#define  HEAP_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
#ifdef HEAP_DEBUG
extern PUBLIC void vHeapDebug(char *pcMessage);
extern PUBLIC void vHeapDebugHex(uint32 u32Data, int iSize);
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifdef HEAP_DEBUG
#define vHeap_Alloc(A, B, C, D, E) \
    A = (B *)pvHeap_Alloc((void *)(A), C, D); \
    vHeapDebug(E); \
    vHeapDebug(":Req:"); \
    vHeapDebugHex(C, 4); \
    vHeapDebug(":"); \
    vHeapDebugHex(u32HeapStart, 8); \
    vHeapDebug(" ")
#else
#define vHeap_Alloc(A, B, C, D, E) A = (B *)pvHeap_Alloc((void *)(A), C, D)
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC void *pvHeap_Alloc(void *pvPointer, uint32 u32BytesNeeded, bool_t bClear);
PUBLIC void vHeap_ResetHeap();
PUBLIC uint32 vHeap_GetEnd();
#ifndef PDM_NO_RTOS
PUBLIC void pvHeap_Free(void *pvPointer);
#endif

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern PUBLIC uint32 u32HeapStart;
extern PUBLIC uint32 u32HeapEnd;

#if defined __cplusplus
}
#endif

#endif  /* HEAP_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

