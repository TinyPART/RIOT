/**
 *
 * Copyright (C) 2020-2024, Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_tinycontainer
 *
 * @{
 *
 * @file
 * @brief   API for Crypto sub-module of TinyContainer
 *
 * @author  samuel.legouix <samuel.legouix@orange.com>
 *
 */

#ifndef TINYCONTAINER_SECURITY_CRYPTO_CRYPTO_H
#define TINYCONTAINER_SECURITY_CRYPTO_CRYPTO_H

#include "tinycontainer/security/crypto/key.h"
#include "tinycontainer/security/crypto/algo.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief function to generate a mac from a message
  */
int crypto_mac(const crypto_key_t * shared_key, crypto_algo_t algo,
               const uint8_t message, size_t message_size,
               uint8_t * signature, size_t signature_size);

/** @brief function to generate a digital signature from a message
  */
int crypto_sign(const crypto_key_t * private_key, crypto_algo_t algo,
                const uint8_t message, size_t message_size,
                uint8_t * mac, size_t mac_size);

/** @brief function to encrypt a message
  */
int crypto_encrypt(const crypto_key_t * shared_key, crypto_algo_t algo,
                   const uint8_t * iv, size_t iv_size,
                   const uint8_t cleartext, size_t cleartext_size,
                   uint8_t * ciphertext, size_t ciphertext_size);

/** @brief function to decrypt an encrypted message
  */
int crypto_decrypt(const crypto_key_t * shared_key, crypto_algo_t algo,
                   const uint8_t * iv, size_t iv_size,
                   const uint8_t ciphertext, size_t ciphertext_size,
                   uint8_t * cleartext, size_t cleartext_size);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_SECURITY_CRYPTO_CRYPTO_H */
/** @} */
