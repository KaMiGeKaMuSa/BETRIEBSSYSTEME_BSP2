#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define READ_END 0
#define WRITE_END 1
    
FILE *mypopen(const char *command, const char *type)
{
	int     fd[2];
	pid_t   childpid;
	
	// Open new pipe 
	// - return value fd[0] is for reading
	// - return value fd[1] is for writing
	pipe(fd);
	
	// fork() return  the process id (PID) of the childprocess
	childpid = fork()
	
	if(childpid == 0)
	{
		// Child process
		// Link STDIN, STDOUT with pipe fd[0], fd[1]
		if (*type == "r" || *type == "R")
		{
			// read command
			if (dup2(fd[READ_END], STDOUT_FILENO) == -1) {
				// Handle dup2() error 
		    	}
		} 
		else 
		{
			// write command
			if (dup2(fd[WRITE_END], STDIN_FILENO) == -1) {
				// Handle dup2() error 
		    	}
		}
	} 
	else 
	{
		// Parent process
	}
	
	close(fd[WRITE_END]);
	close(fd[READ_END]);
	
	return popen(command, type);
}
	  
int mypclose(FILE *stream)
{
	return pclose(stream);
}
