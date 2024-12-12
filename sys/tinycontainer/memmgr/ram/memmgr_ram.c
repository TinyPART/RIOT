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
 * @brief       TinyContainer RAM Memory Manager sub-module implementation
 *
 * @author      Samuel legouix <samuel.legouix@orange.com>
 * @author      Gregory Holder <gregory.holder76@gmail.com>
 *
 * @}
 */

#include <string.h>
#include <stddef.h>

#include "tinycontainer/memmgr/memmgr.h"
#include "tinycontainer/memmgr/memmgr_controller.h"
#include "tinycontainer/memmgr/memmgr_runtime.h"
#include "tinycontainer/service/service_memmgr.h"
#include "tinycontainer/metadata/metadata_memmgr.h"
#include "tinycontainer/security/cwt/cwt.h"
#include "tinycontainer/security/crypto/crypto.h"
#include "nanocbor/nanocbor.h"

#include "memmgr_ram.h"

/* allocate memory for the containers and their descriptors */
static struct container containers[CONTAINER_MAX_NUM] = { 0 };
static struct descriptor descriptors[CONTAINER_MAX_NUM] = { 0 };

static uint8_t *_get_meta_start(struct container *container)
{
    return container->meta;
}

static uint32_t _get_meta_max_size(struct container *container)
{
    (void)container;
    return META_DATA_SIZE;
}

static uint8_t *_get_data_start(struct container *container)
{
    return _get_code_start(container) + _get_code_size_rounded(container);
}

static uint32_t _get_data_size_rounded(struct container *container)
{
    return (container->data_len + 3) & ~3;
}

static uint32_t _get_data_max_size(struct container *container)
{
    return CONTAINER_DATA_SIZE - _get_code_size_rounded(container);
}

static uint8_t *_get_code_start(struct container *container)
{
    return container->container_data;
}

static uint32_t _get_code_size_rounded(struct container *container)
{
    return (container->code_len + 3) & ~3;
}

static uint32_t _get_code_max_size(struct container *container)
{
    (void)container;
    return CONTAINER_DATA_SIZE;
}

static container_id_t _fd_get_id(file_descriptor_t fd)
{
    return fd / 10 - 1;
}

static enum section_type _fd_get_section_type(file_descriptor_t fd)
{
    int type = fd % 10;

    switch (type) {
    case 0:
        return data;
    case 1:
        return code;
    case 2:
        return meta;
    default:
        return unknown;
    }
}
static struct descriptor *_fd_get_descriptor(file_descriptor_t fd)
{
    /* containers ids are assigned to 0 - 9
     * each id has three file descriptors, one for the data section, one for the
     * code section and
     * one for the metadata section
     *
     * id=0 -> data_fd=10, code_fd=11, code fd=12
     * id=1 -> data_fd=20, code_fd=21, code fd=22
     * id=2 -> data_fd=30, code_fd=31, code fd=32
     *                 ^^          ^^          ^^
     *                 XX/10-1     XX/10-1     XX/10-1
     */
    container_id_t id = _fd_get_id(fd);

    if (id < 0 || id >= CONTAINER_MAX_NUM) {
        return NULL;
    }
    return &descriptors[id];
}

static struct container *_fd_get_container(file_descriptor_t fd)
{
    container_id_t id = _fd_get_id(fd);

    if (id < 0 || id >= CONTAINER_MAX_NUM) {
        return NULL;
    }
    /* check if the container has been allocated */
    if (descriptors[id].allocated == 0) {
        return NULL;
    }

    return &containers[id];
}

static int _fd_get_section_info(file_descriptor_t fd,
                                struct section_info *section_info)
{
    struct descriptor *descriptor = _fd_get_descriptor(fd);

    if (descriptor == NULL) {
        return -1;
    }

    struct container *container = _fd_get_container(fd);

    if (container == NULL) {
        return -1;
    }

