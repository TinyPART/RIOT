/**
 *
 * Copyright (C) 2022-2023, Orange.
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
 * @brief   API of Tinycontainer Runtime sub-module for implementation sub-module
 *
 * @author  Samuel Legouix <samuel.legouix@orange.com>
 *
 */

#ifndef TINYCONTAINER_RUNTIME_RUNTIME_IMPL_H
#define TINYCONTAINER_RUNTIME_RUNTIME_IMPL_H

#include <stdbool.h>

#include "tinycontainer/memmgr/memmgr_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ brief opaque type for container runtime
 */
typedef void *runtime_handle_t;

/**
 * @brief   runtime function to called to create and initialize a container
 *
 * @param[in]   data    data part of the container
 * @param[in]   code    code part of the container
 *
 * @return runtime_handler_t (on success return a runtime handler, NULL otherwise)
 */
runtime_handle_t runtime_create(memmgr_block_t *data, memmgr_block_t *code);

/**
 * @brief   runtime function called whenever the container is started
 *
 * @param[in]   runtime handler of the container runtime
 */
void runtime_on_start(runtime_handle_t runtime);

/**
 * @brief   runtime function called on each loop
 *
 * The container loop is called until the end of container lifetime
 *
 * @param[in]   runtime    handler of the container runtime
 *
 * return int (0 to indicate end of container lifetime, non 0 otherwise)
 */
int runtime_on_loop(runtime_handle_t runtime);

/**
 * @brief   runtime function called when the container is stopped
 *
 * @param[in]   runtime   handler of the container runtime
 */
void runtime_on_stop(runtime_handle_t runtime);

/**
 * @brief   runtime function called at end of container lifetime
 *
 * @param[in]   runtime   handler of the runtime
 */
void runtime_on_finalize(runtime_handle_t runtime);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_RUNTIME_RUNTIME_IMPL_H */
/** @} */
