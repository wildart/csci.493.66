/*
 * =========================================================================
 *
 *       Filename:  upcaseclient1.c
 *
 *    Description:  Iterative client
 *
 *        Version:  1.0
 *        Created:  04/28/2013 03:18:42 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include "upcase1.h"

#define PROMPT "string: "
#define UPCASE "UPCASE: "
#define QUIT   "quit"

const char startup_msg[] = "upcased1 does not seem to be running. Please start the server.\n";

volatile sig_atomic_t sig_received = 0;
message msg;

void on_sigpipe(int signo)
{
    fprintf(stderr, "upcased is not reading the pipe.\n");
    unlink(msg.fifo_name);
    exit(1);
}

void on_signal(int signo)
{
    sig_received = 1;
}

int main(int argc, char *argv[])
{
    int         strLength;        // number of bytes in text to convert
    int         nChunk;           // index of text chunk to send to server
    int         bytesRead;        // bytes received in read from server
    int         privatefifo;
    int         publicfifo;
    static char buffer[PIPE_BUF];
    static char textbuf[BUFSIZ];

    struct sigaction handler;

    // Only run if we are using the terminal
    if (!isatty(fileno(stdin)) || !isatty(fileno(stdout)))
        exit(1);

    // Register the interrupt handler
    handler.sa_handler = on_signal;
    if ( (sigaction(SIGINT, &handler, NULL) == -1) ||
         (sigaction(SIGHUP, &handler, NULL) == -1) ||
         (sigaction(SIGQUIT, &handler, NULL) == -1) ||
         (sigaction(SIGTERM, &handler, NULL) == -1) 
       ){
       perror("sigaction");
       exit(1);
    }
    
    handler.sa_handler = on_sigpipe;
    if (sigaction(SIGPIPE, &handler, NULL) == -1){
        perror("sigaction");
        exit(1);
    }

    // Create a unique name for a private FIFO using pid
    sprintf(msg.fifo_name, "/tmp/fifo%d", getpid());

    // Create the private FIFO
    if (mkfifo(msg.fifo_name, 0666) < 0){
        perror(msg.fifo_name);
        exit(1);
    }

    // Open the public FIFO for writing
    if ( (publicfifo = open(PUBLIC, O_WRONLY | O_NONBLOCK)) == -1 ){
        if (errno == ENXIO)
            fprintf(stderr, "%s", startup_msg);
        else
            perror(PUBLIC);
        exit(1);
    }
    printf("Type 'quit' to exit.\n");

    // Prompt user for input, read it and send to server
    while(1){
        // Check for SIGINT, if received close write-end
        // of public fifo, remove private fifo and quit
        if (sig_received){
            close(publicfifo);
            unlink(msg.fifo_name);
            exit(1);
        }

        // Display prompt on the terminal and read input text
        write(fileno(stdout), PROMPT, sizeof(PROMPT));
        memset(msg.text, 0x0, HALF_PIPE_BUF);
        fgets(textbuf, BUFSIZ, stdin);
        strLength = strlen(textbuf);
        if (!strncmp(QUIT, textbuf, strLength-1)) // is it quit?
            break;

        // Display label for returned upper case text
        write(fileno(stdout), UPCASE, sizeof(UPCASE));

        for(nChunk=0; nChunk<strLength; nChunk += HALF_PIPE_BUF-1){
            memset(msg.text, 0x0, HALF_PIPE_BUF);
            strncpy(msg.text, textbuf+nChunk, HALF_PIPE_BUF-1);
            msg.text[HALF_PIPE_BUF-1] = '\0';
            write(publicfifo, (char*)&msg, sizeof(msg));

            // Open private fifo for reading to get output from server
            if((privatefifo = open(msg.fifo_name, O_RDONLY)) == -1){
                perror(msg.fifo_name);
                exit(1);
            }

            // Read maximum number of bytes possible atomically
            // and copy them to stdout
            while((bytesRead = read(privatefifo, buffer, PIPE_BUF)) > 0){
                write(fileno(stdout), buffer, bytesRead);
            }
            close(privatefifo); // close read-end of private fifo
        }
    }

    // User quit, close write-end of public fifo and delete private fifo
    close(publicfifo);
    unlink(msg.fifo_name);
    return 0;
}
