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
 * MODULE:             Zigbee Protocol Stack MAC Shim
 *
 * COMPONENT:          zps_mac.h
 *
 * DESCRIPTION:        MAC shim layer to provide PDU manager i/f to NWK layer
 *
 ****************************************************************************/

#ifndef _zps_mac_h_
#define _zps_mac_h_

#ifdef __cplusplus
extern "C" {
#endif

/***********************/
/**** INCLUDE FILES ****/
/***********************/

#include "jendefs.h"
#include "mac_sap.h"


/************************/
/**** MACROS/DEFINES ****/
/************************/



/**************************/
/**** TYPE DEFINITIONS ****/
/**************************/


/****************************/
/**** EXPORTED VARIABLES ****/
/****************************/


/****************************/
/**** EXPORTED FUNCTIONS ****/
/****************************/

PUBLIC void
ZPS_vNwkHandleMcpsDcfmInd(void *pvNwk,
                          MAC_DcfmIndHdr_s *psMcpsDcfmInd);




PUBLIC uint32 zps_u32GetMacVersion(void);

#ifdef __cplusplus
};
#endif

#endif /* _zps_mac_h_ */

/* End of file **************************************************************/

