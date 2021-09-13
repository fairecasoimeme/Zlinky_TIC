/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include <string.h>

#include "shell.h"
#include "FunctionLib.h"
#include "SerialManager.h"
#include "MemManager.h"
#include "board.h"

#if SHELL_ENABLED
/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
#define CTL_CH(c)               ((c) - 'a' + 1)
#define CTL_BACKSPACE           ('\b')
#define DEL                     ((char)255)
#define DEL7                    ((char)127)

/* Move cursor at the beginning of the line */
#define BEGINNING_OF_LINE()             \
while( mCmdIdx ) {                      \
    shell_putc(CTL_BACKSPACE);          \
    mCmdIdx--;                          \
}

/* Refresh the line starting from the current cursor pozition */
#define REFRESH_TO_EOL()                                \
if( mCmdIdx < mCmdLen )                                 \
{                                                       \
    shell_writeN(mCmdBuf + mCmdIdx, mCmdLen - mCmdIdx); \
    mCmdIdx = mCmdLen;                                  \
}

/* Clear current input */
#define SHELL_RESET() \
{                            \
    mCmdBuf[0] = '\0';       \
    mCmdLen = 0;             \
    mCmdIdx = 0;             \
}

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static void shell_main( void *params );
static int16_t shell_ProcessChr( void );
static void shell_erase_to_eol( void );

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static char*    pPrompt;
static char     mCmdBuf[SHELL_CB_SIZE + 1];
static uint16_t mCmdLen;
static uint16_t mCmdIdx;
uint8_t  gShellSerMgrIf;

uint8_t  mInsert = 1;
uint8_t  mShellMaxCmdLen = 0;

cmd_tbl_t *gpCmdTable[SHELL_CMD_TBL_SIZE];

int8_t (*mpfShellBreak)(uint8_t argc, char * argv[]) = NULL;
void (*pfShellProcessCommand) (char * pCmd, uint16_t length) = NULL;

#if SHELL_USE_LOGO
const char mLogo[] = "\n\n\r\n"
       " ####         ######      ##### ##########    \n\r"
       " ######      # ######    ##### #############  \n\r"  
       " #######     ## ######  ##### ############### \n\r"   
       " ########    ### ########### ####       ##### \n\r"   
       " #### #####  #### ######### #####       ##### \n\r"   
       " ####  ##### #### ######### ################# \n\r"   
       " ####   ######## ########### ###############  \n\r"   
       " ####     ##### ######  ##### ############    \n\r"   
       " ####      ### ######    ##### ##             \n\r"   
       " ####       # ######      ##### #             \n\r";
#endif

#if SHELL_USE_HELP
extern cmd_tbl_t CommandFun_Help;
extern cmd_tbl_t CommandFun_Ver;
#endif

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
#if SHELL_USE_AUTO_COMPLETE
extern int8_t cmd_auto_complete(char * prompt, char * buf, uint8_t * np, uint8_t * colp);
extern uint8_t cmd_auto_complete2(char *buf, uint8_t);
#endif

