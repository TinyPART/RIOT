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

#ifndef TINYCONTAINER_MEMMGR_MEMMGR_CONTROLLER_H
#define TINYCONTAINER_MEMMGR_MEMMGR_CONTROLLER_H

#include <stdint.h>

#include "memmgr_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Create a new empty container
 *
 * @return   container_id_t   (On success a container id is returned)
 * @return   container_id_t   (On failure, -1 is returned)
 */
container_id_t memmgr_newcontainer(void);

/**
 * @brief   open the metadata file and return a file descriptor id
 *
 * @param[in]   id    container file descriptor
 *
 * @return   int  (On success, a file descriptor id is returned)
 * @return   int  (On error, -1 is returned)
 */
file_descriptor_t memmgr_openmetadatafileforcontainer(container_id_t id);

/**
 * @brief open the data file and return a file descriptor id
 *
 * @param[in]   id    container file descriptor
 *
 * @return   int   (On success, a file descriptor id is returned)
 * @return   int   (On error, -1 is returned)
 */
file_descriptor_t memmgr_opendatafileforcontainer(container_id_t id);

/**
 * @brief   open the code file and return a file descriptor id
 *
 * @param[in]  id   container file descriptor
 *
 * @return   int   (On success, a file descriptor id is returned)
 * @return   int   (On error, -1 is returned.
 */
file_descriptor_t memmgr_opencodefileforcontainer(container_id_t id);

/**
 * @brief   close a file descriptor id
 */
void memmgr_close(file_descriptor_t fd);

/**
 * @brief write a container section (data, code, or metadata)
 * write up to count bytes from the buffer starting at buf to the file referred
 * to by the file descriptor fd.
 *
 * @param[in]   fd    container file descriptor
 * @param[out]  buf   buffer to write into
 * @param[in]   count number of byte from fd to write to buf
 *
 * @return   int   (On success, the number of bytes written is returned (zero
 *                  indicates nothing was written). It is not an error if this
 *                  number is smaller than count.)
 * @ return  int   (On error, -1 is returned)
 */
int memmgr_write(file_descriptor_t fd,
                 char const *buf,
                 uint32_t count);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_MEMMGR_MEMMGR_CONTROLLER_H */
/** @} */
