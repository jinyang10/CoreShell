#include "pcb.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * In this implementation, pid is the same as the backing-store file ID.
 */
PCB* makePCB(int numPages, char* pid) {
    PCB *newPCB = malloc(sizeof(PCB));
    if (newPCB == NULL) return NULL;

    newPCB->pid = pid;
    newPCB->num_pages = numPages;
    newPCB->PC = -1;
    newPCB->pc_page = 0;
    newPCB->pc_offset = 0;

    for (int i = 0; i < 20; i++) {
        newPCB->pageTable[i] = -1;
    }

    return newPCB;
}

int isFrameOfPCB(PCB* pcb, int frameNum) {
    for (int i = 0; i < pcb->num_pages; i++) {
        if (pcb->pageTable[i] == frameNum) {
            return 1;
        }
    }
    return 0;
}