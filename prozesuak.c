#include <stdlib.h>
#include <stdio.h>
#include "prozesuak.h"
struct prozesu_ilara ready = {.prozesu_kantitatea = 0, .first = NULL, .last = NULL};
struct prozesu_ilara running = {.prozesu_kantitatea = 0, .first = NULL, .last = NULL};
int current_id = 0;

void prozesu_add_ready(){
    prozesu_add(&ready);
}
void prozesu_add(struct prozesu_ilara *queue){
    printf(" process_add called...");
    struct prozesu_ilara_node* nodoa = (struct prozesu_ilara_node*) malloc(sizeof(struct prozesu_ilara_node));
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
}

struct PCB prozesu_peek(struct prozesu_ilara *queue){
    return queue->first->pcb;
}

struct PCB prozesu_poll(struct prozesu_ilara *queue){
    if (queue->prozesu_kantitatea == 0){
        struct PCB errore = { .id = -1 };
        return errore;
    }
    struct PCB cpcb = queue->first->pcb;
    queue->first = queue->first->next;
    queue->prozesu_kantitatea--;
    return cpcb;
}

void prozesu_push(struct prozesu_ilara *queue, struct PCB *pcb){
    struct prozesu_ilara_node* nodoa = (struct prozesu_ilara_node*) malloc(sizeof(struct prozesu_ilara_node));
    nodoa->pcb = *pcb;
    nodoa->next = NULL;
    queue->last->next = nodoa;
    queue->last = nodoa;
    queue->prozesu_kantitatea++;
}
