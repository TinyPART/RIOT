/**
 *
 * Copyright (C) 2020-2024, Orange
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
 * @brief   API of TinyContainer Controller sub-module
 *
 * @author  BERKANE ghilas <ghilas.berkane@gmail.com>
 * @author  Samuel Legouix <samuel.legouix@orange.com>
 *
 */

#ifndef TINYCONTAINER_CONTROLLER_CONTROLLER_H
#define TINYCONTAINER_CONTROLLER_CONTROLLER_H

#include <string.h>
#include <stdbool.h>

#include "tinycontainer/io_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialize the thread controller
 *
 * @return int (pid of thread controller)
 */
int tinycontainer_controller_init(int prio, io_driver_t * io_driver);

/**
 * @brief start a container
 *
 * @return bool (true if the container has been started)
 */
bool controller_start(int container_id);

/**
 * @brief stop a container
 *
 * @return bool (true if the container has been stopped)
 */
bool controller_stop(int container_id);

/**
 * @brief check running status of container
 *
 * @return bool (true if the container is running)
 */
bool controller_isrunning(int container_id);

/**
 * @brief load a container
 *
 * @return bool (true if the container has been loaded)
 */
bool controller_load(
    uint8_t *metadata,
    int meta_size,
    uint8_t *data,
    int data_size,
    uint8_t *code,
    int code_len
    );

/**
 * @brief get slot id for a container based on container uid
 *
 * @return int (container slot id if exist or -1 otherwise)
 */
int controller_get_slot_id(uint8_t *uid, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_CONTROLLER_CONTROLLER_H */
/** @} */
