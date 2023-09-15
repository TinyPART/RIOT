/*
 * Copyright (C) 2020-2023 Orange
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
 * @brief       functions that will be exposed to JerryScript runtime
 *
 * @author      Samuel legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#ifndef JERRYSCRIPT_NATIVES_H
#define JERRYSCRIPT_NATIVES_H

#include "jerryscript.h"

#ifdef __cplusplus
extern "C" {
#endif

bool register_natives(void);

#ifdef __cplusplus
}
#endif

#endif /* JERRYSCRIPT_NATIVES_H */
