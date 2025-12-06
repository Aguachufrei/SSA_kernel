#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "scheduler.h"
#define MAIZTASUNA 1

//inicialization
extern pthread_mutex_t mutexC;
extern pthread_cond_t cond1;
extern pthread_cond_t cond2;
extern int tmpCount;
extern int ssa_time;
void *erlojua(void *args);
void *tenporizadorea(void *args);
struct temp_arg {
    int maiztasuna;
    void (*funtzioa)(void);
};
