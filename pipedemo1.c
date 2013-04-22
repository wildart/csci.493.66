/*
 * =========================================================================
 *
 *       Filename:  pipedemo1.c
 *
 *    Description:  Pipe demo (half-duplex mode)
 *
 *        Version:  1.0
 *        Created:  04/22/2013 03:06:38 PM
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
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define READ_FD   0
#define WRITE_FD  1
#define BUFSIZE 256

int main(int argc, char *argv[])
{
    int i, nbytes;
    int fd[2];
    char message[BUFSIZE];

    // check proper usage
    if (argc<2){
        fprintf(stderr, "Usage: %s message\n", argv[0]);
        exit(1);
    }

    // try to create pipe
    if (pipe(fd) == -1){
        perror("pipe call");
        exit(2);
    }

    // create child process
    switch(fork()){
    case -1:
        perror("forr");
        exit(3);
    case 0: // child process code
        // close write end,otherwise child will never terminate
        close(fd[WRITE_FD]);

        while(1){
            nbytes = read(fd[READ_FD], message, BUFSIZE); 
            message[nbytes] = '\0';
            if (nbytes == 0) break;
            else if (nbytes == -1) {
                perror("read");
                exit(4);
            }
            printf("Child received the word: '%s'\n", message);  
            fflush(stdout); 
        }
   
        close(fd[READ_FD]);
        exit(0);
    default: // parent process code
        close(fd[READ_FD]); // close read end
        for(i=1; i<argc; ++i)
            // send each word separetly
            if (write(fd[WRITE_FD], argv[i], strlen(argv[i])) != -1) {
                printf("Parent sent the word: '%s'\n", argv[i]);
                fflush(stdout);
                sleep(1);  // need to wait until child could read input
            }
            else {
                perror("write");
                exit(5);
            }
        close(fd[WRITE_FD]); // reader will see EOF

        // wait for child so it does not remain a zombie
        // we do not care about it's status
        if (wait(NULL) == -1){
            perror("wait");
            exit(2);
        }
    }

    exit(0);
}

