/* $title:      'SHOWTIME.C ===== Empty pass thourgh pipe =====' */
/* $subtitle:   'Elipse Ltd.  [jms]                2005 Jul 05' */


#include <stdio.h>
#include <screen.h>


/* MAIN ===== Start of program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    printf( "%s %s\n", format_adate( current_date( ) ), format_time( current_time( ) ) );
    return 0;
    }                                   /* main */
