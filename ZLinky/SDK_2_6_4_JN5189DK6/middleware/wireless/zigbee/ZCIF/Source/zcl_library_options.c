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
 * MODULE:             Smart Energy
 *
 * COMPONENT:          zcl_library_options.c
 *
 * DESCRIPTION:       Core ZCL functionality that is controllable from zcl_options.h
 *                    This file is built with user application code rather than in the
 *                    ZCL library .a file.
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"
#include "zps_nwk_pub.h"
#include "zcl_options.h"
#include "zcl_internal_library_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifdef ZCL_CONFIGURE_ATTRIBUTE_REPORTING_SERVER_SUPPORTED
#ifndef ZCL_ATTRIBUTE_REPORTING_SERVER_SUPPORTED
#error "Must define ZCL_ATTRIBUTE_REPORTING_SERVER_SUPPORTED when ZCL_CONFIGURE_ATTRIBUTE_REPORTING_SERVER_SUPPORTED is defined"
#endif
#endif

#ifdef ZCL_READ_ATTRIBUTE_REPORTING_CONFIGURATION_SERVER_SUPPORTED
#ifndef ZCL_ATTRIBUTE_REPORTING_SERVER_SUPPORTED
#error "Must define ZCL_ATTRIBUTE_REPORTING_SERVER_SUPPORTED when ZCL_READ_ATTRIBUTE_REPORTING_CONFIGURATION_SERVER_SUPPORTED is defined"
#endif
#endif


#ifdef ZCL_CONFIGURE_ATTRIBUTE_REPORTING_CLIENT_SUPPORTED
#ifndef ZCL_ATTRIBUTE_REPORTING_CLIENT_SUPPORTED
#error "Must define ZCL_ATTRIBUTE_REPORTING_CLIENT_SUPPORTED when ZCL_CONFIGURE_ATTRIBUTE_REPORTING_CLIENT_SUPPORTED is defined"
#endif
#endif

#ifdef ZCL_READ_ATTRIBUTE_REPORTING_CONFIGURATION_CLIENT_SUPPORTED
#ifndef ZCL_ATTRIBUTE_REPORTING_CLIENT_SUPPORTED
#error "Must define ZCL_ATTRIBUTE_REPORTING_CLIENT_SUPPORTED when ZCL_READ_ATTRIBUTE_REPORTING_CONFIGURATION_CLIENT_SUPPORTED is defined"
#endif
#endif

#if ( (defined ZCL_COMMAND_GENERATED_DISCOVERY_CLIENT_SUPPORTED) || (defined ZCL_COMMAND_GENERATED_DISCOVERY_SERVER_SUPPORTED) || \
      (defined ZCL_COMMAND_RECEIVED_DISCOVERY_CLIENT_SUPPORTED ) || (defined ZCL_COMMAND_RECEIVED_DISCOVERY_SERVER_SUPPORTED) )
#ifndef ZCL_COMMAND_DISCOVERY_SUPPORTED
#error "Must Define ZCL_COMMAND_DISCOVERY_SUPPORTED in your zcl_options.h File"
#endif
#endif    




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
 ** NAME:       vZCL_HandleEntireProfileCommand
 **
 ** DESCRIPTION:
 ** Handles commands that act accross an entire profile. Built with application so that
 ** app options in zcl_options.h control the build.
 **
 ** PARAMETERS:               Name                    Usage
 ** uint8                     u8CommandIdentifier     ZCL command ID
 ** ZPS_tsAfEvent             *pZPSevent              Event to process
 ** tsZCL_EndPointDefinition  *psZCL_EndPointDefinition  Context of the endpoint
 ** tsZCL_ClusterInstance     *psClusterInstance      Context of the cluster
 ** RETURN:
 ** None
 **
 ****************************************************************************/

PUBLIC void vZCL_HandleEntireProfileCommand(uint8                       u8CommandIdentifier,
                                            ZPS_tsAfEvent               *pZPSevent,
                                            tsZCL_EndPointDefinition    *psZCL_EndPointDefinition,
                                            tsZCL_ClusterInstance       *psClusterInstance)

