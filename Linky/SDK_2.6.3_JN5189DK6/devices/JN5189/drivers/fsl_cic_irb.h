/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_CIC_IRB_H_
#define _FSL_CIC_IRB_H_

#include "fsl_common.h"

/*!
 * @addtogroup jn_cic_irb
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * IR protocol types
 */
typedef enum
{
    kCIC_IRB_ProtocolRC5,  /* Philips RC-5  Protocol	 	*/
    kCIC_IRB_ProtocolRC6,  /* Philips RC-6  Protocol 		*/
    kCIC_IRB_ProtocolSIRC, /* Sony SIRC  Protocol 			*/
    kCIC_IRB_ProtocolRCMM, /*  Philips RC-MM Protocol   	*/

/* Potentially supported protocols. Move the definition into the enum once supported. */
#if 0
    IRB_PROTOCOL_NEC,            	/*  NEC                      	*/
    IRB_PROTOCOL_PANASONIC_OLD,  	/*  Panasonic Old Protocol   	*/
    IRB_PROTOCOL_JVC,            	/*  JVC Protocol             	*/
    IRB_PROTOCOL_NECX,           	/*  NEC Extended Protocol    	*/
    IRB_PROTOCOL_HASH_CODE,     	/*  HASH CODE Protocol       	*/
#endif
} cic_irb_protocols_t;

/**
 * Status code responses from API calls
 */
typedef enum
{
    kCIC_IRB_Status_Ok,
    kCIC_IRB_Status_IllegalProtocol,
    kCIC_IRB_Status_IllegalValue,
    kCIC_IRB_Status_NotEnabled,
    kCIC_IRB_Status_AlreadyEnabled,
    kCIC_IRB_Status_Busy
} cic_irb_status_t;

/**
 * Status code responses from API calls
 */
typedef enum
{
    kCIC_IRB_CarrierFrequency_36kHz,
    kCIC_IRB_CarrierFrequency_38kHz,
    kCIC_IRB_CarrierFrequency_40kHz
} cic_irb_carrier_frequency_t;

/**
 * Sony SIRC version types
 */
typedef enum
{
    kCIC_IRB_SircVersion_12Bit,
    kCIC_IRB_SircVersion_15Bit,
    kCIC_IRB_SircVersion_20Bit
} cic_irb_sirc_version_t;

/**
 * RC-MM mode types
 */
typedef enum
{
    kCIC_IRB_RcmmMode_12Bit,
    kCIC_IRB_RcmmMode_24Bit,
    kCIC_IRB_RcmmMode_Oem
} cic_irb_rcmm_mode_t;

/**
 * RC-MM signal free types
 */
typedef enum
{
    kCIC_IRB_RcmmSignalFreeTime_1ms,
    kCIC_IRB_RcmmSignalFreeTime_3_36ms
} cic_irb_rcmm_signal_free_time_t;

/**
 * CIC IRB configuration structure
 *
 * This structure holds the configuration settings for the CIC IRB peripheral. To initialize this
 * structure to reasonable defaults, call the CIC_IRB_GetDefaultConfig() function and pass a
 * pointer to the configuration structure instance.
 */
typedef struct
{
    cic_irb_carrier_frequency_t selectedCarrierFrequency;
    cic_irb_protocols_t selectedProtocol;
} cic_irb_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*! @name CIC_IRB Get Default Configuration */
/*@{*/

/*!
 *
 * @brief CIC_IRB_GetDefaultConfig
 *
 * This function get default configuration for IRB
 *
 * @param config pointer to a configuration structure
 *
 * @return A cic_irb_status_t status code*
 *
 */
cic_irb_status_t CIC_IRB_GetDefaultConfig(cic_irb_config_t *config);

/*@}*/

/*! @name CIC_IRB Initialization */
/*@{*/
/*!
 *
 * @brief CIC_IRB_Init
 *
 * Initialize the IRB peripheral. Attaches, configures and enables
 * source and peripheral clocks, resets peripheral and initializes
 * instance data. The peripheral is not enabled after this.
 *
 * @param base CIC_IRB peripheral base address
 * @param config pointer to a configuration structure
 *
 * @return A cic_irb_status_t status code
 *
 *
 */
cic_irb_status_t CIC_IRB_Init(CIC_IRB_Type *base, cic_irb_config_t *config);

/*@}*/

/*! @name CIC_IRB Enable */
/*@{*/
/*!
 *
 * @brief CIC_IRB_Enable
 *
 * Enable the IRB peripheral doing protocol specific
 * initializations. The interrupts are enabled after this call.
 *
 * @param base CIC_IRB peripheral base address
 *
 * @return A cic_irb_status_t status code
 *
 *
 */
cic_irb_status_t CIC_IRB_Enable(CIC_IRB_Type *base);

/*@}*/

