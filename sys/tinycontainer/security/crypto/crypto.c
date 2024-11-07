/*
 * Copyright (C) 2024 Orange
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
#include "key.h"

#include <string.h>

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA)
#include "psa/crypto.h"
#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA */

void crypto_init(void) {
#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE)

    /* do nothing */

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE */

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA)

    if( psa_crypto_init() != PSA_SUCCESS) {
        //TODO: should be logged
    }

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA */
}

int crypto_mac(const crypto_key_t * shared_key, crypto_algo_t algo,
               const uint8_t *message, size_t message_size,
               uint8_t * mac , size_t mac_size) {

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE)

    /* do nothing */

    (void)shared_key;
    (void)algo;
    (void)message;
    (void)message_size;
    (void)mac;
    (void)mac_size;

    return 0;

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE */

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA)

    /* only HMAC_SHA_256 is currently supported */
    if (algo != CRYPTO_ALGO_HMAC_SHA256) {
        return -1;
    }

    static size_t HMAC_KEY_LEN = 32;
    size_t digest_size =
        PSA_MAC_LENGTH(PSA_KEY_TYPE_HMAC, HMAC_KEY_LEN,
                       PSA_ALG_HMAC(PSA_ALG_SHA_256));
    if (digest_size != mac_size) {
        return -1;
    }

    psa_key_id_t key_id = shared_key->key_id;

    size_t output_len = 0;

    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;
    status = psa_mac_compute(key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256),
                             message, message_size, mac, mac_size,
                             &output_len);
    if(status != PSA_SUCCESS || output_len != mac_size) {
        return -1;
    }

    return 0;

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA */

    return -1;
}

bool crypto_mac_verify(const crypto_key_t * shared_key, crypto_algo_t algo,
                       const uint8_t *message, size_t message_size,
                       const uint8_t * mac , size_t mac_size) {

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE)
    /* Let's be happy whatever the mac value is */
    (void)shared_key;
    (void)algo;
    (void)message;
    (void)message_size;
    (void)mac;
    (void)mac_size;
    return true;
#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE */

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA)

    /* only HMAC_SHA_256 is currently supported */
    if (algo != CRYPTO_ALGO_HMAC_SHA256) {
        return false;
    }

    static size_t HMAC_KEY_LEN = 32;
    size_t digest_size =
        PSA_MAC_LENGTH(PSA_KEY_TYPE_HMAC, HMAC_KEY_LEN,
                       PSA_ALG_HMAC(PSA_ALG_SHA_256));
    if (digest_size != mac_size) {
        return false;
    }

    psa_key_id_t key_id = shared_key->key_id;

    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;
    status = psa_mac_verify(key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256),
                            message, message_size, mac, mac_size);
    if(status != PSA_SUCCESS) {
        return false;
    }

    return true;

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA */

    return false;
}

int crypto_sign(const crypto_key_t * private_key, crypto_algo_t algo,
                const uint8_t * message, size_t message_size,
                uint8_t * signature, size_t signature_size) {

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE)

    /* do nothing */

    (void)private_key;
    (void)algo;
    (void)message;
    (void)message_size;
    (void)signature;
    (void)signature_size;

    return 0;

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE */

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA)

    /* only ED25519 is currently supported */
    if (algo != CRYPTO_ALGO_ED25519) {
        return -1;
    }

    psa_key_id_t key_id = private_key->key_id;

    size_t signature_len = 0;

    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;
    status = psa_sign_message(key_id, PSA_ALG_PURE_EDDSA,
                              message, message_size, signature, signature_size,
                              &signature_len);
    if(status != PSA_SUCCESS || signature_len != signature_size) {
        return -1;
    }

    return 0;

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA */

    return -1;
}

bool crypto_sign_verify(const crypto_key_t * public_key, crypto_algo_t algo,
                       const uint8_t * message, size_t message_size,
                       const uint8_t * signature, size_t signature_size) {

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE)

    /* Let's be happy whatever the signature value is */

    (void)public_key;
    (void)algo;
    (void)message;
    (void)message_size;
    (void)signature;
    (void)signature_size;

    return true;

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE */

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA)

    /* only ED25519 is currently supported */
    if (algo != CRYPTO_ALGO_ED25519) {
        return false;
    }

    psa_key_id_t key_id = public_key->key_id;

    size_t signature_len = PSA_SIGN_OUTPUT_SIZE(
        PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_TWISTED_EDWARDS),
        256,
        PSA_ALG_PURE_EDDSA);

    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;
    status = psa_verify_message(key_id, PSA_ALG_PURE_EDDSA,
                                message, message_size,
                                signature, signature_size);
    if(status != PSA_SUCCESS || signature_len != signature_size) {
        return false;
    }

    return true;

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA */

    return false;
}

