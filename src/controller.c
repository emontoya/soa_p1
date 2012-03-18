#include "controller.h"
#include "mthread.h"
#include "config_handler.h"
#include "mthread_list.h"
#include "iinterface.h"
#include "scheduler.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <setjmp.h>

#define WORKING_UNIT 50

// Finished jump
jmp_buf wfinished;

// Configuration info
struct config_info * cinfo;

// For test purposes
int finisheds[2];
int finished = 0;
int started = 0;

// Alarm interval (quantum)
struct itimerval timer_interval;

/*
 * Function for a thread to release the processor
 */
void yield(){
        if (0 == sigsetjmp(*(cinfo->current->env), 1)){
                // First call
                struct mthread * next = scheduler_next(cinfo);

                if (next != NULL){ // Next thread founded
                        cinfo->current = next;
                        siglongjmp(*(next->env), 1);
                } else { // We Finished processing
                        // Jump to the finished point
                        siglongjmp(wfinished, 1);
                }
        }
        

}

/*
 * Function to aproximate the PI value:
 * PI =  4 * arctan(1)  
 * = 4 * (1 - 1/3 + 1/5 - 1/7 + 1/9 - 1/11 ... + (((-1)^n)/(2n + 1)))
 */
void calc_function(int thread_id){
        unsigned long long i = 0;
        long double pi = 0;
        long double divd = 1;
        long double sig = 1;

        struct mthread * thread = cinfo->thread_list->threads[cinfo->thread_list->thread_indexs[thread_id]];
        
        unsigned long long w = 0;
        int k = 0;

        while (w < thread->workc){
                for (k = 0; k < WORKING_UNIT; k++){
                        pi += sig * ((long double)1/divd); 
                        thread->cvalue = pi * (long double)4;
                        divd += 2;
                        sig *= -1;
                        i++;
                        
                        /*
                         * Yield the processor if not preemtive scheduling and
                         * the allowed portion of the work is done.
                         * Note: We could code this in different way to prevent
                         * checking this condition every time but, coded this
                         * way is simple to undertand and, any way, the idea is
                         * to increase the processing load. 
                         */
                        if (!cinfo->is_preemptive && i > thread->ticketc * cinfo->quantum){
                                yield();
                        }
                }
        }
}

/*
 * Handles the logic for thread ending
 */
void thread_ended(){
        // TODO: Implement thread ending logic        
        printf("Thread finished\n");

        mthread_free(cinfo->current);

       finisheds[cinfo->current->id] = 1;

       finished = finisheds[0] + finisheds[1] > 1 ? 1 : 0; 

       if (finished){
                timer_interval.it_value.tv_usec = 0;
       }
}

int k = 0;
void controller_alarm_handler(int sig){
        // TODO: Implement alarm logic
        /*
        if (!started){
                started = 1;
                current = threads[0];
                setcontext(threads[0]->env);
        }
        
        int c = current->id;
        int next = 2 - (c + 1);

        printf("Current = [%d]; next = [%d]\n", c, next);

        if (!finished && !finisheds[next]){
                current = threads[next];
                swapcontext(threads[c]->env, threads[next]->env); 
        }
        */
}

static void set_timer(int quantum){
        // configure the alarm
        struct sigaction act;
        struct timeval timer_slice;
        struct timeval timer_init;
        timer_slice.tv_sec = 0;
        timer_slice.tv_usec = quantum * 1000;
        timer_init.tv_sec = 0;
        timer_init.tv_usec =  quantum * 1000;
        timer_interval.it_value = timer_init;
        timer_interval.it_interval = timer_slice;
        act.sa_handler = controller_alarm_handler;
        sigaction (SIGALRM, &act, 0);
}

void controller_init(){
        printf("Controller initialized;\n");

        // Load the configuration
        cinfo = config_handler_load();

        printf("Configuration readed: is_preemptive = %d; quantum = %d\n", cinfo->is_preemptive, cinfo->quantum);
        int i;
        for (i = 0; i < cinfo->thread_list->count; i++){
                printf("Thread: id =%d; tickets = %d; fticket = %d; work = %d\n",
                                cinfo->thread_list->threads[i]->id,
                                cinfo->thread_list->threads[i]->ticketc,
                                cinfo->thread_list->threads[i]->fticket,
                                cinfo->thread_list->threads[i]->workc);
        }

        // Initialize the timer
        set_timer(cinfo->quantum);

        // Initialize thread environment
        //mthread_init_environment(thread_ended);

        
        // Test thread creation
        //threads = malloc(2 * sizeof(struct mthread *));
        
        //threads[0] = mthread_create(calc_function);
        //threads[1] = mthread_create(calc_function);
        //current = threads[0];
        
        //finished = 0;
        //memset(finisheds, 0, 2 + sizeof(int));

}

void controller_start(){
        //mthread_run(current);


        // Start timer
        //setitimer(ITIMER_REAL, &timer_interval, NULL);

        //while(!finished);
}

void controller_end(){
        // Destroy the thread handling environment
        //mthread_destroy_environment();
}
