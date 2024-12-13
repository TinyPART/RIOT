/*
 * Copyright (C) 2023-2024 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory of the TinyContainer module for more details.
 *
*/

/**
 * @ingroup     sys_tinycontainer
 * @{
 *
 * @file
 * @brief       TinyContainer example - wamr container example
 *
 * @author      Samuel legouix <samuel.legouix@orange.com>
 *
 *}
 */

#include <stdint.h>

extern void logger(char *);
extern int32_t open(uint32_t);
extern int32_t close(int32_t);
extern int32_t read(int32_t, uint8_t *, uint32_t);
extern int32_t write(int32_t, uint8_t *, uint32_t);
#define WASM_EXPORT __attribute__((visibility("default")))

int fd;
uint8_t buf[10];
int nb = 100;

WASM_EXPORT void start(void)
{
    logger("simple_container: starting\n");
    fd = open(123);
}

WASM_EXPORT int loop(void)
{
    logger("simple_container: looping\n");
    read(fd, buf, 10);
    write(fd, buf, 10);
    return !nb--;
}

WASM_EXPORT void stop(void)
{
    close(fd);
    logger("simple_container: stopping\n");
}

/*EOF*/
