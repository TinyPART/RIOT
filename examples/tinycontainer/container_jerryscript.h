/*
 * Copyright (C) 2023 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory of TinyContainer module for more details.
 */

#ifndef CONTAINER_JERRYSCRIPT_H
#define CONTAINER_JERRYSCRIPT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t meta[] = "not use yet";
int meta_size = 11 /*strlen(meta)*/;

uint8_t data[] = "{ \"n\": 100 }";
int data_size = 12 /*stlen(data)*/;

uint8_t code[] = "printf(\"hello \" + data.n + \"\\n\");";
int code_size = 33 /*strlen(code)*/;

#ifdef __cplusplus
}
#endif

#endif /* CONTAINER_JERRYSCRIPT_H */
