/*
 * Copyright (C) 2020-2024 Orange
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
 * @brief       TinyContainer Service sub-module implementation
 *
 * @author      BERKANE Ghilas (ghilas.berkane@gmail.com)
 * @author      Samuel legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#include <string.h>

//#define ENABLE_DEBUG (1)

#include "tinycontainer/debugging.h"

#include "service_internal.h"
#include "service_container.h"
#include "service_endpoint.h"
#include "tinycontainer/service/service.h"
#include "tinycontainer/service/service_memmgr.h"
#include "tinycontainer/service/service_controller.h"
#include "tinycontainer/service/service_runtime.h"

#include "tinycontainer/security/sthread/sthread.h"
#include "tinycontainer/runtime/runtime.h"

static kernel_pid_t service_pid = -1;
static int container_priority;

#ifdef TINYCONTAINER_NUMBER_OF_CONTAINERS
#define MAX_NB_OF_CONTAINER   TINYCONTAINER_NUMBER_OF_CONTAINERS
#else
#define MAX_NB_OF_CONTAINER   3
#endif

static container_t containers[MAX_NB_OF_CONTAINER];

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

static int get_container_slot_id_for_pid(int pid)
{

    /* searching the container */
    for (int i = 0; i < MAX_NB_OF_CONTAINER; i++) {
        if (containers[i].pid == pid) {
            /* container found */
            return i;
        }
    }

    /* not found */
    return -1;
}

static void container_callback(kernel_pid_t pid, void *caller_context)
{
    int slot_id = get_container_slot_id_for_pid(pid);

    if (&containers[slot_id] == caller_context) {
        container_setstopping(&containers[slot_id]);
    }
}

/**
 * @brief Create thread of the container

 * @param uri uri CoAP
 */
kernel_pid_t create_container_runtime(int slot_id)
{
    LOG_ENTER();

    kernel_pid_t pid;

    char *stack = (char *)containers[slot_id].stack;
    size_t size = sizeof(containers[slot_id].stack);
    void *callee_context = &containers[slot_id].shared_memory;

    /* create Container */
    pid = sthread(
        &containers[slot_id],       // caller_context
        callee_context,             // callee_context
        container_callback,         // task_callback
        stack,                      // stack array pointer
        size,                       // stack size
        container_priority,         // thread priority
        THREAD_CREATE_STACKTEST,    // thread configuration flag
        runtime_handler,            // thread handler function
        "container"                 // Name of thread
        );

    thread_yield();
    LOG_EXIT();
    return pid;
}

static bool is_container_running(int slot_id)
{
    /* note: Probably the right way would be to directly return true if the
     *       pid value in container_pids array is positive. Right now, we
     *       do not properly handle the end of a container execution.
     *       Also, it seems better to check if a running thread is really
     *       associated with the container pid.
     */

    kernel_pid_t pid = container_getpid(&containers[slot_id]);

    if (pid > 0) {
        thread_t *thread = thread_get(pid);
        if (thread != NULL) {
            thread_status_t status = thread_get_status(thread);
            if (!(status == STATUS_STOPPED ||
                  status == STATUS_ZOMBIE)) {
                return true;
            }
        }
    }

    return false;
}

