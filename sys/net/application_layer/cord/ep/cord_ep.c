/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_cord_ep
 * @{
 *
 * @file
 * @brief       CoRE Resource Directory endpoint implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>

#include "mutex.h"
#include "assert.h"
#include "thread_flags.h"

#include "clif.h"
#include "net/gcoap.h"
#include "net/ipv6/addr.h"
#include "net/cord/ep.h"
#include "net/cord/common.h"
#include "net/cord/config.h"

#ifdef MODULE_CORD_EP_STANDALONE
#include "net/cord/ep_standalone.h"
#endif

#define ENABLE_DEBUG        0
#include "debug.h"

static void _post_state(cord_ep_ctx_t *cord, cord_state_t state)
{
    cord->state = state;
    /* submit event */
    event_post(cord->queue, &cord->state_update);
}

static void _post_retry(cord_ep_ctx_t *cord, cord_state_t state, uint32_t timeout)
{
    cord->state = state;
    /* submit event */
    event_timeout_set(&cord->retry_timer, timeout);
}

static void _post_refresh(cord_ep_ctx_t *cord, cord_state_t state)
{
    _post_retry(cord, state, CONFIG_CORD_LT);
}

static void _clear_rd_location(cord_ep_ctx_t *cord)
{
    memset(&cord->rd_loc, 0, CONFIG_CORD_URI_MAX);
}

static void _clear_regif(cord_ep_ctx_t *cord)
{
    memset(&cord->rd_regif, 0, CONFIG_CORD_URI_MAX);
}

static int _set_rd_settings(cord_ep_ctx_t *cord, const sock_udp_ep_t *remote, const char *regif)
{
    memcpy(&cord->rd_remote, remote, sizeof(cord->rd_remote));
    if (regif) {
        if (strlen(regif) < sizeof(cord->rd_regif)) {
            strcpy(cord->rd_regif, regif);
        }
        else {
            return CORD_EP_OVERFLOW;
        }
    }
    return CORD_EP_OK;
}

static void _on_delete(const gcoap_request_memo_t *memo, coap_pkt_t *pdu,
                       const sock_udp_ep_t *remote)
{
    if (req_state == GCOAP_MEMO_RESP) {
        /* Either the registration was succesfully deleted, or there was no
         * registration in the first place */
        _clear_rd_location(cord);
        _post_state(cord, CORD_STATE_IDLE);
        return;
    }
    /* Retry if the server is temporarily unavailable */
    _post_retry(cord, CORD_STATE_DELETE, CONFIG_CORD_FAIL_RETRY_SEC);
}

static void _on_update(const gcoap_request_memo_t *memo, coap_pkt_t *pdu,
                       const sock_udp_ep_t *remote)
{
    cord_ep_ctx_t *cord = memo->context;
    unsigned req_state = memo->state;
    (void)remote;

    if (req_state == GCOAP_MEMO_RESP) {
        if (pdu->hdr->code == COAP_CODE_CHANGED) {
            _post_refresh(cord, CORD_STATE_WAIT_REFRESH);
            return;
        }
        else if (coap_get_code_class(pdu) == COAP_CLASS_CLIENT_FAILURE) {
            _clear_rd_location(cord);
            /* reset to registration */
            _post_state(cord, CORD_STATE_REGISTRY);
            return;
        }
    }
    _post_retry(cord, CORD_STATE_WAIT_RETRY, CONFIG_CORD_FAIL_RETRY_SEC);
}

static int _send_update_request_with_code(cord_ep_ctx_t *cord, unsigned code, gcoap_resp_handler_t handle)
{
    coap_pkt_t pkt;

    if (cord->rd_loc[0] == 0) {
        return CORD_EP_NO_RD;
    }

    /* build CoAP request packet */
    int res = gcoap_req_init(&pkt, cord->buf, CONFIG_CORD_EP_BUFSIZE, code, cord->rd_loc);
    if (res < 0) {
        return CORD_EP_ERR;
    }
    coap_hdr_set_type(pkt.hdr, COAP_TYPE_CON);
    ssize_t pkt_len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);

    /* send request */
    ssize_t send_len = gcoap_req_send(cord->buf, pkt_len, &cord->rd_remote, handle, cord);
    if (send_len <= 0) {
        return CORD_EP_ERR;
    }
    return 0;
}

