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
#include "shell.h"
#include "FunctionLib.h"
#include <string.h>

#if (SHELL_ENABLED && SHELL_MAX_HIST)
/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
#define CREAD_HIST_CHAR         ('!')
#define incIdx(i, max) {if( ++(i) >= max ) i=0; }
#define decIdx(i, max) {if( i > 0 ) i--; else i=max-1; }

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static int8_t DoHist(uint8_t argc, char * argv[]);

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static char hist_lines[SHELL_MAX_HIST][SHELL_CB_SIZE + 1]; /* Save room for NULL */
static uint16_t histAddIdx;
static uint16_t histCurIdx;

const cmd_tbl_t CommandFun_Hist = 
{
    .name = "history",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = DoHist,
#if SHELL_USE_HELP
    .usage = "Print history",
    .help = NULL,
#endif
#if SHELL_USE_AUTO_COMPLETE
    .complete = NULL
#endif
};
 
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  This function will initialize the SHELL history component
*
********************************************************************************** */
void hist_init(void)
{
    shell_register_function((cmd_tbl_t*)&CommandFun_Hist);

    for(histCurIdx=0; histCurIdx<SHELL_MAX_HIST; histCurIdx++)
    {
        hist_lines[histCurIdx][0] = '\0';
    }

    histAddIdx = 0;
    histCurIdx = SHELL_MAX_HIST;
}

/*! *********************************************************************************
* \brief  This function will add a command into the history table
*
* \param[in]  line  command string to add to history
*
********************************************************************************** */
void hist_add(char * line)
{
    if ((line[0]) && (line[0] != CREAD_HIST_CHAR))
    {
        strcpy(hist_lines[histAddIdx], line);
        incIdx(histAddIdx, SHELL_MAX_HIST);
    }
    histCurIdx = SHELL_MAX_HIST;
}


/*! *********************************************************************************
* \brief  This function will return a pointer to the next command from history
*
* \return  char*   pointer to the next entry from the hitory table
*
*
********************************************************************************** */
char * hist_next(void)
{
    uint32_t i;

    if( histCurIdx < (SHELL_MAX_HIST - 1) )
    {
        i = (histAddIdx + histCurIdx + 1) % SHELL_MAX_HIST;
        
        if( !hist_lines[i][0] )
            return NULL;

        histCurIdx++;
        return hist_lines[i];
    }
    
    return NULL;
}

/*! *********************************************************************************
* \brief  This function will return a pointer to the previous command from history
*
* \return  char*   pointer to the previous entry from the hitory table
*
*
********************************************************************************** */
char * hist_prev(void)
{
    uint32_t i;

    if( histCurIdx )
    {
        i = (histAddIdx + histCurIdx - 1) % SHELL_MAX_HIST;

        if( !hist_lines[i][0] )
            return NULL;

        histCurIdx--;            
        return hist_lines[i];
    }
    
    return NULL;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  Will display all entries from the history 
*
* \param[in]  argc  The number of arguments
* \param[in]  argv  table with command argumens
*
* \return  int8_t  command status (command_ret_t)
*
********************************************************************************** */
static int8_t DoHist(uint8_t argc, char * argv[])
{
    uint16_t n = SHELL_MAX_HIST;
    uint16_t i = histAddIdx;

    while( n-- )
    {
        if( hist_lines[i][0] )
        {
            shell_write("    ");
            shell_write(hist_lines[i]);
            SHELL_NEWLINE();
        }
        incIdx(i, SHELL_MAX_HIST);
    }

    return CMD_RET_SUCCESS;
}
#endif

 /*******************************************************************************
 * EOF
 ******************************************************************************/