/*
 * =========================================================================
 *
 *       Filename:  rcvfifo1.c
 *
 *    Description:  FIFO server
 *
 *        Version:  1.0
 *        Created:  04/24/2013 04:00:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky 
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include "fifo1.h"
#include <signal.h>

int dummyfifo;  // file descriptor to write-end of PUBIC
int publicfifo; // file descriptor to read-end of PUBIC

/* This closes both ends of the FIFO and then removes it,
 * after which it exits the program 
 */
void on_signal(int signo)
{
    close(publicfifo);
    close(dummyfifo);
    unlink(PUBLIC);
    exit(0);
}

int main(int argc, char *argv[])
{
    struct sigaction handler;
    int              nbytes;
    static char      buffer[PIPE_BUF];
    int              count = 0;

    // register handlers for some signals
    handler.sa_handler = on_signal;
    handler.sa_flags = SA_RESTART;
    if ( (sigaction(SIGINT, &handler, NULL) == -1) ||
         (sigaction(SIGHUP, &handler, NULL) == -1) ||
         (sigaction(SIGQUIT, &handler, NULL) == -1) ||
         (sigaction(SIGTERM, &handler, NULL) == -1) 
       )
    {
        perror("sigaction");
        exit(1);
    }

    // Create public FIFO. If exists already, the call will return -1 and set errno to EEXIST.
    // If so we reuse an existing FIFO.
    if (mkfifo(PUBLIC, 0666) < 0){
        if (errno != EEXIST){
            perror(PUBLIC);
            exit(1);
        }
    }

    // Open FIFO for reading with O_NONBLOCK flag clear.
    // The POSIX states the process will be blocked on the open() until some
    // process opens it for writing. Therefore, the server stuck here until client starts up.
    if ((publicfifo = open(PUBLIC, O_RDONLY)) == -1){
        perror(PUBLIC);
        exit(1);
    }

    // Open FIFO for writing, even though we have no intention to do that.
    // We will not reach the call to open() until a client runs, but once the client runs, 
    // the server opens FIFO for writing. If we do not do this, when client terminates and closes
    // its write-end of the FIFO, the server's read loop would exit and the server would also exit.
    // This "dummy" open keeps server alive.
    if ((dummyfifo = open(PUBLIC, O_WRONLY | O_NONBLOCK)) == -1){
        perror(PUBLIC);
        exit(1);
    }

    // Block waiting for a message from a client
    while(1){
        memset(buffer, 0 ,PIPE_BUF);
        if ( (nbytes =  read( publicfifo, buffer, PIPE_BUF)) > 0){
            buffer[nbytes] = '\0';
            printf("Message %d resceived by server: %s", ++count, buffer);
            fflush(stdout);
        }
        else
            break;
    }

    return 0;
}
