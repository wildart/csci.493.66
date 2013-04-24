/*
 * =========================================================================
 *
 *       Filename:  sendfifo1.c
 *
 *    Description:  FIFO client
 *
 *        Version:  1.0
 *        Created:  04/24/2013 04:41:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky 
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include "fifo1.h"
#define  QUIT  "quit"

int main(int argc, char *argv[])
{
    int nbytes;
    int publicfifo;
    char text[PIPE_BUF];

    // Open public FIFO for writing
    if( (publicfifo = open(PUBLIC, O_WRONLY)) == -1 ){
        perror(PUBLIC);
        exit(1);
    }

    printf("Type 'quit' for exit.\n");

    // Repeatedly prompt user for command and send it too server
    while(1){
        memset(text, 0, PIPE_BUF);
        nbytes = read(fileno(stdin), text, PIPE_BUF);
        if(!strncmp(QUIT, text, nbytes-1))
            break;

        if( (write(publicfifo, text, nbytes)) < 0 ){
            perror("Server is no longer running");
            break;
        }
    }

    close(publicfifo);
    return 0;
}
