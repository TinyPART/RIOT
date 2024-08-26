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
 * @brief   API of TinyContainer Security sub-module.
 *
 * A secure thread adds some security context to a normal thread.
 *
 * A security context is composed of
 *     - a callee context
 *     - a caller context
 *     - a callback function
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

#ifndef TINYCONTAINER_SECURITY_SECURITY_ALL_H
#define TINYCONTAINER_SECURITY_SECURITY_ALL_H

#include <stdint.h>

/*include the IPC API */
#include "msg.h"
/* Include threads header*/
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief last function call before the thread ends
 *
 */
typedef void (*task_callback_t)(kernel_pid_t pid, void *caller_context);

/**
 * @brief create a new thread using context
 * @return kernel_pid_t (pid of this thread)
 */

kernel_pid_t secure_thread( void *caller_context, void *callee_context,
                            task_callback_t callback,
                            char *stack, int stacksize,
                            uint8_t priority, int flags,
                            thread_task_func_t task_func,
                            const char *name);

/**
 * @brief send msg_req and wait for mg_resp
 */
void send_synchrone( msg_t msg_req, msg_t msg_resp, kernel_pid_t rcv_pid);

/**
 * @brief waits for a msg and once received send msg_resp to the sender
 */
msg_t rcv_synchrone(void);

/**
 * @brief send msg and continue execution
 *
 * @return int (1:successful, 0:full message queue or  receiver is not waiting, -1:Error PID)
 */
int send_asynchrone(msg_t msg_req, kernel_pid_t rcv_pid);

/**
 * @brief waits for a message without queue
 */
msg_t rcv_asynchrone(void);

/**
 * @brief waits for a message with queue
 *
 */
msg_t rcv_asynchrone_queue(void);

/**
 * @brief return the callee context (from secure context) for the current thread
 */
void *security_getcalleecontext(void);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_SECURITY_SECURITY_ALL_H */
/** @} */