    switch (_fd_get_section_type(fd)) {
    case data:
        section_info->start = _get_data_start(container);
        section_info->max_size = _get_data_max_size(container);
        section_info->pos_offset_ptr = &descriptor->data_pos;
        section_info->len_ptr = &container->data_len;
        break;
    case code:
        section_info->start = _get_code_start(container);
        section_info->max_size = _get_code_max_size(container);
        section_info->pos_offset_ptr = &descriptor->code_pos;
        section_info->len_ptr = &container->code_len;
        break;
    case meta:
        section_info->start = _get_meta_start(container);
        section_info->max_size = _get_meta_max_size(container);
        section_info->pos_offset_ptr = &descriptor->meta_pos;
        section_info->len_ptr = &container->meta_len;
        break;
    default:
        return -1;
    }

    return 0;
}

static bool _is_data_writable(struct descriptor *descriptor)
{
    return descriptor->data_open && descriptor->code_sealed;
}

static bool _is_code_writable(struct descriptor *descriptor)
{
    return descriptor->code_open && !descriptor->code_sealed;
}

static bool _is_meta_writable(struct descriptor *descriptor)
{
    return descriptor->meta_open;
}

static bool _is_section_writable(struct descriptor *descriptor,
                                 enum section_type type)
{
    return (type == data && _is_data_writable(descriptor)) ||
           (type == code && _is_code_writable(descriptor)) ||
           (type == meta && _is_meta_writable(descriptor));
}

static bool _is_data_readable(struct descriptor *descriptor)
{
    return descriptor->data_open && descriptor->code_sealed;
}

static bool _is_code_readable(struct descriptor *descriptor)
{
    return descriptor->code_open && descriptor->code_sealed;
}

static bool _is_meta_readable(struct descriptor *descriptor)
{
    return descriptor->meta_open;
}

static bool _is_section_readable(struct descriptor *descriptor,
                                 enum section_type type)
{
    return (type == data && _is_data_readable(descriptor)) ||
           (type == code && _is_code_readable(descriptor)) ||
           (type == meta && _is_meta_readable(descriptor));
}

int memmgr_newcontainer(void)
{
    /* find an unallocated descriptor */
    for (container_id_t id = 0; id < CONTAINER_MAX_NUM; id++) {
        struct descriptor *descriptor = &descriptors[id];
        if (descriptor->allocated == 0) {
            descriptor->allocated = 1;
            return id;
        }
    }
    return -1;
}

void memmgr_freecontainer(container_id_t id)
{
    if (id >= 0 && id < CONTAINER_MAX_NUM) {
        descriptors[id] = (const struct descriptor){ 0 };
    }
}

int memmgr_openmetadatafileforcontainer(container_id_t id)
{
    if (id < 0 || id >= CONTAINER_MAX_NUM) {
        return -1;
    }

    struct descriptor *descriptor = &descriptors[id];

    if (!descriptor->allocated) {
        return -1;
    }

    descriptor->meta_open = 1;
    descriptor->meta_pos = 0;

    return (id + 1) * 10 + 2;
}

int memmgr_openmetadatafile(void)
{
    int container_id = service_getcontainerslotid();

    return memmgr_openmetadatafileforcontainer(container_id);
}

int memmgr_opencodefileforcontainer(container_id_t id)
{
    if (id < 0 || id >= CONTAINER_MAX_NUM) {
        return -1;
    }

    struct descriptor *descriptor = &descriptors[id];

    if (!descriptor->allocated) {
        return -1;
    }
    if (descriptor->code_open) {
        return -1;
    }

    descriptor->code_open = 1;
    descriptor->code_pos = 0;

    return (id + 1) * 10 + 1;
}

int memmgr_opendatafileforcontainer(container_id_t id)
{
    if (id < 0 || id >= CONTAINER_MAX_NUM) {
        return -1;
    }

    struct descriptor *descriptor = &descriptors[id];

    /* check that the container has been allocated and that the data section
     * hasn't already been opened
     */
    if (!descriptor->allocated) {
        return -1;
    }
    if (descriptor->data_open) {
        return -1;
    }
    if (!descriptor->code_sealed) {
        return -1;
    }

    descriptor->data_open = 1;
    descriptor->data_pos = 0;

    return (id + 1) * 10;
}

