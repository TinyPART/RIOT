/*
 * Copyright (C) 2023-2024 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory of the TinyContainer module for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       TinyContainer example - coap server
 *
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#include "kernel_defines.h"

#if IS_USED(MODULE_GCOAP)

//#define ENABLE_DEBUG (1)
#include "debug.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "coap_server.h"
#include "sched.h"

#include "net/gcoap.h"

#include "tinycontainer.h"

enum { ACTION_LOAD          = 1,
       ACTION_START         = 2,
       ACTION_IS_RUNNING    = 3,
       ACTION_STOP          = 4,
       ACTION_DELETE        = 5, };

/* RFC7254 states that option numbers between 65000 and 65535 inclusive are reserved
 * for experiments.
 */
enum { OPTION_ACTION    = 65001,
       OPTION_METADATA  = 65002,
       OPTION_DATA      = 65003,
       OPTION_CODE      = 65004,
       OPTION_UID       = 65005,
       OPTION_STATUS    = 65006, };

#define DEVICE_ID_MAX_SIZE 64
static struct {
    uint8_t device_id[DEVICE_ID_MAX_SIZE];
    uint32_t device_id_size;
} config = { "", 0 };

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
ssize_t _container_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, coap_request_ctx_t *context)
{

    (void)context;

    DEBUG("[CoAP] Container handler\n");

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
        DEBUG("[CoAP] WARN: no action key in CoAP options\n");
        return gcoap_response(pdu, buf, len, COAP_CODE_BAD_OPTION);
    }

    unsigned method_flag = coap_method2flag(coap_get_code_detail(pdu));

    uint8_t *container_uid;

    switch (action) {

    case ACTION_LOAD:

        DEBUG("[CoAP] Loading a container...\n");

        /* check the CoAP method */
        if (method_flag != COAP_POST) {
            DEBUG("[CoAP] WARN: invalid CoAP method!\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_METHOD_NOT_ALLOWED);
        }

        /* retrieve container parts */
        uint8_t *metadata;
        ssize_t metadata_size = coap_opt_get_opaque(pdu, OPTION_METADATA, &metadata);
        uint8_t *data;
        ssize_t data_size = coap_opt_get_opaque(pdu, OPTION_DATA, &data);
        uint8_t *code;
        ssize_t code_size = coap_opt_get_opaque(pdu, OPTION_CODE, &code);
        if (metadata_size < 0 || data_size < 0 || code_size <= 0) {
            DEBUG("[CoAP] WARN: metadata or data or code are missing!\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_BAD_OPTION);
        }

        /* try to load the container */
        if (tinycontainer_loadcontainer(metadata, metadata_size, data, data_size, code,
                                        code_size) == true) {
            /* success */
            DEBUG("[CoAP] new container loaded!\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_CREATED);
        }
        else {
            /* failure */
            DEBUG("[CoAP] WARN: container loading failure!\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
        }

    case ACTION_START:

        DEBUG("[CoAP] Starting a container...\n");

        /* check the CoAP method */
        if (method_flag != COAP_PUT) {
            DEBUG("[CoAP] WARN: invalid CoAP method!\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_METHOD_NOT_ALLOWED);
        }

        /* retrieve container uid */
        ret = coap_opt_get_opaque(pdu, OPTION_UID, &container_uid);
        if (ret < 0) {
            DEBUG("[CoAP] WARN: container uid is missing!\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_BAD_OPTION);
        }
        DEBUG("[CoAP] INFO: uid:%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
              container_uid[0],
              container_uid[1],
              container_uid[2],
              container_uid[3],
              container_uid[4],
              container_uid[5],
              container_uid[6],
              container_uid[7],
              container_uid[8],
              container_uid[9],
              container_uid[10],
              container_uid[11],
              container_uid[12],
              container_uid[13],
              container_uid[14],
              container_uid[15],
              container_uid[16]);

        /* try to start the container */
        if (tinycontainer_startcontainer(container_uid, ret) == true) {
            /* success */
            DEBUG("[CoAP] container started!\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_CHANGED);
        }
        else {
            /* failure */
            DEBUG("[CoAP] WARN: container starting failure!\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
        }

    case ACTION_IS_RUNNING:

        DEBUG("[CoAP] Checking container status...\n");

        /* check the CoAP method */
        if (method_flag != COAP_GET) {
            DEBUG("[CoAP] WARN: invalid CoAP method!\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_METHOD_NOT_ALLOWED);
        }

        /* retrieve container uid */
        ret = coap_opt_get_opaque(pdu, OPTION_UID, &container_uid);
        if (ret < 0) {
            DEBUG("[CoAP] WARN: container uid is missing!\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_BAD_OPTION);
        }

        /* ask for container status */
        gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
        if (tinycontainer_iscontainerrunning(container_uid, ret) == true) {
            /* container is up */
            DEBUG("[CoAP] container is running\n");
            coap_opt_add_uint(pdu, OPTION_STATUS, 1);
        }
        else {
            /* container is down */
            DEBUG("[CoAP] container is NOT running\n");
            coap_opt_add_uint(pdu, OPTION_STATUS, 0);
        }
        return coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);

    case ACTION_STOP:

        /* check the CoAP method */
        if (method_flag != COAP_PUT) {
            DEBUG("[CoAP] WARN: invalid CoAP method!\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_METHOD_NOT_ALLOWED);
        }

        /* no yet implemented */
        DEBUG("[CoAP] WARN: not yet implemented!\n");
        return gcoap_response(pdu, buf, len, COAP_CODE_NOT_IMPLEMENTED);

    case ACTION_DELETE:

        /* check the CoAP method */
        if (method_flag != COAP_DELETE) {
            DEBUG("[CoAP] WARN: invalid CoAP method!\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_METHOD_NOT_ALLOWED);
        }

        /* no yet implemented */
        DEBUG("[CoAP] WARN: not yet implemented!\n");
        return gcoap_response(pdu, buf, len, COAP_CODE_NOT_IMPLEMENTED);

    default:

        DEBUG("[CoAP] WARN: invalid value for action CoAP option!\n");
        return gcoap_response(pdu, buf, len, COAP_CODE_BAD_OPTION);
    }

    /* this line is never reached */
}

ssize_t _device_id_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, coap_request_ctx_t *context)
{

    (void)context;

    DEBUG("[COAP] Device id Handler\n");

    unsigned method_flag = coap_method2flag(coap_get_code_detail(pdu));

    switch (method_flag) {

    case COAP_GET:

        DEBUG("[COAP] Reading the device ID...\n");

        if (config.device_id_size == 0) {
            DEBUG("[COAP] WARN: not yet assigned\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_PATH_NOT_FOUND);
        }

        gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
        coap_opt_add_format(pdu, COAP_FORMAT_OCTET);
        size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

        if (pdu->payload_len >= config.device_id_size) {
            DEBUG("[COAP] Device ID added to CoAP message Response\n");
            memcpy(pdu->payload, config.device_id, config.device_id_size);
            return resp_len + config.device_id_size;
        }
        else {
            DEBUG("[COAP] WARN: more space in required in the CoAP payload!\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
        }

        break;

    case COAP_POST:

        DEBUG("[COAP] Writing the device ID...\n");

        if (pdu->payload_len == 0) {
            DEBUG("[COAP] WARN: zero length device ID!\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_BAD_REQUEST);
        }
        else if (pdu->payload_len > DEVICE_ID_MAX_SIZE) {
            DEBUG("[COAP] WARN: Device ID is too large!\n");
            gcoap_resp_init(pdu, buf, len, COAP_CODE_REQUEST_ENTITY_TOO_LARGE);
            coap_opt_add_uint(pdu, COAP_OPT_SIZE1, DEVICE_ID_MAX_SIZE);
            return coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
        }
        else {
            memcpy(config.device_id, pdu->payload, pdu->payload_len);
            config.device_id_size = pdu->payload_len;
            DEBUG("[COAP] Device ID updated!\n");
            return gcoap_response(pdu, buf, len, COAP_CODE_CHANGED);
        }

        break;

    default:

        DEBUG("[COAP] WARN: unsupported method!\n");
        return gcoap_response(pdu, buf, len, COAP_CODE_BAD_OPTION);
    }
}

/* CoAP resources. Must be sorted by path (ASCII order). */
static const coap_resource_t _resources[] = {
    { "/container/", COAP_GET | COAP_POST | COAP_PUT | COAP_DELETE, _container_handler, NULL },
    { "/device-id/", COAP_GET | COAP_POST, _device_id_handler, NULL },
};

static gcoap_listener_t _listener = {
    &_resources[0],
    ARRAY_SIZE(_resources),
    GCOAP_SOCKET_TYPE_UNDEF,
    NULL,
    NULL,
    NULL,
};

/* public function */

void coap_server_init(void)
{
    /* note: we don't check the returned value of the next functions : it is
     * called once when the program started and we have tested the program on supported
     * devices.
     */
    gcoap_init();
    gcoap_register_listener(&_listener);
    DEBUG("[COAP] Now, serving registered CoAP endpoints\n");
}

#endif /* MODULE_GCOAP */
