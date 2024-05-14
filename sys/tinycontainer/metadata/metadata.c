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
    if(nanocbor_get_int32(data, key) < 0) {
        DEBUG("Could not read key");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }
    if(nanocbor_get_bstr(data, value, value_len) < 0) {
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
    metadata -> raw_cbor = buf;
    metadata -> raw_cbor_len = len;

    /* initialize the cbor decoder */
    nanocbor_decoder_init(&it, buf, len);

    /* parse tinycontainer tag */
    uint64_t tag;
    if(nanocbor_get_tag64(&it, &tag) < 0 || tag != METADATA_TAG) {
        DEBUG("Metadata tag not found");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    /* parse the map */
    nanocbor_value_t map;
    if(nanocbor_enter_map(&it, &map) < 0) {
        DEBUG("Metadata map not found");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    while (!nanocbor_at_end(&map)) {
        int32_t key;
        const uint8_t * value;
        size_t value_len;

        /* parse the key */
        if(_parse_key_bstr(&map, &key, &value, &value_len) < 0) {
            DEBUG("Could not read key and value from metadata map");
            return METADATA_INVALID_CBOR_STRUCTURE;
        }

        /* and the value */
        switch (key) {
            case METADATA_CONTAINER:
                metadata -> container = value;
                metadata -> container_len = value_len;
                break;

            case METADATA_ENDPOINTS:
                DEBUG("Metadata endpoints is not yet implemented");
                return METADATA_INVALID_CBOR_STRUCTURE;

            case METADATA_SECURITY:
                DEBUG("Metadata security is not yet implemented");
                return METADATA_INVALID_CBOR_STRUCTURE;

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
        unsigned char uid:1;
        unsigned char type:1;
        unsigned char syscall:1;
    } has;

    /* saved the raw data structure */
    metadata_container -> raw_cbor = buf;
    metadata_container -> raw_cbor_len = len;

    /* initialize the cbor decoder */
    nanocbor_decoder_init(&it, buf, len);

    /* parse the map */
    nanocbor_value_t map;
    if(nanocbor_enter_map(&it, &map) < 0) {
        DEBUG("container map not found");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    while (!nanocbor_at_end(&map)) {
        int32_t key;

        /* parse the key */
        if(nanocbor_get_int32(&map, &key) < 0) {
            DEBUG("Could not read key and value from container map");
            return METADATA_INVALID_CBOR_STRUCTURE;
        }

        /* and the value */
        switch (key) {
            case METADATA_CONTAINER_UID:
                if(nanocbor_get_bstr(&map, &metadata_container->uid, &metadata_container->uid_len) < 0) {
                    DEBUG("Could not get uid value from container map");
                    return METADATA_INVALID_CBOR_STRUCTURE;
                }
                has.uid = 1;
                break;

            case METADATA_CONTAINER_TYPE:
                if(nanocbor_get_uint8(&map, &metadata_container->type) < 0) {
                    DEBUG("Could not get type value from container map");
                    return METADATA_INVALID_CBOR_STRUCTURE;
                }
                has.type = 1;
                break;

            case METADATA_CONTAINER_SYSCALL_MASK:
                if(nanocbor_get_bstr(&map, &metadata_container->uid, &metadata_container->syscall_len) < 0) {
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
    if ( !has.uid || !has.type || !has.syscall) {
        DEBUG("Invalid container map object");
        return METADATA_INVALID_CBOR_STRUCTURE;
    }

    return METADATA_OK;
}

/* Parse a Endpoints Metadata object
 * 
 *     endpoints = [ * endpoint ]
 *     endpoint = {
 *         endpoint-id => int
 *         endpoint-peer => bstr .cbor one-peer
 *         ? endpoint-token => bstr .cbor cwt
 *     }
 */
int metadata_endpoints_parse(metadata_endpoints_t *metadata,
                             const uint8_t *buf,
                             size_t len)
{
    (void)metadata;
    (void)buf;
    (void)len;

    //TOTO: not yet implemented
    return METADATA_INVALID_CBOR_STRUCTURE;
}

int metadata_security_parse(metadata_security_t *metadata,
                            const uint8_t *buf,
                            size_t len)
{
    (void)metadata;
    (void)buf;
    (void)len;

    //TOTO: not yet implemented
    return METADATA_INVALID_CBOR_STRUCTURE;
}

