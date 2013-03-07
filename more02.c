/*
 * =====================================================================================
 *
 *       Filename:  more02.c
 *
 *    Description:  more tool
 *
 *        Version:  2.0
 *        Created:  01/30/2013 07:48:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Wild
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#define SCREEN_ROWS 23
#define LINELEN     512
#define SPACE       1
#define RETURN      2
#define QUIT        3
#define INVALID     4

void do_some_of(FILE *fp);
int get_user_input();

int main(int argc, char** argv)
{
    FILE *fp;
    int i = 0;
    if(argc == 1)
    {
        do_some_of( stdin );
    }
    else 
    {
        while(++i<argc)
        {
            fp = fopen(argv[i], "r");
            if(fp != NULL){
                do_some_of(fp);
                fclose(fp);
            }
            else
                printf("Skipping %s\n", argv[i]);
        }
    }
    exit(EXIT_SUCCESS);
}

/*
 * Given a FILE*, display up to a page of the 
 * file, and the display a prompt and wait for user input.
 * If user inputs SPACE, display next page.
 * If user inputs RETURN, display next line.
 * If user inputs QUIT, terminate program.
 */
void do_some_of(FILE *fp)
{
    char line[LINELEN];
    int getmore = 1;
    int line_num = 0;
    int reply;
    FILE *fp_tty;

    fp_tty = fopen("/dev/tty", "r");
    if (fp_tty == NULL)
        exit(EXIT_FAILURE);
   
    while (fgets(line, LINELEN, fp) && getmore)
    {
        if (line_num == SCREEN_ROWS)
        {
            reply = get_user_input(fp_tty);
            switch(reply)
            {
                case SPACE:
                    line_num = 0;
                    break;
                case RETURN:
                    line_num--;
                    break;
                case QUIT:
                    getmore = 0;
                    break;
                default: // invalid input
                    break;
            }
        }
        if(fputs(line, stdout) == EOF)
            exit(EXIT_FAILURE);
        line_num++;
    }
}

/*
 * Display prompt, wait for response and return user input
 * Returns: SPACE, RETURN, QUIT, INVALID
 */
int get_user_input(FILE *fp)
{
    int c;
    printf("\033[7m--More--\033[m");
    while((c = getc(fp)) != EOF)
    {
        switch(c)
        {
            case 'q':
                return QUIT;
            case ' ':
                return SPACE;
            case '\n':
                return RETURN;
            default:
                return INVALID;
        }
    }
    return INVALID;
}

