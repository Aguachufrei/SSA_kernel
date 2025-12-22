#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>
#include "clock.h"
#include "process.h"
#include "cpu.h"
#include "scheduler.h"
int main () {
	
	cpu_inicialize(4, 2);	
	
	//clock
	pthread_t clockthread; 
        pthread_create(&clockthread, NULL, clk, NULL);
        
	//scheduler
	pthread_t sthread; 
        pthread_create(&sthread, NULL, scheduler, NULL);
       
	//temps
	pthread_t* stopwatches = calloc(tmpCount, sizeof(pthread_t));	
	struct temp_arg* arguments = calloc(tmpCount, sizeof(struct temp_arg));
	arguments[0].frequency = 6;
	arguments[0].function = call_scheduler;
	arguments[1].frequency = 10;
	arguments[1].function = process_add_ready;
	arguments[2].frequency = 600;
        pthread_create(&stopwatches[0], NULL, stopwatch, &arguments[0]);
        pthread_create(&stopwatches[1], NULL, stopwatch, &arguments[1]);
        pthread_create(&stopwatches[2], NULL, stopwatch, &arguments[2]);
	
        pthread_exit(NULL);
        free(stopwatches);
        free(arguments);
}

