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

#define N 4

int main(int argc, char *argv[])
{
    int i;

    printf("About to create many processes...\n");
    for(i=0;i<N;i++)
        if (  fork() == -1 )
            exit(1);

    // output in both processes
    printf("Process id = %d\n", getpid());
    fflush(stdout); // force output
    sleep(1);
    return 0;
}
