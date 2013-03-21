/*
 * =========================================================================
 *
 *       Filename:  term_demo.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/13/2013 07:45:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =========================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>

#define RETRIEVE  1
#define RESTORE   2
#define PROMPT    "Do you want to continue"
#define MAX_TRIES 3
#define SLEEPTIME 2
#define BEEP      putchar('\a')
#define TRUE      1
#define FALSE     0

typedef struct __tty_opts tty_opts;
struct __tty_opts
{
    int canon;
    int echo;
    int min;
    int time;
};

typedef struct __ui_params ui_params;
struct __ui_params
{
    int sleeptime;
    int isblocking;
    int maxtries;
};

void save_restore_tty(int fd, int action, struct termios *copy);
void modify_termios(struct termios *cur_tty, tty_opts ts);
void apply_termios_setting(int fd, struct termios cur_tty);
void set_non_block(int fd);
int  get_response(FILE *fp, ui_params uip);
void get_options(int argc, char *argv[], tty_opts *ttyopts, ui_params *uip);
void handle_interupt(int signum);
char* signame(int signum);

int main(int argc, char *argv[])
{
    int response;
    tty_opts ttyopts = {TRUE,FALSE,1,1};
    ui_params ui_parameters = {SLEEPTIME, FALSE, MAX_TRIES};
    struct termios ttyinfo;
    FILE *fp;
   
    get_options(argc, argv, &ttyopts, &ui_parameters);

    if (!isatty(0) || !isatty(1))
        exit(EXIT_FAILURE);
    fp = stdin;
    
    signal(SIGINT , handle_interupt);
    signal(SIGQUIT, handle_interupt);

    save_restore_tty(fileno(fp), RETRIEVE, &ttyinfo);
    modify_termios(&ttyinfo, ttyopts);
    apply_termios_setting(fileno(fp), ttyinfo);
    if(!ui_parameters.isblocking)
    {
        set_non_block(fileno(fp));
    }
    while( TRUE )
    {
        response = get_response(fp, ui_parameters);
        if (response)
        {
            printf("\nMain: Response from user = %c\n", response);
            if (response == 'n')
                break;
        }
        else
            printf("Main: No response from user\n");
    }
    save_restore_tty(fileno(fp), RESTORE, NULL);
    tcflush(0, TCIFLUSH);
    return EXIT_SUCCESS;
}

void save_restore_tty(int fd, int action, struct termios *copy)
{
    static struct termios original_state;
    static int            original_flags = -1;
    static int            retrieved = FALSE;

    if (action == RETRIEVE)
    {
        retrieved = TRUE;
        tcgetattr(fd, &original_state);
        original_flags = fcntl(fd, F_GETFL);
        if(copy != NULL)
            *copy = original_state;
    }
    else if (action == RESTORE && retrieved)
    {
        tcsetattr(fd, TCSADRAIN, &original_state);
        fcntl(fd, F_SETFL, original_flags);
    }
    else
        fprintf(stderr, "Bad action in save_restore_tty()\n");
}

void modify_termios(struct termios *cur_tty, tty_opts ts)
{
    if (ts.canon)
        cur_tty->c_lflag |= ICANON;
    else
        cur_tty->c_lflag &= ~ICANON;

    if (ts.echo)
        cur_tty->c_lflag |= ECHO;
    else
        cur_tty->c_lflag &= ~ECHO;

    cur_tty->c_cc[VMIN]  = ts.min;
    cur_tty->c_cc[VTIME] = ts.time;
}

void apply_termios_setting(int fd, struct termios cur_tty)
{
    tcsetattr(fd, TCSANOW, &cur_tty);
}

void set_non_block(int fd)
{
    int flagset;
    flagset = fcntl(fd, F_GETFL);
    flagset |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flagset);
    printf("no block set\n");
}

int get_response(FILE *fp, ui_params uip)
{
    int n;
    unsigned char c, input;
    time_t time0, time_now;

    input = ' ';
    time(&time0);
    while(TRUE)
    {
        printf("%s (y/n)?", PROMPT);
        // stdout is line buffered so we need to flush prompt
        // otherwise it will not appear until read is done
        fflush(stdout); 
        if (!uip.isblocking)
            sleep(uip.sleeptime);
        if ( (n = read(fileno(fp), &c, 1)) > 0)
        {
            tcflush(fileno(fp), TCIFLUSH);
            input = tolower(c);
        
            if (input == 'y' || input == 'n' )
                return input;
            else
            {
                printf("\nInvalid input: %c\n", input);
                continue;
            }
        }
    
        time(&time_now);
        printf("\nTimeout waiting for input: %d secs elapsed, %d timeouts left\n", 
           (int)(time_now-time0), uip.maxtries);
        if (uip.maxtries-- == 0)
        {
            printf("\nTime is up.\n");
            return 0;
        }
    }
}

void get_options(int argc, char *argv[], tty_opts *ttyopts, ui_params *uip)
{
    char ch;
    char options[] = "t:s:b";
    opterr = 0; // turn off error message by getopt
    while(TRUE)
    {
        ch = getopt(argc, argv, options);
        if ( ch == -1 )
            break;
        switch( ch ){
        case 'b':
           uip->isblocking = TRUE;
           break;
        case 't':
           ttyopts->time = atoi(optarg);
           break;
        case 's':
           uip->sleeptime = atoi(optarg);
           break;
        default:
           fprintf(stderr, "Usage: %s [-b] [-t timeout] [-s sleep_time]\n", argv[0]);
           exit(EXIT_FAILURE);
        }
    }
}

void handle_interupt(int signum)
{
    printf("\nExiting with signal %s\n", signame(signum));
    save_restore_tty(fileno(stdin), RESTORE, NULL);
    exit(2);
}

char* signame(int signum)
{
    static char name[16];
    switch(signum)
    {
    case SIGINT:
        strcpy(name, "SIGINT");
        break;
    case SIGQUIT:
        strcpy(name, "SIGQUIT");
        break;
    }
    return name;
}
