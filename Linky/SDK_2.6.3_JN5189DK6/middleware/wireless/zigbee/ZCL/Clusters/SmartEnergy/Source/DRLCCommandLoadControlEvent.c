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


/****************************************************************************
 *
 * MODULE:             Zigbee Demand Response And Load Control Cluster
 *
 * DESCRIPTION:
 * Load control event code
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>
#include "zcl.h"
#include "zcl_customcommand.h"
#include "DRLC.h"
#include "DRLC_internal.h"
#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eSE_LoadControlEventSend
 **
 ** DESCRIPTION:
 ** Sends Load Control Event Command
 **
 ** PARAMETERS:                 Name                            Usage
 ** uint8                       u8SourceEndPointId              Source EP Id
 ** uint8                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address               *psDestinationAddress           Destination Address
 ** tsSE_DRLCLoadControlEvent   *psLoadControlEvent             Load Control Event Structure
 ** bool_t                      bSendWithTimeNow                send with zero in time slot
 ** uint8                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCLoadControlEventSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    tsSE_DRLCLoadControlEvent   *psLoadControlEvent,
                    bool_t                      bSendWithTimeNow,
                    uint8                       *pu8TransactionSequenceNumber)
{

    uint32 u32StartTime;

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
    {1, E_ZCL_UINT32,   &psLoadControlEvent->u32IssuerId},
    {1, E_ZCL_UINT16,   &psLoadControlEvent->u16DeviceClass},
    {1, E_ZCL_UINT8,    &psLoadControlEvent->u8UtilityEnrolmentGroup},
    {1, E_ZCL_UINT32,   &u32StartTime},
    {1, E_ZCL_UINT16,   &psLoadControlEvent->u16DurationInMinutes},
    {1, E_ZCL_UINT8,    &psLoadControlEvent->u8CriticalityLevel},
    {1, E_ZCL_UINT8,    &psLoadControlEvent->u8CoolingTemperatureOffset},
    {1, E_ZCL_UINT8,    &psLoadControlEvent->u8HeatingTemperatureOffset},
    {1, E_ZCL_UINT16,   &psLoadControlEvent->u16CoolingTemperatureSetPoint},
    {1, E_ZCL_UINT16,   &psLoadControlEvent->u16HeatingTemperatureSetPoint},
    {1, E_ZCL_UINT8,    &psLoadControlEvent->u8AverageLoadAdjustmentSetPoint},
    {1, E_ZCL_UINT8,    &psLoadControlEvent->u8DutyCycle},
    {1, E_ZCL_UINT8,    &psLoadControlEvent->u8EventControl},
                                              };

    /*if(bSendWithTimeNow==TRUE)
    {
        psLoadControlEvent->u32StartTime = 0;
    }*/

    if(bSendWithTimeNow)
    {
        u32StartTime = 0;
    }
    else
    {
        u32StartTime = psLoadControlEvent->u32StartTime;
    }

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                              u8DestinationEndPointId,
                              psDestinationAddress,
                              SE_CLUSTER_ID_DEMAND_RESPONSE_AND_LOAD_CONTROL,
                              TRUE,
                              SE_DRLC_LOAD_CONTROL_EVENT,
                              pu8TransactionSequenceNumber,
                              asPayloadDefinition,
                              FALSE,
                              0,
                              sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}

/****************************************************************************
 **
 ** NAME:       eSE_DRLCLoadControlEventReceive
 **
 ** DESCRIPTION:
 ** Gets Load Control Event Command Field Values
 **
 ** PARAMETERS:                 Name                            Usage
 ** ZPS_tsAfEvent               *pZPSevent                      Zigbee stack event structure
 ** tsZCL_EndPointDefinition    *psEndPointDefinition           EP structure
 ** tsSE_DRLCLoadControlEvent   *psLoadControlEvent             Load Control Event Structure
 ** uint8                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCLoadControlEventReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsSE_DRLCLoadControlEvent   *psLoadControlEvent,
                    uint8                       *pu8TransactionSequenceNumber)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_UINT32,   &psLoadControlEvent->u32IssuerId},
            {1, &u16ActualQuantity, E_ZCL_UINT16,   &psLoadControlEvent->u16DeviceClass},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psLoadControlEvent->u8UtilityEnrolmentGroup},
            {1, &u16ActualQuantity, E_ZCL_UINT32,   &psLoadControlEvent->u32StartTime},
            {1, &u16ActualQuantity, E_ZCL_UINT16,   &psLoadControlEvent->u16DurationInMinutes},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psLoadControlEvent->u8CriticalityLevel},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psLoadControlEvent->u8CoolingTemperatureOffset},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psLoadControlEvent->u8HeatingTemperatureOffset},
            {1, &u16ActualQuantity, E_ZCL_UINT16,   &psLoadControlEvent->u16CoolingTemperatureSetPoint},
            {1, &u16ActualQuantity, E_ZCL_UINT16,   &psLoadControlEvent->u16HeatingTemperatureSetPoint},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psLoadControlEvent->u8AverageLoadAdjustmentSetPoint},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psLoadControlEvent->u8DutyCycle},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psLoadControlEvent->u8EventControl},
                                                };

    if(eZCL_CustomCommandDRLCReceive(pZPSevent,
                                 psEndPointDefinition,
                                 pu8TransactionSequenceNumber,
                                 asPayloadDefinition,
                                 sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                 E_ZCL_DISABLE_DEFAULT_RESPONSE) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    }

    return E_ZCL_SUCCESS;

}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
