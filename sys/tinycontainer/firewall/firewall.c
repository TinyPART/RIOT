/*
 * Copyright (C) 2020-2023 Orange
 *
 * Please, refer to the README.md and LICENSE files of TinyContainer
 *
 */

/**
 * @ingroup     sys_tinycontainer
 * @{
 *
 * @file
 * @brief       TinyContainer Firewall sub-module implementation
 *
 * @author      BERKANE Ghilas (ghilas.berkane@gmail.com)
 *
 * @}
 */

#define DEBUG_ENABLE
#include "debug.h"

#include "tinycontainer/firewall/firewall.h"

/* size que msg */
#define RCV_QUEUE_SIZE  (8)

/**
 * @brief secure_thread lets create a thread returns its pid
 *
 * @param context
 * @param stack stack array pointer
 * @param stacksize stack size
 * @param priority thread priority
 * @param flags thread configuration flag, usually
 * @param task_func thread handler function
 * @param arg argument of thread_handler function
 * @param name thread name
 * @return kernel_pid_t
 */
kernel_pid_t secure_thread( char *context, char *stack, int stacksize,
                            uint8_t priority, int flags,
                            thread_task_func_t task_func, void *arg,
                            const char *name)
{
    DEBUG("[%d] -> firewall:secure_thread()\n", thread_getpid());

    (void)context;
    kernel_pid_t pid;

    pid = thread_create(   stack,
                           stacksize,
                           priority,
                           flags,
                           task_func,
                           arg,
                           name
                           );

    DEBUG("[%d] -- pid %s = %d\n", thread_getpid(), name, pid);

    DEBUG("[%d] <- firewall:secure_thread()\n", thread_getpid());
    return pid;

}

/**
 * @brief send msg_req and wait for mg_resp
 *
 * @param arg
 * @param msg_req
 * @param msg_resp
 * @return void*
 */
void send_synchrone(msg_t msg_req, msg_t msg_resp, kernel_pid_t rcv_pid)
{
    DEBUG("[%d] -> firewall:send_synchrone()\n", thread_getpid());

    msg_send_receive(&msg_req, &msg_resp, rcv_pid);
    DEBUG("Result: %" PRIu32 "\n", msg_resp.content.value);

    DEBUG("[%d] <- firewall:send_synchrone()\n", thread_getpid());

}

/**
 * @brief
 *
 * @param arg
 * @return void*
 */
msg_t rcv_synchrone(void)
{
    DEBUG("[%d] -> firewall:rcv_synchrone()\n", thread_getpid());

    msg_t msg_resp, msg_req;

    msg_receive(&msg_req);
    DEBUG("Received %" PRIu32 "\n", msg_req.content.value);
    msg_resp.content.value = msg_req.content.value;
    msg_reply(&msg_req, &msg_resp);

    DEBUG("[%d] <- firewall:rcv_synchrone()\n", thread_getpid());
    return msg_req;
}

/**
 * @brief
 *
 * @param rcv_pid pid distination
 * @param msg_req msg to send
 * @return int
 */
int send_asynchrone(msg_t msg_req, kernel_pid_t rcv_pid)
{
    DEBUG("[%d] -> firewall:send_asynchrone()\n", thread_getpid());

    int ret = msg_try_send(&msg_req, rcv_pid);

    if (ret != 1) {
        if (ret == -1) {
            DEBUG("Error PID \n");
        }
        else if (ret == 0) {
            DEBUG("full message queue, or receiver is not waiting");
        }
        DEBUG("send msg: %" PRIu32 "\n", msg_req.content.value);

    }

    DEBUG("[%d] <- firewall:send_asynchrone()\n", thread_getpid());
    return ret;
}

/**
 * @brief waits for a message without queue
 *
 * @param arg
 * @return msg_t
 */
msg_t rcv_asynchrone(void)
{
    DEBUG("[%d] -> firewall:rcv_asynchrone()\n", thread_getpid());

    msg_t msg;

    msg_receive(&msg);
    DEBUG("Received %" PRIu32 "\n", msg.content.value);

    DEBUG("[%d] <- firewall:rcv_asynchrone()\n", thread_getpid());
    return msg;
}

/**
 * @brief waits for a message with queue
 *
 * @param arg
 * @return msg_t
 */
msg_t rcv_asynchrone_queue(void)
{
    DEBUG("[%d] -> firewall:rcv_asynchrone_queue()\n", thread_getpid());

    msg_t msg;
    msg_t rcv_queue[RCV_QUEUE_SIZE];

    msg_init_queue(rcv_queue, RCV_QUEUE_SIZE);
    msg_receive(&msg);
    DEBUG("Received %" PRIu32 "\n", msg.content.value);

    DEBUG("[%d] <- firewall:rcv_asynchrone_queue()\n", thread_getpid());
    return msg;
}

kernel_pid_t firewall_getpid(void)
{
    return thread_getpid();
}

int tinycontainer_firewall_init(void)
{
    /*nothing to do*/

    return 0;
}
