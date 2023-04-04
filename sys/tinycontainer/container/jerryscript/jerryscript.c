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
 * @brief       TinyContainer JerryScript runtime implementation
 *
 * @author      Samuel legouix <samuel.legouix@orange.com>
 *
 * @}
 */


#include "debug.h"
#include "thread.h"
#include "xtimer.h"

#include "jerryscript.h"

#include "tinycontainer/container/container_runtime.h"
#include "tinycontainer/memmgr/memmgr_container.h"
#include "jerryscript_natives.h"

#define MAX_CODE_SIZE 64

struct handler {
    bool is_used;
    jerry_value_t looper;
    bool is_finished;
};

#define MAX_HANDLERS 3
struct handler handlers[MAX_HANDLERS];
int handlers_in_use;

bool container_impl_init(void)
{
    DEBUG("[%d] -> container:init()\n", thread_getpid());

    /* nothing to do yet */

    DEBUG("[%d] -- container initialized\n", thread_getpid());

    DEBUG("[%d] -> container:init()\n", thread_getpid());

    return true;
}

container_handle_t container_create(memmgr_block_t * data, memmgr_block_t * code)
{
    DEBUG("[%d] -> container:create()\n", thread_getpid());

    /* new handler */
    struct handler *new_handler = NULL;

    for (int i = 0; i < MAX_HANDLERS; i++) {
        if (handlers[i].is_used == false) {
            new_handler = &handlers[i];
            handlers_in_use++;
        }
    }
    if (new_handler == NULL) {

        DEBUG("[%d] EE unable to get container handler\n", thread_getpid());

        return NULL;
    }

    new_handler->is_used = true;
    new_handler->is_finished = false;

    /* initialize the container */
    if (handlers_in_use == 1) {
        jerry_init(JERRY_INIT_EMPTY);
    }

    /* parse and load data */

    jerry_value_t data_json;

    if (data -> size == 0) {

        DEBUG("[%d] WW container data is empty\n", thread_getpid());

        data_json = jerry_create_object();
    }
    else {
        data_json = jerry_json_parse((jerry_char_t *)data -> ptr, data -> size);
    }

    if (jerry_value_is_error(data_json)) {

        DEBUG("[%d] EE can't parse container data\n", thread_getpid());

        jerry_release_value(data_json);

        new_handler->is_used = false;
        handlers_in_use--;
        if (handlers_in_use == 0) {
            jerry_cleanup();
        }

        return NULL;
    }

    jerry_value_t global_object = jerry_get_global_object();
    jerry_value_t data_name = jerry_create_string((const jerry_char_t *)"data");
    jerry_value_t set_result = jerry_set_property(global_object, data_name,
                                                  data_json);

    if (jerry_value_is_error(global_object) ||
        jerry_value_is_error(data_name) ||
        jerry_value_is_error(set_result)) {

        DEBUG("[%d] EE can't load container data\n", thread_getpid());

        jerry_release_value(data_json);
        jerry_release_value(global_object);
        jerry_release_value(data_name);
        jerry_release_value(set_result);

        new_handler->is_used = false;
        handlers_in_use--;
        if (handlers_in_use == 0) {
            jerry_cleanup();
        }

        return NULL;
    }

    jerry_release_value(data_json);
    jerry_release_value(global_object);
    jerry_release_value(data_name);
    jerry_release_value(set_result);

    /* parse and load low level api */

    if (register_natives() != 0) {
        DEBUG("[%d] EE container can't intialize low level API\n",
              thread_getpid());

        new_handler->is_used = false;
        handlers_in_use--;
        if (handlers_in_use == 0) {
            jerry_cleanup();
        }

        return NULL;
    }

    /* parse code */

    new_handler->looper = jerry_parse_function(NULL, 0, NULL, 0,
                                               (jerry_char_t *)code -> ptr,
                                               code -> size,
                                               JERRY_PARSE_NO_OPTS);

    if (jerry_value_is_error(new_handler->looper)) {

        DEBUG("[%d] EE container can't parse code function [%d]\n",
              thread_getpid(), jerry_get_error_type(new_handler->looper));

        jerry_value_t value = jerry_get_value_from_error(new_handler->looper, false);
        if (jerry_value_is_string(value)) {
            jerry_char_t buf[256];
            jerry_size_t size = jerry_get_string_size(value);
            if (size < 255) {
                jerry_string_to_char_buffer(value, buf, size);
                buf[255] = '\0';
                DEBUG("[%d] EE error value is '%s'", thread_getpid(), buf);
            }
        }
        else {
            DEBUG("[%d] EE error value is '%d'",
                  thread_getpid(), jerry_value_get_type(value));
        }
        jerry_release_value(value);

        jerry_release_value(new_handler->looper);

        new_handler->is_used = false;
        handlers_in_use--;
        if (handlers_in_use == 0) {
            jerry_cleanup();
        }

        return NULL;
    }

    DEBUG("[%d] <- container:create()\n", thread_getpid());

    return new_handler;
}

void container_on_start(container_handle_t handler)
{
    (void)handler;

    DEBUG("[%d] -> container:onstart()\n", thread_getpid());

    DEBUG("[%d] WW onstart() is not yet implemented\n", thread_getpid());

    DEBUG("[%d] <- container:onstart()\n", thread_getpid());
}

int container_on_loop(container_handle_t handler)
{
    DEBUG("[%d] -> container:onloop()\n", thread_getpid());

    int32_t return_value = -1;

    struct handler *my_handler = (struct handler *)handler;

    /* run container */
    jerry_value_t ret_val =
        jerry_call_function(my_handler->looper, jerry_create_undefined(), NULL, 0);

    if (jerry_value_is_error(ret_val)) {

        DEBUG("[%d] EE container execution error (%d)\n",
              thread_getpid(), (int)jerry_get_error_type(ret_val));

        my_handler->is_finished = true;
        return_value = (int)jerry_get_error_type(ret_val);
    }
    else if (!jerry_value_is_boolean(ret_val)) {

        DEBUG("[%d] EE container doesn't return a boolean value (%d)\n",
              thread_getpid(), jerry_value_get_type(ret_val));

        my_handler->is_finished = true;
    }
    else if (!jerry_get_boolean_value(ret_val)) {

        /* stop looping */

        DEBUG("[%d]    going to stop\n", thread_getpid());

        my_handler->is_finished = true;
        return_value = 0;
    }
    else {
        return_value = 1;
    }

    jerry_release_value(ret_val);

    DEBUG("[%d] <- container:onloop()\n", thread_getpid());
    return return_value;
}

bool container_has_finished(container_handle_t handler)
{
    DEBUG("[%d] -> container:hasfinished()\n", thread_getpid());

    struct handler *my_handler = (struct handler *)handler;

    DEBUG("[%d] <- container:hasfinished()\n", thread_getpid());

    return my_handler->is_finished;
}

void container_on_stop(container_handle_t handler)
{
    (void)handler;

    DEBUG("[%d] -> container:onstop()\n", thread_getpid());

    DEBUG("[%d] WW onstop() is not yet implemented\n", thread_getpid());

    DEBUG("[%d] <- container:onstop()\n", thread_getpid());
}

void container_on_finalize(container_handle_t handler)
{
    DEBUG("[%d] -> container:onfinalize()\n", thread_getpid());

    struct handler *my_handler = (struct handler *)handler;

    jerry_release_value(my_handler->looper);
    my_handler->is_used = false;
    handlers_in_use--;
    if (handlers_in_use == 0) {
        jerry_cleanup();
    }

    DEBUG("[%d] <- container:onfinalize()\n", thread_getpid());
}
