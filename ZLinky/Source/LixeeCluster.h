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
	E_CLD_LINKY_ADIR1                          = 0x0006,
	E_CLD_LINKY_ADIR2                          = 0x0007,
	E_CLD_LINKY_ADIR3                          = 0x0008,

	E_CLD_LINKY_PERIODIC_SEND                  = 0x0100,

	E_CLD_LINKY_STANDARD_LTARF				   = 0x0200,
	E_CLD_LINKY_STANDARD_NTARF				   = 0x0201,
	E_CLD_LINKY_STANDARD_DATE				   = 0x0202,
	E_CLD_LINKY_STANDARD_EASD01				   = 0x0203,
	E_CLD_LINKY_STANDARD_EASD02				   = 0x0204,
	E_CLD_LINKY_STANDARD_EASD03				   = 0x0205,
	E_CLD_LINKY_STANDARD_EASD04				   = 0x0206,

	E_CLD_LINKY_STANDARD_SINSTI				   = 0x0207,
	E_CLD_LINKY_STANDARD_SMAXIN				   = 0x0208,
	E_CLD_LINKY_STANDARD_SMAXIN_1			   = 0x0209,
	E_CLD_LINKY_STANDARD_CCAIN			       = 0x0210,
	E_CLD_LINKY_STANDARD_CCAIN_1			   = 0x0211,

	E_CLD_LINKY_STANDARD_SMAXSN_1   		   = 0x0212,
	E_CLD_LINKY_STANDARD_SMAXSN2_1			   = 0x0213,
	E_CLD_LINKY_STANDARD_SMAXSN3_1			   = 0x0214,

	E_CLD_LINKY_STANDARD_MSG1			   		= 0x0215,
	E_CLD_LINKY_STANDARD_MSG2			   		= 0x0216,

	E_CLD_LINKY_STANDARD_STGE			   		= 0x0217,
	E_CLD_LINKY_STANDARD_DPM1			   		= 0x0218,
	E_CLD_LINKY_STANDARD_FPM1			   		= 0x0219,
	E_CLD_LINKY_STANDARD_DPM2			   		= 0x0220,
	E_CLD_LINKY_STANDARD_FPM2			   		= 0x0221,
	E_CLD_LINKY_STANDARD_DPM3			   		= 0x0222,
	E_CLD_LINKY_STANDARD_FPM3			   		= 0x0223,

	E_CLD_LINKY_STANDARD_RELAIS			   		= 0x0224,
	E_CLD_LINKY_STANDARD_NJOURF			   		= 0x0225,
	E_CLD_LINKY_STANDARD_NJOURF1			   	= 0x0226,
	E_CLD_LINKY_STANDARD_PJOURF1			   	= 0x0227,
	E_CLD_LINKY_STANDARD_PPOINTE			   	= 0x0228,

	E_CLD_LINKY_MODE						   	= 0x0300,

} teCLD_Linky_ClusterID;

typedef struct
{
	 tsZCL_CharacterString       sLinkyOptarif;
	 uint8                       au8LinkyOptarif[16];
	 tsZCL_CharacterString       sLinkyDemain;
	 uint8                       au8LinkyDemain[4];
	 uint8                       au8LinkyHHPHC;
	 uint8   					 au8LinkyPPOT;
	 uint8   					 au8LinkyPEJP;
	 uint16   					 au16LinkyADPS;
	 uint16   					 au16LinkyADIR1;
	 uint16   					 au16LinkyADIR2;
	 uint16   					 au16LinkyADIR3;
	 uint16						 au16LinkyPeriodicSend;
	 tsZCL_CharacterString       sLinkyLTARF;
	 uint8                       au8LinkyLTARF[16];
	 uint8						 au8LinkyNTARF;
	 tsZCL_CharacterString       sLinkyDATE;
	 uint8                       au8LinkyDATE[13];
	 uint32						 u32LinkyEASD01;
	 uint32						 u32LinkyEASD02;
	 uint32						 u32LinkyEASD03;
	 uint32						 u32LinkyEASD04;
	 int16						 u16LinkySINSTI;
	 int16						 u16LinkySMAXIN;
	 int16						 u16LinkySMAXIN_1;
	 int16						 u16LinkyCCAIN;
	 int16						 u16LinkyCCAIN_1;
	 uint16						 u16LinkySMAXSN_1;
	 uint16						 u16LinkySMAXSN2_1;
	 uint16						 u16LinkySMAXSN3_1;
	 tsZCL_CharacterString       sLinkyMSG1;
	 uint8                       au8LinkyMSG1[32];
	 tsZCL_CharacterString       sLinkyMSG2;
	 uint8                       au8LinkyMSG2[16];
	 tsZCL_OctetString           sLinkySTGE;
	 uint8                       au8LinkySTGE[8];
	 uint8                       u8LinkyDPM1;
	 uint8                       u8LinkyFPM1;
	 uint8                       u8LinkyDPM2;
	 uint8                       u8LinkyFPM2;
	 uint8                       u8LinkyDPM3;
	 uint8                       u8LinkyFPM3;
	 uint16						 u16LinkyRELAIS;
	 uint8						 u8LinkyNJOURF;
	 uint8						 u8LinkyNJOURF1;
	 tsZCL_CharacterString       sLinkyPJOURF1;
	 uint8                       au8LinkyPJOURF1[48];
	 tsZCL_CharacterString       sLinkyPPOINTE;
	 uint8                       au8LinkyPPOINTE[48];
	 uint8                       au8LinkyMode;

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

