/****************************************************************************
 *
 * Copyright 2020 NXP.
 *
 * NXP Confidential. 
 * 
 * This software is owned or controlled by NXP and may only be used strictly 
 * in accordance with the applicable license terms.  
 * By expressly accepting such terms or by downloading, installing, activating 
 * and/or otherwise using the software, you are agreeing that you have read, 
 * and that you agree to comply with and are bound by, such license terms.  
 * If you do not agree to be bound by the applicable license terms, 
 * then you may not retain, install, activate or otherwise use the software. 
 * 
 *
 ****************************************************************************/




/* Standard includes. */
#include <stdlib.h>
#include "dbg.h"
#include "portmacro_JN518x.h"
#if ZIGBEE_USE_FRAMEWORK
#include "SecLib.h"
#include "fsl_os_abstraction.h"
#else
#include "aessw_ccm.h"
#include "MicroSpecific.h"
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef ZIGBEE_USE_FRAMEWORK
PUBLIC bool_t bACI_WriteKey(tsReg128 *psKeyData)
{
    status_t status;
    status = AES_SetKey(AES0, (uint8 *)psKeyData, 16);
    if (status != kStatus_Success)
    {
        return FALSE;
    }
    return TRUE;
}

/****************************************************************************
 *
 * NAME: ZPS_u32Reverse
 *
 * DESCRIPTION:  reverse the byte order of a word
 *               
 *
 * RETURNS:
 *
 ****************************************************************************/
PUBLIC uint32 u32Reverse(uint32 u32InWord)
{
    uint32 u32OutWord;

    asm volatile ("REV %[reverse], %[input];"
                  : [reverse] "=r" (u32OutWord)
                  : [input]  "r"  (u32InWord)   );

    return u32OutWord;
}

/****************************************************************************
 *
 * NAME:       vSwipeEndian
 */
/**
 * Reverses 1282bit data between AESSW_Block_u and tsReg128
 *
 *
 * @param puBlock  128 bit data of type AESSW_Block_u
 *
 * @param psReg  128 bit data of type  tsReg128
 *
 * @param bBlockToReg  direction of converesion
 * 						1 = AESSW_Block_u to tsReg128
 *						0 = tsReg128 to AESSW_Block_u
 *
 * @return
 *
 * @note
 *
 ****************************************************************************/
PUBLIC void vSwipeEndian(AESSW_Block_u *puBlock, tsReg128 *psReg, bool_t bBlockToReg)
{
    int i=0;
    for (i = 0; i < 4 ; i++)
    {
        if(bBlockToReg)
            ((uint32*)psReg)[i] = u32Reverse((uint32)(puBlock->au32[i]));
        else
            puBlock->au32[i]  = u32Reverse(((uint32*)psReg)[i]);
	}
}
#endif
#ifdef JENNIC_DEBUG_ENABLE
#define BUFFER_SIZE 24


PRIVATE bool_t bIsNewline = TRUE;
PRIVATE void vNewLine(const char *pcFileName, uint32 u32LineNumber);
PRIVATE void vWriteUIntToBuffer(unsigned long long ullInteger,
                                int iBase,
                                char acOutBuffer[BUFFER_SIZE],
                                int *piOutBufferStart,
                                int *piOutBufferSize);


PRIVATE const char *pccHandlePlaceHolder(const char *pcFileName,
                                         uint32 u32LineNumber,
                                         const char *pcFormat,
                                         va_list *pap) __attribute__((unused));

PRIVATE void (*vPutchFn)    (char c)    = NULL;
PRIVATE void (*vFlushFn)    (void)      = NULL;

#endif

/****************************************************************************
 *
 * NAME: ZPS_u8GrabMutexLock
 *
 * DESCRIPTION:  Provides mutex protection for function re-entrancy
 *               
 *               
 *
 * RETURNS: uint8 
 *
 ****************************************************************************/
uint8 ZPS_u8GrabMutexLock( void* hMutex , uint32* psIntStore )
{
    if( hMutex != NULL )
    {
        if(*( (bool_t*)(( (void* (*) (void))hMutex )())))
        {
          * ( (uint32*) (0xBADADD04) ) = 1;
        }
    }
#if ZIGBEE_USE_FRAMEWORK
   OSA_InterruptEnableRestricted(psIntStore);
#else
    /* disable interrupts */
    MICRO_DISABLE_AND_SAVE_INTERRUPTS(*psIntStore);
#endif
    if( hMutex != NULL )
        (*( (bool_t*)(( (void* (*) (void))hMutex )()))) = TRUE;
#if ZIGBEE_USE_FRAMEWORK
    OSA_InterruptEnableRestore(psIntStore);
#else
    MICRO_RESTORE_INTERRUPTS(*psIntStore);
#endif
    return 0;

}


