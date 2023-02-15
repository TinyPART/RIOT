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
 * @brief   API of TinyContainer Memory Manager sub-module
 *
 * @author BERKANE Ghilas (ghilas.berkane@gmail.com)
 *
 */


#ifndef TINYCONTAINER_MEMMGR_H
#define TINYCONTAINER_MEMMGR_H

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

#endif /* TINYCONTAINER_MEMMGR_H */
/** @} */
