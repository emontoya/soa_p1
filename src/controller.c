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
#include <time.h>
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
        // First call
        struct mthread * next = scheduler_next(cinfo);
        
        if (next != NULL){ // Next thread founded
                 
                // Update the interface
                if (cinfo->current != NULL){
                        thread_value_changed(cinfo->current->id, cinfo->current->cvalue, 
                                WORKING_UNIT * cinfo->current->workc, cinfo->current->worku);
                }

                cinfo->current = next;

                // Notify the change
                running_thread_changed(next->id);

                // Restart the timer
                setitimer(ITIMER_REAL, &timer_interval, NULL);
                
                siglongjmp(*(next->env), 1);
        } else { // We Finished processing
                // Update the interface
                if (cinfo->current != NULL){
                        thread_value_changed(cinfo->current->id, cinfo->current->cvalue, 
                                WORKING_UNIT * cinfo->current->workc, cinfo->current->worku);
                }
                
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
        unsigned long long j = 0;
        long double pi = 0;
        long double divd = 1;
        long double sig = 1;

        struct mthread * thread = cinfo->thread_list->threads[cinfo->thread_list->thread_indexs[thread_id]];
        
        unsigned long long w = 0;
        int k = 0;

        thread->worku = 0; 
        j = 0;
        while (w < thread->workc){
                for (k = 0; k < WORKING_UNIT; k++){
                        pi += sig * ((long double)1/divd); 
                        thread->cvalue = pi * (long double)4;
                        divd += 2;
                        sig *= -1;
                        thread->worku++;
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
                                thread_value_changed(thread->id, thread->cvalue, WORKING_UNIT * thread->workc, thread->worku);
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
        thread_value_changed(thread->id, thread->cvalue, WORKING_UNIT * thread->workc, thread->worku);
        fthread = thread;
}

void controller_alarm_handler(int sig){
        yield();
}

static void set_timer(int quantum){
        // configure the alarm
        struct sigaction act;
        struct timeval timer_slice;
        struct timeval timer_init;
        timer_slice.tv_sec = 0;
        timer_slice.tv_usec = 0;//quantum * 1000;
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
/*
static void print_loaded_info(){
        int i;
        for (i = 0; i < cinfo->thread_list->count; i++){
                printf("Thread: id =%d; index = %d; tickets = %ld; fticket = %lld; work = %ld\n",
                                cinfo->thread_list->threads[i]->id,
                                cinfo->thread_list->thread_indexs[cinfo->thread_list->threads[i]->id],
                                cinfo->thread_list->threads[i]->ticketc,
                                cinfo->thread_list->threads[i]->fticket,
                                cinfo->thread_list->threads[i]->workc);
        }
}
*/
void controller_init(){

        mthread_finished = malloc(sizeof(jmp_buf));

        // Load the configuration
        cinfo = config_handler_load();

        // Indicate that there is any thread running
        cinfo->running = 0;

        // Initialize thread environment
        mthread_init_environment(&wfinished);//mthread_finished);

        // Initialize the threads
        init_threads();
        
        if (cinfo->is_preemptive){
                // Initialize the timer
                set_timer(cinfo->quantum);
        }
        
        setup_interface();

        create_ui_main(cinfo->thread_list->count);

        launch_interface();
}

void controller_start(){
        int i;

        for (i = 0; i < cinfo->thread_list->count; i++){
                set_tickets_count(cinfo->thread_list->threads[i]->id,
                                cinfo->thread_list->threads[i]->ticketc);
        }

        // Seed the pseudo-random number generator
        srand(time(0));


        if (0 == sigsetjmp(wfinished, 1)){
                if (cinfo->is_preemptive){
                        // Start timer
                        setitimer(ITIMER_REAL, &timer_interval, NULL);

                        while(1); // Wait for the timer
                } else {
                        yield();
                }
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

                setitimer(ITIMER_REAL, NULL, NULL);

                // All threads finished
        }
}

void controller_end(){
        // Destroy the thread handling environment
        //mthread_destroy_environment();
}
