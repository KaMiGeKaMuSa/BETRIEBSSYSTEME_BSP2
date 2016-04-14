#include "mypopen.h"

static FILE* fp = NULL;

// BSP: http://stackoverflow.com/questions/9255425/writing-to-a-pipe-with-a-child-and-parent-process    
FILE *mypopen(const char *command, const char *type)
{
	int fd[2];
	pid_t childpid;
	
	if (fp != NULL)
	{
		errno = EAGAIN;
		return NULL;
	}
	
	if ((strcmp(type, "r") != 0 && strcmp(type, "w") != 0) || command == NULL)
	{
		// no valid type
		errno = EINVAL;
		return NULL;
	}
	
	// Open new pipe 
	// - return value fd[0] is for reading
	// - return value fd[1] is for writing
	if(pipe(fd))
	{
		// pipe failed: according to documentation NULL should be returned
		return NULL;
	}
	
	// if fork() is successful it return twice: 
	// 		- in parent process the return value is the process id (PID) of the childprocess
	//		- in child process the return value is 0
    childpid = fork();
	
	// Child process
	if(childpid == (pid_t) 0)
	{
		// read command
		if (strcmp(type, "r"))
			childAction(fd, READ_END, WRITE_END, command);
		// write command		
		else 
			childAction(fd, WRITE_END, READ_END, command);
	}
	// Parent process
	else if (childpid > (pid_t) 0)
	{
		// read command
		if (strcmp(type, "r"))
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
	  
int mypclose(FILE *stream)
{
	return pclose(stream);
}

static void childAction(int fd[2], int unused_end, int used_end, const char *command)
{
	// close unused pipe end
	close (fd[unused_end]);
	
	// Link STDOUT with pipe fd
	if (dup2(fd[used_end], STDOUT_FILENO) == -1) {
		// dup2 failed: close pipe and end child process (NO RETURN, because then child and parent would send return value to main) 
		close (fd[used_end]);
		_exit(EXIT_FAILURE);
	}
	
	// Execute the command in a shell:
	execl("/bin/sh", "sh", "-c", command, NULL);
	
	// This code only get executed if execl fails. This code is used to "kill" the child process:
	_exit(EXIT_FAILURE);
}

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
		close(pipe_ends[used_end]);
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
