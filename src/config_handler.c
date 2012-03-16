#include "config_handler.h"
#include "config_info.h"
#include "mheap.h"
#include "string.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define CONFIG_BUFFER_SIZE 100

static void load_config(char *file_name, struct config_info *info){
        // TODO: Add validations
        FILE *file = fopen(file_name, "r");

        if(NULL == file){
                perror("Error loading config file");
        } 

        char buffer[CONFIG_BUFFER_SIZE + 1];

        buffer[CONFIG_BUFFER_SIZE] = '\0';

        // Read header info (preemptive condition, quantum)
        fgets(buffer, CONFIG_BUFFER_SIZE, file);
        sscanf(strtok(buffer, " "), "%d", &(info->is_preemptive));
        sscanf(strtok(NULL, " "), "%d", &(info->quantum));

        int id, tc, wc;

        info->thread_list->count = 0;

        // Read each thread configuration
        for(id = 0; id < MTHREAD_MAX && fgets(buffer, CONFIG_BUFFER_SIZE, file); id++){
                sscanf(strtok(buffer, " "), "%d", &tc);
                sscanf(strtok(NULL, " "), "%d", &wc);

                // Add the new thread to the heap
                mheap_add(info->thread_list, tc, wc);
        }

        fclose(file);
}

struct config_info * config_handler_load(){
        struct config_info *info;

        // Initializing the structures
        info = malloc(sizeof(struct config_info));
        info->thread_list = mthread_list_create();


        // Load the configuration from file
        load_config(CONFIG_FILE_NAME, info);

        return info;
}

void config_handler_destroy(struct config_info * info){
        // TODO: Add Validations

        // destroy thread list
        mthread_list_destroy(info->thread_list);

        // Destroy config info
        free(info);
}
