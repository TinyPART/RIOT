/**
 *
 * Copyright (C) 2020-2024, Orange.
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
 * @brief   API of TinyContainer Secure Thread sub-module.
 *
 * A secure thread adds some security context to a normal thread.
 *
 * A security context is composed of
 *     - a callee context
 *     - a caller context
 *     - a caller callback function
 *
 * The callee context can be retrieved by the secure thread as the first
 * argument of task_func() or by calling the security_getcalleecontex() function
 * The meaning of the callee context is implementation dependent and, if not
 * relevant, can be set to NULL.
 *
 * When the task_func returned (i.e. at end of the secure thread), the callback
 * function is called with the caller context as first argument. The meaning of
 * the caller context is implementation dependent and, if not relevant, can be
 * set to NULL. If the callback isn't needed, it can also be set to NULL.
 *
 * @author  BERKANE Ghilas (ghilas.berkane@gmail.com)
 * @author  Samuel Legouix <samuel.leoguix@orange.com>
 *
 */

#ifndef TINYCONTAINER_SECURITY_STHREAD_STHREAD_H
#define TINYCONTAINER_SECURITY_STHREAD_STHREAD_H

#include <stdint.h>

#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief last function call before the thread ends
 *
 */
typedef void (*sthread_callback_t)(kernel_pid_t pid, void *caller_context);

/**
 * @brief create a new thread with a secure context
 * @return kernel_pid_t (pid of this secure thread)
 */

kernel_pid_t sthread( void *caller_context, void *callee_context,
                      sthread_callback_t caller_callback,
                      char *stack, int stacksize,
                      uint8_t priority, int flags,
                      thread_task_func_t task_func,
                      const char *name);

/**
 * @brief return the callee context (from secure context) for the current thread
 */
void *sthread_getcalleecontext(void);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_SECURITY_STHREAD_STHREAD_H*/
/** @} */
