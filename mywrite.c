/*
 * =====================================================================================
 *
 *       Filename:  mywrite.c
 *
 *    Description:  Write to device file
 *
 *        Version:  1.0
 *        Created:  03/04/2013 05:18:20 PM
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
#include <unistd.h>
#include <utmp.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

void create_message(char *);
char *get_user_tty(char *);

int main ( int argc, char *argv[] )
{
    int fd;
    char buf[BUFSIZ];
    char *user_tty;
    char eof[] = "EOF\n";

    // Process command line arguments
    if (argc < 2)
    {
        fprintf(stderr, "usage: mywrite username\n");
        exit(1);
    }
    
    // Find user's terminal name
    if ( (user_tty = get_user_tty( argv[1] ) ) == NULL )
    {
        fprintf(stderr, "User %s is not logged in.\n", argv[1]);
        exit(1);
    }

    // Create device path and open it
    sprintf(buf, "/dev/%s", user_tty);
    fd = open( buf, O_WRONLY );
    if (fd == -1)
    {
        perror(buf);
        exit(1);
    }
    
    // Send invitation        
    create_message(buf);
    if ( write(fd, buf, strlen(buf)) == -1)  
    {
        perror("write");
        close(fd);
        exit(1);
    }

    // Send text from stdin
    while( fgets(buf, BUFSIZ, stdin) != NULL)
    {
        if ( write(fd, buf, strlen(buf)) == -1)
            break;
    }
    // Send EOF
    write(fd, eof, strlen(eof));
    close(fd);
    
    return 0;
}

void create_message(char buf[])
{
    char *sender_tty, *sender_name;
    char sender_host[256];
    time_t now;
    struct tm *timeval;

    sender_name = getlogin();
    sender_tty = ttyname( STDIN_FILENO );
    gethostname ( sender_host , 256 ) ;
    time ( &now ) ;
    timeval = localtime( &now );
    sprintf( buf , "Message from %s@%s on %s at %2d:%02d:%02d ...\n" ,
            sender_name, sender_host, 5+sender_tty,
            timeval->tm_hour, timeval->tm_min, timeval->tm_sec);
}

char *get_user_tty(char *longname)
{
    static struct utmp utmp_rec;
    int utmp_rec_size = sizeof(utmp_rec);
    int utmp_fd;
    int namelen = sizeof( utmp_rec.ut_name );
    char *retval = NULL;

    if ( (utmp_fd = open ( UTMP_FILE, O_RDONLY)) == -1 )
        return NULL;

    while (read (utmp_fd, &utmp_rec, utmp_rec_size) == utmp_rec_size)
        if (strncmp(longname, utmp_rec.ut_name, namelen) == 0)
        {
            retval = utmp_rec.ut_line;
            break;
        }

    close(utmp_fd);
    return retval;
}


