/*
 * LixeeCluster.c
 *
 *  Created on: 21 avr. 2021
 *      Author: akila
 */


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "meter_identification.h"
#include "zcl_options.h"

const tsZCL_AttributeDefinition asCLD_MeterIdentificationClusterAttributeDefinitions[] = {

		{E_CLD_MI_SOFTWAREREVISION,  (E_ZCL_AF_RD),               E_ZCL_UINT16,    (uint32)(&((tsCLD_MeterIdentification*)(0))->au16LinkyMISoftwareRevision),0},
		{E_CLD_MI_POD,               (E_ZCL_AF_RD),               E_ZCL_UINT16,    (uint32)(&((tsCLD_MeterIdentification*)(0))->au16LinkyMIPOD),0},
		{E_CLD_MI_AVAILABLEPOWER,    (E_ZCL_AF_RD),               E_ZCL_UINT16,    (uint32)(&((tsCLD_MeterIdentification*)(0))->au16LinkyMIAvailablePower),0},
		{E_CLD_MI_POWERTHRESHOLD,    (E_ZCL_AF_RD),               E_ZCL_UINT8,     (uint32)(&((tsCLD_MeterIdentification*)(0))->au8LinkyMIPowerThreshold),0},


};

tsZCL_ClusterDefinition sCLD_MeterIdentification = {
		METER_IDENTIFICATION_CLUSTER_ID,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_MeterIdentificationClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_MeterIdentificationClusterAttributeDefinitions,
        NULL
};

/****************************************************************************
 *
 * NAME:       eCLD_BasicCreateBasic
 *
 * DESCRIPTION:
 * Creates a basic cluster
 *
 * PARAMETERS:  Name                         Usage
 *              psClusterInstance            Cluster structure
 *              bIsServer                    Server/Client Flag
 *              psClusterDefinition          Cluster Definitation
 *              pvEndPointSharedStructPtr    EndPoint Shared Structure Pointer
 *              pu8AttributeControlBits      Attribute Control Bits
 * RETURN:
 * teZCL_Status
 *
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_MeterIdentificationCreateMeterIdentification(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits)
{
    #ifdef STRICT_PARAM_CHECK
        /* Parameter check */
        if((psClusterInstance==NULL) ||
           (psClusterDefinition==NULL)  )
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    /* Create an instance of a basic cluster */
    vZCL_InitializeClusterInstance(
           psClusterInstance,
           bIsServer,
           psClusterDefinition,
           pvEndPointSharedStructPtr,
           pu8AttributeControlBits,
           NULL,
           NULL);

        if(pvEndPointSharedStructPtr  != NULL)
        {

        }
    return E_ZCL_SUCCESS;

}