int memmgr_getsize(file_descriptor_t fd)
{
    struct descriptor *descriptor = _fd_get_descriptor(fd);

    if (descriptor == NULL) {
        return -1;
    }
    if (!descriptor->allocated) {
        return -1;
    }

    struct container *container = _fd_get_container(fd);

    switch (_fd_get_section_type(fd)) {
    case data:
        return container->data_len;
    case code:
        return container->code_len;
    case meta:
        return container->meta_len;
    default:
        return -1;
    }
}

void memmgr_close(file_descriptor_t fd)
{
    struct descriptor *descriptor = _fd_get_descriptor(fd);

    if (descriptor == NULL) {
        return;
    }
    if (!descriptor->allocated) {
        return;
    }

    switch (_fd_get_section_type(fd)) {
    case data:
        descriptor->data_open = 0;
        return;
    case code:
        /* set the code_sealed flag to 1 only if the code has already been
         * opened
         */
        if (descriptor->code_open) {
            descriptor->code_sealed = 1;
        }
        descriptor->code_open = 0;
        return;
    case meta:
        descriptor->meta_open = 0;
        return;
    default:
        // panic
        return;
    }
}

int memmgr_write(file_descriptor_t fd, const char *buf, uint32_t count)
{
    struct descriptor *descriptor = _fd_get_descriptor(fd);

    if (descriptor == NULL) {
        return -1;
    }

    enum section_type type = _fd_get_section_type(fd);

    if (!_is_section_writable(descriptor, type)) {
        return -1;
    }

    struct section_info section;

    if (_fd_get_section_info(fd, &section) != 0) {
        return -1;
    }

    /* pointer to the current position in the section */
    uint8_t *section_pos_ptr = section.start + *section.pos_offset_ptr;

    /* get remaining space in the section */
    uint32_t remaining_space = section.max_size - *section.pos_offset_ptr;

    /* make sure we don't write past the end of the section */
    count = MIN(count, remaining_space);

    /* copy the data from the buffer to the container, starting at the current
     * position in the section
     */
    memcpy(section_pos_ptr, buf, count);

    /* update the position offset in the descriptor, and the section length in
     * the container
     */
    *section.pos_offset_ptr += count;
    *section.len_ptr = *section.pos_offset_ptr;

    /* return the number of bytes actually written (may be less than original
     * count)
     */
    return count;
}

int memmgr_read(file_descriptor_t fd, char *buf, uint32_t count)
{
    struct descriptor *descriptor = _fd_get_descriptor(fd);

    if (descriptor == NULL) {
        return -1;
    }

    enum section_type type = _fd_get_section_type(fd);

    if (!_is_section_readable(descriptor, type)) {
        return -1;
    }

    struct section_info section;

    if (_fd_get_section_info(fd, &section) != 0) {
        return -1;
    }

    /* get how many bytes are left to read in the section */
    uint32_t remaining_bytes = *section.len_ptr - *section.pos_offset_ptr;

    /* make sure we don't read past the end of the section */
    count = MIN(count, remaining_bytes);

    /* pointer to the current position in the section */
    uint8_t *section_pos_ptr = section.start + *section.pos_offset_ptr;

    /* copy the data from the container to the buffer, starting at the current
     * position in the section
     */
    memcpy(buf, section_pos_ptr, count);

    /* update the position offset in the descriptor */
    *section.pos_offset_ptr += count;

    /* return the number of bytes actually read (may be less than original
     * count)
     */
    return count;
}

int tinycontainer_memmgr_init(void)
{
    /* nothing to do */

    return 0;
}

int memmgr_getcontainer(memmgr_block_t *container_data,
                        memmgr_block_t *container_code)
{
    if (container_data == NULL || container_code == NULL) {
        return -1;
    }

    int container_id = service_getcontainerslotid();

    if (container_id == -1) {
        return -1;
    }

    struct container *container = &containers[container_id];

    container_data->ptr = _get_data_start(container);
    container_data->size = _get_data_size_rounded(container);

    container_code->ptr = _get_code_start(container);
    container_code->size = container->code_len;

    return 0;
}

