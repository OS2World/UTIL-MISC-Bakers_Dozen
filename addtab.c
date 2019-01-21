/* $title:      'ADDTAB.C ===== To fix makefile for AIX =====' */
/* $subtitle:   'Elipse Ltd.  [jms]      Revised: 1996 Oct 30' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEF_SPACE   4

int main( int argc, char *argv[] );
void addtab( int spc );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int spc = DEF_SPACE;
    fprintf( stderr, "\nAdd tabs to text file\n" );
    if ( argc > 1 )
        spc = atoi( argv[1] );
    if ( (spc < 1) || (spc > 40) )
        spc = DEF_SPACE;
    addtab( spc );
    return 0;
    }                                   /* main */


/* ADDTAB ===== Body of program ===== $pageif:6 */

void addtab( int spc )
    {                                   /* addtab */
    char buf[2048];
    memset( buf, '\0', sizeof (buf) );

    while ( gets( buf ) != NULL )
        {
        int i;
        for ( i = 0;  i < spc;  ++i )
            {
            if ( buf[i] != ' ' )
                break;
            }
        if ( i >= spc )
            printf( "\t%s\n", &buf[spc] );
        else
            printf( "%s\n", buf );
        memset( buf, '\0', sizeof (buf) );
        }
    }                                   /* addtab */
