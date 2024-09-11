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
 * @brief       TinyContainer Security sub-module implementation
 *
 * @author      BERKANE Ghilas (ghilas.berkane@gmail.com)
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#define DEBUG_ENABLE
#include "debug.h"

#include "tinycontainer/security/security.h"
#include "tinycontainer/security/crypto/crypto.h"
#include "tinycontainer/security/crypto/key.h"
#include "ed25519.h"

int tinycontainer_security_init(void)
{
    /* initialize the crypto module */
    crypto_init();

    /* add the public key to validate CWT signature */
    if (crypto_key_new(256, public_key) == NULL) {
        /* fail to add the public key */
    }
    /* private key is ignored */
    (void)private_key;

    return 0;
}