bool memmgr_check_metadata(container_id_t slot_id)
{

    metadata_t metadata;

    if (metadata_parse(&metadata, containers[slot_id].meta,
                       containers[slot_id].meta_len) != METADATA_OK) {
        goto on_error;
    }

    metadata_container_t container;

    if (metadata_container_parse(&container, metadata.container,
                                 metadata.container_len) != METADATA_OK) {
        goto on_error;
    }

    metadata_endpoints_t endpoints;

    if (metadata_endpoints_parse(&endpoints, metadata.endpoints,
                                 metadata.endpoints_len) != METADATA_OK) {
        goto on_error;
    }

    metadata_security_t security;

    if (metadata_security_parse(&security, metadata.security,
                                metadata.security_len) != METADATA_OK) {
        goto on_error;
    }

    /* currently all cwt are check using the key in slot 0 */
    const crypto_key_t *key = crypto_key_get(0);
    if (key == NULL) {
        /* could not acquire the key in slot 0 */
        return 0;
    }

    /* First handle the metadata token */

    /* parse the CWT */
    cwt_t cwt;
    if (cwt_parse(&cwt, security.cwt[0], security.cwt_size[0]) == false) {
        /* could not parse the CWT */
        return false;
    }

    /* select the algo to used based on CWT type */
    crypto_algo_t algo;
    switch (cwt.type) {
    case CWT_TYPE_UNKNOWN:
    /* use SIGN1 if the CWT is not tagged */
    case CWT_TYPE_COSE_SIGN1:
        algo = CRYPTO_ALGO_ED25519;
        break;
    case CWT_TYPE_COSE_MAC0:
        algo = CRYPTO_ALGO_HMAC_SHA256;
        break;
    case CWT_TYPE_COSE_ENCRYPT0:
        algo = CRYPTO_ALGO_AES_128_CBC;
        break;
    case CWT_TYPE_COSE_SIGN:
    case CWT_TYPE_COSE_MAC:
    case CWT_TYPE_COSE_ENCRYPT:
    /* not supported yet */
    default:
        /* invalid internal state */
        return false;
    }

    /* Verify the cwt with the key for the selected algo */
    if (cwt_verify(&cwt, key, algo) == false) {
        /* the CWT could not be verify with the key */
        return false;
    }

    /* Retrieve the metadata hash from the CWT claim set */
    nanocbor_value_t decoder;
    nanocbor_decoder_init(&decoder, cwt.claim_set, cwt.claim_set_size);
    nanocbor_value_t map;
    if (nanocbor_enter_map(&decoder, &map) < 0) {
        /* claim set is malformated */
        return false;
    }
    int32_t hash_key;
    if (nanocbor_get_int32(&map, &hash_key) < 0
        || hash_key != CWT_CLAIM_KEY_SHA256) {
        /* claim set is malformated */
        return false;
    }
    const uint8_t *hash;
    size_t hash_size;
    if (nanocbor_get_bstr(&map, &hash, &hash_size) < 0 ||
        hash_size != 32) {
        /* claim set is malformated */
        return false;
    }

    /* verify the hash */
    if (crypto_hash_verify(CRYPTO_ALGO_SHA_256, metadata.raw_cbor,
                           metadata.raw_cbor_len - 111 /* cwt token size */,
                           hash, hash_size) == false) {
        return false;
    }

    /* 2nd step: handle the data token */

    /* parse the CWT */
    if (cwt_parse(&cwt, security.cwt[1], security.cwt_size[1]) == false) {
        /* could not parse the CWT */
        return false;
    }

    /* select the algo to used based on CWT type */
    switch (cwt.type) {
    case CWT_TYPE_UNKNOWN:
    /* use SIGN1 if the CWT is not tagged */
    case CWT_TYPE_COSE_SIGN1:
        algo = CRYPTO_ALGO_ED25519;
        break;
    case CWT_TYPE_COSE_MAC0:
        algo = CRYPTO_ALGO_HMAC_SHA256;
        break;
    case CWT_TYPE_COSE_ENCRYPT0:
        algo = CRYPTO_ALGO_AES_128_CBC;
        break;
    case CWT_TYPE_COSE_SIGN:
    case CWT_TYPE_COSE_MAC:
    case CWT_TYPE_COSE_ENCRYPT:
    /* not supported yet */
    default:
        /* invalid internal state */
        return false;
    }

    /* Verify the cwt with the key for the selected algo */
    if (cwt_verify(&cwt, key, algo) == false) {
        /* the CWT could not be verify with the key */
        return false;
    }

    /* Retrieve the data hash from the CWT claim set */
    nanocbor_decoder_init(&decoder, cwt.claim_set, cwt.claim_set_size);
    if (nanocbor_enter_map(&decoder, &map) < 0) {
        /* claim set is malformated */
    }
    if (nanocbor_get_int32(&map, &hash_key) < 0
        || hash_key != CWT_CLAIM_KEY_SHA256) {
        /* claim set is malformated */
        return false;
    }
    if (nanocbor_get_bstr(&map, &hash, &hash_size) < 0) {
        /* claim set is malformated */
        return false;
    }

    /* verify the hash */
    if (crypto_hash_verify(CRYPTO_ALGO_SHA_256,
                           _get_data_start(&containers[slot_id]),
                           containers[slot_id].data_len,
                           hash, hash_size) == false) {
        return false;
    }

    /* 3rd step: handle the code token */

    /* parse the CWT */
    if (cwt_parse(&cwt, security.cwt[2], security.cwt_size[2]) == false) {
        /* could not parse the CWT */
        return false;
    }

    /* select the algo to used based on CWT type */
    switch (cwt.type) {
    case CWT_TYPE_UNKNOWN:
    /* use SIGN1 if the CWT is not tagged */
    case CWT_TYPE_COSE_SIGN1:
        algo = CRYPTO_ALGO_ED25519;
        break;
    case CWT_TYPE_COSE_MAC0:
        algo = CRYPTO_ALGO_HMAC_SHA256;
        break;
    case CWT_TYPE_COSE_ENCRYPT0:
        algo = CRYPTO_ALGO_AES_128_CBC;
        break;
    case CWT_TYPE_COSE_SIGN:
    case CWT_TYPE_COSE_MAC:
    case CWT_TYPE_COSE_ENCRYPT:
    /* not supported yet */
    default:
        /* invalid internal state */
        return false;
    }

    /* Verify the cwt with the key for the selected algo */
    if (cwt_verify(&cwt, key, algo) == false) {
        /* the CWT could not be verify with the key */
        return false;
    }

    /* Retrieve the code hash from the CWT claim set */
    nanocbor_decoder_init(&decoder, cwt.claim_set, cwt.claim_set_size);
    if (nanocbor_enter_map(&decoder, &map) < 0) {
        /* claim set is malformated */
        return false;
    }
    if (nanocbor_get_int32(&map, &hash_key) < 0
        || hash_key != -65537) {
        /* claim set is malformated */
        return false;
    }
    if (nanocbor_get_bstr(&map, &hash, &hash_size) < 0) {
        /* claim set is malformated */
        return false;
    }

    /* verify the hash */
    if (crypto_hash_verify(CRYPTO_ALGO_SHA_256,
                           _get_code_start(&containers[slot_id]),
                           containers[slot_id].code_len,
                           hash, hash_size) == false) {
        return false;
    }

    /* we had successfully checked the metadata */
    return true;

on_error:

    return false;
}

