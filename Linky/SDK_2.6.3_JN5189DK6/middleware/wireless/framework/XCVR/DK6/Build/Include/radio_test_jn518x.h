/*
 * @brief JN518X Radio driver test functions
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __RADIO_TEST_JN518X_H_
#define __RADIO_TEST_JN518X_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <string.h>

/****************************************************************************/
/***        Macro/Type Definitions                                        ***/
/****************************************************************************/

//#define TRACE_RX_TX   // !!! to be disabled if debugger is to be used

#ifndef MICRO_MAC_H
typedef struct
{
    uint32_t u32L;  /**< Low word */
    uint32_t u32H;  /**< High word */
} tsExtAddr;

typedef union
{
    uint16_t    u16Short;
    tsExtAddr sExt;
} tuAddr;
#endif

/* Structure for building a MAC frame, where the MAC header alignment is
   handled by the hardware */
typedef struct RadioMacFrame
{
    uint8_t           u8PayloadLength;
    uint8_t           u8SequenceNum;
    uint16_t          u16FCF;
    uint16_t          u16DestPAN;
    uint16_t          u16SrcPAN;
    tuAddr          uDestAddr;
    tuAddr          uSrcAddr;
    uint16_t          u16FCS;
    uint16_t          u16Unused;
    union
    {
        uint8_t     au8Byte[127]; /* Payload as both bytes and words */
        uint32_t    au32Word[32];
    } uPayload;
} tsRadioJn518xMacFrame, *tsRadioJn518xMacFramePt;

/* Declare union for shared 32-bit Long Address Low Word and 16-bit Short Address */
typedef union
{
    uint32_t      u32Ad; /* 32-bit Long Address Low Word */
    uint16_t      u16Ad; /* 16-bit Short Address */
} tuAd;



/* Declare packet buffer data structure */
typedef struct MacBuf
{
    uint8_t       u8Len;          /* MAC Payload length  									-> u8PayloadLength*/
    uint8_t       u8Seq;          /* Sequence number 										-> u8SequenceNum*/
    uint16_t      u16Fctl;        /* Frame Control 										-> u16FCF*/
    uint16_t      u16Dpan;        /* Destination PAN 										-> u16DestPAN*/
    uint16_t      u16Span;        /* Source PAN 											-> u16SrcPAN*/
    tuAd    	uDad;           /* Destination Long Address Low Word or Short Address */
    uint32_t      u32Dadh;        /* Destination Long Address High Word */
    tuAd    	uSad;           /* Source Long Address Low Word or Short Address */
    uint32_t      u32Sadh;        /* Source Long Address High Word */
    uint16_t      u16Fcs;         /* Frame Check Sequence 								-> u16FCS*/
    uint16_t      u16Rsvd;        /* Reserved half-word 									-> u16Unused*/
    uint8_t       au8Pay[256];    /* MAC Payload (size supports extended length) */
} tsMacBuf, *tsMacBufPt;

typedef struct
{
    uint8_t u8Energy;
    uint8_t u8SQI;
    uint8_t u8LQI;
    int16_t s16RSSI;

} tsRadioInfos;

typedef struct {
    uint8_t u8Mode;
    uint8_t u8CCA;
    uint8_t u8Channel;
    uint8_t u8Retries;
    uint32_t u32Total;
    uint32_t u32Seen;
    uint32_t u32Errors;
    uint8_t u8Lqi;
} tsSSPT_MasterState;

typedef struct {
    uint8_t u8Mode;
    uint8_t u8Channel;
} tsSSPT_SlaveState;

/****************************************************************************/
extern uint64_t u64jn518x_MacAddress;

/****************************************************************************/
/*** radio driver types/macros/prototypes                                 ***/
/****************************************************************************/

#define RADIO_TEST_JN518X_VERSION (2042)

// For ATE usage uncomment following line
//#define ATE_USAGE

//#define TURNAROUNDTEST


// Maximum packet buffer length (use length for extended payloads)
#define MAX_BUFLEN          0x11c

#define MEM_BLOCK_START  0x04008000 // byte address