#if SHELL_MAX_HIST
extern void hist_init(void);
extern void hist_add(char * line);
extern char * hist_next(void);
extern char * hist_prev(void);
#endif

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  Initializes the SHELL module .
*
* \param[in]  prompt the string which will be used for command prompt
*
* \remarks
*
********************************************************************************** */
void shell_init(char* prompt)
{
    pPrompt = prompt;
    
    Serial_InitManager();

    do {
        /* Register Serial Manager interface */
        if( Serial_InitInterface(&gShellSerMgrIf, SHELL_IO_TYPE, SHELL_IO_NUMBER) != gSerial_Success_c )
            break;

        /* Set serial baud rate */
        (void)Serial_SetBaudRate(gShellSerMgrIf, SHELL_IO_SPEED);
#if (SHELL_USE_ALT_TASK == 0)
        /* Set RX callback */
        if(Serial_SetRxCallBack(gShellSerMgrIf, shell_main, NULL) != gSerial_Success_c)
            break;
#endif

        mCmdLen = 0;
        mCmdIdx = 0;
        FLib_MemSet(gpCmdTable, 0, sizeof(gpCmdTable));
        FLib_MemSet(mCmdBuf, 0, sizeof(mCmdBuf));
#if SHELL_USE_HELP
        shell_register_function(&CommandFun_Help);
        shell_register_function(&CommandFun_Ver);
#endif
#if SHELL_MAX_HIST
        hist_init();
#endif
#if SHELL_USE_LOGO
        if (strlen(prompt) != 0)
        {
            /* Print out logo only if the configured prompt is not empty */
            shell_write((char*)mLogo);
            shell_write("\r\nSHELL build: ");
            shell_write(__DATE__);
            shell_write("\n\rCopyright (c) 2016 NXP Semiconductors\r\n");
            shell_write(pPrompt);
        }
#endif
    } while (0);
}

void shell_reinit(void)
{
    SerialInterface_Reinit(gShellSerMgrIf);
    Serial_SetBaudRate(gShellSerMgrIf, SHELL_IO_SPEED);
}

/*! *********************************************************************************
* \brief  Changes the prompt name for the SHELL module .
*
* \param[in]  prompt the string which will be used for command prompt
*
* \remarks
*
********************************************************************************** */
void shell_change_prompt(char* prompt, bool_t no_print)
{
    pPrompt = prompt;
    if (no_print)
        shell_reset();
    else
        shell_refresh();
}

/*! *********************************************************************************
* \brief  Notify shell that the async command has finished
*
********************************************************************************** */
void shell_cmd_finished(void)
{
    mpfShellBreak = NULL;
    SHELL_RESET();
    SHELL_NEWLINE();
    shell_write(pPrompt);
}

void shell_reset(void)
{
    SHELL_RESET();
}

/*! *********************************************************************************
* \brief  Empty command buffer and print command prompt
*
********************************************************************************** */
void shell_refresh(void)
{
    shell_reset();
#if (SHELL_USE_ECHO)
      SHELL_NEWLINE();
#if !gHybridApp_d
      shell_write(pPrompt);
#endif
#endif
}

/*! *********************************************************************************
* \brief  This function will write N bytes  over the serial interface
*
* \param[in]  pBuff pointer to a string
* \param[in]  n number of chars to be written
*
* \remarks
*
********************************************************************************** */
void shell_writeN(const char *pBuff, const uint16_t n)
{
    if( !pBuff || !n )
        return;

    if (SHELL_IO_TYPE == gSerialMgrIICSlave_c || 
        SHELL_IO_TYPE == gSerialMgrSPISlave_c)
    {
        uint8_t *pHdr = MEM_BufferAlloc(n+5);

        if (pHdr)
        {
            pHdr[0] = 0x02;
            pHdr[1] = 0x77;
            pHdr[2] = 0x77;
            pHdr[3] = n;
            FLib_MemCpy(&pHdr[4], pBuff, n);
            pHdr[4+n] = 0;
            Serial_SyncWrite( gShellSerMgrIf, pHdr, n+5 );
            MEM_BufferFree(pHdr);
        }
    }
    else
    {
        Serial_SyncWrite(gShellSerMgrIf, (uint8_t*)pBuff, n);
    }
}

/*! *********************************************************************************
* \brief  This function will write a NULL terminated string over the serial interface
*
* \param[in]  pBuff pointer to a string
*
* \remarks
*
********************************************************************************** */
void shell_write( const char *pBuff )
{
    shell_writeN(pBuff, strlen(pBuff));
}

/*! *********************************************************************************
* \brief  This function will write one character over the serial interface
*
* \param[in]  c character to be written
*
* \remarks
*
********************************************************************************** */
void shell_putc(const char c)
{
    Serial_SyncWrite(gShellSerMgrIf, (uint8_t*)&c, 1);
}

