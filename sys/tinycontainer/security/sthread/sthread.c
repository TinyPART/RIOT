/*
 * Copyright (C) 2020-2024 Orange
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
 * @brief       TinyContainer Secure Thread sub-module implementation
 *
 * This implementation assume a 32 bit architecture where memory address and
 * pointer are equal and could be cast to uint32_t.
 *
 * @author      BERKANE Ghilas (ghilas.berkane@gmail.com)
 * @author      Samuel Legouix <samuel.legouix@orange.com>
 *
 * @}
 */

#define DEBUG_ENABLE
#include "debug.h"

#include <stdalign.h>

#include "thread.h"

#include "tinycontainer/security/sthread/sthread.h"

/* retrieve the secure context which is on top of thread control block */
static uintptr_t get_secure_context(void)
{
    uintptr_t ptr = (uintptr_t)thread_get(thread_getpid());

    ptr += sizeof(thread_t);
    return ptr;
}

/* retrieve the caller context which is the first element of the secure
 * context
 */
static void *get_caller_context(void)
{
    uint32_t *pctx = (uint32_t *)get_secure_context();

    return (void *)pctx[0];
}

/* retrieve the callee context which is the second element of the secure
 * context
 */
static void *get_callee_context(void)
{
    uint32_t *pctx = (uint32_t *)get_secure_context();

    return (void *)pctx[1];
}

/* retrieve the caller callback which is the third element of the secure
 * context
 */
static sthread_callback_t get_caller_callback(void)
{
    uint32_t *pctx = (uint32_t *)get_secure_context();

    return (sthread_callback_t)pctx[2];
}

/* the secure wrapper:
 *     - retrieves the secure context associated to this thread
 *     - calls the task function with the callee context
 *     - calls the caller callback with caller context
 *     - returns the same value as returned by the task function
 */
static void *secure_wrapper(void *arg)
{

    DEBUG("[%d] -> sthread:secure_wrapper()\n", thread_getpid());

    /* get the thread task function from arg */
#ifndef BOARD_NATIVE
    thread_task_func_t task_func = (thread_task_func_t)arg;
#else /*BOARD_NATIVE*/
    /* note: gcc complains about direct cast on native: "ISO C forbids
     *       conversion of object pointer to function pointer type"
     */
    thread_task_func_t task_func;
    *((uint32_t *)&task_func) = (uint32_t)arg;
#endif

    /* first call the thread task function */
    void *callee_context = get_callee_context();
    void *ret = task_func(callee_context);

    /* and next the caller callback */
    sthread_callback_t callback = get_caller_callback();
    if (callback != NULL) {
        void *caller_context = get_caller_context();
        kernel_pid_t pid = thread_getpid();
        callback(pid, caller_context);
    }

    DEBUG("[%d] <- sthread:secure_wrapper()\n", thread_getpid());

    return ret;
}

/* This function anticipates where the Thread Block Control will be assignedand
 * uses the address just above the TBC as the start for our own data (the secure
 * context)
 */
static uintptr_t secure_context_address(char *stack, int stacksize)
{

    DEBUG("[%d] -> sthread:secure_context_address()\n", thread_getpid());

    /* the code below is duplicated from thread_create (see core/thread.c) */
    uintptr_t misalignment = (uintptr_t)stack % alignof(void *);

    if (misalignment) {
        misalignment = alignof(void *) - misalignment;
        stack += misalignment;
        stacksize -= misalignment;
    }
    stacksize -= sizeof(thread_t);

    /* finalize the stack alignment (see core/thread.c) */
    stacksize -= stacksize % alignof(thread_t);

    uintptr_t top_of_tbc = (uintptr_t)stack + stacksize + sizeof(thread_t);

    DEBUG("[%d] -- sthread:secure context address 0x%#x\n",
          thread_getpid(), top_of_tbc);

    DEBUG("[%d] <- sthread:secure_context_address()\n", thread_getpid());

    /* we can now return the address of the secure context */
    return top_of_tbc;
}

/* Public functions
 * ----------------
 */

kernel_pid_t sthread( void *caller_context,
                      void *callee_context,
                      sthread_callback_t caller_callback,
                      char *stack, int stacksize,
                      uint8_t priority, int flags,
                      thread_task_func_t task_func,
                      const char *name )
{
    DEBUG("[%d] -> sthread()\n", thread_getpid());

    kernel_pid_t pid;
    uint32_t *context;

    /* reserve some space for the secure context */
    stacksize -= sizeof(uint32_t) * 3;

    /* get the secure context address */
    context = (uint32_t *)secure_context_address(stack, stacksize);

    /* save the secure context on top of the stack */
    *context = (uint32_t)caller_context;
    context++;
    *context = (uint32_t)callee_context;
    context++;
    *context = (uint32_t)caller_callback;

    /* call the thread*/
    pid = thread_create(   stack,
                           stacksize,
                           priority,
                           flags,
                           secure_wrapper,  /*function*/
#ifndef BOARD_NATIVE
                           task_func,       /*arg*/
#else /*BOARD_NATIVE*/
                           /* HACK: the following ugly cast is required for.
                            *       native. See secure_wrapper() above.
                            */
                           (void *)*(uint32_t *)&task_func,
#endif /*BOARD_NATIVE*/
                           name
                           );

    DEBUG("[%d] -- pid %s = %d\n", thread_getpid(), name, pid);

    DEBUG("[%d] <- sthread()\n", thread_getpid());

    return pid;
}

void *sthread_getcalleecontext(void)
{
    return get_callee_context();
}
