/*
 * =========================================================================
 *
 *       Filename:  mylogin.c
 *
 *    Description:  This util hides user input as 'login' tool
 *
 *        Version:  1.0
 *        Created:  03/08/2013 03:49:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Wild
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

int main(int argc, char **argv)
{
    struct termios info, orig;
    char username[33], passwd[33];
    FILE *fp;

    // get FILE* to the control terminal -- do not assume stdin
    if ((fp = fopen(ctermid(NULL), "r+")) == NULL)
        return EXIT_FAILURE;
    
    printf("login: ");
    fgets(username, 32, stdin);
    printf("password: ");

    // get terminal parameters and switch off ECHO mode
    tcgetattr(fileno(fp), &info);
    orig = info;
    info.c_lflag &= ~ECHO;
    tcsetattr(fileno(fp), TCSANOW, &info);

    // read user input and restore settings
    fgets(passwd, 32, stdin);
    tcsetattr(fileno(fp), TCSAFLUSH, &orig);

    printf("\n");
    printf("Last login: Tue Mar 11........ ┏(-_-)┛┗(-_-)┓┗(-_-)┛┏(-_-)┓ \n");
    return EXIT_SUCCESS;
}
