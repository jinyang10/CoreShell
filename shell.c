#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <direct.h>

#include "interpreter.h"
#include "shellmemory.h"
#include "kernel.h"

extern int frameSize;
extern int varMemSize;

int MAX_USER_INPUT = 1000;
int parseInput(char ui[]);

int main(int argc, char *argv[]) {
    printf("%s\n", "Shell version 1.1 Created January 2022");
    help();

    char prompt = '$';
    char userInput[MAX_USER_INPUT];
    int errorCode = 0;

    printf("Frame Store Size = %d; Variable Store Size = %d\n", frameSize, varMemSize);

    errno = 0;
	int ret = mkdir("BackingStore");
	if (ret == -1 && errno == EEXIST) {
    	system("rm -rf BackingStore");
    	mkdir("BackingStore");
	}

    for (int i = 0; i < MAX_USER_INPUT; i++) {
        userInput[i] = '\0';
    }

    mem_init();
    ready_queue_initialize();
    srand(time(NULL));

    while (1) {
        printf("%c ", prompt);
        fgets(userInput, MAX_USER_INPUT - 1, stdin);

        if (feof(stdin)) {
            freopen("/dev/tty", "r", stdin);
        }

        errorCode = parseInput(userInput);
        if (errorCode == -1) exit(99);
        memset(userInput, 0, sizeof(userInput));
    }

    return 0;
}

int parseInput(char ui[]) {
    char tmp[200];
    char *words[100];
    int a = 0;
    int b;
    int w = 0;
    int errorCode;

    for (a = 0; ui[a] == ' ' && a < 1000; a++);

    while (ui[a] != '\n' && ui[a] != '\0' && a < 1000) {
        for (b = 0; ui[a] != ';' && ui[a] != '\0' && ui[a] != '\n' && ui[a] != ' ' && a < 1000; a++, b++) {
            tmp[b] = ui[a];
        }
        tmp[b] = '\0';

        words[w] = strdup(tmp);

        if (ui[a] == ';') {
            w++;
            errorCode = interpreter(words, w);
            if (errorCode == -1) {
                return errorCode;
            }

            a++;
            w = 0;
            for (; ui[a] == ' ' && a < 1000; a++);
            continue;
        }

        w++;
        if (ui[a] == '\0') {
            break;
        }
        a++;
    }

    errorCode = interpreter(words, w);
    return errorCode;
}