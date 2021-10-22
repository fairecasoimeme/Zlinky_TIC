/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef  PDMIDS_H_INCLUDED
#define  PDMIDS_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif


/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "EmbeddedTypes.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define PDM_ID_APP_COORD                             (0x6)
#define PDM_ID_APP_ROUTER                            (0x7)
#define PDM_ID_APP_END_DEVICE                        (0x8)
#define PDM_ID_APP_REPORTS                           (0x9)

#define PDM_ID_OTA_DATA                           (0xA102)
#define PDM_ID_APP_CLD_GP_TRANS_TABLE             (0xA103)
#define PDM_ID_APP_CLD_GP_SINK_PROXY_TABLE        (0xA104)
#define PDM_ID_POWER_ON_COUNTER                   (0xA106)
#define PDM_ID_OTA_APP                            (0xA109) /* Application OTA data */


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif /* PDMIDS_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
