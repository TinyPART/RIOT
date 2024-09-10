/*
 * Copyright (C) 2022-2023, Orange.
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
 * @brief       TinyContainer RAM Memory Manager sub-module definitions
 *
 * @author      Gregory Holder <gregory.holder76@gmail.com>
 *
 * @}
 */

#ifndef MEMMGR_RAM_H
#define MEMMGR_RAM_H

#include <stdint.h>
#include <stdbool.h>

#include "tinycontainer/memmgr/memmgr_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#ifdef TINYCONTAINER_MAX_SIZE_FOR_METADATA
#define META_DATA_SIZE      TINYCONTAINER_MAX_SIZE_FOR_METADATA
#else
#define META_DATA_SIZE      512
#endif
#ifdef TINYCONTAINER_MAX_SIZE_FOR_CONTAINER
#define CONTAINER_DATA_SIZE TINYCONTAINER_MAX_SIZE_FOR_CONTAINER
#else
#define CONTAINER_DATA_SIZE 1024
#endif

#ifdef TINYCONTAINER_NUMBER_OF_CONTAINERS
#define CONTAINER_MAX_NUM   TINYCONTAINER_NUMBER_OF_CONTAINERS
#else
#define CONTAINER_MAX_NUM   1
#endif

struct container {
    uint32_t meta_len;
    uint8_t meta[META_DATA_SIZE];

    uint32_t code_len;
    uint32_t data_len;
    uint8_t container_data[CONTAINER_DATA_SIZE];
};

#define CONTAINER_SIZE (sizeof(struct container))

enum section_type {
    data    = 0,
    code    = 1,
    meta    = 2,
    unknown = -1,
};

/* container description */
struct descriptor {
    uint8_t allocated    : 1;
    uint8_t meta_open    : 1;
    uint8_t code_open    : 1;
    uint8_t code_sealed  : 1;   // once code has been opened, written and closed,
                                // it is sealed. It can still be opened to be
                                // read, but not written to.
    uint8_t data_open    : 1;

    uint32_t meta_pos;
    uint32_t code_pos;
    uint32_t data_pos;
};

/* Used by the read and write functions.
 * It is a way of generalizing for the different sections (data, code, meta)
 */
struct section_info {
    uint8_t *start;             // pointer to the start of the section
    uint32_t max_size;          // max size allowed for the section
    uint32_t *pos_offset_ptr;   // pointer to where the current position in the
                                // section is stored in the descriptor [1]
    uint32_t *len_ptr;          // pointer to where the length of the section
                                // is stored in the container [2]

    /* [1]  Within the descriptor, we store an offset from the start of the
     *      section to the current position in the section.
     *      Here we have a pointer to that offset, not a pointer to the actual
     *       position in the section.
     *
     * [2]  Same idea as [1]. Within the container memory, we also store the
     *      current length of the section.
     *      Here we have a pointer to that length, not a pointer to the end of
     *      the section.
     */
};

/* get a pointer to the start of the meta section */
static uint8_t *_get_meta_start(struct container *container);

/* get a pointer to the start of the code section */
static uint8_t *_get_code_start(struct container *container);

/* get a pointer to the start of the data section
 * note: the code and data sections share the same region, so the code
 *       section must have been sealed first (opened, written to, and closed)
 *       to ensure correct results
 */
static uint8_t *_get_data_start(struct container *container);

/* get the size of the code section, rounded up to the nearest 4 byte boundary
 */
static uint32_t _get_code_size_rounded(struct container *container);

/* get the maximum size the data section can be at the current time */
static uint32_t _get_data_max_size(struct container *container);

/* get the maximum size the code section can be at the current time */
static uint32_t _get_code_max_size(struct container *container);

/* get the maximum size the meta section can be at the current time */
static uint32_t _get_meta_max_size(struct container *container);

/* get the id of the container that the file descriptor belongs to */
static container_id_t _fd_get_id(file_descriptor_t fd);

/* get the section type from the file descriptor (data, code, meta) */
static enum section_type _fd_get_section_type(file_descriptor_t fd);

/* get the descriptor for the container that the file descriptor belongs to */
static struct descriptor *_fd_get_descriptor(file_descriptor_t fd);

/* get the container that the file descriptor belongs to */
static struct container *_fd_get_container(file_descriptor_t fd);

/* see [struct section_info] */
static int _fd_get_section_info(file_descriptor_t fd, struct section_info *section_info);

/* is the data section ready to be written to? */
static bool _is_data_writable(struct descriptor *descriptor);

/* is the code section ready to be written to? */
static bool _is_code_writable(struct descriptor *descriptor);

/* is the meta section ready to be written to? */
static bool _is_meta_writable(struct descriptor *descriptor);

/* is the given section ready to be written to? */
static bool _is_section_writable(struct descriptor *descriptor, enum section_type section_type);

/* is the data section ready to be read from? */
static bool _is_data_readable(struct descriptor *descriptor);

/* is the code section ready to be read from? */
static bool _is_code_readable(struct descriptor *descriptor);

/* is the meta section ready to be read from? */
static bool _is_meta_readable(struct descriptor *descriptor);

/* is the given section ready to be read from? */
static bool _is_section_readable(struct descriptor *descriptor, enum section_type section_type);

#ifdef __cplusplus
}
#endif

#endif /* MEMMGR_RAM_H */
