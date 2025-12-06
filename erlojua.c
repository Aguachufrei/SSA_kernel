#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "scheduler.h"
#include "erlojua.h"
#define MAIZTASUNA 1


//inicialization
pthread_mutex_t mutexC = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
int tmpCount = 3;
int done = 0;
int ssa_time = 1;

void *erlojua(void *args){
	printf("clock initialized\n");
	while (1) {
		pthread_mutex_lock(&mutexC);
		while (tmpCount > done){
			
			pthread_cond_wait(&cond1, &mutexC);
		}
	    ssa_time++;	
		printf("\nclk(%d): ",ssa_time);	
		usleep(300000);
		done = 0; 
		pthread_cond_broadcast(&cond2);
		pthread_mutex_unlock(&mutexC);
	}
}

void *tenporizadorea(void *args){
	struct temp_arg *argstruct = (struct temp_arg *)args;
	pthread_mutex_lock(&mutexC);
	int i= 0; 
	while (1) {
		i++;	
		done++;
		if(i==argstruct->maiztasuna){
			i = 0;
			//do sth
			printf(" <-- tclk ( %d )",argstruct->maiztasuna);
			if (argstruct->funtzioa != NULL) {
				argstruct->funtzioa();
			}
		}	
		pthread_cond_signal(&cond1);
		pthread_cond_wait(&cond2, &mutexC);
	}
}
