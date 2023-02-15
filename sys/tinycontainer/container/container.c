/*
 * Copyright (C) 2020-2023 Orange
 *
 * Please, refer to the README.md and LICENSE files of TinyContainer
 *
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


#include "debug.h"

#include "tinycontainer/container/container.h"
#include "tinycontainer/container/container_runtime.h"

#include "ztimer.h"

bool container_init(void)
{
    /* not used yet */
    return true;
}

void *container_handler(void *arg)
{
    /* TODO: retrieve metadata from arg */

    container_handle_t handle = container_create();

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
