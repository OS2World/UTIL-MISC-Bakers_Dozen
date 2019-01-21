/* $title:      'ADJFILE.C ===== File routines for adjtext =====' */
/* $subtitle:   'Elipse Ltd.  [jms]         Revised: 1993 Jan 02' */


#include <stdio.h>
#include <string.h>
#include "adjfile.h"

static int minline = 0;
static int linelen = -1;
static FILE *infile = NULL;
static char nextline[BUFSIZE];

static void readnext( void );


/* FILE_OPEN ===== Open text file for input ===== $pageif:6 */

int file_open( char *filename, int linewidth )
    {                                   /* file_open */
    minline = linewidth / 2;
    memset( nextline, '\0', sizeof (nextline) );
    infile = fopen( filename, "r" );
    readnext( );
    if ( infile == NULL )
        return 0;                       /* failure */
    else
        return 1;                       /* success */
    }                                   /* file_open */


/* FILE_PEEK ===== Look ahead to next line ===== $pageif:6 */

int file_peek( void )
    {                                   /* file_peek */
    if ( (linelen == 0) && (infile == NULL) )
        return ENDFILE;
    if ( *nextline == ' ' )
        return SPACE;
    if ( strchr( nextline, '\t' ) != NULL )
        return TABS;
    if ( linelen == 0 )
        return NIL;
    if ( linelen < minline )
        return SHORT;
    return NORMAL;
    }                                   /* file_peek */


/* FILE_READ ===== Return next line of text ===== $pageif:6 */

int file_read( char *databuf )
    {                                   /* file_read */
    int type = file_peek( );
    memset( databuf, '\0', BUFSIZE );
    strcpy( databuf, nextline );
    readnext( );
    return type;
    }                                   /* file_read */


/* READNEXT ===== Get next text line into buffer ===== $pageif:6 */

static void readnext( void )
    {                                   /* readnext */
    char *end = NULL;
    if ( infile != NULL )
        {
        if ( fgets( nextline, BUFSIZE, infile ) == NULL )
            {                           /* empty file! */
            fclose( infile );
            memset( nextline, '\0', sizeof (nextline) );
            infile = NULL;
            }
        end = strchr( nextline, '\n' );
        if ( end == NULL )
            linelen = strlen( nextline );
        else
            {
            *end = '\0';
            linelen = end - nextline;
            }
        while ( (linelen > 0) && (nextline[linelen - 1] == ' ') )
            nextline[--linelen] = '\0';
        }
    }                                   /* readnext */

