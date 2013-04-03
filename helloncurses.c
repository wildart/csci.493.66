/*
 * =========================================================================
 *
 *       Filename:  helloncurses.c
 *
 *    Description:  Simle usage of NCurses lib
 *
 *        Version:  1.0
 *        Created:  04/03/2013 03:25:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include <ncurses.h>

int main()
{
    initscr();              // initialize the library
    printw("Hello World!"); // print at cursor
    refresh();              // update screen
    getch();                // wait for key press
    endwin();               // clean up and quit
    return 0;
}