/****************************************************************************
 *
 * NAME: ZPS_u8ReleaseMutexLock
 *
 * DESCRIPTION:  Provides mutex protection for function re-entrancy
 *               
 *               
 *
 * RETURNS: uint8 
 *
 ****************************************************************************/
uint8 ZPS_u8ReleaseMutexLock( void* hMutex , uint32* psIntStore )
{
#if ZIGBEE_USE_FRAMEWORK
   OSA_InterruptEnableRestricted(psIntStore);
#else
    /* disable interrupts */
    MICRO_DISABLE_AND_SAVE_INTERRUPTS(*psIntStore);
#endif
    if( hMutex != NULL )
        (*( (bool_t*)(( (void* (*) (void))hMutex )()))) = FALSE;
#if ZIGBEE_USE_FRAMEWORK
    OSA_InterruptEnableRestore(psIntStore);
#else
    MICRO_RESTORE_INTERRUPTS(*psIntStore);
#endif
    return 0;
}


/****************************************************************************
 *
 * NAME: ZPS_eEnterCriticalSection
 *
 * DESCRIPTION:  Provides protection from function re-entrancy and interrupt
 *               pre-emption.
 *               
 *
 * RETURNS:
 *
 ****************************************************************************/
PUBLIC uint8 ZPS_eEnterCriticalSection(void* hMutex, uint32* psIntStore)
{
    if( hMutex != NULL )
    {
    
        if(*( (bool_t*)(( (void* (*) (void))hMutex )())))
        {
          * ( (uint32*) (0xBADADD08) ) = 1;
        }
    }
#if ZIGBEE_USE_FRAMEWORK
   OSA_InterruptEnableRestricted(psIntStore);
#else
    /* disable interrupts */
    MICRO_DISABLE_AND_SAVE_INTERRUPTS(*psIntStore);
#endif

    if( hMutex != NULL )
        (*( (bool_t*)(( (void* (*) (void))hMutex )()))) = TRUE;

    return 0;
}

/****************************************************************************
 *
 * NAME: ZPS_eExitCriticalSection
 *
 * DESCRIPTION:  Allows interrupt pre-emption and function re-entrancy.
 *               
 *
 * RETURNS:
 *
 ****************************************************************************/
PUBLIC uint8 ZPS_eExitCriticalSection(void* hMutex , uint32* psIntStore)
{
    if( hMutex != NULL )
        (*( (bool_t*)(( (void* (*) (void))hMutex )()))) = FALSE;
#if ZIGBEE_USE_FRAMEWORK
    OSA_InterruptEnableRestore(psIntStore);
#else
    MICRO_RESTORE_INTERRUPTS(*psIntStore);
#endif
    return 0;
}
#ifdef JENNIC_DEBUG_ENABLE


PUBLIC void DBG_vInit(tsDBG_FunctionTbl *psFunctionTbl)
{
    vPutchFn = psFunctionTbl->prPutchCb;
    vFlushFn = psFunctionTbl->prFlushCb;
}
/****************************************************************************
 *
 * NAME: vNewLine
 *
 * DESCRIPTION:
 * Outputs the filename and line number
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vNewLine(const char *pcFileName, uint32 u32LineNumber)
{
    char acBuffer[BUFFER_SIZE];
    int iStart;
    int iSize;

    if(*pcFileName == '\0')
    {
        return;
    }

    while(*pcFileName != '\0')
    {
        vPutchFn(*pcFileName);
        pcFileName++;
    }

    vPutchFn(':');

    vWriteUIntToBuffer(u32LineNumber,
                       10,
                       acBuffer,
                       &iStart,
                       &iSize);

    while(iSize)
    {
        vPutchFn(acBuffer[iStart]);
        iStart++;
        iSize--;
    }

    vPutchFn(':');
    vPutchFn(' ');
}

/****************************************************************************
 *
 * NAME: DbgConsole_Printf
 *
 * DESCRIPTION:
 * Verbose version of printf
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/

PUBLIC int DbgConsole_Printf( const char *pcFormat,
                            ...)
{
    va_list ap;

	if(!vPutchFn)
	{
	    return -1;
	}

	if(!pcFormat)
	{
	    return -1;
	}
    va_start(ap, pcFormat);

    for(; *pcFormat != '\0'; pcFormat++)
    {
        if(bIsNewline)
        {
            bIsNewline = FALSE;

            vNewLine("", 0);
        }

        if(*pcFormat == '%')
        {
            pcFormat = pccHandlePlaceHolder("",
                                            0,
                                            pcFormat + 1,
                                            &ap);
        }
        else if(*pcFormat == '\n')
        {
            vPutchFn('\r');
            vPutchFn('\n');

            bIsNewline = TRUE;
        }
        else
        {
            vPutchFn(*pcFormat);
        }
    }

    /* call flush before exit */
    if(vFlushFn)
    {
       vFlushFn();
    }
	va_end(ap);
	return 0;
}