int memmgr_get_slot_id(uint8_t *uid, size_t size)
{
    for (int slot_id = 0; slot_id < CONTAINER_MAX_NUM; slot_id++) {
        if (descriptors[slot_id].allocated == false) {
            continue;
        }

        metadata_t metadata;

        if (metadata_parse(&metadata, containers[slot_id].meta,
                           containers[slot_id].meta_len) != METADATA_OK) {
            /* note: should not occur. Maybe an error should be raised here */
            continue;
        }

        metadata_container_t container;
        if (metadata_container_parse(&container, metadata.container,
                                     metadata.container_len) != METADATA_OK) {
            /* note: should not occur. Maybe an error should be raised here */
            continue;
        }

        if (container.uid_len == size &&
            memcmp(container.uid, uid, size) == 0) {
            return slot_id;
        }
    }

    return -1;
}

int memmgr_getendpoint(int slot_id, uint32_t endpoint_id,
                       metadata_endpoint_t *endpoint)
{
    metadata_t metadata;

    if (metadata_parse(&metadata, containers[slot_id].meta,
                       containers[slot_id].meta_len) != METADATA_OK) {
        /* note: should not occur */
        return METADATA_NOT_FOUND;
    }

    return metadata_endpoints_search(endpoint,
                                     metadata.endpoints, metadata.endpoints_len,
                                     endpoint_id);
}

