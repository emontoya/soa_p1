#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "config_info.h"

/*
 * Return the next thread to run
 */
struct mthread * scheduler_next(struct config_info *config);
#endif
