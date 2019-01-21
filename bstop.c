/* $title:      'BSTOP.C ===== Remove Btrieve from memory =====' */
/* $subtitle:   'Elipse Ltd.  [jms]        Revised: 1999 Mar 26' */


#include <stdio.h>
#include <string.h>
#include <bfile.h>

static int bstop( void );


/* MAIN ===== Start up ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\n\t\t REMOVE BTRIEVE FROM MEMORY\n" );
    fprintf( stderr, "\n\t\t Last revision: %s\n", __DATE__ );
    return bstop( );
    }                                   /* main */


/* BSTOP ===== Body of program ===== $pageif:6 */

static int bstop( void )
    {                                   /* bstop */
    int sts = btrieve_reset( );
    if (sts != 0)
        fprintf( stderr, "\nBtrieve error %d (resetting)\n", sts );
    else
        {
        unsigned short sz;
        char dummy[128];
        memset( dummy, '\0', sizeof( dummy ) );
        sz = sizeof( dummy );
        sts = btrv( B_STOP, dummy, dummy, &sz, dummy, 0 );
        if (sts != 0)
            fprintf( stderr, "\nBtrieve error %d (removing)\n", sts );
        }
    return sts;
    }                                   /* bstop */
