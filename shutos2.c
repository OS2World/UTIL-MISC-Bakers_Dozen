/* $title:       'ShutOS2.C  =====  Shut Down System  =====' */
/* $subtitle:   'Elipse Ltd.  [jms]   Revised: 1994 Oct 17' */


#define INCL_DOSFILEMGR   /* File Manager values */
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>


int main( int argc, char *argv[] );
void shut( ULONG value );


/* MAIN ===== Start ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    printf( "\nShutOS2 program\n" );
    if (argc > 1)
        {
        ULONG value = atoi( argv[1] );
        printf( "\nParameter is %lu\n", value );
        shut( value );
        }
    else
        {
        printf( "\nUSAGE: ShutOS2 shutval\n" );
        printf( "\nshutval = 0 for full, or = 1 for partial\n" );
        }
    return 1;                           /* some error occurred */
    }                                   /* main */


/* SHUT ===== Execute shut down ===== $pageif:6 */

void shut( ULONG value )
    {                                   /* shut */
    APIRET rc = DosShutdown( value );
    if (rc != 0)
        printf( "\nDosShutdown failed: return code = %ld\n", rc );
    }                                   /* shut */
