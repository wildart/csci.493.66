/*
 * =========================================================================
 *
 *       Filename:  mypwd.c
 *
 *    Description:  Implementation of 'pwd' util
 *
 *        Version:  1.0
 *        Created:  03/07/2013 07:17:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Wild
 *        Company:  Hunter College
 *
 * =========================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

// BUFSIZ is defiend in stdio.h: it is the maximum string size
#define MAXPATH BUFSIZ

// print_pwd prints the pwd to the string abs_pathname and
// NULL-terminates it
void print_pwd( char *abs_pathname );

// print_path prints to abs_pathname the path from / to the file
// specified be cur_inum
void print_path( char *bs_pathname, ino_t cur_inum );

// inum_to_name puts the filename of the file with i-node inum
// into string buf, at most len chars long and 0 termnates it.
void inum_to_name( ino_t inum, char *buf, int len );

// get_ino gets the i-node number of file name, if that fname
// is the name if a file in the current working directory
// Returns 0 if successful, -1 if not.
int get_ino(char *fname, ino_t *inum);


int main(int argc, char *argv[])
{
    char path[MAXPATH] = "\0";  // string to store pwd
    print_pwd( path );
    printf("%s\n", path);
    return EXIT_SUCCESS;
}

void print_pwd( char *pathname )
{
    ino_t inum;

    get_ino(".", &inum);
    print_path(pathname, inum);
}

void print_path( char *abs_pathname, ino_t cur_inum )
{
    ino_t parent_inode;
    char  its_name[BUFSIZ];

    // get i-number of parent
    get_ino("..", &parent_inode);
    if ( parent_inode != cur_inum)
    {
        chdir("..");  // cd up to parent
        // get filename of current file
        inum_to_name(cur_inum, its_name, BUFSIZ);
        // recursively get path to prent directory
        print_path(abs_pathname, parent_inode);
        strcat(abs_pathname, "/");
        strcat(abs_pathname, its_name);
    }
    else
        strcat(abs_pathname, "/");
}

void inum_to_name(ino_t inode_to_find, char *namebuf, int buflen)
{
    DIR           *dirp;
    struct dirent *direntp;
    
    dirp = opendir(".");
    if ( dirp == NULL )
    {
        perror( "." );
        exit(EXIT_FAILURE);
    }

    // search directory for a file with scpecified inum
    while( (direntp = readdir(dirp) ) != NULL )
        if ( direntp->d_ino == inode_to_find )
        {
            strncpy( namebuf, direntp->d_name, buflen);
            namebuf[buflen-1] = '\0';
            closedir(dirp);
            return;
        }
    fprintf(stderr, "\nError looking for i-node number: %lu\n", inode_to_find);
    exit(EXIT_FAILURE);
}

int get_ino(char *fname, ino_t *inum)
{
    struct stat info;
    if ( stat(fname, &info) == -1)
    {
        fprintf(stderr, "Cannot get stat");
        perror(fname);
        return -1;
    }
    *inum = info.st_ino;
    return 0;
}
