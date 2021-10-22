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

		{E_CLD_LINKY_PERIODIC_SEND,       (E_ZCL_AF_RD|E_ZCL_AF_WR),       E_ZCL_UINT16,   (uint32)(&((tsCLD_Linky*)(0))->au16LinkyPeriodicSend),0},


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
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyOptarif.u8Length = 4;
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyOptarif.pu8Data = ((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyOptarif;

			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyDemain.u8MaxLength = sizeof(((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyDemain);
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyDemain.u8Length = 4;
			((tsCLD_Linky*)pvEndPointSharedStructPtr)->sLinkyDemain.pu8Data = ((tsCLD_Linky*)pvEndPointSharedStructPtr)->au8LinkyDemain;

			((tsCLD_Linky*)pvEndPointSharedStructPtr)->au16LinkyPeriodicSend = 1;


        }
    return E_ZCL_SUCCESS;

}
