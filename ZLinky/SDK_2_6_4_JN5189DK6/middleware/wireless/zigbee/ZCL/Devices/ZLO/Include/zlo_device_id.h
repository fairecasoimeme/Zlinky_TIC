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
 * MODULE:             Zigbee Lighting and Occupancy 
 *
 * COMPONENT:          zlo_device_id.h
 *
 * DESCRIPTION:        Header for ZigBee Lighting and Occupancy Device Id file
 *
 *****************************************************************************/

#ifndef ZLO_DEVICE_ID_H
#define ZLO_DEVICE_ID_H

#if defined __cplusplus
extern "C" {
#endif

#include <jendefs.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* ZLO device types  */
#define ZLO_ON_OFF_LIGHT_DEVICE_ID                  0x0100
#define ZLO_DIMMABLE_LIGHT_DEVICE_ID                0x0101
#define ZLO_COLOUR_DIMMABLE_LIGHT_DEVICE_ID         0x0102
#define ZLO_ON_OFF_LIGHT_SWITCH_DEVICE_ID           0x0103
#define ZLO_DIMMER_SWITCH_DEVICE_ID                 0x0104
#define ZLO_COLOUR_DIMMER_SWITCH_DEVICE_ID          0x0105
#define ZLO_LIGHT_SENSOR_DEVICE_ID                  0x0106
#define ZLO_OCCUPANCY_SENSOR_DEVICE_ID              0x0107
#define ZLO_ON_OFF_BALLAST_DEVICE_ID                0x0108
#define ZLO_DIMMABLE_BALLAST_DEVICE_ID              0x0109
#define ZLO_ON_OFF_PLUG_DEVICE_ID                   0x010A
#define ZLO_DIMMABLE_PLUG_DEVICE_ID                 0x010B
#define ZLO_COLOUR_TEMPERATURE_LIGHT_DEVICE_ID      0x010C
#define ZLO_EXTENDED_COLOUR_LIGHT_DEVICE_ID         0x010D
#define ZLO_LIGHT_LEVEL_SENSOR_DEVICE_ID            0x010E

#define ZLO_COLOUR_CONTROLLER_DEVICE_ID             0x0800
#define ZLO_COLOUR_SCENE_CONTROLLER_DEVICE_ID       0x0810
#define ZLO_NON_COLOUR_CONTROLLER_DEVICE_ID         0x0820
#define ZLO_NON_COLOUR_SCENE_CONTROLLER_DEVICE_ID   0x0830
#define ZLO_CONTROL_BRIDGE_DEVICE_ID                0x0840
#define ZLO_ON_OFF_SENSOR_DEVICE_ID                 0x0850

/* Legacy ZLL Device IDs */
#define ON_OFF_LIGHT_DEVICE_ID             0x0000
#define ON_OFF_PLUG_DEVICE_ID              0x0010
#define DIMMABLE_LIGHT_DEVICE_ID           0x0100
#define DIMMABLE_PLUG_DEVICE_ID            0x0110
#define COLOUR_LIGHT_DEVICE_ID             0x0200
#define EXTENDED_COLOUR_LIGHT_DEVICE_ID     0x0210
#define COLOUR_TEMPERATURE_LIGHT_DEVICE_ID  0x0220

#define COLOUR_REMOTE_DEVICE_ID            0x0800
#define COLOUR_SCENE_REMOTE_DEVICE_ID      0x0810
#define NON_COLOUR_REMOTE_DEVICE_ID        0x0820
#define NON_COLOUR_SCENE_REMOTE_DEVICE_ID  0x0830
#define CONTROL_BRIDGE_DEVICE_ID           0x0840
#define ON_OFF_SENSOR_DEVICE_ID            0x0850

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* ZLO_DEVICE_ID_H */
