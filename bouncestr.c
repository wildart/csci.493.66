/*
 * =========================================================================
 *
 *       Filename:  bouncestr.c
 *
 *    Description:  Demo animation
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

#define INITIAL_SPEED 50
#define RIGHT 1
#define LEFT -1
#define ROW 12
#define MESSAGE "ooooooo=>"
#define REVMSGE "<=ooooooo"
#define BLANK   "         "

int dir;   // Direction of movement
int speed; // Chars per second

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

void move_msg(int signum)
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

int main()
{
    int done;
    int is_changed;
    int c;

    // Setup signal handling
    struct sigaction newhandler;
    sigset_t         blocked;

    newhandler.sa_handler = move_msg;   // name of handler
    newhandler.sa_flags   = SA_RESTART; // restart handler on execution
    sigemptyset(&blocked);
    newhandler.sa_mask = blocked;

    if (sigaction(SIGALRM, &newhandler, NULL) == -1) // try to install
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
    dir = RIGHT;
    done = 0;
    speed = INITIAL_SPEED; 

    // Start the real timer
    set_timer(ITIMER_REAL, 1000/ speed, 1000/speed);

    // Main processing loop
    while(!done)
    {
        is_changed = FALSE;
        c = getch();
        switch(c)
        {
            case 'Q':
            case 'q':
                done = 1;
                break;
            case ' ':
                dir = (dir == LEFT) ? RIGHT: LEFT;
                break;
            case 'f':
                if (1000/speed > 2 )
                {
                    speed = speed + 2;
                    is_changed = TRUE;
                }
                break;
            case 's':
                if (1000/speed <= 500)
                {
                    speed = speed - 2;
                    is_changed = TRUE;
                }
                break;
        }

        if (is_changed)
            set_timer(ITIMER_REAL, 1000/speed, 1000/speed);
    }
    
    // Clean up
    endwin();
    return 0;
}

