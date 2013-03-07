/*
 * =====================================================================================
 *
 *       Filename:  ttyname.c
 *
 *    Description:  Get TTY name
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
#include <unistd.h>

int main ( int argc, char *argv[] )
{
    if (isatty(STDIN_FILENO))   
        printf("Terminal is %s\n", ttyname(STDIN_FILENO));
    else
        printf("not a terminal\n");
    return 0;
}

