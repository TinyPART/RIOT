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

#ifndef TINYCONTAINER_CONTAINER_CONTAINER_RUNTIME_H
#define TINYCONTAINER_CONTAINER_CONTAINER_RUNTIME_H

#include <stdbool.h>

#include "tinycontainer/memmgr/memmgr_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ brief opaque type for container
 */
typedef void *container_handle_t;

/**
 * @brief   runtime function to called to create and initialize a container
 *
 * @param[in]   data    data part of the container
 * @param[in]   code    code part of the container
 *
 * @return container_handler_t (on success return a container handler, NULL otherwise)
 */
container_handle_t container_create(memmgr_block_t * data, memmgr_block_t * code);

/**
 * @brief   runtime function called whenever the container is started
 *
 * @param[in]   container    handler of the container
 */
void container_on_start(container_handle_t container);

/**
 * @brief   runtime function called on each loop
 *
 * The container loop is called until the end of container lifetime
 *
 * @param[in]   container    handler of the container
 *
 * return int (0 to indicate end of container lifetime, non 0 otherwise)
 */
int container_on_loop(container_handle_t container);

/**
 * @brief   runtime function called when the container is stopped
 *
 * @param[in]   container   handler of the container
 */
void container_on_stop(container_handle_t container);

/**
 * @brief   runtime function called at end of container lifetime
 *
 * @param[in]   container   handler of the container
 */
void container_on_finalize(container_handle_t container);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_CONTAINER_CONTAINER_RUNTIME_H */
/** @} */