#define TX_BUF_START	MEM_BLOCK_START
#define TX_BUF_END		(TX_BUF_START + MAX_BUFLEN - 1) // Adresse byte
#define RX_BUF_START	(TX_BUF_END + 1)
#define RX_BUF_END		(RX_BUF_START + MAX_BUFLEN - 1)

#define BBC_INT_TX              (1 << 0)    /* Tx Complete flag */
#define BBC_INT_RXH             (1 << 1)    /* Rx Header flag */
#define BBC_INT_RXP             (1 << 2)    /* Rx Packet flag */

#define JPT_SNIFF_LOCKUP_TIMEOUT        500000
/* Packet parameters used for tests */
#define JPT_DST_PID                 0x1626
#define JPT_SRC_PID                 0x1626
#define JPT_DST_ADDR                0x3926
#define JPT_SRC_ADDR                0x3927


/* Value enumerations: Clear Channel Assesment Test */
#define CCA_MODE_ENERGY				1
#define CCA_MODE_CARRIER			2
#define CCA_MODE_CARRIER_OR_ENERGY	3
#define CCA_MODE_CARRIER_AND_ENERGY	4

/* Value enumerations: Tx Power Test */
#define TXPT_RUN_CW					0
#define TXPT_RUN_PRBS				1
#define TXPT_STOP					2

/* Value enumerations: Site Survey Per Test */
#define E_SSPT_MODE_LOCATE              1
#define E_SSPT_MODE_STOPPED             2
#define E_SSPT_MODE_RUNNING_ACKS        3
#define E_SSPT_MODE_RUNNING_NO_ACKS     4
#define E_SSPT_MODE_RESTART             5

/* Indexes for radio_jn518x_Carrier_Frequency_Select_by_index */
#define CFS_BLE_1MB 0		//2402,1,1
#define CFS_BLE_2MB 1		//2402,2,2
#define CFS_PACALINIT 2		//2432.0, 1.3125, 2
#define CFS_IQCALINIT1 3	//2432.3125, 1.3125, 2
#define CFS_IQCALINIT2 4	//2466, 1, 2
#define CFS_SYNTHCAL 5		//2449.3125, 1.3125, 2
#define CFS_IQCALATE 6		//2442, 1, 2


#define radio_jn518x_Trx_Packet_HW_Ctrl_Init radio_Trx_Packet_HW_Ctrl_Init

/****************************************************************************/
/*** radio driver API prototypes                                          ***/
/****************************************************************************/

/* in radio_jn518x.c */
/* General Init and misc functions */
void radio_jn518x_Init(void);
void radio_jn518x_TMU_Init(void);
void radio_jn518x_pre_start_to_pll_polled(void);
// unused but keep code in case of
#if 0
void radio_jn518x_tx_deassert_polled(void);
void radio_jn518x_tx_assert_polled(void);
#endif
void radio_jn518x_rx_deassert_polled(void);
void radio_jn518x_rx_assert_polled(void);
void radio_jn518x_power_down_to_off_polled(void);
void radio_Trx_Packet_HW_Ctrl_Init(void);
void radio_jn518x_Trx_Packet_SW_Ctrl_Init(void);
void radio_jn518x_Disable_Auto_Kmod_Chan_and_Synth_Cal_Operation(void);

/* Calibration functions */
void radio_jn518x_Rc_Cal(bool bDoCal); // Function used by both of the above functions
void radio_jn518x_Rc_Cal_init(void);
void radio_jn518x_Rc_Cal_algo(void);
void radio_jn518x_Synthesizer_Cal_Startup(bool bDoCal);
void radio_jn518x_Synthesizer_Cal_Startup_init(void);
void radio_jn518x_Synthesizer_Cal_Startup_algo(void);
void radio_jn518x_Synthesizer_Cal_Operation_And_Lock(bool bDoCal);
void radio_jn518x_Synthesizer_Cal_Operation_And_Lock_init(void);
void radio_jn518x_Synthesizer_Cal_Operation_And_Lock_algo(void);
void radio_jn518x_Synthesizer_Kmod_Band(bool bDoCal);
void radio_jn518x_Synthesizer_Kmod_Band_init(void);
uint32_t radio_jn518x_Synthesizer_Kmod_Band_algo(void);
void radio_jn518x_Synthesizer_Cw_Mode_Init(void);
void radio_jn518x_Synthesizer_Full_Cal(void);
void radio_jn518x_DCO_cal(void);
void radio_jn518x_DCO_cal_init(void);
int  radio_jn518x_rfbandgap_cal(uint32_t u32CalMode); //SMB 13/06
void radio_jn518x_PA_cal(void);
void radio_jn518x_PA_rewrite_lut(void);
void radio_jn518x_IQ_cal(void);
#ifdef ATE_USAGE
void  radio_jn518x_IQ_cal_init(void);	// for ATE tests
void radio_jn518x_IQ_cal_algo(void);	// for ATE tests
#endif
void radio_jn518x_Use_Cal_Values(void);
void radio_RSSI_cal(void);