/*! *********************************************************************************
* \brief  This function will write a decimal number over the serial interface
*
* \param[in]  nb number to be written
*
* \remarks
*
********************************************************************************** */
void shell_writeDec
(
    const uint32_t nb
)
{
    Serial_PrintDec(gShellSerMgrIf, nb);
}

/*! *********************************************************************************
* \brief  This function will write a signed decimal number over the serial interface
*
* \param[in]  nb number to be written
*
* \remarks
*
********************************************************************************** */
void shell_writeSignedDec
(
    const int8_t nb
)
{
	int8_t signed_nb = nb;
    if (signed_nb >> 7)
    {
        /* Negative Value */
        shell_write("-");
        signed_nb = ~(signed_nb - 1);
    }
    Serial_PrintDec(gShellSerMgrIf, signed_nb);
}

/*! *********************************************************************************
* \brief  This function will write a decimal number over the serial interface
*
* \param[in]  pHex  pointer to an octet string
* \param[in]  len   the length of the string
*
* \remarks
*
********************************************************************************** */
void shell_writeHex
(
    const uint8_t *pHex,
    const uint8_t len
)
{
    Serial_PrintHex(gShellSerMgrIf, pHex, len, gPrtHexBigEndian_c);
}

/*! *********************************************************************************
* \brief  This function will write a decimal number over the serial interface
*
* \param[in]  pHex  pointer to an octet string
* \param[in]  len   the length of the string
*
* \remarks
*
********************************************************************************** */
void shell_writeHexLe
(
    const uint8_t *pHex,
    const uint8_t len
)
{
    Serial_PrintHex(gShellSerMgrIf, pHex, len, gPrtHexNoFormat_c);
}

/*! *********************************************************************************
* \brief  This function will write a boolean value in pretty format: "TRUE" or "FALSE"
*
* \param[in]  b  Boolean value
*
* \remarks
*
********************************************************************************** */
void shell_writeBool
(
    bool_t boolValue
)
{
    if (boolValue)
    {
        shell_write("TRUE");
    }
    else
    {
        shell_write("FALSE");
    }
}

/*! *********************************************************************************
* \brief  This function will output a formated string onto the serial interface
*
* \param[in]  format string defining the output
* \param[in]  ... variable number of parameters
*
* \remarks
*
********************************************************************************** */
#if SHELL_USE_PRINTF
#include <stdio.h>
#include <stdarg.h>

uint16_t shell_printf(char * format,...)
{
    va_list ap;
    uint16_t n = 0;
    char *pStr = (char*)MEM_BufferAlloc(SHELL_CB_SIZE);

    if (pStr)
    {
        va_start(ap, format);
        n = vsnprintf(pStr, SHELL_CB_SIZE, format, ap);
        //va_end(ap); /* follow MISRA... */
        Serial_SyncWrite(gShellSerMgrIf, (uint8_t*)pStr, n);
        MEM_BufferFree(pStr);
    }

    return n;
}
#endif

/*! *********************************************************************************
* \brief  This function registers a command into the SHELL
*
* \param[in]  pAddress pointer to the command structure
*
* \remarks
*
********************************************************************************** */
uint8_t shell_register_function(cmd_tbl_t * pAddress)
{
    uint16_t i;
    /* check name conflict */
    for (i = 0; i < SHELL_CMD_TBL_SIZE; i++)
    {
        if (gpCmdTable[i] && !strcmp(pAddress->name, gpCmdTable[i]->name))
        {
            return 1;
        }
    }
    /* insert */
    for (i = 0; i < SHELL_CMD_TBL_SIZE; i++)
    {
        if (gpCmdTable[i] == NULL)
        {
            gpCmdTable[i] =  pAddress;
            // Update max command length
            i = strlen(pAddress->name);
            if (i > mShellMaxCmdLen)
            {
                mShellMaxCmdLen = i;
            }
            
            return 0;
        }
    }
    return 1;
}

