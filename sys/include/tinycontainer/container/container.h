/**
 *
 * Copyright (C) 2020-2023, Orange.
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
 * @brief   API of Tinycontainer Container sub-module
 *
 * @author  Samuel Legouix <samuel.legouix@orange.com>
 *
 */

#ifndef TINYCONTAINER_CONTAINER_CONTAINER_H
#define TINYCONTAINER_CONTAINER_CONTAINER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   initialisation of container runtime
 *
 * This function should be call before any container creation.
 *
 * @ return bool (true on success)
 */
bool container_init(void);

/**
 * @brief   handler for the container thread
 *
 * @param[in]   arg         Not use yet
 */
void *container_handler(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_CONTAINER_CONTAINER_H */
/** @} */
