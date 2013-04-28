/*
 * =========================================================================
 *
 *       Filename:  upcased1.c
 *
 *    Description:  Iterative server
 *
 *        Version:  1.0
 *        Created:  04/28/2013 05:56:46 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include "upcase1.h"

#define WARGING  "\nNOTE: SERVER ** NEVER** accessed private FIFO\n"
#define MAXTRIES 5

int dummyfifo;   // file descriptor to write-end of PUBLIC
int privatefifo; // file descriptor to write-end of PRIVATE
int publicfifo;  // file descriptor to read-end of PUBLIC

void on_sigpipe(int signo)
{
    fprintf(stderr, "Client is not reading the pipe.\n");
}

void on_signal(int signo)
{
    close(publicfifo);
    close(dummyfifo);
    if( privatefifo != -1 )
        close(privatefifo);
    unlink(PUBLIC);
    exit(1);
}

int main(int argc, char *argv[])
{
    int              i;
    int              nbytes;  // number of bytes read from popen()
    int              tries;   // number of tries to open private FIFO
    int              done;    // flag to stop main loop
    message   msg;     // stores private fifo name and command
    struct sigaction handler; 

    // Register signal handlers
    handler.sa_handler = on_signal;
    handler.sa_flags = SA_RESTART;
    if( (sigaction(SIGINT, &handler, NULL) == -1) ||
         (sigaction(SIGHUP, &handler, NULL) == -1) ||
         (sigaction(SIGQUIT, &handler, NULL) == -1) ||
         (sigaction(SIGTERM, &handler, NULL) == -1) 
      ){
        perror("sigaction");
        exit(1);
    }

    handler.sa_handler = on_sigpipe;
    if(sigaction(SIGPIPE, &handler, NULL) == -1){
        perror("sigaction");
        exit(1);
    }

    // Create public FIFO
    if(mkfifo(PUBLIC, 0666) < 0){
        if(errno != EEXIST)
            perror(PUBLIC);
        else
            fprintf(stderr, "%s already exists. Delete it and restart.\n", PUBLIC);
        exit(1);
    }

    // Open public FIFO for reading and writing
    if( (publicfifo = open(PUBLIC, O_RDONLY)) == -1 ||
        (dummyfifo  = open(PUBLIC, O_WRONLY | O_NDELAY)) == -1){
        perror(PUBLIC);
        exit(1);
    }

    // Block waiting for a msg struct from client
    while( read(publicfifo, (char*) &msg, sizeof(msg)) > 0 ){
        // Message arrived, so start trying to open write end of private FIFO
        tries = done = 0;
        privatefifo = -1;
        do {
            if ( (privatefifo = open(msg.fifo_name, O_WRONLY | O_NDELAY)) == -1)
                sleep(1); // sleep if failed
            else {
                // COnvert the text to uppercase
                nbytes = strlen(msg.text);
                for(i=0;i<nbytes;++i)
                    if(islower(msg.text[i]))
                        msg.text[i] = toupper(msg.text[i]);
                
                // Send converted text to client
                if(write(privatefifo, msg.text, nbytes) == -1)
                    if (errno == EPIPE)
                        done = 1;
                // close write-end of private FIFO
                close(privatefifo);
                // terminate loop
                done = 1; 
            }
        } while( ++tries < MAXTRIES && !done );
        
        if(!done)
            // Failed to open client private FIFO for writing
            write(fileno(stderr), WARGING, sizeof(WARGING));
    }

    return 0;
}
