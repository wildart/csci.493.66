/*
 * =========================================================================
 *
 *       Filename:  pwd02.c
 *
 *    Description:  Implementation of 'pwd' util
 *
 *        Version:  2.0
 *        Created:  03/08/2013 07:17:22 PM
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
// specified be cur_inum on device with device id dev_num
void print_path( char *abs_pathname, ino_t cur_inum, dev_t dev_num );

// inum_to_name puts the filename of the file with i-node inum
// on device with device number dev_num
// into string buf, at most len chars long and 0 termnates it.
void inum_to_name( ino_t inum, dev_t dev_num, char *buf, int len );

// get_ino gets the device id and i-node number of file name, 
// if that fname is the name if a file in the current working directory
// Returns 0 if successful, -1 if not.
int get_device_inode(char *fname, ino_t *inum, dev_t *dev_id);


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
    dev_t devnum;

    get_device_inode(".", &inum, &devnum); 
    print_path(pathname, inum, devnum);
}

void print_path( char *abs_pathname, ino_t cur_inum, dev_t cur_dev )
// Recurcevly prints path leading down to file with cur_inum on cur_dev
// Uses static int height to determine which recursive level it is in
{
    ino_t parent_inode;
    char  its_name[BUFSIZ];
    dev_t dev_of_parent;
    static int height = 0;

    // get i-number and dev id of parent
    get_device_inode("..", &parent_inode, &dev_of_parent);
    
    // at root iff parent_inode == cur_inum and device ids are same 
    if (( parent_inode != cur_inum) || (dev_of_parent != cur_dev) )
    {
        chdir("..");  // cd up to parent
        // get filename of current file
        inum_to_name(cur_inum, cur_dev, its_name, BUFSIZ);
        height++;
        // recursively get path to prent directory
        print_path(abs_pathname, parent_inode, dev_of_parent);
        strcat(abs_pathname, its_name); 
        if (height > 1)
            // Since height is decremented whenever we leave call 
            // it can only be > 1 if we have not yet popped all
            // calls from the stack
            strcat(abs_pathname, "/");
        height--;
    }
    else // must be root
        strcat(abs_pathname, "/");
}

void inum_to_name(ino_t inode_to_find, dev_t dev_to_find, char *namebuf, int buflen)
{
    DIR           *dirp;
    struct dirent *direntp;
    struct stat   statbuf;
    
    dirp = opendir(".");
    if ( dirp == NULL )
    {
        perror( "." );
        exit(EXIT_FAILURE);
    }

    // search directory for a file with scpecified inum
    while( (direntp = readdir(dirp) ) != NULL )
    {
        if ( stat( direntp->d_name, &statbuf) == -1)
        {
            fprintf(stderr, "Cannot get stat");
            perror(direntp->d_name);
            exit(EXIT_FAILURE);
        }
        if ( ( statbuf.st_ino == inode_to_find ) &&
             ( statbuf.st_dev == dev_to_find ))
        {
            strncpy( namebuf, direntp->d_name, buflen);
            namebuf[buflen-1] = '\0';
            closedir(dirp);
            return;
        }
    }
    fprintf(stderr, "\nError looking for i-node number: %lu\n", inode_to_find);
    exit(EXIT_FAILURE);
}

int get_device_inode(char *fname, ino_t *inum, dev_t *dev_id)
{
    struct stat info;
    if ( stat(fname, &info) == -1)
    {
        fprintf(stderr, "Cannot get stat");
        perror(fname);
        return -1;
    }
    //printf("%s, d#: %lu, i#: %lu\n", fname, info.st_dev, info.st_ino); 
    *inum = info.st_ino;
    *dev_id = info.st_dev;
    return 0;
}