{
    // pass buffer to specific ZCL handlers
    switch(u8CommandIdentifier)
    {

#ifdef ZCL_ATTRIBUTE_READ_SERVER_SUPPORTED
        case(E_ZCL_READ_ATTRIBUTES):
        {
            vZCL_HandleAttributesReadRequest(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif

#ifdef ZCL_ATTRIBUTE_READ_CLIENT_SUPPORTED
        case(E_ZCL_READ_ATTRIBUTES_RESPONSE):
        {
            vZCL_HandleAttributesReadResponse(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif

#ifdef ZCL_ATTRIBUTE_REPORTING_CLIENT_SUPPORTED
        case(E_ZCL_REPORT_ATTRIBUTES):
        {
            vZCL_HandleAttributesReadResponse(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif

#ifdef ZCL_ATTRIBUTE_WRITE_SERVER_SUPPORTED
        case(E_ZCL_WRITE_ATTRIBUTES):
        {
            vZCL_HandleAttributesWriteRequest(pZPSevent, psZCL_EndPointDefinition, psClusterInstance, FALSE, FALSE);
            break;
        }
        case(E_ZCL_WRITE_ATTRIBUTES_UNDIVIDED):
        {
            vZCL_HandleAttributesWriteRequest(pZPSevent, psZCL_EndPointDefinition, psClusterInstance, TRUE, FALSE);
            break;
        }
        case(E_ZCL_WRITE_ATTRIBUTES_NO_RESPONSE):
        {
            vZCL_HandleAttributesWriteRequest(pZPSevent, psZCL_EndPointDefinition, psClusterInstance, FALSE, TRUE);
            break;
        }
#endif

#ifdef ZCL_ATTRIBUTE_WRITE_CLIENT_SUPPORTED
        case(E_ZCL_WRITE_ATTRIBUTES_RESPONSE):
        {
            vZCL_HandleAttributesWriteResponse(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif

#ifdef ZCL_CONFIGURE_ATTRIBUTE_REPORTING_SERVER_SUPPORTED
        case(E_ZCL_CONFIGURE_REPORTING):
        {
            vZCL_HandleConfigureReportingCommand(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif

#ifdef ZCL_CONFIGURE_ATTRIBUTE_REPORTING_CLIENT_SUPPORTED
        case(E_ZCL_CONFIGURE_REPORTING_RESPONSE):
        {
            vZCL_HandleConfigureReportingResponse(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif

#ifdef ZCL_READ_ATTRIBUTE_REPORTING_CONFIGURATION_SERVER_SUPPORTED
        case(E_ZCL_READ_REPORTING_CONFIGURATION):
        {
            vZCL_HandleReadReportingConfigurationCommand(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif

#ifdef ZCL_READ_ATTRIBUTE_REPORTING_CONFIGURATION_CLIENT_SUPPORTED
        case(E_ZCL_READ_REPORTING_CONFIGURATION_RESPONSE):
        {
            vZCL_HandleReadReportingConfigurationResponse(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif

#ifdef ZCL_ATTRIBUTE_DISCOVERY_SERVER_SUPPORTED

        case(E_ZCL_DISCOVER_ATTRIBUTES):
        {
            vZCL_HandleAttributesDiscoverRequest(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif

#ifdef ZCL_ATTRIBUTE_DISCOVERY_CLIENT_SUPPORTED

        case(E_ZCL_DISCOVER_ATTRIBUTES_RESPONSE):
        {
            vZCL_HandleDiscoverAttributesResponse(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif

#ifdef ZCL_COMMAND_RECEIVED_DISCOVERY_SERVER_SUPPORTED

        case(E_ZCL_DISCOVER_COMMANDS_RECEIVED):
        {
            vZCL_HandleDiscoverCommandsReceivedRequest(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif

#ifdef ZCL_COMMAND_RECEIVED_DISCOVERY_CLIENT_SUPPORTED

        case(E_ZCL_DISCOVER_COMMANDS_RECEIVED_RESPONSE):
        {
            vZCL_HandleDiscoverCommandsReceivedResponse(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif
#ifdef ZCL_COMMAND_GENERATED_DISCOVERY_SERVER_SUPPORTED

        case(E_ZCL_DISCOVER_COMMANDS_GENERATED):
        {
            vZCL_HandleDiscoverCommandsGeneratedRequest(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif
#ifdef ZCL_COMMAND_GENERATED_DISCOVERY_CLIENT_SUPPORTED

        case(E_ZCL_DISCOVER_COMMANDS_GENERATED_RESPONSE):
        {
            vZCL_HandleDiscoverCommandsGeneratedResponse(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif

#ifdef ZCL_ATTRIBUTE_DISCOVERY_EXTENDED_SERVER_SUPPORTED

        case(E_ZCL_DISCOVER_ATTRIBUTES_EXTENDED):
        {
            vZCL_HandleAttributesDiscoverExtendedRequest(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif
#ifdef ZCL_ATTRIBUTE_DISCOVERY_EXTENDED_CLIENT_SUPPORTED

        case(E_ZCL_DISCOVER_ATTRIBUTES_EXTENDED_RESPONSE):
        {
            vZCL_HandleDiscoverAttributesExtendedResponse(pZPSevent, psZCL_EndPointDefinition, psClusterInstance);
            break;
        }
#endif
        default:
        {
            // Unrecognised command that should act accross all profiles
            eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_UNSUP_GENERAL_COMMAND);
        }
    }
}


/****************************************************************************
 **
 ** NAME:       eZCL_CreateOptionalManagers
 **
 ** DESCRIPTION:
 ** Creates managers that are controlled by the app options in zcl_options.h.
 ** If further optional managers are needed, their parameters can be added to the
 ** list of parameters for this function.
 **
 ** PARAMETERS:         Name                                    Usage
 ** uint8               u8NumberOfReports                       Number of reports
 ** uint16              u16SystemMinimumReportingInterval       Min System Reporting Interval
 ** uint16              u16SystemMaximumReportingInterval       Max System Reporting Interval
 **
 ** RETURN:
 ** teZCL_Status E_ZCL_SUCCESS if all managers created OK or a manager specific
 ** error on failulre.
 **
 ****************************************************************************/
PUBLIC teZCL_Status eZCL_CreateOptionalManagers(
            uint8       u8NumberOfReports,
            uint16      u16SystemMinimumReportingInterval,
            uint16      u16SystemMaximumReportingInterval)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
#ifdef ZCL_ATTRIBUTE_REPORTING_SERVER_SUPPORTED
    // Create Reports
    eStatus  = eZCL_CreateReportManager(u8NumberOfReports, u16SystemMinimumReportingInterval, u16SystemMaximumReportingInterval);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }
#endif
    return eStatus;
 }

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
