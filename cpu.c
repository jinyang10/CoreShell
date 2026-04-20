#include "cpu.h"
#include "pcb.h"
#include "interpreter.h"
#include "shell.h"
#include "shellmemory.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * IP: current frame number
 * offset: current line offset within the frame (0,1,2)
 * IR: current instruction
 * quanta: max number of lines to run this turn
 */
struct CPU {
    int IP;
    int offset;
    char IR[1000];
    int quanta;
};

static struct CPU aCPU = { .IP = -1, .offset = 0, .quanta = 0 };

int cpu_get_ip() {
    return aCPU.IP;
}

int cpu_get_offset() {
    return aCPU.offset;
}

void cpu_set_ip(int pIP) {
    aCPU.IP = pIP;
}

void cpu_set_offset(int offset) {
    aCPU.offset = offset;
}

void cpu_empty() {
    aCPU.IP = -1;
    aCPU.offset = 0;
    aCPU.quanta = 0;
    aCPU.IR[0] = '\0';
}

void load_PCB_TO_CPU(int PC) {
    cpu_set_ip(PC);
}

/*
 * Runs up to quanta lines starting from (IP, offset).
 * Returns:
 *   0 = time slice finished normally
 *   2 = reached end of current page/frame
 */
int cpu_run(int quanta) {
    aCPU.quanta = quanta;

    for (int i = 0; i < quanta; i++) {
        int frame = aCPU.IP;
        int line = aCPU.offset;

        if (checkFrame(frame, line) == 0) {
            return 2;
        }

        strcpy(aCPU.IR, getFrameLine(frame, line));
        parseInput(aCPU.IR);

        aCPU.offset++;

        if (aCPU.offset == 3) {
            return 2;
        }
    }

    return 0;
}