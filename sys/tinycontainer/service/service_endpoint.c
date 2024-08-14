/*
 * Copyright (C) 2024 Orange
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
 * @brief       TinyContainer Service sub-module implementation for endpoint
 *
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#include "service_container.h"
#include "service_endpoint.h"
#include "service_endpoint_fd.h"
#include "tinycontainer/memmgr/memmgr_service.h"
#include "tinycontainer/controller/controller_service.h"

uint32_t endpoint_open(container_t *container)
{

    service_io_t *io = (service_io_t *)&container->shared_memory.rw.io;

    /* for open function the peer endpoint id is stored in io -> fd */
    if (io->fd < 0) {
        /* invalid endpoint id */
        return -1;
    }

    /* retrieve the metadata endpoint */
    metadata_endpoint_t endpoint;
    int ret = memmgr_getendpoint(container->id, io->fd, &endpoint);

    if (ret != METADATA_OK) {
        return -1;
    }

    /* check if the endpoint is already opened */
    if (fd_is_open(container->id, endpoint.id)) {
        /* opening a endpoint twice is not allowed */
        return -1;
    }

    /* get a new fd */
    int fd = fd_new(container->id, endpoint.id, endpoint.direction);
    int peer_fd;

    switch (endpoint.peer_type) {
    case 0: /* container peer */
        //TODO: how to get peer slot?
        //fd_set_container_peer(fd, slot_id, endpoint.peer_endpoint_id);
        break;
    case 1: /* local peer */
        peer_fd = tinycontainer_controller_open(endpoint.peer_endpoint_id);
        if (peer_fd <= 0) {
            fd_close(fd);
            return -1;
        }
        fd_set_local_peer(fd, peer_fd);
        break;
    case 2: /* remote peer */
        peer_fd = tinycontainer_controller_open(endpoint.peer_endpoint_id);
        if (peer_fd <= 0) {
            fd_close(fd);
            return -1;
        }
        fd_set_remote_peer(fd, peer_fd);
        break;
    default:
        /* unsupported peer type */
        fd_close(fd);
        return -1;
    }

    return fd;
}

uint32_t endpoint_close(container_t *container)
{

    service_io_t *io = (service_io_t *)&container->shared_memory.rw.io;

    if (io->size == 0) {   //FIXME: is size shall be non null?
        io->fd = -1;
        return -1;
    }

    int fd = io->fd;

    if (fd <= 0) {
        /*invalid fd*/
        return -1;
    }

    uint8_t type = fd_get_peer_type(fd);
    int peer_fd = -1;

    switch (type) {
    case 0: /*container peer*/
        //TODO: not yet implemented
        break;
    case 1: /*local peer*/
        peer_fd = fd_get_peer_local_fd(fd);
        tinycontainer_controller_close(peer_fd);
        break;
    case 2: /*remote peer*/
        peer_fd = fd_get_peer_remote_fd(fd);
        tinycontainer_controller_close(peer_fd);
        break;
    default: /*invalid peer type*/
        /* should not occurs*/
        return -1;
    }

    /* release the file descriptor */
    fd_close(fd);

    return 0;
}

typedef bool (*allowed_t)(uint8_t, int);
typedef int (*operation_t)(int, uint8_t *, uint32_t);
static uint32_t read_or_write(container_t *container, allowed_t allowed,
                              operation_t operation)
{

    /* retrieve container io structure */
    service_io_t *io = (service_io_t *)&container->shared_memory.rw.io;

    /* check that we have a consistent io for performing a read or a write
     * operation
     */
    if (!allowed(container->id, io->fd) || io->size == 0) {
        io->fd = -1;
        return -1;
    }

    /* the read or write depend on the endpoint type */
    uint8_t type = fd_get_peer_type(io->fd);
    int peer_fd = -1;
    int nb_of_bytes = -1;

    switch (type) {
    case 0: /*container peer*/
        //TODO: not yet implemented
        break;
    case 1: /*local peer*/
        peer_fd = fd_get_peer_local_fd(io->fd);
        nb_of_bytes = operation(peer_fd, io->buffer, io->size);
        break;
    case 2: /*remote peer*/
        peer_fd = fd_get_peer_remote_fd(io->fd);
        nb_of_bytes = operation(peer_fd, io->buffer, io->size);
        break;
    default: /*invalid peer type*/
        /* should not occurs*/
        return -1;
    }

    return nb_of_bytes;
}

uint32_t endpoint_read(container_t *container)
{
    return read_or_write(container,
                         fd_is_readable,
                         tinycontainer_controller_read);
}

uint32_t endpoint_write(container_t *container)
{
    /* note: the compilator need the cast to operation_t as the 2nd args passed
     *       to the write operation is declared as constant (the buffer is not
     *       changed during a write operation).
     */
    return read_or_write(container,
                         fd_is_writeable,
                         (operation_t)tinycontainer_controller_write);
}
