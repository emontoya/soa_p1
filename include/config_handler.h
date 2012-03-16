#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H

#include "config_info.h"

#define CONFIG_FILE_NAME "config.txt"

/*
 * Load the configuration from file config.txt
 *
 */
struct config_info * config_handler_load();

/*
 * Release the configuration info
 */
void config_handler_destroy(struct config_info * info);
#endif
