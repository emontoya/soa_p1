#include "mthread.h"

#include <assert.h>
#include <stdlib.h>

// Size for the stack created for each thread
#define MTHREAD_STACK_SIZE 4096

// Stack for the ended thread context 
#define MTHREAD_ENDED_STACK_SIZE 1024

/*
 * Stack for the function in charge of releasing
 * allocated threads
 */
static char mthread_ended_stack[MTHREAD_ENDED_STACK_SIZE];

/*
 * Context variable to handle the thread ended
 */
ucontext_t mthread_ended_env;

/*
 * Context used as a reference for swapping
 * to the thread context
 */
ucontext_t mthread_env;

void mthread_ended(struct mthread * thread){
        assert(thread != NULL); 

        free(thread->env->uc_stack.ss_sp);// TODO: Confirm that the pointer hasn't changed
        free(thread->env);


}

void mthread_init_environment(mthread_callback func){
        getcontext(&mthread_ended_env);
        mthread_ended_env.uc_stack.ss_sp = mthread_ended_stack;
        mthread_ended_env.uc_stack.ss_size = MTHREAD_ENDED_STACK_SIZE;

        makecontext(&mthread_ended_env, func, 0);

        getcontext(&mthread_env);
}

void mthread_destroy_environment(){
        // TODO: Add destroy logic if needed;
}

struct mthread * mthread_create(int id, void (*func)()){ 
        // TODO: Add validations

        struct mthread *thread = malloc(sizeof(struct mthread));
        assert(thread != NULL);

        thread->id = id;
        thread->cvalue = 0;

        // Create the stack for this thread
        char *stack = malloc(MTHREAD_STACK_SIZE * sizeof(char));
        
        thread->env = malloc(sizeof(ucontext_t));
        assert(thread->env != NULL);

        // Initialize the thread context
        getcontext(thread->env);

        /*
         * Changing the thread context to use the new stack
         * and to point to the function
         */
        thread->env->uc_stack.ss_sp = stack;
        thread->env->uc_stack.ss_size = MTHREAD_STACK_SIZE * sizeof(char);
        thread->env->uc_link = &mthread_ended_env; // Context for thread ended
        makecontext(thread->env, func, 0);
        
        // Return the created thread
        return thread;
}

void mthread_run(struct mthread *thread){
        // TODO: Add validations

        swapcontext(&mthread_env, thread->env);
}

struct mthread * mthread_free(struct mthread * thread){
        assert(thread != NULL); 

        free(thread->env->uc_stack.ss_sp);// TODO: Confirm that the pointer hasn't changed
        free(thread->env);

        return thread;
}
