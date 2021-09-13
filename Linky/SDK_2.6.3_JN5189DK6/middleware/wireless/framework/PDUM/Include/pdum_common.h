/*****************************************************************************
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2008, 2020 NXP
 * All rights reserved.
 *
 * MODULE:             PDU Manager
 *
 * COMPONENT:          pdum_common.h
 *
 * AUTHOR:             MRW
 *
 * DESCRIPTION:        PDU manager common definitions
 *                     Manages NPDUs
 *
 *
 *
 ****************************************************************************/

#ifndef PDUM_COMMON_H_
#define PDUM_COMMON_H_

#include "jendefs.h"

/* ensure pdum_common.h is only included from pdum_nwk or pdum_app headers */
#if !(defined(PDUM_NWK_H_) || defined(PDUM_APP_H_))
#error "pdum_common.h should not be directly included. Include either pdum_nwk.h or pdum_app.h"
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define PDUM_INVALID_HANDLE     NULL

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Status return codes */
typedef enum {
    PDUM_E_OK,
    PDUM_E_INVALID_HANDLE,
    PDUM_E_BAD_DIRECTION,
    PDUM_E_BAD_PARAM,
    PDUM_E_NPDUS_EXHAUSTED,
    PDUM_E_NPDU_TOO_BIG,
    PDUM_E_NPDU_ALREADY_FREE,
    PDUM_E_APDU_INSTANCE_ALREADY_FREE,
    PDUM_E_APDU_INSTANCE_TOO_BIG,
    PDUM_E_INTERNAL_ERROR
} PDUM_teStatus;

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
typedef void (*pfCallbackNotificationApduType)(uint8);
typedef void (*pfCallbackNotificationType)(void);
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC uint16 PDUM_u16SizeNBO(const char *szFormat);
PUBLIC void ZPS_vNwkRegisterNpduNotification ( void* pvFnCallback );
PUBLIC void ZPS_vNwkRegisterApduNotification ( void* pvFnCallback );


#endif /*PDUMAN_H_*/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
