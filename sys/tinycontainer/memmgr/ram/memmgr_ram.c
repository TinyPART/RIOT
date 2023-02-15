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
 * @brief       TinyContainer RAM Memory Manager sub-module implementation
 *
 * @author      Samuel legouix <samuel.legouix@orange.com>
 * @author      Gregory Holder <gregory.holder@orange.com>
 *
 * @}
 */


#include <string.h>
#include <stddef.h>

#include "tinycontainer/memmgr/memmgr.h"
#include "tinycontainer/memmgr/memmgr_controller.h"
#include "tinycontainer/memmgr/memmgr_service.h"
#include "tinycontainer/memmgr/memmgr_container.h"
#include "tinycontainer/service/service_memmgr.h"
#include "tinycontainer/firewall/firewall_all.h"

#include "memmgr_ram.h"

/* allocate memory for the containers and their descriptors */
static struct container containers[CONTAINER_MAX_NUM] = { 0 };
static struct descriptor descriptors[CONTAINER_MAX_NUM] = { 0 };

static uint8_t *_get_meta_start(struct container *container)
{
    return container->meta;
}

static uint8_t *_get_code_start(struct container *container)
{
    return container->container_data;
}

static uint32_t _get_code_size_rounded(struct container *container)
{
    return (container->code_len + 3) & ~3;
}

static uint8_t *_get_data_start(struct container *container)
{
    return _get_code_start(container) + _get_code_size_rounded(container);
}

static uint32_t _get_data_max_size(struct container *container)
{
    return CONTAINER_DATA_SIZE - _get_code_size_rounded(container);
}

static uint32_t _get_code_max_size(struct container *container)
{
    (void)container;
    return CONTAINER_DATA_SIZE;
}

static uint32_t _get_meta_max_size(struct container *container)
{
    (void)container;
    return META_DATA_SIZE;
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
     * each id has three file descriptors, one for the data section, one for the code section and
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

static int _fd_get_section_info(file_descriptor_t fd, struct section_info *section_info)
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

static bool _is_section_writable(struct descriptor *descriptor, enum section_type type)
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

static bool _is_section_readable(struct descriptor *descriptor, enum section_type type)
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
    int container_id = service_getcontaineridfrompid(firewall_getpid());

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

int memmgr_opencodefile(void)
{
    //FIXME: temporary workaround to retrieve container id
    return memmgr_opencodefileforcontainer(0);
}

int memmgr_opendatafileforcontainer(container_id_t id)
{
    if (id < 0 || id >= CONTAINER_MAX_NUM) {
        return -1;
    }

    struct descriptor *descriptor = &descriptors[id];

    /* check that the container has been allocated and that the data section hasn't already been opened */
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

int memmgr_opendatafile(void)
{
    //FIXME: temporary workaround to retrieve container id
    return memmgr_opendatafileforcontainer(0);
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
        /* set the code_sealed flag to 1 only if the code has already been opened */
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

    /* copy the data from the buffer to the container, starting at the current position in the section */
    memcpy(section_pos_ptr, buf, count);

    /* update the position offset in the descriptor, and the section length in the container */
    *section.pos_offset_ptr += count;
    *section.len_ptr = *section.pos_offset_ptr;

    /* return the number of bytes actually written (may be less than original count) */
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

    /* copy the data from the container to the buffer, starting at the current position in the section */
    memcpy(buf, section_pos_ptr, count);

    /* update the position offset in the descriptor */
    *section.pos_offset_ptr += count;

    /* return the number of bytes actually read (may be less than original count) */
    return count;
}

int tinycontainer_memmgr_init(void)
{
    /* nothing to do */

    return 0;
}
