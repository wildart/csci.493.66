/*
 * =====================================================================================
 *
 *       Filename:  updateflag.c
 *
 *    Description:  Update file status flags
 *
 *        Version:  1.0
 *        Created:  03/04/2013 04:57:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main ( int argc, char *argv[] )
{
    int flags, fd, result;

    fd = open( argv[1], O_WRONLY );
    if (fd == -1)
    {
        perror("Cannot create file.");
        return EXIT_FAILURE;
    }

    flags = fcntl(fd, F_GETFL);
    flags |= (O_APPEND);
    result = fcntl(fd, F_SETFL, flags);
    if (result == -1)
    {
        perror("Error setting O_APPEND");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
