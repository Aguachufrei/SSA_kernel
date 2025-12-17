#ifndef PROZESUAK_H
#define PROZESUAK_H
#include <stdlib.h>
#include <stdio.h>
struct PCB {
    int id;
    int priority;
    int lastTime;
};
struct prozesu_ilara_node {
    struct PCB pcb;
    struct prozesu_ilara_node *next;
};
struct prozesu_ilara {
    int prozesu_kantitatea; 
    struct prozesu_ilara_node *first;
    struct prozesu_ilara_node *last;
};
extern struct prozesu_ilara ready; 
extern int current_id;
void prozesu_add_ready();
void prozesu_add(struct prozesu_ilara *queue);
struct PCB prozesu_peek(struct prozesu_ilara *queue);
struct PCB prozesu_poll(struct prozesu_ilara *queue);
void prozesu_push(struct prozesu_ilara *queue, struct PCB *pcb);
struct prozesu_ilara_node *prozesu_get_next(struct prozesu_ilara_node *nodoa);
struct PCB *prozesu_destroy_next(struct prozesu_ilara *queue, struct prozesu_ilara_node *nodoa);
void printqueue(struct prozesu_ilara *q);

#endif
