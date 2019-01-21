/* PARENT.C */

 
#include <stdio.h>
#include <stdlib.h>


int main( int argc, char *argv[] )
    {
    int rc;
    printf( "\nBegin PARENT\n" );
    if ( argc < 2 )
        {
        printf( "\nMust give command to execute\n" );
        }

    rc = system( argv[1] );
    printf( "\nReturn Code %d\n", rc );
    printf( "\nDone\n" );
    return 0;
    }
