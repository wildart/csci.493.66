/*
 * =========================================================================
 *
 *       Filename:  cursortrack.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/03/2013 05:29:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  
 *
 * =========================================================================
 */
#include <curses.h>

/*  LINES and COLS are NCurses variables that get initialized when 
    initscr() is called. 
*/
#define CENTERY (LINES/2 - 2) /*  The middle line in terminal */
#define CENTERX (COLS/2 - 2) /*  The middle column in terminal */

int main(int argc, char *argv[])
{
    int x, y;/* to retrieve coordinates of cursor */
    int ch;  /* to receive user input character */
    int r,c; /* to store coordinates of cursor*/

    /* A string to display in the "statusbar" at the bottom of the screen */
    char MESSAGE[]="Use the arrow keys to move the cursor."
        " Press F1 to exit";

    initscr();   /* Initialize screen */
    clear();     /* Clear the screen */
    noecho();    /* turn off character echo */
    cbreak();    /* disable line buffering */
    keypad(stdscr,TRUE); /* Turn on function keys */

    /* Move to bottom left corner of screen, write message there */
    move(LINES-1,0);
    addstr(MESSAGE);

    /*  Start the cursor at the screen center */
    r = CENTERY;
    c = CENTERX;
    move(r,c);

    /*  Print the cursor's coordinates at the lower right */
    move(LINES-1,COLS-8);
    printw("%02d,%02d",r,c);
    refresh();

    /*  Move back to center */
    move(r,c);

    /*  Repeatedly wait for user input using getch(). Because we turned off 
        echo and put curses into cbreak mode, getch() will return without 
        needing to get a newline char and will not echo the character. 
        When the user presses the F1 key, the program quits.
     */
    while((ch=getch())!=KEY_F(5))
    {
        switch(ch){
            /*  When keypad() turns on function keys, the arrow keys are enabled 
                and are named KEY_X, where X is LEFT,RIGHT,etc.
                This switch updates the row or column as needed, modulo COLS
                horizontally to wrap, and LINES-1 to wrap vertically without
                entering the sanctity of the statusbar.
             */
        case KEY_LEFT:
            c = (0==c)? COLS-1 : c-1;
            break;
        case KEY_RIGHT:
            c = (c==COLS-1) ? 0 : c+1;
            break;
        case KEY_UP:
            r = (0==r) ? LINES-2 : r-1;
            break;
        case KEY_DOWN:
            r = (r==LINES-2) ? 0 : r+1;
            break;
        }

        /*  Now we move the cursor to the new position, get its coordinates
            and then move to the lower right to print the new position
         */
        move(r,c);
        getyx(stdscr,y,x);
        move(LINES-1,COLS-8);
        printw("(%02d,%02d)",y,x);
        refresh();
        /*  Now we have to move back to where cursor was */
        move(r,c);
    }
    endwin();
    return 0;
}
