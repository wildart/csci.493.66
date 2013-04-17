/*
 * =========================================================================
 *
 *       Filename:  waitiddemo.c
 *
 *    Description:  Using waitid()
 *
 *        Version:  1.0
 *        Created:  04/17/2013 02:20:36 PM
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
#include <sys/wait.h>
#include <signal.h>

#define SLEEPTIME 60

int main(int argc, char *argv[])
{
    pid_t     pid;
    siginfo_t siginfo;

    if ((pid = fork()) == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0){
        printf("Child pid is %d.\n", getpid());
        sleep(SLEEPTIME);
        exit(0);
    }
    else do { // parent code
        // Zeor out si_pid in case the sig_info_t struct does not get
        // initialized because no children are waitable.
        siginfo.si_pid = 0;

        // Wait for changes in the state of the child created above,
        // specifically, stopping, resuming, exiting and return
        // immediately if no child is waitable
        if ( waitid(P_PID, pid, &siginfo, 
                    WEXITED | WSTOPPED | WCONTINUED | WNOHANG) == -1){
            perror("waitid");
            exit(EXIT_FAILURE);
        }

        if (siginfo.si_pid == 0)
            // no child is waitable
            continue;

        switch(siginfo.si_code){
            case CLD_EXITED:
                printf("Child exited with status %d.\n", siginfo.si_status);
                break;
            case CLD_KILLED:
            case CLD_DUMPED:
                printf("Child killed by signal %d.\n", siginfo.si_status);
                break;
            case CLD_CONTINUED:
                printf("Child continued.\n");
                break;
        }

    } while(siginfo.si_code != CLD_EXITED &&
            siginfo.si_code != CLD_KILLED &&
            siginfo.si_code != CLD_DUMPED);
    return 0;
}
