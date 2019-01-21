/* title:       'KEYS.C ===== Determine keyboard codes =====' */
/* subtitle:    'Elipse Ltd.  [jms]     Revised: 1992 Aug 27' */

#include <stdio.h>

/* MAIN ===== Starting point ===== $pageif:6 */

int main(int argc, char *argv[])
    {                                   /* main */
    int ch = 0;
    fprintf( stderr, "\nKEYS ===== Determine Key Codes\n" );
    printf( "\nBegin keyboard_input\n" );
    printf( "\nUse <ESC> to quit\n\n" );

    while ( (ch = getch( )) != 27 )
        printf( "Decimal %3d; hex %02x; alpha ---%c---\n", ch, ch, ch );

    fprintf( stderr, "\nEnd of Program\n" );
    }                                   /* main */

