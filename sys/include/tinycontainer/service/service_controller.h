/**
 *
 * Copyright (C) 2022-2023, Orange
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
 * @brief   API of Tinycontainer Service sub-module for Controller sub-module
 *
 * @author  Samuel Legouix <samuel.legouix@orange.com>
 *
 */


#ifndef TINYCONTAINER_SERVICE_CONTROLLER_H
#define TINYCONTAINER_SERVICE_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief start a container
 *
 * @return bool (true if the container has been started)
 */
int service_start(int container_id);

/**
 * @brief stop a container
 *
 * @return bool (true if the container has been stopped)
 */
int service_stop(int container_id);

/**
 * @brief check if a container is running
 *
 * @return bool (true if the container is running)
 */
int service_isrunning(int container_id);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_SERVICE_CONTROLLER_H */
/** @} */
