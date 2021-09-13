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


/****************************************************************************
 *
 * MODULE:			zps_mac_if_table.h
 *
 * COMPONENT:       ZPSMAC Interface Table
 *
 * DESCRIPTION:		ZPS MAC Interface table definitions
 *
 ****************************************************************************/

#ifndef  ZPS_MAC_IF_TABLE_H_INCLUDED
#define  ZPS_MAC_IF_TABLE_H_INCLUDED


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>



#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifndef ZPS_MAC_IS_MULTIMAC
#define ZPS_MAC_IS_MULTIMAC	FALSE
#endif

#define MAC_ID_DEFAULT							(0)		/* This is the default MAC on which the Zigbee network first forms */
#define MAC_ID_SOC								(0)
#define MAC_ID_SERIAL1 							(1)
#define MAC_ID_SERIAL2 							(2)
#define MAC_ID_AUX								MAC_ID_SERIAL1

#define MAC_ID_ALL								(0xFFFE)
#define MAC_ID_UNKNOWN 							(0xFFFF)

#define MAC_CHANNEL_MASK_LIST_TERMINATOR		0xFFFFFFFF

#define MAC_ALL_CHANNELS_MASK_PAGE0				(( 0 << 27) | 0x07FFF800)
#define MAC_ALL_CHANNELS_MASK_PAGE28			((28 << 27) | 0x07FFFFFF)
#define MAC_ALL_CHANNELS_MASK_PAGE29			((29 << 27) | 0x000000FF)
#define MAC_ALL_CHANNELS_MASK_PAGE30			((30 << 27) | 0x00003FFF)
#define MAC_ALL_CHANNELS_MASK_PAGE31			((31 << 27) | 0x00001FFF)
#define MAC_MI_ROUTER_ALLOWED(x)                ( ( ( (x) & 0x2 ) >> 1 ) )
#define MAC_MI_ENABLED(x)                       ( (x) & 0x1 )
#define MAC_MI_CHANNEL_COUNT(x)                 ( ( ( (x) & 0x3C ) >> 2 ) )
#define MAC_MI_SET_STATE(x)                     ( (x) | 0x1  )
#define MAC_MI_CLEAR_STATE(x)                   ( (x) & (~(0x01)) )
#define MAC_MI_SET_ROUTER_ALLOWED(x)            ( (x) | (0x1 << 1u))
#define MAC_MI_CLEAR_ROUTER_ALLOWED(x)          ( (x) & (~(0x1 << 1u)))



/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* ZPS_MULTIMASK_SUPPORT ...*/

typedef bool_t (*PR_APP_JOIN_CALLBACK)(void *);  /* void * = ZPS_tsNwkNetworkDescr */

typedef enum zps_teAplAfMMServerState_e
{
    ZPS_E_MULTIMASK_STATE_NOT_INIT=0,
    ZPS_E_MULTIMASK_STATE_IDLE,
    ZPS_E_MULTIMASK_STATE_DISCOVERY,
    ZPS_E_MULTIMASK_STATE_FORMING,
    ZPS_E_MULTIMASK_STATE_JOINING,
    ZPS_E_MULTIMASK_STATE_REJOINING

} zps_teAplAfMMServerState;

typedef struct
{
	zps_teAplAfMMServerState		 eState;
    uint8				  			 u8ChanMaskListCount;
    uint8				   			 u8ChanMaskListActvIndex;
    PR_APP_JOIN_CALLBACK			 prAppJoinCallback;

} zps_tsAplAfMMServerContext;

/* ... ZPS_MULTIMASK_SUPPORT */


typedef struct
{
	uint8						u8ActiveChannel;
	uint8                       u8Bitmask; /* (bit 0 <Enabled>, bit 1 <RouterAllowed> bit 2-5 <channel counts> bit6-7 free  ) */
	uint8 						u8RadioType;
	uint8						u8MacType;
} MAC_tsMacInterface;


typedef struct
{
    zps_tsAplAfMMServerContext	*psMultiMaskServer;    /* ZPS_MULTIMASK_SUPPORT used by MAC_ID_DEFAULT */
    MAC_tsMacInterface          *psMacInterfaces;
    uint32*                     pu32ChannelMaskList;
    uint32*                     pu32MacTxUcastAvgRetry;
    uint32*                     pu32MacTxUcastRetry;
    uint32*                     pu32MacTxUcastFail ;
    uint32*                     pu32MacRxUcast ;
    uint32*                     pu32MacTxUcast ;
    uint32*                     pu32MacRxBcast;
    uint32*                     pu32MacTxBcast;
    uint32*                     pu32MacCCaFail;
    uint32*                     pu32ApsRetry;
    uint16*                     pu16PacketValidateDropCount;
    uint16*                     pu16PacketBufferAllocateFailure;
    uint16*                     pu16PhyToMacQueDrop;
    bool_t*                     pbIgnoreBroadcasts;
    uint8                       u8NumInterfaces;
} MAC_tsMacInterfaceTable;

