/*
 * =========================================================================
 *
 *       Filename:  syncdemo1.c
 *
 *    Description:  Sync forked processes
 *
 *        Version:  1.0
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

void c_action(int signum)
{
    /* nothing to do here */
}

int main(int argc, char *argv[])
{
    pid_t pid;
    int status;
    static struct sigaction childAct;

    switch(pid = fork())
    {
        case -1:
            // fork failed
            perror("fork() failed!");
            exit(1);
    
        case 0: {
            // child brunch
            // set SIGUSR1 action for child
            int i, x = 1;
            childAct.sa_handler = c_action;
            sigaction(SIGUSR1, &childAct, NULL);
            pause();
            printf("Child process: starting computations...\n");
            for(i=0;i<10;i++)
            {
                printf("2^%d = %d\n", i, x);
                x = 2*x;
            }
            exit(0);
        }
        default: {
            // parent branch
            printf("Parent process: "
                   "Will wait 2 sec to prove child waits.\n");
            sleep(2);
            printf("Parent process: "
                   "Sending child signal to start computation.\n");
            kill(pid, SIGUSR1);

            /* parent waits for child to return here */
            if ((pid = wait(&status)) == -1)
            {
                perror("wait failed");
                exit(2);
            }
            printf("Parent process: child terminated.\n");
            exit(0);
        }
    }

    return 0;
}
