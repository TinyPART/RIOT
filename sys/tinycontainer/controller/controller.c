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
 * @brief       TinyContainer Controller sub-module implementation
 *
 * @author      BERKANE Ghilas (ghilas.berkane@gmail.com)
 *              HOLDER Gregory (gregory.holder@orange.com)
 *
 * @}
 */

//#define ENABLE_DEBUG (1)
#include "tinycontainer/debugging.h"
#include "thread.h"

#include "tinycontainer/controller/controller.h"
#include "tinycontainer/service/service_controller.h"
#include "tinycontainer/memmgr/memmgr_controller.h"
#include "tinycontainer/firewall/firewall_all.h"

#include <stdbool.h>

enum controller_msg_type {
    msg_type_ok             = 0,
    msg_type_ko             = 1,
    msg_type_start          = 2,
    msg_type_stop           = 3,
    msg_type_isrunning      = 4,
    msg_type_loading        = 5,
    msg_type_metadata_size  = 6,
    msg_type_metadata       = 7,
    msg_type_data_size      = 8,
    msg_type_data           = 9,
    msg_type_code_size      = 10,
    msg_type_code           = 11,
    msg_type_load_end       = 12,
};

/* Message type */
typedef enum controller_msg_type s_msg_type;

const char *controller_msg_type_str[] = {
    "msg_type_ok",
    "msg_type_ko",
    "msg_type_start",
    "msg_type_stop",
    "msg_type_isrunning",
    "msg_type_loading",
    "msg_type_metadata_size",
    "msg_type_metadata",
    "msg_type_data_size",
    "msg_type_data",
    "msg_type_code_size",
    "msg_type_code",
    "msg_type_load_end",
    "msg_type_cancel",
};

enum controller_loading_state {
    loading_none    = 0,
    loading_started = 1,
    loading_meta    = 2,
    loading_data    = 3,
    loading_code    = 4,
    loading_end     = 5,
};

const char *controller_loading_state_str[] = {
    "loading_none",
    "loading_started",
    "loading_meta",
    "loading_data",
    "loading_code",
    "loading_end",
};

static kernel_pid_t controller_pid = -1;
static enum controller_loading_state loading_state = loading_none;
static uint32_t container_id = 0;
static file_descriptor_t fd = -1;
static int remaining_section_size = 0;

typedef int (*open_section_fn)(container_id_t);

/**
 * @brief
 * @param expected_state   What is the expected state before starting the section
 * @param new_state        What the new state should be after the section is started
 * @param size             The size of the section
 * @param open_section     The function that opens the section
 * @return
 */
static bool _start_section_write(
    enum controller_loading_state expected_state,
    enum controller_loading_state new_state,
    int size,
    open_section_fn open_section)
{
    if (loading_state != expected_state) {
        const char *expected_state_str = controller_loading_state_str[expected_state];
        const char *new_state_str = controller_loading_state_str[new_state];
        const char *loading_state_str = controller_loading_state_str[loading_state];
        DEBUG_PID("ERROR: expected state '%s' before changing to '%s', but current state is '%s'\n",
                  expected_state_str, new_state_str, loading_state_str);
        return false;
    }

    if (remaining_section_size != 0) {
        DEBUG_PID("ERROR: previous section was not fully written, %d bytes left\n",
                  remaining_section_size);
        return false;
    }

    if (fd != -1) {
        DEBUG_PID("ERROR: previous section was not closed, fd = %d\n", fd);
        return false;
    }

    /* open the section */
    fd = open_section(container_id);
    if (fd == -1) {
        /* note: an uint32_t is not always an unsigned long on all board (e.g. native) */
        DEBUG_PID("ERROR: failed to open section, container_id = %ld\n",
                  (unsigned long)container_id);
        return false;
    }

    /* set the remaining size of the section */
    remaining_section_size = size;
    /* set the new state */
    loading_state = new_state;

    /* if the section is empty, close it immediately */
    if (remaining_section_size == 0) {
        memmgr_close(fd);
        fd = -1;
    }

    return true;
}

/* takes expected_state and the byte
 * automatically closes the section if it is full
 */
