#ifndef KERNEL_H
#define KERNEL_H

#include "pcb.h"

void ready_queue_initialize();
void ready_queue_Empty();
void ready_queue_destory();

int get_scheduling_policy_number(char* policy);

int myinit(const char *filename);
int scheduler(int policyNumber);

int findVictim(PCB* pcb);
void resolvePageFault(PCB* pcb);
void updatePageTable(PCB* pcb, int pageNum, int frameNum, int victimFrame);
PCB* getFrameOwner(int frameNum);

/* scheduling helpers */
int pcb_total_lines(PCB *pcb);
void sort_ready_queue_by_job_length();
void age_ready_queue();
int ready_queue_has_work();

#endif