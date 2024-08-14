/*
 * Copyright (C) 2024 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_tinycontainer
 *
 * @{
 *
 * @file
 * @brief       Structure definition for IO Driver
 *
 * @author      Samuel legouix <samuel.legouix@orange.com>
 */

#ifndef TINYCONTAINER_CONTROLLER_CONTROLLER_IO_DRIVER_H
#define TINYCONTAINER_CONTROLLER_CONTROLLER_IO_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief structure to record an external input output driver
 */
typedef struct {
    /** @brief bsd style open function recorded in the io driver */
    int (*open)(uint32_t peer_endpoint_id);
    /** @brief bsd style close function recorded in the io driver */
    int (*close)(int fd);
    /** @brief bsd style read function recorded in the io driver */
    int (*read)(int fd, uint8_t *buffer, size_t buffer_size);
    /** @brief bsd style write function recorded in the io driver */
    int (*write)(int fd, uint8_t *buffer, size_t buffer_size);
} controller_io_driver_t;

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_CONTROLLER_CONTROLLER_IO_DRIVER_H */
/** @} */
