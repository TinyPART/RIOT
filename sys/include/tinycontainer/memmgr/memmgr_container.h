/**
 *
 * Copyright (C) 2020-2023, Orange.
 *
 * Please, refer to the README.md and LICENSE files of TinyContainer
 *
 */

/**
 * @ingroup sys_tinycontainer
 *
 * @{
 *
 * @file
 * @brief   API of TinyContainer Memory Manager sub-module for Container sub-module
 *
 * @author  Samuel Legouix <samuel.legouix@orange.com>
 *
 */


#ifndef TINYCONTAINER_MEMMGR_CONTAINER_H
#define TINYCONTAINER_MEMMGR_CONTAINER_H

#include <stdint.h>

#include "tinycontainer/memmgr/memmgr_common.h"

/**
 * @brief Retrieve the memory blocks for data and code of the container
 *
 * @param[out] container_data     Memory block to the data part of the container
 *
 * @param[out] container_code     Memory block to the code part of the container
 *
 * @return negative value on error
 */
int memmgr_getcontainer(memmgr_block_t * container_data, memmgr_block_t * container_code);

#endif /* TINYCONTAINER_MEMMGR_CONTAINER_H */
/** @} */
