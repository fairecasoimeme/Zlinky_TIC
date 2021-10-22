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

#if (SHELL_ENABLED && SHELL_USE_AUTO_COMPLETE)
/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
  
/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  find the available matchs and return possiable match number.
*
* \param[in]  argc       The number of arguments
* \param[in]  argv       Table with command argumens
* \param[in]  last_char  The Last character received
* \param[in]  maxv       The maximun number of matchs
* \param[in]  cmdv       Table with possible matchs
*
* \return  int8_t  The number of possible matchs
*
********************************************************************************** */
static int8_t complete_cmdv(uint8_t argc, char * argv[], char last_char, uint8_t maxv, char *cmdv[])
{
    cmd_tbl_t *cmdtp;
    uint8_t i;
    uint8_t n_found = 0;

    cmdv[0] = NULL;

    /* sanity */
    if (maxv < 2)
        return -2;

    /* more than one arg or one but the start of the next */
    if ((argc > 1) || 
        ((argc == 1) && ((last_char == '\0') || (last_char == ' '))))
    {
        cmdtp = shell_find_command(argv[0]);
        if ((cmdtp == NULL) || (cmdtp->complete == NULL)) 
        {
            return 0;
        }
        return (*cmdtp->complete)(argc, argv, last_char, maxv, cmdv);
    }
    /*
    * one or no arguments
    * If no arguments, output full list of commands.
    * Some commands allow length modifiers (like "cp.b");
    * compare command name only until first dot.
    */
    for (i = 0; i < SHELL_CMD_TBL_SIZE; i++)
    {
        if (gpCmdTable[i] && gpCmdTable[i]->name)
        {
            if (argc && !FLib_MemCmp(argv[0], gpCmdTable[i]->name, strlen(argv[0])))
            {
                continue;
            }

            /* too many! */
            if (n_found >= maxv - 2)
            {
                cmdv[n_found++] = "...";
                break;
            }
            cmdv[n_found++] = gpCmdTable[i]->name;
        }
    }
    cmdv[n_found] = NULL;
    return n_found;
}

/*! *********************************************************************************
* \brief  print possible matchs
*
* \param[in]  banner     A string to be displayed before the list
* \param[in]  leader     The line leader string
* \param[in]  sep        The matchs separator string
* \param[in]  linemax    The maximum length of a line
* \param[in]  argv       Table with possible matchs
* \param[in]  num        The number of matchs
*
********************************************************************************** */
static void print_argv(char * banner, char * leader, char * sep, uint8_t linemax, char * argv[], uint8_t num)
{
    uint8_t ll = leader != NULL ? strlen(leader) : 0;
    uint8_t sl = sep != NULL ? strlen(sep) : 0;
    uint8_t len, i;

    if (banner)
    {
        SHELL_NEWLINE();
        shell_write((char*)banner);
    }
    i = linemax;    /* force leader and newline */
    while ((*argv != NULL) && (num--))
    {
        len = strlen(*argv) + sl;
        if (i + len >= linemax)
        {
            SHELL_NEWLINE();
            if (leader)
            {
                shell_writeN((char*)leader, ll);
            }
            i = ll - sl;
        }
        else if (sep)
        {
            shell_writeN((char*)sep, sl);
        }
        shell_write(*argv++);
        i += len;
    }
    SHELL_NEWLINE();
}

/*! *********************************************************************************
* \brief  find the number of common characters of matchs.
*
* \param[in]  argv       Table with possible matchs
* \param[in]  num        The number of matchs
*
* \return  uint8_t  The number of common characters
*
********************************************************************************** */
static uint8_t find_common_prefix(char * argv[], uint8_t num)
{
    uint8_t i, len;
    char * anchor, *s, *t;

    if (*argv == NULL)
    {
        return 0;
    }
    /* begin with max */
    anchor = argv[0];
    len = strlen(anchor);
    while ((num--) && (*argv != NULL))
    {
        t = *argv++;
        s = anchor;
        for (i = 0; i < len; i++)
        {
            t++;s++;
            if (*t != *s)
            {
                break;
            }       
        }
        len = s - anchor;
    }
    return len;
}

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief  auto complete interface function.
*
* \param[in]  prompt   The prompt string
* \param[in]  buf      The current command string
* \param[in]  np       Table with possible matchs
* \param[in]  colp     The number of matchs
*
* \return  int8_t      status: 0 - success; 1 - error
*
********************************************************************************** */
 /*!
 * @brief auto complete interface function.
 */
int8_t cmd_auto_complete(char * prompt, char * buf, uint8_t * np, uint8_t * colp)
{
    char *tmp_buf;
    uint8_t n = *np, col = *colp;
    char * argv[SHELL_MAX_ARGS + 1];        /* NULL terminated  */
    char * cmdv[10];
    char * s, * t;
    char *sep;
    int i, j, k, len, seplen, argc;
    uint8_t cnt;
    char last_char;

    cnt = strlen(buf);
    if (cnt >= 1)
    {
        last_char = buf[cnt - 1];
    }
    else
    {
        last_char = '\0';
    }
    /* copy to secondary buffer which will be affected */
    tmp_buf = MEM_BufferAlloc(SHELL_CB_SIZE+1);
    if(!tmp_buf)
        return 1;

    strcpy(tmp_buf, buf);
    /* separate into argv */
    argc = make_argv(tmp_buf, NumberOfElements(argv), argv);
    /* do the completion and return the possible completions */
    i = complete_cmdv(argc, argv, last_char, NumberOfElements(cmdv), cmdv);
    /* no match; bell and out */
    if (i == 0)
    {
        MEM_BufferFree(tmp_buf);
        if (argc > 1)   /* allow tab for non command */
        {
            return 0;
        }
        SHELL_BEEP();
        return 1;
    }

    s = NULL;
    len = 0;
    sep = NULL;
    seplen = 0;

    if (i == 1)
    { /* one match; perfect */
        k = strlen(argv[argc - 1]);
        s = cmdv[0] + k;
        len = strlen(s);
        sep = " ";
        seplen = 1;
    }
    else if ((i > 1) && ((j = find_common_prefix(cmdv, i)) != 0))
    {   /* more */
        k = strlen(argv[argc - 1]);
        j -= k;
        if (j > 0)
        {
            s = cmdv[0] + k;
            len = j;
        } 
    }

    if (s != NULL)
    {
        k = len + seplen;
        /* make sure it fits */
        if (n + k >= SHELL_CB_SIZE - 2) 
        {
            SHELL_BEEP();
            MEM_BufferFree(tmp_buf);
            return 1;
        }
        t = buf + cnt;
        for (i = 0; i < len; i++)
        {
          *t++ = *s++;
        }
        
        if (sep != NULL)
        {
            for (i = 0; i < seplen; i++)
            {
                *t++ = sep[i];
            }   
        }
        *t = '\0';
        n += k;
        col += k;
        shell_write(t - k);
        if (sep == NULL)
        {
            SHELL_BEEP(); 
        }
        *np = n;
        *colp = col;
    }
    else
    {
        print_argv(NULL , "  ", " ", 78, cmdv, i);
        shell_write((char*)prompt);
        shell_write(buf);
    }

    MEM_BufferFree(tmp_buf);
    return 1;
}

#endif
/*******************************************************************************
 * EOF
 ******************************************************************************/
