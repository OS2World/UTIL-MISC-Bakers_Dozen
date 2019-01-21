/* $title:      'NOTHING.C ===== Empty pass thourgh pipe =====' */
/* $subtitle:   'Elipse Ltd.  [jms]                2005 Feb 17' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int main( int argc, char *argv[] );
static void nothing( void );
static void doline( void );


/* MAIN ===== Start of program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\nNOTHING, (C) Elipse Ltd., Feb 2005\n" );
    doline( );
    nothing( );
    doline( );
    return 0;
    }                                   /* main */


/* NOTHING ===== Program body ===== $pageif:6 */

static void nothing( void )
    {                                   /* nothing */
    while (1)
        {
        int ch = getchar( );
        if (ch == EOF)
            break;
        putchar( ch );
        }
    }                                   /* nothing */


/* DOLINE ===== Draw horizontal line ===== $pageif:6 */

static void doline( void )
    {                                   /* doline */
    int i;
    for (i = 0;  i < 80;  ++i)
        putchar( '_' );
    putchar( '\n' );
    putchar( '\n' );
    }                                   /* doline */
