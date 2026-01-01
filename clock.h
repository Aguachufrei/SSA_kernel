#ifndef CLOCK_H
#define CLOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "scheduler.h"

//inicialization
extern pthread_mutex_t mutexC;
extern pthread_cond_t cond1;
extern pthread_cond_t cond2;
extern int tmpCount;
extern int se_time;
void *clk(void *args);
void *stopwatch(void *args);
struct temp_arg {
    int frequency;
    void (*function)(void);
};
#endif
