/* title:       'NONASCII.C ===== Look for unusual characters =====' */
/* subtitle:    'Elipse Ltd.  [jms]            Revised: 2012 Mar 28' */


/* Read stdin and report characters outside the standard ASCII range */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main( int argc, char *argv[] );
void nonascii( int from, int to );


/* MAIN ===== Entry point for program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\nNONASCII ===== (C) 2012, Elipse Ltd.\n\n" );
    nonascii( 32, 126 );
    return 0;
    }                                   /* main */


/* NONASCII ===== Body of program ===== $pageif:6 */

void nonascii( int lo, int hi )
    {                                   /* nonascii */
    unsigned count = 0;
    int ch;
    while ((ch = getchar( )) != EOF)
        {
        ++count;
        if ((ch < lo) || (ch > hi))
            {
            if ((ch != 9) && (ch != 10) && (ch != 13))
                printf( "   pos =%6d dec or %04x hex, char = %3d dec or %02x hex or [%c] \n", count, count, ch, ch, ch );
            }
        }
    }                                   /* nonascii */
