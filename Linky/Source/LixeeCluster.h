/*
 * LixeeCluster.h
 *
 *  Created on: 21 avr. 2021
 *      Author: akila
 */

#ifndef LINKY_SOURCE_LIXEECLUSTER_H_
#define LINKY_SOURCE_LIXEECLUSTER_H_

#include <jendefs.h>
#include "zcl_options.h"
#include "zcl.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */
#define LIXEE_CLUSTER_ID_LINKY                        0xFF66

typedef enum
{

    E_CLD_LINKY_OPTARIF                        = 0x0000,
	E_CLD_LINKY_DEMAIN                         = 0x0001,
	E_CLD_LINKY_HHPHC                          = 0x0002,
	E_CLD_LINKY_PPOT                           = 0x0003,
	E_CLD_LINKY_PEJP                           = 0x0004,
	E_CLD_LINKY_ADPS                           = 0x0005,

	E_CLD_LINKY_PERIODIC_SEND                  = 0x0100,

} teCLD_Linky_ClusterID;;

typedef struct
{
	 tsZCL_CharacterString       sLinkyOptarif;
	 uint8                       au8LinkyOptarif[4];
	 tsZCL_CharacterString       sLinkyDemain;
	 uint8                       au8LinkyDemain[4];
	 uint8                       au8LinkyHHPHC;
	 uint8   					 au8LinkyPPOT;
	 uint8   					 au8LinkyPEJP;
	 uint16   					 au16LinkyADPS;
	 uint16						 au16LinkyPeriodicSend;

} tsCLD_Linky;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC teZCL_Status eCLD_LixeeCreateLinky(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits);

extern tsZCL_ClusterDefinition sCLD_Linky;
extern const tsZCL_AttributeDefinition asCLD_LixeeLinkyClusterAttributeDefinitions[];
#endif /* LINKY_SOURCE_LIXEECLUSTER_H_ */

