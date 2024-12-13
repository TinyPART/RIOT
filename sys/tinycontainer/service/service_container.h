/*
 * Copyright (C) 2024, Orange.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_tinycontainer
 * @{
 *
 * @file
 * @brief       TinyContainer Service sub-module definitions for container
 *
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#ifndef SERVICE_CONTAINER_H
#define SERVICE_CONTAINER_H

#include <thread.h>
#include <stdint.h>

#include "tinycontainer/service/service_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UID_MAXSIZE 64

#define BIT(x) (1 << x)
#define CONTAINER_FLAGS_PROVISIONNING  BIT(0)
#define CONTAINER_FLAGS_READY          BIT(1)
#define CONTAINER_FLAGS_RUNNING        BIT(2)
#define CONTAINER_FLAGS_STOPPING       BIT(3)
#define CONTAINER_FLAGS_DELETING       BIT(4)

typedef struct {
    /* internal slot id of the container */
    uint8_t id;
    /* container uid retrieving from metadata */
    uint8_t uid[UID_MAXSIZE];
    /* thread stack size */
    uint8_t stack[4 * THREAD_STACKSIZE_SMALL];
    /* pid of the thread that run this container */
    kernel_pid_t pid;
    /* bit array to manage the container state */
    uint32_t flags;
    /* bitmask for native calls (from metadata) */
    uint32_t natives_mask;
    /* shared memory with the container */
    service_shared_mem_t shared_memory;
} container_t;

static inline bool container_isprovisionning(container_t *container)
{
    return container->flags & CONTAINER_FLAGS_PROVISIONNING;
}

static inline bool container_isready(container_t *container)
{
    return container->flags & CONTAINER_FLAGS_READY;
}

static inline bool container_isrunning(container_t *container)
{
    return container->flags & CONTAINER_FLAGS_RUNNING;
}

static inline bool container_isstopping(container_t *container)
{
    return container->flags & CONTAINER_FLAGS_STOPPING;
}

static inline bool container_isdeleting(container_t *container)
{
    return container->flags & CONTAINER_FLAGS_DELETING;
}

static inline kernel_pid_t container_getpid(container_t *container)
{
    return container->pid;
}

static inline void container_setrunning(container_t *container)
{
    container->flags |= CONTAINER_FLAGS_RUNNING;
    container->flags &= ~CONTAINER_FLAGS_READY;
}

static inline void container_setstopping(container_t *container)
{
    container->flags |= CONTAINER_FLAGS_STOPPING;
    container->flags &= ~CONTAINER_FLAGS_STOPPING;
}

static inline void container_setdeleting(container_t *container)
{
    container->flags |= CONTAINER_FLAGS_STOPPING;
    container->flags &= ~CONTAINER_FLAGS_DELETING;
}

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_CONTAINER_H */
