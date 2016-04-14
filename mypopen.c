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
		{
			// close unused pipe end
			close (fd[READ_END]);
			
			// Link STDOUT with pipe fd
			if (dup2(fd[WRITE_END], STDOUT_FILENO) == -1) {
				// dup2 failed: close pipe and end child process (NO RETURN, because then child and parent would send return value to main) 
				close (fd[WRITE_END]);
				_exit(EXIT_FAILURE);
			}
		}
		// write command		
		else 
		{
			// close unused pipe end
			close (fd[WRITE_END]);
			
			// Link STDIN with pipe fd
			if (dup2(fd[READ_END], STDIN_FILENO) == -1) {
				// dup2 failed: close pipe and end child process (NO RETURN, because then child and parent would send return value to main) 
				close (fd[READ_END]);
				_exit(EXIT_FAILURE);
			}
		}
		
		// Execute the command in a shell:
		execl("/bin/sh", "sh", "-c", command, NULL);
		
		// This code only get executed if execl fails. This code is used to "kill" the child process:
		_exit(EXIT_FAILURE);
	}
	// Parent process
	else if (childpid > (pid_t) 0)
	{
		// read command
		if (strcmp(type, "r"))
		{
			// close unused pipe end
			close (fd[WRITE_END]);
			
			// Link STDIN with pipe fd
			if (dup2(fd[READ_END], STDIN_FILENO) == -1) {
				// dup2 failed 
				close (fd[READ_END]);
				return NULL;
			}
			
			fp = fdopen(fd[READ_END], type);
		}
		// write command		
		else 
		{
			// close unused pipe end
			close (fd[READ_END]);
			
			// Link STDOUT with pipe fd
			if (dup2(fd[WRITE_END], STDOUT_FILENO) == -1) {
				// dup2 failed 
				close (fd[WRITE_END]);
				return NULL;
			}
			
			fp = fdopen(fd[WRITE_END], type);
		}
	}
	else {
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
// WENN FEHLER DANN VORHER AUFRÄUMEN: pipe schließen etc
// execl("/bin/sh", "sh", "-c", command, NULL);

// mypclose():
// waitpid(); 
// Pipe wird geschlossen wenn letzter filedeskriptor geschlossen wird
// fclose von Filepointer (fclose schließt den darunter liegeneden filedeskriptor fd)
