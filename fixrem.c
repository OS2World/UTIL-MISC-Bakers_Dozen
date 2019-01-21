/* $title:      'FIXREM.C ==== Change remark to standard C notation =====' */
/* $subtitle:   'Elipse Ltd.  [jms]                  Revised: 1998 Jun 28' */


#include <stdio.h>
#include <string.h>

int main( int argc, char *argv[] );
static void fixrem( void );
static char *convert( char *str );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\n\t FIXREM ===== Change remarks to standard C notation\n" );
    fprintf( stderr, "\n\t Last revision: %s \n\n", __DATE__ );
    fixrem( );
    fprintf( stderr, "\n\t Done\n\n" );
    return 0;
    }                                   /* main */


/* FIXREM ===== Body of program ===== $pageif:6 */

static void fixrem( void )
    {                                   /* fixrem */
    char buf[2048];
    memset( buf, '/0', sizeof (buf) );
    while ( gets( buf ) != NULL )
        printf( "%s\n", convert( buf ) );
    putc( '\x1a', stdout );
    fflush( stdout );
    }                                   /* fixrem */


/* CONVERT ===== Replace symbols ===== $pageif:6 */

static char *convert( char *str )
    {                                   /* convert */
    char *p = strstr( str, "//" );
    if ( p != NULL )
        {
        *(++p) = '*';
        strcat( str, " */" );
        }
    return str;
    }                                   /* convert */
