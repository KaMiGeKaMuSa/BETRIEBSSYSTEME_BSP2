#ifndef _MYPOPEN_H_
#define _MYPOPEN_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#define READ_END 0
#define WRITE_END 1

FILE *mypopen(const char *command, const char *type);
int mypclose(FILE *stream);
static void childAction(int fd[2], int unused_end, int used_end, const char *command);
static FILE * parentAction(int fd[2], int unused_end, int used_end, const char *type);

#endif /* _MYPOPEN_H_ */
