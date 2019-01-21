/* $title:      'WAITKEY.C ===== Exit on key or timeout =====' */
/* $subtitle:   'Elipse Ltd.  [jms]      Revised: 2008 Jan 15' */


#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>

#define TIC_FACT    (1000L / CLOCKS_PER_SEC)

int main( int argc, char *argv[] );
int waitkey( unsigned ms );

#define INTERVAL 20


/* MAIN ===== Calling program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    unsigned int secs = 0;
    char *msg = "Press any key... ";
    if (argc > 1)
        {
        secs = atoi( argv[1] );
        if (argc > 2)
            msg = argv[2];
        }
    if ((secs < 5) || (secs > 65))
        secs = INTERVAL;
    puts( msg );
    exit( waitkey( secs * 1000 ) );
    return 0;
    }                                   /* main */


/* WAITKEY ===== Wait for time or keypress ===== $pageif:6 */

int waitkey( unsigned ms )
    {                                   /* waitkey */
    long stime, etime;
    stime = (long) clock( ) * TIC_FACT; /* read current time in ms */
    etime = stime + (long) ms;          /* calc final time */
    while ( ((long) clock( ) * TIC_FACT) < etime )
        if (kbhit( ))
            return 1;
    return 0;
    }                                   /* waitkey */
