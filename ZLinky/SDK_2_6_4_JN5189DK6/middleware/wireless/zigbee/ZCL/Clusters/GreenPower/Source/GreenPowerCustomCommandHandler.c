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
 * MODULE:             Green power Custom command handler
 *
 * COMPONENT:          GreenPowerCustomCommandHandler.c
 *
 * DESCRIPTION:        handles green power custom commands
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "zcl.h"
#include "zcl_options.h"
#include "GreenPower.h"
#include "GreenPower_internal.h"
#include "dbg.h"
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
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eGP_GreenPowerCommandHandler
 **
 ** DESCRIPTION:
 ** Green Power command handler
 **
 ** PARAMETERS:                    Name                           Usage
 ** ZPS_tsAfEvent               *pZPSevent                  ZPS event
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       endpoint definition
 ** tsZCL_ClusterInstance       *psClusterInstance          cluster instance
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC teZCL_Status eGP_GreenPowerCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{
	tsZCL_HeaderParams sZCL_HeaderParams;
	uint16 u16HeaderSize;
    teZCL_Status eStatus = E_ZCL_SUCCESS;

    // further error checking can only be done once we have interrogated the ZCL payload
    u16HeaderSize =     u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    switch (psClusterInstance->bIsServer)
    {
        case(FALSE):
        {
           /* as per spec A ZGP infrastructure device shall silently drop any received
              ZGP command it does not support.It shall not send the ZCL Default Response command.*/
            switch(sZCL_HeaderParams.u8CommandIdentifier)
            {
            case E_GP_ZGP_SINK_TABLE_RESPONSE:
            	eStatus = eGP_HandleSinkTableResponse(pZPSevent, psEndPointDefinition, psClusterInstance, u16HeaderSize);
            	break;
            case E_GP_ZGP_PROXY_TABLE_REQUEST:
            	eStatus = eGP_HandleProxyTableRequest(pZPSevent, psEndPointDefinition, psClusterInstance, u16HeaderSize);
            	break;

                case E_GP_ZGP_PAIRING:
                        eStatus = eGP_HandleZgpPairing(pZPSevent, psEndPointDefinition, psClusterInstance, u16HeaderSize);
                break;

                case E_GP_ZGP_PROXY_COMMOSSIONING_MODE:
#ifndef GP_DISABLE_PROXY_COMMISSION_MODE_CMD
                     eStatus = eGP_HandleProxyCommissioningMode(pZPSevent, psEndPointDefinition, psClusterInstance, u16HeaderSize);
#else
                     eStatus = E_ZCL_SUCCESS;
#endif
                break;

                case E_GP_ZGP_RESPONSE:
#ifndef GP_DISABLE_ZGP_RESPONSE_CMD

                    eStatus = eGP_HandleZgpReponse(pZPSevent, psEndPointDefinition, psClusterInstance, u16HeaderSize);;

#endif
                    break;

                case E_GP_ZGP_TRANSLATION_TABLE_RESPONSE:
#ifndef GP_DISABLE_TRANSLATION_TABLE_RSP_CMD
#if((defined GP_COMBO_BASIC_DEVICE) || (defined GP_TEST_HARNESS))
                    eStatus = eGP_HandleZgpTranslationResponse(pZPSevent, psEndPointDefinition, psClusterInstance, u16HeaderSize);
#else
                    eStatus = E_ZCL_SUCCESS;
#endif
#endif
                break;
                case E_GP_ZGP_NOTIFICATION:
                        eStatus = eGP_HandleZgpNotification(pZPSevent, psEndPointDefinition, psClusterInstance, u16HeaderSize);

                    break;
                default:
                    eStatus = E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR;
                break;
            }
        }
        break;
        case(TRUE):
        {  /* as per spec A ZGP infrastructure device shall silently drop any received
              ZGP command it does not support.It shall not send the ZCL Default Response command.*/
            switch(sZCL_HeaderParams.u8CommandIdentifier)
            {
#ifdef GP_COMBO_BASIC_DEVICE
            	case E_GP_ZGP_SINK_TABLE_REQUEST:
            		eStatus = eGP_HandleSinkTableRequest(pZPSevent, psEndPointDefinition, psClusterInstance, u16HeaderSize);
            	break;
#endif
            	case E_GP_ZGP_PROXY_TABLE_RESPONSE:
            		eStatus = eGP_HandleProxyTableResponse(pZPSevent, psEndPointDefinition, psClusterInstance, u16HeaderSize);
            	break;

                case E_GP_ZGP_NOTIFICATION:
                        eStatus = eGP_HandleZgpNotification(pZPSevent, psEndPointDefinition, psClusterInstance, u16HeaderSize);

                    break;

                case E_GP_ZGP_PAIRING_SEARCH:
#ifndef GP_DISABLE_PAIRING_SEARCH_CMD
#ifdef GP_COMBO_BASIC_DEVICE
                        eStatus = eGP_HandleZgpPairingSearch(pZPSevent, psEndPointDefinition, psClusterInstance, u16HeaderSize);
#else
                        eStatus = E_ZCL_SUCCESS;
#endif
#endif
                    break;

                case E_GP_ZGP_TUNNELING_STOP:
                    eStatus = E_ZCL_SUCCESS;
                    break;

                case E_GP_ZGP_COMMOSSIONING_NOTIFICATION:

#ifndef GP_DISABLE_COMMISSION_NOTIFICATION_CMD
                       eStatus = eGP_HandleZgpCommissioningNotification(pZPSevent, psEndPointDefinition, psClusterInstance, u16HeaderSize);

#else
                        eStatus = E_ZCL_SUCCESS;
#endif
                    break;

                case E_GP_ZGP_TRANSLATION_TABLE_UPDATE:

#ifdef GP_COMBO_BASIC_DEVICE
                        eStatus = eGP_HandleZgpTranslationTableUpdate(pZPSevent, psEndPointDefinition, psClusterInstance, u16HeaderSize);
#else
                        eStatus = E_ZCL_SUCCESS;
#endif
                    break;

                case E_GP_ZGP_TRANSLATION_TABLE_REQUEST:
#ifndef GP_DISABLE_TRANSLATION_TABLE_REQ_CMD
#ifdef GP_COMBO_BASIC_DEVICE
                        eStatus = eGP_HandleZgpTranslationRequest(pZPSevent, psEndPointDefinition, psClusterInstance, u16HeaderSize);
#else
                        eStatus = E_ZCL_SUCCESS;
#endif
#endif
                    break;
                case E_GP_ZGP_PAIRING_CONFIGURATION:

#ifdef GP_COMBO_BASIC_DEVICE
                        eStatus = eGP_HandleZgpPairingConfig(pZPSevent, psEndPointDefinition, psClusterInstance, u16HeaderSize);
#else
                       eStatus = E_ZCL_SUCCESS;
#endif
                    break;
                default:

                   return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
                break;

            }
        }
        break;
        default:
            eStatus = E_ZCL_FAIL;
            break;
    }

    return eStatus;
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/


/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
