/*
 * Copyright (C) 2023-2024 Orange
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
 * @brief       TinyContainer implementation
 *
 * @author      Samuel legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#include "tinycontainer.h"

#include "tinycontainer/controller/controller.h"
#include "tinycontainer/firewall/firewall.h"
#include "tinycontainer/memmgr/memmgr.h"
#include "tinycontainer/service/service.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/*
 * private functions
 * ================
 */

static int get_container_slot_id(uint8_t *uid, size_t len)
{
    int slot_id = controller_get_slot_id(uid, len);

    return slot_id;
}

/* public functions
 * ================
 */

kernel_pid_t tinycontainer_init(uint8_t controller_prio,
                                uint8_t service_prio,
                                uint8_t containers_prio,
                                tinycontainer_io_driver_t * driver)
{
    kernel_pid_t pid;

    /* Intialiaze the firewall module */

    tinycontainer_firewall_init();

    /* Intialiaze the memmgr module */

    tinycontainer_memmgr_init();

    /* Start the service thread */

    pid = tinycontainer_service_init(service_prio, containers_prio);

    if (pid <= 0) {
        DEBUG("fail to start tinycontainer service thread!\n");
        return pid;
    }

    /* Start the controller thread */

    pid = tinycontainer_controller_init(controller_prio, (io_driver_t*)driver);

    if (pid <= 0) {
        DEBUG("fail to start tinycontainer controller thread!\n");
        return pid;
    }

    DEBUG("TinyContainer started\n");

    return pid;
}

bool tinycontainer_loadcontainer(uint8_t *metadata, int metadata_size,
                                 uint8_t *data, int data_size,
                                 uint8_t *code, int code_size)
{
    return controller_load(metadata, metadata_size,
                           data, data_size,
                           code, code_size);
}

bool tinycontainer_startcontainer(uint8_t *container_uid, size_t container_uid_len)
{
    int slot_id = get_container_slot_id(container_uid, container_uid_len);

    if (slot_id == -1) {
        return false;
    }

    return controller_start(slot_id);
}

bool tinycontainer_stopcontainer(uint8_t *container_uid, size_t container_uid_len)
{
    int slot_id = get_container_slot_id(container_uid, container_uid_len);

    if (slot_id == -1) {
        return false;
    }

    return controller_stop(slot_id);
}

bool tinycontainer_iscontainerrunning(uint8_t *container_uid, size_t container_uid_len)
{
    int slot_id = get_container_slot_id(container_uid, container_uid_len);

    if (slot_id == -1) {
        return false;
    }

    return controller_isrunning(slot_id);
}
