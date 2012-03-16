#ifndef CONFIG_INFO_H
#define CONFIG_INFO_H

#include "mthread_list.h"

struct config_info{
        int is_preemptive; // Preemptive or not
        int quantum; // time slice per thread

        // Structure containing the threads info 
        struct mthread_list *thread_list;
};
#endif
