#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "memory.h"
#include "process.h"

uint8_t bitmap[PAGE_NUM];
uint8_t physical_memory[MEMORY_SIZE];

void meminit(){
	memset(bitmap, 1, KERNEL_END_PAGE);
	memset(bitmap + KERNEL_END_PAGE, 0, PAGE_NUM-KERNEL_END_PAGE);
}


uint32_t memory_alloc(){
	for(uint32_t i = KERNEL_END_PAGE; i<PAGE_NUM; i++){
		if(!bitmap[i]){
			bitmap[i]=1;
			return i;
		}
	}
}


void memory_free(uint32_t i){
	if(bitmap[i]){
		bitmap[i]=0;
		return;
	}
	printf("WARNING THIS MEMORY FRAME WAS NEVER ALLOCATED TO BEGIN WITH");
}


uint32_t memory_translate(struct PCB *pcb, uint32_t logic_addr){
	
	//return pcb->page_entry[logic_addr].physical_page;
	uint32_t logic_page = logic_addr / PAGE_SIZE;
	if(logic_page > PAGE_NUM && pcb->page_entry[logic_page].free){
		printf("JA!, SEGMENTATION FAULT!");
		return 1<<31;
	}

	uint32_t relative_addr = logic_addr % PAGE_SIZE;
	uint32_t physical_page = pcb->page_entry[logic_page].physical_page;
	uint32_t physical_addr = physical_page * PAGE_SIZE + relative_addr;
	return physical_addr;
}