/*! *********************************************************************************
* \brief  This function registers multiple commands into the SHELL
*
* \param[in]  pAddress pointer to an array of commands
* \param[in]  num the number of entries
*
* \remarks
*
********************************************************************************** */
void shell_register_function_array(cmd_tbl_t * pAddress, uint8_t num)
{
    while (num--)
    {
        shell_register_function(pAddress++);
    }
}

/*! *********************************************************************************
* \brief  This function remove a command from the SHELL
*
* \param[in]  name a string reprezenting the command to be removed
*
* \remarks
*
********************************************************************************** */
uint8_t shell_unregister_function(char * name)
{
    uint16_t i;

    for( i=0; i<SHELL_CMD_TBL_SIZE; i++ )
    {
        if( gpCmdTable[i] && !strcmp(name, gpCmdTable[i]->name) )
        {
            gpCmdTable[i] = NULL;
            return 0;
        }
    }

    return 1;
}

/*! *********************************************************************************
* \brief  This function is used to get the pointer to the value of the option
*         specified by pOption parameter.
*
* \param [in]   argc      number of available arguments
* \param [in]   argv      pointer to the list of arguments
* \param [in]   pOption   pointer to the string which describes the option
*
* \return       char*     pointer to the value of the option
*
********************************************************************************** */
char * shell_get_opt
(
    uint8_t argc,
    char *argv[],
    char *pOption
)
{
    uint8_t idx;
    char *pRes = NULL;

    /* Check all arguments */
    for(idx=0;(idx<argc)&&(!pRes);idx++)
    {
        /* The option was found */
        if(FLib_MemCmp(argv[idx], pOption, MIN(strlen(argv[idx]), strlen(pOption))) && argv[idx + 1])
        {
            /* The value should be in this argument after the pOption text */
            if(*(argv[idx] + strlen(pOption)))
            {
                pRes = argv[idx] + strlen(pOption);
            }
            /* The value should be in the next argument */
            else
            {
                pRes = argv[idx + 1];
            }
        }
    }

    return pRes;
}

/*! *********************************************************************************
* \brief  This function return a pointer to the command data structure
*
* \param [in]   cmd      string reprezenting the command
*
* \return       cmd_tbl_t*     pointer to the command's data
*
********************************************************************************** */
cmd_tbl_t * shell_find_command (char * cmd)
{
    uint16_t i;

    if (!cmd)
    {
        return NULL;
    }

    for (i = 0; i < SHELL_CMD_TBL_SIZE; i++)
    {
        if ((gpCmdTable[i] != NULL) && !strcmp((char*)cmd, gpCmdTable[i]->name))
        {
            return gpCmdTable[i];
        }
    }

    return NULL;
}

/*! *********************************************************************************
* \brief  This function splits a command string into arguments
*
* \param [in]   s       command strin
* \param [in]   argvsz  maximun number of arguments
* \param [in]   argv    pointer to the argument vector
*
* \return       uint8_t     the number of arguments found
*
********************************************************************************** */
uint8_t make_argv(char *s, uint8_t argvsz, char * argv[])
{
    uint8_t argc = 0;
    /* split into argv */
    while (argc < argvsz - 1)
    {
        /* skip any white space */
        while( *s == ' ' )
        {
            ++s;
        }
        if (*s == '\0') /* end of s, no more args   */
        {
            argv[argc] = NULL;
            return argc;
        }    
        argv[argc++] = s;   /* begin of argument string */
        /* find end of string */
        while( (*s) && ( *s != ' ' ) )
        {
            ++s;
        }
        if (*s == '\0')     /* end of s, no more args   */
        {
            argv[argc] = NULL;
            return argc;
        }
        *s++ = '\0';        /* terminate current arg     */
    }
    return argc;
}

