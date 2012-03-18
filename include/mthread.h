#ifndef MTHREAD_H
#define MTHREAD_H

#include <setjmp.h>

/*
 * Callback function definition.
 * This function is executed when a thread end
 */
typedef void (* mthread_callback)();

/**
 * Structure to handle the information regarding a thread
 */
struct mthread{
        int id; // thread identifyer (consecutive)
        long double cvalue; // Current calculated value (We add this here for simplicity)
        //ucontext_t *env;   // To store the thread state
        jmp_buf *env;   // To store the thread state
        int ticketc; // Tickets quantity
        int fticket; // Accumulated ticket sum or firts ticket number
        int workc;  // Assigned work; 
};

/*
 * Initialize the environment to create and destroy threads
 * params:
 *      func = callback function to handle thread ended
 */
void mthread_init_environment(mthread_callback func);

/*
 * Release the environment created to create and destroy threads
 */
void mthread_destroy_environment();

/*
 * Create and initialize a thread
 */
struct mthread * mthread_new(int id_p, int ticketc_p, int workc_p);

/*
 * Create the environment to execute the thread
 */
struct mthread * mthread_create(void (*func)()); 

/*
 * Destroy the thread specific information.
 */
struct mthread * mthread_free(struct mthread * thread);

/*
 * Run the thread
 */
void mthread_run(struct mthread *thread);

#endif
