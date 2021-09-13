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


/*****************************************************************************
 *
 * MODULE:
 *
 * COMPONENT:
 *
 * DESCRIPTION:
 *
 *****************************************************************************/

#ifndef ZIGBEE_PORTMACRO_H
#define ZIGBEE_PORTMACRO_H

#if (defined JN517x)
#include "portmacro_JN517x.h"
#elif (defined JN516x)
#include "portmacro_JN516x.h"
#elif (defined JN518x)
#include "portmacro_JN518x.h"
#else
#error "define the chip family"
#endif

#endif /* ZIGBEE_PORTMACRO_H */
