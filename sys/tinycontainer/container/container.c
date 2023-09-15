/*
 * Copyright (C) 2020-2023 Orange
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

#include "tinycontainer/container/container.h"
#include "tinycontainer/container/container_runtime.h"
#include "tinycontainer/memmgr/memmgr_container.h"

#include "ztimer.h"

bool container_init(void)
{
    /* not used yet */
    return true;
}

void *container_handler(void *arg)
{
    /* TODO: retrieve metadata from arg */

    /* retrieve the data and code part of the container */

    memmgr_block_t data;
    memmgr_block_t code;
    if (memmgr_getcontainer(&data, &code) < 0) {
        DEBUG("container_handler: failed to retrieve container from memory manager\n");
        return NULL;
    }

    /* create the container */

    container_handle_t handle = container_create(&data, &code);

    if (!handle) {
        DEBUG("container_handler: failed to create container\n");
        return NULL;
    }

    container_on_start(handle);

    uint32_t period = ((uint32_t)arg);
    uint32_t last_wakeup = ztimer_now(ZTIMER_MSEC);

    while (container_on_loop(handle) == 0) {
        ztimer_periodic_wakeup(ZTIMER_MSEC, &last_wakeup, period);
    }

    container_on_stop(handle);
    container_on_finalize(handle);

    return NULL;
}
