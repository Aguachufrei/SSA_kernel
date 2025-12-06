#include <semaphore.h>
#include <stdio.h>
#include "prozesuak.h"
sem_t mutexS;


void *scheduler(void *args) {
	sem_init(&mutexS, 1, 0);
	while(1) {
		sem_wait(&mutexS);
		printf(" scheduler called...");
		struct PCB hurrengoprograma = prozesu_poll(&ready);
		prozesu_push(&ready, &hurrengoprograma);
	}
}
void call_scheduler(){
	sem_post(&mutexS);
}
