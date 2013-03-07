/*
 * =====================================================================================
 *
 *       Filename:  utmp_utils.c
 *
 *    Description:  utmp utils
 *
 *        Version:  1.0
 *        Created:  02/11/2013 04:44:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <utmp_utils.h>

static char utmpbf[BUF_SIZE];
static int  number_of_recs_in_buff;
static int  current_record;
static int  fd_utmp = -1;

void die(char* str1, char* str2)
{
    fprintf(stderr, "Error: %s ", str1);
    perror(str2);
    exit(EXIT_FAILURE);
}

int open_utmp(char * utmp_file)
{
    fd_utmp = open( utmp_file, O_RDONLY);
    current_record = 0;
    number_of_recs_in_buff = 0;
    return fd_utmp;
}

int fill_utmp()
{
    int bytes_read;

    //read NUM_RECORDS records from the utmp file into buffer
    bytes_read = read(fd_utmp, utmpbf, BUF_SIZE);
    if (bytes_read < 0)
        die("Failed to read from utmp file", "");
    
    // convert byte count into records
    number_of_recs_in_buff = bytes_read / SIZE_OF_UTMP_RECORD;

    // reset current record to start
    current_record = 0;
    return number_of_recs_in_buff;
}

utmp_record* next_utmp()
{
    utmp_record *recordp;
    int          byte_position;

    // file not opened
    if (fd_utmp == -1)
        return NULL_UTMP_RECORD_PTR;

    // no unread record in the buffer, need to refill
    if (current_record == number_of_recs_in_buff)
        if (fill_utmp() == 0)
            // no recors left
            return NULL_UTMP_RECORD_PTR;

    byte_position = current_record * SIZE_OF_UTMP_RECORD;
    recordp = (utmp_record *) &utmpbf[byte_position];

    // advance record pointer
    current_record++;
    return recordp;
}

void close_utmp()
{
    if (fd_utmp != -1)
        close(fd_utmp);
}
