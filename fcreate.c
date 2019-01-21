/* $title:      'FCREATE.C ===== Create empty file or wipe existing =====' */
/* $subtitle:   'Elipse Ltd.  (jms)                  Revised: 1995 Sep 04' */


#include <stdio.h>


/* MAIN ===== Program start up ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    if ( argc > 1 )
        {
        FILE *fp = fopen( argv[1], "w" );
        if (fp == NULL)
            {
            printf( "\nUnable to create file %s\n", argv[1] );
            return 2;                   /* open failed */
            }
        fclose( fp );
        return 0;                       /* success */
        }
    printf( "\nMust specify name of file to create\n" );
    return 3;                           /* no input */
    }
