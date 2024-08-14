/**
 *
 * Copyright (C) 2024, Orange.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    sys_tinycontainer
 * @brief      Native functions exposed to container
 *
 * Native functions can be exposed by the container runtime to containers to
 * allow communications with external components, or to get some useful
 * information from the board.
 *
 * How these functions are mapped to the container depends on the container
 * runtime.
 *
 * Supported functions
 * ===================
 *
 *  - I/O functions
 *    - open
 *    - read
 *    - write
 *    - close
 *  - heartbeat
 *
 * Security feature
 * ================
 *
 * The use of native functions is enforced at three levels in TinyContainer :
 *
 * Firstly, the container runtime should expose only functions according to
 * container metadata. Limiting the list of addressable functions from a
 * container reduce the attack surface and is considered as a good security
 * practice. TinyContainer metadata provides a decicated syscall mask to control
 * which natives are allowed to each containers.
 *
 * Secondly, every call to a native function from a container implies some
 * dynamically check some access control rules. Indeed, a call to one of these
 * functions begins with a context switching between the container and the
 * service context. The Service module controls whether the call to
 * the function is allowed, and if so, it finally execute the function on behalf
 * of the container.
 *
 * Thirdly, each communication endpoint are managed by TinyContainer, and
 * a container has no direct information of its real peer. The mapping between
 * the endpoint id viewed by the container and the endpoint peer is defined the
 * container metadata and managed by TinyContainer.
 *
 * @{
 *
 * @file
 * @brief   natives functions definitions
 *
 * @author  Samuel Legouix <samuel.legouix@orange.com>
 *
 */

#ifndef TINYCONTAINER_CONTAINER_CONTAINER_NATIVES_H
#define TINYCONTAINER_CONTAINER_CONTAINER_NATIVES_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   log a message to the console
 *
 * @param[in]   msg        a null terminate string to log
 *
 */
void native_log(char *msg);

/**
 * @brief   open a file to exchange with a peer
 *
 * @param[in]   endpoint_id   endpoint identifier
 *
 * @return int32_t (A nonnegative value on success which is the new file descriptor, or -1 on error)
 */
int32_t native_open(uint32_t endpoint_id);

/**
 * @brief   read data from a previously opened file
 *
 * The function is blocking.
 *
 * @param[in]   fd         the file descriptor
 * @param[out]  buf        the buffer to copy data
 * @param[in]   size       the size of buffer
 *
 * @return int32_t (A nonnegative value on success which is the number of bytes
                    read, or -1 on error)
 */
int32_t native_read(int32_t fd, uint8_t *buf, uint32_t size);

/**
 * @brief   write data from a previously opened file
 *
 * The function is blocking.
 *
 * @param[in]   fd         the file descriptor
 * @param[in]   buf        the buffer to copy data
 * @param[in]   size       the number of bytes to write
 *
 * @return int32_t (A nonnegative value on success which is the number of bytes
                    written, or -1 on error)
 */
int32_t native_write(int32_t fd, uint8_t *buf, uint32_t size);

/**
 * @brief   close a file
 *
 * @param[in]   fd         the file descriptor
 */
int32_t native_close(int32_t fd);

/**
 * @brief   Periodic signal to indicate the normal operation of the container
 *
 * note: heartbeating to TinyContainer core may be implemented by the container
 *       runtime.
 *
 */
//TODO: not yet implemented
int32_t native_heartbeat(void);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_CONTAINER_CONTAINER_NATIVES_H */
/** @} */
