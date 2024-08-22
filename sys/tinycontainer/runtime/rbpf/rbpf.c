/*
 * Copyright (C) 2023 Freie Universität Berlin
 * Copyright (C) 2023 Koen Zandberg <koen@bergzand.net>
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
 * @brief       TinyContainer rBPF runtime implementation
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tinycontainer/runtime/runtime_impl.h"
#include "tinycontainer/memmgr/memmgr_container.h"
#include "runtime_rbpf.h"


#define ENABLE_DEBUG 0
#include "tinycontainer/debugging.h"

static struct RbpfHandle handles[CONFIG_TINYCONTAINER_RBPF_MAX_HANDLES] = { 0 };
static size_t handles_in_use = 0;

static bool is_initialised = false;
static bool runtime_init(void)
{
    /* if already initialised, return early */
    if (is_initialised) {
        return true;
    }

    is_initialised = true;
    return true;
}

static void runtime_destroy(void)
{}

static runtime_handle_t handle_init(void)
{
    struct RbpfHandle *new_handle = NULL;

    /* Loop through all allocated handles until one not in use is found */
    for (size_t i = 0; i < CONFIG_TINYCONTAINER_RBPF_MAX_HANDLES; i++) {
        if (!tinycontainer_rbpf_is_used(&handles[i])) {
            new_handle = &handles[i];
            break;
        }
    }

    if (handles_in_use == 0) {
        runtime_init();
    }

    handles_in_use++;
    new_handle->is_used = true;
    return new_handle;
}

static bool handle_destroy(runtime_handle_t handle)
{
    struct RbpfHandle *old_handle = (struct RbpfHandle *)handle;

    if (!old_handle || !tinycontainer_rbpf_is_used(old_handle)) {
        return false;
    }

    memset(old_handle, 0, sizeof(struct RbpfHandle));
    handles_in_use--;

    if (handles_in_use == 0) {
        runtime_destroy();
    }

    return true;
}

runtime_handle_t runtime_create(memmgr_block_t *data, memmgr_block_t *code)
{
    (void)data;

    struct RbpfHandle *new_handle = handle_init();

    if (!new_handle) {
        return NULL;
    }

    rbpf_application_setup(&new_handle->app, new_handle->stack,
                           (const rbpf_application_t *)(void *)code->ptr, code->size);

    return new_handle;
}

void runtime_on_start(runtime_handle_t interp_handle)
{
    struct RbpfHandle *handle = (struct RbpfHandle *)interp_handle;

    (void)handle;
}

int runtime_on_loop(runtime_handle_t interp_handle)
{
    struct RbpfHandle *handle = (struct RbpfHandle *)interp_handle;

    int64_t return_value = -1;

    int res = rbpf_application_run_ctx(&handle->app, NULL, 0, &return_value);

    if (res < 0) {
        DEBUG_PID("Failed to call loop: %d\n", res);
    }

    return return_value;
}

void runtime_on_stop(runtime_handle_t interp_handle)
{
    struct RbpfHandle *handle = (struct RbpfHandle *)interp_handle;

    (void)handle;
}

void runtime_on_finalize(runtime_handle_t interp_handle)
{
    handle_destroy(interp_handle);
}
