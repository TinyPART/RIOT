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
 * @brief   API of TinyContainer Metadata submodule - Structure and type definitions
 *
 * @author Samuel Legouix <samuel.legouix@orange.com>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TINYCONTAINER_METADATA_METADATA_COMMON_H
#define TINYCONTAINER_METADATA_METADATA_COMMON_H

#include <stdint.h>
#include <stddef.h>

#define METADATA_OK 0
#define METADATA_INVALID_CBOR_STRUCTURE 1
#define METADATA_SECURITY_ERR 2
#define METADATA_NOT_FOUND 3

/* structures definitions
 * ======================
 */

/* structure for the container object of metadata */

struct metadata_container {
    const uint8_t *raw_cbor;
    size_t raw_cbor_len;
    const uint8_t *uid;
    size_t uid_len;
    uint8_t type;
    const uint8_t *syscall_mask;
    size_t syscall_mask_len;
};

/* structures for the endpoints object of metadata */

struct metadata_endpoint {
    uint32_t id;
    uint8_t peer_type;
    const uint8_t *peer_uid;
    uint32_t peer_endpoint_id;
    uint8_t direction;
    //TODO: token is not yet implemented
};

struct metadata_endpoints {
    const uint8_t *raw_cbor;
    size_t raw_cbor_len;
    uint8_t *next_endpoint;
};

/* structures for the security object of metadata */

struct metadata_security {
    const uint8_t *raw_cbor;
    size_t raw_cbor_len;
    const uint8_t *cwt[3];
    size_t cwt_size[3];
    uint32_t start_max_duration;
    uint32_t loop_period;
    uint32_t loop_max_duration;
    uint32_t loop_max_lifetime;
    uint32_t stop_max_duration;
};

/* structures for the metadata object */

struct metadata {
    const uint8_t *raw_cbor;
    size_t raw_cbor_len;
    const uint8_t *container;
    size_t container_len;
    const uint8_t *endpoints;
    size_t endpoints_len;
    const uint8_t *security;
    size_t security_len;
};

/* types definitions
 * =================
 */

/* type for the metadata object */
typedef struct metadata metadata_t;

/* type for the container object of metadata */
typedef struct metadata_container metadata_container_t;

/* type for the endpoints object of metadata */
typedef struct metadata_endpoints metadata_endpoints_t;

/* type for the security object of metadata */
typedef struct metadata_security metadata_security_t;

/* type for the endpoint element of endpoints */
typedef struct metadata_endpoint metadata_endpoint_t;
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_METADATA_METADATA_COMMON_H */
