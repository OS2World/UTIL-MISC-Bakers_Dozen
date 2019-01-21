/* $title:      'DELAY.C ===== Wait for given number of seconds =====' */
/* $subtitle:   'Elipse Ltd.  (jms)              Revised: 2008 May 05' */


#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#define OVERHEAD    1180UL              /* in milliseconds */


/* MAIN ===== Program start up ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    printf( "printf...\n" );
    fprintf( stderr, "fprintf...\n" );
    if ( argc > 1 )
        {
        int userin = atoi( argv[1] );
        if ( (userin > 0) && (userin < 36000) )   /* up to 10 hours */
            {
            fprintf( stderr, "\nDelay for %d seconds.\n", userin );
            sleep( userin );
            return 0;
            }
        printf( "\nInvalid parameter %d\n", userin );
        return 2;                       /* invalid input */
        }
    printf( "\nMust specify number of seconds to delay\n" );
    return 3;                           /* no input */
    }
