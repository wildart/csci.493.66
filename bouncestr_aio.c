/*
 * =========================================================================
 *
 *       Filename:  bouncestr_aio.c
 *
 *    Description:  Usage of AIO for animation demo
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
#include <aio.h>

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

struct aiocb kbcbuf;

void on_alarm(int);          // handler for SIGALRM
void on_input(int);          // handler for SIGIO
int update_from_input(int*, int*);
void setup_aio_buffer(struct aiocb *buff);

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

int main(int argc, char *argv[])
{
    // Setup signal handling
    struct sigaction newhandler;
    sigset_t         blocked;
    int              finished;

    newhandler.sa_handler = on_input;   // name of handler
    newhandler.sa_flags   = SA_RESTART; // restart handler on execution
    sigemptyset(&blocked);              // create empty set of blocked sirnals
    newhandler.sa_mask = blocked;       // setup this set to be mask
    if( sigaction(SIGIO, &newhandler, NULL) == -1)
        perror("sigaction");

    newhandler.sa_handler = on_alarm;  // set handler for SIGALRM
    if ( sigaction(SIGALRM, &newhandler, NULL) == -1)
        perror("sigaction");

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
    finished = 0;
    speed = INITIAL_SPEED; 

    // Turn on keyboard signals
    setup_aio_buffer(&kbcbuf);
    aio_read(&kbcbuf);

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
            finished = update_from_input(&speed, &dir);
            input_ready = 0;
        }
        else
            pause();
    }

    // Clean up
    endwin();
    return 0;
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

// Handler called when aio_read() has something to read
int update_from_input(int *speed, int *dir)
{
    int c;
    int is_changed = FALSE;
    char mssg[40];
    char *cp = (char *) kbcbuf.aio_buf; // cast to char
    int finished = FALSE;

    // Check for errors
    if ( aio_error(&kbcbuf) == 1)
    {
        perror("reading failed");
    }
    else
        // get number of char to read
        if (aio_return(&kbcbuf) == 1)
        {

            c = *cp;
            /*ndelay = 0;*/
            switch(c)
            {
                case 'Q':
                case 'q':
                    finished = 1;    // quit program
                    break;

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
            }

            move(LINES-1, COLS-12);
            sprintf(mssg, "Last char: %c", c);
            addstr(mssg);
            refresh();
        }
    
    // Place new request
    aio_read(&kbcbuf);
    return finished;
}

void setup_aio_buffer(struct aiocb *buf)
{
    static char input[1];

    // describe what to read
    buf->aio_fildes    = 0;      // file descriptor for I/O
    buf->aio_buf       = input;  // address of buffer
    buf->aio_nbytes    = 1;      // buffer size
    buf->aio_offset    = 0;    

    // describe what to do when read is ready
    buf->aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    buf->aio_sigevent.sigev_signo  = SIGIO;        // send SIGIO
}
