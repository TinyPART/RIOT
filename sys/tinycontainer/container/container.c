/*
 * Copyright (C) 2020-2024 Orange
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
 * @brief       TinyContainer Container sub-module implementation
 *
 * @author      Samuel legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#define ENABLE_DEBUG 0
#include "debug.h"

#include "tinycontainer/runtime/runtime.h"
#include "tinycontainer/runtime/runtime_impl.h"
#include "tinycontainer/memmgr/memmgr_container.h"
#include "tinycontainer/service/service_container.h"

#include "ztimer.h"

bool runtime_init(void)
{
    /* not used yet */
    return true;
}

void *runtime_handler(void *arg)
{
    /* retrieve the data and code part of the container */

    memmgr_block_t data;
    memmgr_block_t code;

    if (memmgr_getcontainer(&data, &code) < 0) {
        DEBUG("runtime_handler: failed to retrieve container from memory manager\n");
        return NULL;
    }

    /* create the container */

    runtime_handle_t handle = runtime_create(&data, &code);

    if (!handle) {
        DEBUG("runtime_handler: failed to create container\n");
        return NULL;
    }

    runtime_on_start(handle);

    service_shared_mem_t *shared_memory = (service_shared_mem_t *)arg;

    //FIXME: the documentation said that the clock shall be acquired
    uint32_t last_wakeup = ztimer_now(ZTIMER_MSEC);

    while (runtime_on_loop(handle) == 0) {
        ztimer_periodic_wakeup(ZTIMER_MSEC, &last_wakeup,
                               shared_memory->ro.period);
    }

    runtime_on_stop(handle);
    runtime_on_finalize(handle);

    return NULL;
}
