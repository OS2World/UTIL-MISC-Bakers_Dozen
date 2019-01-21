/* $title:      'SHOWARG.C ===== Display command line arguments =====' */
/* $subtitle:   'Elipse Ltd.  (jms)              Revised: 2008 Dec 27' */


#include <stdlib.h>

/* MAIN ===== Program start up ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int i;
    printf( "\nCommand:" );
    for (i = 0;  i < argc;  ++i)
        printf( " %s", argv[i] );
    printf( "\n" );
    return 0;
    }
