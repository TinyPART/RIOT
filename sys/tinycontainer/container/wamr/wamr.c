/*
 * Copyright (C) 2022-2023 Orange
 *
 * Please, refer to the README.md and LICENSE files of TinyContainer
 *
 */

/**
 * @ingroup     sys_tinycontainer
 * @{
 *
 * @file
 * @brief       TinyContainer WAMR runtime implementation
 *
 * @author      Samuel legouix <samuel.legouix@orange.com>
 * @author      Gregory Holder <gregory.holder@orange.com>
 *
 * @}
 */

#include <string.h>
#include <stdlib.h>

#define ENABLE_DEBUG 0
#include "tinycontainer/debugging.h"

#include "ztimer.h"

#include "tinycontainer/container/container_runtime.h"
#include "wamr.h"
#include "wamr_natives.h"

#define GET_EXCEPTION(handle) (wasm_runtime_get_exception(handle->module_instance))

static struct InterpHandle handles[MAX_HANDLES];
static uint8_t handles_in_use = 0;

static bool is_initialised = false;
static bool runtime_init(void)
{
    /* if already initialised, return early */
    if (is_initialised) {
        return true;
    }

    static RuntimeInitArgs init_args = {
        .mem_alloc_type = Alloc_With_System_Allocator
    };

    if (!wasm_runtime_full_init(&init_args)) {
        goto runtime_init_fail;
    }
    if (!register_natives()) {
        goto runtime_init_fail;
    }

    is_initialised = true;
    return true;

runtime_init_fail:
    /* something went wrong, clean up */
    runtime_destroy();
    return false;
}

static void runtime_destroy(void)
{
    /* free all handles, but log those that are still in use */
    for (int i = 0; i < MAX_HANDLES; i++) {
        /* get the handle */
        struct InterpHandle *handle = &handles[i];
        /* if the handle is in use */
        if (handle->is_used) {
            DEBUG("container_wamr: handle %d still in use\n", i);
            /* free the handle */
            handle_destroy(handle);
        }
    }
    /* destroy the runtime */
    wasm_runtime_destroy();

    is_initialised = false;
}

static container_handle_t handle_init(void)
{
    struct InterpHandle *new_handle = NULL;

    /* Loop through all allocated handles until one not in use is found */
    for (uint8_t i = 0; i < MAX_HANDLES; i++) {
        if (!handles[i].is_used) {
            new_handle = &handles[i];
            break;
        }
    }

    if (new_handle == NULL) {
        DEBUG_PID("EE unable to get container handle\n");
        return NULL;
    }

    if (handles_in_use == 0 && !runtime_init()) {
        DEBUG_PID("EE unable to init container\n");
        return NULL;
    }

    handles_in_use++;
    new_handle->is_used = true;
    new_handle->is_finished = false;

    return new_handle;

}

static bool handle_destroy(container_handle_t handle)
{
    struct InterpHandle *h = (struct InterpHandle *)handle;

    if (h == NULL || !h->is_used) {
        return false;
    }

    if (h->wasm_buf != NULL) {
        free(h->wasm_buf);
    }
    if (h->module_instance != NULL) {
        wasm_runtime_deinstantiate(h->module_instance);
    }
    if (h->module != NULL) {
        wasm_runtime_unload(h->module);
    }

    handles_in_use--;

    memset(h, 0, sizeof(struct InterpHandle));

    if (handles_in_use == 0) {
        runtime_destroy();
    }

    return true;
}

container_handle_t container_create(memmgr_block_t * data, memmgr_block_t * code)
{
    (void)data;

    LOG_ENTER();

    /* Generic error buffer into which potential errors can be written */
    char error_buf[128];

    /* try to get a new handle. If not possible, just return NULL. */
    struct InterpHandle *new_handle = handle_init();

    if (new_handle == NULL) {
        goto container_create_fail;
    }

    /* Load the module from the bytecode */
    new_handle->module = wasm_runtime_load(
        code -> ptr,
        code -> size,
        error_buf,
        sizeof(error_buf)
        );
    if (!new_handle->module) {
        goto container_create_fail;
    }

    /* Instantiate the module */
    new_handle->module_instance = wasm_runtime_instantiate(
        new_handle->module,
        STACK_SIZE,
        HEAP_SIZE,
        error_buf,
        sizeof(error_buf)
        );
    if (!new_handle->module_instance) {
        goto container_create_fail;
    }

    /* Create an execution environment to go with our module instance */
    new_handle->exec_env = wasm_runtime_create_exec_env(new_handle->module_instance, STACK_SIZE);
    if (!new_handle->exec_env) {
        goto container_create_fail;
    }

    wasm_module_inst_t inst = new_handle->module_instance;

    /* Lookup the associated start, loop and stop functions */
    new_handle->start_func = wasm_runtime_lookup_function(inst, "start", "()");
    new_handle->loop_func = wasm_runtime_lookup_function(inst, "loop", "()i");
    new_handle->stop_func = wasm_runtime_lookup_function(inst, "stop", "()");

    LOG_EXIT();

    return new_handle;

container_create_fail:
    /* determine what went wrong and log it */
    if (!new_handle) {
        DEBUG_PID("EE unable to get container handle\n");
    }
    else if (code -> ptr == NULL) {
        DEBUG_PID("EE unable to read wasm code\n");
    }
    else if (!new_handle->module) {
        DEBUG_PID("EE unable to load wasm module: %s\n", error_buf);
    }
    else if (!new_handle->module_instance) {
        DEBUG_PID("EE unable to instantiate wasm module: %s\n", error_buf);
    }
    else if (!new_handle->exec_env) {
        DEBUG_PID("EE unable to create execution environment: %s\n", error_buf);
    }
    else {
        DEBUG_PID("EE unknown error\n");
    }

    handle_destroy(new_handle);
    LOG_EXIT();
    return NULL;
}

void container_on_start(container_handle_t interp_handle)
{
    struct InterpHandle *handle = (struct InterpHandle *)interp_handle;

    bool call_success = wasm_runtime_call_wasm(
        handle->exec_env,
        handle->start_func,
        0,
        NULL
        );

    if (!call_success) {
        DEBUG_PID("Failed to call start: %s\n", GET_EXCEPTION(handle));
    }
}

int container_on_loop(container_handle_t interp_handle)
{
    struct InterpHandle *handle = (struct InterpHandle *)interp_handle;

    int32_t return_value = -1;

    bool call_success = wasm_runtime_call_wasm(
        handle->exec_env,
        handle->loop_func,
        1,
        (uint32_t *)&return_value
        );

    if (!call_success) {
        DEBUG_PID("Failed to call loop: %s\n", GET_EXCEPTION(handle));
    }

    return return_value;
}

void container_on_stop(container_handle_t interp_handle)
{
    struct InterpHandle *handle = (struct InterpHandle *)interp_handle;

    bool call_success = wasm_runtime_call_wasm(
        handle->exec_env,
        handle->stop_func,
        0,
        NULL
        );

    if (!call_success) {
        DEBUG_PID("Failed to call stop: %s\n", GET_EXCEPTION(handle));
    }
}

void container_on_finalize(container_handle_t interp_handle)
{
    LOG_ENTER();

    handle_destroy(interp_handle);

    LOG_EXIT();
}

bool container_has_finished(container_handle_t interp_handle)
{
    (void)interp_handle;

    LOG_ENTER();
    DEBUG_PID("WW hasfinished() is not yet implemented\n");
    LOG_EXIT();
    return 0;
}
