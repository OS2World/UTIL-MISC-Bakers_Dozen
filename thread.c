/* $title:      'Thread.C ===== Test if threads die when 1 does =====' */
/* $subtitle:   'Elipse Ltd.  [jms]              Revised: 1994 Apr 04' */


#define INCL_DOSPROCESS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <os2.h>

static TID tid;
static int repeat = 500;

int main( int argc, char *argv[] );
void thread( void *argstr );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int i, n = 1;
    TID t;
    fprintf( stderr, "\n\n\t Thead Test\t---\t (C) Elipse Ltd.  Apr. 1994\n\n" );
    
    if (argc > 1)
        n = atoi( argv[1] );
    if (n > 1)
        fprintf( stderr, "\n%d extra threads requested\n" );
    else
        n = 1;
    if (argc > 2)
        repeat = atoi( argv[2] );
    fprintf( stderr, "\n%d counts per thead\n", repeat );

    for ( i = 0;  i < n;  ++i )
        {
        DosEnterCritSec( );
        tid = _beginthread(thread, NULL, 12288, NULL );
        DosExitCritSec( );
        printf( "\nThread number %ld running\n", tid );
        }

    _getch( );
    printf( "\nRequest to end\n" );

    do
        {
        t = 0;
        DosWaitThread( &t, DCWW_WAIT );
        printf( "\nFound thread %d running\n", t );
        }
    while (t != 0);

    printf( "\nDone\n" );
    return 0;
    }                                   /* main */


/* THREAD ===== Counting routine ===== $pageif:6 */

void thread( void *voidptr )
    {                                   /* thread */
    int i, mytid, mycount;
    
    DosEnterCritSec( );
    mytid = (int) tid;
    mycount = repeat;
    DosExitCritSec( );
    
    for ( i = 0;  i < mycount;  ++i )
        printf( "\t%5d: Count%4d\n", mytid, i );
    }                                   /* thread */

