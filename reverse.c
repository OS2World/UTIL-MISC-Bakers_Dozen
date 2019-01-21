/* title:       'REVERSE.C ===== Reverse words in file =====' */
/* subtitle:    'Elipse Ltd.  [jms]     Revised: 1999 Oct 28' */


#include <stdio.h>
#include <string.h>

#define MAX_LINE    4090
#define MAX_WORD    255

static void reverse( void );
static void one_line( char *buf );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\n\n\tReverse words in file\n\n" );
    reverse( );
    fprintf( stderr, "\n\n\tProgram complete\n\n" );
    return 0;
    }                                   /* main */


/* REVERSE ===== Body of program ===== $pageif:6 */

static void reverse( void )
    {                                   /* reverse */
    char buf[MAX_LINE + 4];
    memset( buf, '\0', sizeof (buf) );
    while ( gets( buf ) != NULL )
        one_line( buf );
    }                                   /* reverse */


/* ONE_LINE ===== Reverse single line of input ===== $pageif:6 */

static void one_line( char *buf )
    {                                   /* one_line */
    char *p[MAX_WORD + 1];
    int i = 0;
    p[MAX_WORD] = NULL;
    p[i] = strtok( buf, " \t\n" );

    while ( (i < MAX_WORD) && (p[i] != NULL) )
        p[++i] = strtok( NULL, " \t\n" );

    if ( i > 0 )
        {
        while ( --i > 0 )
            printf( "%s ", p[i] );
        printf( "%s\n", p[0] );
        }
    *buf = '\0';
    }                                   /* one_line */
