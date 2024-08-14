/*
 * Copyright (C) 2022-2024 Orange
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
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#include <stdio.h>
#include "wasm_export.h"
#include "wamr_natives.h"

#include "tinycontainer/container/container_natives.h"

#define ENABLE_DEBUG 0

#include "debug.h"

void wamr_native_log(wasm_exec_env_t exec_env, char *msg)
{
    (void)exec_env;
    native_log(msg);
}

int32_t wamr_native_open(wasm_exec_env_t exec_env, uint32_t endpoint_id)
{
    (void)exec_env;
    return native_open(endpoint_id);
}

int32_t wamr_native_read(wasm_exec_env_t exec_env, int32_t fd, uint8_t *buf,
                         uint32_t size)
{
    (void)exec_env;
    return native_read(fd, buf, size);
}

int32_t wamr_native_write(wasm_exec_env_t exec_env, int32_t fd, uint8_t *buf,
                          uint32_t size)
{
    (void)exec_env;
    return native_write(fd, buf, size);
}

int32_t wamr_native_close(wasm_exec_env_t exec_env, int32_t fd)
{
    (void)exec_env;
    return native_close(fd);
}

/* registering native functions */
bool register_natives(void)
{
#if defined(BOARD_NATIVE)
#pragma message ( "The native board doesn't support registering wasm native functions!" )
    return true;
#else
    DEBUG("Registering natives\n");
    static NativeSymbol native_symbols[] =
    {
        { "logger", wamr_native_log,   "($)",    NULL },
        { "open",   wamr_native_open,  "(i)i",  NULL },
        { "read",   wamr_native_read,  "(i*~)i", NULL },
        { "write",  wamr_native_write, "(i*~)i", NULL },
        { "close",  wamr_native_close, "(i)i",   NULL },
    };

    int n_native_symbols = ARRAY_SIZE(native_symbols);

    if (wasm_runtime_register_natives("env", native_symbols, n_native_symbols)) {
        return true;
    }
#endif /*BOARD_NATIVE*/

    DEBUG("Could not register native functions with wasm runtime\n");
    return false;
}
