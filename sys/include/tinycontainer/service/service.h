/**
 *
 * Copyright (C) 2020-2023, Orange
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
 * @brief   API of Tinycontainer Service sub-module
 *
 * @author  BERKANE Ghilas (ghilas.berkane@gmail.com)
 *
 */

#ifndef TINYCONTAINER_SERVICE_SERVICE_H
#define TINYCONTAINER_SERVICE_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialize TinyContainer service thread
 *
 * @return int (pid of thread service)
 */
int tinycontainer_service_init(int service_prio, int container_prio);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_SERVICE_SERVICE_H */
/** @} */
