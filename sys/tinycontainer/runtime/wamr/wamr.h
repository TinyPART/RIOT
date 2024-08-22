/*
 * Copyright (C) 2022-2024, Orange.
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
 * @brief       TinyContainer WAMR runtime
 *
 * @author      Gregory Holder <gregory.holder76@gmail.com>
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#ifndef WAMR_H
#define WAMR_H

#include "wasm_export.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct InterpHandle {
    bool is_used;
    wasm_module_t module;
    wasm_module_inst_t module_instance;
    wasm_exec_env_t exec_env;
    wasm_function_inst_t start_func;
    wasm_function_inst_t loop_func;
    wasm_function_inst_t stop_func;
};

/**
 * @brief Attempt to find a free runtime handle, assign it and return it.
 *
 * @details If this is the first handle to be used, it will also try to initialise the runtime.
 *
 *
 * @return A pointer to the handle, or NULL if no free handle was found.
 */
static runtime_handle_t handle_init(void);

/**
 * @brief Free a handle, which includes freeing the WASM module, instance and code.
 *
 * @details This will also free the memory used by the runtime if this is the last handle.
 *
 * @param[in] handle The handle to free.
 * @return true on success, false on error.
 */
static bool handle_destroy(runtime_handle_t handle);

#ifndef TINYCONTAINER_NUMBER_OF_CONTAINERS
#define MAX_HANDLES 3
#else
#define MAX_HANDLES TINYCONTAINER_NUMBER_OF_CONTAINERS
#endif

#define STACK_SIZE 1024
#define HEAP_SIZE 1024

static bool runtime_init(void);
static void runtime_destroy(void);

#ifdef __cplusplus
}
#endif

#endif /* WAMR_H */
