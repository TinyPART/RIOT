/*
 * Copyright (C) 2024 Orange
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
 * @brief       TinyContainer Metadata sub-module implementation
 *
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#define ENABLE_DEBUG 0

/* from riot */
#include "debug.h"

/* from external riot package */
#include "nanocbor/nanocbor.h"

/* from this module */
#include "tinycontainer/metadata/metadata_memmgr.h"
#include "metadata_cbor.h"

/* functions used locally
 * ======================
 */

/* parse a cbor key-value couple. The value shall be a bstr */
int _parse_key_bstr(nanocbor_value_t *data,
                    int32_t *key,
                    const uint8_t **value,
                    size_t *value_len)
{
    if (nanocbor_get_int32(data, key) < 0) {
        DEBUG("Could not read key");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }
    if (nanocbor_get_bstr(data, value, value_len) < 0) {
        DEBUG("Could not read bstr value");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    return METADATA_OK;
}

/* public functions
 * ================
 */

/* Parse a Metadata object
 *
 *     metadata-tag = #6.6082514239057121876(metadata)
 *     metadata = {
 *         metadata-container => bstr .cbor container
 *         ? metadata-endpoints => bstr .cbor endpoints
 *         ? metadata-security => bstr .cbor security
 *     }
 *
 */
int metadata_parse(metadata_t *metadata,
                   const uint8_t *buf,
                   size_t len)
{
    nanocbor_value_t it;

    /* saved the raw data structure */
    metadata->raw_cbor = buf;
    metadata->raw_cbor_len = len;

    /* initialize the cbor decoder */
    nanocbor_decoder_init(&it, buf, len);

    /* parse tinycontainer tag */
    uint64_t tag;

    if (nanocbor_get_tag64(&it, &tag) < 0 || tag != METADATA_TAG) {
        DEBUG("Metadata tag not found");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    /* parse the map */
    nanocbor_value_t map;

    if (nanocbor_enter_map(&it, &map) < 0) {
        DEBUG("Metadata map not found");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    while (!nanocbor_at_end(&map)) {
        int32_t key;
        const uint8_t *value;
        size_t value_len;

        /* parse the key */
        if (_parse_key_bstr(&map, &key, &value, &value_len) < 0) {
            DEBUG("Could not read key and value from metadata map");
            return METADATA_INVALID_CBOR_STRUCTURE;
        }

        /* and the value */
        switch (key) {
        case METADATA_CONTAINER:
            metadata->container = value;
            metadata->container_len = value_len;
            break;

        case METADATA_ENDPOINTS:
            metadata->endpoints = value;
            metadata->endpoints_len = value_len;
            break;

        case METADATA_SECURITY:
            metadata->security = value;
            metadata->security_len = value_len;
            break;

        default:
            /* we don't accept unknown keys */
            DEBUG("Metadata map contain an unknown key");
            return METADATA_INVALID_CBOR_STRUCTURE;
        }
    }

    nanocbor_leave_container(&it, &map);

    return METADATA_OK;
}

/* Parse a Container Metadata object
 *
 *     container = {
 *         container-uid => bstr
 *         container-type => interpreter-types
 *         container-syscall-mask => bstr .cbor cwt
 *     }
 */
int metadata_container_parse(metadata_container_t *metadata_container,
                             const uint8_t *buf,
                             size_t len)
{
    nanocbor_value_t it;

    /* internal state */
    struct {
        unsigned char uid : 1;
        unsigned char type : 1;
        unsigned char syscall : 1;
    } has;

    /* saved the raw data structure */
    metadata_container->raw_cbor = buf;
    metadata_container->raw_cbor_len = len;

    /* initialize the cbor decoder */
    nanocbor_decoder_init(&it, buf, len);

    /* parse the map */
    nanocbor_value_t map;

    if (nanocbor_enter_map(&it, &map) < 0) {
        DEBUG("container map not found");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    while (!nanocbor_at_end(&map)) {
        int32_t key;

        /* parse the key */
        if (nanocbor_get_int32(&map, &key) < 0) {
            DEBUG("Could not read key and value from container map");
            return METADATA_INVALID_CBOR_STRUCTURE;
        }

        /* and the value */
        switch (key) {
        case METADATA_CONTAINER_UID:
            if (nanocbor_get_bstr(&map,
                                  &metadata_container->uid,
                                  &metadata_container->uid_len) < 0) {
                DEBUG("Could not get uid value from container map");
                return METADATA_INVALID_CBOR_STRUCTURE;
            }
            has.uid = 1;
            break;

        case METADATA_CONTAINER_TYPE:
            if (nanocbor_get_uint8(&map, &metadata_container->type) < 0) {
                DEBUG("Could not get type value from container map");
                return METADATA_INVALID_CBOR_STRUCTURE;
            }
            has.type = 1;
            break;

        case METADATA_CONTAINER_SYSCALL_MASK:
            if (nanocbor_get_bstr(&map,
                                  &metadata_container->syscall_mask,
                                  &metadata_container->syscall_mask_len) < 0) {
                DEBUG("Could not get syscall value from container map");
                return METADATA_INVALID_CBOR_STRUCTURE;
            }
            has.syscall = 1;
            break;

        default:
            /* we don't accept unknown keys */
            DEBUG("Container map contain an unknown key");
            return METADATA_INVALID_CBOR_STRUCTURE;
        }
    }

    nanocbor_leave_container(&it, &map);

    /* check if the container is valid */
    if (!has.uid || !has.type || !has.syscall) {
        DEBUG("Invalid container map object");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    return METADATA_OK;
}

/* Parse a Endpoints object
 *
 *     endpoints = [ * endpoint ]
 *
 *     endpoint = {
 *         endpoint-id => int
 *         peer-type => &peer-types
 *         peer-endpoint-id => int
 *         direction => &directions
 *         ? endpoint-token => bstr .cbor cwt
 *     }
 *
 *     endpoint-id = 1
 *     peer-type = 2
 *     peer-types = ( peer-type-container: 0,
 *                    peer-type-local: 1,
 *                    peer-type-remote: 2 )
 *     peer-uid = 3
 *     peer-endpoint-id = 4
 *     direction = 5
 *     directions = ( in: 0, out: 1, both: 2 )
 *     endpoint-token = 6
 */

/* parse endpoints array until founding endpoint_id.
 */
static int parse_and_search_endpoints(
    metadata_endpoints_t *metadata_endpoints,
    const uint8_t *buf, size_t len,
    metadata_endpoint_t *endpoint, uint32_t endpoint_id)
{
    /* internal state */
    struct {
        unsigned char id : 1;
        unsigned char type : 1;
        unsigned char peer_uid : 1;
        unsigned char peer_endpoint_id : 1;
        unsigned char direction : 1;
        unsigned char token : 1;
    } has;

    nanocbor_value_t it;

    /* saved the raw data structure */
    metadata_endpoints->raw_cbor = buf;
    metadata_endpoints->raw_cbor_len = len;

    /* initialize the cbor decoder */
    nanocbor_decoder_init(&it, buf, len);

    /* parse the array */
    nanocbor_value_t array;

    if (nanocbor_enter_array(&it, &array) < 0) {
        DEBUG("array of endpoints not found");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    while (!nanocbor_at_end(&array)) {
        metadata_endpoint_t ep;

        /* parse the endpoint map*/
        nanocbor_value_t map;

        if (nanocbor_enter_map(&array, &map) < 0) {
            DEBUG("map of endpoint not found");
            return METADATA_INVALID_CBOR_STRUCTURE;
        }

        has.id = 0;
        has.type = 0;
        has.peer_uid = 0;
        has.peer_endpoint_id = 0;
        has.direction = 0;
        has.token = 0;

        while (!nanocbor_at_end(&map)) {
            int32_t key;
            const uint8_t *peer_uid;
            size_t peer_uid_len;

            /* parse the key */
            if (nanocbor_get_int32(&map, &key) < 0) {
                DEBUG("Could not read key and value from container map");
                return METADATA_INVALID_CBOR_STRUCTURE;
            }

            /* and the value */
            switch (key) {
            case METADATA_ENDPOINT_ID:
                if (has.id || nanocbor_get_uint32(&map, &ep.id) < 0) {
                    DEBUG("Could not get id value from endpoint map");
                    return METADATA_INVALID_CBOR_STRUCTURE;
                }

                has.id = 1;
                break;

            case METADATA_ENDPOINT_TYPE:
                if (has.type ||
                    nanocbor_get_uint8(&map, &ep.peer_type) < 0) {
                    DEBUG("Could not get type value from endpoint map");
                    return METADATA_INVALID_CBOR_STRUCTURE;
                }

                if (ep.peer_type != METADATA_PEER_CONTAINER &&
                    ep.peer_type != METADATA_PEER_LOCAL &&
                    ep.peer_type != METADATA_PEER_REMOTE) {
                    DEBUG("Invalid peer type value from endpoint map");
                    return METADATA_INVALID_CBOR_STRUCTURE;
                }

                has.type = 1;
                break;

            case METADATA_ENDPOINT_PEER_UID:
                if (has.peer_uid
                    || nanocbor_get_bstr(&map, &peer_uid, &peer_uid_len) < 0) {
                    DEBUG("Could not get peer id hex string from endpoint map");
                    return METADATA_INVALID_CBOR_STRUCTURE;
                }

                has.peer_uid = 1;
                break;

            case METADATA_ENDPOINT_PEER_ENDPOINT_ID:
                if (has.peer_endpoint_id ||
                    nanocbor_get_uint32(&map, &ep.peer_endpoint_id) < 0) {
                    DEBUG("Could not get peer endpoint id value from "
                          "endpoint map");
                    return METADATA_INVALID_CBOR_STRUCTURE;
                }

                has.peer_endpoint_id = 1;
                break;

            case METADATA_ENDPOINT_DIRECTION:
                if (has.direction ||
                    nanocbor_get_uint8(&map, &ep.direction) < 0) {
                    DEBUG("Could not get direction value from endpoint map");
                    return METADATA_INVALID_CBOR_STRUCTURE;
                }

                if (ep.direction != METADATA_DIRECTION_IN &&
                    ep.direction != METADATA_DIRECTION_OUT &&
                    ep.direction != METADATA_DIRECTION_BOTH) {
                    DEBUG("Invalid direction value from endpoint map");
                    return METADATA_INVALID_CBOR_STRUCTURE;
                }

                has.direction = 1;
                break;

            default:
                /* we don't accept unknown keys */
                DEBUG("Endpoint map contain an unknown key");
                return METADATA_INVALID_CBOR_STRUCTURE;
            }
        }

        nanocbor_leave_container(&array, &map);

        /* check if all mandatory fields of the endpoint map is set */
        if (!has.id || !has.type ||
            (ep.peer_type == METADATA_PEER_CONTAINER  && !has.peer_uid) ||
            !has.peer_endpoint_id || !has.direction) {
            DEBUG("Invalid endpoint map");
            return METADATA_INVALID_CBOR_STRUCTURE;
        }

        if (endpoint != NULL && endpoint_id == ep.id) {
            /* found the endpoint */
            *endpoint = ep;
            return METADATA_OK;
        }
    }

    nanocbor_leave_container(&it, &array);

    if (endpoint != NULL) {
        return METADATA_NOT_FOUND;
    }
    else {
        return METADATA_OK;
    }
}

int metadata_endpoints_parse(metadata_endpoints_t *metadata_endpoints,
                             const uint8_t *buf, size_t len)
{
    return parse_and_search_endpoints(metadata_endpoints, buf, len, NULL, 0);
}

int metadata_endpoints_search(metadata_endpoint_t *endpoint,
                              const uint8_t *buf, size_t len,
                              uint32_t endpoint_id)
{
    metadata_endpoints_t endpoints;

    return parse_and_search_endpoints(&endpoints, buf, len,
                                      endpoint, endpoint_id);
}

int metadata_security_parse(metadata_security_t *metadata_security,
                            const uint8_t *buf,
                            size_t len)
{
    nanocbor_value_t it;

    /* saved the raw data structure */
    metadata_security->raw_cbor = buf;
    metadata_security->raw_cbor_len = len;

    /* initialize the cbor decoder */
    nanocbor_decoder_init(&it, buf, len);

    /* parse the array */
    nanocbor_value_t array;

    if (nanocbor_enter_array(&it, &array) < 0) {
        DEBUG("security array not found!");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    /* parse the max duration for the on_start() function */
    uint32_t *pvalue = &metadata_security->start_max_duration;
    if (nanocbor_get_uint32(&array, pvalue) < 0) {
        DEBUG("start-max-duration not found!");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    /* parse the period at which the on_loop() function is called */
    pvalue = &metadata_security->loop_period;
    if (nanocbor_get_uint32(&array, pvalue) < 0) {
        DEBUG("loop-period not found!");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    /* parse the max duration for the on_loop() function */
    pvalue = &metadata_security->loop_max_duration;
    if (nanocbor_get_uint32(&array, pvalue) < 0) {
        DEBUG("loop-max-duration not found!");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    /* parse the max lifetime for the on_loop() function */
    pvalue = &metadata_security->loop_max_lifetime;
    if (nanocbor_get_uint32(&array, pvalue) < 0) {
        DEBUG("loop-max-lifetime not found!");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    /* parse the max duration for the on_stop() function */
    pvalue = &metadata_security->stop_max_duration;
    if (nanocbor_get_uint32(&array, pvalue) < 0) {
        DEBUG("stop-max-duration not found!");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    /* parse the data-token */
    const uint8_t **cwt = &metadata_security->cwt[1];
    size_t *cwt_size = &metadata_security->cwt_size[1];
    if (nanocbor_get_bstr(&array, cwt, cwt_size) < 0) {
        DEBUG("data token not found!");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    /* parse the code-token */
    cwt = &metadata_security->cwt[2];
    cwt_size = &metadata_security->cwt_size[2];
    if (nanocbor_get_bstr(&array, cwt, cwt_size) < 0) {
        DEBUG("code token not found!");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    /* parse the metadata-token */
    cwt = &metadata_security->cwt[0];
    cwt_size = &metadata_security->cwt_size[0];
    if (nanocbor_get_bstr(&array, cwt, cwt_size) < 0) {
        DEBUG("metadata token not found!");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    if (!nanocbor_at_end(&array)) {
        DEBUG("unknown security element!");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    return METADATA_OK;
}
