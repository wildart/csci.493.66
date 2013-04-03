/*
 * =========================================================================
 *
 *       Filename:  drawgrid.c
 *
 *    Description:  Simple grid
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
#include <curses.h>

#define CENTERY (LINES/2-2) // Middle line in terminal
#define CENTERX (COLS/2-2)  // Middle column in terminal
#define NUMROWS (LINES/2)
#define NUMCOLS (COLS/2)

int main()
{
    int r, c, i, j;
    
    /* Initialize ncurses and clear the screen */
    initscr();
    clear();
    noecho();

    
    /* This will wrap a cross the screen */
    move(LINES/2, 0);
    char grid[NUMROWS][NUMCOLS];
    for(i = 0; i < NUMROWS; i++) 
    {
        for(j = 0;j < NUMCOLS; j++)        
            grid[i][j] = '.';
        grid[i][NUMCOLS-1] = '\0';            
    }
    /* MOve to center */
    r = CENTERY - (NUMROWS / 2);
    c = CENTERX - (NUMCOLS / 2);
    move(r,c);

    /*  Draw grid  */
    for(i = 0; i < NUMROWS; i++)
    {
        mvaddstr(r+i, c, grid[i]);
    }

    /*  Park cursor at bottom  */
    move(LINES-1,0);
    addstr("Type any char to quit:");
    refresh();

    /*  Wait for user action */
    getch();
    clear();
    endwin();
    return 0;

}
