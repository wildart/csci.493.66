/*
 * =========================================================================
 *
 *       Filename:  sig02.c
 *
 *    Description:  Signal handling
 *
 *        Version:  1.0
 *        Created:  03/20/2013 11:38:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int signo, siginfo_t *info, void *context)
{
    printf("\n");
    printf("Signal number: %d\n", info->si_signo);
    printf("Error number:  %d\n", info->si_errno);
    printf("PID of sender: %d\n", info->si_pid);
    printf("UID of sender: %d\n", info->si_uid);
}

int main(int argc, char* argv[])
{
    struct sigaction the_action;
    
    the_action.sa_flags = SA_SIGINFO;
    the_action.sa_sigaction = sig_handler;

    sigaction(SIGINT, &the_action, NULL);

    printf("Type ^C within the next minute"
           " or send signal 2 to process %d. \n", getpid());
    sleep(60);
    return 0;
}

