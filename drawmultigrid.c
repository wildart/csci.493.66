/*
 * =========================================================================
 *
 *       Filename:  drawgrid.c
 *
 *    Description:  Simple grid in multiple windows
 *
 *        Version:  1.0
 *        Created:  04/03/2013 04:00:42 PM
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
#include <curses.h>
#include <string.h>

#define CENTERY (LINES/2-2) // Middle line in terminal
#define CENTERX (COLS/2-2)  // Middle column in terminal
#define NUMROWS (LINES/2)
#define NUMCOLS (COLS/2)
#define REPEATS 5
#define GRIDCHARS ".*@+#"

int main()
{
    int  i, j, k, length;
    char MESSAGE[]=
         "TYp the character of the grid to bding it forward, 'q' to quit.";
    WINDOW *wMessage;
    WINDOW *windows[REPEATS];
    char    gridchar[REPEATS] = GRIDCHARS;
    int     rowshift, colshift;
    int     ch;


    /* Initialize ncurses and clear the screen */
    initscr();
    noecho();

    
    /* Make sure that this winows is wide enough for message  at the bottom */
    length = strlen(MESSAGE);
    if (length > COLS - 2)
    {
        endwin();
        printf("This program need a wider window.\n");
        exit(1);
    }

    /*  Calculate the amount by witch we shift each window whn we draw it. */
    rowshift = (LINES - NUMROWS)/5;
    colshift = (COLS - NUMCOLS)/5;

    /* In this loop, we create a new window, fill it with a grid of a unique char */
    for(j=0;j<REPEATS;j++)
    {
        /*  Create a new windo at an offset from (0,0) with grid */
        windows[j] = newwin(NUMROWS, NUMCOLS, rowshift*j, colshift*j);
        if (windows[j] == NULL)
        {
            endwin();
            fprintf(stderr, "Error creating window\n");
            exit(1);
        }
        
        /*  Draw each grid row as string into new window */
        for(i = 0; i < NUMROWS; i++) 
        {
            for(k = 0;k < NUMCOLS; k++) 
            {
                wmove(windows[j], i, k);
                if (waddch(windows[j], gridchar[j]) == ERR)
                    // Ignore errors
                    ;
            }
         }
         
         /* Update the virtual scree with this window's content */
         wnoutrefresh(windows[j]);
    }
    /*  NOw send virtual screen to physical screen */
    doupdate();

    /*  Create a window to hold a message and put it in the bottom row */
    wMessage = newwin(1, COLS, LINES-1, 0);

    /*  Write message into it */
    mvwaddstr(wMessage, 0, 0, MESSAGE);
    wrefresh(wMessage);

    while(1)
    {
        /*  Read a chr from the message window not from stdscr 
         *  Call to wgetch forces a refresh of its window argument.
         *  If we refresh stdscr, our grid will disappear.
         */
        ch = wgetch(wMessage);
        if (ch == 'q')
            break; // Exit

        /*  Check if they typed a grid char */
        for (j = 0; j< REPEATS; j++)
        {
            if (ch == gridchar[j])
            {
                wmove(wMessage, 0, length); // Move cursor to bottom
                touchwin(windows[j]);       // Force window update
                wrefresh(windows[j]);       // Refresh window and exit check loop
                break;
            }
        }
    }
    
    /*  Clean up and exit */
    clear();
    endwin();
    return 0;

}
