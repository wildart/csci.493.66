/*
 * =============================================================================
 *
 *       Filename:  cp01.c
 *
 *    Description:  copy util
 *
 *        Version:  1.0
 *        Created:  02/11/2013 03:12:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art 
 *
 * =============================================================================
 */
#include    <stdio.h>
#include	<stdlib.h>
#include    <fcntl.h>
#include    <unistd.h>

#define BUFFER_SIZE     4096
#define COPYMODE        0644

void die(char* str1, char* str2); // print error and quit

int main ( int argc, char *argv[] )
{
    int source_fd, target_fd, n_chars;
    char buffer[BUFFER_SIZE];

    if (argc != 3)
    {
        fprintf(stderr, "usage: %s source destination\n", *argv);
        exit(EXIT_FAILURE);
    }

    // try open files
    if ( (source_fd = open(argv[1], O_RDONLY)) == -1)
        die("Cannot open ", argv[1]);
  
    if ( (target_fd = creat(argv[2], COPYMODE)) == -1)
        die("Cannot open ", argv[2]);
                
    while( (n_chars = read(source_fd, buffer, BUFFER_SIZE) ) > 0 )
    {
        if (n_chars != write(target_fd, buffer, n_chars))
            die("Write error to ", argv[2]);
    }

    if (n_chars == -1)
        die("Read error from ", argv[1]);

    if ( close(source_fd) == -1 || close(target_fd) == -1)
        die("Error closing files", "");

    return EXIT_SUCCESS;
} 
/* ----------  end of function main  ---------- */

void die(char* str1, char* str2)
{
    fprintf(stderr, "Error: %s ", str1);
    perror(str2);
    exit(EXIT_FAILURE);
}
