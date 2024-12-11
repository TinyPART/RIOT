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
 * @brief       TinyContainer CWT Verifier sub-module implementation
 *
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#define DEBUG_ENABLE
#include "debug.h"

#include "tinycontainer/security/cwt/cwt.h"
#include "tinycontainer/security/crypto/crypto.h"
#include "cwt_cbor.h"
#include "nanocbor/nanocbor.h"

/* Public functions
 * ----------------
 */

static void cwt_reset(cwt_t *cwt)
{
    cwt->type = CWT_TYPE_UNKNOWN;
    cwt->claim_set = NULL;
    cwt->claim_set_size = 0;
    cwt->security = NULL;
    cwt->security_size = 0;
}

bool cwt_parse(cwt_t *cwt, const uint8_t *buffer, size_t buffer_len)
{

    cwt_reset(cwt);

    nanocbor_value_t it;
    nanocbor_decoder_init(&it, buffer, buffer_len);

    /* Removing the CWT CBOR tag */

    bool cose_untagged_allowed = true;
    if (nanocbor_get_type(&it) == NANOCBOR_TYPE_TAG) {
        uint32_t tag;
        if (nanocbor_get_tag(&it, &tag) != NANOCBOR_OK && tag == CBOR_TAG_CWT) {
            return false;
        }
        cose_untagged_allowed = false;
    }

    /* Determine the type of CWT from the COSE CBOR tag */

    cwt->type = CWT_TYPE_UNKNOWN;
    if (nanocbor_get_type(&it) == NANOCBOR_TYPE_TAG) {
        uint32_t tag;
        if (nanocbor_get_tag(&it, &tag) != NANOCBOR_OK) {
            return false;
        }
        switch (tag) {
        case CBOR_TAG_COSE_ENCRYPT0:
            cwt->type = CWT_TYPE_COSE_ENCRYPT0;
            break;
        case CBOR_TAG_COSE_MAC0:
            cwt->type = CWT_TYPE_COSE_MAC0;
            break;
        case CBOR_TAG_COSE_SIGN1:
            cwt->type = CWT_TYPE_COSE_SIGN1;
            break;
        case CBOR_TAG_COSE_ENCRYPT:
            cwt->type = CWT_TYPE_COSE_ENCRYPT;
            break;
        case CBOR_TAG_COSE_MAC:
            cwt->type = CWT_TYPE_COSE_MAC;
            break;
        case CBOR_TAG_COSE_SIGN:
            cwt->type = CWT_TYPE_COSE_SIGN;
            break;
        default:
            return false;
        }
    }
    else if (cose_untagged_allowed == false) {
        return false;
    }

    /* Verify the resulting COSE header includes only valid values*/

    /* first enter in the cose array */
    nanocbor_value_t array;
    if (nanocbor_enter_array(&it, &array) != NANOCBOR_OK) {
        return false;
    }

    /* the protected headers shall be empty */
    const uint8_t *tmp;
    size_t tmp_len;
    if (nanocbor_get_bstr(&array, &tmp, &tmp_len) != NANOCBOR_OK ||
        tmp_len != 0) {
        return false;
    }

    /* the unprotected header shall be an empty map */
    nanocbor_value_t unprotected;
    if (nanocbor_enter_map(&array, &unprotected) != NANOCBOR_OK ||
        !nanocbor_at_end(&unprotected)) {
        return false;
    }
    nanocbor_leave_container(&array, &unprotected);

    /* get the payload or ciphertext */
    if (nanocbor_get_bstr(&array,
                          &cwt->claim_set,
                          &cwt->claim_set_size) != NANOCBOR_OK &&
        nanocbor_get_null(&array) != NANOCBOR_OK) {
        return false;
    }

    /* get the signature or the tag */
    if (nanocbor_get_bstr(&array,
                          &cwt->security,
                          &cwt->security_size) != NANOCBOR_OK) {
        if (cwt->type == CWT_TYPE_COSE_MAC0 ||
            cwt->type == CWT_TYPE_COSE_SIGN1 ||
            (cwt->type != CWT_TYPE_UNKNOWN &&
             cwt->type != CWT_TYPE_COSE_ENCRYPT0)) {
            return false;
        }
    }

    /* we don't support any other data added to the COSE array */
    if (nanocbor_container_remaining(&array) != 0) {
        return false;
    }
    nanocbor_leave_container(&it, &array);

    return true;
}

bool cwt_verify(cwt_t *cwt, const crypto_key_t *key, crypto_algo_t algo)
{
    switch (cwt->type) {
    case CWT_TYPE_COSE_MAC0:
        return crypto_mac_verify(key, algo,
                                 cwt->claim_set, cwt->claim_set_size,
                                 cwt->security, cwt->security_size);
    case CWT_TYPE_UNKNOWN:
    /* SIGN1 is assumed */
    case CWT_TYPE_COSE_SIGN1:
        return crypto_sign_verify(key, algo,
                                  cwt->claim_set, cwt->claim_set_size,
                                  cwt->security, cwt->security_size);
    case CWT_TYPE_COSE_ENCRYPT0:
    case CWT_TYPE_COSE_MAC:
    case CWT_TYPE_COSE_SIGN:
    case CWT_TYPE_COSE_ENCRYPT:
    //TODO: not yet implemented
    default:
        //TODO: internal error: invalid state
        return false;
    }

    /*never reached*/
    return false;
}

void cwt_set_type(cwt_t *cwt, cwt_type_t type)
{
    if (cwt->type == CWT_TYPE_UNKNOWN) {
        cwt->type = type;
    }
}

bool cwt_is_sign(cwt_t *cwt)
{
    return cwt->type == CWT_TYPE_COSE_SIGN;
}

bool cwt_is_sign1(cwt_t *cwt)
{
    return cwt->type == CWT_TYPE_COSE_SIGN;
}

bool cwt_is_mac(cwt_t *cwt)
{
    return cwt->type == CWT_TYPE_COSE_SIGN;
}

bool cwt_is_mac0(cwt_t *cwt)
{
    return cwt->type == CWT_TYPE_COSE_SIGN;
}

bool cwt_is_encrypt(cwt_t *cwt)
{
    return cwt->type == CWT_TYPE_COSE_SIGN;
}

bool cwt_is_encrypt0(cwt_t *cwt)
{
    return cwt->type == CWT_TYPE_COSE_SIGN;
}

void cwt_set_claimset(cwt_t *cwt, const uint8_t *claim_set, size_t size)
{
    cwt->claim_set = claim_set;
    cwt->claim_set_size = size;
}
