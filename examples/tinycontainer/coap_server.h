/*
 * Copyright (C) 2023-2024 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory of the TinyContainer module for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       TinyContainer example - coap server
 *
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#ifndef COAP_SERVER_H
#define COAP_SERVER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void coap_server_init(void);
void coap_server_start(void);
void coap_server_stop(void);
bool coap_server_isrunning(void);

#ifdef __cplusplus
}
#endif

#endif /* COAP_SERVER_H */
