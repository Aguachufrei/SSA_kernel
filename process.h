#ifndef PROCESS_H
#define PROCESS_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>

struct PCB {
    int id;
    uint32_t ir;
    uint32_t pc;
    uint8_t allocated_page_num;
    bool hasCode; 
    int priority;
    int lastTime;
    uint32_t text;
    uint32_t data;
    uint32_t end;
    struct page_entry *page_entry;
    uint32_t registers[16];
};
struct process_node {
    struct PCB *pcb;
    struct process_node *next;
};
struct process_queue {
    int process_num; 
    struct process_node *first;
    struct process_node *last;
};

extern struct process_queue ready; 
extern struct process_queue terminated; 
extern int current_id;

void process_add_ready();
void process_add(struct process_queue *queue);
void process_loader(struct process_queue *queue, uint8_t *name, int priority);
struct PCB *process_peek(struct process_queue *queue);
struct PCB *process_poll(struct process_queue *queue);
void process_push(struct process_queue *queue, struct PCB *pcb);
struct process_node *process_get_next(struct process_node *nodoa);
struct PCB *process_destroy_next(struct process_queue *queue, struct process_node *nodoa);
void process_alloc_multiple(struct PCB *pcb, int num);
void process_free_multiple(struct PCB *pcb);
void process_read(struct PCB *pcb, uint32_t vaddr, uint8_t *buffer, long size);
void process_write(struct PCB *pcb, uint32_t vaddr, uint8_t *buffer, long size);
void process_execute(struct PCB *pcb);
void process_instructions();
void process_print(struct process_queue *q);
void process_print_pages(struct PCB *pcb);
void process_print_hex(uint8_t *data, long size);


#endif
