/*
 * =========================================================================
 *
 *       Filename:  atexitdemo.c
 *
 *    Description:  Using atexit()
 *
 *        Version:  1.0
 *        Created:  04/17/2013 08:01:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void worker(void)
{
    printf("Worker #1 : Finished for the day.\n");
}

void foreman(void)
{
    printf("Foreman   : Workers can stop for the day.\n");
}

void boss(void)
{
    printf("First Boss: Foreman, tell all workers to stop work.\n");
}

int main(void)
{
    long max_exit_functions = sysconf(_SC_ATEXIT_MAX);

    printf("Maximum number of exit functions is %ld\n", max_exit_functions);

    if ((atexit(worker)) != 0){
        fprintf(stderr, "cannot set exit function\n");
        return EXIT_FAILURE;
    }
    
    if ((atexit(foreman)) != 0){
        fprintf(stderr, "cannot set exit function\n");
        return EXIT_FAILURE;
    }

    if ((atexit(boss)) != 0){
        fprintf(stderr, "cannot set exit function\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
