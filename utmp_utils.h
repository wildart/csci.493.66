/*
 * =====================================================================================
 *
 *       Filename:  utmp_utils.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/11/2013 04:41:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <utmp.h>

typedef struct utmp utmp_record;


#define NUM_RECORDS             20
#define NULL_UTMP_RECORD_PTR    ((utmp_record *) NULL)
#define SIZE_OF_UTMP_RECORD     (sizeof(utmp_record))
#define BUF_SIZE                (NUM_RECORDS * SIZE_OF_UTMP_RECORD)


int open_utmp(char * utmp_file);
// opens the given utmp_file for buffered reading
// returns: a valid file descriptor on success
//          -1 on error

utmp_record* next_utmp();
// returns: a pointer to next utmp record, NULL if no more records

void close_utmp();
// closes the utmp_file and frees the file descriptor

void die(char* str1, char* str2);
