/*
 * Copyright (C) 2022-2023, Orange.
 *
 * Please, refer to the README.md and LICENSE files of TinyContainer
 *
 */

/**
 * @ingroup     sys_tinycontainer
 * @{
 *
 * @file
 * @brief       functions that will be exposed to WAMR runtime
 *
 * @author      Gregory Holder <gregory.holder@orange.com>
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
