/*
 * =========================================================================
 *
 *       Filename:  upcaseclient2.c
 *
 *    Description:  Concurrent client
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
#include "upcase2.h"

#define MAXTRIES 5

const char startup_msg[] = "upcased2 does not seem to be running. Please start the server.\n";
const char server_no_read_msg[] = "The server is not readin the pipe.\n";

int convertedtext; // file descriptor for private read-end fifo
int rawtext;       // file descriptor for private write-end fifo
int dummyreadfifo; // to hold fifo open
int dummyrawfifo;  // to hold the raw text fifo open
int publicfifo;    // file descriptor to public write-end fifo
FILE* input_src;   // file pointer to inut strem
message msg;       // 2-way communication structure

void on_sigpipe(int signo)
{
    fprintf(stderr, "upcased is not reading the pipe.\n");
    unlink(msg.raw_text_fifo);
    unlink(msg.converted_text_fifo);
    exit(1);
}

void on_signal(int signo)
{
    close(publicfifo);
    if(convertedtext != -1)
        close(convertedtext);
    if(rawtext != -1)
        close(rawtext);
    unlink(msg.raw_text_fifo);
    unlink(msg.converted_text_fifo);
    exit(0);
}

void clean_up()
{
    close(publicfifo);
    close(rawtext);
    unlink(msg.converted_text_fifo);
    unlink(msg.raw_text_fifo);
}

int main(int argc, char *argv[])
{
    int         strLength;        // number of bytes in text to convert
    int         nChunk;           // index of text chunk to send to server
    int         bytesRead;        // bytes received in read from server
    int         tries;
    static char buffer[PIPE_BUF];
    static char textbuf[BUFSIZ];

    struct sigaction handler;

    // Check whether there is a command line argument
    if (argc > 1){
        if ((input_src = fopen(argv[1], "r")) == NULL){
            perror(argv[1]);
            exit(1);
        }
    }
    else
        input_src = stdin;

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

    // Initialize file descriptors for error handling
    publicfifo = -1;
    convertedtext = -1;
    rawtext = -1;

    // Create a unique name for a private FIFOs using pid
    sprintf(msg.converted_text_fifo, "/tmp/fifo_rd%d", getpid());
    sprintf(msg.raw_text_fifo, "/tmp/fifo_wr%d", getpid());

    // Create the private FIFOs
    if (mkfifo(msg.converted_text_fifo, 0666) < 0){
        perror(msg.converted_text_fifo);
        exit(1);
    }

    if (mkfifo(msg.raw_text_fifo, 0666) < 0){
        perror(msg.raw_text_fifo);
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

    // Send a message to server with names of two FIFOs
    write(publicfifo, (char*) &msg, sizeof(msg));

    // Try to open the raw text FIFO for writing.
    tries = 0;
    while( ((rawtext = open(msg.raw_text_fifo, O_WRONLY | O_NDELAY)) == -1) &&
           ( tries < MAXTRIES))
    {
        sleep(1);
        tries++;
    }
    if(tries == MAXTRIES){
        fprintf(stderr, "%s", server_no_read_msg);
        clean_up();
        exit(1);
    }

    // Get one line of input at a time from the input source
    while(1){
        memset(textbuf, 0x0, BUFSIZ);
        if(fgets(textbuf, BUFSIZ, input_src) == NULL)
            break;

        strLength = strlen(textbuf);
        
        // Break input lines into chunks and 
        // send them at a time through client 's write FIFO
        for(nChunk=0; nChunk<strLength; nChunk += PIPE_BUF-1){
            memset(buffer, 0x0, PIPE_BUF);
            strncpy(buffer, textbuf+nChunk, PIPE_BUF-1);
            buffer[PIPE_BUF-1] = '\0';
            write(rawtext, buffer, sizeof(buffer));

            // Open private fifo for reading to get output from server
            if((convertedtext = open(msg.converted_text_fifo, O_RDONLY)) == -1){
                perror(msg.converted_text_fifo);
                exit(1);
            }

            // Read maximum number of bytes possible atomically
            // and copy them to stdout
            memset(buffer, 0x0, PIPE_BUF);
            while((bytesRead = read(convertedtext, buffer, PIPE_BUF)) > 0){
                write(fileno(stdout), buffer, bytesRead);
            }
            close(convertedtext); 
            convertedtext = -1;
        }
    }

    // User quit, close write-end of public fifo and delete private fifo
    clean_up();
    return 0;
}
