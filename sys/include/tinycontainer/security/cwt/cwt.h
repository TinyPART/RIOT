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

/**
 * definition of CWT type
 */
typedef enum {
    CWT_TYPE_UNKNOWN,
    CWT_TYPE_COSE_SIGN,
    CWT_TYPE_COSE_SIGN1,
    CWT_TYPE_COSE_MAC,
    CWT_TYPE_COSE_MAC0,
    CWT_TYPE_COSE_ENCRYPT,
    CWT_TYPE_COSE_ENCRYPT0
} cwt_type_t;

/**
 * structure definition for cwt_t
 */
typedef struct {
    const uint8_t *claim_set;   /**< payload of COSE_Mac0 or COSE_Sign1, or
                                     cyphertext of COSE_Encrypt1 */
    size_t claim_set_size;      /**< memory size of the claim_set */
    cwt_type_t type;            /**< type of the cwt */
    const uint8_t *security;    /**< tag of a COSE_Mac0 object
                                     or signature of a COSE_Sign1 object */
    size_t security_size;       /**< memory size of the security */
} cwt_t;

/**
 * function to parse a CWT from a buffer
 *
 * @return boolean true if buffer is a CWT and false otherwise
 */
bool cwt_parse(cwt_t *cwt, const uint8_t *buffer, size_t buffer_size);

/**
 * function to parse a CWT from a buffer
 *
 * @return boolean true if buffer is a CWT and false otherwise
 */
void cwt_set_type(cwt_t *cwt, cwt_type_t type);

/**
 * function to set the claimset of a CWT
 *
 */
void cwt_set_claimet(cwt_t *cwt, const uint8_t *claim_set, size_t size);

/**
 * function to check if a CWT is a COSE Sign object
 *
 * @return boolean true if the CWT is COSE Sign object and false otherwise
 */
bool cwt_is_sign(cwt_t *cwt);

/**
 * function to check if a CWT is a COSE Sign1 object
 *
 * @return boolean true if the CWT is a COSE Sign1 object and false otherwise
 */
bool cwt_is_sign1(cwt_t *cwt);

/**
 * function to check if a CWT is a COSE Mac object
 *
 * @return boolean true if the CWT is a COSE Mac object and false otherwise
 */
bool cwt_is_mac(cwt_t *cwt);

/**
 * function to check if a CWT is a COSE Mac0 object
 *
 * @return boolean true the CWT is a COSE Mac0 object and false otherwise
 */
bool cwt_is_mac0(cwt_t *cwt);

/**
 * function to check if a CWT is a COSE Encrypt object
 *
 * @return boolean true if the CWT is a COSE Encrypt object and false otherwise
 */
bool cwt_is_encrypt(cwt_t *cwt);

/**
 * function to check if a CWT is a COSE Encrypt0 object
 *
 * @return boolean true if the CWT is a COSE Encrypt0 object and false otherwise
 */
bool cwt_is_encrypt0(cwt_t *cwt);

/**
 * function to validate a CWT
 *
 * @return boolean true is the CWT is valid and false otherwise
 */
bool cwt_verify(cwt_t *cwt, const crypto_key_t *key, crypto_algo_t algo);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_SECURITY_CWT_CWT_H*/
/** @} */