/*! *********************************************************************************
* \brief  This function process the characters received over the serial interface
*         
*
********************************************************************************** */
#if SHELL_USE_ALT_TASK
void shell_task(void)
{
    shell_main(NULL);
}
#endif

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  This function is called every time a character is received.
*         The main SHELL processing is done from here
*
* \param [in]   params       unused
*
********************************************************************************** */
static void shell_main( void *params )
{
    int16_t ret;
    uint8_t argc;
    char * argv[SHELL_MAX_ARGS+1];    /* NULL terminated  */
    cmd_tbl_t * cmdtp;

    // Process the received char
    ret = shell_ProcessChr();
    
    if (ret == 0)
    {
        if (mCmdLen == 0)
        {
            SHELL_RESET();
            if (!mpfShellBreak)
            {
                shell_write(pPrompt);
            }
        }
        else if (pfShellProcessCommand)
        {
            pfShellProcessCommand(mCmdBuf, mCmdLen);
        }
        else
        {
            // Split command into arguments
            argc = make_argv(mCmdBuf, SHELL_MAX_ARGS+1, argv);
            if (argc >= SHELL_MAX_ARGS)
            {
                shell_write("** Too many args (max. ");
                shell_writeDec(SHELL_MAX_ARGS);
                shell_write (") **\r\n");
            }
            // Search for the appropriate command
            cmdtp = shell_find_command(argv[0]);
            if ((cmdtp != NULL) && (cmdtp->cmd != NULL))
            {
                if (argc > cmdtp->maxargs)
                {
                    ret = CMD_RET_USAGE;
                }
                else
                {
                    ret = (cmdtp->cmd)(argc, argv);
                }
            }
            else
            {
                shell_write("Unknown command '");
                shell_write(argv[0]);
#if SHELL_USE_HELP
                shell_write("' - try 'help'\r\n");
#else
                shell_write("' ");
#endif
            }
#if SHELL_USE_HELP
            if (ret == CMD_RET_USAGE)
            {
                if (cmdtp->usage != NULL)
                {
                    shell_write("Usage:\r\n");
                    shell_write(cmdtp->name);
                    shell_writeN(" - ", 3);
                    shell_write(cmdtp->usage);
                    SHELL_NEWLINE();
                }
                if (cmdtp->help != NULL)
                {
                    shell_write(cmdtp->name);
                    shell_writeN(" ", 1);
                    shell_write(cmdtp->help);
                    SHELL_NEWLINE();
                }
                else
                {
                    shell_write ("- No additional help available.\r\n");
                }
            }
#endif
            if (ret == CMD_RET_ASYNC)
            {
                mpfShellBreak = cmdtp->cmd;
                SHELL_RESET();
            }
            else
            {
                mpfShellBreak = NULL;
                shell_refresh();
            }
        }
    }
    else if (ret == -1)
    {
        shell_write("<INTERRUPT>\r\n");
        if (mpfShellBreak)
        {
            mpfShellBreak(0,0);
            mpfShellBreak = NULL;
            shell_refresh();
        }
        SHELL_RESET();
        if (pfShellProcessCommand)
        {
            pfShellProcessCommand(NULL, 0);
        }
    }
}

