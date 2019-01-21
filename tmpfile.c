/* $title:      'TMPFILE.C ==== Test ANSI call to make temporary file =====' */
/* $subtitle:   'Elipse Ltd.  [jms]         		   Revised: 1997 Feb 27' */


#include <stdio.h>
#include <string.h>
#include <conio.h>


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int i;
    FILE *fp = NULL;
    fprintf( stderr, "\nTMPFILE\n" );
    fp = tmpfile( );
    if (fp == NULL)
        {
        printf( "\nCreate temporary file failed!\n" );
        return 1;
        }
    for (i = 0;  i < argc;  ++i)
        printf( "%s/n", argv[i] );
    printf( "\nFile created and has data\n" );
    printf( "\nPress any key to continue ... \n" );
    getch( );
    fclose( fp );
    printf( "\nFile is closed\n" );
    printf( "\nPress any key to end ... \n" );
    getch( );
    return 0;
    }                                   /* main */

