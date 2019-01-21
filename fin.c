/* $title:      'FIN.C ===== Find text in files (with line number) =====' */
/* $subtitle:   'Elipse Ltd.  [jms]                Revised: 2009 Dec 26' */

/* NOTE: For wildcard expansion to work from the command line, */
/*       Link with WildArgv.obj                                */
/*       compile command:  wcl386 fin.c WildArgv.obj           */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#define MAXBUF      8190

static char *search_text = NULL;        /* what we are looking for */

int  main( int argc, char *argv[] );
void usage( void );
void fin( const char *infil );
void scan( FILE *input, const char *infil );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int i;
    fprintf( stderr, "\n\n\t FIN\t---\t (C) Elipse Ltd.  Dec 2008-2009\n\n" );
    if ( argc < 3 )
        usage( );

    search_text = strlwr( argv[1] );
    for ( i = 2;  i < argc;  ++i )
        fin( argv[i] );
    printf( "\n" );
    return 0;
    }                                   /* main */


/* USAGE ===== Input file name and open ===== $pageif:6 */

void usage( void )
    {                                   /* usage */
    fprintf( stderr, "\n\t USAGE:  fin text filename [filename] ...\n\n" );
    abort( );
    }                                   /* usage */


/* FIN ===== Main search routine ===== $pageif:6 */

void fin( const char *infil )
    {                                   /* fin */
    if ( infil != NULL )
        {
        FILE *fp = fopen( infil, "rb" );
        if ( fp != NULL )
            {
            scan( fp, infil );
            fclose( fp );
            }
        }
    }                                   /* fin */


/* SCAN ===== Main scan routine (per file) ===== $pageif:6 */

void scan( FILE *input, const char *infil )
    {                                   /* scan */
    char buf[MAXBUF + 2], *p;
    unsigned line = 0;
    unsigned count = 0;
    memset( buf, '\0', sizeof( buf ) );

    while (fgets( buf, MAXBUF, input ) != NULL)
        {
        ++line;
        if (strstr( strlwr( buf ), search_text ) != NULL)
            {
            if (count++ == 0)
                printf( "\n----- %s\n", infil );
            p = strchr( buf, '\r' );
            if (p != NULL)
                *p = ' ';
            printf( "[%03u] %s", line, buf );
            }
        }
/***
    if (count != 0)
        {
        if (count == 1)
            printf( "----- %u occurrance\n", count );
        else
            printf( "----- %u occurrances\n", count );
        }
***/
    }                                   /* scan */
