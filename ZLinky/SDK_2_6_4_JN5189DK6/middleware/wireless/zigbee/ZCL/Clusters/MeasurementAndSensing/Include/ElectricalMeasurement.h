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
 * COMPONENT:          ElectricalMeasurement.h
 *
 * DESCRIPTION:        Header for Electrical Measurement Cluster
 *
 *****************************************************************************/

#ifndef ELECTRICAL_MEASUREMENT_H
#define ELECTRICAL_MEASUREMENT_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/*       Electrical Measurement Cluster - Optional Attributes              */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the temperature measurement cluster.                       */
/****************************************************************************/

/* Enable the optional Tolerance attribute */
//#define CLD_ELECTMEAS_ATTR_AC_FREQUENCY
#define CLD_ELECTMEAS_ATTR_RMS_VOLTAGE
#define CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_PHB
#define CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_PHC
#define CLD_ELECTMEAS_ATTR_RMS_CURRENT
#define CLD_ELECTMEAS_ATTR_RMS_CURRENT_MAX
#define CLD_ELECTMEAS_ATTR_RMS_CURRENT_PHB
#define CLD_ELECTMEAS_ATTR_RMS_CURRENT_MAX_PHB
#define CLD_ELECTMEAS_ATTR_CURRENT_MAX_PHB
#define CLD_ELECTMEAS_ATTR_RMS_CURRENT_PHC
#define CLD_ELECTMEAS_ATTR_RMS_CURRENT_MAX_PHC
#define CLD_ELECTMEAS_ATTR_CURRENT_MAX_PHC

#define CLD_ELECTMEAS_ATTR_TOTAL_ACTIVE_POWER
#define CLD_ELECTMEAS_ATTR_ACTIVE_POWER
#define CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MAX
#define CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MAX_PHB
#define CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MAX_PHC

#define CLD_ELECTMEAS_ATTR_REACTIVE_POWER

#define CLD_ELECTMEAS_ATTR_TOTAL_APPARENT_POWER
#define CLD_ELECTMEAS_ATTR_APPARENT_POWER
#define CLD_ELECTMEAS_ATTR_APPARENT_POWER_PHB
#define CLD_ELECTMEAS_ATTR_APPARENT_POWER_PHC

#define CLD_ELECTMEAS_ATTR_TOTAL_REACTIVE_POWER
#define CLD_ELECTMEAS_ATTR_REACTIVE_POWER_PHB
#define CLD_ELECTMEAS_ATTR_REACTIVE_POWER_PHC


#define CLD_ELECTMEAS_ATTR_ACTIVE_POWER_PHB

#define CLD_ELECTMEAS_ATTR_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD
#define CLD_ELECTMEAS_ATTR_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD_PHB
#define CLD_ELECTMEAS_ATTR_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD_PHC
//#define CLD_ELECTMEAS_ATTR_POWER_FACTOR
//#define CLD_ELECTMEAS_ATTR_AC_VOLTAGE_MULTIPLIER
//#define CLD_ELECTMEAS_ATTR_AC_VOLTAGE_DIVISOR
//#define CLD_ELECTMEAS_ATTR_AC_CURRENT_MULTIPLIER
//#define CLD_ELECTMEAS_ATTR_AC_CURRENT_DIVISOR
//#define CLD_ELECTMEAS_ATTR_AC_POWER_MULTIPLIER
//#define CLD_ELECTMEAS_ATTR_AC_POWER_DIVISOR
/* End of optional attributes */

/* Manufacturer Specific Attributes */
//#define CLD_ELECTMEAS_ATTR_MAN_SPEC_APPARENT_POWER
//#define CLD_ELECTMEAS_ATTR_MAN_SPEC_NON_ACTIVE_POWER
//#define CLD_ELECTMEAS_ATTR_MAN_SPEC_FNDMTL_REACTIVE_POWER
//#define CLD_ELECTMEAS_ATTR_MAN_SPEC_FNDMTL_APPARENT_POWER
//#define CLD_ELECTMEAS_ATTR_MAN_SPEC_FNDMTL_POWER_FACTOR
//#define CLD_ELECTMEAS_ATTR_MAN_SPEC_NON_FNDMTL_APPARENT_POWER
//#define CLD_ELECTMEAS_ATTR_MAN_SPEC_TOTAL_HARMONIC_DISTORTION
//#define CLD_ELECTMEAS_ATTR_MAN_SPEC_VBIAS
//#define CLD_ELECTMEAS_ATTR_MAN_SPEC_DIVISOR

