/*
 * Copyright (C) 2023 Orange
 *
 * Please, refer to the README.md and LICENSE files of TinyContainer
 *
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

#ifndef TINYCONTAINER_H
#define TINYCONTAINER_H

#include <stdint.h>

#include "sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  setup and start TinyContainer
 *
 * This function starts the two main processes of TinyContainer: a service
 * thread and a controller thread . The service thread is in charge of running
 * containers and the controller thread is used to manage containers lifecycle.
 *
 * The function first create the service thread and, on success, create the
 * controller thread.
 *
 * @param[in]   controller_prio    thread priority of thre Controller thread
 * @param[in]   service_prio       thread priority of the Service thread
 * @param[in]   containers_prio    thread priority of containers
 *
 * @return a the PID of the controller thread
 */
kernel_pid_t tinycontainer_init(uint8_t controller_prio, uint8_t service_prio,
                                uint8_t containers_prio);

/**
 * @brief load a new container
 *
 * @return bool (true if the container has been loaded)
 */
bool tinycontainer_loadcontainer(uint8_t *metadata, int metadata_size,
                                 uint8_t *data, int data_size,
                                 uint8_t *code, int code_size);

/**
 * @brief start a container
 *
 * @return bool (true if the container has been started)
 */
bool tinycontainer_startcontainer(uint8_t *container_uid);

/**
 * @brief stop a container
 *
 * @return bool (true if the container has been stopped)
 */
bool tinycontainer_stopcontainer(uint8_t *container_uid);

/**
 * @brief check the running status of a container
 *
 * @return bool (true if the container is running)
 */
bool tinycontainer_iscontainerrunning(uint8_t *container_uid);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_H */
/** @} */
