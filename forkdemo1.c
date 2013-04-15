/*
 * =========================================================================
 *
 *       Filename:  forkdemo1.c
 *
 *    Description:  Demo for fork function
 *
 *        Version:  1.0
 *        Created:  04/15/2013 05:05:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Artyom Diky 
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int global = 10;

int main(int argc, char *argv[])
{
    int   local = 0;
    pid_t pid;

    printf("Parent process: (Before fork())");
    printf("  local = %d, global = %d \n", local, global);

    if ( (pid = fork()) == -1 )
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {
        // Child brunch
        printf("After the fork in the child:");
        local++;
        global++;
        printf("  local = %d, global = %d \n", local, global);
    }
    else
    {
        // parent brunch
        sleep(2);
    }

    // output in both processes
    printf("pid = %d, local = %d, global = %d \n", getpid(), local, global);
    return 0;
}
