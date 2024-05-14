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

#ifndef _METADATA_CBOR_H
#define _METADATA_CBOR_H

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

#endif /*_METADATA_CBOR_H*/
/** @} */
