/*
 * Copyright (C) 2023-2024 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" void logger(char *);
extern "C" int32_t open(uint32_t);
extern "C" void close(int32_t);
extern "C" int32_t read(int32_t, uint8_t *, uint32_t);
extern "C" int32_t write(int32_t, uint8_t *, uint32_t);
#define WASM_EXPORT __attribute__((visibility("default"))) extern "C"
#else
extern void logger(char *);
extern int32_t open(uint32_t);
extern int32_t close(int32_t);
extern int32_t read(int32_t, uint8_t *, uint32_t);
extern int32_t write(int32_t, uint8_t *, uint32_t);
#define WASM_EXPORT __attribute__((visibility("default")))
#endif

int fd;
uint8_t buf[10];
int nb = 100;

WASM_EXPORT void start(void)
{
    logger("WAMR: starting\n");
    fd = open(123);
}

WASM_EXPORT int loop(void)
{
    logger("WAMR: looping\n");
    read(fd, buf, 10);
    write(fd, buf, 10);
    return !nb--;
}

WASM_EXPORT void stop(void)
{
    close(fd);
    logger("WAMR: stopping\n");
}
