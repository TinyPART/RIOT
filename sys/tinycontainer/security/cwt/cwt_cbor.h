/*
 * Copyright (C) 2024, Orange.
 *
 * Please, refer to the README.md and LICENSE files of TinyContainer
 *
 */

/**
 * @ingroup     sys_tinycontainer
 * @{
 *
 * @file
 * @brief       TinyContainer CWT submodule - cbor definition to parse cwt
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#ifndef CWT_CBOR_H
#define CWT_CBOR_H

#include <thread.h>
#include <stdint.h>

#include "tinycontainer/service/service_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CBOR_TAG_CWT 61

#define CBOR_TAG_COSE_ENCRYPT0 16
#define CBOR_TAG_COSE_MAC0 17
#define CBOR_TAG_COSE_SIGN1 18
#define CBOR_TAG_COSE_ENCRYPT 96
#define CBOR_TAG_COSE_MAC 97
#define CBOR_TAG_COSE_SIGN 98

#ifdef __cplusplus
}
#endif

#endif /* CWT_CBOR_H */
