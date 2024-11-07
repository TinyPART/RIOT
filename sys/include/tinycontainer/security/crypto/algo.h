/**
 *
 * Copyright (C) 2024, Orange
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
 * @brief   API for Crypto sub-module of TinyContainer - Algorithms
 *
 * @author  samuel.legouix <samuel.legouix@orange.com>
 *
 */

#ifndef TINYCONTAINER_SECURITY_CRYPTO_ALGO_H
#define TINYCONTAINER_SECURITY_CRYPTO_ALGO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CRYPTO_ALGO_UNKNOWN,
    CRYPTO_ALGO_NONE,
    /* hash */
    CRYPTO_ALGO_SHA_256,
    /* mac */
    CRYPTO_ALGO_HMAC_SHA256,
    /* sign */
    CRYPTO_ALGO_ED25519,
    /* encrypt */
    CRYPTO_ALGO_AES_128_CBC,
} crypto_algo_t;

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_SECURITY_CRYPTO_ALGO_H */
/** @} */
