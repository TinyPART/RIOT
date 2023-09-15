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
 * @brief   API of TinyContainer Memory Manager sub-module
 *
 * @author BERKANE Ghilas (ghilas.berkane@gmail.com)
 *
 */

#ifndef TINYCONTAINER_MEMMGR_MEMMGR_H
#define TINYCONTAINER_MEMMGR_MEMMGR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialize TinyContainer Memory Manager Module
 *
 * @return int (-1 on error)
 */
int tinycontainer_memmgr_init(void);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_MEMMGR_MEMMGR_H */
/** @} */