uint32_t memmgr_getsyscallmask(int slot_id)
{
    metadata_t metadata;

    if (metadata_parse(&metadata, containers[slot_id].meta,
                       containers[slot_id].meta_len) != METADATA_OK) {
        /* note: should not occur */
        return 0;
    }

    metadata_container_t container;
    if (metadata_container_parse(&container, metadata.container,
                                 metadata.container_len) != METADATA_OK) {
        /* note: should not occur */
        return 0;
    }

    /* Parse the CWT syscall_mask */
    cwt_t cwt;
    if (cwt_parse(&cwt, container.syscall_mask, container.syscall_mask_len) == false) {
        /* could not parse the CWT syscall mask */
        return 0;
    }

    /* Check the CWT syscall mask using the key in the first slot */
    //FIXME: the key used should be configurable
    const crypto_key_t *key = crypto_key_get(0);
    if (key == NULL) {
        /* could not acquire the key in slot 0 */
        return 0;
    }

    /* select the algo to used based on CWT type */
    crypto_algo_t algo;
    switch (cwt.type) {
    case CWT_TYPE_UNKNOWN:
    /* use SIGN1 if the CWT is not tagged */
    case CWT_TYPE_COSE_SIGN1:
        algo = CRYPTO_ALGO_ED25519;
        break;
    case CWT_TYPE_COSE_MAC0:
        algo = CRYPTO_ALGO_HMAC_SHA256;
        break;
    case CWT_TYPE_COSE_ENCRYPT0:
        algo = CRYPTO_ALGO_AES_128_CBC;
        break;
    case CWT_TYPE_COSE_SIGN:
    case CWT_TYPE_COSE_MAC:
    case CWT_TYPE_COSE_ENCRYPT:
    /* not supported yet */
    default:
        /* invalid internal state */
        return 0;
    }

    /* Verify the cwt with the key for the selected algo */
    if (cwt_verify(&cwt, key, algo) == false) {
        /* the CWT could not be verify with the key */
        return 0;
    }

    /* Retrieve the CWT claim set */
    nanocbor_value_t decoder;
    nanocbor_decoder_init(&decoder, cwt.claim_set, cwt.claim_set_size);
    nanocbor_value_t map;
    if (nanocbor_enter_map(&decoder, &map) < 0) {
        /* claim set is malformated */
    }
    int32_t bitmask_key;
    if (nanocbor_get_int32(&map, &bitmask_key) < 0
        || bitmask_key != CWT_CLAIM_KEY_BITMASK) {
        /* claim set is malformated */
        return 0;
    }
    const uint8_t *bitmask; size_t bitmask_size;
    if (nanocbor_get_bstr(&map, &bitmask, &bitmask_size) < 0) {
        /* claim set is malformated */
        return 0;
    }
    if (bitmask_size > sizeof(uint32_t)) {
        /* large bitmask is not supported yet */
        return 0;
    }
    uint32_t syscall_mask = 0;
    for (unsigned int i = 0; i < bitmask_size; i++) {
        syscall_mask <<= 8;
        syscall_mask |= bitmask[i];
    }

    return syscall_mask;
}
