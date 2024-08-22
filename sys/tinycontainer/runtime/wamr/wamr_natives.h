/*
 * Copyright (C) 2022-2023, Orange.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_tinycontainer
 * @{
 *
 * @file
 * @brief       functions that will be exposed to WAMR runtime
 *
 * @author      Gregory Holder <gregory.holder76@gmail.com>
 *
 * @}
 */

#ifndef WAMR_NATIVES_H
#define WAMR_NATIVES_H

#ifdef __cplusplus
extern "C" {
#endif

bool register_natives(void);

#ifdef __cplusplus
}
#endif

#endif /* WAMR_NATIVES_H */
