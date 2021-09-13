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
 * COMPONENT:          zps_nwk_config.h
 *
 * DESCRIPTION:        ZPS NWK Configuration header
 *
 *****************************************************************************/

#ifndef _zps_nwk_config_h_
#define _zps_nwk_config_h_

#ifdef __cplusplus
extern "C" {
#endif

/***********************/
/**** INCLUDE FILES ****/
/***********************/
/************************/
/**** MACROS/DEFINES ****/
/************************/

#ifndef ZPS_NWK_OPT_IN_MAKE

/* Default feature set options */

/* Define this if direct join is needed to be supported through primitives */
#define ZPS_NWK_OPT_FS_DIRECT_JOIN
#define ZPS_NWK_OPT_FS_ED_SCAN

//#define ZPS_NWK_OPT_FS_DYNAMIC_SEC_LVL

/*
 * Define this if a parent should send a NLME-NWK-STATUS.indication to the higher layer
 * indicating an address update.
 */
//#define ZPS_NWK_OPT_PARENT_SEND_ADDR_UPDATE_IND

/*
 * Define this if a device changing its own or its ZED child's address should also
 * send a network status showing address conflict.
 *
 */
//#define ZPS_NWK_OPT_LOCAL_CHILD_ADDR_CNFL_SEND_NWK_STATUS_CMD

/*
 * Define this if sibling routers should be added to the neighbour table if heard
 */
//#define ZPS_NWK_OPT_ADD_SIBLING_ROUTERS

/*
 * Define this if PAN ID conflict processing is compiled in
 */
#define ZPS_NWK_OPT_PAN_ID_CNFL

/*
 * Define this if Rejoin discovering is done
 */


/**
 * Device capability
 */

/**
 * Hard-coded stack profile, set all parameters at compile time.
 * The alternative is to hard code all stack profiles parameters in a table, then to
 * hard code the stack profile itself for the JC and to communicate it via the beacon
 * for all devices tree order >1
 */

/**
 * Topology limits
 * These are needed to calculate the magical CSkip formula
 */

#define ZPS_NWK_OPT_FS_MAX_DEPTH          15  /**< How deep a network is */
#define ZPS_NWK_OPT_FS_MAX_CHILDREN       20  /**< How many children a ZC/ZR can have - ZigBee FS only */
#define ZPS_NWK_OPT_FS_MAX_ROUTERS        6  /**< How many of the children can be routers */

/**
 * Routing table size
 */
#define ZPS_NWK_OPT_FS_RT_SIZE            8  /**< Size of the routing table */

#endif /* ZPS_NWK_OPT_IN_MAKE */

/* Bitmap of capabilities */
#define ZPS_NWK_OPT_CAP_COORD             4      /**< Enables coordinator functionality */
#define ZPS_NWK_OPT_CAP_ROUTER            2      /**< Enables router functionality */
#define ZPS_NWK_OPT_CAP_END_DEVICE        1      /**< Enables end device functionality */
#define ZPS_NWK_OPT_CAP_CRE               (ZPS_NWK_OPT_CAP_COORD | ZPS_NWK_OPT_CAP_ROUTER | ZPS_NWK_OPT_CAP_END_DEVICE)
#define ZPS_NWK_OPT_CAP_CR                (ZPS_NWK_OPT_CAP_COORD | ZPS_NWK_OPT_CAP_ROUTER)
#define ZPS_NWK_OPT_CAP_RE                (ZPS_NWK_OPT_CAP_ROUTER | ZPS_NWK_OPT_CAP_END_DEVICE)
#define ZPS_NWK_OPT_CAP_E                 ZPS_NWK_OPT_CAP_END_DEVICE

#ifndef ZPS_NWK_OPT_CAPABILITY
#define ZPS_NWK_OPT_CAPABILITY            (ZPS_NWK_OPT_CAP_CRE)
//#define ZPS_NWK_OPT_CAPABILITY            (ZPS_NWK_OPT_CAP_END_DEVICE)
#endif

/**
 * @name NIB table sizes
 * @ingroup g_zps_nwk_nib
 * Suggested table sizes in the NIB
 * @{
 */
#define ZPS_NWK_OPT_NUM_DISC_NT_ENTRIES    16
#define ZPS_NWK_OPT_NUM_ACTV_NT_ENTRIES    (ZPS_NWK_OPT_FS_MAX_CHILDREN + 1) /* Could be more if mesh */
#define ZPS_NWK_OPT_NUM_RT_DISC_ENTRIES    16
#define ZPS_NWK_OPT_NUM_RT_ENTRIES         16
#define ZPS_NWK_OPT_NUM_BTT_ENTRIES        9 /* according to feature set */
#define ZPS_NWK_OPT_NUM_RCT_ENTRIES        4 /* Config */
#define ZPS_NWK_OPT_NUM_ADDR_MAP_ENTRIES   16
#define ZPS_NWK_OPT_NUM_IN_FC_SETS         ZPS_NWK_OPT_NUM_ACTV_NT_ENTRIES /* Equal to Number of NT Entries */
#define ZPS_NWK_OPT_NUM_SEC_MATERIAL_SETS  2 /* according to feature set */

/**************************/
/**** TYPE DEFINITIONS ****/
/**************************/

/****************************/
/**** EXPORTED VARIABLES ****/
/****************************/

/****************************/
/**** EXPORTED FUNCTIONS ****/
/****************************/

#ifdef __cplusplus
};
#endif

#endif /* _zps_nwk_config_h_ */

/* End of file **************************************************************/
