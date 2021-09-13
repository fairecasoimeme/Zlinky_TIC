/*****************************************************************************
 *
 * MODULE:             JN-AN-1220 ZLO Sensor Demo
 *
 * COMPONENT:          app_common.h
 *
 * DESCRIPTION:        Application Device Type Includes
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

#ifndef APP_COMMON_H_
#define APP_COMMON_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "ZTimer.h"
#include "ZQueue.h"
/* Device specific includes
 * As a new light or controller is added it needs an entry here
 *
 */
#ifdef DimmerSwitch
    #include "App_DimmerSwitch.h"
#endif

#ifdef ColorDimmerSwitch
    #include "App_ColorDimmerSwitch.h"
#endif

#ifdef DimmableLight
    #include "App_DimmableLight.h"
#endif

#ifdef OTAColorDimmableLightWithOccupancy
    #include "App_OTAColorDimmableLightWithOccupancy.h"
#endif

#ifdef ColorTempTunableWhiteLight
    #include "App_ColorTempTunableWhiteLight.h"
#endif

#ifdef ColorDimmableLight
    #include "App_ColorDimmableLight.h"
#endif

#ifdef OccupancySensor
    #include "App_OccupancySensor.h"
#endif

#ifdef LightSensor
    #include "App_LightSensor.h"
#endif

#ifdef Linky
    #include "App_Linky.h"
	//#include "metering_sensor.h"
#endif

#ifdef LTOSensor
    #include "App_LTOSensor.h"
#endif

#ifdef RemoteControl
    #include "App_RemoteControl.h"
#endif
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define POLL_TIME               ZTIMER_TIME_MSEC(1000)
#define POLL_TIME_FAST          ZTIMER_TIME_MSEC(100)
#define TEN_HZ_TICK_TIME        ZTIMER_TIME_MSEC(100)
#define MAX_PACKET_SIZE         270

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef enum
{
    E_STARTUP,
    E_JOINING_NETWORK,
    E_RUNNING
} teNODE_STATES;

typedef teNODE_STATES teNodeState;

typedef struct
{
    teNODE_STATES   eNodeState;
    teNODE_STATES   eNodePrevState;
#ifdef CLD_OTA
    bool        bValid;
    uint64      u64IeeeAddrOfServer;
    uint16      u16NwkAddrOfServer;
    uint8       u8OTAserverEP;
#endif
}tsDeviceDesc;

#if (defined JENNIC_CHIP_FAMILY_JN517x)
#define SDA_PIN    (2)
#define SCL_PIN    (3)
#else
#define SDA_PIN    (15)
#define SCL_PIN    (14)
#endif
#define IIC_MASK    ((1<<SDA_PIN) | (1<<SCL_PIN))

#define ZNC_RTN_U64( BUFFER, i )  ( ( ( uint64 ) ( BUFFER )[ i ]  <<  56) |\
    ( ( uint64 ) ( BUFFER )[ i + 1 ]  << 48) |\
    ( ( uint64 ) ( BUFFER )[ i + 2 ]  << 40) |\
    ( ( uint64 ) ( BUFFER )[ i + 3 ]  << 32) |\
    ( ( uint64 ) ( BUFFER )[ i + 4 ]  << 24) |\
    ( ( uint64 ) ( BUFFER )[ i + 5 ]  << 16) |\
    ( ( uint64 ) ( BUFFER )[ i + 6 ]  << 8) |\
    ( ( uint64 ) ( BUFFER )[ i + 7 ] & 0xFF))\

#define ZNC_RTN_U32( BUFFER, i ) ( ( ( uint32 ) ( BUFFER )[ i ] << 24) |\
    ( ( uint32 ) ( BUFFER )[ i + 1 ]  << 16) |\
    ( ( uint32 ) ( BUFFER )[ i + 2 ]  << 8) |\
    ( ( uint32 ) ( BUFFER )[ i + 3 ] & 0xFF))\

#define ZNC_RTN_U16( BUFFER, i ) ( ( ( uint16 ) (BUFFER)[ i ] << 8) |\
    ( ( uint16 ) (BUFFER)[ i + 1 ] & 0xFF))\

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
extern tszQueue                   APP_msgSerialRx;
extern uint8 APP_vProcessRxData ( void );
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#endif /*APP_COMMON_H_*/
