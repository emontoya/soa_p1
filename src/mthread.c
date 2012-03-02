#include "mthread.h"

#include <assert.h>
#include <stdlib.h>

struct mthread * mthread_init(struct mthread * thread, int id){ 
        assert(thread != NULL);

        thread->id = id;
        thread->cvalue = 0;
        thread->env = (jmp_buf *)malloc(sizeof(jmp_buf));

        return thread;
}

struct mthread * mthread_free_data(struct mthread * thread){
        assert(thread != NULL); 

        free(thread->env);

        return thread;
}
