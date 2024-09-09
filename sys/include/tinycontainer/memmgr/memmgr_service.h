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
 * @brief   API of TinyContainer Memory Manager sub-module for Service sub-module
 *
 * @author  Samuel Legouix <samuel.legouix@orange.com>
 *
 */

#ifndef TINYCONTAINER_MEMMGR_MEMMGR_SERVICE_H
#define TINYCONTAINER_MEMMGR_MEMMGR_SERVICE_H

#include <stdint.h>

#include "tinycontainer/metadata/metadata_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief retrieve a metadata endpoint for a container_id
 *
 * @param[in] slot_id     Container slot identifier
 *
 * @param[in] endpoint_id Endpoint Identifier as set in container metadata
 * @param[out] endpoint   the returned Metadata Endpoint
 *
 * @return negative value on error
 */
int memmgr_getendpoint(int slot_id, int endpoint_id,
                       metadata_endpoint_t *endpoint);

/**
 * @brief retrieve the bitmask for syscall of a container
 *
 * @param[in] slot_id     Container slot identifier
 *
 * @return the syscall mask (on error all bit are set to 0)
 */
uint32_t memmgr_getsyscallmask(int slot_id);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_MEMMGR_MEMMGR_SERVICE_H */
/** @} */
