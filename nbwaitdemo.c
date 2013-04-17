/*
 * =========================================================================
 *
 *       Filename:  nbwaitdemo.c
 *
 *    Description:  Non-blocking process waiting
 *
 *        Version:  1.0
 *        Created:  04/17/2013 10:02:57 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <limits.h>
#include <sys/wait.h>
#include <termios.h>

#define CHILDREN  5
#define SLEEPTIME 30

/* The code that us executed by each child process 
 * All this does is registers the SIGINT signal handler and the sleeps 
 * If a child is delivered SIGINT, it exits with the exit code 99
 */
void child();

/* Signal handler for SIGINT
 * All it does it call exit with a code of 99
 */
void on_sigint(int signo);

/* Singnal handler for  SIGCHLD
 * This calls wait() to retrieve the status of the terminated child
 * and get its pid. These are both stored into global variables that 
 * the perent can access it the main class. It sets a global flag.
 */
void on_sigchld(int signo);

/* These varibles are declared with the volatile qualifire to tell
 * the cmpiler that they are used in a signalhandler and their values
 * changed asynchroniusly, so no optimization required.
 */
volatile sig_atomic_t child_terminated;
volatile int          status;
volatile pid_t        pid;

int main(int argc, char *argv[])
{
    int count = 0;
    const int NumChildren = CHILDREN;
    int i;
    struct sigaction newhandler;

    printf("About to create many children processes...\n");
    for(i=0;i<NumChildren;i++){
        if ((pid = fork()) == -1){
            perror("fork");
            exit(-1);
        }
        else if (pid == 0){ // child code
            // Close standard output so that children do not print to parent output again
            close(1);
            child();
            exit(1);
        }
        else { // parent code
            if (i == 0)
                printf("Another ");
            else if (i < NumChildren-1)
                printf("and another ");
            else 
                printf("and another.\n");
        }
    }

    // parent continues here

    // setup signal handler
    newhandler.sa_handler = on_sigchld;
    sigemptyset(&newhandler.sa_mask);
    if (sigaction(SIGCHLD, &newhandler, NULL) == -1){
        perror("sigaction");
        return 1;
    }

    // Enter loop in which work could happen while the global flg
    // is checked to see if any child has terminated
    child_terminated = 0;
    while(count < NumChildren){
        if(child_terminated){
            if (WIFEXITED(status))
                printf("Child process %d died with code %d.\n", pid, WEXITSTATUS(status));
            else if(WIFSIGNALED(status))
                printf("Child process %d was killed by signal %d.\n", pid, WTERMSIG(status));
            else
                printf("Child process %d dies with status %d.\n", pid, status);
            child_terminated = 0;
            count++;
        }
        else {
            // do something
            sleep(1);
        }

    }

    printf("All children processes terminated.\n");
    return 0;
}

void on_sigint(int signo)
{
    exit(99);
}

void on_sigchld(int signo)
{
    int child_status;
    if ((pid = wait(&child_status)) == -1)
        perror("wait failed");

    child_terminated = 1;
    status = child_status;
}

void child()
{
    struct sigaction newhandler;

    newhandler.sa_handler = on_sigint;
    sigemptyset(&newhandler.sa_mask);
    if ( sigaction(SIGINT, &newhandler, NULL) == -1){
        perror("sigaction");
        exit(1);
    }
    sleep(SLEEPTIME);
}
