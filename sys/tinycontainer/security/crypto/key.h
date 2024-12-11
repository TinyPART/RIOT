/*
 * Copyright (C) 2024, Orange.
 *
 * Please, refer to the README.md and LICENSE files of TinyContainer
 *
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

#ifndef CRYPTO_KEY_H
#define CRYPTO_KEY_H

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

#endif /* CRYPTO_KEY_H */
