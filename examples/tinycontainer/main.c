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

#include "tinycontainer.h"

#include "shell.h"
#include "xtimer.h"
#include "kernel_defines.h"

#if IS_USED(MODULE_GCOAP)
#include "coap_server.h"
#endif

#include "io_driver.h"

#if defined(MODULE_TINYCONTAINER_CONTAINER_WAMR)
#include "container_wamr.h"
#elif defined(MODULE_TINYCONTAINER_CONTAINER_JERRYSCRIPT)
#include "container_jerryscript.h"
#elif defined(MODULE_TINYCONTAINER_CONTAINER_RBPF)
#include "blob/rbpf/example_app.bin.h"
static uint8_t meta[] =
{ 0xdb, 0x54, 0x69, 0x6e, 0x79, 0x50, 0x41, 0x52, 0x54, 0xa1, 0x01, 0x58, 0x1d, 0xa3, 0x01, 0x47,
  0x69, 0x78, 0xc3, 0xa2, 0x6d, 0x65, 0x73, 0x02, 0x06, 0x03, 0x4f, 0xd8, 0x3d, 0xd1, 0x4b, 0x66,
  0x61, 0x72, 0x63, 0x69, 0x73, 0x73, 0x69, 0x6f, 0x6e, 0x73 };
static const int meta_size = 0;
static const uint8_t *data = NULL;
static const int data_size = 0;
#define code example_app_bin
#define code_size example_app_bin_len
#else
#error "WAMR or JERRYSCRIPT or RBPF module is required"
#endif

#define SERVICE_PRIO     10
#define CONTROLLER_PRIO  11
#define CONTAINERS_PRIO  12

/* Following constants shall be set according with container metadata */
#define CONTAINER_UID_SIZE 17
#define CONTAINER_UID_PREFIX "container--id--"
#define CONTAINER_UID_POS 16

static uint8_t next_uid = 1;
static uint8_t container_id[CONTAINER_UID_SIZE + 1];

static int cmd_load(int argc, char **argv)
{
    memset(container_id, 0, CONTAINER_UID_SIZE + 1);

    if (argc == 1) {
        sprintf((char *)container_id, "%s%02d", CONTAINER_UID_PREFIX, next_uid % 100);
        next_uid++;
    }
    else if (argc != 2) {
        (void)puts("usage: load <container-id>");

        return 0;
    }
    else {
        if (strlen(argv[1]) > CONTAINER_UID_SIZE) {
            (void)puts("<container-id> is too long");

            return 0;

        }
        else {
            strcpy((char *)container_id, argv[1]);
        }
    }

    memcpy(meta + CONTAINER_UID_POS, container_id, CONTAINER_UID_SIZE);

    if (tinycontainer_loadcontainer((uint8_t *)meta, meta_size, (uint8_t *)data, data_size,
                                    (uint8_t *)code, code_size) == true) {
        printf("Container '%s' loaded\n", container_id);
    }
    else {
        printf("Fails to load container '%s'!\n", container_id);
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
    memset(container_id, 0, CONTAINER_UID_SIZE + 1);

    if (argc != 2) {
        (void)puts("usage: start <container-id>");

        return 0;
    }
    else {
        if (strlen(argv[1]) > CONTAINER_UID_SIZE) {
            (void)puts("<container-id> is too long");

            return 0;

        }
        else {
            strcpy((char *)container_id, argv[1]);
        }
    }

    if (tinycontainer_startcontainer(container_id, CONTAINER_UID_SIZE) == true) {
        printf("Container '%s' started\n", container_id);
    }
    else {
        printf("Fails to start container '%s'!\n", container_id);
    }

    return 0;
}

static int cmd_stop(int argc, char **argv)
{
    memset(container_id, 0, CONTAINER_UID_SIZE + 1);

    if (argc != 2) {
        (void)puts("usage: stop <container-id>");

        return 0;
    }
    else {
        if (strlen(argv[1]) > CONTAINER_UID_SIZE) {
            (void)puts("<container-id> is too long");

            return 0;

        }
        else {
            strcpy((char *)container_id, argv[1]);
        }
    }

    if (tinycontainer_stopcontainer(container_id, CONTAINER_UID_SIZE) ==  true) {
        printf("Container '%s' stopped\n", container_id);
    }
    else {
        printf("Fails to stop container '%s'!\n", container_id);
    }

    return 0;
}

static int cmd_status(int argc, char **argv)
{
    memset(container_id, 0, CONTAINER_UID_SIZE + 1);

    if (argc != 2) {
        (void)puts("usage: status <container-id>");

        return 0;
    }
    else {
        if (strlen(argv[1]) > CONTAINER_UID_SIZE) {
            (void)puts("<container-id> is too long");

            return 0;

        }
        else {
            strcpy((char *)container_id, argv[1]);
        }
    }

    if (tinycontainer_iscontainerrunning(container_id, CONTAINER_UID_SIZE) ==  true) {
        (void)puts("container is started");
    }
    else {
        (void)puts("container is stopped");
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
    { "load",   "load container",                cmd_load },
    { "unload", "unload container",              cmd_unload },
    { "start",  "start the container",           cmd_start },
    { "stop",   "stop the container",            cmd_stop },
    { "status", "status of the container",       cmd_status },
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
