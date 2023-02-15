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
 * @brief       TinyContainer Service sub-module implementation
 *
 * @author      BERKANE Ghilas (ghilas.berkane@gmail.com)
 *
 * @}
 */

#include <string.h>

#define ENABLE_DEBUG 0

#include "tinycontainer/debugging.h"

#include "service_internal.h"
#include "tinycontainer/service/service.h"
#include "tinycontainer/service/service_controller.h"

#include "tinycontainer/firewall/firewall_all.h"
#include "tinycontainer/container/container.h"

static kernel_pid_t service_pid = -1;
static int container_priority;

#define MAX_NB_OF_CONTAINER 2
static int container_pids[MAX_NB_OF_CONTAINER];

bool container_exist(int *list, int id_cont)
{
    LOG_ENTER();
    for (int i = 0; i < (int)sizeof list; i++) {
        if (list[i] == id_cont) {
            LOG_EXIT();
            return true;
        }
    }

    LOG_EXIT();
    return false;
}

/**
 * @brief Create thread of the container

 * @param uri uri CoAP
 */
kernel_pid_t create_container( char *uri, uint32_t periodicity)
{
    LOG_ENTER();

    static char *context = 0;
    kernel_pid_t pid;

    /*declare the thread stack for containers */
    static char container_stack[5 * THREAD_STACKSIZE_SMALL];     //FIXME: dynamic alloc from static buf?

    /*create Container */
    pid = secure_thread(
        context,                    // context to create thred man
        container_stack,            // stack array pointer
        sizeof(container_stack),    // stack size
        container_priority,         // thread priority
        THREAD_CREATE_STACKTEST,    // thread configuration flag
        container_handler,          // thread handler function
        (void *)periodicity /*ms*/, // argument of thread_handler function
        uri                         // Name of thread
        );

    thread_yield();
    LOG_EXIT();
    return pid;
}


void *handler_service(void *arg)
{
    LOG_ENTER();
    (void)arg;

    memset(container_pids, -1, sizeof(container_pids));

    while (1) {
        LOG_PID_FUNC("awaiting message...\n");

        /* block the thread until a message is received */
        service_msg_t service_msg_req;
        SERVICE_MSG_RECEIVE(&service_msg_req);

        service_msg_type type = service_msg_req.type;
        uint32_t value = service_msg_req.value;

        /* note: an uint32_t is not always an unsigned long (e.g. native board)
         */
        LOG_PID_FUNC("new message received: type= %d, value= %ld\n", type, (unsigned long)value);

        /* container management */

        service_msg_type response_type;
        int response_value = -1;

        switch (type) {

        case run:
            LOG_PID_FUNC("type %d -> run\n", run);

            LOG_PID_FUNC("calling create_container()\n");

            /* create the container with 1s periodicity */
            kernel_pid_t container_pid = create_container("container", 1000 /*ms*/);

            if (container_pid == -1) {
                LOG_PID_FUNC("create_container() failed\n");
                response_type = ko;
                break;
            }

            LOG_PID_FUNC("create_container() success\n");
            container_pids[value] = container_pid;
            response_type = ok;
            break;

        case stop:

            LOG_PID_FUNC("type %d -> stop\n", service_msg_req.type);

            /* TODO: not yet implemented */
            LOG_PID_FUNC("not yet implemented\n");

            response_type = ko;
            break;

        case is_running:

            LOG_PID_FUNC("type %d -> is_running\n", service_msg_req.type);

            response_type = ko;

            /* 'value' contains the slot id of the container */
            thread_t *thread = thread_get(container_pids[value]);
            if (thread != NULL) {
                thread_status_t status = thread_get_status(thread);
                if (!(status == STATUS_STOPPED ||
                      status == STATUS_ZOMBIE)) {
                    response_type = ok;
                }
            }

            break;

        case container_id:

            LOG_PID_FUNC("type %d -> container_id\n", service_msg_req.type);

            for (int i = 0; i < MAX_NB_OF_CONTAINER; i++) {
                /* value contains the searched pid*/
                int pid = (int)value;
                if (container_pids[i] == pid) {
                    /* container found  */
                    response_type = ok;
                    response_value = (uint32_t)i;
                }
            }

            /* no container associated with this pid */
            response_type = ko;
            break;

        default:
            LOG_PID_FUNC("unknown message type '%d'\n", service_msg_req.type);

            response_type = ko;
            break;
        }
        service_msg_t service_msg_res = { .type = response_type, .value = response_value };

        SERVICE_MSG_REPLY(&service_msg_req, &service_msg_res);
    }

    LOG_EXIT();
    return NULL;
}

int tinycontainer_service_init(int service_prio, int container_prio)
{
    LOG_ENTER();

    if (service_pid != -1) {
        return -1;
    }

    static char service_stack[THREAD_STACKSIZE_SMALL];

    container_priority = container_prio;

    service_pid = secure_thread(
        NULL,                       // context to create thred man
        service_stack,              // stack array pointer
        sizeof(service_stack),      // stack size
        service_prio,               // thread priority
        THREAD_CREATE_STACKTEST,    // thread configuration flag, usually
        handler_service,            // thread handler function
        NULL,                       // argument of thread_handler function
        "service"
        );

    LOG_EXIT();
    return service_pid;
}

int service_start(int container_id)
{
    service_msg_t msg = { .type = run, .value = container_id };

    SERVICE_MSG_SEND_RECEIVE_INPLACE(&msg);
    return msg.type == ok;
}

int service_stop(int container_id)
{
    service_msg_t msg = { .type = stop, .value = container_id };

    SERVICE_MSG_SEND_RECEIVE_INPLACE(&msg);
    return msg.type == ok;
}

int service_isrunning(int container_id)
{
    service_msg_t msg = { .type = is_running, .value = container_id };

    SERVICE_MSG_SEND_RECEIVE_INPLACE(&msg);
    return msg.type == ok;
}

int service_getcontaineridfrompid(int pid)
{
    service_msg_t msg = { .type = container_id, .value = pid };

    SERVICE_MSG_SEND_RECEIVE_INPLACE(&msg);
    return (msg.type == ok)?(int)msg.value:-1;
}
