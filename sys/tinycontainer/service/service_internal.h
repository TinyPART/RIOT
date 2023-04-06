/*
 * Copyright (C) 2022-2023, Orange.
 *
 * Please, refer to the README.md and LICENSE files of TinyContainer
 *
 */

/**
 * @ingroup     sys_tinycontainer
 * @{
 *
 * @file
 * @brief       TinyContainer Service sub-module definitions
 *
 * @author      Gregory Holder <gregory.holder@orange.com>
 *
 * @}
 */

#ifndef SERVICE_INTERNAL_H
#define SERVICE_INTERNAL_H

#include <thread.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    ok              = 1,
    ko              = 2,
    run             = 3,
    stop            = 4,
    is_running      = 5,
    delete          = 6,
    update          = 7,
    container_id    = 9,
};

typedef uint16_t service_msg_type;

/* create a msg struct where the type is service_msg_type, and the content is
 * always a uint32_t. It is equivalent to msg_t, but more explicit
 */
typedef struct service_msg {
    kernel_pid_t sender_pid;
    service_msg_type type;
    uint32_t value;
} service_msg_t;

#define SERVICE_MSG_RECEIVE(msg_ptr) msg_receive((msg_t *)msg_ptr)
#define SERVICE_MSG_REPLY(msg_req_ptr, msg_reply_ptr) \
    msg_reply((msg_t *)msg_req_ptr, (msg_t *)msg_reply_ptr)
#define SERVICE_MSG_SEND_RECEIVE(msg_req_ptr, msg_reply_ptr) \
    msg_send_receive((msg_t *)msg_req_ptr, (msg_t *)msg_reply_ptr, service_pid)

/* send a receive a message to the service.
 * The reply rewrites the request message, negating the need for two
 * allocationgs.
 * Use SERVICE_MSG_SEND_RECEIVE if you need to keep the request message.
 */
#define SERVICE_MSG_SEND_RECEIVE_INPLACE(msg_ptr) \
    SERVICE_MSG_SEND_RECEIVE(msg_ptr, msg_ptr)

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_INTERNAL_H */
