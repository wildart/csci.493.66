/*
 * =========================================================================
 *
 *       Filename:  drawpattern.c
 *
 *    Description:  Simple draw pattern
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

int main()
{
    char pattern [] = "1234567890";
    int i;
    /* Initialize ncurses and clear the screen */
    initscr();
    clear() ;
    
    /* This will wrap a cross the screen */
    move(LINES/2, 0);
    for(i = 1; i <= 8; i++) 
    {
        addstr(pattern);
        addch(' ');
    }

    /*  Park cursor at bottom  */
    move(LINES-1,0);
    addstr("Type any char to quit:");
    refresh();

    /*  Wait for user action */
    getch();
    endwin();
    return 0;

}
