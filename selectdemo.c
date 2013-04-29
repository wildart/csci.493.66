/*
 * =========================================================================
 *
 *       Filename:  selectdemo.c
 *
 *    Description:  Multiplexed IO with select()
 *
 *        Version:  1.0
 *        Created:  04/29/2013 04:13:02 PM
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
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>

#define MSGSIZE 6

char msg1[] = "Hello";
char msg2[] = "Bye!!";

void parent(int pipeset[3][2]);
void child(int fd[2]);

int main(int argc, char *argv[])
{
    int fd[3][2]; // arry of 3 pipes
    int i;

    for(i=0;i<3;++i){
        // create pipes
        if(pipe(fd[i]) == -1){
            perror("pipe");
            exit(1);
        }

        // fork children
        switch(fork()){
        case -1:
            fprintf(stderr, "fork failed.\n");
            exit(1);
        case 0:
            child(fd[i]);
        }
    }
    // run parent code
    parent(fd);
    return 0;
}

void parent(int pipeset[3][2])
{
    char   buf[MSGSIZE];
    char   line[80];
    fd_set initial, copy;
    int i, nbytes;

    for(i=0; i<3; ++i)
        close(pipeset[i][1]); // close write ends

    // Create descriptor mask
    FD_ZERO(&initial);
    FD_SET(0, &initial); // add stdin

    for(i=0; i<3; ++i)
        FD_SET(pipeset[i][0], &initial); // add read end of each pipe

    copy = initial;
    while( select(pipeset[2][0]+1, &copy, NULL, NULL, NULL) > 0 ){
        // check stdin first
        if(FD_ISSET(0, &copy)){
            printf("From standard input: ");
            nbytes = read(0, line, 81);
            line[nbytes] = '\0';
            printf("%s", line);
        }

        // check the pipe from each child
        for(i=0; i<3; ++i){
            if(FD_ISSET(pipeset[i][0], &copy)){
                // it is ready to read
                if(read(pipeset[i][0], buf, MSGSIZE) > 0){
                    printf("Message from child %d:%s\n", i, buf);
                }
            }
        }
        if(waitpid(-1,NULL,WNOHANG) == -1)
            return;
        copy = initial;
    }
}

void child(int fd[2])
{
    int count;
    close(fd[0]);
    
    for(count=0; count<10; ++count){
        write(fd[1], msg1, MSGSIZE);    
        sleep(1 + getpid()%6);
    }

    write(fd[1], msg2, MSGSIZE);
    exit(0);
}
