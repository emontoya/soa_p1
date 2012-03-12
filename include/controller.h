#ifndef CONTROLLER_H
#define CONTROLLER_H

/**
 * Function to initialize the controller
 */
void controller_init();

/**
 * Start the thread simulation
 */
void controller_start();

/*
 * Finish thread simulation
 */
void controller_end();

/*
 * Routine to handle the alarm
 */
void controller_alarm_handler(int sig);
#endif
