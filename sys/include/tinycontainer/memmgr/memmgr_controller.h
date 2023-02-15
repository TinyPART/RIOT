/**
 *
 * Copyright (C) 2020-2023, Orange.
 *
 * Please, refer to the README.md and LICENSE files of TinyContainer
 *
 */

/**
 * @ingroup sys_tinycontainer
 *
 * @{
 *
 * @file
 * @brief   API of TinyContainer Memory Manager sub-module for Controller sub-module
 *
 * @author  Samuel Legouix <samuel.legouix@orange.com>
 *
 */


#ifndef TINYCONTAINER_MEMMGR_CONTROLLER_H
#define TINYCONTAINER_MEMMGR_CONTROLLER_H

#include <stdint.h>

#include "memmgr_common.h"

/*
 * Create a new empty container
 *
 * On success a container id is returned.
 * On failure, -1 is returned.
 */
container_id_t memmgr_newcontainer(void);

/*
 * open the metadata file and return a file descriptor id
 *
 * On success, a file descriptor id is returned.
 * On error, -1 is returned.
 */
file_descriptor_t memmgr_openmetadatafileforcontainer(container_id_t id);

/*
 * open the data file and return a file descriptor id
 *
 * On success, a file descriptor id is returned.
 * On error, -1 is returned.
 */
file_descriptor_t memmgr_opendatafileforcontainer(container_id_t id);

/*
 * open the code file and return a file descriptor id
 *
 * On success, a file descriptor id is returned.
 * On error, -1 is returned.
 */
file_descriptor_t memmgr_opencodefileforcontainer(container_id_t id);

/*
 * close a file descriptor id
 */
void memmgr_close(file_descriptor_t fd);

/* write up to count bytes from the buffer starting at buf to the file referred
 * to by the file descriptor fd.
 *
 * On success, the number of bytes written is returned (zero indicates nothing was written). It is not an error if this number is smaller than count.
 * On error, -1 is returned.
 */
int memmgr_write(file_descriptor_t fd,
                 char const *buf,
                 uint32_t count);

#endif /* TINYCONTAINER_MEMMGR_CONTROLLER_H */
/** @} */
