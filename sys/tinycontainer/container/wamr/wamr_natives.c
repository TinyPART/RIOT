/*
 * Copyright (C) 2022-2023 Orange
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



#include <stdio.h>
#include "wasm_export.h"
#include "wamr_natives.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* example of some native functions */

#if !defined(BOARD_NATIVE)
static void foo(void)
{
    printf("foo\n");
}

static void bar(void)
{
    printf("bar\n");
}
#endif



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
        EXPORT_WASM_API(foo),
        EXPORT_WASM_API(bar),
    };

    int n_native_symbols = sizeof(native_symbols) / sizeof(NativeSymbol);

    if (wasm_runtime_register_natives("env", native_symbols, n_native_symbols)) {
        return true;
    }
#endif /*BOARD_NATIVE*/

    DEBUG("Could not register native functions with wasm runtime\n");
    return false;
}
