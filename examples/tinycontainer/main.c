/*
 * Copyright (C) 2023-2024 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       TinyContainer example
 *
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#include <stdlib.h>
#include <errno.h>

#include "tinycontainer.h"

#include "shell.h"
#include "xtimer.h"
#include "kernel_defines.h"

#if IS_USED(MODULE_GCOAP)
#include "coap_server.h"
#endif

#include "io_driver.h"

#if !defined(MODULE_TINYCONTAINER_RUNTIME_WAMR) \
 && !defined(MODULE_TINYCONTAINER_RUNTIME_JERRYSCRIPT) \
 && !defined(MODULE_TINYCONTAINER_RUNTIME_RBPF)

    #error "WAMR or JERRYSCRIPT or RBPF module is required"

#endif

#include "containers/containers.h"

#define SERVICE_PRIO     10
#define CONTROLLER_PRIO  11
#define CONTAINERS_PRIO  12

static void list_array(const char** array, size_t array_size) {
    for (unsigned int index=0; index < array_size; index++) {
        printf("  %d. ", index + 1);
        printf("%s\n", array[index]);
    }
}

static int cmd_list(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("List of registered containers:\n");
    list_array(containers_name, sizeof(containers_name)/sizeof(char*));

    return 0;
}

static int search_value(const char * value, const char** array,
                        size_t array_size){
    int index;

    /* searching the value string */
    for (index = 0; index < (int)array_size; index++) {
        int len = strlen(array[index]);
        if(strncmp(array[index], value, len) == 0) {
            return index;
        }
    }

    /* maybe it is the value number that is provided */
    char * endptr;
    index = strtol(value, &endptr, 10);
    index--; /* because using friendly user index in cmd_list() */
    if(errno != ERANGE && errno != EINVAL &&
       index >= 0 && index < (int)array_size) {
        return index;
    }

    return -1;
}

static int cmd_load(int argc, char **argv)
{
    if (argc != 2) {
        (void)puts("usage: load container");

        return 0;
    }

    int index = search_value(argv[1], containers_name,
                             sizeof(containers_name)/sizeof(char*));
    if (index < 0) {
        printf("Value '%s' was not registered!\n", argv[1]);

        return 0;
    }

    /* found the container. Try to load it */
    const uint8_t * meta = containers_meta[index];
    const uint8_t * data = containers_data[index];
    const uint8_t * code = containers_code[index];
    size_t meta_size = containers_meta_size[index];
    size_t data_size = containers_data_size[index];
    size_t code_size = containers_code_size[index];

    if (tinycontainer_loadcontainer((uint8_t *)meta, meta_size,
                                    (uint8_t *)data, data_size,
                                    (uint8_t *)code, code_size) == true) {
        printf("Container '%s' loaded\n", containers_name[index]);
    }
    else {
        printf("Fails to load container '%s'!\n", containers_name[index]);
    }

    return 0;
}

static int cmd_unload(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    (void)puts("Not yet implemented!");

    return 0;
}

static int cmd_start(int argc, char **argv)
{
    if (argc != 2) {
        (void)puts("usage: start container");

        return 0;
    }

    uint8_t * uid;
    size_t uid_len;
    int index = search_value(argv[1], containers_name,
                             sizeof(containers_name)/sizeof(char*));
    if (index < 0) {
        printf("Value '%s' was not registered!\n", argv[1]);
        return 0;
    }

    uid = (uint8_t*)containers_name[index];
    uid_len = strlen(containers_name[index]);

    if (tinycontainer_startcontainer(uid, uid_len) == true) {
        printf("Container '%s' started\n", containers_name[index]);
    }
    else {
        printf("Fails to start container '%s'!\n", containers_name[index]);
    }

    return 0;
}

static int cmd_stop(int argc, char **argv)
{
    if (argc != 2) {
        (void)puts("usage: stop container");

        return 0;
    }

    uint8_t * uid;
    size_t uid_len;
    int index = search_value(argv[1], containers_name,
                             sizeof(containers_name)/sizeof(char*));
    if (index < 0) {
        printf("Value '%s' was not registered!\n", argv[1]);
        return 0;
    }

    uid = (uint8_t*)containers_name[index];
    uid_len = strlen(containers_name[index]);

    if (tinycontainer_stopcontainer(uid, uid_len) ==  true) {
        printf("Container '%s' stopped\n", containers_name[index]);
    }
    else {
        printf("Fails to stop container '%s'!\n", containers_name[index]);
    }

    return 0;
}

static int cmd_status(int argc, char **argv)
{
    if (argc != 2) {
        (void)puts("usage: status <container-id>");

        return 0;
    }

    uint8_t * uid;
    size_t uid_len;
    int index = search_value(argv[1], containers_name,
                             sizeof(containers_name)/sizeof(char*));
    if (index < 0) {
        printf("Value '%s' was not registered!\n", argv[1]);
        return 0;
    }

    uid = (uint8_t*)containers_name[index];
    uid_len = strlen(containers_name[index]);

    if (tinycontainer_iscontainerrunning(uid, uid_len) ==  true) {
        printf("container '%s' is started", containers_name[index]);
    }
    else {
        printf("container '%s' is stopped", containers_name[index]);
    }

    return 0;
}

static int cmd_wait(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    int n;

    if (argc == 1) {
        n = 10;
    }
    else if (argc != 2) {
        (void)puts("please enter a valid number of second!");

        return 0;
    }
    else {
        n = atoi(argv[1]);
    }
    printf("let the container running a while. Please wait %ds.\n", n);
    xtimer_sleep(n);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "list",   "list registered",               cmd_list },
    { "load",   "load container",                cmd_load },
    { "unload", "unload container",              cmd_unload },
    { "start",  "start a loaded container",      cmd_start },
    { "stop",   "stop a stated container",       cmd_stop },
    { "status", "status of a container",         cmd_status },
    { "wait",   "let the container run a while", cmd_wait },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* Start TinyContainer */

    tinycontainer_init(CONTROLLER_PRIO, SERVICE_PRIO, CONTAINERS_PRIO,
                       &io_driver);

#if IS_USED(MODULE_GCOAP)
    /* setup coap server */
    coap_server_init();
#endif

    /* provides a shell */

    (void)puts("Welcome to TinyContainer!");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
