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
 * MODULE:             Appliance Statistics Cluster
 *
 * COMPONENT:          ApplianceStatistics_internal.h
 *
 * DESCRIPTION:        The internal API for the Appliance Statistics Cluster
 *
 *****************************************************************************/

#ifndef  APPLIANCE_STATISTICS_INTERNAL_H_INCLUDED
#define  APPLIANCE_STATISTICS_INTERNAL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"

#include "zcl.h"
#include "ApplianceStatistics.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_ApplianceStatisticsCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

#ifdef APPLIANCE_STATISTICS_SERVER
PUBLIC teZCL_Status eCLD_ASCLogRequestReceive(
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_ASC_LogRequestPayload                     *psPayload);                                        

PUBLIC teZCL_Status eCLD_ASCLogQueueRequestReceive(
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber);
#endif

#ifdef APPLIANCE_STATISTICS_CLIENT
PUBLIC teZCL_Status eCLD_ASCLogNotificationORLogResponseReceive(
                    ZPS_tsAfEvent                                               *pZPSevent,
                    uint8                                                       *pu8TransactionSequenceNumber,
                    tsCLD_ASC_LogNotificationORLogResponsePayload               *psPayload);

PUBLIC teZCL_Status eCLD_ASCLogQueueResponseORStatisticsAvailableReceive(
                    ZPS_tsAfEvent                                               *pZPSevent,
                    uint8                                                       *pu8TransactionSequenceNumber,
                    tsCLD_ASC_LogQueueResponseORStatisticsAvailablePayload       *psPayload);

#endif

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* APPLIANCE_STATISTICS_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
