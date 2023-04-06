/*
 * Copyright (C) 2023 Orange
 * Please, refer to the README.md and LICENSE files of TinyContainer
 *
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

static int get_container_slot_id(uint8_t *uid)
{
    /* TODO: searching the container slot id from uid string is not yet
     *       implemented. Until that, return the 1st slot
     */

    (void)uid;
    return 0;
}

/* public functions
 * ================
 */

kernel_pid_t tinycontainer_init(uint8_t controller_prio, uint8_t service_prio,
                                uint8_t containers_prio)
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

    pid = tinycontainer_controller_init(controller_prio);

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

bool tinycontainer_startcontainer(uint8_t *container_uid)
{
    int slot_id = get_container_slot_id(container_uid);

    return controller_start(slot_id);
}

bool tinycontainer_stopcontainer(uint8_t *container_uid)
{
    int slot_id = get_container_slot_id(container_uid);

    return controller_stop(slot_id);
}

bool tinycontainer_iscontainerrunning(uint8_t *container_uid)
{
    int slot_id = get_container_slot_id(container_uid);

    return controller_isrunning(slot_id);
}
