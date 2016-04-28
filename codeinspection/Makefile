## @file Makefile
## Betriebssysteme mypopen makefile
## Beispiel 2
##
## @author Stammgruppe 5: 
## @author Brueckler Bernhard <ic15b029@technikum-wien.at>
## @author Lassel Frederic <ic15b063@technikum-wien.at>
##
## @date: 2016/04/10
##
## @version 1
##

##
## ------------------------------------------------------------- variables --
##

CC=gcc52
CFLAGS=-DDEBUG -Wall -pedantic -Werror -Wextra -Wstrict-prototypes -fno-common -g -O3 -std=gnu11

RM=rm
CP=cp
CD=cd
MV=mv
GREP=grep
DOXYGEN=doxygen

OBJECTS=mypopen.c

##
## ----------------------------------------------------------------- rules --
##

%.o: %.c
	$(CC) $(CFLAGS) -c $<

##
## --------------------------------------------------------------- targets --
##

.PHONY: all
all: mypopen.o

mypopen: $(OBJECTS)
	$(CC) $(CFLAGS) -c $@ $^

.PHONY: clean
clean: mypopen.o
	$(RM) *.o

.PHONY: distclean
distclean: 
	$(RM) -rf doc

doc: html pdf

.PHONY: html
html:
	$(DOXYGEN) doxygen.dcf

pdf: html
	$(CD) doc/pdf && \
	$(MV) refman.tex refman_save.tex && \
	$(GREP) -v $(EXCLUDE_PATTERN) refman_save.tex > refman.tex && \
	$(RM) refman_save.tex && \
	make && \
	$(MV) refman.pdf refman.save && \
	$(RM) *.pdf *.html *.tex *.aux *.sty *.log *.eps *.out *.ind *.idx \
	      *.ilg *.toc *.tps Makefile && \
	$(MV) refman.save refman.pdf

##
## ---------------------------------------------------------- dependencies --
##

##
## =================================================================== eof ==
##
