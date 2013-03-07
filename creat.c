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

int main ( int argc, char *argv[] )
{
    int fd;
    size_t num_bytes = 0;
    char *buffer = NULL;

    fd = creat("prototype", 0751);
   
    if (write(fd, buffer, num_bytes) != num_bytes)
    {
        fprintf(stderr, "Problem writing fo file.\n");
    }

    close(fd);

    return EXIT_SUCCESS;
} 
/* ----------  end of function main  ---------- */
