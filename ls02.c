/*
 * =====================================================================================
 *
 *       Filename:  ls02.c
 *
 *    Description:  ls tool (list directories)
 *
 *        Version:  2.0
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

#define HERE "."

void print_file_status(char *, struct stat *);
void mode2str(int, char[]);
char* get_date_no_day(time_t);
char* uid2name(uid_t);
char* gid2name(gid_t);
void ls(char [], int);

int main ( int argc, char *argv[] )
{
    int i = 1;

    if (argc == 1)
        ls(HERE, 1);
    else
        while (i < argc)
        {
            printf("%s:\n", argv[i]);
            ls( argv[i], 1 );
            i++;
        }

    return EXIT_SUCCESS;
} 

// List content of a direcory
void ls(char dirname[], int do_longlisting)
{
    DIR	    		*dir_ptr; // dir stream
    struct dirent	*direntp; // dir entry
    struct stat     statbuff; // file stat results

    if ( (dir_ptr = opendir(dirname) ) == NULL)
        fprintf(stderr, "ls2: cannot open %s\n", dirname);
    else
    {
        while ( (direntp = readdir(dir_ptr) ) != NULL)
        {
            if (do_longlisting)
            {
                if (stat(direntp->d_name, &statbuff) == -1)
                {
                    perror(direntp->d_name);
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
    char modestr[11];
    ssize_t count;
    char buf[NAME_MAX];

    mode2str(info_p->st_mode, modestr);
    printf("%s"    , modestr);    // type + mode
    printf("%4d "  , (int)info_p->st_nlink);   // # links
    printf("%-8s " , uid2name(info_p->st_uid));     // user id 
    printf("%-8s " , gid2name(info_p->st_gid));    // group
    printf("%8ld " , (long)info_p->st_size);    // file size
    printf("%.12s ", get_date_no_day(info_p->st_mtime));    // last modified
    printf("%s"    , fname);           // filename

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



void mode2str(int mode, char str[])
{
    strcpy(str, "----------");

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
}
char* get_date_no_day(time_t time)
{
  return ctime(&time)+4;
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

