/*
 * =====================================================================================
 *
 *       Filename:  mypid.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/28/2013 07:45:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <unistd.h>
#include	<stdlib.h>

int main ( int argc, char *argv[] )
{
    printf("My PID: %d\n", getpid());
    printf("PATH: %s\n", getenv("PATH"));
    getchar();
    return EXIT_SUCCESS;
}
