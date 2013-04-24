/*
 * =========================================================================
 *
 *       Filename:  shpipi2.c
 *
 *    Description:  Shell pipe simulation
 *
 *        Version:  1.0
 *        Created:  04/24/2013 02:31:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int fd[2];
    int i;
    pid_t child1, child2;

    if(argc < 3){
        fprintf(stderr, "Usage %s command1 command2\n", argv[0]);
        exit(1);
    }

    if (pipe(fd) == -1){
        perror("pipe");
        exit(2);
    }

    switch(child1 = fork()){
    case -1:
        perror("fork");
        exit(1);

    case 0:  // child1 code
        // switch stdout
        dup2(fd[1],fileno(stdout));
        close(fd[0]);
        close(fd[1]);
        // execute command 1
        execlp(argv[1], argv[1], NULL);
        // should not reach here!
        perror("execlp");
        exit(1);
    
    default: 
        switch(child2 = fork()){
        case -1:
            perror("fork");
            exit(1);

        case 0:  // child2 code
            // seitch stdin
            dup2(fd[0],fileno(stdin));
            close(fd[0]);
            close(fd[1]);
            // execute command 1
            execlp(argv[2], argv[2], NULL);
            // should not reach here!
            perror("execlp");
            exit(2);

        default: // parent code
            close(fd[1]);
            close(fd[0]);
            for(i = 1; i<=2; ++i)
                if (wait(NULL) == -1){
                    perror("wait");
                    exit(3);
                }
            return 0;
        }
    }
    return 0;
}



