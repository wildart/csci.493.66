/*
 * =====================================================================================
 *
 *       Filename:  more03.c
 *
 *    Description:  more tool
 *
 *        Version:  3.0
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
#include <sys/ioctl.h>
#include <unistd.h>
#define LINELEN     512
#define SPACE       1
#define RETURN      2
#define QUIT        3
#define INVALID     4

void do_some_of(FILE *fp);
int get_user_input();
void get_tty_size(FILE *tty, int* numrows, int* numcols);

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
    int tty_rows, tty_cols;

    fp_tty = fopen("/dev/tty", "r");
    if (fp_tty == NULL)
        exit(EXIT_FAILURE);
    
    // Get size of terminal window
    get_tty_size(fp_tty, &tty_rows, &tty_cols);
    
    while (fgets(line, LINELEN, fp) && getmore)
    {
        if (line_num == tty_rows-1)
        {
            reply = get_user_input(fp_tty);
            switch(reply)
            {
                case SPACE:
                    line_num = 0;
                    printf("\033[1A\033[2K\033[1G");
                    break;
                case RETURN:
                    line_num--;
                    printf("\033[1A\033[2K\033[1G");
                    break;
                case QUIT:
                    getmore = 0;
                    printf("\033[1A\033[2K\033[1B\033[7D");
                    exit(EXIT_SUCCESS);
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
    int tty_rows;
    int tty_cols;

    // Get sie of the terminal window dynamically
    get_tty_size(fp, &tty_rows, &tty_cols);
    printf("\033[%d;1H",tty_rows);

    printf("\033[7m--More--\033[m");
    while((c = fgetc(fp)) != EOF)
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

/*
 * Get size of terminal window
 */ 
void get_tty_size(FILE *tty, int* numrows, int* numcols)
{
    /* 
    int num_lines;
    char *endptr;
    char *linestr = getenv("LINES");
    if (linestr != NULL)
    {
        num_lines = strtol(linestr, &endptr, 0);
        if (errno != 0) // add #include <errno.h>
        {
            // handle error and exit
        }
        if (endptr == linestr)
        {
            // not a number, so handle error and exit
        }
    }
    */

    struct winsize windows_arg;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &windows_arg) == -1)
        exit(EXIT_FAILURE);
    *numrows = windows_arg.ws_row;
    *numcols = windows_arg.ws_col;
}
