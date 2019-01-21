/* $title:      'ASCII.C ===== User functions for ASCII Table =====' */
/* $subtitle:   'Elipse Ltd.  [jms]            Revised: 2002 Sep 09' */


#include <stdio.h>
#include <string.h>


int main( int argc, char *argv[] )
    {                                   /* main */
    int i;
    for ( i = 0;  i < 256;  ++i )
        {
        printf( " %3d   %02x   %c\n", i, i, i );
        }
    return 0;
    }                                   /* main */

