#include <semaphore.h>
#include <stdio.h>
#include "prozesuak.h"
extern sem_t mutexS;
void *scheduler(void *args);
void call_scheduler();
