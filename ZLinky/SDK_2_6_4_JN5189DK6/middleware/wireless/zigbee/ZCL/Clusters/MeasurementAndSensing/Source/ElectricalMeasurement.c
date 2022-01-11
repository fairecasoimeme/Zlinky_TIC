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
 * MODULE:             Electrical Measurement Cluster
 *
 * COMPONENT:          ElectricalMeasurement.c
 *
 * DESCRIPTION:        Electrical Measurement cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "zcl_customcommand.h"
#include "ElectricalMeasurement.h"


#ifdef CLD_ELECTRICAL_MEASUREMENT
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
const tsZCL_AttributeDefinition asCLD_ElectricalMeasurementClusterAttributeDefinitions[] = {
#ifdef ELECTRICAL_MEASUREMENT_SERVER
        {E_CLD_ELECTMEAS_ATTR_ID_MEASUREMENT_TYPE,                      E_ZCL_AF_RD,                    E_ZCL_BMAP32,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u32MeasurementType),      0},   /* Mandatory */
    #ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE                      
        {E_CLD_ELECTMEAS_ATTR_ID_DC_VOLTAGE,                            (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16DCVoltage),            0},   /* Optional */
    #endif
        
    #ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE_MIN
        {E_CLD_ELECTMEAS_ATTR_ID_DC_VOLTAGE_MIN,                        E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16DCVoltageMin),         0},   /* Optional */
    #endif    
        
    #ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE_MAX 
        {E_CLD_ELECTMEAS_ATTR_ID_DC_VOLTAGE_MAX,                        E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16DCVoltageMax),         0},   /* Optional */
    #endif 
        
    #ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT
        {E_CLD_ELECTMEAS_ATTR_ID_DC_CURRENT,                            (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16DCCurrent),            0},   /* Optional */
    #endif 
                
    #ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT_MIN
        {E_CLD_ELECTMEAS_ATTR_ID_DC_CURRENT_MIN,                        E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16DCCurrentMin),         0},   /* Optional */
    #endif 
        
    #ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT_MAX
        {E_CLD_ELECTMEAS_ATTR_ID_DC_CURRENT_MAX,                        E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16DCCurrentMax),         0},   /* Optional */
    #endif
        
    #ifdef CLD_ELECTMEAS_ATTR_DC_POWER
        {E_CLD_ELECTMEAS_ATTR_ID_DC_POWER,                              (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16DCPower),              0},   /* Optional */
    #endif 
        
    #ifdef CLD_ELECTMEAS_ATTR_DC_POWER_MIN 
        {E_CLD_ELECTMEAS_ATTR_ID_DC_POWER_MIN,                          E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16DCPowerMin),            0},   /* Optional */
    #endif 
        
    #ifdef CLD_ELECTMEAS_ATTR_DC_POWER_MAX  
        {E_CLD_ELECTMEAS_ATTR_ID_DC_POWER_MAX,                          E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16DCPowerMax),            0},   /* Optional */
    #endif 
        
    #ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE_MULTIPLIER
        {E_CLD_ELECTMEAS_ATTR_ID_DC_VOLTAGE_MULTIPLIER,                (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_UINT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16DCVoltageMultiplier),  0},   /* Optional */
    #endif
        
    #ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE_DIVISOR
        {E_CLD_ELECTMEAS_ATTR_ID_DC_VOLTAGE_DIVISOR,                    (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16DCVoltageDivisor),     0},   /* Optional */
    #endif 
        
    #ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT_MULTIPLIER 
        {E_CLD_ELECTMEAS_ATTR_ID_DC_CURRENT_MULTIPLIER,                 (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16DCCurrentMultiplier),  0},   /* Optional */
    #endif
        
    #ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT_DIVISOR
        {E_CLD_ELECTMEAS_ATTR_ID_DC_CURRENT_DIVISOR,                    (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16DCCurrentDivisor),     0},   /* Optional */
    #endif
        
    #ifdef CLD_ELECTMEAS_ATTR_DC_POWER_MULTIPLIER
        {E_CLD_ELECTMEAS_ATTR_ID_DC_POWER_MULTIPLIER,                   (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16DCPowerMultiplier),    0},   /* Optional */
    #endif
        
    #ifdef CLD_ELECTMEAS_ATTR_DC_POWER_DIVISOR    
        {E_CLD_ELECTMEAS_ATTR_ID_DC_POWER_DIVISOR,                      (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16DCPowerDivisor),       0},   /* Optional */
    #endif
        
    #ifdef CLD_ELECTMEAS_ATTR_AC_FREQUENCY
        {E_CLD_ELECTMEAS_ATTR_ID_AC_FREQUENCY,                          (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ACFrequency),          0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_AC_FREQUENCY_MIN
        {E_CLD_ELECTMEAS_ATTR_ID_AC_FREQUENCY_MIN,                      E_ZCL_AF_RD,                    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ACFrequencyMin),       0},   /* Optional */
    #endif        

    #ifdef CLD_ELECTMEAS_ATTR_AC_FREQUENCY_MAX
        {E_CLD_ELECTMEAS_ATTR_ID_AC_FREQUENCY_MAX,                      E_ZCL_AF_RD,                    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ACFrequencyMax),       0},   /* Optional */
    #endif 
        
    #ifdef CLD_ELECTMEAS_ATTR_NEUTRAL_CURRENT
        {E_CLD_ELECTMEAS_ATTR_ID_NEUTRAL_CURRENT,                       (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16NeutralCurrent),       0},   /* Optional */
    #endif        

    #ifdef CLD_ELECTMEAS_ATTR_TOTAL_ACTIVE_POWER
        {E_CLD_ELECTMEAS_ATTR_ID_TOTAL_ACTIVE_POWER,                    (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT32,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i32TotalActivePower),     0},   /* Optional */
    #endif 

    #ifdef CLD_ELECTMEAS_ATTR_TOTAL_REACTIVE_POWER
        {E_CLD_ELECTMEAS_ATTR_ID_TOTAL_REACTIVE_POWER,                  (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT32,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i32TotalReactivePower),    0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_TOTAL_APPARENT_POWER
        {E_CLD_ELECTMEAS_ATTR_ID_TOTAL_APPARENT_POWER,                  (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT32,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u32TotalApparentPower),  0},   /* Optional */
    #endif  
        
    #ifdef CLD_ELECTMEAS_ATTR_MEASURED_1_HARMONIC_CURRENT 
        {E_CLD_ELECTMEAS_ATTR_ID_MEASURED_1_HARMONIC_CURRENT,           (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16Measured1stHarmonicCurrent),   0},   /* Optional */
    #endif  

    #ifdef CLD_ELECTMEAS_ATTR_MEASURED_3_HARMONIC_CURRENT 
        {E_CLD_ELECTMEAS_ATTR_ID_MEASURED_3_HARMONIC_CURRENT,           (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16Measured3rdHarmonicCurrent),   0},   /* Optional */
    #endif 

    #ifdef CLD_ELECTMEAS_ATTR_MEASURED_5_HARMONIC_CURRENT 
        {E_CLD_ELECTMEAS_ATTR_ID_MEASURED_5_HARMONIC_CURRENT,           (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16Measured5thHarmonicCurrent),   0},   /* Optional */
    #endif 

    #ifdef CLD_ELECTMEAS_ATTR_MEASURED_7_HARMONIC_CURRENT 
        {E_CLD_ELECTMEAS_ATTR_ID_MEASURED_7_HARMONIC_CURRENT,           (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16Measured7thHarmonicCurrent),   0},   /* Optional */
    #endif 

    #ifdef CLD_ELECTMEAS_ATTR_MEASURED_9_HARMONIC_CURRENT 
        {E_CLD_ELECTMEAS_ATTR_ID_MEASURED_9_HARMONIC_CURRENT,           (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16Measured9thHarmonicCurrent),   0},   /* Optional */
    #endif 

    #ifdef CLD_ELECTMEAS_ATTR_MEASURED_11_HARMONIC_CURRENT 
        {E_CLD_ELECTMEAS_ATTR_ID_MEASURED_11_HARMONIC_CURRENT,          (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16Measured11thHarmonicCurrent),   0},   /* Optional */
    #endif 

    #ifdef CLD_ELECTMEAS_ATTR_MEASURED_1_PHASE_CURRENT 
        {E_CLD_ELECTMEAS_ATTR_ID_MEASURED_1_PHASE_CURRENT,              (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16Measured1stPhaseCurrent),   0},   /* Optional */
    #endif  

    #ifdef CLD_ELECTMEAS_ATTR_MEASURED_3_PHASE_CURRENT 
        {E_CLD_ELECTMEAS_ATTR_ID_MEASURED_3_PHASE_CURRENT,              (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16Measured3rdPhaseCurrent),   0},   /* Optional */
    #endif                                                              
                                                                        
    #ifdef CLD_ELECTMEAS_ATTR_MEASURED_5_PHASE_CURRENT                  
        {E_CLD_ELECTMEAS_ATTR_ID_MEASURED_5_PHASE_CURRENT,              (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16Measured5thPhaseCurrent),   0},   /* Optional */
    #endif                                                              
                                                                        
    #ifdef CLD_ELECTMEAS_ATTR_MEASURED_7_PHASE_CURRENT                  
        {E_CLD_ELECTMEAS_ATTR_ID_MEASURED_7_PHASE_CURRENT,              (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16Measured7thPhaseCurrent),   0},   /* Optional */
    #endif                                                              
                                                                        
    #ifdef CLD_ELECTMEAS_ATTR_MEASURED_9_PHASE_CURRENT                  
        {E_CLD_ELECTMEAS_ATTR_ID_MEASURED_9_PHASE_CURRENT,              (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16Measured9thPhaseCurrent),   0},   /* Optional */
    #endif 

    #ifdef CLD_ELECTMEAS_ATTR_MEASURED_11_PHASE_CURRENT 
        {E_CLD_ELECTMEAS_ATTR_ID_MEASURED_11_PHASE_CURRENT,             (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16Measured11thtPhaseCurrent), 0},   /* Optional */
    #endif 

    #ifdef CLD_ELECTMEAS_ATTR_AC_FREQUENCY_MULTIPLIER 
        {E_CLD_ELECTMEAS_ATTR_ID_AC_FREQUENCY_MULTIPLIER,               (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ACFrequencyMultiplier),    0},   /* Optional */
    #endif   
        
    #ifdef CLD_ELECTMEAS_ATTR_AC_FREQUENCY_DIVISOR
        {E_CLD_ELECTMEAS_ATTR_ID_AC_FREQUENCY_DIVISOR,                  (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ACFrequencyDivisor),   0},   /* Optional */
    #endif   

    #ifdef CLD_ELECTMEAS_ATTR_POWER_MULTIPLIER 
        {E_CLD_ELECTMEAS_ATTR_ID_POWER_MULTIPLIER,                      (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT32,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u32PowerMultiplier),      0},   /* Optional */
    #endif   
        
    #ifdef CLD_ELECTMEAS_ATTR_POWER_DIVISOR
        {E_CLD_ELECTMEAS_ATTR_ID_POWER_DIVISOR,                         (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT32,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u32PowerDivisor),         0},   /* Optional */
    #endif  

    #ifdef CLD_ELECTMEAS_ATTR_HARMONIC_CURRENT_MULTIPLIER 
        {E_CLD_ELECTMEAS_ATTR_ID_HARMONIC_CURRENT_MULTIPLIER,           (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT8,      (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i8HarmonicCurrentMultiplier),      0},   /* Optional */
    #endif   
        
    #ifdef CLD_ELECTMEAS_ATTR_PHASE_HARMONIC_CURRENT_MULTIPLIER
        {E_CLD_ELECTMEAS_ATTR_ID_PHASE_HARMONIC_CURRENT_MULTIPLIER,     (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT8,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i8PhaseHarmonicCurrentMultiplier),         0},   /* Optional */
    #endif 

    #ifdef CLD_ELECTMEAS_ATTR_LINE_CURRENT 
        {E_CLD_ELECTMEAS_ATTR_ID_LINE_CURRENT,                          (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16LineCurrent),         0},   /* Optional */
    #endif        

    #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_CURRENT 
        {E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_CURRENT,                        (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ActiveCurrent),        0},   /* Optional */
    #endif   
        
    #ifdef CLD_ELECTMEAS_ATTR_REACTIVE_CURRENT
        {E_CLD_ELECTMEAS_ATTR_ID_REACTIVE_CURRENT,                      (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ReactiveCurrent),      0},   /* Optional */
    #endif 
        
    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE,                           (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSVoltage),           0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MIN
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_MIN,                       E_ZCL_AF_RD,                    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSVoltageMin),        0},   /* Optional */
    #endif
        
    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MAX
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_MAX,                       E_ZCL_AF_RD,                    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSVoltageMax),        0},   /* Optional */
    #endif
        
    #ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT,                           (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSCurrent),           0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_MIN
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT_MIN,                       E_ZCL_AF_RD,                    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSCurrentMin),         0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_MAX
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT_MAX,                       E_ZCL_AF_RD,                    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSCurrentMax),         0},   /* Optional */
    #endif
        
    #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER 
        {E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER,                          (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ActivePower),          0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MIN
        {E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_MIN,                      E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ActivePowerMin),       0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MAX
        {E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_MAX,                      (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ActivePowerMax),       0},   /* Optional */
    #endif
        
    #ifdef CLD_ELECTMEAS_ATTR_REACTIVE_POWER
        {E_CLD_ELECTMEAS_ATTR_ID_REACTIVE_POWER,                        (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ReactivePower),        0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_APPARENT_POWER
        {E_CLD_ELECTMEAS_ATTR_ID_APPARENT_POWER,                        (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ApparentPower),        0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_POWER_FACTOR
        {E_CLD_ELECTMEAS_ATTR_ID_POWER_FACTOR,                          E_ZCL_AF_RD,                    E_ZCL_INT8,      (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i8PowerFactor),           0},   /* Optional */
    #endif
        
    #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD
        {E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD,    (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16AverageRMSVoltageMeasurementPeriod),        0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_OVER_VOLTAGE_COUNTER
        {E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_OVER_VOLTAGE_COUNTER,          (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16AverageRMSOverVoltageCounter),        0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_AVG_RMS_UNDER_VOLTAGE_COUNTER
        {E_CLD_ELECTMEAS_AVG_ID_RMS_UNDER_VOLTAGE_COUNTER,              (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16AverageRMSUnderVoltageCounter),        0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_OVER_VOLTAGE_PERIOD
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_OVER_VOLTAGE_PERIOD,       (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSExtremeOverVoltagePeriod),        0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_UNDER_VOLTAGE_PERIOD
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_UNDER_VOLTAGE_PERIOD,      (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSExtremeUnderVoltagePeriod),        0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SAG_PERIOD 
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SAG_PERIOD,                (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSVoltageSagPeriod),        0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SWELL_PERIOD 
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SWELL_PERIOD,              (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSVoltageSwellPeriod),        0},   /* Optional */
    #endif
        
    #ifdef CLD_ELECTMEAS_ATTR_AC_VOLTAGE_MULTIPLIER
        {E_CLD_ELECTMEAS_ATTR_ID_AC_VOLTAGE_MULTIPLIER,                 (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ACVoltageMultiplier),  0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_AC_VOLTAGE_DIVISOR
        {E_CLD_ELECTMEAS_ATTR_ID_AC_VOLTAGE_DIVISOR,                    (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ACVoltageDivisor),     0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_AC_CURRENT_MULTIPLIER
        {E_CLD_ELECTMEAS_ATTR_ID_AC_CURRENT_MULTIPLIER,                 (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ACCurrentMultiplier),  0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_AC_CURRENT_DIVISOR
        {E_CLD_ELECTMEAS_ATTR_ID_AC_CURRENT_DIVISOR,                    (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ACCurentDivisor),      0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_AC_POWER_MULTIPLIER
        {E_CLD_ELECTMEAS_ATTR_ID_AC_POWER_MULTIPLIER,                   (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ACPowerMultiplier),    0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_AC_POWER_DIVISOR
        {E_CLD_ELECTMEAS_ATTR_ID_AC_POWER_DIVISOR,                      (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ACPowerDivisor),       0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_DC_OVERLOAD_ALARMS_MASK 
        {E_CLD_ELECTMEAS_ATTR_ID_DC_OVERLOAD_ALARMS_MASK,              (E_ZCL_AF_RD|E_ZCL_AF_WR),       E_ZCL_BMAP8,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u8DCOverloadAlarmsMask),  0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE_OVERLOAD
        {E_CLD_ELECTMEAS_ATTR_ID_DC_VOLTAGE_OVERLOAD,                   E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16DCVoltageOverload),    0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT_OVERLOAD
        {E_CLD_ELECTMEAS_ATTR_ID_DC_CURRENT_OVERLOAD,                   E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16DCCurrentOverload),    0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_AC_ALARMS_MASK  
        {E_CLD_ELECTMEAS_ATTR_ID_AC_ALARMS_MASK,                        (E_ZCL_AF_RD|E_ZCL_AF_WR),       E_ZCL_BMAP16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ACAlarmsMask),        0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_AC_VOLTAGE_OVERLOAD
        {E_CLD_ELECTMEAS_ATTR_ID_AC_VOLTAGE_OVERLOAD,                   E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ACVoltageOverload),    0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_AC_CURRENT_OVERLOAD
        {E_CLD_ELECTMEAS_ATTR_ID_AC_CURRENT_OVERLOAD,                   E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ACCurrentOverload),    0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_AC_ACTIVE_POWER_OVERLOAD
        {E_CLD_ELECTMEAS_ATTR_ID_AC_ACTIVE_POWER_OVERLOAD,              E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ACActivePowerOverload),    0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_AC_REACTIVE_POWER_OVERLOAD
        {E_CLD_ELECTMEAS_ATTR_ID_AC_REACTIVE_POWER_OVERLOAD,            E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ACReactivePowerOverload),  0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_OVER_VOLTAGE
        {E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_OVER_VOLTAGE,                  E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16AverageRMSOverVoltage),    0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_UNDER_VOLTAGE
        {E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_UNDER_VOLTAGE,                 E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16AverageRMSUnderVoltage),    0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_OVER_VOLTAGE
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_OVER_VOLTAGE,              (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16RMSExtremeOverVoltage),     0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_UNDER_VOLTAGE
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_UNDER_VOLTAGE,             (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16RMSExtremeUnderVoltage),    0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SAG
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SAG,                       (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16RMSVoltageSag),         0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SWELL
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SWELL,                     (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16RMSVoltageSwell),       0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_LINE_CURRENT_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_LINE_CURRENT_PHB,                      (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16LineCurrentPhB),       0},   /* Optional */
    #endif
    
    #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_CURRENT_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_CURRENT_PHB,                    (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ActiveCurrentPhB),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_REACTIVE_CURRENT_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_REACTIVE_CURRENT_PHB,                  (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ReactiveCurrentPhB),   0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_PHB,                       (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSVoltagePhB),        0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MIN_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_MIN_PHB,                   E_ZCL_AF_RD,                    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSVoltageMinPhB),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MAX_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_MAX_PHB,                   E_ZCL_AF_RD,                    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSVoltageMaxPhB),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT_PHB,                       (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSCurrentPhB),        0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_CURRENT_MIN_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_CURRENT_MIN_PHB,                       E_ZCL_AF_RD,                    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSCurrentMinPhB),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_CURRENT_MAX_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT_MAX_PHB,                       E_ZCL_AF_RD,                    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSCurrentMaxPhB),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_PHB,                      (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ActivePowerPhB),       0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MIN_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_MIN_PHB,                  E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ActivePowerMinPhB),    0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MAX_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_MAX_PHB,                  E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ActivePowerMaxPhB),    0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_REACTIVE_POWER_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_REACTIVE_POWER_PHB,                    (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ReactivePowerPhB),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_APPARENT_POWER_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_APPARENT_POWER_PHB,                    (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ApparentPowerPhB),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_POWER_FACTOR_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_POWER_FACTOR_PHB,                      E_ZCL_AF_RD,                    E_ZCL_INT8,      (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i8PowerFactorPhB),        0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD_PHB,  (E_ZCL_AF_RD|E_ZCL_AF_RP),    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16AverageRMSVoltageMeasurementPeriodPhB),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_OVER_VOLTAGE_COUNTER_PHB 
        {E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_OVER_VOLTAGE_COUNTER_PHB,        (E_ZCL_AF_RD|E_ZCL_AF_WR),    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16AverageRMSOverVoltageCounterPhB),           0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_UNDER_VOLTAGE_COUNTER_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_UNDER_VOLTAGE_COUNTER_PHB,       (E_ZCL_AF_RD|E_ZCL_AF_WR),    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16AverageRMSUnderVoltageCounterPhB),          0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PHB,     (E_ZCL_AF_RD|E_ZCL_AF_WR),    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSExtremeOverVoltagePeriodPhB),            0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PHB,    (E_ZCL_AF_RD|E_ZCL_AF_WR),   E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSExtremeUnderVoltagePeriodPhB),           0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SAG_PERIOD_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SAG_PERIOD_PHB,              (E_ZCL_AF_RD|E_ZCL_AF_WR),   E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSVoltageSagPeriodPhB),                    0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SWELL_PERIOD_PHB
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SWELL_PERIOD_PHB,            (E_ZCL_AF_RD|E_ZCL_AF_WR),   E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSVoltageSwellPeriodPhB),                  0},   /* Optional */
    #endif  
        
    #ifdef CLD_ELECTMEAS_ATTR_LINE_CURRENT_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_LINE_CURRENT_PHC,                      (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16LineCurrentPhC),       0},   /* Optional */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_CURRENT_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_CURRENT_PHC,                    (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ActiveCurrentPhC),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_REACTIVE_CURRENT_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_REACTIVE_CURRENT_PHC,                  (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ReactiveCurrentPhC),   0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_PHC,                       (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSVoltagePhC),        0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MIN_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_MIN_PHC,                   E_ZCL_AF_RD,                    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSVoltageMinPhC),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MAX_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_MAX_PHC,                   E_ZCL_AF_RD,                    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSVoltageMaxPhC),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT_PHC,                       (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSCurrentPhC),        0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_CURRENT_MIN_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_CURRENT_MIN_PHC,                       E_ZCL_AF_RD,                    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSCurrentMinPhC),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_CURRENT_MAX_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT_MAX_PHC,                       E_ZCL_AF_RD,                    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSCurrentMaxPhC),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_PHC,                      (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ActivePowerPhC),       0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MIN_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_MIN_PHC,                  E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ActivePowerMinPhC),    0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MAX_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_MAX_PHC,                  E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ActivePowerMaxPhC),    0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_REACTIVE_POWER_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_REACTIVE_POWER_PHC,                    (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,     (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i16ReactivePowerPhC),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_APPARENT_POWER_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_APPARENT_POWER_PHC,                    (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ApparentPowerPhC),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_POWER_FACTOR_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_POWER_FACTOR_PHC,                      E_ZCL_AF_RD,                    E_ZCL_INT8,      (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i8PowerFactorPhC),        0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD_PHC,  (E_ZCL_AF_RD|E_ZCL_AF_RP),    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16AverageRMSVoltageMeasurementPeriodPhC),     0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_OVER_VOLTAGE_COUNTER_PHC 
        {E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_OVER_VOLTAGE_COUNTER_PHC,        (E_ZCL_AF_RD|E_ZCL_AF_WR),    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16AverageRMSOverVoltageCounterPhC),           0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_UNDER_VOLTAGE_COUNTER_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_UNDER_VOLTAGE_COUNTER_PHC,       (E_ZCL_AF_RD|E_ZCL_AF_WR),    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16AverageRMSUnderVoltageCounterPhC),          0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PHC,     (E_ZCL_AF_RD|E_ZCL_AF_WR),    E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSExtremeOverVoltagePeriodPhC),            0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PHC,    (E_ZCL_AF_RD|E_ZCL_AF_WR),   E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSExtremeUnderVoltagePeriodPhC),           0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SAG_PERIOD_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SAG_PERIOD_PHC,              (E_ZCL_AF_RD|E_ZCL_AF_WR),   E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSVoltageSagPeriodPhC),                    0},   /* Optional */
    #endif    
    
    #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SWELL_PERIOD_PHC
        {E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SWELL_PERIOD_PHC,            (E_ZCL_AF_RD|E_ZCL_AF_WR),   E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16RMSVoltageSwellPeriodPhC),                  0},   /* Optional */
    #endif
    /*Manufacturer Specific Attributes*/
    #ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_APPARENT_POWER   
        {E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_APPARENT_POWER,               (E_ZCL_AF_RD|E_ZCL_AF_MS),      E_ZCL_UINT32,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u32ManSpecificApparentPower),       0},   /* Manu Specific */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_NON_ACTIVE_POWER
        {E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_NON_ACTIVE_POWER,             (E_ZCL_AF_RD|E_ZCL_AF_MS),      E_ZCL_UINT32,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u32NonActivePower),       0},   /* Manu Specific */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_FNDMTL_REACTIVE_POWER
        {E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_FNDMTL_REACTIVE_POWER,        (E_ZCL_AF_RD|E_ZCL_AF_MS),      E_ZCL_INT32,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->i32FundamentalReactivePower),       0},   /* Manu Specific */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_FNDMTL_APPARENT_POWER
        {E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_FNDMTL_APPARENT_POWER,        (E_ZCL_AF_RD|E_ZCL_AF_MS),      E_ZCL_UINT32,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u32FundamentalApparentPower),       0},   /* Manu Specific */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_FNDMTL_POWER_FACTOR
        {E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_FNDMTL_POWER_FACTOR,          (E_ZCL_AF_RD|E_ZCL_AF_MS),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16FundamentalPowerFactor),       0},   /* Manu Specific */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_NON_FNDMTL_APPARENT_POWER
        {E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_NON_FNDMTL_APPARENT_POWER,    (E_ZCL_AF_RD|E_ZCL_AF_MS),      E_ZCL_UINT32,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u32NonFundamentalApparentPower),       0},   /* Manu Specific */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_TOTAL_HARMONIC_DISTORTION
        {E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_TOTAL_HARMONIC_DISTORTION,    (E_ZCL_AF_RD|E_ZCL_AF_MS),      E_ZCL_UINT32,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u32TotalHarmonicDistortion),       0},   /* Manu Specific */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_VBIAS
        {E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_VBIAS,                        (E_ZCL_AF_RD|E_ZCL_AF_MS),      E_ZCL_UINT32,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u32VBias),       0},   /* Manu Specific */
    #endif

    #ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_DIVISOR
        {E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_DIVISOR,                      (E_ZCL_AF_RD|E_ZCL_AF_MS),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ManSpecDivisor),       0},   /* Manu Specific */
    #endif

