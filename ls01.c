/*
 * =====================================================================================
 *
 *       Filename:  ls01.c
 *
 *    Description:  ls tool (list directories)
 *
 *        Version:  1.0
 *        Created:  02/25/2013 05:41:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky (artyom.diky@hunter.cuny.edu) 
 *        Company:  Hunter College
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#define HERE "."

void ls(char dirname[]);

int main ( int argc, char *argv[] )
{
    int i = 1;
    if (argc == 1)
        ls(HERE);
    else
        while (i < argc)
        {
            printf("%s:\n", argv[i]);
            ls(argv[i]);
            i++;
        }

    return EXIT_SUCCESS;
} 

// List content of a direcory
void ls(char dirname[])
{
    DIR	    		*dir_ptr;
    struct dirent	*direntp;

    if ( (dir_ptr = opendir(dirname) ) == NULL)
        fprintf(stderr, "Cannot open %s\n", dirname);
    else
    {
        while ( (direntp = readdir(dir_ptr) ) != NULL)
            printf("%s\n", direntp->d_name);
        closedir(dir_ptr);
    }
}

