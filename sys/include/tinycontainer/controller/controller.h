/**
 *
 * Copyright (C) 2020-2023, Orange
 *
 * Please, refer to the README.md and LICENSE files of TinyContainer
 *
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
 *
 */

#ifndef TINYCONTAINER_CONTROLLER_CONTROLLER_H
#define TINYCONTAINER_CONTROLLER_CONTROLLER_H

#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialize the thread controller
 *
 * @return int (pid of thread controller)
 */
int tinycontainer_controller_init(int prio);

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

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_CONTROLLER_CONTROLLER_H */
/** @} */
