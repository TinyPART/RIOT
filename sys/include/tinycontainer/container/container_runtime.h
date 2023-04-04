/**
 *
 * Copyright (C) 2022-2023, Orange.
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
 * @brief   API of Tinycontainer Container sub-module for runtimes sub-module
 *
 * @author  Samuel Legouix <samuel.legouix@orange.com>
 *
 */


#ifndef TINYCONTAINER_CONTAINER_RUNTIME_H
#define TINYCONTAINER_CONTAINER_RUNTIME_H

#include <stdbool.h>

#include "tinycontainer/memmgr/memmgr_runtime.h"

typedef void *container_handle_t;

container_handle_t container_create(memmgr_block_t * data, memmgr_block_t * code);

void container_on_start(container_handle_t);
int container_on_loop(container_handle_t);
void container_on_stop(container_handle_t);

void container_on_finalize(container_handle_t);

#endif /* TINYCONTAINER_CONTAINER_RUNTIME_H */
/** @} */
