/* $title:      'TODAY.C ===== Return current date (+- offset) =====' */
/* $subtitle:   'Elipse Ltd.  [jms]             Revised: 1999 Jul 05' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <screen.h>

#define DLIMIT      10000

int main( int argc, char *argv[] );
static void today( int offset );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int offset = 0;
    fprintf( stderr, "\n TODAY ===== Return current date (+/- offset) =====\n " );
    fprintf( stderr, "\n             (C) Elipse Ltd. %s\n\n ", __DATE__ );
    if (argc > 1)
        {
        offset = atoi( argv[1] );
        if (offset == 0)
            {
            if ((strchr( argv[1], '?' ) != NULL) || (strchr( argv[1], 'H' ) != NULL) || (strchr( argv[1], 'h' ) != NULL))
                {
                fprintf( stderr, "\n Usage: today [+/- offset days]\n " );
                return 1;
                }
            }
        }
    if ((offset > -DLIMIT) && (offset < +DLIMIT))
        {
        today( offset );
        return 0;
        }
    fprintf( stderr, "\n offset is out of bounds \n" );
    return 2;
    }                                   /* main */


/* NOW ===== Body of program ===== $pageif:6 */

static void today( int offset )
    {                                   /* today */
    b_date cdate;
    memcpy( &cdate, current_date( ), sizeof( b_date ) );
    if (offset != 0)
        memcpy( &cdate, date_of( &cdate, offset ), sizeof( b_date ) );
    printf( " %s \n", format_ndate( &cdate ) );
    }                                   /* today */
