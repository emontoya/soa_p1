#ifndef MHEAP_H
#define MHEAP_H

#include "mthread_list.h"

/*
 * Adds a new thread to the Heap
 */
struct mthread * mheap_add(struct mthread_list * list, int tc, int wc);

struct mthread * mheap_remove(struct mthread_list * list, int thread_id);
#endif
