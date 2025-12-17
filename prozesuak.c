#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "prozesuak.h"
#include "erlojua.h"
struct prozesu_ilara ready = {.prozesu_kantitatea = 0, .first = NULL, .last = NULL};
int current_id = 0;
pthread_mutex_t ready_mutex = PTHREAD_MUTEX_INITIALIZER;

void prozesu_add_ready(){
	prozesu_add(&ready);
}
void prozesu_add(struct prozesu_ilara *queue){
	printf(" process_add called...");
	pthread_mutex_lock(&ready_mutex);
	struct prozesu_ilara_node *nodoa = (struct prozesu_ilara_node*) malloc(sizeof(struct prozesu_ilara_node));
	nodoa->pcb.id = current_id;
	nodoa->pcb.priority = rand()%100;
	nodoa->pcb.lastTime = ssa_time;
	nodoa->next = NULL;
	current_id++;
	queue->prozesu_kantitatea++;
	if (queue->first == NULL){
		queue->first = nodoa;
		queue->last = nodoa;

	} else {
		queue->last->next = nodoa;
		queue->last = nodoa;
	}
	pthread_mutex_unlock(&ready_mutex);
}

struct PCB prozesu_peek(struct prozesu_ilara *queue){
	pthread_mutex_lock(&ready_mutex);
	return queue->first->pcb;
	pthread_mutex_unlock(&ready_mutex);
}

struct PCB prozesu_poll(struct prozesu_ilara *queue){
	pthread_mutex_lock(&ready_mutex);
	if (queue->prozesu_kantitatea == 0){
		struct PCB errore = { .id = -1 };
		return errore;
	}
	struct PCB cpcb = queue->first->pcb;
	queue->first = queue->first->next;
	queue->prozesu_kantitatea--;
	if(queue->first==NULL) queue->last = NULL;
	pthread_mutex_unlock(&ready_mutex);
	return cpcb;
}

void prozesu_push(struct prozesu_ilara *queue, struct PCB *pcb){
	pthread_mutex_lock(&ready_mutex);
	struct prozesu_ilara_node *nodoa = (struct prozesu_ilara_node*) malloc(sizeof(struct prozesu_ilara_node));
	nodoa->pcb = *pcb;
	nodoa->next = NULL;
	if(queue->last == NULL){
		queue->first = nodoa;
	} else {
		queue->last->next = nodoa;
	}
	queue->last = nodoa;
	queue->prozesu_kantitatea++;
	pthread_mutex_unlock(&ready_mutex);
}
struct prozesu_ilara_node *prozesu_get_next(struct prozesu_ilara_node *nodoa){
	return nodoa->next;
} 
struct PCB *prozesu_destroy_next(struct prozesu_ilara *queue, struct prozesu_ilara_node *nodoa){
	pthread_mutex_lock(&ready_mutex);
	struct PCB *current = &nodoa->next->pcb;
	if(nodoa->next == NULL){
		queue->first = NULL;
		queue->last = NULL;
	} else {
		nodoa->next = nodoa->next->next;
	}
	if(nodoa->next==NULL)queue->last = nodoa;
	queue->prozesu_kantitatea--;
	pthread_mutex_unlock(&ready_mutex);
	return current;
}
void printqueue(struct prozesu_ilara *q)
{
    if (!q) {
        printf("queue = NULL\n");
        return;
    }

    printf("Queue @ %p\n", (void *)q);
    printf("count = %d\n", q->prozesu_kantitatea);
    printf("first = %p, last = %p\n",
           (void *)q->first, (void *)q->last);

    struct prozesu_ilara_node *curr = q->first;
    int i = 0;

    while (curr) {
        printf("[%d] node=%p  id=%d  priority=%d  lastTime=%d  next=%p\n",
               i,
               (void *)curr,
               curr->pcb.id,
               curr->pcb.priority,
               curr->pcb.lastTime,
               (void *)curr->next);

        curr = curr->next;
        i++;
    }

    if (i != q->prozesu_kantitatea) {
        printf("WARNING: counted %d nodes but prozesu_kantitatea says %d\n",
               i, q->prozesu_kantitatea);
    }
}

