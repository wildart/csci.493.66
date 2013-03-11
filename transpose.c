/*
 * =========================================================================
 *
 *       Filename:  transpose.c
 *
 *    Description:  Input transpose to lower.
 *
 *        Version:  1.0
 *        Created:  03/04/2013 04:57:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main ( int argc, char *argv[] )
{
    int c;

    printf("Press Ctrl-D to exit\n");
    while( (c = fgetc(stdin)) != EOF)
    {
        if(islower(c))
            c = 'A' + c - 'a';
        fputc(c, stdout);
    }

    return EXIT_SUCCESS;
}
