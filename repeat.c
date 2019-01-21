/* $title:      'REPEAT.C ===== Repeat string multiple times =====' */
/* $subtitle:   'Elipse Ltd.  [jms]                    2003 Oct 07' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char *argv[] );
static void repeat( unsigned long n, const char *str );


/* MAIN ===== Start of program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int i;
    unsigned long num_rep;
    char str_val[512];
    memset( str_val, '\0', sizeof (str_val) );
    fprintf( stderr, "\nREPEAT, (C) Elipse Ltd. %s\n", __DATE__ );
    if ( argc < 2 )
        {
        fprintf( stderr, "\nUSAGE: repeat n [string]\n" );
        fprintf( stderr, "\nString default is NULL\n" );
        return 1;
        }
    if ( argc > 1 )
        num_rep = strtoul( argv[1], NULL, 10 );
    for ( i = 2;  i < argc;  ++i )
        strncat( str_val, argv[i], sizeof (str_val) );

    repeat( num_rep, str_val );
    fprintf( stderr, "\nEnd of program\n" );
    return 0;
    }                                   /* main */


/* REPEAT ===== Program body ===== $pageif:6 */

static void repeat( unsigned long n, const char *str )
    {                                   /* repeat */
    unsigned long i = 0;
    if ( (str == NULL) || (strlen( str ) == 0) )
        {
        while ( ++i <= n )
            putc( 0, stdout );
        }
    else
        {
        while ( ++i <= n )
            printf( str );
        }
    }                                   /* repeat */

