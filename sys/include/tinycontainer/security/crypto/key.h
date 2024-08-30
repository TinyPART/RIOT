/**
 *
 * Copyright (C) 2020-2024, Orange
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
 * @brief   API for Crypto sub-module of TinyContainer
 *
 * @author  samuel.legouix <samuel.legouix@orange.com>
 *
 */

#ifndef TINYCONTAINER_SECURITY_CRYPTO_KEY_H
#define TINYCONTAINER_SECURITY_CRYPTO_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Key type used for crypto operations
  */
typedef struct crypto_key crypto_key_t;

/** @brief create a new crypto key
  * @param  nbits           number of bits in the key
  * @param  from            if non nul, used to to import the key
  * @return crypto_key_t*   (a new key or null on error)
  */
const crypto_key_t * crypto_key_new(size_t nbits, uint8_t * from);

/** @brief release a crypto key
  */
void crypto_key_delete(const crypto_key_t * key);

/** @brief get the key slot id associated with a crypto key
  * @param  key   the crypto key
  * @return int   (slot id)
  */
int crypto_key_get_slot(const crypto_key_t * key);

/** @brief get the crypto key associated with a key slot
  * @param  slot_id
  * @return crypto_key_t*  (the crypto key store in the slot or null)
  */
const crypto_key_t * crypto_key_get(int slot_id);

/** @brief get the length of the crypto key
  * @param  key      the crypto key
  * @return size_t   (number of bits in the key)
  */
size_t crypto_key_get_nbit(const crypto_key_t *);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_SECURITY_CRYPTO_KEY_H */
/** @} */
