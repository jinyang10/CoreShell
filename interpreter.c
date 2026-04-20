#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shellmemory.h"
#include "shell.h"
#include "kernel.h"

int MAX_ARGS_SIZE = 7;

int help();
int quit();
int badcommand();
int badcommandTooManyTokens();
int bad_command_file_does_not_exist();
int badcommand_scheduling_policy_error();
int badcommand_no_mem_space();
int badcommand_ready_queue_full();
int badcommand_same_file_name();
int handleError(int error_code);
int set(char* var, char* value);
int print(char* var);
int run(char* script);
int exec(char *fname1, char *fname2, char *fname3, char* policy);
int my_ls();
int echo(char* var);

int interpreter(char* command_args[], int args_size){
    if (args_size < 1 || args_size > MAX_ARGS_SIZE){
        if (args_size > 0 && strcmp(command_args[0], "set") == 0 && args_size > MAX_ARGS_SIZE) {
            return badcommandTooManyTokens();
        }
        return badcommand();
    }

    for (int i = 0; i < args_size; i++){
        command_args[i][strcspn(command_args[i], "\r\n")] = 0;
    }

    if (strcmp(command_args[0], "help") == 0){
        if (args_size != 1) return badcommand();
        return help();

    } else if (strcmp(command_args[0], "quit") == 0) {
        if (args_size != 1) return badcommand();
        return quit();

    } else if (strcmp(command_args[0], "set") == 0) {
        if (args_size < 3) return badcommand();

        char* value = calloc(1, 150);
        for (int i = 2; i < args_size; i++) {
            strncat(value, command_args[i], 30);
            if (i < args_size - 1) {
                strncat(value, " ", 1);
            }
        }
        int rc = set(command_args[1], value);
        free(value);
        return rc;

    } else if (strcmp(command_args[0], "print") == 0) {
        if (args_size != 2) return badcommand();
        return print(command_args[1]);

    } else if (strcmp(command_args[0], "run") == 0) {
        if (args_size != 2) return badcommand();
        return run(command_args[1]);

    } else if (strcmp(command_args[0], "exec") == 0) {
        if (args_size <= 2 || args_size > 5) return badcommand();

        if (args_size == 3) {
            return exec(command_args[1], NULL, NULL, command_args[2]);
        } else if (args_size == 4) {
            return exec(command_args[1], command_args[2], NULL, command_args[3]);
        } else {
            return exec(command_args[1], command_args[2], command_args[3], command_args[4]);
        }

    } else if (strcmp(command_args[0], "my_ls") == 0) {
        if (args_size != 1) return badcommand();
        return my_ls();

    } else if (strcmp(command_args[0], "echo") == 0) {
        if (args_size != 2) return badcommand();
        return echo(command_args[1]);
    }

    return badcommand();
}

int help(){
    char help_string[] =
        "COMMAND                 DESCRIPTION\n"
        "help                    Displays all the commands\n"
        "quit                    Exits / terminates the shell with \"Bye!\"\n"
        "set VAR STRING          Assigns a value to shell memory\n"
        "print VAR               Displays the STRING assigned to VAR\n"
        "run SCRIPT.TXT          Executes the file SCRIPT.TXT\n";

    printf("%s\n", help_string);
    return 0;
}

int quit(){
    printf("%s\n", "Bye!");
    system("rm -rf BackingStore");
    exit(0);
}

int badcommand(){
    printf("%s\n", "Unknown Command");
    return 1;
}

int badcommandTooManyTokens(){
    printf("%s\n", "Bad command: Too many tokens");
    return 1;
}

int bad_command_file_does_not_exist(){
    printf("%s\n", "Bad command: File not found");
    return 1;
}

int badcommand_scheduling_policy_error(){
    printf("%s\n", "Bad command: scheduling policy incorrect");
    return 1;
}

int badcommand_no_mem_space(){
    printf("%s\n", "Bad command: no space left in shell memory");
    return 1;
}

int badcommand_ready_queue_full(){
    printf("%s\n", "Bad command: ready queue is full");
    return 1;
}

int badcommand_same_file_name(){
    printf("%s\n", "Bad command: same file name");
    return 1;
}

int handleError(int error_code){
    if (error_code == 11) {
        return bad_command_file_does_not_exist();
    } else if (error_code == 21) {
        return badcommand_no_mem_space();
    } else if (error_code == 14) {
        return badcommand_ready_queue_full();
    } else if (error_code == 15) {
        return badcommand_scheduling_policy_error();
    }
    return 0;
}

int set(char* var, char* value){
    mem_set_value(var, value);
    return 0;
}

int print(char* var){
    printf("%s\n", mem_get_value(var));
    return 0;
}

int run(char* script){
    int errCode = myinit(script);
    if (errCode == 11) {
        return handleError(errCode);
    }

    scheduler(0);
    return errCode;
}

int exec(char *fname1, char *fname2, char *fname3, char* policy){
    system("rm -rf BackingStore");
    system("mkdir BackingStore");

    int error_code = 0;
    int policyNumber = get_scheduling_policy_number(policy);

    if (policyNumber == 15) {
        return handleError(policyNumber);
    }

    if (fname1 != NULL) {
        error_code = myinit(fname1);
        if (error_code != 0) return handleError(error_code);
    }

    if (fname2 != NULL) {
        error_code = myinit(fname2);
        if (error_code != 0) return handleError(error_code);
    }

    if (fname3 != NULL) {
        error_code = myinit(fname3);
        if (error_code != 0) return handleError(error_code);
    }

    scheduler(policyNumber);
    return error_code;
}

int my_ls(){
    return system("ls | sort");
}

int echo(char* var){
    if (var[0] == '$') {
        var++;
        printf("%s\n", mem_get_value(var));
    } else {
        printf("%s\n", var);
    }
    return 0;
}