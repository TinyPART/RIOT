/**
 *
 * Copyright (C) 2020-2023, Orange
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
 * @brief   API for Firewall sub-module of TinyContainer
 *
 * @author  BERKANE Ghilas (ghilas.berkane@gmail.com)
 *
 */

#ifndef TINYCONTAINER_FIREWALL_FIREWALL_H
#define TINYCONTAINER_FIREWALL_FIREWALL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialize TinyContainer Firewall module
 *
 * @return int (-1 on error)
 */
int tinycontainer_firewall_init(void);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_FIREWALL_FIREWALL_H */
/** @} */
