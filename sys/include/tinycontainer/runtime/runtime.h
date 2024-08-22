/**
 *
 * Copyright (C) 2020-2023, Orange.
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
 * @brief   API of Tinycontainer Container sub-module
 *
 * @author  Samuel Legouix <samuel.legouix@orange.com>
 *
 */

#ifndef TINYCONTAINER_RUNTIME_RUNTIME_H
#define TINYCONTAINER_RUNTIME_RUNTIME_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   initialisation of runtime
 *
 * This function should be call before any container runtime creation.
 *
 * @ return bool (true on success)
 */
bool runtime_init(void);

/**
 * @brief   handler for the container runtime thread
 *
 * @param[in]   arg         Not use yet
 */
void *runtime_handler(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_RUNTIME_RUNTIME_H */
/** @} */
