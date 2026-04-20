#include "pcb.h"
#include "kernel.h"
#include "cpu.h"
#include "shell.h"
#include "shellmemory.h"
#include "interpreter.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define QUEUE_LENGTH 10
#define MAX_INT 2147483646

PCB* readyQueue[QUEUE_LENGTH];

extern size_t sizeVarStore;
extern size_t sizeFrameStore;

static PCB* ready_queue_peek(int index) {
    if (index < 0 || index >= QUEUE_LENGTH) return NULL;
    if (readyQueue[index] == NULL) return NULL;
    if (readyQueue[index]->PC == -1) return NULL;
    return readyQueue[index];
}

int ready_queue_has_work() {
    return ready_queue_peek(0) != NULL;
}

void ready_queue_initialize() {
    for (size_t i = 0; i < QUEUE_LENGTH; ++i) {
        readyQueue[i] = (PCB*)malloc(sizeof(PCB));
        readyQueue[i]->PC = -1;
        readyQueue[i]->pc_page = -1;
        readyQueue[i]->pc_offset = -1;
        readyQueue[i]->pid = NULL;
        readyQueue[i]->num_pages = -1;
        for (int j = 0; j < 20; j++) readyQueue[i]->pageTable[j] = -1;
    }
}

void ready_queue_Empty() {
    for (size_t i = 0; i < QUEUE_LENGTH; ++i) {
        readyQueue[i]->PC = -1;
        readyQueue[i]->pc_page = -1;
        readyQueue[i]->pc_offset = -1;
        readyQueue[i]->pid = NULL;
        readyQueue[i]->num_pages = -1;
        for (int j = 0; j < 20; j++) readyQueue[i]->pageTable[j] = -1;
    }
}

void ready_queue_destory() {
    for (size_t i = 0; i < QUEUE_LENGTH; ++i) {
        free(readyQueue[i]);
    }
}

PCB ready_queue_pop(int index, bool inPlace) {
    PCB head = *(readyQueue[index]);

    if (inPlace) {
        for (size_t i = index + 1; i < QUEUE_LENGTH; i++) {
            readyQueue[i - 1]->PC = readyQueue[i]->PC;
            readyQueue[i - 1]->pc_page = readyQueue[i]->pc_page;
            readyQueue[i - 1]->pc_offset = readyQueue[i]->pc_offset;
            readyQueue[i - 1]->pid = readyQueue[i]->pid;
            readyQueue[i - 1]->num_pages = readyQueue[i]->num_pages;
            for (int j = 0; j < 20; j++) {
                readyQueue[i - 1]->pageTable[j] = readyQueue[i]->pageTable[j];
            }
        }

        readyQueue[QUEUE_LENGTH - 1]->PC = -1;
        readyQueue[QUEUE_LENGTH - 1]->pc_page = -1;
        readyQueue[QUEUE_LENGTH - 1]->pc_offset = -1;
        readyQueue[QUEUE_LENGTH - 1]->pid = NULL;
        readyQueue[QUEUE_LENGTH - 1]->num_pages = -1;
        for (int j = 0; j < 20; j++) {
            readyQueue[QUEUE_LENGTH - 1]->pageTable[j] = -1;
        }
    }

    return head;
}

void ready_queue_add_to_end(PCB *pPCB) {
    for (int i = 0; i < QUEUE_LENGTH; i++) {
        if (readyQueue[i]->PC == -1) {
            readyQueue[i]->PC = pPCB->PC;
            readyQueue[i]->pc_page = pPCB->pc_page;
            readyQueue[i]->pc_offset = pPCB->pc_offset;
            readyQueue[i]->pid = pPCB->pid;
            readyQueue[i]->num_pages = pPCB->num_pages;
            for (int j = 0; j < 20; j++) {
                readyQueue[i]->pageTable[j] = pPCB->pageTable[j];
            }
            break;
        }
    }
}

