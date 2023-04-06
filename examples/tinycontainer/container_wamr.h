/*
 * Copyright (C) 2023 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory of the TinyContainer module for more details.
 */

#ifndef CONTAINER_WAMR_H
#define CONTAINER_WAMR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t meta[] = "not use yet";
int meta_size = 11 /*strlen(meta)*/;

uint8_t data[] = "not use yet";
int data_size = 11 /*stlen(data)*/;

uint8_t code[] = {
  0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x13, 0x04, 0x60,
  0x02, 0x7f, 0x7f, 0x01, 0x7f, 0x60, 0x01, 0x7f, 0x01, 0x7f, 0x60, 0x00,
  0x00, 0x60, 0x00, 0x01, 0x7f, 0x02, 0x19, 0x02, 0x03, 0x65, 0x6e, 0x76,
  0x06, 0x70, 0x72, 0x69, 0x6e, 0x74, 0x66, 0x00, 0x00, 0x03, 0x65, 0x6e,
  0x76, 0x04, 0x70, 0x75, 0x74, 0x73, 0x00, 0x01, 0x03, 0x04, 0x03, 0x02,
  0x03, 0x02, 0x05, 0x03, 0x01, 0x00, 0x01, 0x06, 0x13, 0x03, 0x7f, 0x01,
  0x41, 0xc0, 0x28, 0x0b, 0x7f, 0x00, 0x41, 0xb8, 0x08, 0x0b, 0x7f, 0x00,
  0x41, 0xc0, 0x28, 0x0b, 0x07, 0x3b, 0x06, 0x06, 0x6d, 0x65, 0x6d, 0x6f,
  0x72, 0x79, 0x02, 0x00, 0x05, 0x73, 0x74, 0x61, 0x72, 0x74, 0x00, 0x02,
  0x04, 0x6c, 0x6f, 0x6f, 0x70, 0x00, 0x03, 0x04, 0x73, 0x74, 0x6f, 0x70,
  0x00, 0x04, 0x0a, 0x5f, 0x5f, 0x64, 0x61, 0x74, 0x61, 0x5f, 0x65, 0x6e,
  0x64, 0x03, 0x01, 0x0b, 0x5f, 0x5f, 0x68, 0x65, 0x61, 0x70, 0x5f, 0x62,
  0x61, 0x73, 0x65, 0x03, 0x02, 0x0a, 0x80, 0x01, 0x03, 0x0f, 0x00, 0x41,
  0x80, 0x88, 0x80, 0x80, 0x00, 0x10, 0x81, 0x80, 0x80, 0x80, 0x00, 0x1a,
  0x0b, 0x5e, 0x01, 0x02, 0x7f, 0x23, 0x80, 0x80, 0x80, 0x80, 0x00, 0x41,
  0x10, 0x6b, 0x22, 0x00, 0x24, 0x80, 0x80, 0x80, 0x80, 0x00, 0x41, 0x00,
  0x41, 0x00, 0x28, 0x02, 0xb4, 0x88, 0x80, 0x80, 0x00, 0x41, 0x01, 0x6a,
  0x22, 0x01, 0x36, 0x02, 0xb4, 0x88, 0x80, 0x80, 0x00, 0x20, 0x00, 0x20,
  0x01, 0x36, 0x02, 0x00, 0x41, 0x9b, 0x88, 0x80, 0x80, 0x00, 0x20, 0x00,
  0x10, 0x80, 0x80, 0x80, 0x80, 0x00, 0x1a, 0x41, 0x00, 0x28, 0x02, 0xb4,
  0x88, 0x80, 0x80, 0x00, 0x21, 0x01, 0x20, 0x00, 0x41, 0x10, 0x6a, 0x24,
  0x80, 0x80, 0x80, 0x80, 0x00, 0x20, 0x01, 0x41, 0xe4, 0x00, 0x46, 0x0b,
  0x0f, 0x00, 0x41, 0x8e, 0x88, 0x80, 0x80, 0x00, 0x10, 0x81, 0x80, 0x80,
  0x80, 0x00, 0x1a, 0x0b, 0x0b, 0x3a, 0x01, 0x00, 0x41, 0x80, 0x08, 0x0b,
  0x33, 0x57, 0x41, 0x53, 0x4d, 0x3a, 0x20, 0x73, 0x74, 0x61, 0x72, 0x74,
  0x28, 0x29, 0x00, 0x57, 0x41, 0x53, 0x4d, 0x3a, 0x20, 0x73, 0x74, 0x6f,
  0x70, 0x28, 0x29, 0x00, 0x57, 0x41, 0x53, 0x4d, 0x3a, 0x20, 0x6c, 0x6f,
  0x6f, 0x70, 0x20, 0x6e, 0x3d, 0x25, 0x64, 0x20, 0x6f, 0x66, 0x20, 0x31,
  0x30, 0x30, 0x0a, 0x00
};
int code_size = 340 /*strlen(code)*/;

#ifdef __cplusplus
}
#endif

#endif /* CONTAINER_WAMR_H */
