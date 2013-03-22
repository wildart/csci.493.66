/*
 * =========================================================================
 *
 *       Filename:  sig04.c
 *
 *    Description:  Signal masking
 *
 *        Version:  1.0
 *        Created:  03/22/2013 03:41:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    int i;
    sigset_t sigs, prevsigs;

    // Createing signal set
    sigemptyset(&sigs);
    sigaddset(&sigs, SIGINT);

    // Blocking signals specified in mask
    sigprocmask(SIG_BLOCK, &sigs, &prevsigs);
    for(i=0;i<5;++i)
    {
        printf("Waiting: %d\n", i);
        sleep(1);
    }

    // Restore signal mask
    sigprocmask(SIG_SETMASK, &prevsigs, NULL);
    for(i=0;i<5;++i)
    {
        printf("After: %d\n", i);
        sleep(1);
    }

    return 0;
}
