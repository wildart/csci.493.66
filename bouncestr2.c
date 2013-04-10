/*
 * =========================================================================
 *
 *       Filename:  bouncestr2.c
 *
 *    Description:  Proper async animation demo
 *
 *        Version:  1.0
 *        Created:  04/08/2013 06:45:04 PM
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
#include <curses.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>

#define INITIAL_SPEED 50
#define RIGHT 1
#define LEFT -1
#define ROW 12
#define MESSAGE "ooooooo=>"
#define REVMSGE "<=ooooooo"
#define BLANK   "         "

int dir;   // Direction of movement
int speed; // Chars per second
int row;   // Current row
int col;   // Current column
volatile sig_atomic_t input_ready;

void on_alarm(int);          // handler for alarm
void on_input(int);          // handler for SIGIO
void enable_keydb_signals(); // setup SIGIO
int update_from_input(int, int*, int*);

int set_timer(int which, long initial, long repeat)
{
    struct itimerval itimer;
    long secs;
                
    // Init delay
    secs = initial / 1000;
    itimer.it_value.tv_sec  = secs;
    itimer.it_value.tv_usec = (initial - secs*1000) * 1000;
        
    // Init repeat interval
    secs = repeat / 1000;
    itimer.it_interval.tv_sec  = secs;
    itimer.it_interval.tv_usec = (repeat - secs*1000) * 1000;
        
    return setitimer(which, &itimer, NULL);
}

int main()
{
    int done;
    int is_changed;
    int c, finished;

    // Setup signal handling
    struct sigaction newhandler;
    sigset_t         blocked;

    newhandler.sa_handler = on_input;   // name of handler
    newhandler.sa_flags   = SA_RESTART; // restart handler on execution
    sigemptyset(&blocked);              // create empty set of blocked sirnals
    newhandler.sa_mask = blocked;       // setup this set to be mask
    if( sigaction(SIGIO, &newhandler, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    sigemptyset(&blocked);             // setu mask with SIGIO
    sigaddset(&blocked, SIGIO);
    newhandler.sa_handler = on_alarm;  // set handler for SIGALRM
    newhandler.sa_mask = blocked;
    if ( sigaction(SIGALRM, &newhandler, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    // Prepare terminal
    initscr();
    cbreak();
    noecho();
    clear();
    curs_set(0); // hide cursor

    // Initialize parameters
    row = ROW;
    col = 0;
    dir = RIGHT;
    done = 0;
    speed = INITIAL_SPEED; 
    finished = 0;
    input_ready = 0;

    // Turn on keyboard signals
    enable_keydb_signals();

    // Start the real timer
    set_timer(ITIMER_REAL, 1000/ speed, 1000/speed);

    mvaddstr(LINES-1, 0, "Current speed:");
    refresh();

    /* Put the message into the first position and start */
    mvaddstr(row, col, MESSAGE);

    while(!finished)
    {
        if(input_ready)
        {
            c =getch();
            finished = update_from_input(c, &speed, &dir);
            input_ready = 0;
        }
        else
            pause();
    }

    // Clean up
    endwin();
    return 0;
}

/* Setup for SIGIO */
void enable_keydb_signals()
{
    int fd_flags;

    fcntl(0, F_SETOWN, getpid());
    fd_flags = fcntl(0, F_GETFL);
    fcntl(0, F_SETFL, (fd_flags | O_ASYNC));
}

void on_alarm(int signum)
{
    static int row = ROW;
    static int col = 0;
    char message[40];
    move(row,col);
    addstr(BLANK);  // erase old string
    col += dir;
    move(row, col);
    if (dir == RIGHT)
    {
        addstr(MESSAGE);
        if (col+strlen(MESSAGE) >= COLS+1)
            dir = LEFT;
    }
    else 
    {
        addstr(REVMSGE);
        if (col <= 0)
            dir = RIGHT;
    }
    
    move (LINES -1, 0);
    sprintf(message, "Current speed: %d (chr/s)", speed);
    addstr(message);
    refresh();
}

void on_input(signum)
{
    input_ready = 1;
}

int update_from_input(int c, int *speed, int *dir)
{
    int is_changed = FALSE;
    char mssg[40];

    switch(c)
    {
        case 'Q':
        case 'q':
            return 1;

        case ' ':
            *dir = ((*dir) == LEFT) ? RIGHT: LEFT;
            break;
        
        case 'f':
            if (1000 / *speed > 2 )
            {
                *speed = *speed + 2;
                is_changed = TRUE;
            }
            break;
        case 's':
            if (1000 / *speed <= 500)
            {
                *speed = *speed - 2;
                is_changed = TRUE;
            }
            break;
    }
    if (is_changed)
    {
        set_timer(ITIMER_REAL, 1000 / *speed, 1000 / *speed);
        sprintf(mssg, "Current speed: %d (c/s)", *speed);
        mvaddstr(LINES-1, 0, mssg);
        move(LINES-1, COLS-12);
        sprintf(mssg, "Last char: %c", c);
        addstr(mssg);
        refresh();
    }

    return 0;
}
