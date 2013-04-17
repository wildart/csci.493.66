/*
 * =========================================================================
 *
 *       Filename:  waitpiddemo.c
 *
 *    Description:  Using waitpid() function
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
    pid_t pid;
    int status;

    printf("Starting up...\n");
    if((pid = fork()) == -1){
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
        child();
    else { // parent code
        /* wait for specific child process with waitpid() 
         * if no child has terminated, do not block in waitpid()
         * Instead just sleep
         */
        while (waitpid(pid,&status,WNOHANG) == 0){
            printf("still waiting for child\n");
            sleep(1);
        }
        
        /*  pid is the pid of the child that terminated */
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

