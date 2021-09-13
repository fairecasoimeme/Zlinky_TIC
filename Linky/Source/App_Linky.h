/*****************************************************************************
 *
 * MODULE:             JN-AN-1220 ZLO Sensor Demo
 *
 * COMPONENT:          App_LightSensor.h
 *
 * DESCRIPTION:        ZLO Demo Light Sensor -Implementation
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5169,
 * JN5179, JN5189].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2016-2018. All rights reserved
 *
 ***************************************************************************/

#ifndef APP_LINKY_H_
#define APP_LINKY_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "linky_sensor.h"
#include "zcl_options.h"
#include "app_reporting.h"


/****************************************************************************/
/***        Macro Definition                                              ***/
/****************************************************************************/
#define LINKY_MINIMUM_MEASURED_VALUE                            0x0001
#define LINKY_MAXIMUM_MEASURED_VALUE                            0xFAF
#define LINKY_MINIMUM_REPORTABLE_CHANGE                         1
#define LINKY_SAMPLING_TIME_IN_SECONDS                          5
#define APP_TICKS_PER_SECOND    1000
#define APP_JOINING_BLINK_TIME          (ZTIMER_TIME_MSEC(1000))
#define APP_FIND_AND_BIND_BLINK_TIME    (ZTIMER_TIME_MSEC(500))
#define APP_KEEP_AWAKE_TIME             (ZTIMER_TIME_MSEC(250))

#define LED_LEVEL                                                      20
#define LED_MAX_LEVEL                                                  255
#define LED_MIN_LEVEL                                                  0

#define FAILED_POLL_COUNT                                              3
#define FAILED_REJOIN_COUNT                                            3

#define SENSOR_OTA_SLEEP_IN_SECONDS                                    1
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern tsZLO_LinkyDevice sSensor;
extern tsReports asDefaultReports[ZCL_NUMBER_OF_REPORTS];
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

void vAPP_ZCL_DeviceSpecific_Init(void);
teZCL_Status eApp_ZCL_RegisterEndpoint(tfpZCL_ZCLCallBackFunction fptr);
PUBLIC void vAPP_ZCL_DeviceSpecific_UpdateIdentify(void);
PUBLIC void vAPP_ZCL_DeviceSpecific_SetIdentifyTime(uint16 u16Time);
PUBLIC void vAPP_ZCL_DeviceSpecific_IdentifyOff(void);
PUBLIC void APP_cbTimerLinkySample(void *pvParam);
PUBLIC void vAPP_LinkySensorSample(void);
PUBLIC uint8 app_u8GetDeviceEndpoint( void);


/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* APP_LIGHT_SENSOR_H_ */