/* channel change */
void radio_jn518x_Carrier_Frequency_Select_by_index(uint8_t index);

/* Stack dependent init */
void radio_jn518x_ZigBee_Regular_Init(void);
void radio_jn518x_ZigBee_Prop_1_Init(void);
void radio_jn518x_ZigBee_Prop_2_Init(void);
void radio_jn518x_TX_ZigBee_Regular_Init(void);
void radio_jn518x_RX_ZigBee_Regular_Init(void);
void radio_jn518x_TX_ZigBee_Prop_1_Regular_Init(void);
void radio_jn518x_RX_ZigBee_Prop_1_Regular_Init(void);
void radio_jn518x_TX_ZigBee_Prop_2_Regular_Init(void);
void radio_jn518x_RX_ZigBee_Prop_2_Regular_Init(void);

/* Utility functions */
void vSetRecalThres(int16_t s16LimiTemp);
void vGetCalTemp(int16_t *s16CalTemp);
void vSetKmodMedianIndex(uint8_t u8Index);
void vGetKmodMedianIndex(uint8_t *u8Index);
void vRadio_Reset_RadioConfig(void);
void vResetFlashCalData(void);

/* temporary */
void radio_jn518x_Use_Default_Cal_Values(void); // do not use
void radio_jn518x_Use_Cal_Values(void); // do not use  // SMB 09June
void radio_jn518x_Read_Cal_Values(void);
void radio_jn518x_ZB_Tx_Test_Mode(int enable, int chan, int test_mode, int test_seq, int test_polarity);


/* in radio_test_jn518x.c */
uint32_t u32GetDieID(void);

void vRadio_Test_Jn518x_PacketTx(uint8_t u8Channel, uint8_t u8NbTries, tsMacBuf *psPacket, bool Use_CCA, bool *pbPacketGood);
void vRadio_Test_Jn518x_PacketTxSend(uint8_t u8Channel, uint8_t u8NbTries, tsMacBuf *psPacket, bool Use_CCA);
bool bRadio_Test_Jn518x_PacketTxWaitTransmitted(void);
void bRadio_Test_Jn518x_PacketTxWaitPhyTransmitted(void);
bool bRadio_Test_Jn518x_PacketRx(uint8_t u8Channel, tsMacBuf *psPacket, tsRadioInfos *psInfos, bool *pbPacketGood);
bool bRadio_Test_Jn518x_PT_FrameTxComplete(void);
bool bRadio_Test_Jn518x_RadioSetChannel(uint8_t u8Channel);
uint8_t u8Radio_Test_Jn518x_RadioGetChannel(void);
uint32_t u32Radio_Test_Jn518x_Init(void);
void vRadio_Test_Jn518x_PostRadioInit(void);
void vRadio_Test_Jn518x_RadioInit(uint32_t u32RadioMode);
void vRadio_Test_Jn518x_RadioDeInit(void);
int8_t i8Radio_Test_Jn518x_RadioGetPower(void);
void vRadio_Test_Jn518x_RadioSetPower(int8_t i8PowerLevel);
int8_t s8Radio_Test_Jn518x_EnergyDetect(uint8_t u8Channel, uint32_t u32Samples);
bool bRadio_Test_Jn518x_CCA(uint8_t u8Channel, uint8_t u8Mode, int16_t i16Threshold);
void vRadio_Test_Jn518x_TPT_Start(void);
void vRadio_Test_Jn518x_TPT_Start_ISR(void);
uint32_t u32Radio_Test_Jn518x_TPT_WaitPacket(uint32_t u32Timeout, uint32_t *pu32Total, uint32_t *pu32Seen, uint32_t *pu32ChipErrors);
uint32_t u32Radio_Test_Jn518x_WaitPacket(uint32_t u32Timeout, uint32_t *pu32Total, uint32_t *pu32Seen, uint32_t *pu32ChipErrors);
void vRadio_Test_Jn518x_TPT_Stop(void);
void vRadio_Test_Jn518x_SetMaxInputLevel(uint8_t u8MaxInpLev);
void vRadio_Test_Jn518x_AntennaDiversityEnable(bool bEnabled);
void vRadio_Test_Jn518x_TxPowerAdjust(uint8_t u8PowerAdj, uint8_t u8Att3db, uint8_t u8Channel);
int8_t i8Radio_Test_Jn518x_RadioGetPowerFine(void);
void vRadio_Test_Jn518x_RadioSetPowerFine(int8_t i8PowerLevel);
void vRadio_Test_Jn518x_TxPowerTest(uint32_t u32Mode);
int16_t i16Radio_Test_Jn518x_GetRSSI(uint8_t u8Channel, uint32_t u32Samples, uint8_t *u8antenna);
uint8_t u8Radio_Test_Jn518x_GetLQIfromRSSI(int16_t s16RSSI);
void vPrintDebugInfos(uint8_t newlines);
void vRadio_Test_Jn518x_Print_RFP_registers(void);
void vRadio_Test_Jn518x_Print_transceiver_registers(void);

