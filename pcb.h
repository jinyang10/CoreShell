#ifndef PCB_H
#define PCB_H

/*
 * pid: process id / backing-store file ID
 * PC: current frame number
 * pageTable[page] = frame number, or -1 if not loaded
 * pc_page: current page index
 * pc_offset: current line offset inside current frame
 * num_pages: total number of pages in the program
 */
typedef struct {
    char* pid;
    int PC;
    int pageTable[20];
    int pc_page;
    int pc_offset;
    int num_pages;
} PCB;

PCB* makePCB(int numPages, char* pid);
int isFrameOfPCB(PCB* pcb, int frameNum);

#endif