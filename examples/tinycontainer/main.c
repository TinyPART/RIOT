/*
 * Copyright (C) 2023 Orange
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

#if defined(MODULE_TINYCONTAINER_CONTAINER_WAMR)
#include "container_wamr.h"
#elif defined(MODULE_TINYCONTAINER_CONTAINER_JERRYSCRIPT)
#include "container_jerryscript.h"
#elif defined(MODULE_TINYCONTAINER_CONTAINER_RBPF)
#include "blob/rbpf/example_app.bin.h"
static const uint8_t *meta = NULL;
static const int meta_size = 0;
static const uint8_t *data = NULL;
static const int data_size = 0;
static const uint8_t *code = example_app_bin;
static const int code_size = example_app_bin_len;
#else
#error "WARM or JERRYSCRIPT or RBPF module is required"
#endif

#define SERVICE_PRIO     10
#define CONTROLLER_PRIO  11
#define CONTAINERS_PRIO  12

static int cmd_load(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (tinycontainer_loadcontainer((uint8_t*)meta, meta_size, (uint8_t*)data, data_size, (uint8_t*)code, code_size) == true)
    {
        (void) puts("container loaded");
    } else
    {
        (void) puts("fails to load the container");
    }

    return 0;
}

static int cmd_unload(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    (void) puts("Not yet implemented!");

    return 0;
}

static int cmd_start(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (tinycontainer_startcontainer((uint8_t*)"") == true)
    {
        (void) puts("container started");
    } else
    {
        (void) puts("fails to start the container");
    }

    return 0;
}

static int cmd_stop(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (tinycontainer_stopcontainer((uint8_t*)"") ==  true)
    {
        (void) puts("container stopped");
    } else
    {
        (void) puts("fails to stop the container");
    }

    return 0;
}

static int cmd_status(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (tinycontainer_iscontainerrunning((uint8_t*)"") ==  true)
    {
        (void) puts("container is started");
    } else
    {
        (void) puts("container is stopped");
    }

    return 0;
}

static int cmd_wait(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    int n;
    if (argc == 1)
    {
        n = 10;
    } else if (argc != 2)
    {
        (void) puts("please enter a valid number of second!");

        return 0;
    } else
    {
        n = atoi(argv[1]);
    }
    printf("let the container running a while. Please wait %ds.\n", n);
    xtimer_sleep(n);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "load",   "load container",                cmd_load },
    { "unload", "unload container",              cmd_unload},
    { "start",  "start the container",           cmd_start},
    { "stop",   "stop the container",            cmd_stop},
    { "status", "status of the container",       cmd_status},
    { "wait",   "let the container run a while", cmd_wait},
    { NULL, NULL, NULL }
};

int main(void)
{
    /* Start TinyContainer */

    tinycontainer_init(CONTROLLER_PRIO, SERVICE_PRIO, CONTAINERS_PRIO);

    /* provides a shell */

    (void) puts("Welcome to TinyContainer!");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
