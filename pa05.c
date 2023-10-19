/**
 * @file pa05.c
 * @author Luke Foster - fost4403@vandals.uidaho.edu
 * @brief This is a simple program that has the user enter in a file name as an argument. It will then open the file and use a pipe to have
 *        so that the child process can print the content of the files using a string format.
 * @version 0.1
 * @date 2023-03-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX 256

int main(int argc, char *argv[])
{
    if(argc != 2) {
        fprintf(stderr, "Error accepting arguments!\n");
        return 1;
    }

    pid_t pid, status;

    int pipefd[2];

    char buf[MAX];

    if(pipe(pipefd) == -1) {
        fprintf(stderr, "Error creating pipe!\n");
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");

    if(fp == NULL) {
        fprintf(stderr, "Error opening file!\n");
        return 1;
    } 

    pid = fork();

    if(pid == 0) { /* Child */
        close(pipefd[1]); /* Close write */

        while(read(pipefd[0], buf, MAX) > 0) {
            fprintf(stdout, "%s", buf); /* Print file contents as string */
            // Can use write(1, &buf, MAX) instead if raw content is to be printed to screen 
        }

        putchar('\n');

        close(pipefd[0]);
        fclose(fp);
        
        exit(0);
    }
    else { /* Parent */
        close(pipefd[0]); /* Close read */

        while(fgets(buf, MAX, fp) != NULL || !feof(fp)) {

            if(write(pipefd[1], buf, MAX) == -1) {
                fprintf(stderr, "Error writing to pipe!\n");

                close(pipefd[1]);
                fclose(fp);
                return 1;
            }
        }

        close(pipefd[1]);
        fclose(fp);
    }

    wait(&status);

    return 0;
}