/* Cluster ID's */
#define MEASUREMENT_AND_SENSING_CLUSTER_ID_ELECTRICAL_MEASUREMENT  0x0B04

#ifndef CLD_ELECTMEAS_CLUSTER_REVISION
    #define CLD_ELECTMEAS_CLUSTER_REVISION                         3
#endif 

#ifndef CLD_ELECTMEAS_FEATURE_MAP
    #define CLD_ELECTMEAS_FEATURE_MAP                              0
#endif 

#define CLD_ELECTMEAS_MEASUREMENT_TYPE_ACTIVE           (1 << 0)
#define CLD_ELECTMEAS_MEASUREMENT_TYPE_REACTIVE         (1 << 1)
#define CLD_ELECTMEAS_MEASUREMENT_TYPE_APPARENT         (1 << 2)
#define CLD_ELECTMEAS_MEASUREMENT_TYPE_PHASE_A          (1 << 3)
#define CLD_ELECTMEAS_MEASUREMENT_TYPE_PHASE_B          (1 << 4)
#define CLD_ELECTMEAS_MEASUREMENT_TYPE_PHASE_C          (1 << 5)
#define CLD_ELECTMEAS_MEASUREMENT_TYPE_DC               (1 << 6)
#define CLD_ELECTMEAS_MEASUREMENT_TYPE_HARMONIC         (1 << 7)
#define CLD_ELECTMEAS_MEASUREMENT_TYPE_POWER_QUALITY    (1 << 8)

#define CLD_ELECTMEAS_DC_OVERLOAD_ALARMS_VOLTAGE_OVERLOAD   (1 << 0)
#define CLD_ELECTMEAS_DC_OVERLOAD_ALARMS_CURRENT_OVERLOAD   (1 << 1)

