#ifndef MTHREAD_H
#define MTHREAD_H

#include <setjmp.h>
#include <signal.h>

struct mthread;

/*
 * Callback function definition.
 * This function is executed when a thread end
 */
typedef void (* mthread_callback)(struct mthread *thread);

/**
 * Structure to handle the information regarding a thread
 */
struct mthread{
        int id; // thread identifyer (consecutive)
        long double cvalue; // Current calculated value (We add this here for simplicity)
        jmp_buf *env;   // To store the thread state
        long ticketc; // Tickets quantity
        long long fticket; // Accumulated ticket sum or firts ticket number
        long workc;  // Assigned work; 
        stack_t *stack; // Stack point where the thread is running
        void (*func)(); // Function to be executed by the thread
        mthread_callback callback; // Function to execute when the thread finished
        unsigned long long worku; // Current working unit
};

/*
 * Initialize the environment to create and destroy threads
 * params:
 *      func = callback function to handle thread ended
 */
void mthread_init_environment(jmp_buf *freturn_point);

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
struct mthread * mthread_init(struct mthread *thread, void (*func)(), mthread_callback callback); 

/*
 * Destroy the thread specific information.
 */
struct mthread * mthread_free(struct mthread * thread);
#endif
