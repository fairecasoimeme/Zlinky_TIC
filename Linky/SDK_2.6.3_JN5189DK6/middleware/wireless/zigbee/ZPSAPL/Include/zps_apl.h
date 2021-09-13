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
 * MODULE:      Zigbee Protocol Stack Application Layer
 *
 * COMPONENT:   zps_apl.h
 *
 * DESCRIPTION: Application Layer global definitions
 *
 *****************************************************************************/

#ifndef ZPS_APL_H_
#define ZPS_APL_H_

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/*
 * general status type
 * The value returned can be from any layer of the ZigBee stack
 * Consult the relevant header file depending on the value
 * 0x80 - 0x8F : Zigbee device profile (zps_apl_zdp.h)
 * 0xA0 - 0xBF : Application support sub-layer (zps_apl_aps.h)
 * 0xC0 - 0xCF : Network layer (zps_nwk_sap.h)
 * 0xE0 - 0xFF : MAC layer (mac_sap.h)
 */
typedef uint8 ZPS_teStatus;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#endif /* ZPS_APL_H_ */