#endif    
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                              (E_ZCL_AF_RD|E_ZCL_AF_GA),      E_ZCL_BMAP32,   (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                         (E_ZCL_AF_RD|E_ZCL_AF_GA),      E_ZCL_UINT16,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u16ClusterRevision),      0},   /* Mandatory  */
    
    #if (defined ELECTRICAL_MEASUREMENT_SERVER) && (defined CLD_ELECTMEAS_ATTR_ATTRIBUTE_REPORTING_STATUS) 
        {E_CLD_GLOBAL_ATTR_ID_ATTRIBUTE_REPORTING_STATUS,               (E_ZCL_AF_RD|E_ZCL_AF_GA),      E_ZCL_ENUM8,    (uint32)(&((tsCLD_ElectricalMeasurement*)(0))->u8AttributeReportingStatus),0},
    #endif
    };

tsZCL_ClusterDefinition sCLD_ElectricalMeasurement = {
        MEASUREMENT_AND_SENSING_CLUSTER_ID_ELECTRICAL_MEASUREMENT,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_ElectricalMeasurementClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_ElectricalMeasurementClusterAttributeDefinitions,
        NULL
};

uint8 au8ElectricalMeasurementAttributeControlBits[(sizeof(asCLD_ElectricalMeasurementClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eCLD_ElectricalMeasurementCreateElectricalMeasurement
 **
 ** DESCRIPTION:
 ** Creates a temperature measurement cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ElectricalMeasurementCreateElectricalMeasurement(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits)
{
    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if((psClusterInstance==NULL) ||
           (psClusterDefinition==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    /* Create an instance of a temperature measurement cluster */
    vZCL_InitializeClusterInstance(
                   psClusterInstance, 
                   bIsServer,
                   psClusterDefinition,
                   pvEndPointSharedStructPtr,
                   pu8AttributeControlBits,
                   NULL,
                   NULL);

        if(pvEndPointSharedStructPtr != NULL)
        {
#ifdef ELECTRICAL_MEASUREMENT_SERVER            
            /* Initializing these values to invalid as ZCL spec does not mention the defualt */ 
 
            ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u32MeasurementType = 0;       
                     
            #ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE                      
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16DCVoltage = 0x8000;
            #endif
                
            #ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE_MIN
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16DCVoltageMin = 0x8000;
            #endif    
                
            #ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE_MAX 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16DCVoltageMax = 0x8000;
            #endif 
                
            #ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16DCCurrent = 0x8000;
            #endif 
                        
            #ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT_MIN
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16DCCurrentMin = 0x8000;
            #endif 
                
            #ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT_MAX
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16DCCurrentMax = 0x8000;
            #endif
                
            #ifdef CLD_ELECTMEAS_ATTR_DC_POWER
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16DCPower = 0x8000;
            #endif 
                
            #ifdef CLD_ELECTMEAS_ATTR_DC_POWER_MIN 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16DCPowerMin = 0x8000;
            #endif 
                
            #ifdef CLD_ELECTMEAS_ATTR_DC_POWER_MAX  
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16DCPowerMax = 0x8000;
            #endif   
        
            #ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE_MULTIPLIER
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16DCVoltageMultiplier = 0x0001;
            #endif
                
            #ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE_DIVISOR
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16DCVoltageDivisor = 0x0001;
            #endif 
                
            #ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT_MULTIPLIER 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16DCCurrentMultiplier = 0x0001;
            #endif
                
            #ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT_DIVISOR
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16DCCurrentDivisor = 0x0001;
            #endif
                
            #ifdef CLD_ELECTMEAS_ATTR_DC_POWER_MULTIPLIER
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16DCPowerMultiplier = 0x0001;
            #endif
                
            #ifdef CLD_ELECTMEAS_ATTR_DC_POWER_DIVISOR    
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16DCPowerDivisor = 0x0001;
            #endif

            #ifdef CLD_ELECTMEAS_ATTR_AC_FREQUENCY
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ACFrequency = 0xFFFF;
            #endif

            #ifdef CLD_ELECTMEAS_ATTR_AC_FREQUENCY_MIN
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ACFrequencyMin = 0xFFFF;
            #endif

            #ifdef CLD_ELECTMEAS_ATTR_AC_FREQUENCY_MAX
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ACFrequencyMax = 0xFFFF;
            #endif
         
            #ifdef CLD_ELECTMEAS_ATTR_MEASURED_1_HARMONIC_CURRENT 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16Measured1stHarmonicCurrent = 0x8000;
            #endif  
         
            #ifdef CLD_ELECTMEAS_ATTR_MEASURED_3_HARMONIC_CURRENT 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16Measured3rdHarmonicCurrent = 0x8000;
            #endif 
         
            #ifdef CLD_ELECTMEAS_ATTR_MEASURED_5_HARMONIC_CURRENT 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16Measured5thHarmonicCurrent = 0x8000;
            #endif 
         
            #ifdef CLD_ELECTMEAS_ATTR_MEASURED_7_HARMONIC_CURRENT 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16Measured7thHarmonicCurrent = 0x8000;
            #endif 
         
            #ifdef CLD_ELECTMEAS_ATTR_MEASURED_9_HARMONIC_CURRENT 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16Measured9thHarmonicCurrent = 0x8000;
            #endif 
         
            #ifdef CLD_ELECTMEAS_ATTR_MEASURED_11_HARMONIC_CURRENT 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16Measured11thHarmonicCurrent = 0x8000;
            #endif 
         
            #ifdef CLD_ELECTMEAS_ATTR_MEASURED_1_PHASE_CURRENT 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16Measured1stPhaseCurrent = 0x8000;
            #endif  
         
            #ifdef CLD_ELECTMEAS_ATTR_MEASURED_3_PHASE_CURRENT 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16Measured3rdPhaseCurrent = 0x8000;
            #endif                                                              
                                                                                
            #ifdef CLD_ELECTMEAS_ATTR_MEASURED_5_PHASE_CURRENT                  
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16Measured5thPhaseCurrent = 0x8000;
            #endif                                                              
                                                                                
            #ifdef CLD_ELECTMEAS_ATTR_MEASURED_7_PHASE_CURRENT                  
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16Measured7thPhaseCurrent = 0x8000;
            #endif                                                              
                                                                                
            #ifdef CLD_ELECTMEAS_ATTR_MEASURED_9_PHASE_CURRENT                  
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16Measured9thPhaseCurrent = 0x8000;
            #endif 
         
            #ifdef CLD_ELECTMEAS_ATTR_MEASURED_11_PHASE_CURRENT 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16Measured11thtPhaseCurrent = 0x8000;
            #endif 
                
            #ifdef CLD_ELECTMEAS_ATTR_AC_FREQUENCY_MULTIPLIER 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ACFrequencyMultiplier = 0x0001;
            #endif   
                
            #ifdef CLD_ELECTMEAS_ATTR_AC_FREQUENCY_DIVISOR
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ACFrequencyDivisor = 0x0001;
            #endif   
        
            #ifdef CLD_ELECTMEAS_ATTR_POWER_MULTIPLIER 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u32PowerMultiplier = 0x000001;
            #endif 
            
            #ifdef CLD_ELECTMEAS_ATTR_POWER_DIVISOR                
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u32PowerDivisor = 0x000001;
            #endif  

            #ifdef CLD_ELECTMEAS_ATTR_HARMONIC_CURRENT_MULTIPLIER 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i8HarmonicCurrentMultiplier = 0x00;
            #endif   
                
            #ifdef CLD_ELECTMEAS_ATTR_PHASE_HARMONIC_CURRENT_MULTIPLIER
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i8PhaseHarmonicCurrentMultiplier = 0x00;
            #endif                 
                           
            #ifdef CLD_ELECTMEAS_ATTR_LINE_CURRENT 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16LineCurrent = 0xFFFF;
            #endif        
        
            #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_CURRENT 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ActiveCurrent = 0x8000;
            #endif   
                
            #ifdef CLD_ELECTMEAS_ATTR_REACTIVE_CURRENT
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ReactiveCurrent = 0x8000;
            #endif         
            #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSVoltage = 0xFFFF;
            #endif

            #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MIN
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSVoltageMin = 0xFFFF;
            #endif

            #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MAX
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSVoltageMax = 0xFFFF;
            #endif
                
            #ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSCurrent = 0xFFFF;
            #endif

            #ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_MIN
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSCurrentMin = 0xFFFF;
            #endif

            #ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_MAX
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSCurrentMax = 0xFFFF;
            #endif
                
            #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ActivePower = 0x8000;
            #endif

            #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MIN
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ActivePowerMin = 0x8000;
            #endif

            #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MAX
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ActivePowerMax = 0x8000;
            #endif
                
            #ifdef CLD_ELECTMEAS_ATTR_REACTIVE_POWER
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ReactivePower = 0x8000;
            #endif
            
            #ifdef CLD_ELECTMEAS_ATTR_APPARENT_POWER
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ApparentPower = 0xFFFF;
            #endif
            
            #ifdef CLD_ELECTMEAS_ATTR_POWER_FACTOR
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i8PowerFactor = 0x00;
            #endif
                
            #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16AverageRMSVoltageMeasurementPeriod = 0x0000;
            #endif
        
            #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_OVER_VOLTAGE_COUNTER
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16AverageRMSOverVoltageCounter = 0x0000;
            #endif
        
            #ifdef CLD_ELECTMEAS_AVG_RMS_UNDER_VOLTAGE_COUNTER
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16AverageRMSUnderVoltageCounter = 0x0000;
            #endif
        
            #ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_OVER_VOLTAGE_PERIOD
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSExtremeOverVoltagePeriod = 0x0000;
            #endif
        
            #ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_UNDER_VOLTAGE_PERIOD
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSExtremeUnderVoltagePeriod = 0x0000;
            #endif
        
            #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SAG_PERIOD 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSVoltageSagPeriod = 0x0000;
            #endif
        
            #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SWELL_PERIOD 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSVoltageSwellPeriod = 0x0000;
            #endif
        
            #ifdef CLD_ELECTMEAS_ATTR_AC_VOLTAGE_MULTIPLIER
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ACVoltageMultiplier = 0x0001;
            #endif
            
            #ifdef CLD_ELECTMEAS_ATTR_AC_VOLTAGE_DIVISOR
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ACVoltageDivisor = 0x0001;
            #endif
    
            #ifdef CLD_ELECTMEAS_ATTR_AC_CURRENT_MULTIPLIER
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ACCurrentMultiplier = 0x0001;
            #endif
    
            #ifdef CLD_ELECTMEAS_ATTR_AC_CURRENT_DIVISOR
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ACCurentDivisor = 0x0001;
            #endif
    
            #ifdef CLD_ELECTMEAS_ATTR_AC_POWER_MULTIPLIER
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ACPowerMultiplier = 0x0001;
            #endif
    
            #ifdef CLD_ELECTMEAS_ATTR_AC_POWER_DIVISOR
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ACPowerDivisor = 0x0001;
            #endif

            #ifdef CLD_ELECTMEAS_ATTR_DC_OVERLOAD_ALARMS_MASK 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u8DCOverloadAlarmsMask = 0;
            #endif
                
            #ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE_OVERLOAD
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16DCVoltageOverload = 0xFFFF;
            #endif
        
            #ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT_OVERLOAD
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16DCCurrentOverload = 0xFFFF;
            #endif                
                        
            #ifdef CLD_ELECTMEAS_ATTR_AC_VOLTAGE_OVERLOAD
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ACVoltageOverload = 0xFFFF;
            #endif

            #ifdef CLD_ELECTMEAS_ATTR_AC_ALARMS_MASK 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ACAlarmsMask = 0;
            #endif
        
            #ifdef CLD_ELECTMEAS_ATTR_AC_CURRENT_OVERLOAD
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ACCurrentOverload = 0xFFFF;
            #endif
        
            #ifdef CLD_ELECTMEAS_ATTR_AC_ACTIVE_POWER_OVERLOAD
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ACActivePowerOverload = 0xFFFF;
            #endif
        
            #ifdef CLD_ELECTMEAS_ATTR_AC_REACTIVE_POWER_OVERLOAD
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ACReactivePowerOverload = 0xFFFF;
            #endif
                
            #ifdef CLD_ELECTMEAS_ATTR_LINE_CURRENT_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16LineCurrentPhB = 0xFFFF;
            #endif
            
            #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_CURRENT_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ActiveCurrentPhB = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_REACTIVE_CURRENT_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ReactiveCurrentPhB = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSVoltagePhB = 0xFFFF;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MIN_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSVoltageMinPhB = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MAX_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSVoltageMaxPhB = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSCurrentPhB = 0xFFFF;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_CURRENT_MIN_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSCurrentMinPhB = 0xFFFF;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_CURRENT_MAX_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSCurrentMaxPhB = 0xFFFF;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ActivePowerPhB = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MIN_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ActivePowerMinPhB = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MAX_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ActivePowerMaxPhB = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_REACTIVE_POWER_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ReactivePowerPhB = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_APPARENT_POWER_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ApparentPowerPhB = 0xFFFF;
            #endif    

            #ifdef CLD_ELECTMEAS_ATTR_POWER_FACTOR_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i8PowerFactorPhB = 0x00;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16AverageRMSVoltageMeasurementPeriodPhB = 0x0000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_OVER_VOLTAGE_COUNTER_PHB 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16AverageRMSOverVoltageCounterPhB = 0x0000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_UNDER_VOLTAGE_COUNTER_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16AverageRMSUnderVoltageCounterPhB = 0x0000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSExtremeOverVoltagePeriodPhB = 0x0000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSExtremeUnderVoltagePeriodPhB = 0x0000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SAG_PERIOD_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSVoltageSagPeriodPhB = 0x0000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SWELL_PERIOD_PHB
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSVoltageSwellPeriodPhB = 0x0000;
            #endif                 
                
            #ifdef CLD_ELECTMEAS_ATTR_LINE_CURRENT_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16LineCurrentPhC = 0xFFFF;
            #endif
            
            #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_CURRENT_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ActiveCurrentPhC = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_REACTIVE_CURRENT_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ReactiveCurrentPhC = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSVoltagePhC = 0xFFFF;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MIN_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSVoltageMinPhC = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MAX_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSVoltageMaxPhC = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSCurrentPhC = 0xFFFF;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_CURRENT_MIN_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSCurrentMinPhC = 0xFFFF;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_CURRENT_MAX_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSCurrentMaxPhC = 0xFFFF;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ActivePowerPhC = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MIN_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ActivePowerMinPhC = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MAX_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ActivePowerMaxPhC = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_REACTIVE_POWER_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i16ReactivePowerPhC = 0x8000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_APPARENT_POWER_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ApparentPowerPhC = 0xFFFF;
            #endif   

            #ifdef CLD_ELECTMEAS_ATTR_POWER_FACTOR_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->i8PowerFactorPhC = 0x00;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16AverageRMSVoltageMeasurementPeriodPhC = 0x0000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_OVER_VOLTAGE_COUNTER_PHC 
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16AverageRMSOverVoltageCounterPhC = 0x0000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_UNDER_VOLTAGE_COUNTER_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16AverageRMSUnderVoltageCounterPhC = 0x0000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSExtremeOverVoltagePeriodPhC = 0x0000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSExtremeUnderVoltagePeriodPhC = 0x0000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SAG_PERIOD_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSVoltageSagPeriodPhC = 0x0000;
            #endif    
            
            #ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SWELL_PERIOD_PHC
                ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16RMSVoltageSwellPeriodPhC = 0x0000;
            #endif 
    
#endif     
                    
#ifdef ELECTRICAL_MEASUREMENT_SERVER
        if(bIsServer)
        {    
            /* As this cluster has reportable attributes enable default reporting */
            vZCL_SetDefaultReporting(psClusterInstance);
        }
 #endif
            ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_ELECTMEAS_FEATURE_MAP;
                
            ((tsCLD_ElectricalMeasurement*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_ELECTMEAS_CLUSTER_REVISION;
        }

    return E_ZCL_SUCCESS;

}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

#endif/* CLD_ELECTRICAL_MEASUREMENT */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

