/*
 * Copyright (C) 2020-2023 Orange
 *
 * Please, refer to the README.md and LICENSE files of TinyContainer
 *
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
