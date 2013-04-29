/*
 * =========================================================================
 *
 *       Filename:  upcased2.c
 *
 *    Description:  Concurrent server
 *
 *        Version:  1.0
 *        Created:  04/28/2013 05:56:46 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Art Diky
 *        Company:  Hunter College
 *
 * =========================================================================
 */
#include "upcase2.h"
#include <sys/wait.h>
#include <syslog.h>

#define WARGING  "\nNOTE: server cannot access client FIFO\n"
#define MAXTRIES 5

int dummyfifo;   // file descriptor to write-end of PUBLIC
int client_convertedtext; // filedescriptor to write-end of PRIVATE
int client_rawtext; // file descriptor to read-end of PRIVATE
int publicfifo;  // file descriptor to read-end of PUBLIC
FILE* upcaselog;
pid_t server_pid;

void daemon_init(const char*, int);

void on_sigpipe(int signo)
{
    fprintf(stderr, "Client is not reading the pipe.\n");
}

void on_signal(int signo)
{
    close(publicfifo);
    close(dummyfifo);
    if( client_convertedtext != -1 )
        close(client_convertedtext);
    if( client_rawtext != -1 )
        close(client_rawtext);
    if ( getpid() == server_pid )
        unlink(PUBLIC);
    fclose(upcaselog);
    exit(0);
}

void on_sigchld(int signo)
{
    pid_t pid;
    int status;

    while((pid=waitpid(-1, &status, WNOHANG)) > 0)
        fprintf(upcaselog, "Child process %d terminated.\n", pid);
    fflush(upcaselog);
    return;
}

int main(int argc, char *argv[])
{
    //daemon_init(argv[0], 0);
    
    int              i;
    int              nbytes;  // number of bytes read from popen()
    int              tries;   // number of tries to open private FIFO
    int              done;    // flag to stop main loop
    message          msg;     // stores private fifo name and command
    struct sigaction handler; 
    char             buffer[PIPE_BUF];
    char             logfilepath[PATH_MAX];
    char             *homepath;  // path to home directory
    pid_t            child_pid;  // pid of each spawed child

    // Open the log file in the user's home direcoty
    homepath = getenv("HOME");
    sprintf(logfilepath, "%s/.upcase_log", homepath);
    if((upcaselog = fopen(logfilepath, "a")) == NULL){
        perror(logfilepath);
        exit(1);
    }

    // Register signal handlers
    handler.sa_handler = on_signal;
    handler.sa_flags = SA_RESTART;
    if( (sigaction(SIGINT, &handler, NULL) == -1) ||
         (sigaction(SIGHUP, &handler, NULL) == -1) ||
         (sigaction(SIGQUIT, &handler, NULL) == -1) ||
         (sigaction(SIGTERM, &handler, NULL) == -1) 
      ){
        perror("sigaction");
        exit(1);
    }

    handler.sa_handler = on_sigpipe;
    if(sigaction(SIGPIPE, &handler, NULL) == -1){
        perror("sigaction");
        exit(1);
    }

    handler.sa_handler = on_sigchld;
    if(sigaction(SIGCHLD, &handler, NULL) == -1){
        perror("sigaction");
        exit(1);
    }

    // Create public FIFO
    if(mkfifo(PUBLIC, 0666) < 0){
        if(errno != EEXIST)
            perror(PUBLIC);
        else
            fprintf(stderr, "%s already exists. Delete it and restart.\n", PUBLIC);
        exit(1);
    }

    // Open public FIFO for reading and writing
    if( (publicfifo = open(PUBLIC, O_RDONLY)) == -1 ||
        (dummyfifo  = open(PUBLIC, O_WRONLY | O_NDELAY)) == -1){
        perror(PUBLIC);
        exit(1);
    }

    server_pid = getpid();

    // Block waiting for a msg struct from client
    while( read(publicfifo, (char*) &msg, sizeof(msg)) > 0 ){
        // spawn child for a msg structure from a client
        if( fork() == 0 ){
            // We get the pid for message id
            child_pid = getpid();
            // We use the value of client_rawtext to detect errors
            client_rawtext = -1;
            // Client should open rawtext for writing before sending message structure
            // so the following open should success immidiately. If not it blocks
            // until the client opens it.
            if( (client_rawtext = open(msg.raw_text_fifo, O_RDONLY)) == -1){
                fprintf(upcaselog, "Client did not have pipe open for writing.\n");
                exit(1);
            }
            // Clear buffer
            memset(buffer, 0x0, PIPE_BUF);
            // Attempt to read from client's raw_text_fifo. This read will block
            // until either input is avalible or it receives an EOF.
            // An EOF is delivered only when the client closes the write-end
            // of its raw_text_fifo.
            while( (nbytes = read(client_rawtext, buffer, PIPE_BUF)) > 0){
                // Convert the text to uppercase
                for(i=0;i<nbytes;i++)
                    if( islower(buffer[i]) )
                        buffer[i]=toupper(buffer[i]);
                // Message arrived, so start trying to open write end of private FIFO
                tries = 0;
                while (((client_convertedtext = open(msg.converted_text_fifo, 
                          O_WRONLY | O_NDELAY)) == -1) && (tries < MAXTRIES))
                {
                    sleep(2);
                    tries++;
                }
                if(tries == MAXTRIES){
                    fprintf(upcaselog, "%d: "WARGING, child_pid);
                    exit(1);
                }

                // Send converted text to client
                if(write(client_convertedtext, buffer, nbytes) == -1)
                    if (errno == EPIPE)
                        exit(1);
               
                close(client_convertedtext);
                client_convertedtext = -1;

                // Clear the buffer used for reading
                memset(buffer, 0x0, PIPE_BUF);
            }
            // Stop child process
            exit(0);
        }
    }
        
    return 0;
}

void daemon_init(const char *pname, int facility)
{
    int     i;
    pid_t pid;

    if((pid=fork()) == -1){
        perror("fork");
        exit(1);
    }
    else if(pid != 0)
        exit(0); // parent terminates

    /* Child continues from here */
    // Detach itself and make itself a session leader
    setsid();

    // Ignore SIGHUP
    signal(SIGHUP, SIG_IGN);

    if((pid=fork()) == -1){
        perror("fork");
        exit(1);
    }
    else if(pid !=0)
        exit(0); // first child terminates

    /* Grandchild continues from here */
    // Set working directory
    chdir("/");

    // Clear out file more creation mask
    umask(0);

    // Close all open file descriptors
    for(i=getdtablesize(); i>=0; --i)
        close(i);

    // Start loggign with syslog()
    openlog(pname, LOG_PID, facility);
}
