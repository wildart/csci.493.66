/*
 * =====================================================================================
 *
 *       Filename:  thread_detach.c
 *
 *    Description:  Thread detach demo
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
#include <unistd.h>
#include <string.h>

void* routine(void *arg)
{
    int i;
    int bufsize = strlen(arg);
    int fd = 1;
    printf("Child running...\n");
    for(i=0;i<bufsize;++i)
    {
        usleep(500000);
        write(fd, arg+i, 1);
    }
    printf("\nChild is now exiting.\n");
    return NULL;
}

int main(int argc, char * argv[])
{
    pthread_t thread;
    pthread_attr_t attr;
    char *buf = "abcdefghijklmnopqrstuvwxyz";

    // Set attributes
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // Create thread
    if(pthread_create(&thread, NULL, routine, (void *)(buf)) != 0)
    {
        fprintf(stderr, "error creating a new thread\n");
        exit(1);
    }

    printf("Main thread is now exiting.\n");
    pthread_exit(NULL);
}
