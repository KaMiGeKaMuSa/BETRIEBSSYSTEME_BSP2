/**
 * @file mypopen.c
 * mypopen_mypclose
 * Beispiel 2
 *
 * @author Karin Kalman <karin.kalman@technikum-wien.at>
 * @author Michael Mueller <michael.mueller@technikum-wien.at>
 * @author Gerhard Sabeditsch <gerhard.sabeditsch@technikum-wien.at>
 * @date 2016/04/17
 *
 * @version $Revision: 1 $
 *
 *
 * URL: $HeadURL$
 *
 * Last Modified: $Author: Michael $
 */

/*
 * -------------------------------------------------------------- includes --
 */
#include "mypopen.h"

/*
 * -------------------------------------------------------------- local static variables --
 */
static FILE* fp = NULL;


/*
 * -------------------------------------------------------------- mypopen -function --
 */
// BSP: http://stackoverflow.com/questions/9255425/writing-to-a-pipe-with-a-child-and-parent-process    
FILE *mypopen(const char *command, const char *type)
{
	//variable for file descriptor
    int fd[2];
	pid_t childpid;
	
	//check if filepointer is already open -> if so, then thats not OK -> only one instance is allowed
    if (fp != NULL)
	{
		errno = EAGAIN;
		return NULL;
	}
	
	//check if correct type is given
    if ((strcmp(type, "r") != 0 && strcmp(type, "w") != 0) || command == NULL)
	{
		// no valid type
		errno = EINVAL;
		return NULL;
	}
	
	/* 
     * Open new pipe
     * - return value fd[0] is for reading
	 * - return value fd[1] is for writing
     */
     if(pipe(fd) == -1)
	{
		// pipe failed: according to documentation NULL should be returned
		return NULL;
	}
	
	/*
     * if fork() is successful it return twice:
     * - in parent process the return value is the process id (PID) of the childprocess
     * - in child process the return value is 0
     */
    childpid = fork();
	
	/// Child process
	if(childpid == (pid_t) 0)
	{
		// read command
		if (strcmp(type, "r") == 0)
			childAction(fd, READ_END, WRITE_END, command);
		// write command		
		else 
			childAction(fd, WRITE_END, READ_END, command);
	}
	/// Parent process
	else if (childpid > (pid_t) 0)
	{
		// read command
		if (strcmp(type, "r") == 0)
			fp = parentAction(fd, WRITE_END, READ_END, type);
		// write command		
		else 
			fp = parentAction(fd, READ_END, WRITE_END, type);
	}
	else 
	{
		// fork failed: according to documentation NULL should be returned, but before close pipe:
		close (fd[READ_END]);
		close (fd[WRITE_END]);
		return NULL;
	}
	
	return fp;
}




/*
 * -------------------------------------------------------------- mypclose -function --
 */
int mypclose(FILE *stream)
{
    int state;

    /*check if filepointer is already open -> if NOT, then thats not OK -> popen should be run first!*/
    if (fp == NULL)
    {
        errno = ECHILD;
        return -1;
    }
    
    
    /* check if stream is the correct stream to close */
    if (fp != stream) {
        errno = EINVAL;
        return -1;
    }
    
    /*if fclose returns an EOF, then there was already a stream passing-> Error -1*/
    if (fclose(stream) == EOF) {
        fp = NULL;
        errno = ECHILD;
        return -1;
    }
    
    /*wait till errno == EINTR*/
    do {
        wait(&state);
    } while (errno == EINTR);
    
    /*after errno == EINTR, then filepointer can set to NULL */
    fp = NULL;
    
    
    /* only when state != 0 everything is OK */
    if (WIFEXITED(state) != 0) {
        return WEXITSTATUS(state);
    }
    
    errno = ECHILD;
    return -1;
    
}


/*
 * -------------------------------------------------------------- help - functions --
 */

/*
 * static void childAction(int fd[2], int unused_end, int used_end, const char *command)
 * FUNCTION is only for the CHILD PROCESS:
 * -this function link the used file descriptor with the parent process stdout
 * -this function also close the unused file descriptor
 * -this function executes the given command in a normal shell like popen
 */
static void childAction(int fd[2], int unused_end, int used_end, const char *command)
{
	// close unused pipe end
	close (fd[unused_end]);

        // Link STDOUT with pipe fd
        if (dup2(fd[used_end], STDOUT_FILENO) == -1) {
		// dup2 failed: close pipe and end child process (NO RETURN, because then child and parent would send return value to main) 
		close (fd[used_end]);
		exit(EXIT_FAILURE);
        }

	// Execute the command in a shell:
	execl("/bin/sh", "sh", "-c", command, NULL);
	
	// This code only get executed if execl fails. This code is used to "kill" the child process:
	exit(EXIT_FAILURE);
}


/*
 * static FILE * parentAction(int fd[2], int unused_end, int used_end, const char *type)
 * FUNCTION is only for the PARENT PROCESS:
 * -this function link the used file descriptor with the parent process stin
 * -this function also close the unused file descriptor
 * -this function convert the used filedescriptor into a filepointer and returns the filepointer
 */
static FILE * parentAction(int fd[2], int unused_end, int used_end, const char *type)
{
	FILE* fp_temp = NULL; 
	
	// close unused pipe end
	close (fd[unused_end]);
	
	// Link STDIN with pipe fd
	if (dup2(fd[used_end], STDIN_FILENO) == -1) {
		// dup2 failed 
		close (fd[used_end]);
		return NULL;
	}
	
	// Convert file descriptor into file pointer
	if ((fp_temp = fdopen(fd[used_end], type)) == NULL) 
	{
		// fdopen failed: close pipe and return null
		close(fd[used_end]);
		return NULL;
	}
	
	return fp_temp;
}

// WENN FEHLER DANN VORHER AUFRÄUMEN: pipe schließen etc
// execl("/bin/sh", "sh", "-c", command, NULL);

// mypclose():
// waitpid(); 
// Pipe wird geschlossen wenn letzter filedeskriptor geschlossen wird
// fclose von Filepointer (fclose schließt den darunter liegeneden filedeskriptor fd)
