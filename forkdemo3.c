/*
 * =========================================================================
 *
 *       Filename:  forkdemo1.c
 *
 *    Description:  Demo for fork function (correct)
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
#include <string.h>
#include <fcntl.h>
#include <termios.h>

int main(int argc, char *argv[])
{
    int i;
    int N;
    char str[32];
    int flags;

    // Put stdout into atomic append mode
    flags = fcntl(1, F_GETFL);
    flags |= (O_APPEND);
    if (fcntl(1, F_SETFL, flags) == -1)
        exit(1);

    // Get command line value of proc count
    N = (argc > 1) ? atoi(argv[1]) : 4;
    if (N == 0)
        exit(1);

    // Print a message and flush it if this is not terminal
    printf("About to create many processes...\n");
    if (!isatty(fileno(stdout)))
        fflush(stdout);

    // Fork children
    for(i=0;i<N;i++)
        if (  fork() == -1 )
            exit(1);

    // Create output string and write it with system call
    sprintf(str, "Process id = %d\n", getpid());
    write(1, str, strlen(str));
    fflush(stdout); // force output
    sleep(1);
    return 0;
}
