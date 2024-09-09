/**
 *
 * Copyright (C) 2024, Orange.
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
 * @brief   API of TinyContainer to definition CWT for CWT sub-module.
 *
 * @author  Samuel Legouix <samuel.leoguix@orange.com>
 *
 */

#ifndef TINYCONTAINER_SECURITY_CWT_CWT_H
#define TINYCONTAINER_SECURITY_CWT_CWT_H

#include "tinycontainer/security/crypto/key.h"
#include "tinycontainer/security/crypto/algo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CWT_TYPE_UNKNOWN,
    CWT_TYPE_COSE_SIGN,
    CWT_TYPE_COSE_SIGN1,
    CWT_TYPE_COSE_MAC,
    CWT_TYPE_COSE_MAC0,
    CWT_TYPE_COSE_ENCRYPT,
    CWT_TYPE_COSE_ENCRYPT0
} cwt_type_t;

typedef struct {
    const uint8_t * claim_set; /* payload of COSE_Mac0 or COSE_Sign1, or
                                  cyphertext of COSE_Encrypt1 */
    size_t claim_set_size;
    cwt_type_t type;
    const uint8_t * security;  /* tag of COSE_Mac0 or signature for COSE_Sign1 */
    size_t security_size;
} cwt_t;

bool cwt_parse(cwt_t * cwt, const uint8_t * buffer, size_t buffer_size);
void cwt_set_type(cwt_t * cwt, cwt_type_t type);
void cwt_set_claimet(cwt_t * cwt, const uint8_t * claim_set, size_t size);
bool cwt_is_sign(cwt_t * cwt);
bool cwt_is_sign1(cwt_t * cwt);
bool cwt_is_mac(cwt_t * cwt);
bool cwt_is_mac0(cwt_t * cwt);
bool cwt_is_encrypt(cwt_t * cwt);
bool cwt_is_encrypt0(cwt_t * cwt);
bool cwt_verify(cwt_t * cwt, const crypto_key_t * key, crypto_algo_t algo);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_SECURITY_CWT_CWT_VERIFIER_H*/
/** @} */
