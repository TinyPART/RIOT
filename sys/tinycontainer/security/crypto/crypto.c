/*
 * Copyright (C) 2020-2024 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_tinycontainer
 * @{
 *
 * @file
 * @brief       TinyContainer Security Crypto sub-module implementation
 *
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#define DEBUG_ENABLE
#include "debug.h"

#include "tinycontainer/security/crypto/crypto.h"

int crypto_mac(const crypto_key_t * shared_key, crypto_algo_t algo,
               const uint8_t message, size_t message_size,
               uint8_t * signature, size_t signature_size) {
#if IS_USED(TINYCONTAINER_SECURITY_CRYPTO_PSA)
#endif /* TINYCONTAINER_SECURITY_CRYPTO_PSA */
    //TODO: not yet implemented
    (void)shared_key;
    (void)algo;
    (void)message;
    (void)message_size;
    (void)signature;
    (void)signature_size;
    return -1;
}

int crypto_sign(const crypto_key_t * private_key, crypto_algo_t algo,
                const uint8_t message, size_t message_size,
                uint8_t * mac, size_t mac_size) {
    //TODO: not yet implemented
    (void)private_key;
    (void)algo;
    (void)message;
    (void)message_size;
    (void)mac;
    (void)mac_size;
    return -1;
}

int crypto_encrypt(const crypto_key_t * shared_key, crypto_algo_t algo,
                   const uint8_t * iv, size_t iv_size,
                   const uint8_t cleartext, size_t cleartext_size,
                   uint8_t * ciphertext, size_t ciphertext_size) {
    //TODO: not yet implemented
    (void)shared_key;
    (void)algo;
    (void)iv;
    (void)iv_size;
    (void)cleartext;
    (void)cleartext_size;
    (void)ciphertext;
    (void)ciphertext_size;
    return -1;
}

int crypto_decrypt(const crypto_key_t * shared_key, crypto_algo_t algo,
                   const uint8_t * iv, size_t iv_size,
                   const uint8_t ciphertext, size_t ciphertext_size,
                   uint8_t * cleartext, size_t cleartext_size) {
    //TODO: not yet implemented
    (void)shared_key;
    (void)algo;
    (void)iv;
    (void)iv_size;
    (void)cleartext;
    (void)cleartext_size;
    (void)ciphertext;
    (void)ciphertext_size;
    return -1;
}
