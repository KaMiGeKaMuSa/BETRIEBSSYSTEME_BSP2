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
	
	if (*type == "r" || *type == "R")
	{
		// read command
		if(childpid == 0)
		{
				// Child process
				// closes read side of pipe, because child need to write 
				close(fd[READ_END]);
		} 
		else 
		{
				// Parent process
				// close write side of pipe, because parent need to read
				if (dup2(fd[WRITE_END], STDOUT_FILENO) == -1) {
				   // Handle dup2() error 
			    }
				close(fd[WRITE_END]);
		}
		
	} 
	else if (*type == "w" || *type == "W")
	{
		// write command
		if(childpid == 0)
		{
				// Child process
				// close write side of pipe, because child need to read
				close(fd[WRITE_END]);
		} 
		else
		{
				// Parent process 
				// close write side of pipe, because parent need to write
				close(fd[READ_END]);
		}
	}
	
	return popen(command, type);
}
	  
int mypclose(FILE *stream)
{
	return pclose(stream);
}
