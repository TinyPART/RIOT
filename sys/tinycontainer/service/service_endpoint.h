/*
 * Copyright (C) 2024, Orange.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_tinycontainer
 * @{
 *
 * @file
 * @brief       TinyContainer Service sub-module definitions for endpoint
 *
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#ifndef SERVICE_ENDPOINT_H
#define SERVICE_ENDPOINT_H

#ifdef __cplusplus
extern "C" {
#endif

uint32_t endpoint_open(container_t *container);
uint32_t endpoint_close(container_t *container);
uint32_t endpoint_read(container_t *container);
uint32_t endpoint_write(container_t *container);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_ENDPOINT_H */
