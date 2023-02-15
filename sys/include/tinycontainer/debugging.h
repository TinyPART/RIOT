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
 * @brief   some debugging macro for TinyContainer
 *
 *          Define ENABLE_DEBUG before including this file to enable debug output for current file
 *          Define DO_LOG_FILENAME to log the filename in addition to the function name for LOG_ENTER and LOG_EXIT
 *
 * @author  Gregory Holder <gregory.holder@orange.com>
 *
 */

#ifndef TINYCONTAINER_DEBUGGING_H
#define TINYCONTAINER_DEBUGGING_H


/* Uncomment the following line to enable debug everywhere (very verbose) */
//#define ENABLE_DEBUG (1)

//#define DO_LOG_FILENAME (1)

#include "debug.h"

/* debug macro for printing messages with PID */
#define DEBUG_PID(...) do { DEBUG("[%d] ", thread_getpid()); DEBUG(__VA_ARGS__); } while (0)

/* create a macro to log the pid, function name and a message */
#define LOG_PID_FUNC(...) do { DEBUG_PID(" | %s: ", __func__); DEBUG(__VA_ARGS__); } while (0)

/* macro to obtain the filename from the __FILE__ macro */
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

//#define DO_LOG_FILENAME 1

#ifdef DO_LOG_FILENAME
#define LOG_ENTER() DEBUG_PID("-> %s: %s\n", __FILENAME__, __func__)
#define LOG_EXIT() DEBUG_PID("<- %s: %s\n", __FILENAME__, __func__)
#else
#define LOG_ENTER() DEBUG_PID("-> %s\n", __func__)
#define LOG_EXIT() DEBUG_PID("<- %s\n", __func__)
#endif

#endif /* TINYCONTAINER_DEBUGGING_H */
/** @} */
