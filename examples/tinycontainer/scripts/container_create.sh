#!/bin/bash

test $# -ne 2 \
&& echo "Usage: $0 <container_directory> wamr|rbpf|jerryscript" \
&& exit

CONTAINER_DIR=$1
CONTAINER_NAME=$(basename "$1")

RUNTIME_TYPE=$2
test "$RUNTIME_TYPE" != "wamr" -a \
     "$RUNTIME_TYPE" != "rbpf" -a \
     "$RUNTIME_TYPE" != "jerryscript" \
&& echo "Usage: $0 <container_directory> wamr|rbpf|jerryscript" \
&& exit

test -d ${CONTAINER_DIR} &&
    echo "Error: this container already exit!" &&
    exit -1

mkdir ${CONTAINER_DIR} ||
(
    echo "Error: could not create container directory!"
    exit -1
)

echo $RUNTIME > ${CONTAINER_DIR}/RUNTIME

if test "$RUNTIME" = "wamr"; then
    echo "\
/*
 * This file implement a WebAssembly container named ${CONTAINER_NAME}
 *
 * Copyright (C) 2024 Orange
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory of the TinyContainer module for more details.
 */

#include <stdint.h>

extern void logger(char *);
extern int32_t open(uint32_t);
extern int32_t close(int32_t);
extern int32_t read(int32_t, uint8_t *, uint32_t);
extern int32_t write(int32_t, uint8_t *, uint32_t);

#define WASM_EXPORT __attribute__((visibility(\"default\")))

WASM_EXPORT void start(void)
{
    logger(\"${CONTAINER_NAME}: starting\n\");
}

WASM_EXPORT int loop(void)
{
    logger(\"${CONTAINER_NAME}: looping\n\");

    return 0;
}

WASM_EXPORT void stop(void)
{
    logger(\"${CONTAINER_NAME}: stopping\n\");
}

/*EOF*/" > ${CONTAINER_DIR}/${CONTAINER_NAME}.c;
elif test "$RUNTIME" = "rbpf"; then
    touch ${CONTAINER_DIR}/${CONTAINER_NAME}.c
    echo "Please update file '${CONTAINER_DIR}/${CONTAINER_NAME}.c'"
elif test "$RUNTIME" = "jerryscript"; then
    touch ${CONTAINER_DIR}/${CONTAINER_NAME}.js
    echo "Please update file '${CONTAINER_DIR}/${CONTAINER_NAME}.js'"
else
    echo "Error: this runtime is not supported!"
    exit
fi

echo "not yet implemented" > ${CONTAINER_DIR}/${CONTAINER_NAME}.data

echo "Please copy the container metadata into the file '${CONTAINER_DIR}/${CONTAINER_NAME}.metadata'"
