#include "scheduler.h"
#include "mheap.h"

#include <stdlib.h>

struct mthread * scheduler_next(struct config_info *config){
        struct mthread *result = NULL; 

        struct mthread_list * list = config->thread_list;

        if (list->count == 0){
                return NULL;
        }

        // TODO: Implement Lottery scheduling
        // Temp implementation (Round-robin)
        struct mthread * current = config->current;
        int pos = list->thread_indexs[current->id];

        if (pos + 1 < list->count){
                result = list->threads[pos + 1];
        } else {
                result = list->threads[0];
        }

       return result;
}