#define CLD_ELECTMEAS_AC_ALARMS_VOLTAGE_OVERLOAD            (1 << 0)
#define CLD_ELECTMEAS_AC_ALARMS_CURRENT_OVERLOAD            (1 << 1)
#define CLD_ELECTMEAS_AC_ALARMS_ACTIVE_POWER_OVERLOAD       (1 << 2)
#define CLD_ELECTMEAS_AC_ALARMS_REACTIVE_POWER_OVERLOAD     (1 << 3)
#define CLD_ELECTMEAS_AC_ALARMS_AVG_RMS_OVER_VOLTAGE        (1 << 4)
#define CLD_ELECTMEAS_AC_ALARMS_AVG_RMS_UNDER_VOLTAGE       (1 << 5)
#define CLD_ELECTMEAS_AC_ALARMS_RMS_EXTREME_OVER_VOLTAGE    (1 << 6)
#define CLD_ELECTMEAS_AC_ALARMS_RMS_EXTREME_UNDER_VOLTAGE   (1 << 7)
#define CLD_ELECTMEAS_AC_ALARMS_RMS_VOLTAGE_SAG             (1 << 8)
#define CLD_ELECTMEAS_AC_ALARMS_RMS_VOLTAGE_SWELL           (1 << 9)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum 
{
    /* Electrical Measurement Basic Information attribute set attribute ID's (9.1.2.2.1) */
    E_CLD_ELECTMEAS_ATTR_ID_MEASUREMENT_TYPE                = 0x0000,  /* Mandatory */
    /* Electrical Measurement DC Measurement attribute set attribute ID's (9.1.2.2.2) */
    E_CLD_ELECTMEAS_ATTR_ID_DC_VOLTAGE                      = 0x0100,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_DC_VOLTAGE_MIN                  = 0x0101,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_DC_VOLTAGE_MAX                  = 0x0102,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_DC_CURRENT                      = 0x0103,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_DC_CURRENT_MIN                  = 0x0104,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_DC_CURRENT_MAX                  = 0x0105,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_DC_POWER                        = 0x0106,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_DC_POWER_MIN                    = 0x0107,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_DC_POWER_MAX                    = 0x0108,     /* Optional */
    /* Electrical Measurement DC Formatting attribute set attribute ID's (9.1.2.2.3) */
    E_CLD_ELECTMEAS_ATTR_ID_DC_VOLTAGE_MULTIPLIER           = 0x0200,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_DC_VOLTAGE_DIVISOR              = 0x0201,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_DC_CURRENT_MULTIPLIER           = 0x0202,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_DC_CURRENT_DIVISOR              = 0x0203,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_DC_POWER_MULTIPLIER             = 0x0204,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_DC_POWER_DIVISOR                = 0x0205,     /* Optional */
    /* Electrical Measurement AC(Non-phase specific) Measurement attribute set attribute ID's (9.1.2.2.4) */
    E_CLD_ELECTMEAS_ATTR_ID_AC_FREQUENCY                    = 0x0300,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AC_FREQUENCY_MIN                = 0x0301,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AC_FREQUENCY_MAX                = 0x0302,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_NEUTRAL_CURRENT                 = 0x0303,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_TOTAL_ACTIVE_POWER              = 0x0304,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_TOTAL_REACTIVE_POWER            = 0x0305,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_TOTAL_APPARENT_POWER            = 0x0306,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_MEASURED_1_HARMONIC_CURRENT     = 0x0307,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_MEASURED_3_HARMONIC_CURRENT     = 0x0308,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_MEASURED_5_HARMONIC_CURRENT     = 0x0309,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_MEASURED_7_HARMONIC_CURRENT     = 0x030a,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_MEASURED_9_HARMONIC_CURRENT     = 0x030b,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_MEASURED_11_HARMONIC_CURRENT    = 0x030c,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_MEASURED_PHASE_1_HARMONIC_CURRENT        = 0x030d,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_MEASURED_PHASE_3_HARMONIC_CURRENT        = 0x030e,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_MEASURED_PHASE_5_HARMONIC_CURRENT        = 0x030f,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_MEASURED_PHASE_7_HARMONIC_CURRENT        = 0x0310,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_MEASURED_PHASE_9_HARMONIC_CURRENT        = 0x0311,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_MEASURED_PHASE_11_HARMONIC_CURRENT       = 0x0312,     /* Optional */
    /* Electrical Measurement AC(Non-phase specific) Formatting attribute set attribute ID's (9.1.2.2.5) */
    E_CLD_ELECTMEAS_ATTR_ID_AC_FREQUENCY_MULTIPLIER         = 0x0400,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AC_FREQUENCY_DIVISOR            = 0x0401,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_POWER_MULTIPLIER                = 0x0402,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_POWER_DIVISOR                   = 0x0403,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_HARMONIC_CURRENT_MULTIPLIER     = 0x0404,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_PHASE_HARMONIC_CURRENT_MULTIPLIER        = 0x0405,     /* Optional */
    /* Electrical Measurement AC(Single Phase or Phase A) Single Phase or Phase A attribute set attribute ID's (9.1.2.2.6) */    
    E_CLD_ELECTMEAS_ATTR_ID_LINE_CURRENT                    = 0x0501,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_CURRENT                  = 0x0502,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_REACTIVE_CURRENT                = 0x0503,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE                     = 0x0505,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_MIN                 = 0x0506,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_MAX                 = 0x0507,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT                     = 0x0508,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT_MIN                 = 0x0509,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT_MAX                 = 0x050A,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER                    = 0x050B,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_MIN                = 0x050C,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_MAX                = 0x050D,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_REACTIVE_POWER                  = 0x050E,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_APPARENT_POWER                  = 0x050F,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_POWER_FACTOR                    = 0x0510,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD       = 0x0511,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_OVER_VOLTAGE_COUNTER             = 0x0512,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_UNDER_VOLTAGE_COUNTER            = 0x0513,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_OVER_VOLTAGE_PERIOD          = 0x0514,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_UNDER_VOLTAGE_PERIOD         = 0x0515,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SAG_PERIOD          = 0x0516,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SWELL_PERIOD        = 0x0517,     /* Optional */   
    /* Electrical Measurement AC(Single Phase or Phase A) Formatting attribute set attribute ID's (9.1.2.2.7) */
    E_CLD_ELECTMEAS_ATTR_ID_AC_VOLTAGE_MULTIPLIER           = 0x0600,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AC_VOLTAGE_DIVISOR              = 0x0601,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AC_CURRENT_MULTIPLIER           = 0x0602,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AC_CURRENT_DIVISOR              = 0x0603,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AC_POWER_MULTIPLIER             = 0x0604,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AC_POWER_DIVISOR                = 0x0605,     /* Optional */
    /* Electrical Measurement DC Manufacturer Threshold Alarms attribute set attribute ID's (9.1.2.2.8) */
    E_CLD_ELECTMEAS_ATTR_ID_DC_OVERLOAD_ALARMS_MASK         = 0x0700,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_DC_VOLTAGE_OVERLOAD             = 0x0701,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_DC_CURRENT_OVERLOAD             = 0x0702,     /* Optional */    
    /* Electrical Measurement AC Manufacturer Threshold Alarms attribute set attribute ID's (9.1.2.2.9) */
    E_CLD_ELECTMEAS_ATTR_ID_AC_ALARMS_MASK                  = 0x0800,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AC_VOLTAGE_OVERLOAD             = 0x0801,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AC_CURRENT_OVERLOAD             = 0x0802,     /* Optional */  
    E_CLD_ELECTMEAS_ATTR_ID_AC_ACTIVE_POWER_OVERLOAD        = 0x0803,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AC_REACTIVE_POWER_OVERLOAD      = 0x0804,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_OVER_VOLTAGE            = 0x0805,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_UNDER_VOLTAGE           = 0x0806,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_OVER_VOLTAGE        = 0x0807,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_UNDER_VOLTAGE       = 0x0808,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SAG                 = 0x0809,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SWELL               = 0x080A,     /* Optional */   
    /* Electrical Measurement AC Phase B  Measurement attribute set attribute ID's (9.1.2.2.10) */
    E_CLD_ELECTMEAS_ATTR_ID_LINE_CURRENT_PHB                = 0x0901,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_CURRENT_PHB              = 0x0902,     /* Optional */  
    E_CLD_ELECTMEAS_ATTR_ID_REACTIVE_CURRENT_PHB            = 0x0903,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_PHB                 = 0x0905,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_MIN_PHB             = 0x0906,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_MAX_PHB             = 0x0907,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT_PHB                 = 0x0908,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT_MIN_PHB             = 0x0909,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT_MAX_PHB             = 0x090A,     /* Optional */ 
    E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_PHB                = 0x090B,     /* Optional */ 
    E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_MIN_PHB             = 0x090C,     /* Optional */  
    E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_MAX_PHB            = 0x090D,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_REACTIVE_POWER_PHB              = 0x090E,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_APPARENT_POWER_PHB              = 0x090F,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_POWER_FACTOR_PHB                = 0x0910,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD_PHB        = 0x0911,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_OVER_VOLTAGE_COUNTER_PHB              = 0x0912,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_UNDER_VOLTAGE_COUNTER_PHB             = 0x0913,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PHB           = 0x0914,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PHB          = 0x0915,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SAG_PERIOD_PHB      = 0x0916,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SWELL_PERIOD_PHB    = 0x0917,     /* Optional */    
    /* Electrical Measurement AC Phase C  Measurement attribute set attribute ID's (9.1.2.2.11) */ 
    E_CLD_ELECTMEAS_ATTR_ID_LINE_CURRENT_PHC                = 0x0A01,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_CURRENT_PHC              = 0x0A02,     /* Optional */  
    E_CLD_ELECTMEAS_ATTR_ID_REACTIVE_CURRENT_PHC            = 0x0A03,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_PHC                 = 0x0A05,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_MIN_PHC             = 0x0A06,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_MAX_PHC             = 0x0A07,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT_PHC                 = 0x0A08,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT_MIN_PHC             = 0x0A09,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_CURRENT_MAX_PHC             = 0x0A0A,     /* Optional */ 
    E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_PHC                = 0x0A0B,     /* Optional */ 
    E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_MIN_PHC             = 0x0A0C,     /* Optional */  
    E_CLD_ELECTMEAS_ATTR_ID_ACTIVE_POWER_MAX_PHC            = 0x0A0D,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_REACTIVE_POWER_PHC              = 0x0A0E,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_APPARENT_POWER_PHC              = 0x0A0F,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_POWER_FACTOR_PHC                = 0x0A10,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD_PHC        = 0x0A11,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_OVER_VOLTAGE_COUNTER_PHC              = 0x0A12,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_AVG_RMS_UNDER_VOLTAGE_COUNTER_PHC             = 0x0A13,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PHC           = 0x0A14,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PHC          = 0x0A15,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SAG_PERIOD_PHC      = 0x0A16,     /* Optional */
    E_CLD_ELECTMEAS_ATTR_ID_RMS_VOLTAGE_SWELL_PERIOD_PHC    = 0x0A17,     /* Optional */    
    /* Manufacturer Specific Attributes */
	E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_APPARENT_POWER         = 0xFF00,     /* Man Spec */
	E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_NON_ACTIVE_POWER,                    /* Man Spec */
	E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_FNDMTL_REACTIVE_POWER,               /* Man Spec */
	E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_FNDMTL_APPARENT_POWER,               /* Man Spec */
	E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_FNDMTL_POWER_FACTOR,                 /* Man Spec */
	E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_NON_FNDMTL_APPARENT_POWER,           /* Man Spec */
	E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_TOTAL_HARMONIC_DISTORTION,           /* Man Spec */
	E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_VBIAS,                               /* Man Spec */
	E_CLD_ELECTMEAS_ATTR_ID_MAN_SPEC_DIVISOR,                             /* Man Spec */
} teCLD_ElectricalMeasurement_AttributeID;

