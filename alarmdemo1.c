/*
 * =========================================================================
 *
 *       Filename:  alarmdemo1.c
 *
 *    Description:  Working with alarm signal
 *
 *        Version:  1.0
 *        Created:  04/08/2013 02:53:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =========================================================================
 */

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>

/* This is SIGALRM handler. When signal is delivered to this process, 
 * it resets the handler and displays current time
 */
void catchalarm(int signo)
{
    signal(SIGALRM, catchalarm); // install handler again
    time_t t;
    struct tm *tp;

    time(&t); // get current time
    tp = localtime(&t); // convert time to tm struct
    printf("Caught alarm at %d:%d:%d\n", tp->tm_hour, tp->tm_min, tp->tm_sec);
}

int main(int argc, char *argv[])
{
    int k;
    struct tm *tp;
    time_t t;

    /*  check proper usage */
    if (argc< 2)
    {
        printf("Usage: %s  n\n", argv[0]);
        exit(1);
    }

    k = atoi(argv[1]); // convert to intager argument
    signal(SIGALRM, catchalarm);

    time(&t);
    tp = localtime(&t); // convert time to tm struct
    printf("Time is %d:%d:%d\n", tp->tm_hour, tp->tm_min, tp->tm_sec);
    
    // set alarm
    printf("Alarm is set for %d seconds.\n", k);
    alarm(k);

    pause();
    return 0;
}







