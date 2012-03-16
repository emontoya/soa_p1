#include "mheap.h"


struct mthread * mheap_add(struct mthread_list * list, int tc, int wc){
        // TODO: implement the Heap insertion
        // TODO: Add validations

        // This is a temp implementation
        return list->threads[list->count++] = mthread_new(list->count, tc, wc);
}
