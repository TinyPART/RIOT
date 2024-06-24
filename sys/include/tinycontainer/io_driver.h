/*
 * Copyright (C) 2024 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_tinycontainer TinyContainer
 * @ingroup     sys
 * @brief       Containerisation solution for RIOT that support WASM and JerryScript.
 *
 * @experimental
 *
 * @{
 *
 * @file
 * @brief       Containerisation solution for RIOT that support WASM and JerryScript.
 *
 * @author      Samuel legouix <samuel.legouix@orange.com>
 */

#ifndef TINYCONTAINER_IO_DRIVER_H
#define TINYCONTAINER_IO_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int (*open)(uint8_t* uri, uint32_t uri_size);
  int (*close)(int fd);
  int (*read)(int fd, uint8_t* buffer, size_t buffer_size);
  int (*write)(int fd, uint8_t* buffer, size_t buffer_size);
} io_driver_t;

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_IO_DRIVER_H */
/** @} */
