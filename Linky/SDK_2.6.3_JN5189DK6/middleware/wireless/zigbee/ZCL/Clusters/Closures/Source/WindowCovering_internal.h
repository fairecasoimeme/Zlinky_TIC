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
 * MODULE:             Window Covering Cluster
 *
 * COMPONENT:          cluster_window_covering_internal.h
 *
 * DESCRIPTION:        The internal API for the Window Covering Cluster
 *
 *****************************************************************************/
#ifndef  CLUSTER_WINDOW_COVERING_INTERNAL_H_
#define  CLUSTER_WINDOW_COVERING_INTERNAL_H_

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
#include "zcl.h"
#include "WindowCovering.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC teZCL_Status eCLD_WindowCoveringCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

#ifdef WINDOW_COVERING_SERVER
PUBLIC teZCL_Status eCLD_WindowCoveringCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber);

#ifdef CLD_WC_CMD_GO_TO_LIFT_VALUE
PUBLIC teZCL_Status eCLD_WindowCoveringCommandGoToLiftValueReceive(
    ZPS_tsAfEvent                             *pZPSevent,
    uint8                                     *pu8TransactionSequenceNumber,
    tsCLD_WindowCovering_GoToLiftValuePayload *psPayload);
#endif

#ifdef CLD_WC_CMD_GO_TO_LIFT_PERCENTAGE
PUBLIC teZCL_Status eCLD_WindowCoveringCommandGoToLiftPercentageReceive(
    ZPS_tsAfEvent                                  *pZPSevent,
    uint8                                          *pu8TransactionSequenceNumber,
    tsCLD_WindowCovering_GoToLiftPercentagePayload *psPayload);
#endif

#ifdef CLD_WC_CMD_GO_TO_TILT_VALUE
PUBLIC teZCL_Status eCLD_WindowCoveringCommandGoToTiltValueReceive(
    ZPS_tsAfEvent                             *pZPSevent,
    uint8                                     *pu8TransactionSequenceNumber,
    tsCLD_WindowCovering_GoToTiltValuePayload *psPayload);
#endif

#ifdef CLD_WC_CMD_GO_TO_TILT_PERCENTAGE
PUBLIC teZCL_Status eCLD_WindowCoveringCommandGoToTiltPercentageReceive(
    ZPS_tsAfEvent                                  *pZPSevent,
    uint8                                          *pu8TransactionSequenceNumber,
    tsCLD_WindowCovering_GoToTiltPercentagePayload *psPayload);
#endif
#endif /* WINDOW_COVERING_SERVER */

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* CLUSTER_WINDOW_COVERING_INTERNAL_H_ */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
