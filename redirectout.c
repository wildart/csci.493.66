/*
 * =========================================================================
 *
 *       Filename:  redirectout.c
 *
 *    Description:  Output redirection
 *
 *        Version:  1.0
 *        Created:  04/24/2013 02:31:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int fd;

    if(argc < 3){
        fprintf(stderr, "Usage %s command output-file\n", argv[0]);
        exit(1);
    }

    switch(fork()){
    case -1:
        perror("fork");
        exit(1);

    case 0:  // child code
        // close stdout
        close(1);
        
        // open the file into which to redirec stdout
        if ( (fd = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0644) == -1) )
            exit(1);

        // execute command
        execlp(argv[1], argv[1], NULL);

        // should not reach here!
        perror("execlp");
        exit(1);
    
    default: // parent code
        wait(NULL);
    }
    return 0;
}



