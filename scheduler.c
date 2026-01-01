#include <semaphore.h>
#include <unistd.h>
#include "process.h"
#include "clock.h"
#include "cpu.h"

sem_t mutexS;

void *scheduler(void *args) {
	sem_init(&mutexS, 1, 0);
	while(1) {
		sem_wait(&mutexS);

		//Prozesagailuen dauden aginduak berreskuratu
		for(int i = 0; i<cpu.coreNum; i++){
			for(int j = 0; j<cpu.hthreadNum; j++){
				if(cpu.cores[i][j]==NULL)continue;
				process_push(&ready, cpu.cores[i][j]);
			}
		}
		//process_print(&ready);
		for(int i = 0; i<cpu.coreNum; i++){
			for(int j = 0; j<cpu.hthreadNum; j++){
				
				struct process_node *current = ready.first;
				if(current == NULL){
					continue;
				}

				struct process_node *best = current;
				struct process_node *previous = current;
				struct process_node *previousBest = current;
				int max_value = current->pcb->priority + se_time - current->pcb->lastTime;        

				while((current = process_get_next(current)) != NULL){
					if(current->pcb->priority + se_time - current->pcb->lastTime > max_value){
						best = current;
						previousBest = previous;
						max_value = current->pcb->priority + se_time - current->pcb->lastTime;        
					}  
					previous=current;
				}
				best->pcb->lastTime = se_time;
				struct PCB *bestpcb;
				if(best == previousBest){
					bestpcb = process_poll(&ready);
				} else { 
					bestpcb = process_destroy_next(&ready, previousBest);
				}
				//process_print(&ready);
				cpu.cores[i][j]=bestpcb;
			}
		}
	}
}


void call_scheduler(){
	sem_post(&mutexS);
}
