#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 101

void parse_and_run_command(const char *command) {
    /* TODO: Implement this.
       Note that this is not the correct way to test for the exit command.
       For example the command "  exit  " should also exit your shell. */
    char* save;
    int MAX_ARGS = 64;
    char *token = strtok_r((char *)command, " ", &save);
    
    if (token == NULL) {
        fprintf(stderr, "\ninvalid command.\n");        
    } else if (strcmp(token, "exit") == 0) {
        //printf("\nExit status: 0\n");
        exit(0);
    } else {
        if (fork() == 0) {
            char *args[MAX_ARGS];
            int argc = 0;

            //command is first token 
            char *cmd = token;
            args[0] = cmd;
            argc = 1;

            // Remaining tokens are arguments
            token = strtok_r(NULL, " ", &save);
            while (token && argc < MAX_ARGS - 1) {
                args[argc] = token;
                token = strtok_r(NULL, " ", &save);
                argc++;
            }
            args[argc] = NULL; // null-terminate the array

            if (execve(cmd, args, NULL) == -1) {
                    fprintf(stderr, "\ninvalid command.\n");
                }
        } else {
            wait(NULL);
            printf("\nExit status: 0.\n");
        }    
    }
}

int main(void) {
    char line[MAX_LINE];

    while (1) {
        printf("> ");
        fflush(stdout);
        if (fgets(line, sizeof(line), stdin) == NULL)
            break;
        line[strcspn(line, "\n")] = '\0';
        parse_and_run_command(line);
        wait(NULL);
    }

    return 0;
}
