/* $title:      'DLLTEST.C ===== Try out DLLs in OS/2 =====' */
/* $subtitle:   'Elipse Ltd.  [jms]    Revised: 1995 May 06' */


#include <stdio.h>
#include <string.h>

int main( int argc, char *argv[] );
void dlltest( void );

void dll_init( void );
void dll_get( char *buf, int len );
void dll_put( const char *buf );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    printf( "\nDLLtest\n" );
    dlltest( );
    printf( "\nEnd program\n" );
    return 0;
    }                                   /* main */


/* DLLTEST ===== Body of program ===== $pageif:6 */

void dlltest( void )
    {                                   /* dlltest */
    char buf[1024];
    memset( buf, '\0', sizeof( buf ) );
    while (1)
        {
        printf( "\nPress enter to read string\n", buf );
        gets( buf );
        dll_get( buf, sizeof( buf ) );
        printf( "\nReturned string is: %s\n", buf );
        printf( "\nInput replacement string (<ENTER> to quit):\n" );
        gets( buf );
        if ((buf[0] == '\0') || (buf[0] =='*'))
            return;
        dll_put( buf );
        }
    }                                   /* dlltest */
