#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define SHELL_MEM_LENGTH 1000
#define FS 500
#define VS 500

int frameSize = FS;
int varMemSize = VS;

struct memory_struct {
    char *var;
    char *value;
};

struct frame_struct {
    struct memory_struct frame[3];
};

struct frame_struct frameStore[500];
struct memory_struct varStore[500];

size_t sizeVarStore = 500;
size_t sizeFrameStore = 500;

// check frame slot at frameNum, offset; 1 if occupied, 0 if not
int checkFrame(int frameNum, int offset) {
    if (strcmp(frameStore[frameNum].frame[offset].var, "none") == 0) {
        return 0;
    }
    return 1;
}

char* getFrameLine(int frameNum, int offset) {
    return strdup(frameStore[frameNum].frame[offset].value);
}

void resetmem() {
    for (int i = 0; i < sizeVarStore; i++) {
        varStore[i].var = "none";
        varStore[i].value = "none";
    }
}

void mem_init() {
    for (int i = 0; i < sizeFrameStore; i++) {
        for (int j = 0; j < 3; j++) {
            frameStore[i].frame[j].var = "none";
            frameStore[i].frame[j].value = "none";
        }
    }
    resetmem();
}

void mem_set_value(char *var_in, char *value_in) {
    for (int i = 0; i < sizeVarStore; i++) {
        if (strcmp(varStore[i].var, var_in) == 0) {
            varStore[i].value = strdup(value_in);
            return;
        }
    }

    for (int i = 0; i < sizeVarStore; i++) {
        if (strcmp(varStore[i].var, "none") == 0) {
            varStore[i].var = strdup(var_in);
            varStore[i].value = strdup(value_in);
            return;
        }
    }
}

char *mem_get_value(char *var_in) {
    for (int i = 0; i < sizeVarStore; i++) {
        if (strcmp(varStore[i].var, var_in) == 0) {
            return strdup(varStore[i].value);
        }
    }
    return "Variable does not exist";
}

void clean_mem(int frameNum) {
    for (int i = 0; i < 3; i++) {
        frameStore[frameNum].frame[i].var = "none";
        frameStore[frameNum].frame[i].value = "none";
    }
}

int load_page_to_mem(FILE* fp, int pageNum, int frameNum, char* fileID) {
    rewind(fp);

    char line[1000];
    char buffer[1000];
    int i = 0;

    // skip previous pages
    while (i < pageNum * 3 && fgets(buffer, sizeof(buffer), fp) != NULL) {
        i++;
    }

    // load up to 3 lines into the SAME frame
    i = 0;
    while (i < 3 && fgets(line, sizeof(line), fp) != NULL) {
        frameStore[frameNum].frame[i].value = strdup(line);
        frameStore[frameNum].frame[i].var = strdup(fileID);
        i++;
    }

    while (i < 3) {
        frameStore[frameNum].frame[i].value = "none";
        frameStore[frameNum].frame[i].var = "none";
        i++;
    }

    return 0;
}

int findFrame() {
    for (int i = 0; i < sizeFrameStore; i++) {
        int freeFrame = 1;
        for (int j = 0; j < 3; j++) {
            if (strcmp(frameStore[i].frame[j].var, "none") != 0) {
                freeFrame = 0;
                break;
            }
        }
        if (freeFrame) {
            return i;
        }
    }
    return -1;
}