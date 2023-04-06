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

#include "debug.h"

#include "string.h"
#include "jerryscript.h"

#include "jerryscript_natives.h"

static jerry_value_t bar_handler( const jerry_value_t func_value,
                                  const jerry_value_t this_value,
                                  const jerry_value_t *args_p,
                                  const jerry_length_t args_cnt);

static jerry_value_t baz_handler( const jerry_value_t func_value,
                                  const jerry_value_t this_value,
                                  const jerry_value_t *args_p,
                                  const jerry_length_t args_cnt);

bool register_natives(void)
{
    DEBUG("[%d] -> jerryscript:register_natives()\n", thread_getpid());

    jerry_value_t global_object;
    jerry_value_t riot_object;
    jerry_value_t riot_name;

    jerry_value_t attr_object;
    jerry_value_t attr_name;

    jerry_value_t func_object;
    jerry_value_t func_name;

    /* retrieve the global object  */
    global_object = jerry_get_global_object();

    /* create a riot object */
    riot_object = jerry_create_object();
    riot_name = jerry_create_string((const jerry_char_t *)"riot");

    /* create a saul object */
    attr_object = jerry_create_object();
    attr_name = jerry_create_string((const jerry_char_t *)"foo");

    /* add functions "bar" to foo object */
    func_object = jerry_create_external_function(bar_handler);
    func_name = jerry_create_string((const jerry_char_t *)"bar");
    jerry_release_value( jerry_set_property(attr_object, func_name,
                                            func_object));
    jerry_release_value(func_object);
    jerry_release_value(func_name);

    /* add functions "baz" to foo object */
    func_object = jerry_create_external_function(baz_handler);
    func_name = jerry_create_string((const jerry_char_t *)"baz");
    jerry_release_value( jerry_set_property(attr_object, func_name,
                                            func_object));
    jerry_release_value(func_object);
    jerry_release_value(func_name);

    /* add foo object to riot object */
    jerry_release_value( jerry_set_property(riot_object, attr_name,
                                            attr_object));
    jerry_release_value(attr_object);
    jerry_release_value(attr_name);

    /* add riot object to global object */
    jerry_release_value( jerry_set_property(global_object, riot_name,
                                            riot_object));
    jerry_release_value(riot_object);
    jerry_release_value(riot_name);
    jerry_release_value(global_object);

    DEBUG("[%d] <- jerryscript:register_natives()\n", thread_getpid());
    return 0;
}

static jerry_value_t bar_handler(
    const jerry_value_t func_value,
    const jerry_value_t this_value,
    const jerry_value_t *args_p,
    const jerry_length_t args_cnt)
{
    (void)func_value;
    (void)this_value;
    (void)args_p;
    (void)args_cnt;

    DEBUG("[%d] -> jerryscript:bar_handler()\n", thread_getpid());

    DEBUG("[%d] <- jerryscript:bar_handler()\n", thread_getpid());
    return jerry_create_undefined();
}

static jerry_value_t baz_handler(
    const jerry_value_t func_value,
    const jerry_value_t this_value,
    const jerry_value_t *args_p,
    const jerry_length_t args_cnt)
{
    (void)func_value;
    (void)this_value;
    (void)args_p;
    (void)args_cnt;

    DEBUG("[%d] -> jerryscript:baz_handler()\n", thread_getpid());

    jerry_value_t o = jerry_create_object();
    jerry_value_t attr_name;
    jerry_value_t attr_value;

    attr_name = jerry_create_string((const jerry_char_t *)"name");
    attr_value = jerry_create_string((const jerry_char_t *)"baz");
    jerry_release_value(jerry_set_property(o, attr_name, attr_value));
    jerry_release_value(attr_name);
    jerry_release_value(attr_value);

    DEBUG("[%d] <- jerryscript:baz_handler()\n", thread_getpid());
    return o;
}