static bool _write_byte_to_section(enum controller_loading_state expected_state, char byte)
{
    if (loading_state != expected_state) {
        const char *expected_state_str = controller_loading_state_str[expected_state];
        const char *loading_state_str = controller_loading_state_str[loading_state];
        DEBUG_PID("ERROR: expected state '%s', but current state is '%s'\n", expected_state_str,
                  loading_state_str);
        return false;
    }

    if (fd == -1) {
        DEBUG_PID("ERROR: no sections open\n");
        return false;
    }

    if (remaining_section_size == 0) {
        DEBUG_PID("ERROR: section %d is full\n", fd);
        return false;
    }

    if (memmgr_write(fd, &byte, 1) == -1) {
        DEBUG_PID("ERROR: failed to write to section %d\n", fd);
        return false;
    }

    remaining_section_size--;

    /* if the section is full, close it */
    if (remaining_section_size == 0) {
        memmgr_close(fd);
        fd = -1;
    }

    return true;
}

/**
 * @brief function du thrtead controller
 *
 * @return void*
 */
static void *handler_controller(void *arg)
{
    (void)arg;
    LOG_ENTER();

    /* recommended to be static */
    static msg_t msg_queue[1];

    msg_init_queue(msg_queue, 1);

    while (true) {
        msg_t received_msg;
        msg_receive(&received_msg);

        kernel_pid_t sender_pid = received_msg.sender_pid;
        s_msg_type type = received_msg.type;
        uint32_t value = received_msg.content.value;

        /* default to ok */
        s_msg_type reply_type = msg_type_ok;

        /* don't print single byte messages (there are too many of them) */
        if (type != msg_type_data &&
            type != msg_type_code &&
            type != msg_type_metadata) {
            const char *type_str = controller_msg_type_str[type];
            /* note: an uint32_t is not always an unsigned long (e.g. on native board) */
            DEBUG_PID("-- received message: { pid: %d, type: %-22s, value: %ld }\n", sender_pid,
                      type_str, (unsigned long)value);
        }

        switch (type) {
        /* start a container */
        case msg_type_start:
            reply_type = service_start(value) ? msg_type_ok : msg_type_ko;
            goto reply;

        /* stop a container */
        case msg_type_stop:
            reply_type = service_stop(value) ? msg_type_ok : msg_type_ko;
            goto reply;

        /* check if a container is running */
        case msg_type_isrunning:
            reply_type = service_isrunning(value) ? msg_type_ok : msg_type_ko;
            goto reply;

        /* start loading a container */
        case msg_type_loading:
            if (loading_state != loading_none) {
                goto loading_fail;
            }
            container_id = memmgr_newcontainer();
            loading_state = loading_started;
            goto reply;

        /* set container metadata size */
        case msg_type_metadata_size:
            if (_start_section_write(loading_started, loading_meta, value,
                                     memmgr_openmetadatafileforcontainer)) {
                goto reply;
            }
            else {
                goto loading_fail;
            }

        /* load next byte of container metadata */
        case msg_type_metadata:
            if (_write_byte_to_section(loading_meta, (char)value)) {
                goto reply;
            }
            else {
                goto loading_fail;
            }

        /* set container data size */
        case msg_type_data_size:
            if (_start_section_write(loading_code, loading_data, value,
                                     memmgr_opendatafileforcontainer)) {
                goto reply;
            }
            else {
                goto loading_fail;
            }

        /* load next byte of container data */
        case msg_type_data:
            if (_write_byte_to_section(loading_data, (char)value)) {
                goto reply;
            }
            else {
                goto loading_fail;
            }

        /* set container code size */
        case msg_type_code_size:
            if (_start_section_write(loading_meta, loading_code, value,
                                     memmgr_opencodefileforcontainer)) {
                goto reply;
            }
            else {
                goto loading_fail;
            }

        /* load next byte of container code */
        case msg_type_code:
            if (_write_byte_to_section(loading_code, (char)value)) {
                goto reply;
            }
            else {
                goto loading_fail;
            }

        /* finalize container loading */
        case msg_type_load_end:
            if (loading_state != loading_data || remaining_section_size != 0) {
                goto loading_fail;
            }

            memmgr_close(fd);
            fd = -1;
            loading_state = loading_none;

            goto reply;

        default:
            DEBUG("[%d] EE invalid message type: %d\n", controller_pid, type);
            goto loading_fail;
        }

/* something went wrong, abort the loading */
loading_fail:
        loading_state = loading_none;
        remaining_section_size = 0;

        /* send the reply and yield to other threads */
        reply_type = msg_type_ko;

        if (fd != -1) {
            memmgr_close(fd);
        }
        fd = -1;

reply:
        msg_reply(&received_msg, &(msg_t){ .type = reply_type });
        thread_yield();
    }

    DEBUG_PID("EE unreachable line\n");

    LOG_EXIT();
    return NULL;
}

