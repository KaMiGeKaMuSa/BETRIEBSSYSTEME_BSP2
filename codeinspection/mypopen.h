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
/* ### FB: 1) Good practice um sicherzustellen, dass Header nicht öfters definiert wird:
*           #ifndef _MYPOPEN_H_
*           ......
*           #endif 
*         2) includes gehören in Headerfile nicht in .c File
*         3) Warum sind Funktionsprototypen als extern deklariert? Header gehört in .c File inkludiert dann muss es nicht über Makefile gelinkt werden
*/
/*
 * ------------------------------------------------------------- functions --
 */

extern FILE *mypopen(const char *command, const char *type);
extern int mypclose(FILE *stream);

/*
 * =================================================================== eof ==
 */
