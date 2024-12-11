#!/bin/bash

do_get() {
    config_file=$1
    directive=$2

    test -f "${config_file}" \
    && grep "^${directive} " "${config_file}" \
    || echo "not found!" >&2
}

do_del() {
    config_file=$1
    directive=$2

    test -f "${config_file}" \
    && sed -i "/^${directive} /d" "${config_file}"
}

do_set() {
    config_file=$1
    directive=$2
    value=$3

    test -f "${config_file}" \
    && grep -q "^${directive} " "${config_file}" \
    && sed -i "s/^${directive} .*/${directive} ${value}/" "${config_file}" \
    || echo "${directive} $3" >> "${config_file}"
}

get_value() {
    config_file=$1
    directive=$2
    value=$3

    grep -q "^${directive} " "${config_file}" \
    && value=$(grep "^${directive} " "${config_file}" | cut -d" " -f2)
    echo "$value"
}

do_make() {
    config_file=$1
    make_file=$2

    test ! -f "${config_file}" \
    && echo "Configuration file not found!" >&2 \
    && exit

    # reset make file
    : > "${make_file}"

    # board
    value=$(get_value "${config_file}" "BOARD" "dwm1001")
    echo "BOARD ?= ${value}" >> "${make_file}"

    # runtime
    value=$(get_value "${config_file}" "RUNTIME" "wamr")
    echo "TINYCONTAINER_RUNTIME ?= ${value}" >> "${make_file}"

    # memmgr
    value=$(get_value "${config_file}" "MEMMGR" "ram")
    echo "TINYCONTAINER_MEMMGR ?= ${value}" >> "${make_file}"

    # crypto
    value=$(get_value "${config_file}" "CRYPTO" "none")
    echo "TINYCONTAINER_CRYPTO ?= ${value}" >> "${make_file}"
    if test "${value}" = "psa"; then
        value=$(get_value "${config_file}" "CRYPTO_PSA_KEY_COUNT" 1)
        echo "CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=${value}" \
          >> "${make_file}"
    fi

    # number of containers
    value=$(get_value "${config_file}" "NUMBER_OF_CONTAINERS" 1)
    echo "NUMBER_OF_CONTAINERS ?= ${value}" >> ${make_file}

    # max size for container metadata
    value=$(get_value "${config_file}" "MAX_SIZE_FOR_METADATA" 1024)
    echo "MAX_SIZE_FOR_METADATA ?= ${value}" >> "${make_file}"

    # max size for container data
    #value=$(get_value ${config_file} "MAX_SIZE_FOR_DATA" 128)
    #echo "MAX_SIZE_FOR_DATA ?= ${value}" >> ${make_file}

    # max size for container code
    value=$(get_value "${config_file}" "MAX_SIZE_FOR_CODE" 1024)
    echo "MAX_SIZE_FOR_CONTAINER ?= ${value}" >> "${make_file}"

    # networking
    value=$(get_value "${config_file}" "NETWORKING" 0)
    if test "value" = "on"; then
        echo "TINYCONTAINER_NETWORK ?= 1" >> "${make_file}"
    else
        echo "TINYCONTAINER_NETWORK ?= 0" >> "${make_file}"
    fi

}

case $1 in
    set)
        shift
        do_set "$@"
        ;;
    get)
        shift
        do_get "$@"
        ;;
    del)
        shift
        do_del "$@"
        ;;
    make)
        shift
        do_make "$@"
        ;;
    *)
        echo "usage: $0 set|get|del <config_file> <directive> [value]"
        echo "usage: $0 make <config_file> <makefile_file>"
        ;;
esac
