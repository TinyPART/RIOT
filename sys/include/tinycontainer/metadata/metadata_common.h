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

#ifndef METADATA_COMMON_H
#define METADATA_COMMON_H

#include <stdint.h>
#include <stddef.h>

#define METADATA_OK 0
#define METADATA_INVALID_CBOR_STRUCTURE 1
#define METADATA_SECURITY_ERR 2

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
    const uint8_t *syscall;
    size_t syscall_len;
};

/* peer object of container  */
struct metadata_container_peer {
    uint8_t *uid;
    int uid_size;
    uint8_t endpoint_id;
};
struct metadata_local_peer {
    uint8_t local_type; /* spi, i2c, gpio */
    /*TBC*/
};
struct metadata_remote_peer {
    uint8_t remote_type; /* dtsl, mqtt, lwm2m */
    /*TBC*/
};

struct metadata_endpoint {
    uint32_t id;
    uint8_t peer_type;
    union metadata_peer {
        struct metadata_container_peer container;
        struct metadata_local_peer local;
        struct metadata_remote_peer remote;
    } peer;
    /*TBC: token*/
};

struct metadata_endpoints {
    uint8_t *raw_cbor;
    size_t raw_cbor_len;
    uint8_t *next_endpoint;
};

struct metadata_security {
    uint8_t *raw_cbor;
    size_t raw_cbor_len;
    //TBC
};

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
typedef struct metadata_enpoints metadata_endpoints_t;

/* type for the security object of metadata */
typedef struct metadata_security metadata_security_t;

/* type for the endpoint element of endpoints */
typedef struct metadata_endpoint metadata_endpoint_t;

#endif /*METADATA_COMMON_H*/
/** @} */
