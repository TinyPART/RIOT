/**
 *
 * Copyright (C) 2024, Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_tinycontainer
 *
 * @{
 *
 * @file
 * @brief   API of Tinycontainer Service sub-module for Runtime sub-module
 *
 * The callee security context for a container runtime is a shared memory as
 * defined here. It consists of two parts: one is read only, and the other is
 * read write.
 *
 * Currently, the read only part only contains the period in millisecond between
 * two consecutive calls to the container loop function.
 *
 * The read write part is used to performs natives syscalls.
 *
 * Whenever a container runtime performs a natives function call, the runtime
 * shall fill the io structure of the read write part, and then invoke the
 * service_syscall() function.
 *
 * @author  Samuel Legouix <samuel.legouix@orange.com>
 *
 */

#ifndef TINYCONTAINER_SERVICE_SERVICE_RUNTIME_H
#define TINYCONTAINER_SERVICE_SERVICE_RUNTIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Size of the io buffer of the service_io_t structure */
#ifdef TINYCONTAINER_IO_BUFFER_SIZE
#define IO_BUFFER_SIZE TINYCONTAINER_IO_BUFFER_SIZE
#else
#define IO_BUFFER_SIZE 256
#endif /* TINYCONTAINER_IO_BUFFER_SIZE */

/** syscall identifier to heartbeat */
#define SERVICE_SYSCALL_HEARTBEAT   1
/** syscall identifier to open an endpoint */
#define SERVICE_SYSCALL_OPEN        2
/** syscall identifier to close an endpoint */
#define SERVICE_SYSCALL_CLOSE       3
/** syscall identifier to read from and endpoint */
#define SERVICE_SYSCALL_READ        4
/** syscall identifier to write to and endpoint */
#define SERVICE_SYSCALL_WRITE       5

/**
 * @brief perform a system call from a container runtime
 */
int32_t service_syscall(void);

/**
 * @brief input output structure to communicate between service and container
          runtime
 */
typedef struct {
    uint32_t syscall_id;                /**< the syscall operation to perform */
    int8_t fd;                          /**< the peer endpoint id or the file
                                             descriptor for io                */
    uint8_t size;                       /**< the size of the buffer to read,
                                             write                            */
    uint8_t buffer[IO_BUFFER_SIZE];     /**< the runtime memory for io
                                             operations                       */
} service_io_t;

/**
 * @brief shared memory
 */
typedef struct {
    struct {
        uint32_t period;        /**< period between two loop() call in ms  */
    } ro;                       /**< read only part of the shared memory   */
    union {
        uint32_t syscall_id;    /**< a shortcut to io.syscall_id           */
        service_io_t io;        /**< the structure for io operation        */
    } rw;                       /**< read write part of the shared memory  */
} service_shared_mem_t;

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_SERVICE_SERVICE_RUNTIME_H */
/** @} */
