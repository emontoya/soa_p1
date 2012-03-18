#include "mthread_list.h"

#include <stdlib.h>


struct mthread_list * mthread_list_create(){
        struct mthread_list * list;

        list = (struct mthread_list *)malloc(sizeof(struct mthread_list)); 
        list->threads = malloc(MTHREAD_MAX * sizeof(struct mthread *));
        list->thread_indexs = malloc(MTHREAD_MAX * sizeof(int));
        list->count = 0;

        return list;
}

void mthread_list_destroy(struct mthread_list * list){
        // Releasing thread specific info
        int i;

        // Destroy thread info
        for (i = 0; i < list->count; i++){
                free(mthread_free(list->threads[i]));
        }

        // Destroy list threads
        free(list->threads);

        // Destroy thread index
        free(list->thread_indexs);

        // destroy the list
        free(list);
}
