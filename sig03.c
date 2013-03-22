/*
 * =========================================================================
 *
 *       Filename:  sig03.c
 *
 *    Description:  Signal demo
 *
 *        Version:  1.0
 *        Created:  03/21/2013 12:10:20 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky 
 *        Company:  Hunter College
 *
 * =========================================================================
 *
 * Usage:
 *      sigaction_demo [reset|noblock|restart]*
 * 
 *  i.e.,0 or more of the words reset, noblock,and restart may appear
 *  on the command line, and multiple instances of the same word as the same
 *  effect as a single instance.
 * 
 *  reset   turns on SA_RESETHAND
 *  noblock turns on SA_NODEFER
 *  restart turns on SA_RESTART
 *
 * NOTES
 * (1) If you supply the word "reset" on the command line it will set the
 *      handling to SIG_DFL for signals that arrive when the process is
 *      in the handler. If noblock is also set, the signal will have the
 *      default behavior immediately. If it is not set, the default will
 *      delay until after the handler exits. If noblock is set but reset is
 *      not, it will recursively enter the handler.
 * (2) The interrupt_handler purposely delays for a few seconds in order to
 *     give the user time to enter a few interrupts on thekeyboard.
 * (3) interrupt_handler is the handler for both SIGINT and SIGQUIT, so if it
 *     is not reset, neither Ctrl-C nor Ctrl-\ will kill it.
 * (4) It will ask you to enter the numeric values of signals to block. If
 *     you don't give any, no signals are blocked.
 *
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#define INPUTLEN 100

void interrupt_handler(int signo, siginfo_t *info, void *context);

int main(int argc, char *argv[])
{
    struct sigaction newhandler;    /*  new settings */
    sigset_t         blocked;       /*  set of blocked sigs */
    char             x[INPUTLEN];
    int              flags = 0;
    int              signo, n;
    char             s[] = "Entered text: ";
    int              s_len = strlen(s);

    while (argc > 1)
    {
        if (strncmp("reset", argv[argc-1], strlen(argv[argc-1])) == 0){
            flags |= SA_RESETHAND;
            printf("SA_RESETHAND is set\n");
        }
        else if (strncmp("noblock", argv[argc-1], strlen(argv[argc-1])) == 0){
            flags |= SA_NODEFER;
            printf("SA_NODEFER is set\n");
        }
        else if (strncmp("restart", argv[argc-1], strlen(argv[argc-1])) == 0){
            flags |= SA_RESTART;
            printf("SA_RESTART is set\n");
        }
        argc--;
    }

    /*  load these two fields first */
    newhandler.sa_sigaction = interrupt_handler; /* handler function */
    newhandler.sa_flags = SA_SIGINFO | flags;    /* new style handler */

    /*  then build the list of blocked first */
    sigemptyset(&blocked);

    printf("PID: %d\n", getpid());
    printf("Type the numeric value of a signal to block (0 to stop): ");
    while(1)
    {
        scanf("%d", &signo);
        if (signo == 0)
            break;
        if (sigaddset(&blocked, signo) == -1)
        {
            printf("Problem adding signal %d to mask.\n", signo);
            break;
        }
        printf("Signal %d added to mask.\n", signo);
        printf("next signal number (0 to stop): ");
    }
    newhandler.sa_mask = blocked; /* store blockmask  */

    // install interrupt_handler as the SIGINT handler
    if(sigaction(SIGINT, &newhandler, NULL) == -1)
        perror("sigaction with SIGINT");
    // if successful, install interrupt_handler as the SIGQUIT handler also
    else if(sigaction(SIGQUIT, &newhandler, NULL) == -1)
        perror("sigaction with SIGQUIT");
    else
        while(1)
        {
            x[0]='\0';
            tcflush(0, TCIOFLUSH);
            printf("Enter text then <RET> or 'quit' for quit:\n");
            n = read(0, &x, INPUTLEN);
            if(n == -1 && errno == EINTR)
            {
                printf("read call was interupted\n");
                x[n]='\0';
                write(1, &x, n+1);
            }
            else if(strncmp("quit", x, 4) == 0)
                break;
            else
            {
                x[n] = '\0';
                write(1, &s, s_len);
                write(1, &x, n+1);
                printf("\n");
            }
        } // while

    return 0;
}

void interrupt_handler(int signo, siginfo_t *info, void *context)
{
    int     localid;  // stores number to uniquely identify signal
    time_t  timenow;  // curent time
    static  int ticker = 0;
    struct  tm *tp;
    
    time(&timenow);
    tp = localtime(&timenow);
    localid = 36000*tp->tm_hour + 600*tp->tm_min + 10*tp->tm_sec + 
              ticker++ % 10;
    printf("Entered handler: sig = %d, \tid = %d\n",
            info->si_signo, localid);
    sleep(5);
    printf("Leaving handler: sig = %d, \tid = %d\n",
            info->si_signo, localid);
}

