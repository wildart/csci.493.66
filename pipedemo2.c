/*
 * =========================================================================
 *
 *       Filename:  pipedemo2.c
 *
 *    Description:  Atomic pipe operations
 *
 *        Version:  1.0
 *        Created:  04/22/2013 03:06:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =========================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <sys/wait.h>

#define READ_END  0
#define WRITE_END 1
#define RD_CHUNK  10
//#define ATOMIC

#ifndef PIPE_BUF
    #define PIPE_BUF POSIX_PIPE_BUF
#endif

void do_nothing(int signo)
{
    return;
}

int main(int argc, char *argv[])
{
    int i, repeat;
    int bytesread;
    int mssglen;
    pid_t child1, child2;
    int fd[2];
    int outfd;
    char message[RD_CHUNK+1];
    char *child1_chunk, *child2_chunk;
    long chunk_size;

    static struct sigaction sigact;

    // set signal handler
    sigact.sa_handler = do_nothing;
    sigfillset(&(sigact.sa_mask));
    sigaction(SIGUSR1, &sigact, NULL);

    // check proper usage
    if (argc<2){
        fprintf(stderr, "Usage: %s szie\n", argv[0]);
        exit(1);
    }

    // try to create pipe
    if (pipe(fd) == -1){
        perror("pipe call");
        exit(2);
    }

    // set pipe buffer size
    repeat = atoi(argv[1]);
#ifdef ATOMIC
    chunk_size = PIPE_BUF;
#else
    chunk_size = PIPE_BUF + 200;
#endif
    printf("Chunk size = %ld\n", chunk_size);
    printf("Value of PIPE_BUF is %d\n", PIPE_BUF);

    // create bufferes
    child1_chunk = calloc(chunk_size, sizeof(char));
    child2_chunk = calloc(chunk_size, sizeof(char));
    if ( (child1_chunk == NULL) || 
         (child2_chunk == NULL)) {
        perror("calloc");
        exit(2);
    }

    // create string that child1 writes
    child1_chunk[0]= '\0';
    for(i=0;i<chunk_size-2;i++)
        strcat(child1_chunk,"X");
    strcat(child1_chunk, "\n");
    
    // create string that child2 writes
    child2_chunk[0]= '\0';
    for(i=0;i<chunk_size-2;i++)
        strcat(child2_chunk,"y");
    strcat(child2_chunk, "\n");

    // create first child process
    switch( child1 = fork() ){
    case -1: // fork failed
        perror("fork");
        exit(3);

    case 0: // child1 code
        mssglen = strlen(child1_chunk);
        pause();
        for(i=0; i<repeat; ++i){     
            if ( write(fd[WRITE_END], child1_chunk, mssglen) != mssglen ){
                perror("write");
                exit(4);
            }
        }
        close(fd[WRITE_END]);
        exit(0);

   default: // parent create second child proceess
        switch(child2 = fork()){
        case -1:
            perror("fork");
            exit(5);

        case 0: // child2 code
            mssglen = strlen(child2_chunk);
            pause();
            for(i=0; i<repeat; ++i){     
                if ( write(fd[WRITE_END], child2_chunk, mssglen) != mssglen ){
                    perror("write");
                    exit(6);
                }
            }
            close(fd[WRITE_END]);
            exit(0);
        
        default: // parent code
            outfd = open("pd2_output", O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (outfd == -1){
                perror("open");
                exit(7);
            }

            close(fd[WRITE_END]);

            // signal to children
            kill(child1, SIGUSR1);
            kill(child2, SIGUSR1);

            // read messages
            while( (bytesread = read(fd[READ_END], message, RD_CHUNK)) != 0){
                if (bytesread > 0) {
                    write(outfd, message, bytesread);
                }
                else {
                    perror("read");
                    exit(8);
                }
            }
            close(outfd);
            
            // collect zombies
            for(i=1;i<=2;i++)
                if (wait(NULL) == -1){
                    perror("wait");
                    exit(9);
                }

            close(fd[READ_END]);
            free(child1_chunk);
            free(child2_chunk);
        }
    }

    exit(0);
}

