/*
 * Copyright (C) 2023 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory of the TinyContainer module for more details.
 */

#include "kernel_defines.h"

#if IS_USED(MODULE_GCOAP)

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "coap_server.h"
#include "sched.h"

#include "net/gcoap.h"

#include "tinycontainer.h"

typedef enum { STATE_UNINIALIAZED, STATE_OFF, STATE_ON, } state_t;

enum { ACTION_LOAD       = 1,
       ACTION_START      = 2,
       ACTION_IS_RUNNING = 3,
       ACTION_STOP       = 4,
       ACTION_DELETE     = 5,
};

/* RFC7254 states that option numbers between 65000 and 65535 inclusive are reserved
 * for experiments.
 */
enum { OPTION_ACTION   = 65001,
       OPTION_METADATA = 65002,
       OPTION_DATA     = 65003,
       OPTION_CODE     = 65004,
       OPTION_UID      = 65005,
       OPTION_STATUS   = 65006,
};

#define DEVICE_ID_MAX_SIZE 64
static struct {
    state_t state;
    kernel_pid_t thread_pid;
    uint8_t device_id[DEVICE_ID_MAX_SIZE];
    uint32_t device_id_size;
} config = { STATE_UNINIALIAZED, KERNEL_PID_UNDEF, "", 0 };

/* list of option:
 *   - 1 action, uint:
 *           - 1 => LOAD
 *           - 2 => START
 *           - 3 => IS_RUNNING
 *           - 4 => STOP
 *           - 5 => DELETE
 *   - 2 metadata, opaque
 *   - 3 data, opaque
 *   - 4 code, opaque
 *   - 5 container uid, opaque
 *   - 6 status, uint:
 *           - 0 => container is not running
 *           - 1 => container is running
 */
ssize_t _container_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, coap_request_ctx_t *context) {

    (void) context;

    (void) puts("Coap Handler!");

    uint32_t action;
    int ret = coap_opt_get_uint(pdu, OPTION_ACTION, &action);
    switch (ret) {
        case 0:
            /* all right continue  */
            break;
        case ENOENT:
        case ENOSPC:
        case EBADMSG:
        default:
            //TODO: add warn log
            (void) puts("Can't get action in option");
            return gcoap_response(pdu, buf, len, COAP_CODE_BAD_OPTION);
    }

    unsigned method_flag = coap_method2flag(coap_get_code_detail(pdu));

    uint8_t * container_uid;

    switch (action) {

        case ACTION_LOAD:

            /* check the CoAP method */
            if (method_flag != COAP_POST) {
                return gcoap_response(pdu, buf, len, COAP_CODE_METHOD_NOT_ALLOWED);
            }

            /* retrieve container parts */
            uint8_t * metadata;
            ssize_t metadata_size = coap_opt_get_opaque(pdu, OPTION_METADATA, &metadata);
            uint8_t * data;
            ssize_t data_size = coap_opt_get_opaque(pdu, OPTION_DATA, &data);
            uint8_t * code;
            ssize_t code_size = coap_opt_get_opaque(pdu, OPTION_CODE, &code);
            if(metadata_size < 0 || data_size < 0 || code_size <= 0) {
                //TODO: add warn log
                return gcoap_response(pdu, buf, len, COAP_CODE_BAD_OPTION);
            }

            /* try to load the container */
            if (tinycontainer_loadcontainer(metadata, metadata_size, data, data_size, code, code_size) == true) {
                /* success */
                //TODO: add info log
                return gcoap_response(pdu, buf, len, COAP_CODE_CREATED);
            } else {
                /* failure */
                //TODO: add warn log
                return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
            }

        case ACTION_START:

            /* check the CoAP method */
            if (method_flag != COAP_PUT) {
                return gcoap_response(pdu, buf, len, COAP_CODE_METHOD_NOT_ALLOWED);
            }

            /* retrieve container uid */
            ret = coap_opt_get_opaque(pdu, OPTION_UID, &container_uid);
            if(ret < 0) {
                //TODO: add warn log
                return gcoap_response(pdu, buf, len, COAP_CODE_BAD_OPTION);
            }

            /* try to start the container */
            if(tinycontainer_startcontainer((uint8_t*)"") == true) {
                /* success */
                //TODO: add info log
                return gcoap_response(pdu, buf, len, COAP_CODE_CHANGED);
            } else {
                /* failure */
                //TODO: add warn log
                return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
            }

        case ACTION_IS_RUNNING:

            /* check the CoAP method */
            if (method_flag != COAP_GET) {
                return gcoap_response(pdu, buf, len, COAP_CODE_METHOD_NOT_ALLOWED);
            }

            /* retrieve container uid */
            ret = coap_opt_get_opaque(pdu, OPTION_UID, &container_uid);
            if(ret < 0) {
                //TODO: add warn log
                return gcoap_response(pdu, buf, len, COAP_CODE_BAD_OPTION);
            }

            /* ask for container status */
            gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
            if(tinycontainer_iscontainerrunning((uint8_t*)"") == true) {
                /* container is up */
                coap_opt_add_uint(pdu, OPTION_STATUS, 1);
            } else {
                /* container is down */
                coap_opt_add_uint(pdu, OPTION_STATUS, 0);
            }
            return coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);

        case ACTION_STOP:

            /* check the CoAP method */
            if (method_flag != COAP_PUT) {
                return gcoap_response(pdu, buf, len, COAP_CODE_METHOD_NOT_ALLOWED);
            }

            /* no yet implemented */
            return gcoap_response(pdu, buf, len, COAP_CODE_NOT_IMPLEMENTED);

        case ACTION_DELETE:

            /* check the CoAP method */
            if (method_flag != COAP_DELETE) {
                return gcoap_response(pdu, buf, len, COAP_CODE_METHOD_NOT_ALLOWED);
            }

            /* no yet implemented */
            return gcoap_response(pdu, buf, len, COAP_CODE_NOT_IMPLEMENTED);

        default:

            //TODO: add error log
            return gcoap_response(pdu, buf, len, COAP_CODE_BAD_OPTION);
    }

    /* this line is never reached */
}

