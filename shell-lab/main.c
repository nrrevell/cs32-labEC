#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_LINE 101

void parse_and_run_command(const char *command) {
    /* TODO: Implement this.
       Note that this is not the correct way to test for the exit command.
       For example the command "  exit  " should also exit your shell. */
    char* save;
    int MAX_ARGS = 64;
    char *token = strtok_r((char *)command, " ", &save);
    
    if (token == NULL) {
        fprintf(stderr, "invalid command.\n");        
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
            char* redirect_out = NULL;
            char* redirect_in = NULL;
            char* file_out = NULL;
            char* file_in = NULL;
                    
            // Remaining tokens are arguments
            token = strtok_r(NULL, " ", &save);
            while (token && argc < MAX_ARGS - 1) {
                if (strcmp(token, ">") == 0 || strcmp(token, "<") == 0) {
                    //printf("REDIRECTOR FOUND: ");
                    //unless a token is > or <, then it's a redirect
                    //and the next token is a file
                    if(strcmp(token, "<") == 0) {
                        redirect_in = token;
                        file_in = strtok_r(NULL, " ", &save);
                        if (access(file_in, F_OK) == -1) {
                            fprintf(stderr, "invalid command.\n");
                            kill(getppid(), SIGKILL);
                            exit(0);
                        }                    
                    }
                    if(strcmp(token, ">") == 0) {
                        redirect_out = token;
                        file_out = strtok_r(NULL, " ", &save);
                        if (access(file_out, F_OK) == -1) {
                            fprintf(stderr, "invalid command.\n");
                            kill(getppid(), SIGKILL);
                            exit(0);
                        }
                    }
                    token = strtok_r(NULL, " ", &save);
                } else {
                    args[argc] = token;
                    token = strtok_r(NULL, " ", &save);
                    argc++;
                }

            }
            args[argc] = NULL; // null-terminate the array
            if (redirect_out != NULL) {
                int fd = open (file_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("Open failed");
                    exit(1);
                }
                if (dup2(fd, STDOUT_FILENO) < 0) {
                    perror("dup2 failed");
                    exit(1);
                }
                close(fd);
            }
            if (redirect_in != NULL) {
                int fd = open (file_in, O_RDONLY | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("Open failed");
                    exit(1);
                }
                if (dup2(fd, STDIN_FILENO) < 0) {
                    perror("dup2 failed");
                    exit(1);
                }
                close(fd);
            }            

            if (execve(cmd, args, NULL) == -1) {
                    fprintf(stderr, "invalid command.\n");
                    exit(1);
            }
        } else {
            wait(NULL);
            printf("Exit status: 0.\n");
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
    }

    return 0;
}
