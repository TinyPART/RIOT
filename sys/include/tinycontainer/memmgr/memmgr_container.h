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
 * @brief   API of TinyContainer Memory Manager sub-module for Container sub-module
 *
 * @author  Samuel Legouix <samuel.legouix@orange.com>
 *
 */


#ifndef TINYCONTAINER_MEMMGR_CONTAINER_H
#define TINYCONTAINER_MEMMGR_CONTAINER_H

#include <stdint.h>

#include "tinycontainer/memmgr/memmgr_common.h"

/*
 * open the metadata file and return a file descriptor id
 *
 * On success, a file descriptor id is returned.
 * On error, -1 is returned.
 */
file_descriptor_t memmgr_openmetadatafile(void);

/*
 * open the data file and return a file descriptor id
 *
 * On success, a file descriptor id is returned.
 * On error, -1 is returned.
 */
file_descriptor_t memmgr_opendatafile(void);

/*
 * open the code file and return a file descriptor id
 *
 * On success, a file descriptor id is returned.
 * On error, -1 is returned.
 */
file_descriptor_t memmgr_opencodefile(void);

/* Retrieve the size of a file
 *
 * On success, the file size is returned.
 * On invalid file descriptor, -1 is returned.
 */
int memmgr_getsize(file_descriptor_t fd);

/*
 * close a file descriptor id
 */
void memmgr_close(file_descriptor_t fd);

/* Attemps to read up to count bytes from file descriptor fd into buffer
 * starting at buf.
 *
 * return the number of bytes read (zero indicates end of file). It is not an
 * error if this number is smaller than count.
 * On error, -1 is returned.
 */
int memmgr_read(file_descriptor_t fd,
                char *buf,
                uint32_t count);

#endif /* TINYCONTAINER_MEMMGR_CONTAINER_H */
/** @} */