/****************************************************************************
 *
 * NAME: vWriteUIntToBuffer
 *
 * DESCRIPTION:
 * Converts a number of the specified base to a string
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vWriteUIntToBuffer(unsigned long long ullInteger,
                                int iBase,
                                char acOutBuffer[BUFFER_SIZE],
                                int *piOutBufferStart,
                                int *piOutBufferSize)
{
    int i = BUFFER_SIZE;

    do
    {
        /* cheap way to get a mod and a divide together -
         * multiply is way cheaper! */
        unsigned long long ullIntegerTmp = ullInteger / iBase;
        unsigned int uCurrentDigit = ullInteger - ullIntegerTmp * iBase;
        ullInteger = ullIntegerTmp;

        i--;
        if(uCurrentDigit < 0xA)
        {
            acOutBuffer[i] = '0' + uCurrentDigit;
        }
        else
        {
            acOutBuffer[i] = 'a' + uCurrentDigit - 0xA;
        }

    } while(ullInteger);

    *piOutBufferStart = i;
    *piOutBufferSize = BUFFER_SIZE - i;
}


/****************************************************************************
 *
 * NAME: pccHandlePlaceHolder
 *
 * DESCRIPTION:
 * Handles format specifiers
 *
 * RETURNS:
 * const char *
 *
 ****************************************************************************/
