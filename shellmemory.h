#ifndef SHELLMEMORY_H
#define SHELLMEMORY_H

#include <stdio.h>

void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);

void clean_mem(int frameNum);
int load_page_to_mem(FILE* fp, int pageNum, int frameNum, char* fileID);
int findFrame();
int checkFrame(int frameNum, int offset);
char* getFrameLine(int frameNum, int offset);

#endif