void *handler_service(void *arg)
{
    LOG_ENTER();
    (void)arg;

    memset(containers, 0, sizeof(containers));

    while (1) {

        LOG_PID_FUNC("awaiting message...\n");

        /* block the thread until a message is received */
        service_msg_t service_msg_req;
        SERVICE_MSG_RECEIVE(&service_msg_req);

        service_msg_type type = service_msg_req.type;
        uint32_t value = service_msg_req.value;

        /* note: here the cast is mandatory because an uint32_t is not always
         *       an unsigned long (e.g. native board).
         */
        LOG_PID_FUNC("new message received: type= %d, value= %ld\n", type,
                     (unsigned long)value);

        /* container management */

        service_msg_type response_type;
        uint32_t response_value = -1;

        int slot_id = -1;

        switch (type) {

        case run:
            LOG_PID_FUNC("type %d -> run\n", run);

            /* value contains the slot_id */
            slot_id = value;

            /* check the running status of the container */
            if (is_container_running(slot_id)) {
                LOG_PID_FUNC("Container %ld already started!\n",
                             (unsigned long)slot_id);
                response_type = ko;
                break;
            }

            /* create the container with 1s periodicity */
            LOG_PID_FUNC("calling create_container_runtime()\n");
            kernel_pid_t container_pid = create_container_runtime(slot_id);

            if (container_pid == -1) {
                LOG_PID_FUNC("create_container_runtime() failed\n");
                response_type = ko;
                break;
            }

            LOG_PID_FUNC("create_container_runtime() success\n");

            /* setting up the container env */
            //memset(&containers[slot_id], 0, sizeof(container_t));
            containers[slot_id].id = slot_id;
            containers[slot_id].pid = container_pid;
            containers[slot_id].shared_memory.ro.period = 1000 /*ms*/;
            LOG_PID_FUNC("slot_id=%ld, pid=%d\n", (unsigned long)slot_id,
                         container_pid);
            response_type = ok;
            break;

        case stop:

            LOG_PID_FUNC("type %d -> stop\n", service_msg_req.type);

            //TODO: not yet implemented
            LOG_PID_FUNC("not yet implemented\n");

            response_type = ko;
            break;

        case is_running:

            LOG_PID_FUNC("type %d -> is_running\n", service_msg_req.type);

            response_type = ko;

            /* 'value' contains the slot id of the container */
            if (is_container_running(value)) {
                response_type = ok;
            }

            break;

        case container_slot_id:

            LOG_PID_FUNC("type %d -> container_slot_id\n",
                         service_msg_req.type);

            /* no container associated with this pid yet */
            response_type = ko;

            /* search the container associated with the current thread */
            slot_id = get_container_slot_id_for_pid(service_msg_req.sender_pid);
            if (slot_id != -1) {
                /* container found  */
                response_type = ok;
                response_value = (uint32_t)slot_id;
                LOG_PID_FUNC("slot_id=%d matched\n", slot_id);
                break;
            }

            break;

        case msg_syscall:

            LOG_PID_FUNC("type %d -> syscall\n", msg_syscall);

            response_type = none;

            slot_id = get_container_slot_id_for_pid(service_msg_req.sender_pid);
            if (slot_id == -1) {
                response_type = ko;
                break;
            }

            container_t *container = &containers[slot_id];
            uint32_t syscall_id = container->shared_memory.rw.syscall_id;

            /* value contains the syscall type  */
            switch (syscall_id) {
            case SERVICE_SYSCALL_HEARTBEAT:

                //TODO: not yet implemented
                LOG_PID_FUNC("not yet implemented\n");

                response_type = ko;
                break;

            case SERVICE_SYSCALL_OPEN:

                LOG_PID_FUNC("syscall_id %ld -> OPEN\n", (long)syscall_id);

                response_value = endpoint_open(container);
                response_type = ok;

                break;

            case SERVICE_SYSCALL_CLOSE:

                LOG_PID_FUNC("syscall_id %ld -> CLOSE\n", (long)syscall_id);

                response_value = endpoint_close(container);
                response_type = ok;

                break;

            case SERVICE_SYSCALL_READ:

                LOG_PID_FUNC("syscall_id %ld -> READ\n", (long)syscall_id);

                response_value = endpoint_read(container);
                response_type = ok;

                break;

            case SERVICE_SYSCALL_WRITE:

                LOG_PID_FUNC("syscall_id %ld -> READ\n", (long)syscall_id);

                response_value = endpoint_write(container);
                response_type = ok;

                break;

            }

            break;

        default:
            LOG_PID_FUNC("unknown message type '%d'\n", service_msg_req.type);

            response_type = ko;
            break;
        }
        service_msg_t service_msg_res = { .type = response_type,
                                          .value = response_value };

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

//TODO
//    static char service_stack[THREAD_STACKSIZE_SMALL];
    static char service_stack[THREAD_STACKSIZE_SMALL * 3];

    container_priority = container_prio;

    service_pid = sthread(
        NULL,                       // caller context
        NULL,                       // callee context
        NULL,                       // task callback
        service_stack,              // stack array pointer
        sizeof(service_stack),      // stack size
        service_prio,               // thread priority
        THREAD_CREATE_STACKTEST,    // thread configuration flag, usually
        handler_service,            // thread handler function
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

int service_getcontainerslotid(void)
{
    LOG_ENTER();
    service_msg_t msg = { .type = container_slot_id, .value = 0 };

    SERVICE_MSG_SEND_RECEIVE_INPLACE(&msg);
    if (msg.type == ok) {
        LOG_PID_FUNC("container %ld is associated with pid %d\n",
                     (unsigned long)msg.value, thread_getpid());
    }
    else {
        LOG_PID_FUNC("no container is associated with pid %d\n",
                     thread_getpid());
    }
    LOG_EXIT();
    return (msg.type == ok)?(int)msg.value:-1;
}

int32_t service_syscall(void)
{
    LOG_ENTER();

    service_msg_t msg = { .type = msg_syscall, .value = 0 };

    SERVICE_MSG_SEND_RECEIVE_INPLACE(&msg);

    LOG_EXIT();

    /* note: in case of success msg.value is used as the returned value.
     *       The value meaning depends of the type of syscall.
     *       -1 is returned otherwise.
     */
    return (msg.type == ok)?(int)msg.value:-1;
}