/* Electrical Measurement Cluster */
typedef struct
{

#ifdef ELECTRICAL_MEASUREMENT_SERVER
    zbmap32                u32MeasurementType;
     
#ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE  
    zint16                 i16DCVoltage;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE_MIN 
    zint16                 i16DCVoltageMin;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE_MAX
    zint16                 i16DCVoltageMax;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT  
    zint16                 i16DCCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT_MIN 
    zint16                 i16DCCurrentMin;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT_MAX  
    zint16                 i16DCCurrentMax;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_DC_POWER    
    zint16                 i16DCPower;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_DC_POWER_MIN  
    zint16                 i16DCPowerMin;
#endif
      
#ifdef CLD_ELECTMEAS_ATTR_DC_POWER_MAX 
    zint16                 i16DCPowerMax;
#endif

#ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE_MULTIPLIER  
    zuint16                u16DCVoltageMultiplier;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE_DIVISOR 
    zuint16                u16DCVoltageDivisor;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT_MULTIPLIER  
    zuint16                u16DCCurrentMultiplier;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT_DIVISOR     
    zuint16                u16DCCurrentDivisor;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_DC_POWER_MULTIPLIER
    zuint16                u16DCPowerMultiplier;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_DC_POWER_DIVISOR
    zuint16                u16DCPowerDivisor;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AC_FREQUENCY
    zuint16                u16ACFrequency;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AC_FREQUENCY_MIN  
    zuint16                u16ACFrequencyMin;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AC_FREQUENCY_MAX 
    zuint16                u16ACFrequencyMax;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_NEUTRAL_CURRENT  
    zuint16                u16NeutralCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_TOTAL_ACTIVE_POWER 
    zint32                 i32TotalActivePower;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_TOTAL_REACTIVE_POWER
    zint32                 i32TotalReactivePower;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_TOTAL_APPARENT_POWER 
    zuint32                 u32TotalApparentPower;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_MEASURED_1_HARMONIC_CURRENT  
    zint16                 i16Measured1stHarmonicCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_MEASURED_3_HARMONIC_CURRENT   
    zint16                 i16Measured3rdHarmonicCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_MEASURED_5_HARMONIC_CURRENT  
    zint16                 i16Measured5thHarmonicCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_MEASURED_7_HARMONIC_CURRENT
    zint16                 i16Measured7thHarmonicCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_MEASURED_9_HARMONIC_CURRENT    
    zint16                 i16Measured9thHarmonicCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_MEASURED_11_HARMONIC_CURRENT   
    zint16                 i16Measured11thHarmonicCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_MEASURED_PHASE_1_HARMONIC_CURRENT  
    zint16                 i16MeasuredPhase1stHarmonicCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_MEASURED_PHASE_3_HARMONIC_CURRENT   
    zint16                 i16MeasuredPhase3rdHarmonicCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_MEASURED_PHASE_5_HARMONIC_CURRENT 
    zint16                 i16MeasuredPhase5thHarmonicCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_MEASURED_PHASE_7_HARMONIC_CURRENT 
    zint16                 i16MeasuredPhase7thHarmonicCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_MEASURED_PHASE_9_HARMONIC_CURRENT        
    zint16                 i16MeasuredPhase9thHarmonicCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_MEASURED_PHASE_11_HARMONIC_CURRENT    
    zint16                 i16MeasuredPhase11thHarmonicCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AC_FREQUENCY_MULTIPLIER  
    zuint16                u16ACFrequencyMultiplier;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AC_FREQUENCY_DIVISOR  
    zuint16                u16ACFrequencyDivisor;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_POWER_MULTIPLIER
    zuint32                u32PowerMultiplier;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_POWER_DIVISOR 
    zuint32                u32PowerDivisor;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_HARMONIC_CURRENT_MULTIPLIER  
    zint8                  i8HarmonicCurrentMultiplier;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_PHASE_HARMONIC_CURRENT_MULTIPLIER
    zint8                  i8PhaseHarmonicCurrentMultiplier;
#endif

#ifdef CLD_ELECTMEAS_ATTR_LINE_CURRENT   
    zuint16                u16LineCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_ACTIVE_CURRENT 
    zint16                 i16ActiveCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_REACTIVE_CURRENT 
    zint16                 i16ReactiveCurrent;
#endif

#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE
    zuint16                u16RMSVoltage;
#endif

#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MIN  
    zuint16                u16RMSVoltageMin;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MAX 
    zuint16                u16RMSVoltageMax;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT
    zuint16                u16RMSCurrent;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_MIN   
    zuint16                u16RMSCurrentMin;
#endif  
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_MAX
    zuint16                u16RMSCurrentMax;
#endif    
    
#ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER
    zint16                 i16ActivePower;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MIN 
    zint16                 i16ActivePowerMin;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MAX 
    zint16                 i16ActivePowerMax;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_REACTIVE_POWER
    zint16                 i16ReactivePower;
#endif

#ifdef CLD_ELECTMEAS_ATTR_APPARENT_POWER
    zuint16                u16ApparentPower;
#endif

#ifdef CLD_ELECTMEAS_ATTR_POWER_FACTOR
    zint8                  i8PowerFactor;
#endif

#ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD   
    zuint16                u16AverageRMSVoltageMeasurementPeriod;
#endif

#ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_OVER_VOLTAGE_COUNTER
    zuint16                u16AverageRMSOverVoltageCounter;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_UNDER_VOLTAGE_COUNTER
    zuint16                u16AverageRMSUnderVoltageCounter;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_OVER_VOLTAGE_PERIOD
    zuint16                u16RMSExtremeOverVoltagePeriod;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_UNDER_VOLTAGE_PERIOD
    zuint16                u16RMSExtremeUnderVoltagePeriod;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SAG_PERIOD    
    zuint16                u16RMSVoltageSagPeriod;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SWELL_PERIOD 
    zuint16                u16RMSVoltageSwellPeriod;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AC_VOLTAGE_MULTIPLIER
    zuint16                u16ACVoltageMultiplier;
#endif

#ifdef CLD_ELECTMEAS_ATTR_AC_VOLTAGE_DIVISOR
    zuint16                u16ACVoltageDivisor;
#endif

#ifdef CLD_ELECTMEAS_ATTR_AC_CURRENT_MULTIPLIER
    zuint16                u16ACCurrentMultiplier;
#endif

#ifdef CLD_ELECTMEAS_ATTR_AC_CURRENT_DIVISOR
    zuint16                u16ACCurentDivisor;
#endif

#ifdef CLD_ELECTMEAS_ATTR_AC_POWER_MULTIPLIER
    zuint16                u16ACPowerMultiplier;
#endif

#ifdef CLD_ELECTMEAS_ATTR_AC_POWER_DIVISOR
    zuint16                u16ACPowerDivisor;
#endif

#ifdef CLD_ELECTMEAS_ATTR_DC_OVERLOAD_ALARMS_MASK  
    zbmap8                 u8DCOverloadAlarmsMask;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_DC_VOLTAGE_OVERLOAD  
    zint16                 i16DCVoltageOverload;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_DC_CURRENT_OVERLOAD   
    zint16                 i16DCCurrentOverload;
#endif

#ifdef CLD_ELECTMEAS_ATTR_AC_ALARMS_MASK 
    zbmap16                u16ACAlarmsMask;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AC_VOLTAGE_OVERLOAD
    zint16                 i16ACVoltageOverload;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AC_CURRENT_OVERLOAD
    zint16                 i16ACCurrentOverload;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AC_ACTIVE_POWER_OVERLOAD 
    zint16                 i16ACActivePowerOverload;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AC_REACTIVE_POWER_OVERLOAD 
    zint16                 i16ACReactivePowerOverload;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_OVER_VOLTAGE 
    zint16                 i16AverageRMSOverVoltage;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_UNDER_VOLTAGE 
    zint16                 i16AverageRMSUnderVoltage;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_OVER_VOLTAGE  
    zint16                 i16RMSExtremeOverVoltage;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_UNDER_VOLTAGE 
    zint16                 i16RMSExtremeUnderVoltage;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SAG      
    zint16                 i16RMSVoltageSag;
#endif

#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SWELL  
    zint16                 i16RMSVoltageSwell;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_LINE_CURRENT_PHB  
    zuint16                u16LineCurrentPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_ACTIVE_CURRENT_PHB  
    zint16                 i16ActiveCurrentPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_REACTIVE_CURRENT_PHB    
    zint16                 i16ReactiveCurrentPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_PHB     
    zuint16                u16RMSVoltagePhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MIN_PHB    
    zuint16                u16RMSVoltageMinPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MAX_PHB   
    zuint16                u16RMSVoltageMaxPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_PHB      
    zuint16                u16RMSCurrentPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_MIN_PHB    
    zuint16                u16RMSCurrentMinPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_MAX_PHB 
    zuint16                u16RMSCurrentMaxPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_PHB  
    zint16                 i16ActivePowerPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MIN_PHB 
    zint16                 i16ActivePowerMinPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MAX_PHB 
    zint16                 i16ActivePowerMaxPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_REACTIVE_POWER_PHB   
    zint16                  i16ReactivePowerPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_APPARENT_POWER_PHB  
    zuint16                u16ApparentPowerPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_POWER_FACTOR_PHB   
    zint8                  i8PowerFactorPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD_PHB    
    zuint16                u16AverageRMSVoltageMeasurementPeriodPhB;
#endif

#ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_OVER_VOLTAGE_COUNTER_PHB  
    zuint16                u16AverageRMSOverVoltageCounterPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_UNDER_VOLTAGE_COUNTER_PHB    
    zuint16                u16AverageRMSUnderVoltageCounterPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PHB  
    zuint16                u16RMSExtremeOverVoltagePeriodPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PHB  
    zuint16                u16RMSExtremeUnderVoltagePeriodPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SAG_PERIOD_PHB 
    zuint16               u16RMSVoltageSagPeriodPhB;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SWELL_PERIOD_PHB     
    zuint16                u16RMSVoltageSwellPeriodPhB;
#endif

#ifdef CLD_ELECTMEAS_ATTR_LINE_CURRENT_PHC  
    zuint16                u16LineCurrentPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_ACTIVE_CURRENT_PHC  
    zint16                 i16ActiveCurrentPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_REACTIVE_CURRENT_PHC    
    zint16                  i16ReactiveCurrentPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_PHC     
    zuint16                 u16RMSVoltagePhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MIN_PHC    
    zuint16                 u16RMSVoltageMinPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_MAX_PHC   
    zuint16                  u16RMSVoltageMaxPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_PHC      
    zuint16                  u16RMSCurrentPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_MIN_PHC    
    zuint16                  u16RMSCurrentMinPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_CURRENT_MAX_PHC 
    zuint16                  u16RMSCurrentMaxPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_PHC  
    zint16                   i16ActivePowerPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MIN_PHC 
    zint16                   i16ActivePowerMinPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_ACTIVE_POWER_MAX_PHC 
    zint16                   i16ActivePowerMaxPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_REACTIVE_POWER_PHC   
    zint16                   i16ReactivePowerPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_APPARENT_POWER_PHC  
    zuint16                  u16ApparentPowerPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_POWER_FACTOR_PHC   
    zint8                    i8PowerFactorPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_VOLTAGE_MEASUREMENT_PERIOD_PHC    
    zuint16                  u16AverageRMSVoltageMeasurementPeriodPhC;
#endif

#ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_OVER_VOLTAGE_COUNTER_PHC  
    zuint16                  u16AverageRMSOverVoltageCounterPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_AVG_RMS_UNDER_VOLTAGE_COUNTER_PHC    
    zuint16                  u16AverageRMSUnderVoltageCounterPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PHC  
    zuint16                  u16RMSExtremeOverVoltagePeriodPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PHC  
    zuint16                  u16RMSExtremeUnderVoltagePeriodPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SAG_PERIOD_PHC 
    zuint16                  u16RMSVoltageSagPeriodPhC;
#endif
    
#ifdef CLD_ELECTMEAS_ATTR_RMS_VOLTAGE_SWELL_PERIOD_PHC     
    zuint16                  u16RMSVoltageSwellPeriodPhC;
#endif 
    
#ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_APPARENT_POWER
    zuint32                 u32ManSpecificApparentPower;
#endif

#ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_NON_ACTIVE_POWER
    zuint32                 u32NonActivePower;
#endif

#ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_FNDMTL_REACTIVE_POWER
    zint32                  i32FundamentalReactivePower;
#endif

#ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_FNDMTL_APPARENT_POWER
    zuint32                 u32FundamentalApparentPower;
#endif

#ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_FNDMTL_POWER_FACTOR
    zuint16                 u16FundamentalPowerFactor;
#endif

#ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_NON_FNDMTL_APPARENT_POWER
    zuint32                 u32NonFundamentalApparentPower;
#endif

#ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_TOTAL_HARMONIC_DISTORTION
    zuint32                 u32TotalHarmonicDistortion;
#endif

#ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_VBIAS
    zuint32                 u32VBias;
#endif

#ifdef CLD_ELECTMEAS_ATTR_MAN_SPEC_DIVISOR
    zuint16                 u16ManSpecDivisor;
#endif

#endif
    zbmap32                u32FeatureMap;

    zuint16                u16ClusterRevision;
    
#ifdef CLD_ELECTMEAS_ATTR_ATTRIBUTE_REPORTING_STATUS
    zenum8                 u8AttributeReportingStatus;
#endif

} tsCLD_ElectricalMeasurement;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_ElectricalMeasurementCreateElectricalMeasurement(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern tsZCL_ClusterDefinition sCLD_ElectricalMeasurement;
extern uint8 au8ElectricalMeasurementAttributeControlBits[];
extern const tsZCL_AttributeDefinition asCLD_ElectricalMeasurementClusterAttributeDefinitions[];
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* ELECTRICAL_MEASUREMENT_H */