void vRadio_Test_Jn518x_AntennaDiversityPinEnable(bool bEnabled);
void vRadio_Test_Jn518x_StartRX_ACK_NOPROM(void);
void vRadio_Test_Jn518x_StartRX_ACK_PROM(void);

void vRadio_Test_Jn518x_DisableRXTX(bool bDisable);
void vRadio_Test_Jn518x_SetTXMBEBT(uint32_t u32Value);
void vRadio_Test_Jn518x_Enable_ITs_RFT1788(void);
void vRadio_Test_Jn518x_Disable_ITs_RFT1778(void);

void vRadio_Test_Jn518x_Register_ISR_MAC_Handler(void * vISRforMAC);

void vRadio_Test_Reset_RadioConfig(void);

void vRadio_Test_Print_ATE_DCO_Flash_Data(void);


#ifdef TRACE_RX_TX
#define ITRXSTART 0x0080
#define ITRXSTOP  0x0100
#define ITTXSTART 0x0200
#define ITTXSTOP  0x0400
void vRadio_Test_Jn518x_Enable_IT_TraceRxTx(void);
void vRadio_Test_Jn518x_Disable_IT_TraceRxTx(void);
void vRadio_Test_Jn518x_TraceRxTx_IT_Status_GetClear(uint32_t *u32Status);
#endif


/* in radio_jn518x_SiteSurveyPerTest.c */

void vRadio_Test_Jn518xSSPT_MasterInit(void);
bool bRadio_Test_Jn518xSSPT_MasterSetState(tsSSPT_MasterState *psState);
void vRadio_Test_Jn518xSSPT_MasterGetState(tsSSPT_MasterState *psState);
void vRadio_Test_Jn518xSSPT_SlaveInit(void);
void vRadio_Test_Jn518xSSPT_SlaveGetState(tsSSPT_SlaveState *psState);
void vRadio_Test_Jn518xSSPT_Stop(void);
void vJPT_XtalOscillatorTest(int stopstart);
void vJPT_Xtal32kOscillatorTest(int stopstart);

void SSPT_ZIGBEE_MAC_IRQHandler(void);

void vRadio_Test_jn518x_Read_Cal_Values(uint32_t *u32CalParams);
void vRadio_Test_jn518x_Print_Cal_Values(uint32_t *u32CalParams);
void vRadio_Test_Print_Banked_Registers();

#ifdef __cplusplus
}
#endif

#endif /* __RADIO_TEST_JN518X_H_ */
