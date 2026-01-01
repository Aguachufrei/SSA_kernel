#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include "process.h"
#include "memory.h" 
#include "cpu.h"
#include "clock.h"
struct process_queue ready = {.process_num = 0, .first = NULL, .last = NULL};
struct process_queue terminated = {.process_num = 0, .first = NULL, .last = NULL};
int current_id = 0;
pthread_mutex_t ready_mutex = PTHREAD_MUTEX_INITIALIZER;

//////////
//LOADER//
//////////

void process_add_ready(){
	process_add(&ready);
}


void process_add(struct process_queue *queue){
	pthread_mutex_lock(&ready_mutex);
	struct process_node *node = malloc(sizeof(struct process_node));
	node->pcb = malloc(sizeof(struct PCB));
	node->pcb->id = current_id;
	node->pcb->priority = rand()%100;
	node->pcb->lastTime = se_time;
	node->pcb->hasCode = false;
	node->pcb->page_entry = malloc(PAGE_NUM*sizeof(struct page_entry));
	for(int i = 0; i < PAGE_NUM; i++){
		node->pcb->page_entry[i].free=0;
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
	//open file
	FILE *f = fopen(name, "r");
	if(!f){
		printf("\nFile %s not found\n", name);
		return;
	}

	//Create process
	struct process_node *node = malloc(sizeof(struct process_node));
	node->pcb = malloc(sizeof(struct PCB));
	node->pcb->id = current_id;
	node->pcb->hasCode = true;
	if(priority>=100)printf("\nWARNING priority exceeding maximun value.\nValue range [0, 100); Recieved value: %d\n", priority);
	node->pcb->priority = priority%100;
	node->pcb->lastTime = se_time;

	//first two lines became "pointers"
	uint8_t buffer[32];
	if(fgets(buffer, 32, f))node->pcb->text = (uint32_t)strtoul(buffer + 6, NULL, 16);
	if(fgets(buffer, 32, f))node->pcb->data = (uint32_t)strtoul(buffer + 6, NULL, 16);
	node->pcb->pc = node->pcb->text;

	//get rest of file length
	long pos = ftell(f);
	fseek(f, 0, SEEK_END);
	long end = ftell(f);
	fseek(f, pos, SEEK_SET);
	long size = end - pos;
	node->pcb->end = end;

	//get the rest of the file
	uint8_t *text = malloc(size + 1);
	fread(text, 1, size, f);	
	text[size] = '\0';

	//convert from ascii to bin
	uint8_t *data = malloc(size / 2);
	long realSize = 0;
	uint8_t temp = 0;
	for(int i = 0; i < size; i++){
		if(text[i] == '\n') continue;
		if(!temp){
			temp = text[i];
			continue;
		}
		uint8_t asciiCode[3] = { temp, text[i], '\0'};
		data[realSize] = strtoul(asciiCode, NULL, 16);
		temp = 0;
		realSize++;
	}
	size = realSize;
	process_print_hex(data, size);

	//create the virtual memory
	node->pcb->page_entry = malloc(PAGE_NUM*sizeof(struct page_entry));
	for(int i = 0; i < PAGE_NUM; i++){
		node->pcb->page_entry[i].free=0;
	}
	int page_num = (size + PAGE_SIZE - 1) / PAGE_SIZE;

	//alloc and write into memory
	process_alloc_multiple(node->pcb, page_num);
	uint32_t bytes_left = size;
	for (int i = 0; i<page_num; i++){
		uint8_t *data_ptr = data + i * PAGE_SIZE;
		uint32_t phys_page = node->pcb->page_entry[i].physical_page;
		uint8_t *phys_ptr = &physical_memory[phys_page * PAGE_SIZE];
		uint32_t bytes_to_copy = (bytes_left<PAGE_SIZE)?bytes_left:PAGE_SIZE;	
		memcpy(phys_ptr, data_ptr, bytes_to_copy);	
		bytes_left-=bytes_to_copy;
	} 

	//push the process into the queue
	pthread_mutex_lock(&ready_mutex);
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
	free(text);
	free(data);
}

/////////
//QUEUE//
/////////

struct PCB *process_peek(struct process_queue *queue){
	return queue->first->pcb;
}


struct PCB *process_poll(struct process_queue *queue){
	pthread_mutex_lock(&ready_mutex);

	if (queue->process_num == 0){
		printf("ERROR, queue is empty");
	}

	struct process_node *node = queue->first;
	struct PCB *cpcb = node->pcb;

	queue->first = node->next;
	queue->process_num--;
	if(queue->first==NULL) queue->last = NULL;

	pthread_mutex_unlock(&ready_mutex);

	free(node);
	return cpcb;
}


void process_push(struct process_queue *queue, struct PCB *pcb){
	pthread_mutex_lock(&ready_mutex);
	struct process_node *node = (struct process_node*) malloc(sizeof(struct process_node));
	node->pcb = pcb;
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
	if(node->next == NULL){
		printf("trying to destroy a NULL process\n");
		pthread_mutex_unlock(&ready_mutex);
		return NULL;
	}
	struct process_node *target = node->next;
	struct PCB *current = target->pcb;
	
	node->next = target->next;
	if(node->next==NULL)queue->last = node;
	queue->process_num--;
	pthread_mutex_unlock(&ready_mutex);
	free(target);
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
void process_read(struct PCB *pcb, uint32_t vaddr, uint8_t *buffer, long size){
	long bytes_processed = 0;
	uint32_t initial_offset = vaddr%PAGE_SIZE;
	while(bytes_processed < size){
		uint32_t vpage = (vaddr+bytes_processed)/PAGE_SIZE;
		long bytes_to_process=(size-bytes_processed<PAGE_SIZE?size-bytes_processed:PAGE_SIZE-initial_offset);
		uint32_t ppage = pcb->page_entry[vpage].physical_page;
		memcpy(buffer+bytes_processed, &physical_memory[ppage*PAGE_SIZE]+initial_offset, bytes_to_process);
		initial_offset = 0;
		bytes_processed += bytes_to_process;
	}
}
void process_write(struct PCB *pcb, uint32_t vaddr, uint8_t *buffer, long size){
	long bytes_processed = 0;
	uint32_t initial_offset = vaddr%PAGE_SIZE;
	while(bytes_processed < size){
		uint32_t vpage = (vaddr+bytes_processed)/PAGE_SIZE;
		long bytes_to_process=(size-bytes_processed<PAGE_SIZE?size-bytes_processed:PAGE_SIZE-initial_offset);
		uint32_t ppage = pcb->page_entry[vpage].physical_page;
		memcpy(&physical_memory[ppage*PAGE_SIZE]+initial_offset, buffer+bytes_processed, bytes_to_process);
		initial_offset = 0;
		bytes_processed += bytes_to_process;
	}
}

/////////////
//Execution//
/////////////

void process_execute(struct PCB *pcb){
	uint8_t buffer[4];
	process_read(pcb, pcb->pc, buffer, 4);
	pcb->ir = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
	pcb->pc+=4;
	printf("Data to execute for pid %d\n", pcb->id);
	printf("%08X\n", pcb->ir);
	uint8_t instruction = (pcb->ir >> 28) & 0x0F; 
	printf("INSTRUCTION: %d\n", instruction);

	uint8_t rx, ry, rz;
	uint32_t vaddr;

	switch(instruction){
		case 0:
			//ld rx, vaddr
			rx = pcb->ir>>24 & 0x0F;
			vaddr = pcb->ir & 0x00FFFFFF; 
			process_read(pcb, vaddr, buffer, 4);
			pcb->registers[rx] = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
			printf("ld r%d, %6X\n", rx, vaddr);
			break;
		case 1:
			//st rx vaddr
			rx = pcb->ir>>24 & 0x0F;
			vaddr = pcb->ir & 0x00FFFFFF; 
			buffer[0] = (uint8_t)(pcb->registers[rx]>>24);
			buffer[1] = (uint8_t)(pcb->registers[rx]>>16); 
			buffer[2] = (uint8_t)(pcb->registers[rx]>>8);
			buffer[3] = (uint8_t)(pcb->registers[rx]); 
			process_write(pcb, vaddr, buffer, 4);
			printf("st r%d, %6X\n", rx, vaddr);

			break;
		case 2:
			//add rx, ry, rz
			rx = pcb->ir>>24 & 0x0F;
			ry = pcb->ir>>20 & 0x0F;
			rz = pcb->ir>>16 & 0x0F;
			printf("add r%d, r%d, r%d\n", rx, ry ,rz);
			pcb->registers[rx] = pcb->registers[ry] + pcb->registers[rz]; 

			break;
		case 15:
			//exit
			printf("exit\n");
			uint8_t *buff = malloc(pcb->end-pcb->data);
			process_read(pcb, pcb->data, buff, pcb->end-pcb->data);
			process_print_hex(buff, (pcb->end-pcb->data)/4);
			pcb->hasCode=0;


			break;
		default:
			printf("instruction is unknown\n");
			break;
	}
}


void process_instructions(){
	for(int i = 0; i < cpu.coreNum; i++){
		for(int j = 0; j < cpu.hthreadNum; j++){
			if(cpu.cores[i][j]==NULL||cpu.cores[i][j]->hasCode==false)continue;
			process_execute(cpu.cores[i][j]);	
		}
	}
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
		printf("[%d] node=%p  id=%d  priority=%d  lastTime=%d code=%d next=%p\n",
				i,
				current,
				current->pcb->id,
				current->pcb->priority,
				current->pcb->lastTime,
				current->pcb->hasCode,
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
	printf("\n");
}


