/****************************************************************************
 *
 * MODULE:             heap manager
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

* Copyright NXP B.V. 2019. All rights reserved
*
***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>
#include "Heap.h"
#include "MicroSpecific.h"
#include "FunctionLib.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void *pvHeap_DefAlloc(void *pvPointer, uint32 u32BytesNeeded,
                              bool_t bClear);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
PUBLIC void *(*prHeap_AllocFunc)(void *, uint32, bool_t) = pvHeap_DefAlloc;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
extern uint32 _pvHeapStart;
extern uint32 _pvHeapLimit;

PUBLIC uint32 u32HeapStart = (uint32)&_pvHeapStart;
PUBLIC uint32 u32HeapEnd = (uint32)&_pvHeapLimit;

/****************************************************************************/
/***        Imported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME:        pvHeap_Alloc
 *
 * DESCRIPTION:
 * Allocates a block of memory from the heap.
 *
 * RETURNS:
 * Pointer to block, or NULL if the heap didn't have enough space. If block
 * was already assigned, returns original value and doesn't take anything
 * from heap.
 *
 * NOTES:
 * If buffer has already been allocated, it is not cleared. If it is a fresh
 * allocation, it is cleared on request.
 *
 ****************************************************************************/


PUBLIC void *pvHeap_Alloc(void *pvPointer, uint32 u32BytesNeeded, bool_t bClear)
{
    return prHeap_AllocFunc(pvPointer, u32BytesNeeded, bClear);
}

PRIVATE void *pvHeap_DefAlloc(void *pvPointer, uint32 u32BytesNeeded, bool_t bClear)
{

    /* If value already assigned, use that value. Otherwise, try to grab space
       from heap */
    if (pvPointer == NULL)
    {
        /* Assume heap start is already word aligned. If not enough space,
           return null */
        if ((u32HeapStart + u32BytesNeeded) > u32HeapEnd)
        {

            /* Trap: immediate value is bit # in SR: if bit is set, trap
               occurs. Wanted to use 15, as this bit is fixed at 1, but
               compiler doesn't seem able to cope with anything other than 0,
               which should also be valid */
            MICRO_TRAP();
            return NULL;
        }

        /* Clear assigned space, if requested */
        if (bClear)
        {
            FLib_MemSet((void *)u32HeapStart, 0, u32BytesNeeded);
        }

        /* Record current heap start to return to caller */
        pvPointer = (void *)u32HeapStart;

        /* Move heap start along and align it */
        u32HeapStart += u32BytesNeeded;
        u32HeapStart = (u32HeapStart + 3) & 0xfffffffc;
    }
    return pvPointer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
static int u32OverwriteCheckStart = 0;
#define HEAP_OVERWRITE_CHECK_CHAR 0x12


/****************************************************************************
 *
 * NAME:        iHeap_InitialiseOverwriteCheck
 *
 * DESCRIPTION:
 * Initialise the remaining free space with a check character
 *
 * RETURNS:
 * The number of remaining free bytes on the heap.
 *
 * NOTES:
 * Designed to be called after everything has been allocated.
 * Used to see how close the heap is to running out of memory.
 * Used in conjunction with iHeap_ReturnNumberOfOverwrittenBytes() to see
 * if the free area of the heap is getting overwritten by the stack and thus get
 * an early warning that the stack may be overwriting the heap and causing a crash.
 *
 ****************************************************************************/

PUBLIC int iHeap_InitialiseOverwriteCheck()
{
    // Start on aligned boundary
    u32OverwriteCheckStart = (u32HeapStart + 3) & 0xfffffffc;;
    memset((void *)u32OverwriteCheckStart, HEAP_OVERWRITE_CHECK_CHAR, u32HeapEnd - u32OverwriteCheckStart);
    return u32HeapEnd - u32OverwriteCheckStart;
}

/****************************************************************************
 *
 * NAME:        iHeap_ReturnNumberOfOverwrittenBytes
 *
 * DESCRIPTION:
 *  Checks how many bytes of the free heap space have been overwritten.
 *
 * RETURNS:
 * The number of bytes that have been overwritten by a stack overflow.
 *
 * NOTES:
 * Following a call to iHeap_InitialiseOverwriteCheck,
 * Checks how many bytes of the free heap space have been overwritten
 * from the end of the free space.
 *
 * Checks from start of free space until it finds a bad byte.
 * This means it will immediately find a changed byte and
 * return the total free space if anything has been allocated
 * from the heap after the call to iHeap_InitialiseOverwriteCheck.
 *
 ****************************************************************************/


PUBLIC int iHeap_ReturnNumberOfOverwrittenBytes()
{
    char *pPos = (char*)u32OverwriteCheckStart;

    while (*pPos == HEAP_OVERWRITE_CHECK_CHAR && (int)pPos < u32HeapEnd)
    {
        pPos++;
    }

    return u32HeapEnd - (int)pPos;
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
