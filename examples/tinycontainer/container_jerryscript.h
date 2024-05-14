/*
 * Copyright (C) 2023-2024 Orange
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

uint8_t meta[] =
{ 0xdb, 0x54, 0x69, 0x6e, 0x79, 0x50, 0x41, 0x52, 0x54, 0xa1, 0x01, 0x56, 0xa2, 0x01, 0x51, 0x73,
  0x6f, 0x6d, 0x65, 0x2d, 0x63, 0x6f, 0x6e, 0x74, 0x61, 0x69, 0x6e, 0x65, 0x72, 0x2d, 0x69, 0x64,
  0x02, 0x04
};
int meta_size = sizeof(meta);

uint8_t data[] = "not use yet";
int data_size = sizeof(data);

uint8_t code[] = "printf(\"hello \" + data.n + \"\\n\");";
int code_size = sizeof(code);

#ifdef __cplusplus
}
#endif

#endif /* CONTAINER_JERRYSCRIPT_H */
