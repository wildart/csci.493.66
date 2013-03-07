/*
 * =====================================================================================
 *
 *       Filename:  who05.c
 *
 *    Description:  who command (buffered)
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
#include <stdlib.h>
#include <utmp_utils.h>
#include <time.h>

void show_info( utmp_record *utbufp );
void show_time( const long time );

int main ( int argc, char *argv[] )
{
    utmp_record *utbufp;

    if (open_utmp(UTMP_FILE) == -1)
    {
        perror(UTMP_FILE);
        exit(EXIT_FAILURE);
    }
    
    while( (utbufp = next_utmp()) != NULL_UTMP_RECORD_PTR)
        show_info(utbufp);
    
    close_utmp();

    return EXIT_SUCCESS;
}

void show_info( utmp_record *utbufp )
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
