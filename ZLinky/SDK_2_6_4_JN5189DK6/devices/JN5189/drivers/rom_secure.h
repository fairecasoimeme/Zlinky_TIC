/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ROM_SECURE_H_
#define ROM_SECURE_H_

#if defined __cplusplus
extern "C" {
#endif

/*!
 * @addtogroup ROM_API
 * @{
 */

/*! @file */

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include <rom_common.h>
#include <stdint.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/*! @brief  CERTIFICATE_MARKER magic value identifying certificate */
#define CERTIFICATE_MARKER (0xCE27CE27)

#define SIGNATURE_LEN (2048 / 8)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/*! @brief  IMAGE_CERT_T structure */
typedef struct
{
    uint32_t certificate_marker; /*!< Certificate marker: magic see @ CERTIFICATE_MARKER */
    uint32_t certificate_id;     /*!< Certificate id */
    uint32_t usage_flags;        /*!< Usage flags: mostly used in the unlocking procedure */
    uint32_t reserved;
    uint64_t customer_id;                   /*!< Customer Id: customer chosen identifier */
    uint64_t min_device_id;                 /*!< Min device id: min device version from which certificate applies */
    uint64_t max_device_id;                 /*!< Max device id: max device version up to which certificate applies */
    uint32_t public_key[SIGNATURE_LEN / 4]; /*!< RSA-2048 public key */
} IMAGE_CERT_T;

typedef struct _ImgTrailer
{
    IMAGE_CERT_T certificate;              /*!< The certificate see @ IMAGE_CERT_T */
    uint8_t cert_signature[SIGNATURE_LEN]; /*!< The signature of the certificate */
    uint8_t img_signature[SIGNATURE_LEN];  /*!< The image siganture*/
} ImageAuthTrailer_t;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/*! @brief This function performs an RSA 2048 signature verification
 *
 * Verify a signature by encrypting it using the provided public key and
 * validating the output matches the provided hash resulting from the SHA-256
 *
 * @param  hash: pointer on computed SHA-256 hash
 * @param  signature: pointer on RSA-2048 signature to be checked
 * @param  key: pointer on public key
 *
 * @return 1: if correct, 0: otherwise.
 */
static inline uint32_t secure_VerifySignature(uint8_t *hash, const uint8_t *signature, const uint32_t *key)
{
    uint32_t (*p_secure_VerifySignature)(uint8_t * hash, const uint8_t *signature, const uint32_t *key);
    p_secure_VerifySignature = (uint32_t(*)(uint8_t * hash, const uint8_t *signature, const uint32_t *key))0x03005c65U;

    return p_secure_VerifySignature(hash, signature, key);
}

/*! @brief This function performs an RSA 2048 signature verification over specified data block
 *
 * Verify a data block with appended signature.
 * Computes the SHA-256 hash.
 * calls see @secure_VerifySignature
 *
 * @param  start: pointer on start of data block
 * @param  length: length of data block
 * @param  key: pointer on public key
 * @param  signature: pointer on RSA-2048 signature to be checked
 *
 * @return 1: if correct, 0: otherwise.
 */
static inline uint32_t secure_VerifyBlock(uint8_t *start,
                                          uint32_t length,
                                          const uint32_t *key,
                                          const uint8_t *signature)
{
    uint32_t (*p_secure_VerifyBlock)(uint8_t * start, uint32_t length, const uint32_t *key, const uint8_t *signature);
    p_secure_VerifyBlock =
        (uint32_t(*)(uint8_t * start, uint32_t length, const uint32_t *key, const uint8_t *signature))0x03005cadU;

    return p_secure_VerifyBlock(start, length, key, signature);
}

/*! @brief This function performs an RSA 2048 signature verification
 *
 * Verify certificate is valid for this device and is authentic.
 * Certificate is checked for validity against customer and device ID stored in PFLASH.
 *
 * @param  certificate: pointer on computed SHA-256 hash
 * @param  key: pointer on public key
 * @param  cert_signature: pointer on RSA-2048 signature to be checked
 *
 * @return 1: if certificate is valid, 0: otherwise.
 */
static inline uint32_t secure_VerifyCertificate(const IMAGE_CERT_T *certificate,
                                                const uint32_t *key,
                                                const uint8_t *cert_signature)
{
    uint32_t (*p_secure_VerifyCertificate)(const IMAGE_CERT_T *certificate, const uint32_t *key,
                                           const uint8_t *cert_signature);
    p_secure_VerifyCertificate =
        (uint32_t(*)(const IMAGE_CERT_T *certificate, const uint32_t *key, const uint8_t *cert_signature))0x03005cd1U;

    return p_secure_VerifyCertificate(certificate, key, cert_signature);
}

/*! @brief This function verifies image authenticity.
 *
 * The function retrieves the certificate pointed by the boot block certificate offset field.
 * If present it has to be verified using the root certificate.
 *
 * @param  image_addr: pointer on start of image to be checked.
 *
 * @param  root_cert: pointer on root certificate (that contains a public key).
 *         If the root certificate is present the key is gotten from it.
 *
 * @return 1: if certificate is valid, 0: otherwise.
 */
static inline uint32_t secure_VerifyImage(uint32_t image_addr, const IMAGE_CERT_T *root_cert)
{
    uint32_t (*p_secure_VerifyImage)(uint32_t image_addr, const IMAGE_CERT_T *root_cert);
    p_secure_VerifyImage = (uint32_t(*)(uint32_t image_addr, const IMAGE_CERT_T *root_cert))0x03005d59U;

    return p_secure_VerifyImage(image_addr, root_cert);
}

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif /* ROM_SECURE_H_ */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