int crypto_encrypt(const crypto_key_t * shared_key, crypto_algo_t algo,
                   const uint8_t * iv, size_t iv_size,
                   const uint8_t * cleartext, size_t cleartext_size,
                   uint8_t * ciphertext, size_t ciphertext_size) {

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE)

    /* just copy cleartext to ciphertext */

    if(cleartext_size != ciphertext_size) {
        return -1;
    }
    memcpy(ciphertext, cleartext, cleartext_size);

    (void) shared_key;
    (void) algo;
    (void) iv;
    (void) iv_size;

    return 0;

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE */

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA)

    /* only AES_128_CBC is currently supported */
    if (algo != CRYPTO_ALGO_AES_128_CBC) {
        return -1;
    }

    /* no iv for AES_128_CBC */
    if(iv != NULL || iv_size != 0) {
        return -1;
    }

    psa_key_id_t key_id = shared_key->key_id;

    size_t output_len = 0;

    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;
    status = psa_cipher_encrypt(key_id, PSA_ALG_CBC_NO_PADDING,
                                cleartext, cleartext_size,
                                ciphertext, ciphertext_size,
                                &output_len);
    if(status != PSA_SUCCESS || output_len != ciphertext_size) {
        return -1;
    }

    return 0;

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA */

    return -1;
}

int crypto_decrypt(const crypto_key_t * shared_key, crypto_algo_t algo,
                   const uint8_t * iv, size_t iv_size,
                   const uint8_t * ciphertext, size_t ciphertext_size,
                   uint8_t * cleartext, size_t cleartext_size) {

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE)

    /* just copy ciphertext to cleartext */
    if(ciphertext_size != cleartext_size) {
        return -1;
    }
    memcpy(cleartext, ciphertext, ciphertext_size);

    (void)shared_key;
    (void)algo;
    (void)iv;
    (void)iv_size;

    return 0;

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE */

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA)

    /* only AES_128_CBC is currently supported */
    if (algo != CRYPTO_ALGO_AES_128_CBC) {
        return -1;
    }

    /* no iv for AES_128_CBC */
    if(iv != NULL || iv_size != 0) {
        return -1;
    }

    psa_key_id_t key_id = shared_key->key_id;

    size_t output_len = 0;

    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;
    status = psa_cipher_decrypt(key_id, PSA_ALG_CBC_NO_PADDING,
                                ciphertext, ciphertext_size,
                                cleartext, cleartext_size,
                                &output_len);
    if(status != PSA_SUCCESS || output_len != cleartext_size) {
        return -1;
    }

    return 0;

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA */

    return -1;
}

int crypto_hash(crypto_algo_t algo,
                const uint8_t* message, size_t message_size,
                uint8_t *hash, size_t hash_size) {
#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE)

    (void)algo;
    (void)message;
    (void)message_size;
    (void)hash;
    (void)hash_size;

    return 0;

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE */

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA)

    psa_algorithm_t alg;
    switch (algo) {
        case CRYPTO_ALGO_SHA_256:
            alg = PSA_ALG_SHA_256;
            break;
        default:
            /* invalid */
            return -1;
    }

    if(hash_size != PSA_HASH_LENGTH(alg)) {
        /* invalid hash size */
        return -1;
    }

    size_t len;
    psa_status_t status = psa_hash_compute(alg,
                                           message, message_size,
                                           hash, hash_size,
                                           &len);
    if(status != PSA_SUCCESS) {
        return -1;
    }

    return 0;

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA */

    return -1;
}

bool crypto_hash_verify(crypto_algo_t algo,
                       const uint8_t* message, size_t message_size,
                       const uint8_t *hash, size_t hash_size) {
#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE)

    (void)algo;
    (void)message;
    (void)message_size;
    (void)hash;
    (void)hash_size;

    return true;

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE */

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA)

    psa_algorithm_t alg;
    switch (algo) {
        case CRYPTO_ALGO_SHA_256:
            alg = PSA_ALG_SHA_256;
            break;
        default:
            /* invalid */
            return -1;
    }

    psa_status_t status = psa_hash_compare(alg,
                                           message, message_size,
                                           hash, hash_size);
    if(status != PSA_SUCCESS) {
        return false;
    }

    return true;

#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA */

    return false;
}
