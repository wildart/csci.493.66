/*
 * =====================================================================================
 *
 *       Filename:  who01.c
 *
 *    Description:  Who Command
 *
 *        Version:  1.0
 *        Created:  02/05/2013 05:18:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Wild 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <utmp.h>

void show_info( struct utmp *utmpbufp );

int main ( int argc, char *argv[] )
{
    int fd;
    struct utmp current_record;
    int reclen = sizeof(struct utmp);

    fd = open(UTMP_FILE, O_RDONLY);
    //fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror(UTMP_FILE);
        //perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    while (read(fd, &current_record, reclen) == reclen)
        show_info(&current_record);

    close(fd);

    return EXIT_SUCCESS;
}

void show_info( struct utmp *utbufp )
{
    printf("%-8.8s ", utbufp->ut_name); // login name
    printf("%-8.8s ", utbufp->ut_line); // tty 
    printf("%10ds ", utbufp->ut_time); // login time ut_tv.tv_sec 
    printf("(%s)\n", utbufp->ut_host);   // host name
}
