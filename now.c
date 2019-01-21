/* $title:      'NOW.C ===== Return current date and time =====' */
/* $subtitle:   'Elipse Ltd.  [jms]        Revised: 2002 Dec 10' */


#include <stdio.h>
#include <string.h>
#include <screen.h>

int main( int argc, char *argv[] );
static void now( void );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int i;
    fprintf( stderr, "\n NOW ===== Return current date and time =====\n " );
    fprintf( stderr, "\n           (C) Elipse Ltd. %s\n\n ", __DATE__ );
    now( );
    for (i = 1;  i < argc;  ++i)
        printf( " %s", argv[i] );
    printf( "\n" );
    return 0;
    }                                   /* main */


/* NOW ===== Body of program ===== $pageif:6 */

static void now( void )
    {                                   /* now */
    b_date cdate;
    b_time ctime;
    memcpy( &cdate, current_date( ), sizeof( b_date ) );
    memcpy( &ctime, current_time( ), sizeof( b_time ) );
    printf( " %s %s ", format_ndate( &cdate ), format_time( &ctime ) );
    }                                   /* now */
