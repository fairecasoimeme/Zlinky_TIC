/*****************************************************************************
 *
 * MODULE:             Debug tracing
 *
 * DESCRIPTION:        Provide printf style debug tracing
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
 * Copyright NXP B.V. 2012. All rights reserved
 *
 ***************************************************************************/

#ifndef DBG_H_
#define DBG_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include <jendefs.h>

#if defined __cplusplus
extern "C" {
#endif

#if (!defined JENNIC_CHIP_FAMILY_JN518x)
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/** Flags that change the operation of the DBG library. These flags should be
 *  set in the variable \ref DBG_u32Flags.
 *  @{
 */

/** No flags
 */
#define DBG_FLAG_NONE                       0

/** Convert outgoing newline (\\n) characters into carriage return, newline(\\r\\n)
 *  This is necessary depending on the terminal program being used.
 */
#define DBG_FLAG_OUTGOING_NL_CRNL           (1 << 0)

/** This flag indicates that flush should be called at the end of every 
 *  \ref DBG_vPrintf.
 */
#define DBG_FLAG_AUTO_FLUSH                 (1 << 8)

/** For DBG implementations that use an internal buffer, this flag indicates 
 *  that the buffer should be flushed whenever it is full. 
 *  This could cause the CPU to be stalled at any time during DBG_vPrintf()
 */
#define DBG_FLAG_FLUSH_WHEN_FULL            (1 << 9)

/** @} */


#ifdef __GNUC__

#ifdef DBG_VERBOSE
/* note - a warning will be issued if ASSERTION evaluates to FALSE at
 * compile time */
#define DBG_vAssert(STREAM, ASSERTION)                              \
    do {                                                            \
        const bool_t bAssertion = (ASSERTION);                      \
        typedef char DBG_STATIC_ASSERT_FAILED[bAssertion ? 0 : -1]; \
        bool_t bEval = ((STREAM) && !bAssertion) ?                  \
                dbg_vAssertImplVerbose(__FILE__,                    \
                                       __LINE__,                    \
                                       #ASSERTION), TRUE : FALSE;   \
        bEval = bEval;                                              \
    } while(0)
#else
/* note - a warning will be issued if ASSERTION evaluates to FALSE at
 * compile time */
#define DBG_vAssert(STREAM, ASSERTION)                                \
    do {                                                              \
        const bool_t bAssertion = (ASSERTION);                        \
        typedef char DBG_STATIC_ASSERT_FAILED[bAssertion ? 0 : -1];   \
        bool_t bEval = ((STREAM) && !bAssertion) ?                    \
                dbg_vAssertImplNoneVerbose(#ASSERTION), TRUE : FALSE; \
        bEval = bEval;                                                \
    } while(0)
#endif

/* need to write this in such a way that FORMAT doesn't get linked in
 * if STREAM isn't enabled, but all the ARGS still get evaluated in
 * case they may cause side effects
 * (may still need work to make this portable to non GCC compilers) */
#ifdef DBG_VERBOSE
#define DBG_vPrintf(STREAM, FORMAT, ARGS...)                        \
    do {                                                            \
        const char *pccFormatIn = (FORMAT);                         \
        const char *pccFormat = (STREAM) ?                          \
            pccFormatIn : NULL;                                     \
        dbg_vPrintfImplVerbose(__FILE__, __LINE__, pccFormat, ## ARGS);    \
    } while(0)
#else
#define DBG_vPrintf(STREAM, FORMAT, ARGS...)                        \
    do {                                                            \
        const char *pccFormatIn = (FORMAT);                         \
        const char *pccFormat = (STREAM) ?                          \
            pccFormatIn : NULL;                                     \
        (STREAM) ?                                                  \
            dbg_vPrintfImplNoneVerbose(pccFormat, ## ARGS) :        \
            dbg_vPrintfNullImpl("",0, pccFormat, ## ARGS);          \
    } while(0)
#endif

/* define empty functions as inline, so they
 * 1) don't take up any space
 * 2) don't attempt to link to the library functions */
#define DBG_INLINE static inline __attribute__((always_inline))

/* set up printf style warnings */
#define DBG_FORMAT_PRINTF(A, B) __attribute__((format (__printf__, A, B)))

#else

#ifdef DBG_VERBOSE
/* note - a warning will be issued if ASSERTION evaluates to FALSE at
 * compile time */
#define DBG_vAssert(STREAM, ASSERTION)                              \
    do {                                                            \
        const bool_t bAssertion = (ASSERTION);                      \
        bool_t bEval = ((STREAM) && !bAssertion) ?                  \
                dbg_vAssertImplVerbose(__FILE__,                    \
                                       __LINE__,                    \
                                       #ASSERTION), TRUE : FALSE;   \
        bEval = bEval;                                              \
    } while(0)
#else
/* note - a warning will be issued if ASSERTION evaluates to FALSE at
 * compile time */
#define DBG_vAssert(STREAM, ASSERTION)                                \
    do {                                                              \
        const bool_t bAssertion = (ASSERTION);                        \
        bool_t bEval = ((STREAM) && !bAssertion) ?                    \
                dbg_vAssertImplNoneVerbose(#ASSERTION), TRUE : FALSE; \
        bEval = bEval;                                                \
    } while(0)
#endif

/* need to write this in such a way that FORMAT doesnt get linked in
 * if STREAM isnt enabled, but all the ARGS still get evaluated in
 * case they may cause side effects
 * (may still need work to make this portable to non GCC compilers) */
#ifdef DBG_VERBOSE
#define DBG_vPrintf                                                \
        dbg_vSetFileLine(__FILE__, __LINE__);                      \
        dbg_vPrintfImpl2
#else
#define DBG_vPrintf                                                \
        dbg_vPrintfImplNoneVerbose
#endif

#define DBG_INLINE static
#define DBG_FORMAT_PRINTF(A, B)
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef struct
{
    void (*prInitHardwareCb)(void);
    void (*prPutchCb)       (char c);
    void (*prFlushCb)       (void);
    void (*prFailedAssertCb)(void);
    int  (*prGetCharCb)     (void);
} tsDBG_FunctionTbl;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

#if defined(DBG_ENABLE) || defined(DBG_C_)

PUBLIC void DBG_vInit(tsDBG_FunctionTbl *psFunctionTbl);

PUBLIC void DBG_vReset(void);

PUBLIC void DBG_vFlush(void);

PUBLIC int  DBG_iGetChar(void);

void DBG_vDumpStack(void);

#ifndef __GNUC__
PUBLIC void dbg_vSetFileLine(const char *pcFileName,
                             uint32 u32LineNumber);

DBG_FORMAT_PRINTF(3, 4)
PUBLIC void dbg_vPrintfImpl2(bool_t bStream,
                             const char    *pcFormat,
                             ...);
#endif

DBG_FORMAT_PRINTF(3, 4)
PUBLIC void dbg_vPrintfImplVerbose(const char    *Private_pcSrcFileName,
                                   uint32         Private_u32SrcLineNumber,
                                   const char    *pcFormat,
                                   ...);

PUBLIC void dbg_vPrintfImplNoneVerbose(const char *pcFormat, ...);

PUBLIC void dbg_vAssertImplVerbose(const char    *Private_pcSrcFileName,
                                   uint32         Private_u32SrcLineNumber,
                                   const char    *pcAssertionString);

PUBLIC void dbg_vAssertImplNoneVerbose(const char    *pcAssertionString);

#else /* defined(DBG_ENABLE) || defined(DBG_C_) */

/* need to make sure the parameters are still evaluated in a safe scope */
#define DBG_vInit(psFunctionTbl) ((void)(psFunctionTbl))

#define DBG_vReset() ((void)0)

#define DBG_vFlush() ((void)0)

#define DBG_iGetChar() ((int)-1)

#define DBG_vDumpStack() ((void)0)

#ifndef __GNUC__
DBG_INLINE
PUBLIC void dbg_vSetFileLine(const char *pcFileName,
                             uint32 u32LineNumber){}

DBG_FORMAT_PRINTF(3, 4)
DBG_INLINE
PUBLIC void dbg_vPrintfImpl2(bool_t bStream,
                             const char    *pcFormat,
                             ...){}
#endif

DBG_FORMAT_PRINTF(3, 4)
DBG_INLINE
PUBLIC void dbg_vPrintfImplVerbose(const char    *Private_pcSrcFileName,
                                   uint32         Private_u32SrcLineNumber,
                                   const char    *pcFormat,
                                   ...){}

DBG_INLINE
PUBLIC void dbg_vPrintfImplNoneVerbose(const char    *pcFormat,
                                       ...){}

DBG_INLINE
PUBLIC void dbg_vAssertImplVerbose(const char    *Private_pcSrcFileName,
                                   uint32         Private_u32SrcLineNumber,
                                   const char    *pcAssertionString){}

DBG_INLINE
PUBLIC void dbg_vAssertImplNoneVerbose(const char    *pcAssertionString){}

#endif /* defined(DBG_ENABLE) || defined(DBG_C_) */

DBG_INLINE
PUBLIC void dbg_vPrintfNullImpl(const char    *Private_pcSrcFileName,
                            uint32         Private_u32SrcLineNumber,
                            const char    *pcFormat,
                            ...){}

DBG_INLINE
PUBLIC void dbg_vAssertNullImpl(const char    *Private_pcSrcFileName,
                            uint32         Private_u32SrcLineNumber,
                            const char    *pcAssertionString){}

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

extern const char DBG_gszVERSION[];

/** Mask of flags used by the DBG library to control the output */
extern PUBLIC uint32 DBG_u32Flags;
 

#if defined __cplusplus
};
#endif

#else /* JENNIC_CHIP_FAMILY is JN518x */

#include "fsl_common.h"
#include "fsl_debug_console.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef DBG_vPrintf
#define DBG_vPrintf(STREAM, FORMAT, ARGS...)\
    do {                                                            \
        if(STREAM)                                                  \
            PRINTF((FORMAT), ## ARGS);                             \
    } while(0)
#endif
#ifndef DBG_vAssert
#define DBG_vAssert(STREAM, ASSERTION)                                \
    do {                                                              \
        bool_t bEval = (!(ASSERTION) && (STREAM) ) ?                    \
                PRINTF(#ASSERTION), *((uint32*)0x800000FB) = 0x1, TRUE : FALSE; \
        bEval = bEval;                                                  \
    } while(0)
#endif
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef struct
{
    void (*prInitHardwareCb)(void);
    void (*prPutchCb)       (char c);
    void (*prFlushCb)       (void);
    void (*prFailedAssertCb)(void);
    int  (*prGetCharCb)     (void);
} tsDBG_FunctionTbl;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

void DBG_vInit(tsDBG_FunctionTbl *psFunctionTbl);
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
#endif
#endif /*DBG_H_*/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
