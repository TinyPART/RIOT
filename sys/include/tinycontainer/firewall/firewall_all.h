/**
 *
 * Copyright (C) 2020-2023, Orange.
 *
 * Please, refer to the README.md and LICENSE files of TinyContainer
 *
 */

/**
 * @ingroup sys_tinycontainer
 *
 * @{
 *
 * @file
 * @brief   API of TinyContainer Firewall sub-module.
 *
 * @author  BERKANE Ghilas (ghilas.berkane@gmail.com)
 *
 */


#ifndef TINYCONTAINER_FIREWALL_ALL_H
#define TINYCONTAINER_FIREWALL_ALL_H

#include <stdint.h>

/*include the IPC API */
#include "msg.h"
/* Include threads header*/
#include "thread.h"

/**
 * @brief create a new thread using context
 * @return kernel_pid_t (pid of this thread)
 */

kernel_pid_t secure_thread( char *context, char *stack, int stacksize,
                            uint8_t priority, int flags,
                            thread_task_func_t task_func, void *arg,
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
 * @brief waits for a message whithout queue
 */
msg_t rcv_asynchrone(void);

/**
 * @brief waits for a message whith queue
 *
 */
msg_t rcv_asynchrone_queue(void);

/**
 * @brief return the process ID of the currently running thread.
 *
 */
kernel_pid_t firewall_getpid(void);

#endif /* TINYCONTAINER_FIREWALL_ALL_H */
/** @} */
