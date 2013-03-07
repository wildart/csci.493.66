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
#include <time.h>

void show_info( struct utmp *utbufp );
void show_time( const long time );

int main ( int argc, char *argv[] )
{
    int fd;
    struct utmp current_record;
    int reclen = sizeof(struct utmp);

    fd = open(UTMP_FILE, O_RDONLY);
    if (fd == -1)
    {
        perror(UTMP_FILE);
        exit(EXIT_FAILURE);
    }

    while (read(fd, &current_record, reclen) == reclen)
        show_info(&current_record);

    close(fd);

    return EXIT_SUCCESS;
}

void show_info( struct utmp *utbufp )
{
    if (utbufp->ut_type != USER_PROCESS)
        return;

    printf("%-8.8s", utbufp->ut_name); // login name
    printf(" ");
    printf("%-8.8s", utbufp->ut_line); // tty 
    printf(" ");
    show_time(utbufp->ut_time);
    printf(" ");
    if (utbufp->ut_host[0] != '\0')
        printf("(%s)", utbufp->ut_host);   // host name
    printf("\n");
}

void show_time(const long time)
{
    char* timestr = ctime(&time);
    printf("%12.12s", timestr+4);
}
