/*
 * =====================================================================================
 *
 *       Filename:  thread_join.c
 *
 *    Description:  Thread join demo
 *
 *        Version:  1.0
 *        Created:  05/01/2013 03:35:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int exitval;

void* hello_world(void *world)
{
    printf("Hello World from %s.\n", (char *)world);
    exitval = 2;
    pthread_exit((void*)exitval);
}

int main(int argc, char * argv[])
{
    pthread_t child_thread;
    void *status;
    char *planet = "Pluto";

    // Create thread
    if(pthread_create(&child_thread, NULL, hello_world, (void *)planet) != 0)
    {
        perror("pthred create");
        exit(-1);
    }
    // Join thread
    pthread_join(child_thread, (void **) (&status));
    printf("Child exited with status %ld\n", (long) status);
    return 0;
}
