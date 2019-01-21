/* $title:      'FILTERIDX.C ===== Reformat index file =====' */
/* $subtitle:   'Elipse Ltd.  [jms]     Revised: 2011 Dec 31' */

/* NOTE: For wildcard expansion to work from the command line, */
/*       Link with WildArgv.obj                                */
/*       compile command:  wcl386 filteridx.c WildArgv.obj           */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#define MAXBUF      8190

int  main( int argc, char *argv[] );
void usage( void );
void filteridx( const char *infil );
void scan( FILE *input );
char *method_name( char *buf );
char *location( char *buf );
int check_name( const char *name );
int is_numeric( const char *p,  const char *q );

/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int i;
    fprintf( stderr, "\n\tFilterIdx ===== (C) Elipse Limited, %s\n", __DATE__ );
    if ( argc < 2 )
        usage( );

    for ( i = 1;  i < argc;  ++i )
        filteridx( argv[i] );
    return 0;
    }                                   /* main */


/* USAGE ===== Input file name and open ===== $pageif:6 */

void usage( void )
    {                                   /* usage */
    fprintf( stderr, "\n\t USAGE:  filteridx filename\n\n" );
    abort( );
    }                                   /* usage */


/* FILTERIDX ===== Main search routine ===== $pageif:6 */

void filteridx( const char *infil )
    {                                   /* filteridx */
    if ( infil != NULL )
        {
        FILE *fp = fopen( infil, "rb" );
        if ( fp != NULL )
            {
            scan( fp );
            fclose( fp );
            }
        }
    }                                   /* filteridx */


/* SCAN ===== Main scan routine (per file) ===== $pageif:6 */

void scan( FILE *input )
    {                                   /* scan */
    char buf[MAXBUF + 2];
    char *mthd;
    char *locn;
    memset( buf, '\0', sizeof( buf ) );

    if (fgets( buf, MAXBUF, input ) == NULL)
        {
        fprintf( stderr, "EMPTY INPUT FILE\n" );
        return;
        }
    if (strstr( buf, "#!-1-" ) == NULL)
        {
        fprintf( stderr, "WRONG FILE TYPE\n" );
        return;
        }
    printf( "#!-2- Filtered Index Data\n" );
    while (fgets( buf, MAXBUF, input ) != NULL)
        {
        char *posn = strchr( buf, '(' );
        if (posn != NULL)
            {
            *posn = '\0';
            mthd = method_name( buf );
            if (mthd != NULL)
                {
                locn = location( buf );
                if (locn != NULL)
                    printf( "%s*%s\n", mthd, locn );
                }
            }
        }
    }                                   /* scan */


/*  METHOD_NAME ===== Parse out method name ===== $pageif:6 */

char *method_name( char *buf )
    {                                   /* method_name */
    char *spc = strrchr( buf, ' ' );
    if (spc != NULL)
        {
        if (check_name( ++spc ))
            return spc;
        }
    return NULL;
    }                                   /* method_name */


/*  LOCATION ===== Parse out line number ===== $pageif:6 */

char *location( char *buf )
    {                                   /* location */
    char *p = buf;
    char *q;
    if (buf == NULL)
        {
        fprintf( stderr, "location( ): BAD INPUT\n" );
        return 0;
        }
    do
        {
        q = strchr( p, ':' );
        if (q != NULL)
            {
            if (is_numeric( p, q - 1 ))
                {
                *(++q) = '\0';
                return buf;
                }
            p = q + 1;
            }
        }
    while ((p != NULL) && (q != NULL));
    return NULL;
    }                                   /* location */


/*  CHECK_NAME ===== Make sure method is a valid name ===== $pageif:6 */

int check_name( const char *name )
    {                                   /* check_name */
    if (name == NULL)
        {
        fprintf( stderr, "check_name( ): BAD INPUT\n" );
        return 0;
        }
    do
        {
        if (!isalnum( *name ) && (*name != '_'))
            return 0;
        }
    while (*(++name) != '\0');
    return 1;
    }                                   /* check_name */


/*  IS_NUMERIC ===== Check if all digits between pointers ===== $pageif:6 */

int is_numeric( const char *p,  const char *q )
    {                                   /* is_numeric */
    int ret = 1;
    if ((p == NULL) || (q == NULL))
        {
        fprintf( stderr, "is_numeric( ): BAD INPUT\n" );
        return 0;
        }
    if (p == q)
        return 0;
    while (p <= q)
        {
        if (!isdigit( *p))
            ret = 0;
        ++p;
        }
    return ret;
    }                                   /* is_numeric */
