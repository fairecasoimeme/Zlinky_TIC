/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef _SHELL_H
#define _SHELL_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "shell_config.h"
#include "EmbeddedTypes.h"
#include "SerialManager.h"
#include "Messaging.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
#define SHELL_BEEP()            shell_writeN("\a",1)
#define SHELL_NEWLINE()         shell_writeN("\n\r", 2)

/* Define number of shell history commands, used to determine the size of command table */
#if SHELL_MAX_HIST
#define SHELL_HIST_CMDS 1
#else
#define SHELL_HIST_CMDS 0
#endif

/* Define number of shell help commands, used to determine the size of command table */
#if SHELL_USE_HELP
#define SHELL_HELP_CMDS 2
#else
#define SHELL_HELP_CMDS 0
#endif

/* Compute Shell internal command table size */
#define SHELL_CMD_TBL_SIZE (SHELL_MAX_COMMANDS) + \
                           (SHELL_HELP_CMDS)    + \
                           (SHELL_HIST_CMDS)

/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
********************************************************************************** */
typedef struct  
{
    char        *name;      /* Command Name         */
    uint8_t     maxargs;    /* maximum number of arguments  */
    uint8_t     repeatable; /* autorepeat allowed?      */
    int8_t      (*cmd)(uint8_t argc, char * argv[]);  /* Implementation function    */
#if SHELL_USE_HELP
    char        *usage;     /* Usage message    (short) */
    char        *help;      /* Help  message    (long)  */
#endif
#if SHELL_USE_AUTO_COMPLETE
    int8_t      (*complete)(uint8_t argc, char * argv[], char last_char, uint8_t maxv, char *cmdv[]);   /* do auto completion on the arguments */
#endif
}cmd_tbl_t;

typedef enum
{
    CMD_RET_SUCCESS  = 0,    /* 0 = Success */
    CMD_RET_FAILURE  = 1,    /* 1 = Failure */
    CMD_RET_ASYNC    = 2,    /* 2 = Success, do not refresh the prompt */
    CMD_RET_USAGE    = -1,     /* Failure, please report 'usage' error */
}command_ret_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
#if SHELL_ENABLED
extern cmd_tbl_t *gpCmdTable[];
extern void (*pfShellProcessCommand) (char * pCmd, uint16_t length);
#endif

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
#if SHELL_ENABLED
void shell_init(char* prompt);
void shell_cmd_finished(void);
void shell_refresh(void);
void shell_reset(void);
void shell_change_prompt(char* prompt, bool_t no_print);

/*
 * to be called at each wakeup from Power Down in order to restore the Serial interface
 */
void shell_reinit(void);


uint8_t shell_register_function(cmd_tbl_t * pAddress);
void shell_register_function_array(cmd_tbl_t * pAddress, uint8_t num);
uint8_t shell_unregister_function(char * name);

void shell_write(const char *pBuff);
void shell_writeN(const char *pBuff, uint16_t n);
void shell_writeDec(const uint32_t nb);
void shell_writeSignedDec(const int8_t nb);
void shell_writeHex(const uint8_t *pHex, const uint8_t len);
void shell_writeHexLe(const uint8_t *pHex, const uint8_t len);
void shell_writeBool(const bool_t boolValue);
void shell_putc(const char c);
#if SHELL_USE_PRINTF
uint16_t shell_printf(char * format,...);
#endif
cmd_tbl_t * shell_find_command( char * cmd );
uint8_t make_argv(char *s, uint8_t argvsz, char * argv[]);
char * shell_get_opt(uint8_t argc, char * argv[], char *pOption);
#if SHELL_USE_ALT_TASK
void shell_task(void);
#endif

#else /* SHELL_ENABLED */

#define shell_init(prompt)
#define shell_refresh()
#define shell_cmd_finished()
#define shell_change_prompt(prompt)
#define shell_register_function(pAddress)              0
#define shell_register_function_array(pAddress,num)
#define shell_unregister_function(name)                0
#define shell_write(pBuff)
#define shell_writeN(pBuff,n)
#define shell_writeDec(nb)
#define shell_writeSignedDec(nb)
#define shell_writeHex(pHex,len)
#define shell_writeHexLe(pHex,len)
#define shell_writeBool(boolValue)
#define shell_putc(c)
#define shell_find_command(cmd) NULL
#define make_argv(s,argvsz,argv) 0
#define shell_get_opt(argc,argv,pOption) NULL
#if SHELL_USE_PRINTF
#define shell_printf printf
#endif
#if SHELL_USE_ALT_TASK
#define shell_task()
#endif

#endif /* SHELL_ENABLED */

#ifdef __cplusplus
}
#endif

#endif  /* _SHELL_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
