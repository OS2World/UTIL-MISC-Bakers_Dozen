/* $title:      'ADJFILE.H ===== File routines for adjtext =====' */
/* $subtitle:   'Elipse Ltd.  [jms]         Revised: 1992 Dec 31' */


#ifndef ADJFILE_H
#define ADJFILE_H   1

#include <stdio.h>

#define BUFSIZE     1024
#define MAXWORD     24

#define NORMAL      0
#define ENDFILE     1
#define NEWLINE     2
#define SPACE       3
#define TABS        4
#define NIL         5
#define SHORT       6


int file_open( char *filename, int linewidth );
int file_peek( void );
int file_read( char *databuf );

#endif                                  /* ADJFILE_H */
