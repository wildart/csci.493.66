/*
 * =========================================================================
 *
 *       Filename:  pipedemo0.c
 *
 *    Description:  Pipe usage demo
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

#define READ_END  0
#define WRITE_END 1
#define NUM       5
#define BUFSIZE  32

int main(int argc, char *argv[])
{
    int i, nbytes;
    int fd[2];
    char message[BUFSIZE+1];

    if (pipe(fd) == -1){
        perror("pipe call");
        exit(2);
    }

    for(i=1; i<=NUM; ++i){
        sprintf(message, "hello #%2d\n", i);
        write(fd[WRITE_END], message, strlen(message));
    }
    close(fd[WRITE_END]);

    printf("%d messages sent;  sleeping a bit. Please wait...\n", NUM);
    sleep(3);

    while( (nbytes = read(fd[READ_END], message, BUFSIZE)) != 0){
        if(nbytes > 0){
            message[nbytes] = '\0';
            printf("%s", message);
        }
    }
    fflush(stdout);
    exit(0);
}

