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
 * MODULE:             ZPS NWK
 *
 * COMPONENT:          zps_nwk_slist.h
 *
 * DESCRIPTION:        ZPS NWK Single Linked List
 *
 *****************************************************************************/

#ifndef _zps_nwk_slist_h_
#define _zps_nwk_slist_h_

#ifndef MODULE
#define MODULE /* no modifier implies global */
#endif

/***********************/
/**** INCLUDE FILES ****/
/***********************/

#include "jendefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************/
/**** MACROS/DEFINES ****/
/************************/

#define ZPS_NWK_SLIST_LIMBO_NODE ((zps_tsNwkSlistNode *)0xFFFFFFFF)

/**************************/
/**** TYPE DEFINITIONS ****/
/**************************/

/**
 * ZPS NWK Single linked list node
 * @ingroup g_zps_nwk_slist
 * @note
 * Based on Jeneric JennicStdLib but without the unnecessary stuff
 */
typedef struct zps_tsNwkSlistNode_tag zps_tsNwkSlistNode;
struct zps_tsNwkSlistNode_tag
{
    zps_tsNwkSlistNode *psNext;
};

/**
 * ZPS NWK Single linked list
 * @ingroup g_zps_nwk_slist
 * @note
 * Based on Jeneric JennicStdLib but without the unnecessary stuff
 */
typedef struct zps_tsNwkSlist_tag
{
    zps_tsNwkSlistNode *psHead;     /**< Head node in list (NULL if empty) */
    zps_tsNwkSlistNode *psTail;     /**< Tail node in list (NULL if empty) */
} zps_tsNwkSlist;

/****************************/
/**** IMPORTED FUNCTIONS ****/
/****************************/

/**************************/
/**** MODULE VARIABLES ****/
/**************************/

/****************************/
/**** EXPORTED FUNCTIONS ****/
/****************************/

MODULE void
zps_vNwkSlistInit(zps_tsNwkSlist *psList);

MODULE void
zps_vNwkSlistAddToHead(zps_tsNwkSlist *psList,
                       zps_tsNwkSlistNode *psNode);

MODULE void
zps_vNwkSlistAddToTail(zps_tsNwkSlist *psList,
                       zps_tsNwkSlistNode *psNode);

MODULE void
zps_vNwkSlistInsertAfter(zps_tsNwkSlist *psList,
                         zps_tsNwkSlistNode *psCurrentNode,
                         zps_tsNwkSlistNode *psNewNode);

MODULE zps_tsNwkSlistNode *
zps_psNwkSlistRemoveFromHead(zps_tsNwkSlist *psList);

MODULE zps_tsNwkSlistNode *
zps_psNwkSlistRemove(zps_tsNwkSlist *psList,
                     zps_tsNwkSlistNode *psNode);

MODULE void
zps_vNwkSlistMakeLimboNode(zps_tsNwkSlistNode *psNode);

MODULE bool_t
zps_bNwkSlistIsLimboNode(zps_tsNwkSlistNode *psNode);

#ifdef __cplusplus
};
#endif

#endif /* _mac_prv_h_ */

/* End of file **************************************************************/
