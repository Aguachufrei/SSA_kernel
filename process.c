#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "process.h"
#include "clock.h"
struct process_queue ready = {.process_num = 0, .first = NULL, .last = NULL};
int current_id = 0;
pthread_mutex_t ready_mutex = PTHREAD_MUTEX_INITIALIZER;

void process_add_ready(){
	process_add(&ready);
}
void process_add(struct process_queue *queue){
	printf(" process_add called...");
	pthread_mutex_lock(&ready_mutex);
	struct process_node *node = (struct process_node*) malloc(sizeof(struct process_node));
	node->pcb.id = current_id;
	node->pcb.priority = rand()%100;
	node->pcb.lastTime = ssa_time;
	node->next = NULL;
	current_id++;
	queue->process_num++;
	if (queue->first == NULL){
		queue->first = node;
		queue->last = node;

	} else {
		queue->last->next = node;
		queue->last = node;
	}
	pthread_mutex_unlock(&ready_mutex);
}

struct PCB process_peek(struct process_queue *queue){
	pthread_mutex_lock(&ready_mutex);
	return queue->first->pcb;
	pthread_mutex_unlock(&ready_mutex);
}

struct PCB process_poll(struct process_queue *queue){
	pthread_mutex_lock(&ready_mutex);
	if (queue->process_num == 0){
		struct PCB err = { .id = -1 };
		return err;
	}
	struct PCB cpcb = queue->first->pcb;
	queue->first = queue->first->next;
	queue->process_num--;
	if(queue->first==NULL) queue->last = NULL;
	pthread_mutex_unlock(&ready_mutex);
	return cpcb;
}

void process_push(struct process_queue *queue, struct PCB *pcb){
	pthread_mutex_lock(&ready_mutex);
	struct process_node *node = (struct process_node*) malloc(sizeof(struct process_node));
	node->pcb = *pcb;
	node->next = NULL;
	if(queue->last == NULL){
		queue->first = node;
	} else {
		queue->last->next = node;
	}
	queue->last = node;
	queue->process_num++;
	pthread_mutex_unlock(&ready_mutex);
}
struct process_node *process_get_next(struct process_node *node){
	return node->next;
} 
struct PCB *process_destroy_next(struct process_queue *queue, struct process_node *node){
	pthread_mutex_lock(&ready_mutex);
	struct PCB *current = &node->next->pcb;
	if(node->next == NULL){
		queue->first = NULL;
		queue->last = NULL;
	} else {
		node->next = node->next->next;
	}
	if(node->next==NULL)queue->last = node;
	queue->process_num--;
	pthread_mutex_unlock(&ready_mutex);
	return current;
}
void process_print(struct process_queue *queue){
    printf("Queue @ %p\n", queue);
    printf("count = %d\n", queue->process_num);
    printf("first = %p, last = %p\n", queue->first, queue->last);

    struct process_node *current = queue->first;
    int i = 0;
    while (current) {
        printf("[%d] node=%p  id=%d  priority=%d  lastTime=%d  next=%p\n",
               i,
               current,
               current->pcb.id,
               current->pcb.priority,
               current->pcb.lastTime,
               current->next);

        current = current->next;
        i++;
    }

    if (i != queue->process_num) {
        printf("WARNING: counted %d nodes but process_num says %d\n", i, queue->process_num);
    }
}

