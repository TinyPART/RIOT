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
 * @brief       TinyContainer Container sub-module implementation for native functions
 *
 * @author      Samuel legouix <samuel.legouix@orange.com>
 *
 * @}
 */

//TODO: add a global directive to activate/deactivate console logging
/* by default all messages from containers are send to the console */
#define ENABLE_DEBUG 1
#include "debug.h"

#include <stdbool.h>
#include <string.h>

#include "tinycontainer/runtime/runtime_natives.h"
#include "tinycontainer/service/service_container.h"
#include "tinycontainer/firewall/firewall_all.h"

static service_io_t *get_io(void)
{
    service_shared_mem_t *shared_mem;

    shared_mem = (service_shared_mem_t *)firewall_getcalleecontext();
    service_io_t *io = (service_io_t *)&(shared_mem->rw.io);

    return io;
}

void native_log(char *msg)
{
    DEBUG("%s\n", msg);
}

int32_t native_open(uint32_t endpoint_id)
{
    /* retrieve the io */
    service_io_t *io = get_io();

    /* setup the io */
    io->syscall_id = SERVICE_SYSCALL_OPEN;
    io->fd = endpoint_id;

    /* perform the open syscall */
    int32_t fd = service_syscall();

    return fd;
}

int32_t native_read(int32_t fd, uint8_t *buf, uint32_t size)
{
    /* retrieve the io */
    service_io_t *io = get_io();

    /* setup the io */
    io->syscall_id = SERVICE_SYSCALL_READ;
    io->fd = fd;
    io->size = size;

    /* perform the syscall */
    int32_t nb_of_bytes =  service_syscall();

    if (nb_of_bytes > 0) {
        memcpy(buf, io->buffer, nb_of_bytes);
    }

    return nb_of_bytes;
}

int32_t native_write(int32_t fd, uint8_t *buf, uint32_t size)
{
    /* retrieve the io */
    service_io_t *io = get_io();

    /* setup the io */
    io->syscall_id = SERVICE_SYSCALL_WRITE;
    io->fd = fd;
    io->size = size;
    memcpy(io->buffer, buf, size);

    /* perform the syscall */
    int32_t nb_of_bytes =  service_syscall();

    return nb_of_bytes;
}

int32_t native_close(int32_t fd)
{
    /* retrieve the io */
    service_io_t *io = get_io();

    /* setup the io */
    io->syscall_id = SERVICE_SYSCALL_CLOSE;
    io->fd = fd;

    /* perform the syscall */
    int32_t ret =  service_syscall();

    return ret;
}