PRIVATE const char *pccHandlePlaceHolder(const char *pcFileName,
                                         uint32 u32LineNumber,
                                         const char *pcFormat,
                                         va_list *pap)
{
    char acBuffer[BUFFER_SIZE];

    bool_t bLeftAlign           = FALSE;
    bool_t bPadWith0NotSpace    = FALSE;
    bool_t bSignWithPlus        = FALSE;
    bool_t bSignWithSpace       = FALSE;

    int iMinWidth = 0;

    bool_t bIsShort    = FALSE;
    bool_t bIsLong     = FALSE;
    bool_t bIsLongLong = FALSE;

    unsigned long long ullArg = 0;


    /* flags */
    while(1)
    {
        bool_t bQuit = FALSE;

        switch(*pcFormat)
        {
        case '-':
            bLeftAlign = TRUE;
            break;
        case '0':
            bPadWith0NotSpace = TRUE;
            break;
        case '+':
            bSignWithPlus = TRUE;
            break;
        case ' ':
            bSignWithSpace = TRUE;
            break;
        default:
            bQuit = TRUE;
            break;
        }

        if(bQuit)
        {
            break;
        }
        pcFormat++;
    }

    /* min field width */
    if(*pcFormat >= '1' && *pcFormat <= '9')
    {
        while(1)
        {
            int iThisDigit = *pcFormat - '0';

            if(iThisDigit < 0 || iThisDigit > 9)
            {
                break;
            }

            pcFormat++;

            iMinWidth *= 10;
            iMinWidth += iThisDigit;
        }
    }

    /* size modifier */
    if(*pcFormat == 'l')
    {
        /* long */
        pcFormat++;
        bIsLong = TRUE;

        if(*pcFormat == 'l')
        {
            /* long long */
            pcFormat++;
            bIsLongLong = TRUE;
        }
    }
    else if(*pcFormat == 'h')
    {
        /* short */
        bIsShort = TRUE;
        pcFormat++;
    }

    if(*pcFormat == 'i' ||
       *pcFormat == 'd' ||
       *pcFormat == 'u' ||
       *pcFormat == 'x' ||
       *pcFormat == 'p')
    {
        int iStart = 0;
        int iSize = 0;

        bool_t bIsSigned = FALSE;
        bool_t bIsHex = FALSE;
        bool_t bIsNegative = FALSE;
        bool_t bIsPtr = FALSE;

        char cSign = '\0';

        if(*pcFormat == 'i' ||
           *pcFormat == 'd')
        {
            bIsSigned = TRUE;
        }
        else if(*pcFormat == 'x')
        {
            bIsHex = TRUE;
        }
        else if(*pcFormat == 'p')
        {
            bIsPtr = TRUE;

            bIsLongLong = (sizeof(void *) >= sizeof(long long));
            bIsLong     = (sizeof(void *) >= sizeof(long));
        }

        if(!bIsSigned)
        {
            bSignWithPlus = FALSE;
            bSignWithSpace = FALSE;
        }

        if(bIsLongLong)
        {
            if(bIsSigned)
            {
                ullArg = va_arg(*pap, long long);
            }
            else
            {
                ullArg = va_arg(*pap, unsigned long long);
            }
        }
        else if(bIsLong)
        {
            if(bIsSigned)
            {
                ullArg = va_arg(*pap, long);
            }
            else
            {
                ullArg = va_arg(*pap, unsigned long);
            }
        }
        else if(bIsShort)
        {
            /* take account of lower limits */
            if(bIsSigned)
            {
                short s = va_arg(*pap, int);
                ullArg = s;
            }
            else
            {
                unsigned short us = va_arg(*pap, unsigned int);
                ullArg = us;
            }
        }
        else
        {
            if(bIsSigned)
            {
                ullArg = va_arg(*pap, int);
            }
            else
            {
                ullArg = va_arg(*pap, unsigned int);
            }
        }

        if(bIsSigned)
        {
            long long llArg = ullArg;
            if(llArg < 0)
            {
                bIsNegative = TRUE;

                /* scalar value */
                ullArg = 0 - ullArg;
            }
        }

        vWriteUIntToBuffer(ullArg,
                           (bIsHex || bIsPtr) ? 16 : 10,
                           acBuffer,
                           &iStart,
                           &iSize);

        if(bIsNegative)
        {
            cSign = '-';
        }
        else
        {
            if(bSignWithPlus)
            {
                cSign = '+';
            }
            else if(bSignWithSpace)
            {
                cSign = ' ';
            }
        }

        iMinWidth -= iSize;

        if(cSign)
        {
            iMinWidth--;
        }

        if(bIsPtr)
        {
            iMinWidth -= 2;
        }

        if(bLeftAlign)
        {
            if(cSign)
            {
                /* write the sign */
                vPutchFn(cSign);
            }

            if(bIsPtr)
            {
                vPutchFn('0');
                vPutchFn('x');
            }

            /* write the digits */
            for(;iSize > 0; iSize--)
            {
                vPutchFn(acBuffer[iStart++]);
            }

            /* write the pad char */
            for(; iMinWidth > 0; iMinWidth--)
            {
                /* note, '0' is ignored when the '-' flag is set */
                vPutchFn(' ');
            }
        }
        else
        {
            if(bPadWith0NotSpace)
            {
                if(cSign)
                {
                    vPutchFn(cSign);
                }

                if(bIsPtr)
                {
                    vPutchFn('0');
                    vPutchFn('x');
                }

                /* write the pad char */
                for(; iMinWidth > 0; iMinWidth--)
                {
                    vPutchFn('0');
                }

                /* write the digits */
                for(;iSize > 0; iSize--)
                {
                    vPutchFn(acBuffer[iStart++]);
                }
            }
            else
            {
                /* write the pad char */
                for(; iMinWidth > 0; iMinWidth--)
                {
                    vPutchFn(' ');
                }

                if(cSign)
                {
                    vPutchFn(cSign);
                }

                if(bIsPtr)
                {
                    vPutchFn('0');
                    vPutchFn('x');
                }

                /* write the digits */
                for(;iSize > 0; iSize--)
                {
                    vPutchFn(acBuffer[iStart++]);
                }
            }
        }
    }
    else
    {
        char c = '\0';
        const char *pc = NULL;

        switch(*pcFormat)
        {
        case 'c':
            c = va_arg(*pap, int);
            if (c == '\n')
            {
                vPutchFn('\r');
                vPutchFn('\n');
                bIsNewline = TRUE;
            }
            else /* PR #21 http://trac/Cortex/ticket/21 - output character if it's not a new line */
            {
                vPutchFn(c);
            }
            break;

        case 's':
            {
                const char *pcc;
                pc = va_arg(*pap, char *);
                pcc = pc;
                for(;*pcc != '\0';pcc++)
                {
                    if(bIsNewline)
                    {
                        bIsNewline = FALSE;

                        vNewLine(pcFileName, u32LineNumber);
                    }

                    if(*pcc == '\n')
                    {
                        vPutchFn('\r');
                        vPutchFn('\n');

                        bIsNewline = TRUE;
                    }
                    else
                    {
                        vPutchFn(*pcc);
                    }
                }
            }
            break;

        case '%':
            vPutchFn(*pcFormat);
            break;
        default:
            /* error */
            return pcFormat - 1;
        }
    }

    return pcFormat;
}
#endif

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

