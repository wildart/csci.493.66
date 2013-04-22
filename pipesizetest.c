/*
 * =========================================================================
 *
 *       Filename:  pipesizetest.c
 *
 *    Description:  Testing pipe buffer limits
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

int          count = 0;
sig_atomic_t full  = 0;

void on_alarm(int signo)
{
    printf("\nwrite() blocked with %d char in the pipe.\n", count);
    full = 1;
}

int main(int argc, char *argv[])
{
    int pipe_size;
    int bytesread;
    int amount_to_remove;
    int fd[2];
    char buffer[PIPE_BUF];
    char c = 'x';
    static struct sigaction sigact;

    // set signal handler
    sigact.sa_handler = on_alarm;
    sigfillset(&(sigact.sa_mask));
    sigaction(SIGALRM, &sigact, NULL);

    // try to create pipe
    if (pipe(fd) == -1){
        perror("pipe call");
        exit(2);
    }

    // check whether the _PC_PIPE_BUF constant returns the pipe capacity
    // or the atomic write size
    pipe_size = fpathconf(fd[0], _PC_PIPE_BUF);

    while(1){
        // set an alarm long enough that if write fails it will fail
        // within this amount of time
        alarm(4);
        write(fd[1], &c, 1);

        // unset alarm
        alarm(0);

        // did alarm expire? so write failed
        if (full)
            break;

        // report how many written so far
        if ((++count % 1024) == 0)
            printf ("%d chars in pipe\n", count);
    }

    printf("The max number of bytes that the pipe stored is %d.\n", count);
    printf("The value of _PC_PIPE_BUF is %d.\n\n",pipe_size);

    printf("Now we remove chars from the pipe and demo that we cannot\n"
           "write into the pipe unless it has %d (PIPE_BUF) free bytes.\n", PIPE_BUF);

    amount_to_remove = PIPE_BUF-1;
    printf("First we remove %d chars (PIPE_BUF-1) and try to write into the pipe.\n", amount_to_remove);

    full = 0;
    bytesread = read(fd[0], &buffer, amount_to_remove);
    if (bytesread < 0){
        perror("read");
        exit(1);
    }

    // try to write again
    count = count - bytesread;
    alarm(4);
    write(fd[1], &c, 1);
    alarm(0);
    if (full)
        printf("We could not write into the pipe.\n");
    else
        printf("We successfully wrote into the pipe.\n");

    amount_to_remove = PIPE_BUF - amount_to_remove;
    printf("\nNow we remove one more char and try to write into the pipe.\n");
    
    full = 0;
    bytesread = read(fd[0], &buffer, amount_to_remove);
    if (bytesread < 0){
        perror("read");
        exit(1);
    }
    count = count - bytesread;
    alarm(4);
    write(fd[1], &c, 1);
    alarm(0);
    if (full)
        printf("We could not write into the pipe.\n");
    else
        printf("We successfully wrote into the pipe.\n");
    

    exit(0);
}