/*! @name CIC_IRB Send RC5 Packet */
/*@{*/
/*!
 *
 * @brief CIC_IRB_SendRC5Packet
 *
 * Send a RC-5 packet via the IRB peripheral. The peripheral
 * instance must have been initialised and enabled and not busy.
 *
 * @param base CIC_IRB peripheral base address
 * @param toggle the state of the toggle bit to encode
 * @param address the 5 bit address to go into the message
 * @param command the 7 bit command to go into the message
 *
 * @return A cic_irb_status_t status code
 *
 *
 */
cic_irb_status_t CIC_IRB_SendRC5Packet(CIC_IRB_Type *base, bool toggle, uint8_t address, uint8_t command);

/*@}*/

/*! @name CIC_IRB Send RC6 Packet */
/*@{*/
/*!
 *
 * @brief CIC_IRB_SendRC6Packet
 *
 * Send a RC-5 packet via the IRB peripheral. The peripheral
 * instance must have been initialised and enabled and not busy.
 *
 * @param base CIC_IRB peripheral base address
 * @param toggle the state of the toggle bit to encode
 * @param field the 3 bit field to go into the message
 * @param address the 8 bit address to go into the message
 * @param command the 8 bit command to go into the message
 *
 * @return A cic_irb_status_t status code
 *
 *
 */
cic_irb_status_t CIC_IRB_SendRC6Packet(
    CIC_IRB_Type *base, bool toggle, uint8_t field, uint8_t address, uint8_t command);

/*@}*/

/*! @name CIC_IRB Send SIRC Packet */
/*@{*/
/*!
 *
 * @brief CIC_IRB_SendSIRCPacket
 *
 * Send a SIRC packet via the IRB peripheral. The peripheral
 * instance must have been initialised and enabled and not busy.
 *
 * @param base CIC_IRB peripheral base address
 * @param version the version of the protocol to use, varies the packet length
 * command the 7 bit command to go into the message
 * @param address the 5 or 8 bit address to go into the message
 * @param extendedBits 8 bits of extra data in 20 bit message
 *
 * @return A cic_irb_status_t status code
 *
 *
 */
cic_irb_status_t CIC_IRB_SendSIRCPacket(
    CIC_IRB_Type *base, cic_irb_sirc_version_t version, uint8_t command, uint8_t address, uint8_t extendedBits);

/*@}*/

/*! @name CIC_IRB Send RCMM Packet */
/*@{*/
/*!
 *
 * @brief CIC_IRB_SendRCMMPacket
 *
 * Send a RCMM packet via the IRB peripheral. The peripheral
 * instance must have been initialised and enabled and not busy.
 *
 * @param base CIC_IRB peripheral base address
 * @parammode The RC-MM message mode. See RC-MM documentation
 * @parammode Bits The mode numerical value, varying number of bits
 * @param address RC-MM address data field in some modes
 * @param customerId RC-MM customer if in some modes
 * @param data the RC-MM data field, varying length depending on mode
 * @param signalFreeTime Silent period after packet transmission
 *
 * @return A cic_irb_status_t status code
 *
 *
 */
cic_irb_status_t CIC_IRB_SendRCMMPacket(CIC_IRB_Type *base,
                                        cic_irb_rcmm_mode_t mode,
                                        uint8_t modeBits,
                                        uint8_t address,
                                        uint8_t customerId,
                                        uint32_t data,
                                        cic_irb_rcmm_signal_free_time_t signalFreeTime);

/*@}*/

/*! @name CIC_IRB Is Busy */
/*@{*/
/*!
 *
 * @brief CIC_IRB_IsBusy
 *
 * Determine if the IRB peripheral instance is in the process
 * of sending the previous message.
 *
 * @param base CIC_IRB peripheral base address
 * @param isBusy pointer to bool for result
 *
 * @return A cic_irb_status_t status code
 *
 *
 */
cic_irb_status_t CIC_IRB_IsBusy(CIC_IRB_Type *base, bool *isBusy);

/*@}*/

/*! @name CIC_IRB Disable */
/*@{*/
/*!
 *
 * @brief CIC_IRB_Disable
 *
 * Disable the IRB peripheral such that it can be enabled again
 * without doing another init. This stops interrupts and
 * any part sent message is abandoned.
 *
 * @param base CIC_IRB peripheral base address
 *
 * @return A cic_irb_status_t status code
 *
 *
 */
cic_irb_status_t CIC_IRB_Disable(CIC_IRB_Type *base);
/*@}*/

/*! @name CIC_IRB Deinitializations */
/*@{*/
/*!
 *
 * @brief CIC_IRB_DeInit
 *
 * De-initialises the IRB peripheral instance. The clock is
 * stopped.
 *
 * @param base CIC_IRB peripheral base address
 *
 * @return A cic_irb_status_t status code
 *
 *
 */
cic_irb_status_t CIC_IRB_DeInit(CIC_IRB_Type *base);

/*@}*/

#ifdef __cplusplus
}
#endif

/*!
 * @}
 */

#endif /* _FSL_CIC_IRB_H_ */
