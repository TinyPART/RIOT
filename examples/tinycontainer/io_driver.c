/*
 * Copyright (C) 2024 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory of the TinyContainer module for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       TinyContainer example
 *
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#include "tinycontainer.h"

#include <stdio.h>

/* A rudimentary implementation to eval IO Driver functions */

static int next_fd = 1000;
static uint8_t next_byte = 0;

static int io_open(uint32_t endpoint_id)
{
    printf("IO:%d: open %d\n", next_fd, (int)endpoint_id);
    return next_fd++;
}

static int io_close(int fd)
{
    printf("IO:%d: close\n", fd);
    return 0;
}

static int io_read(int fd, uint8_t *buffer, size_t buffer_size)
{
    if (fd < 0) {
        printf("IO:%d: read - error\n", fd);
        return -1;
    }

    printf("IO:%d: read %d bytes\n", fd, buffer_size);
    if (buffer_size > 0) {
        for (int i = 0; i < (int)buffer_size; i++) {
            buffer[i] = next_byte++;
            printf("%02X ", buffer[i]);
        }
        printf("\n");
    }

    return buffer_size;
}

static int io_write(int fd, uint8_t *buffer, size_t buffer_size)
{
    if (fd < 0) {
        printf("IO:%d: write - error\n", fd);
        return -1;
    }

    printf("IO:%d: write %d bytes\n", fd, buffer_size);
    if (buffer_size > 0) {
        for (int i = 0; i < (int)buffer_size; i++) {
            printf("%02X ", buffer[i]);
        }
        printf("\n");
    }

    return buffer_size;
}

tinycontainer_io_driver_t io_driver = {
    .open = io_open,
    .close = io_close,
    .read = io_read,
    .write = io_write
};
