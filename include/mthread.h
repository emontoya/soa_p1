#ifndef MTHREAD_H
#define MTHREAD_H

#include <setjmp.h>

/**
 * Structure to handle the information regarding a thread
 */
struct mthread{
        int id;
        double cvalue;
        jmp_buf *env;   // To store the thread state
}

/**
 * Initializes the threat structure
 */
struct mthread * mthread_init(struct mthread * thread, int id); 
#endif
