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

#include "tinycontainer/security/crypto/key.h"
#include "key.h"

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA)
#include "psa/crypto.h"
#endif /* MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA */

#define NB_OF_KEY_SLOTS 3 //TODO: shall be globally configurable
static crypto_key_t key_slots[NB_OF_KEY_SLOTS];
static bool key_slots_initialized = false;

crypto_key_t * new_key(void) {
    if(!key_slots_initialized) {
        for (int i=0; i< NB_OF_KEY_SLOTS; i++) {
            key_slots[i].slot_id = -1;
        }
        key_slots_initialized = true;
    }
    for (int i=0; i< NB_OF_KEY_SLOTS; i++)
    {
        if(key_slots[i].slot_id == -1) {
            key_slots[i].slot_id = i;
            return &key_slots[i];
        }
    }
    return NULL;
}

void free_key(crypto_key_t * key){
    if(!key_slots_initialized) {
        return;
    }
    for (int i=0; i< NB_OF_KEY_SLOTS; i++)
    {
        if(key == &key_slots[i]) {
            key_slots[i].slot_id = -1;
            return;
        }
    }
}

/* Public functions
 * ----------------
 */

const crypto_key_t * crypto_key_new(size_t nbits, uint8_t * from) {

    crypto_key_t * key = new_key();
    if(key == NULL) {
        return NULL;
    }

#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE)
    (void)from;
    key->nbits = nbits;

#elif IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA)
    //FIXME: current implementation only support ED25519 to verify signature
    psa_key_attributes_t attr = psa_key_attributes_init();
    psa_set_key_algorithm(&attr, PSA_ALG_PURE_EDDSA);
    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_bits(&attr, nbits);
    psa_set_key_type(&attr, PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_TWISTED_EDWARDS)));
    if (psa_import_key(&attr, from, nbits/8, &key->key_id) != PSA_SUCCESS) {
        free(key);
        key = NULL;
    }
#endif

    return key;
}

void crypto_key_delete(crypto_key_t * key) {
    free_key(key);
}

int crypto_key_get_slot(const crypto_key_t * key) {
    return key -> slot_id;
}

const crypto_key_t * crypto_key_get(int slot_id) {
    if (slot_id >= 0 && slot_id < NB_OF_KEY_SLOTS &&
        key_slots[slot_id].slot_id == slot_id) {
        return &key_slots[slot_id];
    }
    return NULL;
}

size_t crypto_key_get_nbit(const crypto_key_t * key) {
#if IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_NONE)
    return key -> nbits;
#elif IS_USED(MODULE_TINYCONTAINER_SECURITY_CRYPTO_PSA)
    psa_key_attributes_t attr;
    if(psa_get_key_attributes(key->key_id, &attr) != PSA_SUCCESS) {
        return 0;
    }
    return psa_get_key_bits(&attr);
#endif
}