void ready_queue_add_to_front(PCB *pPCB) {
    for (size_t i = QUEUE_LENGTH - 1; i > 0; i--) {
        readyQueue[i]->PC = readyQueue[i - 1]->PC;
        readyQueue[i]->pc_page = readyQueue[i - 1]->pc_page;
        readyQueue[i]->pc_offset = readyQueue[i - 1]->pc_offset;
        readyQueue[i]->pid = readyQueue[i - 1]->pid;
        readyQueue[i]->num_pages = readyQueue[i - 1]->num_pages;
        for (int j = 0; j < 20; j++) {
            readyQueue[i]->pageTable[j] = readyQueue[i - 1]->pageTable[j];
        }
    }

    readyQueue[0]->PC = pPCB->PC;
    readyQueue[0]->pc_offset = pPCB->pc_offset;
    readyQueue[0]->pc_page = pPCB->pc_page;
    readyQueue[0]->pid = pPCB->pid;
    readyQueue[0]->num_pages = pPCB->num_pages;
    for (int j = 0; j < 20; j++) {
        readyQueue[0]->pageTable[j] = pPCB->pageTable[j];
    }
}

int pcb_total_lines(PCB *pcb) {
    if (pcb == NULL || pcb->PC == -1) return MAX_INT;

    int total = 0;
    char path[128] = "BackingStore/";
    strcat(path, pcb->pid);

    FILE *fp = fopen(path, "rt");
    if (fp == NULL) return MAX_INT;

    char buffer[1000];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        total++;
    }
    fclose(fp);

    return total;
}

void sort_ready_queue_by_job_length() {
    for (int i = 0; i < QUEUE_LENGTH - 1; i++) {
        if (readyQueue[i]->PC == -1) break;

        for (int j = i + 1; j < QUEUE_LENGTH; j++) {
            if (readyQueue[j]->PC == -1) continue;

            if (pcb_total_lines(readyQueue[j]) < pcb_total_lines(readyQueue[i])) {
                PCB temp = *readyQueue[i];
                *readyQueue[i] = *readyQueue[j];
                *readyQueue[j] = temp;
            }
        }
    }
}

/*
 * Simple AGING:
 * - measure each job by total script length
 * - move the shortest job toward the front before each time slice
 * This is a practical approximation using your current PCB structure.
 */
void age_ready_queue() {
    sort_ready_queue_by_job_length();
}

int countPages(FILE* fp) {
    int numLines = 0;
    char buffer[1000];

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        numLines++;
    }
    rewind(fp);

    int numPages = numLines / 3;
    if (numLines % 3 != 0) {
        numPages++;
    }
    return numPages;
}

void freeFramesPCB(PCB* pcb) {
    for (int i = 0; i < pcb->num_pages; i++) {
        if (pcb->pageTable[i] != -1) {
            clean_mem(pcb->pageTable[i]);
            pcb->pageTable[i] = -1;
        }
    }
}

PCB* getFrameOwner(int frameNum) {
    for (int i = 0; i < QUEUE_LENGTH; i++) {
        if (readyQueue[i]->PC != -1 && isFrameOfPCB(readyQueue[i], frameNum) == 1) {
            return readyQueue[i];
        }
    }
    return NULL;
}

int findVictim(PCB* pcb) {
    int frameNum = rand() % sizeFrameStore;

    while (true) {
        while (isFrameOfPCB(pcb, frameNum) == 1) {
            frameNum = (frameNum + 1) % sizeFrameStore;
        }

        PCB* victim = getFrameOwner(frameNum);
        if (victim == NULL) return frameNum;

        if (victim->pageTable[victim->pc_page] != frameNum) {
            return frameNum;
        }

        frameNum = (frameNum + 1) % sizeFrameStore;
    }
}

void updatePageTable(PCB* pcb, int pageNum, int frameNum, int victimFrame) {
    if (victimFrame != -1) {
        PCB* victim = getFrameOwner(victimFrame);
        if (victim != NULL) {
            for (int i = 0; i < victim->num_pages; i++) {
                if (victim->pageTable[i] == victimFrame) {
                    victim->pageTable[i] = -1;
                }
            }
        }
    }
    pcb->pageTable[pageNum] = frameNum;
}

void resolvePageFault(PCB* pcb) {
    pcb->pc_page++;

    if (pcb->pc_page >= pcb->num_pages) {
        return;
    }

    if (pcb->pageTable[pcb->pc_page] == -1) {
        char file[100] = "BackingStore/";
        strcat(file, pcb->pid);

        FILE* fp = fopen(file, "rt");
        if (fp == NULL) return;

        int frame = findFrame();
        int victimFrame = -1;

        if (frame == -1) {
            frame = findVictim(pcb);
            victimFrame = frame;
        }

        clean_mem(frame);
        load_page_to_mem(fp, pcb->pc_page, frame, pcb->pid);
        updatePageTable(pcb, pcb->pc_page, frame, victimFrame);
        fclose(fp);
    }

    pcb->PC = pcb->pageTable[pcb->pc_page];
    pcb->pc_offset = 0;
}

