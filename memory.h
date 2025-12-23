#ifndef MEMORY_H
#define MEMORY_H

#include <string.h> 
#include <stdio.h>
#include <stdint.h>
#include "process.h"

#define MEMORY_SIZE (1<<24) //2^24, 16MiB 0x000000-0xFFFFFF
#define KERNEL_END_BYTE (1<<22) //2^22, 4MiB, 0x444444
#define PAGE_SIZE 4096 //4kiB
#define PAGE_NUM (MEMORY_SIZE / PAGE_SIZE)
#define KERNEL_END_PAGE (KERNEL_END_BYTE / PAGE_SIZE)

extern uint8_t physical_memory[MEMORY_SIZE]; //Byte
extern uint8_t bitmap[PAGE_NUM]; //Bool 

void meminit();
uint32_t memory_alloc();
void memory_free(uint32_t page);
uint32_t memory_translate(struct PCB *pcb, uint32_t logic_addr);
struct page_entry {
	uint32_t physical_page;
	uint8_t free; //1 free; 2 ocupied;
};

#endif
