/*
 * =====================================================================================
 *
 *       Filename:  ls03.c
 *
 *    Description:  ls tool (list directories)
 *
 *        Version:  3.0
 *        Created:  02/25/2013 05:41:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky (artyom.diky@hunter.cuny.edu) 
 *        Company:  Hunter College
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <langinfo.h>

#define HERE "."
#define PARENT ".."

void print_file_status(char *, struct stat *);
char* mode2str(int);
char* uid2name(uid_t);
char* gid2name(gid_t);
void ls(char [], int);

int main ( int argc, char *argv[] )
{
    int longlisting = 0;
    int ch;
    char options[] = ":l";

    opterr = 0; // turn off error message by getopt
    while(1)
    {
        ch = getopt(argc, argv, options);

        if ( ch == -1 )
            break;

        switch( ch ){
        case 'l':
            longlisting = 1;
            break;
        case '?':
            printf("Illigal option ignored.\n");
            break;
        default:
            printf("getopt returned character code 0%o ??\n", ch);
            break;
        }
    }

    if (optind == argc)   // no arguments
        ls (HERE, longlisting);
    else
        while (optind < argc)
        {
            ls( argv[optind], longlisting );
            optind++;
        }

    return EXIT_SUCCESS;
} 

// List content of a direcory
void ls(char dirname[], int do_longlisting)
{
    DIR	    		*dir_ptr; // dir stream
    struct dirent	*direntp; // dir entry
    struct stat     statbuff; // file stat results
    char            fname[PATH_MAX]; // holds path name


    // check is it is a file or regular dir
    if ( stat(dirname, &statbuff) == -1)
    {
        perror(fname);
        return; // stat is failed so we quit this call
    }
    else if (! S_ISDIR(statbuff.st_mode) )
    {
        if (do_longlisting)
            print_file_status(dirname, &statbuff);
        else
            printf("%s\n", dirname);
    }

    if ( (dir_ptr = opendir(dirname) ) == NULL)
        fprintf(stderr, "Cannot open %s\n", dirname);
    else
    {
        printf("\n%s:\n", dirname);
        // loop through dir entries
        while ( (direntp = readdir(dir_ptr) ) != NULL)
        {
            if ( strcmp(direntp->d_name, HERE) == 0 ||
                 strcmp(direntp->d_name, PARENT) == 0)
                // skip . and .. entries
                continue;

            if (do_longlisting)
            {
                // construct pathname for file
                sprintf(fname,"%s/%s", dirname, direntp->d_name);

                // fill stat buffer
                if (stat(fname, &statbuff) == -1)
                {
                    perror(fname);
                    continue; // stat call failed
                }
                print_file_status(direntp->d_name, &statbuff);
            }
            else // short listing
                printf("%s\n", direntp->d_name);
        }
        closedir(dir_ptr);
    }
}

void print_file_status(char *fname, struct stat *info_p)
{
    ssize_t count;
    char buf[NAME_MAX];
    struct tm *tm;
    char datestring[256];

    printf("%10.10s", mode2str(info_p->st_mode));    // type + mode
    printf("%4d "  , (int)info_p->st_nlink);   // # links
    printf("%-8s " , uid2name(info_p->st_uid));     // user id 
    printf("%-8s " , gid2name(info_p->st_gid));    // group
    printf("%9jd " , (intmax_t)info_p->st_size);    // file size
    
    // get loclized time in tm structure
    tm = localtime(&info_p->st_mtime);
    strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);

    printf("%s %s", datestring, fname);           // last modified & filename

    if (S_ISLNK(info_p->st_mode))
    {
        if ( (count = readlink(fname, buf, NAME_MAX-1)) == -1)
            perror("print_file_status:");
        else
        {
            buf[count] = '\0';
            printf("->%s", buf);
        }
    }
    printf("\n");
}

char* mode2str(int mode)
{
    static char str[11];
    strcpy(str, "-----------");

    if (S_ISDIR(mode)) 		 str[0] = 'd';	// directory?
    else if (S_ISCHR(mode))	 str[0] = 'c';  // char dev
    else if (S_ISBLK(mode))	 str[0] = 'b';  // block dev
    else if (S_ISLNK(mode))	 str[0] = 'l';  // sym link
    else if (S_ISFIFO(mode)) str[0] = 'p';  // name pipe (FIFO)
    else if (S_ISSOCK(mode)) str[0] = 's';  // socket

    if (mode & S_IRUSR)	 str[1] = 'r';  // 3 bits for user
    if (mode & S_IWUSR)	 str[2] = 'w';  
    if (mode & S_IXUSR)	 str[3] = 'x';  
    
    if (mode & S_IRGRP)	 str[4] = 'r';  // 3 bits for user
    if (mode & S_IWGRP)	 str[5] = 'w';  
    if (mode & S_IXGRP)	 str[6] = 'x';  
    
    if (mode & S_IROTH)	 str[7] = 'r';  // 3 bits for user
    if (mode & S_IWOTH)	 str[8] = 'w';  
    if (mode & S_IXOTH)	 str[9] = 'x';  
    
    if (mode & S_ISUID)	 str[3] = 's';  // set-uid 
    if (mode & S_ISGID)	 str[8] = 'w';  // set-gid
    if (mode & S_ISVTX)	 str[9] = 't';  // sticky bit

    return str;
}

char* uid2name(uid_t uid)
{
    struct passwd *pw_prt;
    static char numstr[10]; // must be static!

    if (( pw_prt = getpwuid(uid)) == NULL)
    {
        sprintf(numstr, "%d", uid);
        return numstr;
    }
    else
        return pw_prt->pw_name;
}

char* gid2name(gid_t gid)
{
    struct group *grp_prt;
    static char numstr[10]; // must be static!
     
    if (( grp_prt = getgrgid(gid)) == NULL)
    {
        sprintf(numstr, "%d", gid);
        return numstr;
    }
    else
        return grp_prt->gr_name;
}

