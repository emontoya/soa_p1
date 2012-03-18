#include "mthread.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

// Size for the stack created for each thread
#define MTHREAD_STACK_SIZE 1024*64

/*
 * Variable to handle the thread creation state
 */
static jmp_buf mthread_create_env;

/*
 * Variable to handle the thread ended state
 */
static jmp_buf *mthread_finished;

/*
 * To store the current thread creation
 */
static struct mthread *current;

void mthread_init_environment(jmp_buf *freturn_point){
        mthread_finished = freturn_point;
        current = NULL;
        
}

void mthread_destroy_environment(){
        // TODO: Add destroy logic if needed;
}


struct mthread * mthread_new( int id_p, int ticketc_p, int workc_p){
        // TODO: Add validations
        struct mthread *thread = malloc(sizeof(struct mthread));

        thread->id = id_p;
        thread->ticketc = ticketc_p;
        thread->workc = workc_p;
        thread->fticket= 0;
        thread->cvalue = 0;

        return thread;
}

/*
 * Wraps the thread creation and the first call 
 *
 */
static void mthread_signal_helper( int arg )
{
        struct mthread *c = current;
        printf("Signal received: function f(%d)\n", c->id);

        if (sigsetjmp(*(c->env), 1)){
                printf("Calling the function f(%d)\n", c->id);
                // Call the function
                c->func(c->id);
                
                printf("Calling the callback: function f(%d)\n", c->id);
                // Call the callback function
                c->callback(c);

                printf("Jumping to the return point: function f(%d)\n", c->id);
                // Jump to the return point
                siglongjmp(*mthread_finished, 1);
                
        } else { 
                printf("Returning from first call to function f(%d)\n", c->id);
                /*
                 * Return inmediately but leaving the function on the stack 
                 */
                siglongjmp(mthread_create_env, 1);
        }
}

/*
 * Creates a new stack
 */
static stack_t * create_new_stack(){
        stack_t *stack = malloc(sizeof(stack_t));
        
        // Create the new stack
        stack->ss_flags = 0;
        stack->ss_size = MTHREAD_STACK_SIZE;
        stack->ss_sp = malloc(MTHREAD_STACK_SIZE);

        return stack;
}

struct mthread * mthread_init(struct mthread *thread, void (*func)(), mthread_callback callback){ 
        // TODO: Add validations

        printf("Starting thread %d creation\n", thread->id);
        struct sigaction sa;

        // Create the stack
        stack_t *stack = create_new_stack();

        // Prepare the signal to be handle in the new stack
        sigaltstack(stack, 0);
        sa.sa_handler = &mthread_signal_helper;
        sa.sa_flags = SA_ONSTACK;
        sigemptyset(&sa.sa_mask );
        sigaction(SIGUSR1, &sa, 0);

        if (0 == sigsetjmp(mthread_create_env, 1)){
                printf("Initializing thread %d\n", thread->id);

                // Set the thread to current to be read on signal handler
                current = thread;

                // Initialize the thread info
                current->func = func;
                current->cvalue = 0;
                current->stack  = stack;
                current->env = malloc(sizeof(jmp_buf));
                current->callback = callback;

                printf("Signal raised\n");
                // To trigger the signal
                raise(SIGUSR1);
                printf("After signal raised\n");
        }

        printf("Finished thread %d creation\n", thread->id);

        return thread;
}

struct mthread * mthread_free(struct mthread * thread){
        assert(thread != NULL); 
        /*
        free(thread->env->uc_stack.ss_sp);// TODO: Confirm that the pointer hasn't changed
        */
        free(thread->env);

        return thread;
}
