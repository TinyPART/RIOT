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
 * @brief       TinyContainer Service sub-module implementation for endpoint fd
 *
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#include <stddef.h>
#include <stdbool.h>

#include "service_endpoint_fd.h"

/* constants
 * ---------
 */

/* max number of fd supported by the system */
#ifdef TINYCONTAINER_MAX_FD
#define MAX_FD TINYCONTAINER_MAX_FD
#else
#define MAX_FD 10
#endif /* TINYCONTAINER_MAX_FD */

/* local variables
 * ---------------
 */
static fd_t fds[MAX_FD];
static int next_id = 1;
static bool has_been_initialized = false;

/* private functions
 * -----------------
 */

/* reset a fd to unused state */
static void fd_reset(fd_t *pfd)
{
    pfd->slot_id = 0;
    pfd->endpoint_id = 0;
    pfd->fd = -1;
    pfd->peer.type = -1;
    pfd->peer.container.slot_id = 0;
    pfd->peer.container.endpoint_id = 0;
}

/* initialize our all fd to unused state */
static void init_fds(void)
{
    for (int i = 0; i < MAX_FD; i++) {
        fd_reset(&fds[i]);
    }
    has_been_initialized = true;
}

/* return the first unused fd */
static fd_t *search_free_fd(void)
{
    if (!has_been_initialized) {
        init_fds();
        return &fds[0];
    }

    for (int i = 0; i < MAX_FD; i++) {
        if (fds[i].fd == -1) {
            return &fds[i];
        }
    }

    /* not free fd found */
    return NULL;
}

/* search and return a fd */
static fd_t *search_fd(int fd)
{
    if (!has_been_initialized) {
        return NULL;
    }

    for (int i = 0; i < MAX_FD; i++) {
        if (fds[i].fd == fd) {
            return &fds[i];
        }
    }

    /* fd not found in fds array */
    return NULL;
}

/* search and return a fd associated with a container endpoint */
static fd_t *search(uint8_t slot_id, uint32_t endpoint_id)
{
    if (!has_been_initialized) {
        return NULL;
    }

    for (int i = 0; i < MAX_FD; i++) {
        if (fds[i].fd > 0 &&
            fds[i].slot_id == slot_id &&
            fds[i].endpoint_id == endpoint_id) {
            return &fds[i];
        }
    }

    /* fd not found in fds array */
    return NULL;
}

/* search and return a fd identifier not already in use */
static int get_next_id(void)
{
    /* check if the next_id value is free */
    while (search_fd(next_id) != NULL) {
        next_id++;
    }

    /* avoid negative value which is used for error */
    if (next_id <= 0) {
        next_id = 1;
        return get_next_id();
    }

    return next_id++;
}

/* public functions
 * ----------------
 */

/* records a new container endpoint and associates it to a new fd */
int fd_new(uint8_t slot_id, uint32_t endpoint_id, uint8_t direction)
{
    fd_t *fd = search_free_fd();

    if (fd == NULL) {
        return -1;
    }

    fd->fd = get_next_id();
    if (fd->fd <= 0) {
        return -1;
    }

    fd->slot_id = slot_id;
    fd->endpoint_id = endpoint_id;
    fd->direction = direction;

    return fd->fd;
}

/* close a fd and free associated data */
void fd_close(int fd)
{
    fd_t *pfd = search_fd(fd);

    if (pfd != NULL) {
        fd_reset(pfd);
    }
}

/* associated a fd to a local peer */
void fd_set_local_peer(int fd, int peer_fd)
{
    fd_t *pfd = search_fd(fd);

    if (pfd != NULL) {
        pfd->peer.type = 1;
        pfd->peer.local.fd = peer_fd;
    }
}

/* associated a fd to a remote peer */
void fd_set_remote_peer(int fd, int peer_fd)
{
    fd_t *pfd = search_fd(fd);

    if (pfd != NULL) {
        pfd->peer.type = 2;
        pfd->peer.remote.fd = peer_fd;
    }
}

/* associated a fd to a container peer */
void fd_set_container_peer(int fd, uint8_t slot_id, uint32_t endpoint_id)
{
    fd_t *pfd = search_fd(fd);

    if (pfd == NULL) {
        pfd->peer.type = 0;
        pfd->peer.container.slot_id = slot_id;
        pfd->peer.container.endpoint_id = endpoint_id;
    }
}

/* check if a container endpoint is already open */
bool fd_is_open(uint8_t slot_id, uint32_t endpoint_id)
{
    fd_t *pfd = search(slot_id, endpoint_id);

    if (pfd != NULL) {
        /* found */
        return true;
    }
    else {
        /* not found */
        return false;
    }
}

/* return the peer type of a fd */
uint8_t fd_get_peer_type(int fd)
{
    fd_t *pfd = search_fd(fd);

    if (pfd == NULL) {
        /*not found*/
        return 0;
    }
    return pfd->peer.type;
}

/* return the container slot id associated with a fd */
uint8_t fd_get_peer_container_slot_id(int fd)
{
    fd_t *pfd = search_fd(fd);

    if (pfd == NULL) {
        /*not found*/
        return 0;
    }
    return pfd->peer.container.slot_id;
}

/* return the container endpoint identifier associated with a fd */
uint32_t fd_get_peer_container_endpoint_id(int fd)
{
    fd_t *pfd = search_fd(fd);

    if (pfd == NULL) {
        /*not found*/
        return 0;
    }
    return pfd->peer.container.endpoint_id;
}

/* return the peer local fd associated with a fd */
uint32_t fd_get_peer_local_fd(int fd)
{
    fd_t *pfd = search_fd(fd);

    if (pfd == NULL) {
        /*not found*/
        return 0;
    }
    return pfd->peer.local.fd;
}

/* return the peer remote fd associated with a fd */
uint32_t fd_get_peer_remote_fd(int fd)
{
    fd_t *pfd = search_fd(fd);

    if (pfd == NULL) {
        /*not found*/
        return 0;
    }
    return pfd->peer.remote.fd;
}

/* check if a fd is readable */
bool fd_is_readable(uint8_t slot_id, int fd)
{
    fd_t *pfd = search_fd(fd);

    if (pfd == NULL) {
        /*fd not found*/
        return false;
    }
    return (pfd->slot_id == slot_id) &&
           (pfd->direction == 1 || pfd->direction == 2);
}

/* check if a fd is writeable */
bool fd_is_writeable(uint8_t slot_id, int fd)
{
    fd_t *pfd = search_fd(fd);

    if (pfd == NULL) {
        /*fd not found*/
        return false;
    }
    return (pfd->slot_id == slot_id) &&
           (pfd->direction == 0 || pfd->direction == 2);
}
