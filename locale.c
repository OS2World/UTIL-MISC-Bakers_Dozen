/* $title:      'LOCALE.C ===== Test use of locale in date & time=====' */
/* $subtitle:   'Elipse Ltd.  (jms)               Revised: 1995 Aug 02' */


#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <time.h>


int main( int argc, char *argv[] );
void try_set_loc( const char *which );
void print_date( void );


/* MAIN ===== Program start up ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    try_set_loc( NULL );
    print_date( );
    try_set_loc( LC_C );
    print_date( );
    try_set_loc( LC_C_FRANCE );
    print_date( );
    try_set_loc( LC_C_UK );
    print_date( );
    try_set_loc( "" );
    print_date( );

    return 0;
    }                                   /* main */



/* TRY_SET_LOC ===== Call setlocale and show result ===== $pageif:6 */

void try_set_loc( const char *which )
    {                                   /* try_set_loc */
    char *string;
    string = setlocale( LC_ALL, which );
    printf( " \n\n For locale %s ...\n\n", which );
    if ( string != NULL )
        printf( " setlocale returns %s \n", string );
    }                                   /* try_set_loc */


/* PRINT_DATE ===== Show data and time ===== $pageif:6 */

void print_date( void )
    {                                   /* print_date */
    char dest[70];
    size_t ch;
    time_t ltime;
    struct tm *timeptr;

    time( &ltime );                     /* Get the time in seconds */
    timeptr = localtime( &ltime );      /* Convert it to the structure tm */
    printf( " The current date and time are %s", asctime( timeptr ) );

    ch = strftime( dest, sizeof (dest) - 1, "Date: %A, %b %d. Time: %I:%M %p", timeptr );
    printf( " %ld chars in string to make:  %s\n", (unsigned long) ch, dest );
    ch = strftime( dest, sizeof (dest) - 1, "Date: %x. Time: %X", timeptr );
    printf( " %ld chars in string to make:  %s\n", (unsigned long) ch, dest );

    printf( " \n\n" );
    }                                   /* print_date */
