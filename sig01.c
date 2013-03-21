/*
 * =========================================================================
 *
 *       Filename:  sig01.c
 *
 *    Description: Signal handling  
 *
 *        Version:  1.0
 *        Created:  03/20/2013 08:13:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky 
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void catch1(int signum)
{
    printf("You cannot interupt me!\n");
}

void catch2(int signum)
{
    printf("You cannot quit!\n");
}

int main()
{
    int i;

    signal(SIGINT, catch1);
    signal(SIGQUIT,catch2);
    //signal(SIGINT, SIG_IGN);
    //signal(SIGQUIT,SIG_IGN);
    //signal(SIGINT, SIG_DFL);
    //signal(SIGQUIT,SIG_DFL);

    for(i=0;i<20;i++)
    {
        printf("Try to kill with ^C or ^\\.\n");
        sleep(1);
    }

    return 0;
}
