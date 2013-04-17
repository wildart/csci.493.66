/*
 * =========================================================================
 *
 *       Filename:  waitdemo2.c
 *
 *    Description:  Using wait() function
 *
 *        Version:  1.0
 *        Created:  04/17/2013 08:22:40 AM
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

void child()
{
    int exit_code;
    printf("Child process with id: %d\n", getpid());
    sleep(2);
    printf("Enter value for the child exit code: ");
    scanf("%d", &exit_code);
    exit(exit_code);
}

int main(int argc, char *argv[])
{
    int pid;
    int status;

    printf("Starting up...\n");
    if((pid = fork()) == -1){
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
        child();
    else { // parent code
        printf("My child has pid %d and my pid is %d.\n", pid, getpid());
        if ((pid = wait(&status)) == -1){
            perror("wait");
            exit(2);
        }
        if (WIFEXITED(status))
            printf("Parent: Child %d exited with status %d.\n", pid, WEXITSTATUS(status)); 
        else if ( WIFSIGNALED(status) ) {
            printf("Parent: Child %d exited with error code %d.\n", pid, WTERMSIG(status));
#ifdef WCOREDUMP
            if ( WCOREDUMP(status) )
                printf("Parent: A core dump took place\n");
#endif
        }
    }
    return 0;
}

