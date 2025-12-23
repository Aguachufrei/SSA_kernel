#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include "process.h"
#include "memory.h" 
#include "clock.h"
struct process_queue ready = {.process_num = 0, .first = NULL, .last = NULL};
int current_id = 0;
pthread_mutex_t ready_mutex = PTHREAD_MUTEX_INITIALIZER;

//////////
//LOADER//
//////////

void process_add_ready(){
	process_add(&ready);
}


void process_add(struct process_queue *queue){
	printf(" process_add called...");
	pthread_mutex_lock(&ready_mutex);
	struct process_node *node = malloc(sizeof(struct process_node));
	node->pcb.id = current_id;
	node->pcb.priority = rand()%100;
	node->pcb.lastTime = ssa_time;
	node->pcb.page_entry = malloc(PAGE_NUM*sizeof(struct page_entry));
	for(int i = 0; i < PAGE_NUM; i++){
		node->pcb.page_entry[i].free=0;
	}
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


void process_loader(struct process_queue *queue, uint8_t *name, int priority){
	printf("\n_______Loader called_______\n");

	FILE *f = fopen(name, "rb");
	if(!f){
		printf("\nFile %s not found\n", name);
		return;
	}

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	rewind(f);

	uint8_t *data = malloc((size+1)*sizeof(uint8_t));
	fread(data, 1, size, f);	
	data[size] = '\0';

	printf("PROGRAM_DATA: (size %ld)\n", size);
	//process_print_hex(data, size);

	struct process_node *node = malloc(sizeof(struct process_node));
	node->pcb.id = current_id;
	if(priority>=100)printf("\nWARNING priority exceeding maximun value.\nValue range [0, 100); Recieved value: %d\n", priority);
	node->pcb.priority = priority%100;
	node->pcb.lastTime = ssa_time;
	node->pcb.page_entry = malloc(PAGE_NUM*sizeof(struct page_entry));

	int page_num = size / PAGE_SIZE;
	printf("%d pages to be allocated", page_num);
	process_alloc_multiple(&node->pcb, page_num);

	for (int i = 0; i<page_num; i++){
		uint8_t *data_ptr = data + i * PAGE_SIZE;
		uint32_t phys_page = node->pcb.page_entry[i].physical_page;
		uint8_t *phys_ptr = &physical_memory[phys_page * PAGE_SIZE];
		memcpy(phys_ptr, data_ptr, PAGE_SIZE);	
	} 
	free(data);

	pthread_mutex_lock(&ready_mutex);
	pthread_mutex_unlock(&ready_mutex);
}

/////////
//QUEUE//
/////////

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

//////////
//MEMORY//
//////////

void process_alloc_multiple(struct PCB *pcb, int num){
	for(int i = 0; i<num; i++){
		int page = memory_alloc();
		pcb->page_entry[i].physical_page = page;
		pcb->page_entry[i].free = 1;
	}
	pcb->allocated_page_num += num;
}


void process_free_multiple(struct PCB *pcb){
	for(int i = 0; i<PAGE_NUM; i++){
		pcb->page_entry[i].free = 0;	

	}
	pcb->allocated_page_num = 0;
}

/////////
//DEBUG//
/////////

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


void process_print_pages(struct PCB *pcb){
	printf("\nprocessing page printing request by process with id %d\n", pcb->id);
	for( int i = 0; i<PAGE_NUM; i++){
		if(!pcb->page_entry[i].free)
			printf("page %d allocated");
	}
}
void process_print_hex(uint8_t *data, long size){
	for(int i = 0; i < size; i++){
		printf("%02X ", data[i]);
	}
}
