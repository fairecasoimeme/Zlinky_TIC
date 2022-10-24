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
#include "LixeeCluster.h"
#include "zcl_options.h"

const tsZCL_AttributeDefinition asCLD_LixeeLinkyClusterAttributeDefinitions[] = {
        /* Optarif Linky */
        {E_CLD_LINKY_OPTARIF,             (E_ZCL_AF_RD),                   E_ZCL_CSTRING,    (uint32)(&((tsCLD_Linky*)(0))->sLinkyOptarif),0},
		{E_CLD_LINKY_DEMAIN ,             (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_CSTRING,    (uint32)(&((tsCLD_Linky*)(0))->sLinkyDemain),0},

		{E_CLD_LINKY_HHPHC,               (E_ZCL_AF_RD),                   E_ZCL_UINT8,    (uint32)(&((tsCLD_Linky*)(0))->au8LinkyHHPHC),0},
		{E_CLD_LINKY_PPOT,                (E_ZCL_AF_RD),                   E_ZCL_UINT8,    (uint32)(&((tsCLD_Linky*)(0))->au8LinkyPPOT),0},
		{E_CLD_LINKY_PEJP,                (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_UINT8,    (uint32)(&((tsCLD_Linky*)(0))->au8LinkyPEJP),0},
		{E_CLD_LINKY_ADPS,                (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_UINT16,   (uint32)(&((tsCLD_Linky*)(0))->au16LinkyADPS),0},
		{E_CLD_LINKY_ADIR1,               (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_UINT16,   (uint32)(&((tsCLD_Linky*)(0))->au16LinkyADIR1),0},
		{E_CLD_LINKY_ADIR2,               (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_UINT16,   (uint32)(&((tsCLD_Linky*)(0))->au16LinkyADIR2),0},
		{E_CLD_LINKY_ADIR3,               (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_UINT16,   (uint32)(&((tsCLD_Linky*)(0))->au16LinkyADIR3),0},

		{E_CLD_LINKY_PERIODIC_SEND,       (E_ZCL_AF_RD|E_ZCL_AF_WR),       E_ZCL_UINT8,   (uint32)(&((tsCLD_Linky*)(0))->au8LinkyPeriodicSend),0},

		{E_CLD_LINKY_STANDARD_LTARF,       (E_ZCL_AF_RD),       E_ZCL_CSTRING,   (uint32)(&((tsCLD_Linky*)(0))->sLinkyLTARF),0},
		{E_CLD_LINKY_STANDARD_NTARF,       (E_ZCL_AF_RD),       E_ZCL_UINT8,   (uint32)(&((tsCLD_Linky*)(0))->au8LinkyNTARF),0},
		{E_CLD_LINKY_STANDARD_DATE ,       (E_ZCL_AF_RD),       E_ZCL_CSTRING,   (uint32)(&((tsCLD_Linky*)(0))->sLinkyDATE),0},

		{E_CLD_LINKY_STANDARD_EASD01 ,     (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_UINT32,   (uint32)(&((tsCLD_Linky*)(0))->u32LinkyEASD01),0},
		{E_CLD_LINKY_STANDARD_EASD02 ,     (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_UINT32,   (uint32)(&((tsCLD_Linky*)(0))->u32LinkyEASD02),0},
		{E_CLD_LINKY_STANDARD_EASD03 ,     (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_UINT32,   (uint32)(&((tsCLD_Linky*)(0))->u32LinkyEASD03),0},
		{E_CLD_LINKY_STANDARD_EASD04 ,     (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_UINT32,   (uint32)(&((tsCLD_Linky*)(0))->u32LinkyEASD04),0},

		{E_CLD_LINKY_STANDARD_SINSTI ,     (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_INT16,   (uint32)(&((tsCLD_Linky*)(0))->u16LinkySINSTI),0},
		{E_CLD_LINKY_STANDARD_SMAXIN ,     (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_INT16,   (uint32)(&((tsCLD_Linky*)(0))->u16LinkySMAXIN),0},
		{E_CLD_LINKY_STANDARD_SMAXIN_1 ,     (E_ZCL_AF_RD),       E_ZCL_INT16,   (uint32)(&((tsCLD_Linky*)(0))->u16LinkySMAXIN_1),0},

		{E_CLD_LINKY_STANDARD_CCAIN ,     (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_INT16,   (uint32)(&((tsCLD_Linky*)(0))->u16LinkyCCAIN),0},
		{E_CLD_LINKY_STANDARD_CCAIN_1 ,     (E_ZCL_AF_RD),       E_ZCL_INT16,   (uint32)(&((tsCLD_Linky*)(0))->u16LinkyCCAIN_1),0},

		{E_CLD_LINKY_STANDARD_SMAXSN_1 ,     (E_ZCL_AF_RD),        E_ZCL_UINT16,   (uint32)(&((tsCLD_Linky*)(0))->u16LinkySMAXSN_1),0},
		{E_CLD_LINKY_STANDARD_SMAXSN1_1 ,     (E_ZCL_AF_RD),       E_ZCL_UINT16,   (uint32)(&((tsCLD_Linky*)(0))->u16LinkySMAXSN1_1),0},
		{E_CLD_LINKY_STANDARD_SMAXSN2_1 ,     (E_ZCL_AF_RD),       E_ZCL_UINT16,   (uint32)(&((tsCLD_Linky*)(0))->u16LinkySMAXSN2_1),0},
		{E_CLD_LINKY_STANDARD_SMAXSN3_1 ,     (E_ZCL_AF_RD),       E_ZCL_UINT16,   (uint32)(&((tsCLD_Linky*)(0))->u16LinkySMAXSN3_1),0},

		{E_CLD_LINKY_STANDARD_MSG1,       (E_ZCL_AF_RD),       E_ZCL_CSTRING,   (uint32)(&((tsCLD_Linky*)(0))->sLinkyMSG1),0},
		{E_CLD_LINKY_STANDARD_MSG2,       (E_ZCL_AF_RD),       E_ZCL_CSTRING,   (uint32)(&((tsCLD_Linky*)(0))->sLinkyMSG2),0},

		{E_CLD_LINKY_STANDARD_STGE,       (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_OSTRING,   (uint32)(&((tsCLD_Linky*)(0))->sLinkySTGE),0},
		{E_CLD_LINKY_STANDARD_DPM1,       (E_ZCL_AF_RD),       E_ZCL_UINT8,   (uint32)(&((tsCLD_Linky*)(0))->u8LinkyDPM1),0},
		{E_CLD_LINKY_STANDARD_FPM1,       (E_ZCL_AF_RD),       E_ZCL_UINT8,   (uint32)(&((tsCLD_Linky*)(0))->u8LinkyFPM1),0},
		{E_CLD_LINKY_STANDARD_DPM2,       (E_ZCL_AF_RD),       E_ZCL_UINT8,   (uint32)(&((tsCLD_Linky*)(0))->u8LinkyDPM2),0},
		{E_CLD_LINKY_STANDARD_FPM2,       (E_ZCL_AF_RD),       E_ZCL_UINT8,   (uint32)(&((tsCLD_Linky*)(0))->u8LinkyFPM2),0},
		{E_CLD_LINKY_STANDARD_DPM3,       (E_ZCL_AF_RD),       E_ZCL_UINT8,   (uint32)(&((tsCLD_Linky*)(0))->u8LinkyDPM3),0},
		{E_CLD_LINKY_STANDARD_FPM3,       (E_ZCL_AF_RD),       E_ZCL_UINT8,   (uint32)(&((tsCLD_Linky*)(0))->u8LinkyFPM3),0},

		{E_CLD_LINKY_STANDARD_RELAIS,       (E_ZCL_AF_RD),       E_ZCL_UINT16,   (uint32)(&((tsCLD_Linky*)(0))->u16LinkyRELAIS),0},

		{E_CLD_LINKY_STANDARD_NJOURF,       (E_ZCL_AF_RD),       E_ZCL_UINT8,   (uint32)(&((tsCLD_Linky*)(0))->u8LinkyNJOURF),0},
		{E_CLD_LINKY_STANDARD_NJOURF1,      (E_ZCL_AF_RD),       E_ZCL_UINT8,   (uint32)(&((tsCLD_Linky*)(0))->u8LinkyNJOURF1),0},

		{E_CLD_LINKY_STANDARD_PJOURF1,       (E_ZCL_AF_RD),       E_ZCL_CSTRING,   (uint32)(&((tsCLD_Linky*)(0))->sLinkyPJOURF1),0},
		{E_CLD_LINKY_STANDARD_PPOINTE,       (E_ZCL_AF_RD),       E_ZCL_CSTRING,   (uint32)(&((tsCLD_Linky*)(0))->sLinkyPPOINTE),0},

		{E_CLD_LINKY_MODE,      			(E_ZCL_AF_RD),       E_ZCL_UINT8,   (uint32)(&((tsCLD_Linky*)(0))->au8LinkyMode),0},

};

tsZCL_ClusterDefinition sCLD_Linky = {
        LIXEE_CLUSTER_ID_LINKY,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_LixeeLinkyClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_LixeeLinkyClusterAttributeDefinitions,
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
PUBLIC  teZCL_Status eCLD_LixeeCreateLinky(
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
        	((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyOptarif.u8MaxLength = sizeof(((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyOptarif);
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyOptarif.u8Length = 16;
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyOptarif.pu8Data = ((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyOptarif;

			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyDemain.u8MaxLength = sizeof(((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyDemain);
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyDemain.u8Length = 4;
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyDemain.pu8Data = ((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyDemain;

			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyLTARF.u8MaxLength = sizeof(((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyLTARF);
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyLTARF.u8Length = 16;
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyLTARF.pu8Data = ((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyLTARF;

			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyMSG1.u8MaxLength = sizeof(((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyMSG1);
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyMSG1.u8Length = 32;
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyMSG1.pu8Data = ((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyMSG1;

			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyMSG2.u8MaxLength = sizeof(((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyMSG2);
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyMSG2.u8Length = 16;
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyMSG2.pu8Data = ((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyMSG2;

			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyDATE.u8MaxLength = sizeof(((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyDATE);
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyDATE.u8Length = 13;
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyDATE.pu8Data = ((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyDATE;

			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkySTGE.u8MaxLength = sizeof(((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkySTGE);
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkySTGE.u8Length = 8;
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkySTGE.pu8Data = ((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkySTGE;

			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyPJOURF1.u8MaxLength = sizeof(((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyPJOURF1);
            ((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyPJOURF1.u8Length = 1;
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyPJOURF1.pu8Data = ((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyPJOURF1;

			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyPPOINTE.u8MaxLength = sizeof(((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyPPOINTE);
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyPPOINTE.u8Length = 1;
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyPPOINTE.pu8Data = ((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyPPOINTE;

			((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyPeriodicSend = 10;


        }
    return E_ZCL_SUCCESS;

}
