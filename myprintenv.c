/*
 * =====================================================================================
 *
 *       Filename:  myprintenv.c
 *
 *    Description:  Implementation of 'printenv' tool
 *
 *        Version:  1.0
 *        Created:  01/28/2013 05:12:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Wild (wildart@gmail.com)
 *        Company:  GC CUNY
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

extern char **environ;

int main ( int argc, char *argv[] )
{
    char **env;
    for(env = environ; *env != NULL; ++env)
        printf("%s\n", *env);
    return EXIT_SUCCESS;
}

