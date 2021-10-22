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
 * MODULE:             Poll Control
 *
 * COMPONENT:          PollControl_internal.h
 *
 * DESCRIPTION:        The internal API for the Poll Control
 *
 *****************************************************************************/

#ifndef  POLLCONTROL_INTERNAL_H_INCLUDED
#define  POLLCONTROL_INTERNAL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"

#include "zcl.h"
#include "PollControl.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_PollControlCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

#ifdef POLL_CONTROL_CLIENT
PUBLIC teZCL_Status eCLD_PollControlCheckinReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber);
					
PUBLIC teZCL_Status eCLD_PollControlCheckinResponseSend(
                    uint8                                         u8SourceEndPointId,
                    uint8                                         u8DestinationEndPointId,
                    tsZCL_Address                                 *psDestinationAddress,
                    uint8                                         *pu8TransactionSequenceNumber,
					tsCLD_PollControl_CheckinResponsePayload	  *psPayload);					
#endif

#ifdef POLL_CONTROL_SERVER
PUBLIC teZCL_Status eCLD_PollControlCheckinResponseReceive(
                    ZPS_tsAfEvent               				*pZPSevent,
                    uint8                       				*pu8TransactionSequenceNumber,
					tsCLD_PollControl_CheckinResponsePayload    *psPayload);
					
PUBLIC teZCL_Status eCLD_PollControlFastPollStopReceive(
                    ZPS_tsAfEvent               				*pZPSevent,
                    uint8                       				*pu8TransactionSequenceNumber);
					
#ifdef CLD_POLL_CONTROL_CMD_SET_LONG_POLL_INTERVAL
PUBLIC teZCL_Status eCLD_PollControlSetLongPollIntervalReceive(
                    ZPS_tsAfEvent               					*pZPSevent,
                    uint8                       					*pu8TransactionSequenceNumber,
					tsCLD_PollControl_SetLongPollIntervalPayload    *psPayload);
#endif

#ifdef CLD_POLL_CONTROL_CMD_SET_SHORT_POLL_INTERVAL
PUBLIC teZCL_Status eCLD_PollControlSetShortPollIntervalReceive(
                    ZPS_tsAfEvent               					*pZPSevent,
                    uint8                       					*pu8TransactionSequenceNumber,
					tsCLD_PollControl_SetShortPollIntervalPayload   *psPayload);
#endif
				
#endif
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* POLLCONTROL_INTERNAL_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
