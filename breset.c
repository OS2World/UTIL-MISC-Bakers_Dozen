/* $title:      'BRESET.C ===== Reset all locks for user =====' */
/* $subtitle:   'Elipse Ltd.  [jms]       Revised: 1999 Mar 26' */


#include <stdio.h>
#include <bfile.h>


/* MAIN ===== Start up ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int st;
    fprintf( stderr, "\n\t\t RESET ALL BTRIEVE LOCKS\n" );
    fprintf( stderr, "\n\t\t BUT ONLY FOR THIS USER ID\n" );
    fprintf( stderr, "\n\t\t Last revision: %s\n", __DATE__ );
    st = btrieve_reset( );
    if (st != 0)
        fprintf( stderr, "\nBtrieve error %d, resetting files!\n", st );
    else
        fprintf( stderr, "\nReset successful\n" );
    return st;
    }                                   /* main */
