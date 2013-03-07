/*
 * =====================================================================================
 *
 *       Filename:  get_char.c
 *
 *    Description:  Get character from console.
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
    int input_char;

    printf("Type any char followed by Enter key");
    printf(" Press Ctrl-D to exit\n");
    while( (input_char = getchar()) != EOF)
        printf("char = '%c', code = %03d\n", input_char, input_char);

    return EXIT_SUCCESS;
}
