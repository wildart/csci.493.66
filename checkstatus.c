/*
 * =====================================================================================
 *
 *       Filename:  checkstatus.c
 *
 *    Description:  Check file status
 *
 *        Version:  1.0
 *        Created:  03/04/2013 11:46:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art 
 *        Company:  Hunter College
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int check_file_status(int fd)
{
    int flags = fcntl (fd, F_GETFL);
    if (flags == -1)
    {
        perror ("Could not get flags.");
        return -1;
    }

    switch( flags & O_ACCMODE)
    {
    case O_WRONLY:
        printf("write-only\n");
        break;
    case O_RDONLY:
        printf("read-only\n");
        break;
    case O_RDWR:
        printf("read-write\n");
        break;
    }

    if ( flags & O_CREAT )		printf("O_CREAT flag is set\n");
    if ( flags & O_EXCL )		printf("O_EXCL flag is set\n");
    if ( flags & O_NOCTTY )		printf("O_NOCTTY flag is set\n");
    if ( flags & O_TRUNC )		printf("O_TRUNC flag is set\n");
    if ( flags & O_APPEND )		printf("O_APPEND flag is set\n");
    if ( flags & O_NONBLOCK )	printf("O_CREAT flag is set\n");
    if ( flags & O_NDELAY )		printf("O_NDELAY flag is set\n");
    if (flags & O_CREAT )	printf("O_CREAT flag is set\n");
#ifdef O_SYNC    
    if ( flags & O_SYNC )		printf("O_SYNC flag is set\n");
#endif    
#ifdef O_FSYNC    
    if ( flags & O_FSYNC )		printf("O_FSYNC flag is set\n");
#endif    
    if ( flags & O_ASYNC )		printf("O_ASYNC flag is set\n");
    printf("\n");
    return 0;
}

int main ( int argc, char *argv[] )
{
    int fd;
    if (argc != 2) 
    {
        printf("usage: %s <descriptor#>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if ( (fd = atoi(argv[1])) < 0)
    {
        printf("usage: %s <descriptor#>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    check_file_status(fd);
    return EXIT_SUCCESS;
}