typedef enum MAC_teRadioType_tag          /* Indicates MAC Radio Frequency types */
{
    E_MAC_FREQ_2400=0,
    E_MAC_FREQ_868=1,
    E_MAC_FREQ_NOT_KNOWN=0xFE,
    E_MAC_FREQ_NOT_SUPPORTED=0xFF,

} MAC_teRadioType;

typedef enum MAC_teMacType_tag				/* Indicates MAC type */
{
	E_MAC_TYPE_UART0=0x00,
	E_MAC_TYPE_UART1=0x01,
	E_MAC_TYPE_UART2=0x02,
	E_MAC_TYPE_UART3=0x03,
	E_MAC_TYPE_SOC=0x80,
	E_MAC_TYPE_NONE=0xFF

} MAC_teMacType;

typedef enum MAC_tsMacIftStatus_tag
{
	E_MIFT_SUCCESS=0,
	E_MIFT_TABLE_FULL,
	E_MIFT_BAD_MASK,
	E_MIFT_BAD_INDEX,
	E_MIFT_BAD_SERIALPORT,
	E_MIFT_UNSUPPORTED_RADIO,
	E_MIFT_BAD_MAC_COMBINATION,
	E_MIFT_CHANNELMASK_BAD_FOR_RADIO,
	E_MIFT_CHANNELMASK_NOT_FOUND,
	E_MIFT_CHANNELMASKS_FULL

} MAC_tsMacIftStatus;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/


/* Initialisation / Population */

PUBLIC void
ZPS_vMacIFTResetTable(void);

/* Population */

PUBLIC uint8
ZPS_u8MacIFTAddInterface(uint8 u8MacIndex, MAC_teRadioType eRadioType, MAC_teMacType eMacType, uint32 *pu32ChannelMaskLis);

PUBLIC uint8
ZPS_u8MacIFTAddUpdateChannelMask(uint8 u8MacIndex, uint32 u32ChannelMask);


/* Utility */

PUBLIC uint8
ZPS_u8MacIFTRadioTypePresent(MAC_teRadioType eRadioType);

PUBLIC uint8
ZPS_u8MacIFTTypePresent(MAC_teMacType eMacType);

PUBLIC uint8
ZPS_u8MacIFTGetNumInterfaces(void);

PUBLIC uint8
ZPS_u8MacIFTGetChannelMaskCount( uint8 u8MacId );

PUBLIC uint32*
ZPS_pu32MacIFTGetChannelMaskForPage(uint8 u8MacIndex, uint8 u8Page);

PUBLIC uint16
ZPS_u16MacIFTGetIDByChannelMask(uint32 u32ChannelMask);

PUBLIC teMacStatus
ZPS_u16MacIFTCheckForRadioType(MAC_teRadioType eRadioType);

PUBLIC uint16
ZPS_u16MacIFTGetIDByRadioType(MAC_teRadioType eRadioType);

PUBLIC uint8
ZPS_u8MacIFTGetActiveChannel(uint8 u8MacID);

PUBLIC void
ZPS_vMacIFTSetActiveChannel(uint8 u8MacID, uint8 u8Channel);

 /* ZPS_MULTIMASK_SUPPORT */
PUBLIC zps_tsAplAfMMServerContext*
ZPS_u8MacIFTGetMultiMaskServer(void);

PUBLIC uint8 ZPS_u8GetChannelOffset ( uint8 u8MacId );

PUBLIC uint8 ZPS_u8MacIFTNumOfEnabledInterfaces ( void );

PUBLIC uint8 ZPS_u8MacIFTGetIntfState ( uint8 u8MacIntfIndx );

PUBLIC void ZPS_vMacIFTSetIntfState ( uint8 u8MacIntfIndx, bool_t bState );

PUBLIC bool_t ZPS_u8MacIFTGetRouterAllowed ( uint8 u8MacIntfIndx );

PUBLIC void ZPS_vMacIFTSetRouterAllowed ( uint8 u8MacIntfIndx, bool_t bRouterAllowed );

PUBLIC bool_t ZPS_vMacIFTUpdateMacInterface (
		 	 	 	 	 	 	 	 uint8 u8MacIntfIndx,
		 	 	 	 	 	 	 	 uint8 u8ActiveChan,
		 	 	 	 	 	 	 	 bool_t bState,
		 	 	 	 	 	 	 	 bool_t bRouterAllowed);

PUBLIC uint32 ZPS_u32MacIFTGetChanMaskFromActChan(uint8 u8MacIntfIndx);

PUBLIC uint32 ZPS_vMacIFTGetChannelMaskFromMacId ( uint8 u8MacID );
/* Direct accessors */

PUBLIC MAC_tsMacInterfaceTable*
ZPS_psMacIFTGetTable(void);

PUBLIC MAC_tsMacInterface*
ZPS_psMacIFTGetInterface(uint8 u8MacID);


PUBLIC void ZPS_vMacIFTClearCounts ( uint8 u8MacIntfIndx );

PUBLIC uint32* ZPS_u32MacIFTGetApsCount ( uint8 u8MacIntfIndx );





#if defined __cplusplus
}
#endif


#endif  /* ZPS_MAC_IF_TABLE_H_INCLUDED */



/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

