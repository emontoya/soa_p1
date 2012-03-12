#ifndef MTHREAD_H
#define MTHREAD_H

#include <ucontext.h>

/*
 * Callback function definition.
 * This function is executed when a thread end
 */
typedef void (* mthread_callback)();

/**
 * Structure to handle the information regarding a thread
 */
struct mthread{
        int id;
        double cvalue;
        ucontext_t *env;   // To store the thread state
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
 * Initializes the threat structure
 */
struct mthread * mthread_create(int id, void (*func)()); 

/*
 * Destroy the thread specific information.
 */
struct mthread * mthread_free(struct mthread * thread);

/*
 * Run the thread
 */
void mthread_run(struct mthread *thread);

#endif
