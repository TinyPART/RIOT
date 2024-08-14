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
 * @brief       TinyContainer Metadata sub-module - CBOR definitions
 *
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 */

#ifndef METADATA_CBOR_H
#define METADATA_CBOR_H

#ifdef __cplusplus
extern "C" {
#endif

#define METADATA_TAG 6082514239057121876

/* keys for metadata  */

#define METADATA_CONTAINER 1
#define METADATA_ENDPOINTS 2
#define METADATA_SECURITY  3

/* keys for metadata container  */

#define METADATA_CONTAINER_UID          1
#define METADATA_CONTAINER_TYPE         2
#define METADATA_CONTAINER_SYSCALL_MASK 3

#define METADATA_CONTAINER_TYPE_UNDEFINED  0
#define METADATA_CONTAINER_TYPE_NATIVE     1
#define METADATA_CONTAINER_TYPE_SHELL      2
#define METADATA_CONTAINER_TYPE_BPF        3
#define METADATA_CONTAINER_TYPE_WASM       4
#define METADATA_CONTAINER_TYPE_JAVACARD   5
#define METADATA_CONTAINER_TYPE_JAVASCRIPT 6
#define METADATA_CONTAINER_TYPE_PYTHON     7

/* keys for metadata endpoint  */

#define METADATA_ENDPOINT_ID                1
#define METADATA_ENDPOINT_TYPE              2
#define METADATA_ENDPOINT_PEER_UID          3
#define METADATA_ENDPOINT_PEER_ENDPOINT_ID  4
#define METADATA_ENDPOINT_DIRECTION         5
#define METADATA_ENDPOINT_TOKEN             6

#define METADATA_PEER_CONTAINER 0
#define METADATA_PEER_LOCAL     1
#define METADATA_PEER_REMOTE    2

#define METADATA_DIRECTION_IN   0
#define METADATA_DIRECTION_OUT  1
#define METADATA_DIRECTION_BOTH 2

#ifdef __cplusplus
}
#endif

#endif /* METADATA_CBOR_H */
/** @} */
