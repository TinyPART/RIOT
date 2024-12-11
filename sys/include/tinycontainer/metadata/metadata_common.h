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

#ifndef TINYCONTAINER_METADATA_METADATA_COMMON_H
#define TINYCONTAINER_METADATA_METADATA_COMMON_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * return status when operation succeeds
 */
#define METADATA_OK 0
/**
 * return status when the cbor structure is invalid
 */
#define METADATA_INVALID_CBOR_STRUCTURE 1
/**
 * return status when metadata security is invalid
 */
#define METADATA_SECURITY_ERR 2
/**
 * return status when a searched item could not be found
 */
#define METADATA_NOT_FOUND 3

/* structures definitions
 * ======================
 */

/**
 * structure for the container object of metadata
 */
struct metadata_container {
    const uint8_t *raw_cbor;        /**< the metadata container object as a
                                         binary cbor bytes string             */
    size_t raw_cbor_len;            /**< the size of the raw cbor data        */
    const uint8_t *uid;             /**< a pointer to the container uid       */
    size_t uid_len;                 /**< the size of the container uid        */
    uint8_t type;                   /**< a pointer to the type of containe    */
    const uint8_t *syscall_mask;    /**< a pointer to the syscall_mask        */
    size_t syscall_mask_len;        /**< the size of the syscall_maxsk        */
};

/**
 * structure for the endpoint object of metadata
 */
struct metadata_endpoint {
    uint32_t id;                /**< the endpoint identifier                  */
    uint8_t peer_type;          /**< the type of endpoint                     */
    const uint8_t *peer_uid;    /**< a pointer to the peer uid                */
    uint32_t peer_endpoint_id;  /**< the size of the peer uid                 */
    uint8_t direction;          /**< the direction of the endpoint            */
};

/**
 * structure for the endpoints array of metdata
 */
struct metadata_endpoints {
    const uint8_t *raw_cbor;    /**< the metadata endpoints array as a binary
                                     cbor bytes string                        */
    size_t raw_cbor_len;        /**< the size of the raw cbor data            */
    uint8_t *next_endpoint;     /**< the current parse/search endpoint        */
};

/**
 * structure for the security object of metadata
 */
struct metadata_security {
    const uint8_t *raw_cbor;        /**< the metadata security array as a
                                          binary cbor bytes string            */
    size_t raw_cbor_len;            /**< the size of the raw cbor data        */
    const uint8_t *cwt[3];          /**< the data, code and metadata CWT      */
    size_t cwt_size[3];             /**< the size of the CWTs                 */
    uint32_t start_max_duration;    /**< how many time is allocated to the
                                         container on_start() function        */
    uint32_t loop_period;           /**< the frequency period the container
                                         on_loop() function is called         */
    uint32_t loop_max_duration;     /**< how many time is allocated to the
                                         container on_loop() function         */
    uint32_t loop_max_lifetime;     /**< the max number of time the container
                                         on_loop() function is called         */
    uint32_t stop_max_duration;     /**< how many time is allocated to the
                                         container on_stop() function         */
};

/**
 * structure for the metadata object
 */
struct metadata {
    const uint8_t *raw_cbor;    /**< the metadata object as a binary cbor
                                     bytes string                             */
    size_t raw_cbor_len;        /**< the size of the raw cbor data            */
    const uint8_t *container;   /**< a pointer to the bytes of the container
                                     object                                   */
    size_t container_len;       /**< the size of the container object bytes   */
    const uint8_t *endpoints;   /**< a pointer to the bytes of the container
                                     endpoints array                          */
    size_t endpoints_len;       /**< the size of the container endpoints
                                     array                                    */
    const uint8_t *security;    /**< a pointer to the bytes of the container
                                     security array                           */
    size_t security_len;        /**< the size of the container security array */
};

/* types definitions
 * =================
 */

/**
 * type definition for the metadata object
 */
typedef struct metadata metadata_t;

/**
 * type for the container object of metadata
 */
typedef struct metadata_container metadata_container_t;

/**
 * type for the endpoints object of metadata
 */
typedef struct metadata_endpoints metadata_endpoints_t;

/**
 * type for the security object of metadata
 */
typedef struct metadata_security metadata_security_t;

/**
 * type for the endpoint element of endpoints
 */
typedef struct metadata_endpoint metadata_endpoint_t;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_METADATA_METADATA_COMMON_H */
