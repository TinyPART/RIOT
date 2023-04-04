/*
 * Copyright (C) 2022-2023, Orange.
 *
 * Please, refer to the README.md and LICENSE files of TinyContainer
 *
 */

/**
 * @ingroup     sys_tinycontainer
 * @{
 *
 * @file
 * @brief       TinyContainer WAMR runtime
 *
 * @author      Gregory Holder <gregory.holder@orange.com>
 *
 * @}
 */


#ifndef TINYCONTAINER_CONTAINER_WAMR_H
#define TINYCONTAINER_CONTAINER_WAMR_H

#include "wasm_export.h"
#include <stdbool.h>
#include <stdint.h>

struct InterpHandle {
    bool is_used;
    bool is_finished;
    uint8_t *wasm_buf;
    wasm_module_t module;
    wasm_module_inst_t module_instance;
    wasm_exec_env_t exec_env;
    wasm_function_inst_t start_func;
    wasm_function_inst_t loop_func;
    wasm_function_inst_t stop_func;
};


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
 * @param[in] interp_handle The handle to free.
 * @return true on success, false on error.
 */
static bool handle_destroy(container_handle_t handle);


#define MAX_HANDLES 3

#define STACK_SIZE 1024
#define HEAP_SIZE 1024

static bool runtime_init(void);
static void runtime_destroy(void);


#endif /* TINYCONTAINER_CONTAINER_WAMR_H */
