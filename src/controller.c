#include "controller.h"
#include "mthread.h"
#include "config_handler.h"
#include "mthread_list.h"
#include "iinterface.h"
#include "scheduler.h"
#include "iinterface.h"
#include "mheap.h"

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

// Thread return point 
static jmp_buf *mthread_finished;

// Alarm interval (quantum)
struct itimerval timer_interval;

// Thread just finished working
static struct mthread * fthread;

static void yield_jump(){
        //printf("New thread just to be resumed\n");
        // First call
        struct mthread * next = scheduler_next(cinfo);
        
        if (next != NULL){ // Next thread founded
                cinfo->current = next;
                // Notify the change
                running_thread_changed(next->id);
                siglongjmp(*(next->env), 1);
        } else { // We Finished processing
                // Jump to the finished point
                siglongjmp(wfinished, 1);
        }
}

/*
 * Function for a thread to release the processor
 */
void yield(){
        if (!cinfo->running){
                cinfo->current = NULL;
                cinfo->running = 1;
                yield_jump();
        } else if (0 == sigsetjmp(*(cinfo->current->env), 1)){
                yield_jump();
        }
}

/*
 * Function to aproximate the PI value:
 * PI =  4 * arctan(1)  
 * = 4 * (1 - 1/3 + 1/5 - 1/7 + 1/9 - 1/11 ... + (((-1)^n)/(2n + 1)))
 */
void calc_function(int thread_id){
        unsigned long long i = 0;
        unsigned long long j = 0;
        long double pi = 0;
        long double divd = 1;
        long double sig = 1;

        struct mthread * thread = cinfo->thread_list->threads[cinfo->thread_list->thread_indexs[thread_id]];
        
        unsigned long long w = 0;
        int k = 0;

        j = 0;
        while (w < thread->workc){
                for (k = 0; k < WORKING_UNIT; k++){
                        pi += sig * ((long double)1/divd); 
                        thread->cvalue = pi * (long double)4;
                        divd += 2;
                        sig *= -1;
                        i++;
                        j++;
                        
                        /*
                         * Yield the processor if not preemtive scheduling and
                         * the allowed portion of the work is done.
                         * Note: We could code this in different way to prevent
                         * checking this condition every time but, coded this
                         * way is simple to undertand and, any way, the idea is
                         * to increase the processing load. 
                         */
                        if (!cinfo->is_preemptive && j > thread->ticketc * cinfo->quantum){
                                j = 0;
                                //thread_value_changed(thread->id, thread->cvalue, WORKING_UNIT * thread->workc, i);
                                yield();
                        }
                }

                w++;
        }
}

/*
 * Handles the logic for thread ending
 */
void thread_ended(struct mthread * thread){
        // TODO: Implement thread ending logic        
        printf("Thread %d finished: result = %Lf\n", thread->id, thread->cvalue);

        fthread = thread;
}

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

static void init_threads(){
        int i;

        for (i = 0; i < cinfo->thread_list->count; i++){
                mthread_init(cinfo->thread_list->threads[i], &calc_function, &thread_ended);
        }
}

static void print_loaded_info(){
        printf("Configuration readed: is_preemptive = %d; quantum = %d\n", cinfo->is_preemptive, cinfo->quantum);
        int i;
        for (i = 0; i < cinfo->thread_list->count; i++){
                printf("Thread: id =%d; index = %d; tickets = %d; fticket = %d; work = %ld\n",
                                cinfo->thread_list->threads[i]->id,
                                cinfo->thread_list->thread_indexs[cinfo->thread_list->threads[i]->id],
                                cinfo->thread_list->threads[i]->ticketc,
                                cinfo->thread_list->threads[i]->fticket,
                                cinfo->thread_list->threads[i]->workc);
        }
}

void controller_init(){
        printf("Starting Controller initialization\n");

        mthread_finished = malloc(sizeof(jmp_buf));

        // Load the configuration
        cinfo = config_handler_load();

        // Indicate that there is any thread running
        cinfo->running = 0;

        print_loaded_info();

        // Initialize thread environment
        mthread_init_environment(&wfinished);//mthread_finished);

        // Initialize the threads
        init_threads();
        
        if (!cinfo->is_preemptive){
                // Initialize the timer
                set_timer(cinfo->quantum);
        }
        
        printf("Finished controller initialization\n");
}

void controller_start(){
        printf("Starting to run threads\n");

        if (cinfo->is_preemptive){
                // Start timer
                setitimer(ITIMER_REAL, &timer_interval, NULL);
        }

        if (0 == sigsetjmp(wfinished, 1)){
                yield();
        } else { 
                while (cinfo->thread_list->count > 0){
                        // Indicate that there is no thread running
                        cinfo->running = 0;
                        cinfo->current = NULL;

                        // Remove the thread from the list
                        mheap_remove(cinfo->thread_list, fthread->id);

                        // Release the resources allocated by this thread
                        mthread_free(fthread);

                        if (0 == sigsetjmp(wfinished, 1)){
                                yield();
                        }
                } 

                // All threads finished
                printf("All threads finished\n");
        }
        printf("Finished running threads\n");
}

void controller_end(){
        // Destroy the thread handling environment
        //mthread_destroy_environment();
}

/************************************************************
 * This section needs to be removed from here
 ************************************************************/
void running_thread_changed(int thread_id){
        printf("Now running thread %d\n", thread_id);
}

void thread_value_changed(int thread_id, long double nvalue, unsigned long long total, unsigned long long actual){
        printf("Thread %d (%llu / %llu * 100 = %%%Lf) = %Lf\n", thread_id, actual, total, (long double)actual / (long double)total * 100, nvalue);
}
