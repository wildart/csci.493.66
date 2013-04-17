/*
 * =========================================================================
 *
 *       Filename:  syncdemo2.c
 *
 *    Description:  Sync forked processes
 *
 *        Version:  2.0
 *        Created:  04/17/2013 06:28:21 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

//=========================================================================

void p_action(int sig);
void c_action(int sig);
void on_sigint(int sig);

//=========================================================================

int                   nSignals = 0;
volatile sig_atomic_t sigint_received = 0;

//=========================================================================

int main(int argc, char *argv[])
{
    pid_t pid, ppid;
    static struct sigaction parentAct, childAct;
    int  fd;
    int  counter = 0;
    char childbuf[40];
    char parentbuf[40];

    // Check arguments
    if (argc < 2){
        printf("usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    // Open file
    if ( (fd = open(argv[1], O_CREAT|O_WRONLY|O_TRUNC, 0644)) == -1){
        perror(argv[1]);
        exit(1);
    }

    switch(pid = fork()){
    case -1:
        // fork failed
        perror("fork() failed!");
        exit(1);
    
    case 0: {
        // child brunch
        childAct.sa_handler = c_action;
        sigaction(SIGUSR1, &childAct, NULL);
        ppid = getppid(); // get parent id
        for(;;){
            sprintf(childbuf, "Child counter = %d\n", counter++);
            write(fd, childbuf, strlen(childbuf));
            printf("Sending signal to parent --- ");
            fflush(stdout);
            kill(ppid, SIGUSR1);
            sleep(3);
        }
    }
    default: {
        // set SIGUSR1 action for parent
        parentAct.sa_handler = p_action;
        sigaction(SIGUSR1, &parentAct, NULL);
        
        // set SIGINT action for parent
        parentAct.sa_handler = on_sigint;
        sigaction(SIGINT, &parentAct, NULL);

        for(;;){
            sleep(3);
            sprintf(parentbuf, "Parent counter = %d\n", counter++);
            write(fd, parentbuf, strlen(parentbuf));
            printf("Sending signal to child ---");
            fflush(stdout);

            kill(pid, SIGUSR1);
            
            // exit when interrupted
            if (sigint_received){
                close(fd);
                exit(0);
            }
        }
    }}

    return 0;
}

//=========================================================================
void p_action(int sig)
{
    printf("Parent caught signal %d\n", ++nSignals);
}

void c_action(int sig)
{
    printf("Parent caught signal %d\n", ++nSignals);
}

void on_sigint(int sig)
{
    sigint_received = 1;
}
