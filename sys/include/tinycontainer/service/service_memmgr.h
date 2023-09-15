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
 * @brief   API of Tinycontainer Service sub-module for Memory Manager sub-module
 *
 * @author  BERKANE Ghilas (ghilas.berkane@gmail.com)
 *
 */

#ifndef TINYCONTAINER_SERVICE_SERVICE_MEMMGR_H
#define TINYCONTAINER_SERVICE_SERVICE_MEMMGR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ brief retrieve container slot id from thread pid
 *
 * @return int (id of the container or -1 if not found)
 */
int service_getcontaineridfrompid(int pid);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_SERVICE_SERVICE_MEMMGR_H */
/** @} */
