/*
 * =========================================================================
 *
 *       Filename:  shpipi3.c
 *
 *    Description:  Shell pipe simulation
 *
 *        Version:  1.0
 *        Created:  04/24/2013 02:31:33 PM
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
#include <unistd.h>
#include <limits.h>

int main(int argc, char *argv[])
{
    int nbytes;
    FILE *fin;
    FILE *fout;
    char buffer[PIPE_BUF];

    if(argc < 3){
        fprintf(stderr, "Usage %s command1 command2\n", argv[0]);
        exit(1);
    }

    if( (fin = popen(argv[1],"r")) == NULL){
        fprintf(stderr, "popen() failed\n");
        exit(1);
    }
    
    if( (fout = popen(argv[2],"w")) == NULL){
        fprintf(stderr, "popen() failed\n");
        exit(1);
    }
    
    while( (nbytes = read(fileno(fin), buffer, PIPE_BUF)) > 0)
        write(fileno(fout), buffer, nbytes);

    pclose(fin);
    pclose(fout);
    return 0;
}



