/**
 * @file mypopen.h
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
 * Last Modified: $Author: Gerhard $
 */


#ifndef _MYPOPEN_H_
#define _MYPOPEN_H_

/*
 * -------------------------------------------------------------- includes --
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

/*
 * --------------------------------------------------------------- defines --
 */

#define READ_END 0
#define WRITE_END 1

/*
 * --------------------------------------------------- function prototypes --
 */

FILE *mypopen(const char *command, const char *type);
int mypclose(FILE *stream);
static void childAction(int fd[2], int unused_end, int used_end, const char *command);
static FILE * parentAction(int fd[2], int unused_end, int used_end, const char *type);

#endif /* _MYPOPEN_H_ */
