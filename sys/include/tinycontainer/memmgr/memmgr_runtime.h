/**
 *
 * Copyright (C) 2020-2023, Orange.
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
 * @brief   API of TinyContainer Memory Manager sub-module for Runtime
            sub-module
 *
 * @author  Samuel Legouix <samuel.legouix@orange.com>
 *
 */

#ifndef TINYCONTAINER_MEMMGR_MEMMGR_RUNTIME_H
#define TINYCONTAINER_MEMMGR_MEMMGR_RUNTIME_H

#include <stdint.h>

#include "tinycontainer/memmgr/memmgr_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Retrieve the memory blocks for data and code of the container
 *
 * @param[out] container_data     Memory block to the data part of the container
 *
 * @param[out] container_code     Memory block to the code part of the container
 *
 * @return negative value on error
 */
int memmgr_getcontainer(memmgr_block_t *container_data, memmgr_block_t *container_code);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_MEMMGR_MEMMGR_RUNTIME_H */
/** @} */
