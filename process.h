#ifndef PROCESS_H
#define PROCESS_H
#include <stdlib.h>
#include <stdio.h>
struct PCB {
    int id;
    int priority;
    int lastTime;
};
struct process_node {
    struct PCB pcb;
    struct process_node *next;
};
struct process_queue {
    int process_num; 
    struct process_node *first;
    struct process_node *last;
};

extern struct process_queue ready; 
extern int current_id;

void process_add_ready();
void process_add(struct process_queue *queue);
struct PCB process_peek(struct process_queue *queue);
struct PCB process_poll(struct process_queue *queue);
void process_push(struct process_queue *queue, struct PCB *pcb);
struct process_node *process_get_next(struct process_node *nodoa);
struct PCB *process_destroy_next(struct process_queue *queue, struct process_node *nodoa);
void process_print(struct process_queue *q);

#endif
