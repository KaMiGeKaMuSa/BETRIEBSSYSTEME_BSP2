#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define READ_END 0
#define WRITE_END 1
// BSP: http://stackoverflow.com/questions/9255425/writing-to-a-pipe-with-a-child-and-parent-process    
FILE *mypopen(const char *command, const char *type)
{
	int     fd[2];
	pid_t   childpid;
	
	if (*type == "r" && *type != "R" && *type == "w" && *type != "W")
	{
		// no valid type
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
	childpid = fork()
	
	// Child process
	if(childpid == (pid_t) 0)
	{
		// read command
		if (*type == "r" || *type == "R")
		{
			// close unused pipe end
			close (mypipe[READ_END]);
			
			// Link STDOUT with pipe fd
			if (dup2(fd[WRITE_END], STDOUT_FILENO) == -1) {
				// dup2 failed 
				return NULL;
			}
			
			// read from pipe .....
		}
		// write command		
		else 
		{
			// close unused pipe end
			close (mypipe[WRITE_END]);
			
			// Link STDIN with pipe fd
			if (dup2(fd[READ_END], STDIN_FILENO) == -1) {
				// dup2 failed 
				return NULL;
			}
			
			// write into pipe ...... (exec)
		}
	}
	// Parent process
	else if (childpid > (pid_t) 0)
	{
		// read command
		if (*type == "r" || *type == "R")
		{
			// close unused pipe end
			close (mypipe[WRITE_END]);
			
			// Link STDIN with pipe fd
			if (dup2(fd[READ_END], STDIN_FILENO) == -1) {
				// dup2 failed 
				return NULL;
			}
			
			// read from pipe ..... (wait until child die)
		}
		// write command		
		else 
		{
			// close unused pipe end
			close (mypipe[READ_END]);
			
			// Link STDOUT with pipe fd
			if (dup2(fd[WRITE_END], STDOUT_FILENO) == -1) {
				// dup2 failed 
				return NULL;
			}
			
			// write into pipe .... (exec)
		}
	}
	else
	{
		// fork failed: according to documentation NULL should be returned
		return NULL;
	}
	
	return popen(command, type);
}
	  
int mypclose(FILE *stream)
{
	return pclose(stream);
}
