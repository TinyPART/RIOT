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
 * @brief   API of TinyContainer Metadata sub-module for MemMgr sub-module
 *
 * @author Samuel Legouix <samuel.legouix@orange.com>
 *
 */

#ifndef TINYCONTAINER_METADATA_METADATA_MEMMGR_H
#define TINYCONTAINER_METADATA_METADATA_MEMMGR_H

#include <stdint.h>
#include <stddef.h>

#include "metadata_common.h"

/* Parse a cbor buffer as metadata object.
 *
 * return METADATA_OK on success
 */
int metadata_parse(metadata_t *metadata, const uint8_t *buf, size_t len);

/* Parse a cbor buffer as a container object.
 *
 * return METADATA_OK on success
 */
int metadata_container_parse(metadata_container_t *metadata_container,
                             const uint8_t *buf, size_t len);

/* Parse a cbor buffer as an endpoints object.
 *
 * return METADATA_OK on success
 */
//int metadata_endpoints_parse(metadata_endpoints_t *metadata,
//                             const uint8_t *buf, size_t len);

/* Parse a cbor buffer as a security object.
 *
 * return METADATA_OK on success
 */
//int metadata_security_parse(metadata_security_t *metadata,
//                            const uint8_t *buf, size_t len);

#endif /* TINYCONTAINER_METADATA_METADATA_MEMMGR_H */
/** @} */
