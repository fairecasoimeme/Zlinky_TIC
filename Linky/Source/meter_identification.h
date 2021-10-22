/*
 * LixeeCluster.h
 *
 *  Created on: 21 avr. 2021
 *      Author: akila
 */

#ifndef LINKY_SOURCE_METERIDENTIFICATION_H_
#define LINKY_SOURCE_METERIDENTIFICATION_H_

#include <jendefs.h>
#include "zcl_options.h"
#include "zcl.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */
#define METER_IDENTIFICATION_CLUSTER_ID                      0x0B01

typedef enum
{
	E_CLD_MI_SOFTWAREREVISION                      = 0x000A,
	E_CLD_MI_POD                                   = 0x000C,
    E_CLD_MI_AVAILABLEPOWER                        = 0x000D,
	E_CLD_MI_POWERTHRESHOLD                        = 0x000E,


} teCLD_MeterIdentification_AttributeID;

typedef struct
{
	 uint8                       au8LinkyMIPowerThreshold;
	 uint16 					 au16LinkyMISoftwareRevision;
	 uint16 					 au16LinkyMIPOD;
	 uint16 					 au16LinkyMIAvailablePower;

} tsCLD_MeterIdentification;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC teZCL_Status eCLD_MICreateLinky(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits);

extern tsZCL_ClusterDefinition sCLD_MeterIdentification;
extern const tsZCL_AttributeDefinition asCLD_MeterIdentificationClusterAttributeDefinitions[];
#endif /* LINKY_SOURCE_MICLUSTER_H_ */

