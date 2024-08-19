/**
 *
 * Copyright (C) 2024, Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_tinycontainer
 *
 * @{
 *
 * @file
 * @brief   API of TinyContainer Controller sub-module for service submodule
 *
 * @author  Samuel Legouix <samuel.legouix@orange.com>
 *
 */

#ifndef TINYCONTAINER_CONTROLLER_CONTROLLER_SERVICE_H
#define TINYCONTAINER_CONTROLLER_CONTROLLER_SERVICE_H

#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief open a remote or local endpoint
 *
 * @param[in]   peer_endpoint_id       local or remote endpoint identifier
 *
 * @return int (a file descriptor or -1 on error)
 */
int tinycontainer_controller_open(uint32_t peer_endpoint_id);

/**
 * @brief close a remote or local endpoint
 *
 * @param[in]   fd   file descriptor
 */
void tinycontainer_controller_close(int fd);

/**
 * @brief attempts to read up to buffer_size bytes from file descriptor
 *
 * @param[in]   fd           file descriptor
 * @param[in]   buffer       a writeable buffer
 * @param[in]   buffer_size  size in byte of the buffer
 *
 * @return int (number of bytes read or -1 on error)
 */
int tinycontainer_controller_read(int fd,
                                  uint8_t *buffer, uint32_t buffer_size);

/**
 * @brief write to a file descriptor
 *
 * @param[in]   fd           file descriptor
 * @param[in]   buffer       a read only buffer
 * @param[in]   buffer_size  size in byte of the buffer
 *
 * @return int (number of bytes read or -1 on error)
 */
int tinycontainer_controller_write(int fd,
                                   const uint8_t *buffer, uint32_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_CONTROLLER_CONTROLLER_SERVICE_H */
/** @} */