static void _on_discover(const gcoap_request_memo_t *memo, coap_pkt_t *pdu,
                         const sock_udp_ep_t *remote)
{
    cord_ep_ctx_t *cord = memo->context;
    (void)remote;

    if (memo->state == GCOAP_MEMO_RESP) {
        unsigned ct = coap_get_content_type(pdu);
        if (ct != COAP_FORMAT_LINK) {
            goto end;
        }
        if (pdu->payload_len == 0) {
            goto end;
        }

        clif_t link;
        ssize_t res = clif_decode_link(&link, NULL, 0, (char*)pdu->payload, pdu->payload_len);
        if (res > 0) {
            /* TODO: verify size of interface resource identifier */
            if (link.target_len >= sizeof(cord->rd_regif)) {
                goto end;
            }
            memcpy(cord->rd_regif, link.target, link.target_len);
            cord->rd_regif[link.target_len] = '\0';

            _post_state(cord, CORD_STATE_REGISTRY);
            return;
        }
    }
end:
    /* retry later */
    _post_retry(cord, CORD_STATE_DISCOVERY, CONFIG_CORD_FAIL_RETRY_SEC);
    return;
}

static int _start_discovery(cord_ep_ctx_t *cord)
{
    coap_pkt_t pkt;
    cord->state = CORD_STATE_DISCOVERING;
    /* do URI discovery for the registration interface */
    int res = gcoap_req_init(&pkt, cord->buf, CONFIG_CORD_EP_BUFSIZE, COAP_METHOD_GET,
                             "/.well-known/core");
    if (res < 0) {
        return CORD_EP_ERR;
    }
    coap_hdr_set_type(pkt.hdr, COAP_TYPE_CON);
    coap_opt_add_uri_query(&pkt, "rt", "core.rd");
    size_t pkt_len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);
    res = gcoap_req_send(cord->buf, pkt_len, &cord->rd_remote, _on_discover, cord);
    if (res <= 0) {
        _post_retry(cord, CORD_STATE_DISCOVERY, CONFIG_CORD_FAIL_RETRY_SEC);
        return CORD_EP_ERR;
    }
    return 0;
}

static void _on_register(const gcoap_request_memo_t *memo, coap_pkt_t* pdu,
                         const sock_udp_ep_t *remote)
{
    cord_ep_ctx_t *cord = memo->context;

    if (memo->state == GCOAP_MEMO_RESP) {
        /* use the remote used to respond with as new sock remote */
        memcpy(&cord->rd_remote, remote, sizeof(cord->rd_remote));
        if (pdu->hdr->code == COAP_CODE_CREATED) {
            /* read the location header and save the RD details on success */
            if (coap_get_location_path(pdu, (uint8_t *)cord->rd_loc,
                                       sizeof(cord->rd_loc)) > 0) {
                _post_refresh(cord, CORD_STATE_WAIT_REFRESH);
                return;
            }
            /* if copy fails cascade through to retrying later again */
        }
        else if (coap_get_code_class(pdu) == COAP_CLASS_CLIENT_FAILURE) {
            _clear_regif(cord);
            /* Retry discovery later */
            _post_retry(cord, CORD_STATE_DISCOVERY, CONFIG_CORD_FAIL_RETRY_SEC);
            return;
        }
    }
    /* Retry registration after some time if other failure */
    _post_retry(cord, CORD_STATE_REGISTRY, CONFIG_CORD_FAIL_RETRY_SEC);
}

static int  _start_registration(cord_ep_ctx_t *cord)
{
    int res;
    cord->state = CORD_STATE_REGISTERING;
    ssize_t pkt_len;
    int retval;
    coap_pkt_t pkt;

    /* build and send CoAP POST request to the RD's registration interface */
    res = gcoap_req_init(&pkt, cord->buf, CONFIG_CORD_EP_BUFSIZE, COAP_METHOD_POST, cord->rd_regif);
    if (res < 0) {
        retval = CORD_EP_ERR;
        goto end;
    }
    /* set hdr type and content format and write query string */
    coap_hdr_set_type(pkt.hdr, COAP_TYPE_CON);
    coap_opt_add_uint(&pkt, COAP_OPT_CONTENT_FORMAT, COAP_FORMAT_LINK);
    res = cord_common_add_qstring(&pkt);
    if (res < 0) {
        retval = CORD_EP_ERR;
        goto end;
    }

    pkt_len = coap_opt_finish(&pkt, COAP_OPT_FINISH_PAYLOAD);

    /* add the resource description as payload */
    res = gcoap_get_resource_list(pkt.payload, pkt.payload_len,
                                  COAP_FORMAT_LINK, GCOAP_SOCKET_TYPE_UNDEF);
    if (res < 0) {
        retval = CORD_EP_ERR;
        goto end;
    }
    pkt_len += res;

    /* send out the request */
    res = gcoap_req_send(cord->buf, pkt_len, &cord->rd_remote, _on_register, cord);
    if (res > 0) {
        /* wait for gcoap result */
        return 0;
    }
    retval = CORD_EP_ERR;
end:
    /* retry registration later on */
    _post_retry(cord, CORD_STATE_REGISTRY, CONFIG_CORD_FAIL_RETRY_SEC);
    return retval;
}

