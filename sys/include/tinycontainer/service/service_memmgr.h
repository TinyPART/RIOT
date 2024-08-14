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
 * @brief   API of Tinycontainer Service sub-module for Memory Manager sub-module
 *
 * @author  BERKANE Ghilas (ghilas.berkane@gmail.com)
 * @author  Samuel Legouix <samuel.legouix@orange.com>
 *
 */

#ifndef TINYCONTAINER_SERVICE_SERVICE_MEMMGR_H
#define TINYCONTAINER_SERVICE_SERVICE_MEMMGR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ brief retrieve container slot id for the current thread
 *
 * @return int (slot id of the container or -1 if not found)
 */
int service_getcontainerslotid(void);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_SERVICE_SERVICE_MEMMGR_H */
/** @} */
