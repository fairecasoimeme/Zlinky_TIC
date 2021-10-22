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
 * MODULE:             Green Power Scheduler
 *
 * COMPONENT:          GreenPowerScheduler.c
 *
 * DESCRIPTION:        The Green Power Cluster Scheduler functions
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "dlist.h"
#include "zcl.h"
#include "GreenPower.h"
#include "GreenPower_internal.h"
#include "dbg.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef TRACE_GP_DEBUG
#define TRACE_GP_DEBUG FALSE
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
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: eGP_Update20mS
 *
 * DESCRIPTION:
 * Should be called by an application timer 50 times per second
 *
 * PARAMETERS:  Name                            Usage
 * uint8        u8GreenPowerEndPointId          Local Green Power End Point Id
 *
 * RETURNS:
 * teZCL_Status
 *
 ****************************************************************************/
PUBLIC teZCL_Status eGP_Update20mS(uint8 u8GreenPowerEndPointId) {
	teZCL_Status etatus = E_ZCL_SUCCESS;
	bool_t bIsServer = TRUE;
	tsZCL_EndPointDefinition *psEndPointDefinition;
	tsZCL_ClusterInstance *psClusterInstance;
	tsGP_GreenPowerCustomData *psGpCustomDataStructure;
	uint8 u8Loop;
	tsGP_ZgpDuplicateTable *psZgpDuplicateTable;

	if ((etatus = eGP_FindGpCluster(u8GreenPowerEndPointId, bIsServer,
			&psEndPointDefinition, &psClusterInstance, &psGpCustomDataStructure))
			!= E_ZCL_SUCCESS) {
		bIsServer = FALSE;

		if ((etatus = eGP_FindGpCluster(u8GreenPowerEndPointId, bIsServer,
				&psEndPointDefinition, &psClusterInstance,
				&psGpCustomDataStructure)) != E_ZCL_SUCCESS) {
			return etatus;
		}
	}

	// get EP mutex
#ifndef COOPERATIVE
	eZCL_GetMutex(psEndPointDefinition);
#endif

	/* Handling Proxy Commission Mode Timeout */
	if (psGpCustomDataStructure->u16CommissionWindow) {
		/* Decrement and if value is zero exit commission mode */
		if ((--psGpCustomDataStructure->u16CommissionWindow == 0x00)
				&& (psGpCustomDataStructure->eGreenPowerDeviceMode
						!= E_GP_OPERATING_MODE)) {
			vGP_ExitCommMode(psEndPointDefinition,
					psGpCustomDataStructure);
		}
	}
	/* Handling transmit channel Timeout */
	if (psGpCustomDataStructure->u16TransmitChannelTimeout) {
		/* Decrement and if value is zero change channel to operational */
		if (--psGpCustomDataStructure->u16TransmitChannelTimeout == 0x00) {
            DBG_vPrintf(TRACE_GP_DEBUG, "\n eGP_Update20mS: setting channel to back to operating = %d  \n", psGpCustomDataStructure->u8OperationalChannel);
#ifndef PC_PLATFORM_BUILD
			ZPS_vNwkNibSetChannel(ZPS_pvAplZdoGetNwkHandle(),
					psGpCustomDataStructure->u8OperationalChannel);
#endif
		}
	}
	/* initialize duplicate filter table pointer */
		psZgpDuplicateTable = psGpCustomDataStructure->asZgpDuplicateFilterTable;

	/* Handling Duplicate Filter Timeout Table */
	for (u8Loop = 0; u8Loop < GP_MAX_DUPLICATE_TABLE_ENTIRES; u8Loop++) {
		/* Decrement only if aging time is non zero value */
		if (psZgpDuplicateTable[u8Loop].u16AgingTime) {
			psZgpDuplicateTable[u8Loop].u16AgingTime--;
			break;
		}
	}
	/*vGp_TransmissionTimerCallback(u8GreenPowerEndPointId, psEndPointDefinition,
			psGpCustomDataStructure);*/

	// release mutex
#ifndef COOPERATIVE
	eZCL_ReleaseMutex(psEndPointDefinition);
#endif

	return etatus;
}

/****************************************************************************
 *
 * NAME: eGP_Update1mS
 *
 * DESCRIPTION:
 * Should be called by an application timer every 1 ms
 *
 * PARAMETERS:  Name                            Usage
 * uint8        u8GreenPowerEndPointId          Local Green Power End Point Id
 *
 * RETURNS:
 * teZCL_Status
 *
 ****************************************************************************/
PUBLIC teZCL_Status eGP_Update1mS(uint8 u8GreenPowerEndPointId) {
	teZCL_Status etatus = E_ZCL_SUCCESS;

	bool_t bIsServer = TRUE;
	tsZCL_EndPointDefinition *psEndPointDefinition;
	tsZCL_ClusterInstance *psClusterInstance;
	tsGP_GreenPowerCustomData *psGpCustomDataStructure;

	if ((etatus = eGP_FindGpCluster(u8GreenPowerEndPointId, bIsServer,
			&psEndPointDefinition, &psClusterInstance, &psGpCustomDataStructure))
			!= E_ZCL_SUCCESS) {
		bIsServer = FALSE;

		if ((etatus = eGP_FindGpCluster(u8GreenPowerEndPointId, bIsServer,
				&psEndPointDefinition, &psClusterInstance,
				&psGpCustomDataStructure)) != E_ZCL_SUCCESS) {
			return etatus;
		}
	}

	// get EP mutex
#ifndef COOPERATIVE
	eZCL_GetMutex(psEndPointDefinition);
#endif

	vGp_TransmissionTimerCallback(u8GreenPowerEndPointId, psEndPointDefinition,
			psGpCustomDataStructure);


	// release mutex
#ifndef COOPERATIVE
	eZCL_ReleaseMutex(psEndPointDefinition);
#endif

	return etatus;

}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
