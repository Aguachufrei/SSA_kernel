#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>
#include "erlojua.h"
#include "prozesuak.h"
#include "cpu.h"
#include "scheduler.h"
int main () {
	
	cpu_inicialize(4, 2);	
	
	//clock
	pthread_t clockthread; 
        pthread_create(&clockthread, NULL, erlojua, NULL);
        
	//scheduler
	pthread_t sthread; 
        pthread_create(&sthread, NULL, scheduler, NULL);
       
	//temps
	pthread_t* temporizadoreak = calloc(tmpCount, sizeof(pthread_t));	
	struct temp_arg* argumentuak = calloc(tmpCount, sizeof(struct temp_arg));
	argumentuak[0].maiztasuna = 6;
	argumentuak[0].funtzioa = call_scheduler;
	argumentuak[1].maiztasuna = 10;
	argumentuak[1].funtzioa = prozesu_add_ready;
	argumentuak[2].maiztasuna = 600;
        pthread_create(&temporizadoreak[0], NULL, tenporizadorea, &argumentuak[0]);
        pthread_create(&temporizadoreak[1], NULL, tenporizadorea, &argumentuak[1]);
        pthread_create(&temporizadoreak[2], NULL, tenporizadorea, &argumentuak[2]);
	
        pthread_exit(NULL);
        free(temporizadoreak);
        free(argumentuak);
	
}

