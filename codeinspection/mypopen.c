/**
 * @file mypopen.c
 * Betriebssysteme mypopen, mypclose Modul
 * Beispiel 2
 *
 * @author Stammgruppe 5:
 * @author Brueckler Bernhard <ic15b029@technikum-wien.at>
 * @author Lassel Frederic <ic15b063@technikum-wien.at>
 * @date 2016/04/18
 *
 * @version 10
 *
 * @todo: add doxygen comments for author, version, functions, 
 *		return values, etc... and maybe the header file
 * @todo: mypclose: return proper exit code if child finishes with errors
 *
 */

/*
 * -------------------------------------------------------------- includes --
 */
	/*
     * ### FB: Wo wird mypopen.h inkludiert?
     *         Includes sollten in Header und nicht im .c File stehen (bis auf mypopen.h)
     */
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
 * --------------------------------------------------------------- globals --
 */

static int number_of_open_pipes = 0;
static pid_t child_pid = -1;
static FILE *fp = NULL;

/*
 * -------------------------------------------------------------- typedefs --
 */

typedef enum {read_pipe, write_pipe} stream_type_enum;

/**
 *
 * \brief Custom implementation of the popen(3) function
 *
 * The  mypopen()  function  opens  a  process by creating a pipe, forking, and 
 * invoking the shell.  Since a pipe is by definition unidirectional, the type 
 * argument may specify only reading or writing, not both; the resulting stream 
 * is correspondingly  read-only  or  write-only.
 *
 * \param command pointer to the (null-terminated) char array of the command to be executed
 * \param type pointer to the (null-terminated) char array of either "r" or "w", depending on 
 *
 * \return pointer to the FILE struct of the read or write end of an unnamed pipe
 * \return NULL if there was an error (errno is also set)
 *
 */

FILE *mypopen(const char *command, const char *type) {
	if (number_of_open_pipes > 0) {
		/* mypopen previously created a pipe that was not closed */
		errno = EAGAIN;
		return NULL;
	}

	stream_type_enum stream_type;

	if (strcmp(type, "r") == 0) {
		stream_type = read_pipe;
	}
	else if (strcmp(type, "w") == 0) {
		stream_type = write_pipe;
	}
	else {
		/* type is neither "r" nor "w" */
		errno = EINVAL;
		return NULL;
	}
	
	/* ### FB: Variablen gehören am Anfang der Funktion */
	int fd[2];

	if (pipe(fd) == -1) {
		/* pipe() failed */
		return NULL;
	}

	/* pipe succeeded */
	number_of_open_pipes++;

	switch (child_pid = fork()) {
		case -1:
			/* fork() failed */
			(void) close(fd[0]);
			(void) close(fd[1]);
			number_of_open_pipes--;
			return NULL;

		case 0:
			/**************************** child ****************************/

			/*
			 * ### FB: Anstatt 2 unabhängige IFs sollte man ein if-else machen, da stream_type sowieso nicht read_pipe und write_pipe gleichzeitig sein kann
			 *         Die 2 IFs sind vom Code her fast ident. Doppelter Code sollte vermieden werden (z.B. Hilfsfunktion mit Parametern um dann in der Hilfsfunktion unterscheiden zu können)
			 */
			if (stream_type == read_pipe) {
				/* replace stdout with write end of pipe fd */
				/* child now puts stdout into the pipe */
				if (dup2(fd[1], STDOUT_FILENO) == -1) {
					/* dup2 failed */
					(void) close(fd[0]);
					(void) close(fd[1]);
					_Exit(EXIT_FAILURE);
				}
			}

			if (stream_type == write_pipe) {
				/* replace stdout with read end of the pipe */
				/* child now reads stdin from the pipe) */
				if (dup2(fd[0], STDIN_FILENO) == -1) {
					/* dup2 failed */
					(void) close(fd[0]);
					(void) close(fd[1]);
					_Exit(EXIT_FAILURE);
				}

			}

			/* closing read and write end of pipe fd */
			(void) close(fd[0]);
			(void) close(fd[1]);

			execl("/bin/sh", "sh", "-c", command, (char *) NULL);
			/* execl failed */
			_Exit(EXIT_FAILURE);

			break;

		default:
			/**************************** parent ****************************/
			/*
			 * ### FB: Anstatt 2 unabhängige IFs sollte man ein if-else machen, da stream_type sowieso nicht read_pipe und write_pipe gleichzeitig sein kann
			 *         Die 2 IFs sind vom Code her fast ident. Doppelter Code sollte vermieden werden (z.B. Hilfsfunktion mit Parametern um dann in der Hilfsfunktion unterscheiden zu können)
			 */
			if (stream_type == read_pipe) {
				(void) close(fd[1]);
				fp = fdopen(fd[0], "r");
			}

			if (stream_type == write_pipe) {
				(void) close(fd[0]);
				fp = fdopen(fd[1], "w");
			}

			if (fp == NULL) {
				/* fdopen failed */
				(void) close(fd[0]);
				(void) close(fd[1]);
				child_pid = -1;
				number_of_open_pipes--;
				return NULL;
			}

			/* fdopen succeeded */
			return fp;
	}
}

/**
 *
 * \brief Custom implementation of the pclose(3) function
 *
 * The mypclose() function waits for the associated process to terminate and 
 * returns the exit status of the command as returned by wait4(2).
 *
 * \param stream pointer to the FILE struct to be closed
 *
 * \return -1 if there was en error (errno is also set)
 * \return exit status of the child process if there was no error
 *
 */
	      
int mypclose(FILE *stream) {
	if (number_of_open_pipes == 0) {
		/* mypclose was called before mypopen */
		errno = ECHILD;
		return -1;
	}

	if ((stream == NULL) || (stream != fp)) {
		/* file pointer fp is NULL or it doesnt match the pointer we */
		/* previously opened with mypopen */
		errno = EINVAL;
		return -1;
	}

	if (fclose(stream) != 0) {
		/* fclose failed */
		errno = EINVAL;
		return -1;
	}
	else {
		/* fclose succeeded */
		number_of_open_pipes--;

		/* wait for the child to exit */
		/* ### FB: Variablen gehören am Anfang der Funktion */
		pid_t wait_pid;
		int status;

		while ((wait_pid = waitpid(child_pid, &status, 0)) != child_pid) {
			if (wait_pid == -1) {
				/* error in waitpid() */
				if (errno == EINTR) {
					/* waitpid() was interrupted, continue loop */
					continue;
				}
				/* waitpid() failed */
				child_pid = -1;
				errno = ECHILD;
				return -1;
			}
		}

		/* child has terminated, check exit status */
		child_pid = -1;
		if (WIFEXITED(status)) {
			/* child terminated normally */
			return WEXITSTATUS(status);
		}
		else {
			/* child did not terminate normally */
			errno = ECHILD;
			return -1;
		}
	}
}

/*
 * =================================================================== eof ==
 */
