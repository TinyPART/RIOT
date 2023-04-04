/**
 *
 * Copyright (C) 2022-2023, Orange.
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
 * @brief   shared definitions of TinyContainer Memory Manager sub-module
 *
 * @author  Gregory Holder <gregory.holder@orange.com>
 *
 */


#ifndef TINYCONTAINER_MEMMGR_COMMON_H
#define TINYCONTAINER_MEMMGR_COMMON_H

typedef int file_descriptor_t;
typedef int container_id_t;

/**
  * @brief Defines a memory block to the code or data of a container
  */
typedef struct {
    uint8_t *ptr;    /**< Pointer to the memory block  */
    size_t size;     /**< size of the memory block */
} memmgr_block_t;


#endif /* TINYCONTAINER_MEMMGR_COMMON_H */
/** @} */
