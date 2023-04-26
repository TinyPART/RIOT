/*
 * Copyright (C) 2013 Freie Universität Berlin
 * Copyright (C) 2013 Inria
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
 * @brief       TinyContainer rBPF runtime functions
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#ifndef TINYCONTAINER_CONTAINER_RBPF_H
#define TINYCONTAINER_CONTAINER_RBPF_H

#include <stdbool.h>
#include <stdint.h>
#include "rbpf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_TINYCONTAINER_RBPF_MAX_HANDLES
#define CONFIG_TINYCONTAINER_RBPF_MAX_HANDLES 3
#endif

#define TINYCONTAINER_RBPF_STACK_SIZE RBPF_STACK_SIZE

struct RbpfHandle {
    bool is_used;
    rbpf_application_t app;
    uint8_t stack[TINYCONTAINER_RBPF_STACK_SIZE];
};

static bool tinycontainer_rbpf_is_used(struct RbpfHandle *handle)
{
    return handle->is_used;
}

/**
 * @brief Attempt to find a free container handle, assign it and return it.
 *
 * @details If this is the first handle to be used, it will also try to initialise the container.
 *
 *
 * @return A pointer to the handle, or NULL if no free handle was found.
 */
static container_handle_t handle_init(void);

/**
 * @brief Free a handle, which includes freeing the WASM module, instance and code.
 *
 * @details This will also free the memory used by the container if this is the last handle.
 *
 * @param[in] handle The handle to free.
 * @return true on success, false on error.
 */
static bool handle_destroy(container_handle_t handle);

static bool runtime_init(void);
static void runtime_destroy(void);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_CONTAINER_RBPF_H */

