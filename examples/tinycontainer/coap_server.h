/*
 * Copyright (C) 2023 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory of the TinyContainer module for more details.
 */

#ifndef COAP_SERVER_H
#define COAP_SERVER_H

#include <stdbool.h>

void coap_server_init(void);
void coap_server_start(void);
void coap_server_stop(void);
bool coap_server_isrunning(void);

#endif /* COAP_SERVER_H */
