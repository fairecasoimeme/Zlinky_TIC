/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef APP_REPORTING_H_
#define APP_REPORTING_H_

#include "zcl.h"
#include "PDM.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct {
    uint16 u16ClusterID;
    tsZCL_AttributeReportingConfigurationRecord sAttributeReportingConfigurationRecord;
    uint32 u32LastFiredUTCTime;
}tsReports;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PDM_teStatus eRestoreReports(void);
void vMakeSupportedAttributesReportable(void);
void vLoadDefaultConfigForReportable(void);
void vSaveReportableRecord( uint16 u16ClusterID, tsZCL_AttributeReportingConfigurationRecord* psAttributeReportingConfigurationRecord);
void vSetReportDataForMinRetention(void);
void vRestoreReportAfterSleep(void);





/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/

/****************************************************************************/
/****************************************************************************/

#endif //APP_REPORTING_H_
