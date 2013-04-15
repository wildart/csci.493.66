/*
 * =========================================================================
 *
 *       Filename:  displayvm.c
 *
 *    Description:  Display process VM
 *
 *        Version:  1.0
 *        Created:  04/15/2013 04:31:43 PM
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
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define SHW_ADR(ID,I) \
        printf("       %s \t is at addr:%8X\t%20u\n", ID, (unsigned int)(&I), (unsigned int)(&I))

extern int etext, edata, end;

void showit(char*);

char *cptr = "Hello World\n"; // initialized global
char buffer1[10];             // uninitialized global

int main(int argc, char *argv[], char *envp[])
{
    int i = 0;        // on stack
    static int diff;  // global in uninitialized data segment

    strcpy(buffer1, "     Layout of virtual memory\n");
    write(1,buffer1, strlen(buffer1)+1);

    printf("Addr etext:  %8X  \t Addr edata:  %8X  \t Addr end:  %8X \n\n",
            (unsigned int)&etext, (unsigned int)&edata, (unsigned int)&end);

    printf("         ID \t            HEX_ADDR\t        DECIMAL_ADDR\n");
    SHW_ADR("main", main);
    SHW_ADR("showit", showit);
    SHW_ADR("etext", etext);
    diff = (int)&showit - (int)&main;
    printf("         showit is %d bytes above main\n", diff);
    SHW_ADR("cptr", cptr);
    diff = (int)&cptr - (int)&showit;
    printf("           cptr is %d bytes above main\n", diff);
    SHW_ADR("buffer1", buffer1);
    SHW_ADR("diff", diff);
    SHW_ADR("edata", edata);
    SHW_ADR("end", end);
    SHW_ADR("argc", argc);
    SHW_ADR("argv", argv);
    SHW_ADR("envp", envp);
    SHW_ADR("i", i);
    showit(cptr);
    return 0;
}

void showit(char *p)
{
    char *buffer2;
    SHW_ADR("buffer2", buffer2);
    if ( (buffer2 = (char*) malloc((unsigned)(strlen(p)+1))) != NULL)
    {
        strcpy(buffer2, p);
        printf("%s", buffer2);
        free(buffer2);
    }
    else
    {
        printf("Allocation error.\n");
        exit(1);
    }
}
