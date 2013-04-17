/*
 * =========================================================================
 *
 *       Filename:  execdemo1.c
 *
 *    Description:  Using exec()
 *
 *        Version:  1.0
 *        Created:  04/17/2013 07:16:49 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky 
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[])
{
    if (argc < 2){
        printf("usage: %s arg1 [arg2 ...]\n",argv[0]);
        exit(1);
    }
    execve("/bin/echo", argv, envp);
    printf("execve() failed to run.\n");
    return 0;
}