ssize_t _device_id_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, coap_request_ctx_t *context) {

    (void) context;

    (void) puts("Device id Handler!");

    unsigned method_flag = coap_method2flag(coap_get_code_detail(pdu));

    switch (method_flag) {

        case COAP_GET:

            if(config.device_id_size == 0 ) {
                 return gcoap_response(pdu, buf, len, COAP_CODE_PATH_NOT_FOUND);
            }

            gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
            coap_opt_add_format(pdu, COAP_FORMAT_OCTET);
            size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

            if (pdu -> payload_len >= config.device_id_size) {
                 memcpy(pdu -> payload, config.device_id, config.device_id_size);
                 return resp_len + config.device_id_size;
            } else {
                 return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
            }

            break;

        case COAP_POST:

            if(pdu -> payload_len == 0) {
                return gcoap_response(pdu, buf, len, COAP_CODE_BAD_REQUEST);
            } else if(pdu -> payload_len > DEVICE_ID_MAX_SIZE) {
                gcoap_resp_init(pdu, buf, len, COAP_CODE_REQUEST_ENTITY_TOO_LARGE);
                coap_opt_add_uint(pdu, COAP_OPT_SIZE1, DEVICE_ID_MAX_SIZE);
                return coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
            } else {
                memcpy(config.device_id, pdu -> payload, pdu -> payload_len);
                config.device_id_size = pdu -> payload_len;
                return gcoap_response(pdu, buf, len, COAP_CODE_CHANGED);
            }

            break;

        default:

            //TODO: add error log
            return gcoap_response(pdu, buf, len, COAP_CODE_BAD_OPTION);
    }
}

/* CoAP resources. Must be sorted by path (ASCII order). */
static const coap_resource_t _resources[] = {
    { "/container/", COAP_GET | COAP_POST | COAP_PUT | COAP_DELETE, _container_handler, NULL },
    { "/device-id/", COAP_GET | COAP_POST, _device_id_handler, NULL },
};

static gcoap_listener_t _listener = {
    & _resources[0],
    ARRAY_SIZE(_resources),
    GCOAP_SOCKET_TYPE_UNDEF,
    NULL,
    NULL,
    NULL,
};

/* public function */

void coap_server_init(void) {
    config.state = STATE_OFF;
}

void coap_server_start(void) {
    if (config.state == STATE_OFF) {
        config.thread_pid = gcoap_init();
        if(config.thread_pid < KERNEL_PID_FIRST || config.thread_pid > KERNEL_PID_LAST) {
            //FIXME: add error log
            config.thread_pid = KERNEL_PID_UNDEF;
        } else {
            gcoap_register_listener(&_listener);
            config.state = STATE_ON;
        }
            gcoap_register_listener(&_listener);
            config.state = STATE_ON;
    }
}

void coap_server_stop(void) {
    if (config.state == STATE_ON) {
        //FIXME: how to kill another thread on riot?
        //TODO: add warm log or remove the function
        config.thread_pid = KERNEL_PID_UNDEF;
        config.state = STATE_OFF;
    }
}

bool coap_server_isrunning(void) {
    return config.state == STATE_ON;
}

#endif /* MODULE_GCOAP */
