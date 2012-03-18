#include "mheap.h"
#include <stdio.h>


struct mthread * mheap_add(struct mthread_list * list, int tc, int wc){
        // TODO: implement the Heap insertion
        // TODO: Add validations

        // This is a temp implementation
        list->thread_indexs[list->count] = list->count;
        return list->threads[list->count++] = mthread_new(list->count, tc, wc);
}

struct mthread * mheap_remove(struct mthread_list * list, int thread_id){
        int pos = list->thread_indexs[thread_id];

        printf("Removing thread %d; pos = %d\n", thread_id, pos);

        struct mthread *thread = list->threads[pos];
        list->thread_indexs[list->threads[pos]->id] = -1;
        
        for(; pos < list->count - 1; pos++){
                list->thread_indexs[list->threads[pos + 1]->id]--;
                list->threads[pos] = list->threads[pos + 1];
       }

        list->count--;
        printf("Threads count = %d\n", list->count);

        if (list->count > 0){
                printf("Threads[0] = %d\n", list->threads[0]->id);
        }
        
        return thread;
}
