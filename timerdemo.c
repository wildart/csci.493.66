/*
 * =========================================================================
 *
 *       Filename:  timerdemo.c
 *
 *    Description:  Time demo
 *
 *        Version:  1.0
 *        Created:  04/08/2013 06:30:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky 
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>

int set_timer(int which, long initial, long repeat)
{
    struct itimerval itimer;
    long secs;
    
    // Init delay
    secs = initial / 1000;
    itimer.it_value.tv_sec  = secs;
    itimer.it_value.tv_usec = (initial - secs*1000) * 1000;

    // Init repeat interval
    secs = repeat / 1000;
    itimer.it_interval.tv_sec  = secs;
    itimer.it_interval.tv_usec = (repeat - secs*1000) * 1000;

    return setitimer(which, &itimer, NULL);
}

void count_alarms(int signo)
{
    int alarms_accepted = 10;
    static int count = 0;
    printf("alarm %d \n", ++count);
    fflush(stdout);
    if (alarms_accepted == count)
    {
        printf("No more alarms allowed!\n");
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    int initial = 250;
    int repeat  = 500;

    if (argc == 3)
    {
        initial = atoi(argv[1]);
        repeat  = atoi(argv[2]);
    }

    if (initial == 0 || repeat == 0)
    {
        printf("Incorrect timer values. It cannot be 0\n");
        exit(1);
    }

    // Install signal handler
    signal(SIGALRM, count_alarms);

    // Setup timer
    if  (set_timer(ITIMER_REAL, initial, repeat) == -1)
        perror("set timer");
    else
        while(1)
            pause();

    return 0;
}