/*! *********************************************************************************
* \brief  This function is called to process the received character
*
* \return       uint16_t     0 - comand received complete
*                           -1 - CTRL + C was pressed
*                           -2 - new character received
*
********************************************************************************** */
static int16_t shell_ProcessChr( void )
{
    uint16_t wlen;
    char ichar;
    static uint8_t esc_len = 0;
//    static char esc_save[4];

    while( 1 ) 
    {
        Serial_Read( gShellSerMgrIf, (uint8_t*)&ichar, 1, &wlen );

        if( !wlen )
        {
            return -2;
        }

        if( (ichar == '\n') || (ichar == '\r') ) 
        {
            Serial_Read( gShellSerMgrIf, (uint8_t*)&ichar, 1, &wlen );
            SHELL_NEWLINE();
            break;
        }

        /* handle standard linux xterm esc sequences for arrow key, etc.*/
        if (esc_len != 0)
        {
            if (esc_len == 1) 
            {
                if (ichar == '[')
                {
//                    esc_save[esc_len] = ichar;
                    esc_len++;
                } 
                else
                {
//                    cread_add_str(esc_save, esc_len, mInsert, &mCmdIdx, &mCmdLen, mCmdBuf, mCmdLen);
                    esc_len = 0;
                }
                continue;
            }

            switch (ichar) 
            {
            case 'D':   /* <- key */
                ichar = CTL_CH('b');
                esc_len = 0;
                break;
            case 'C':   /* -> key */
                ichar = CTL_CH('f');
                esc_len = 0;
                break;  /* pass off to ^F handler */
            case 'H':   /* Home key */
                ichar = CTL_CH('a');
                esc_len = 0;
                break;  /* pass off to ^A handler */
            case 'A':   /* up arrow */
                ichar = CTL_CH('p');
                esc_len = 0;
                break;  /* pass off to ^P handler */
            case 'B':   /* down arrow */
                ichar = CTL_CH('n');
                esc_len = 0;
                break;  /* pass off to ^N handler */
            default:
//                esc_save[esc_len] = ichar;
                esc_len++;
//                cread_add_str(esc_save, esc_len, mInsert, &mCmdIdx, &mCmdLen, mCmdBuf, mCmdLen);
                esc_len = 0;
                continue;
            }
        }

        switch (ichar)
        {
        case 0x1b:
            if (esc_len == 0) 
            {
//                esc_save[esc_len] = ichar;
                esc_len++;
            }
            else 
            {
                shell_write("impossible condition #876\n");
                esc_len = 0;
            }
            break;
        case CTL_CH('a'):
            BEGINNING_OF_LINE();
            break;
        case CTL_CH('c'):   /* ^C - break */
            SHELL_RESET();
            return (-1);
        case CTL_CH('f'):
            if( mCmdIdx < mCmdLen )
            {
                shell_putc(mCmdBuf[mCmdIdx]);
                mCmdIdx++;
            }
            break;
        case CTL_CH('b'):
            if( mCmdIdx )
            {
                shell_putc(CTL_BACKSPACE);
                mCmdIdx--;
            }
            break;
        case CTL_CH('d'):
            if (mCmdIdx < mCmdLen)
            {
                wlen = mCmdLen - mCmdIdx - 1;
                if (wlen)
                {
                    FLib_MemInPlaceCpy(&mCmdBuf[mCmdIdx],&mCmdBuf[mCmdIdx+1],wlen);
                    shell_writeN(mCmdBuf + mCmdIdx, wlen);
                }
                shell_putc(' ');
                do 
                {
                    shell_putc(CTL_BACKSPACE);
                } while (wlen--);
                mCmdLen--;
            }
            break;
        case CTL_CH('k'):
            shell_erase_to_eol();
            break;
        case CTL_CH('e'):
            REFRESH_TO_EOL();
            break;
        case CTL_CH('o'):
            mInsert = !mInsert;
            break;
        case CTL_CH('x'):
        case CTL_CH('u'):
            BEGINNING_OF_LINE();
            shell_erase_to_eol();
            break;
        case DEL:
        case DEL7:
        case 8:
            if (mCmdIdx)
            {
                wlen = mCmdLen - mCmdIdx;
                mCmdIdx--;
                FLib_MemInPlaceCpy(&mCmdBuf[mCmdIdx], &mCmdBuf[mCmdIdx+1], wlen);
                shell_putc(CTL_BACKSPACE);
                shell_writeN(mCmdBuf + mCmdIdx, wlen);
                shell_putc(' ');
                do
                {
                    shell_putc(CTL_BACKSPACE);
                } while (wlen--);
                mCmdLen--;
            }
            break;
            
        case CTL_CH('p'):
        case CTL_CH('n'):
            {
#if SHELL_MAX_HIST
                char *hline;
                esc_len = 0;
                if (ichar == CTL_CH('p'))
                {
                    hline = hist_prev();
                }
                else
                {
                    hline = hist_next();
                }
                if (!hline)
                {
                    SHELL_BEEP();
                    continue;
                }
                /* nuke the current line */
                /* first, go home */
                BEGINNING_OF_LINE();
                shell_erase_to_eol();
                /* copy new line into place and display */
                strcpy(mCmdBuf, hline);
                mCmdLen = strlen(mCmdBuf);
                REFRESH_TO_EOL();
#endif /* SHELL_CONFIG_USE_HIST */
                continue;
            }

        case '\t': 
#if SHELL_USE_AUTO_COMPLETE
            {
                uint16_t col;
                /* do not autocomplete when in the middle */
                if (mCmdIdx < mCmdLen)
                {
                    SHELL_BEEP();
                    break;
                }
                mCmdBuf[mCmdIdx] = '\0';
                col = strlen(pPrompt) + mCmdLen;
                wlen = mCmdIdx;
                if( cmd_auto_complete(pPrompt, mCmdBuf, (uint8_t*)&wlen, (uint8_t*)&col) )
                {
                    col = wlen - mCmdIdx;
                    mCmdIdx += col;
                    mCmdLen += col;
                }
                break;
            }
#else
            {
                continue;
            }
#endif
        default:
#if (SHELL_IS_CASE_SENSITIVE == 0)
            if ((ichar >= 'A' && ichar <= 'Z'))
            {
                /* Convert received char to small caps */
                ichar += ('a' - 'A');
            }
#endif
            /* Add a character to the command buffer */
            if( (mCmdIdx < mCmdLen) && mInsert )
            {
                uint16_t len = mCmdLen - mCmdIdx;
                FLib_MemInPlaceCpy( &mCmdBuf[mCmdIdx+1],        
                                   &mCmdBuf[mCmdIdx], len );   
                mCmdBuf[mCmdIdx] = ichar;                           
                shell_writeN(mCmdBuf + mCmdIdx, len+1);         
                mCmdLen++;                                      
                mCmdIdx++;                                      
                while( len )                                    
                {                                               
                    shell_putc(CTL_BACKSPACE);                  
                    len--;                                      
                }                                               
            }                                                   
            else                                                
            {                                                   
                if( mCmdLen == mCmdIdx )                        
                    mCmdLen++;                                  
                mCmdBuf[mCmdIdx++] = ichar;         
#if SHELL_USE_ECHO
                shell_putc(ichar);
#endif
            }

            /* Check if the received command exceeds he size of the buffer */
            if( mCmdLen >= SHELL_CB_SIZE )
            {
                SHELL_RESET();
            }
            break;
        }
    }

    mCmdBuf[mCmdLen] = '\0';    /* lose the newline */
#if SHELL_MAX_HIST
    hist_add(mCmdBuf);
#endif
    return 0;
}
/*! *********************************************************************************
* \brief  Erase line from the current cursor position
*
********************************************************************************** */
static void shell_erase_to_eol(void)
{
    uint16_t i;

    if (mCmdIdx < mCmdLen)
    {
        for (i = mCmdIdx; i < mCmdLen; i++)
        {
            mCmdBuf[i] = ' ';
        }

        shell_writeN(mCmdBuf + mCmdIdx, mCmdLen - mCmdIdx);

        for (i = mCmdIdx; i < mCmdLen; i++)
        {
            mCmdBuf[i] = CTL_BACKSPACE;
        }

        shell_writeN(mCmdBuf + mCmdIdx, mCmdLen - mCmdIdx);

        mCmdLen = mCmdIdx;
    }
}
#endif /* SHELL_ENABLED */