int cord_ep_register(cord_ep_ctx_t *cord, const sock_udp_ep_t *remote, const char *regif)
{
    assert(remote);
    int res = CORD_EP_OK;
    mutex_lock(&cord->lock);
    cord_state_t state = cord->state;
    if (state == CORD_STATE_IDLE) {
        /* TODO: dedup */
        /* start registration or discovery at remote */
        res = _set_rd_settings(cord, remote, regif);
        if (res == CORD_EP_OK) {
            if (regif) {
                _post_state(cord, CORD_STATE_REGISTRY);
            }
            else {
                _post_state(cord, CORD_STATE_DISCOVERY);
            }
        }
    }
    else {
        res = CORD_EP_BUSY;
    }
    mutex_unlock(&cord->lock);
    return res;
}

int cord_ep_update(cord_ep_ctx_t *cord)
{
    int res = CORD_EP_BUSY;
    mutex_lock(&cord->lock);
    if (cord->state == CORD_STATE_WAIT_REFRESH) {
        event_timeout_clear(&cord->retry_timer);
        _post_state(cord, CORD_STATE_WAIT_REFRESH);
        res = CORD_EP_OK;
    }
    mutex_unlock(&cord->lock);
    return res;
}

int cord_ep_remove(cord_ep_ctx_t *cord)
{
    mutex_lock(&cord->lock);
    event_timeout_clear(&cord->retry_timer);
    mutex_unlock(&cord->lock);
    return CORD_EP_OK;
}

static void _on_state_update(event_t *event)
{
    cord_ep_ctx_t *cord = container_of(event, cord_ep_ctx_t, state_update);
    mutex_lock(&cord->lock);
    switch (cord->state) {
        case CORD_STATE_IDLE:
            /* Do nothing */
            break;
        case CORD_STATE_DISCOVERY:
            _start_discovery(cord);
            break;
        case CORD_STATE_REGISTRY:
            _start_registration(cord);
            break;
        case CORD_STATE_WAIT_REFRESH:
        case CORD_STATE_WAIT_RETRY:
            /* Send refresh */
            _send_update_request_with_code(cord, COAP_METHOD_POST, _on_update);
            break;
        case CORD_STATE_DELETE:
            _send_update_request_with_code(cord, COAP_METHOD_DELETE, _on_delete);
        default:
            DEBUG("[CORD] State: %d\n", cord->state);
            assert(false);
    }
    mutex_unlock(&cord->lock);
}

int cord_ep_init(cord_ep_ctx_t *cord, event_queue_t *queue, const sock_udp_ep_t *remote, const char *regif)
{
    assert(remote);

    memset(cord, 0, sizeof(cord_ep_ctx_t));
    cord->queue = queue;
    cord->state_update.handler = _on_state_update;
    event_timeout_ztimer_init(&cord->retry_timer, ZTIMER_SEC, queue, &cord->state_update);

    int res = _set_rd_settings(cord, remote, regif);
    if (remote && regif) {
        _post_state(cord, CORD_STATE_REGISTRY);
    } else {
        _post_state(cord, CORD_STATE_DISCOVERY);
    }
    /* wait for registration signal */
    return res;
}

void cord_ep_dump_status(const cord_ep_ctx_t *cord)
{
    puts("CoAP RD connection status:");

    if (cord->rd_loc[0] == 0) {
        puts("  --- not registered with any RD ---");
    }
    else {
        /* get address string */
        char addr[IPV6_ADDR_MAX_STR_LEN];
        ipv6_addr_to_str(addr, (ipv6_addr_t *)&cord->rd_remote.addr, sizeof(addr));

        printf("RD address: coap://[%s]:%"PRIu16"\n", addr, cord->rd_remote.port);
        printf("   ep name: %s\n", cord_common_get_ep());
        printf("  lifetime: %is\n", (int)CONFIG_CORD_LT);
        printf("    reg if: %s\n", cord->rd_regif);
        printf("  location: %s\n", cord->rd_loc);
    }
}
