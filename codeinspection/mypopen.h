/**
 * @file mypopen.h
 * Betriebssysteme mypopen, mypclose Header
 * Beispiel 2
 *
 * @author Stammgruppe 5:
 * @author Brueckler Bernhard <ic15b029@technikum-wien.at>
 * @author Lassel Frederic <ic15b063@technikum-wien.at>
 * @date 2016/04/18
 *
 * @version 1
 *
 */

#pragma once

/*
 * ------------------------------------------------------------- functions --
 */

extern FILE *mypopen(const char *command, const char *type);
extern int mypclose(FILE *stream);

/*
 * =================================================================== eof ==
 */