int tinycontainer_controller_init(int prio)
{
    LOG_ENTER();

    /* only one instance of the controller */
    if (controller_pid != -1) {
        return controller_pid;
    }

    static char controller_stack[THREAD_STACKSIZE_DEFAULT];

    DEBUG_PID("-- calling secure_thread()\n");

    controller_pid = secure_thread(
        NULL,                       // context to create thread man
        controller_stack,           // thread stack
        sizeof(controller_stack),   // stack size
        prio,                       // thread priority
        THREAD_CREATE_STACKTEST,    // stack size flag
        handler_controller,         // thread handler function
        NULL,                       // empty argument
        "controller"                // thread name
        );

    LOG_EXIT();
    return controller_pid;
}

bool controller_start(int container_id)
{
    msg_t reply;
    msg_t m = { .type = msg_type_start, .content.value = container_id };

    msg_send_receive(&m, &reply, controller_pid);

    return reply.type == msg_type_ok;
}

bool controller_stop(int container_id)
{
    msg_t reply;
    msg_t m = { .type = msg_type_stop, .content.value = container_id };

    msg_send_receive(&m, &reply, controller_pid);

    return reply.type == msg_type_ok;
}

bool controller_isrunning(int container_id)
{
    msg_t reply;
    msg_t m = { .type = msg_type_isrunning, .content.value = container_id };

    msg_send_receive(&m, &reply, controller_pid);

    return reply.type == msg_type_ok;
}

/* send msg, receive type
 */
static s_msg_type _send_msg_receive_msg_type(s_msg_type type, uint32_t value)
{
    msg_t reply;
    msg_t m = { .type = type, .content.value = value };

    msg_send_receive(&m, &reply, controller_pid);

    return reply.type;
}

/* send msg, expect msg_type_ok
 */
static bool _send_msg_expect_ok(s_msg_type type, uint32_t value)
{
    return _send_msg_receive_msg_type(type, value) == msg_type_ok;
}

/* send msg_type, receive msg_type
 */
static s_msg_type _send_msg_type_receive_msg_type(s_msg_type type)
{
    return _send_msg_receive_msg_type(type, 0);
}

/* send msg_type, expect msg_type_ok
 */
static bool _send_msg_type_expect_ok(s_msg_type type)
{
    return _send_msg_type_receive_msg_type(type) == msg_type_ok;
}

/**
 * @brief
 * @param section_byte_msg_type    Section type used to send a section byte
 * @param section                  Section byte
 * @param section_size_msg_type    Section type used to send a section size
 * @param size                     Section size
 * @return
 */
static bool _send_section(
    s_msg_type section_byte_msg_type, uint8_t *section,
    s_msg_type section_size_msg_type, int size)
{
    /* send the section size */
    if (!_send_msg_expect_ok(section_size_msg_type, size)) {
        return false;
    }

    /* send the section */
    for (int i = 0; i < size; i++) {
        if (!_send_msg_expect_ok(section_byte_msg_type, (uint32_t)section[i])) {
            return false;
        }
    }
    return true;
}

bool controller_load(uint8_t *metadata, int meta_size,
                     uint8_t *data, int data_size,
                     uint8_t *code, int code_size)
{

    /* start loading the container, and expect an ok */
    if (!_send_msg_type_expect_ok(msg_type_loading)) {
        return false;
    }

    /* note: for now, the metadata, the data and the code part are sent byte
     * per byte to the controller thread.
     */

    /* try to send the metadata */
    if (!_send_section(msg_type_metadata, metadata, msg_type_metadata_size, meta_size)) {
        return false;
    }

    /* try to send the code */
    if (!_send_section(msg_type_code, code, msg_type_code_size, code_size)) {
        return false;
    }

    /* try to send the data */
    if (!_send_section(msg_type_data, data, msg_type_data_size, data_size)) {
        return false;
    }

    /* try and finalize the loading */
    if (!_send_msg_type_expect_ok(msg_type_load_end)) {
        return false;
    }

    return true;

}
