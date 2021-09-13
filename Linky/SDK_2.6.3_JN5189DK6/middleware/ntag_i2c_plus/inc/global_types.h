/*
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _GLOBAL_TYPES_H_
#define _GLOBAL_TYPES_H_

/***********************************************************************/
/* INCLUDES                                                            */
/***********************************************************************/
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif /* HAVE_STDBOOL_H */

#ifdef CPU_JN518X
#include "EmbeddedTypes.h"
#define BOOL bool_t
#define I2C_JN_FSL
#endif

/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/
#ifdef HAVE_STDBOOL_H
#ifndef BOOL
#define BOOL bool
#endif
#endif /* HAVE_STDBOOL_H */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#if !defined(MIN)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#if !defined(NULL)
#define NULL 0
#endif

#define UINT16_BYTE_HI(word) ((word & 0xFF00) >> 8)
#define UINT16_BYTE_LO(word) ((word & 0x00FF))

/***********************************************************************/
/* TYPES                                                               */
/***********************************************************************/

/***********************************************************************/
/* GLOBAL VARIABLES                                                    */
/***********************************************************************/

/***********************************************************************/
/* GLOBAL FUNCTION PROTOTYPES                                          */
/***********************************************************************/

#endif /* _GLOBAL_TYPES_H_ */
