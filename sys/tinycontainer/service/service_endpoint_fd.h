/*
 * Copyright (C) 2022-2023, Orange.
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
 * @brief       TinyContainer Service sub-module definitions for endpoint io
 *
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#ifndef SERVICE_ENDPOINT_FD_H
#define SERVICE_ENDPOINT_FD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int slot_id;
    uint32_t endpoint_id;
    int fd;
    struct {
        int type;
        union {
            struct {
                uint8_t slot_id;
                uint32_t endpoint_id;
            } container;
            struct {
                int fd;
            } local;
            struct {
                int fd;
            } remote;
        };
    } peer;
    uint8_t direction;
} fd_t;

/* return a new file descriptor */
int fd_new(uint8_t slot_id, uint32_t endpoint_id, uint8_t direction);

/* close a file descriptor */
void fd_close(int fd);

/* functions to set the opaque fd structure */
void fd_set_slot_id(int fd, int8_t slot_id);
void fd_set_endpoint_id(int fd, uint32_t endpoint_id);
void fd_set_local_peer(int fd, int peer_fd);
void fd_set_remote_peer(int fd, int remote_fd);
void fd_set_container_peer(int fd, uint8_t slot_id, uint32_t endpoint_id);

/* functions to get some data from the opaque fd structure */
uint8_t fd_get_peer_type(int fd);
uint32_t fd_get_peer_local_fd(int fd);
uint32_t fd_get_peer_remote_fd(int fd);
uint8_t fd_get_peer_container_slot_id(int fd);
uint32_t fd_get_peer_container_endpoint_id(int fd);
bool fd_is_readable(uint8_t slot_id, int fd);
bool fd_is_writeable(uint8_t slot_id, int fd);

/* function to check if a specific container endpoint is open */
bool fd_is_open(uint8_t slot_id, uint32_t endpoint_id);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_ENDPOINT_FD_H */
