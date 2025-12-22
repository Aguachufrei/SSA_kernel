#include <semaphore.h>
#include <stdio.h>
#include "process.h"
#include "clock.h"
extern sem_t mutexS;
void *scheduler(void *args);
void call_scheduler();
