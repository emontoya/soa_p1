#ifndef IINTERFACE_H
#define IINTERFACE_H

/*
 * Function to notify that the running thead changed
 * params:
 *      thread_id = Id of the thread that is running now
 */
void running_thread_changed(int thread_id);

/*
 * Function to notify that the value calculated by the
 * running thread has changed
 * params:
 *      total: Total terms to calculate
 *      actual: Number of terms calculated
 */
void thread_value_changed(int thread_id, long double nvalue, unsigned long long total, unsigned long long actual);

void setup_interface();

void create_ui_main(int threads_count);

void launch_interface();

void set_tickets_count(int thread_id, long ticketc);


#endif
