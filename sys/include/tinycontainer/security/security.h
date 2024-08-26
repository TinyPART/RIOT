/**
 *
 * Copyright (C) 2020-2024, Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_tinycontainer
 *
 * @{
 *
 * @file
 * @brief   API for Security sub-module of TinyContainer
 *
 * @author  BERKANE Ghilas (ghilas.berkane@gmail.com)
 * @author  samuel.legouix <samuel.legouix@orange.com>
 *
 */

#ifndef TINYCONTAINER_SECURITY_SECURITY_H
#define TINYCONTAINER_SECURITY_SECURITY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialize TinyContainer Security module
 *
 * @return int (-1 on error)
 */
int tinycontainer_security_init(void);

#ifdef __cplusplus
}
#endif

#endif /* TINYCONTAINER_SECURITY_SECURITY_H */
/** @} */
