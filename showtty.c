/*
 * =========================================================================
 *
 *       Filename:  showtty.c
 *
 *    Description:  Implementation of 'stty' utility
 *
 *        Version:  1.0
 *        Created:  03/08/2013 05:39:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Wild
 *        Company:  
 *
 * =========================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

// Mapping of flag bit poition to name
typedef struct __flaginfo flaginfo;
struct __flaginfo
{
    int   fl_value;  // flag value
    char *fl_name;   // flag description
};

// Mapping of index to decription
typedef struct __cc_entry cc_entry;
struct __cc_entry
{
    int   index;
    char *desc;
};

flaginfo input_flags[] = {
    {IGNBRK, "Ignore break condition"},
    {BRKINT, "Signal interrupt on break"},
    {IGNPAR, "Ignore chars with parity errors"},
    {PARMRK, "Mark parity errors"},
    {INPCK,  "Enable input parity check"},
    {ISTRIP, "Strip character"},
    {INLCR,  "Map NL to CR on input"},
    {IGNCR,  "Ignore CR"},
    {ICRNL,  "Map CR to NL on input"},
    {IXON,   "Enable start / stop output control"},
    {IXOFF,  "Enable start / stop input control"},
    {-1, NULL}
};

cc_entry cc_values[] = {
    {VEOF,    "The EOF character"},
    {VEOL,    "The EOL character"},
    {VERASE,  "The ERASE character"},
#ifdef VWERASE
    {VWERASE, "The WERASE character"},
#endif
    {VINTR,   "The INTR character"},
    {VKILL,   "The KILL character"},
    {VQUIT,   "The QUIT character"},
    {VSTART,  "The START character"},
    {VSTOP,   "The STOP character"},
    {VSUSP,   "The SUSP character"},
    {VMIN,    "The MIN value"},
    {VTIME,   "The TIME value"},
    {-1, NULL}
};

void show_baud_rate(struct termios *ttyinfo);
void show_cc_array(struct termios ttyinfo, cc_entry controlchars[]);
void show_input_flags(int flag, flaginfo bitnames[]);

int main(int argc, char* argv[])
{
    struct termios ttyinfo;
    FILE *fp;

    if( (fp = fopen(ctermid(NULL), "r+")) == NULL )
        exit(EXIT_FAILURE);

    if( tcgetattr( fileno(fp), &ttyinfo ) == -1)
    {
        perror( "Cannot get info about this terminal." );
        exit(EXIT_FAILURE);
    }

    show_baud_rate( &ttyinfo );
    printf("\n");
    show_cc_array( ttyinfo, cc_values );
    printf("\n");
    show_input_flags( ttyinfo.c_iflag, input_flags );
    
    return EXIT_SUCCESS;
}

void show_baud_rate(struct termios *ttyinfo)
{
    int speed = 0;
    speed_t tspeed = cfgetospeed(ttyinfo);
    if (tspeed & B38400)
        speed = 38400;
    else if (tspeed & B57600)
        speed = 57600;

    printf("speed %d baud\n", speed);
}

void show_cc_array(struct termios ttyinfo,
        cc_entry controlchars[])
{
    int i = 0;
    while( controlchars[i].index != -1)
    {
        printf( "%s in ", controlchars[i].desc );
        printf( "Cntl-%c\n", ttyinfo.c_cc[controlchars[i].index]-1+'A' );
        i++;
    }
}

void show_input_flags(int flag, flaginfo bitnames[])
{
    int i = 0;
    while( bitnames[i].fl_value != -1)
    {
        printf( "%s is ", bitnames[i].fl_name);
        if( flag & bitnames[i].fl_value )
            printf("ON\n");
        else
            printf("OFF\n");
        i++;
    }
}

