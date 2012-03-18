#ifndef MTHREAD_LIST_H
#define MTHREAD_LIST_H

#include "mthread.h"

#define MTHREAD_MAX 50

struct mthread_list {
        int count;
        // Threads array
        struct mthread **threads;
        // Index to thread id
        int *thread_indexs;
};

/*
 * Create the list structure
 */
struct mthread_list * mthread_list_create();

/*
 * Destroy the list and the threads containned
 */
void mthread_list_destroy(struct mthread_list * list);
#endif