int myinit(const char *filename) {
    FILE* fp;
    int error_code = 0;

    char *copyf1 = strdup(filename);

    char* fileID = malloc(32);
    sprintf(fileID, "%d", rand());

    char cp[100] = "cp -n ";
    char back[60] = " BackingStore/";
    strcat(back, fileID);
    strcat(cp, copyf1);
    strcat(cp, back);
    system(cp);

    char backfile[100] = "./BackingStore/";
    strcat(backfile, fileID);

    fp = fopen(backfile, "rt");
    if (fp == NULL) {
        free(copyf1);
        return 11;
    }

    int numPages = countPages(fp);
    PCB* newPCB = makePCB(numPages, fileID);
    if (newPCB == NULL) {
        fclose(fp);
        free(copyf1);
        return 21;
    }

    int frame = findFrame();
    int victimFrame = -1;

    if (frame == -1) {
        victimFrame = findVictim(newPCB);
        frame = victimFrame;
    }

    clean_mem(frame);
    error_code = load_page_to_mem(fp, 0, frame, fileID);
    if (error_code != 0) {
        fclose(fp);
        free(copyf1);
        return error_code;
    }

    updatePageTable(newPCB, 0, frame, victimFrame);
    newPCB->PC = frame;

    if (newPCB->num_pages > 1) {
        frame = findFrame();
        victimFrame = -1;

        if (frame == -1) {
            victimFrame = findVictim(newPCB);
            frame = victimFrame;
        }

        clean_mem(frame);
        load_page_to_mem(fp, 1, frame, fileID);
        updatePageTable(newPCB, 1, frame, victimFrame);
    }

    ready_queue_add_to_end(newPCB);
    fclose(fp);
    free(copyf1);

    return error_code;
}

int get_scheduling_policy_number(char* policy) {
    if (strcmp("FCFS", policy) == 0) return 0;
    if (strcmp("SJF", policy) == 0) return 1;
    if (strcmp("RR", policy) == 0) return 2;
    if (strcmp("AGING", policy) == 0) return 3;
    return 15;
}

int scheduler(int policyNumber) {
    int error_code = 0;
    int cpu_quanta_per_program = 2;

    cpu_set_ip(-1);

    if (policyNumber == 0) {
        cpu_quanta_per_program = MAX_INT;
    } else if (policyNumber == 1) {
        cpu_quanta_per_program = MAX_INT;
        sort_ready_queue_by_job_length();
    } else if (policyNumber == 2) {
        cpu_quanta_per_program = 2;
    } else if (policyNumber == 3) {
        cpu_quanta_per_program = 1;
        age_ready_queue();
    }

    while (ready_queue_has_work()) {
        if (policyNumber == 1) {
            sort_ready_queue_by_job_length();
        } else if (policyNumber == 3) {
            age_ready_queue();
        }

        PCB *firstPCB = ready_queue_peek(0);

        load_PCB_TO_CPU(firstPCB->PC);
        cpu_set_offset(firstPCB->pc_offset);

        int rc = cpu_run(cpu_quanta_per_program);
        int newOffset = cpu_get_offset();

        if (rc == 2) {
            if (firstPCB->pc_page == firstPCB->num_pages - 1 && newOffset >= 3) {
                freeFramesPCB(firstPCB);
                ready_queue_pop(0, true);
            } else {
                resolvePageFault(firstPCB);

                PCB temp = ready_queue_pop(0, true);
                ready_queue_add_to_end(&temp);
            }
        } else if (rc < 0) {
            ready_queue_pop(0, true);
        } else {
            firstPCB->pc_offset = newOffset;

            if (policyNumber == 0 || policyNumber == 1) {
                PCB temp = ready_queue_pop(0, true);
                ready_queue_add_to_end(&temp);
            } else {
                PCB temp = ready_queue_pop(0, true);
                ready_queue_add_to_end(&temp);
            }
        }
    }

    ready_queue_Empty();
    cpu_empty();

    return error_code;
}