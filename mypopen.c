#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define READ_END 0
#define WRITE_END 1
// BSP: http://stackoverflow.com/questions/9255425/writing-to-a-pipe-with-a-child-and-parent-process    
FILE *mypopen(const char *command, const char *type)
{
	int fd[2];
	pid_t childpid;
	FILE* fp;
	
	if (strcmp(type, "r") != 0 && strcmp(type, "R") != 0 && strcmp(type, "w") != 0 && strcmp(type, "W") != 0)
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
    childpid = fork();
	
	// Child process
	if(childpid == (pid_t) 0)
	{
		// read command
		if (strcmp(type, "r") == 0 || strcmp(type, "R") == 0)
		{
			// close unused pipe end
			close (fd[READ_END]);
			
			// Link STDOUT with pipe fd
			if (dup2(fd[WRITE_END], STDOUT_FILENO) == -1) {
				// dup2 failed 
				return NULL;
			}
		}
		// write command		
		else 
		{
			// close unused pipe end
			close (fd[WRITE_END]);
			
			// Link STDIN with pipe fd
			if (dup2(fd[READ_END], STDIN_FILENO) == -1) {
				// dup2 failed 
				return NULL;
			}
		}
		
		// Execute the command in a shell:
		execl("/bin/sh", "sh", "-c", command, NULL);
		
		// This code only get executed if execl fails. This code is used to "kill" the child process:
		exit(EXIT_FAILURE);
	}
	// Parent process
	else if (childpid > (pid_t) 0)
	{
		// read command
		if (strcmp(type, "r") == 0 || strcmp(type, "R") == 0)
		{
			// close unused pipe end
			close (fd[WRITE_END]);
			
			// Link STDIN with pipe fd
			if (dup2(fd[READ_END], STDIN_FILENO) == -1) {
				// dup2 failed 
				return NULL;
			}
		}
		// write command		
		else 
		{
			// close unused pipe end
			close (fd[READ_END]);
			
			// Link STDOUT with pipe fd
			if (dup2(fd[WRITE_END], STDOUT_FILENO) == -1) {
				// dup2 failed 
				return NULL;
			}
		}
	}
	else {
		// fork failed: according to documentation NULL should be returned
		return NULL;
	}
	fp = fdopen(fd, type);
	
	return fp;
}
	  
int mypclose(FILE *stream)
{
	return pclose(stream);
}
// WENN FEHLER DANN VORHER AUFRÄUMEN: pipe schließen etc
// execl("/bin/sh", "sh", "-c", command, NULL);

// mypclose()
// waitpid(); 
// Pipe wird geschlossen wenn letzter filedeskriptor geschlossen wird
// fclose von Filepointer (fclose schließt den darunter liegeneden filedeskriptor fd)
