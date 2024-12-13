/*
 * Copyright (C) 2024, Orange.
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
 * @brief       TinyContainer Security Crypto sub-module definitions for key
 *
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#ifndef KEY_H
#define KEY_H

#include "tinycontainer/security/crypto/key.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA)
#include "psa/crypto.h"
#endif

struct crypto_key {
    int slot_id;
#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE)
    uint32_t key_id;
    size_t nbits;
#elif IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA)
    psa_key_id_t key_id;
#endif

};

#ifdef __cplusplus
}
#endif

#endif /* KEY_H */
