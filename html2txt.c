/* $title:      'HTML2TXT.C ===== Remove HTML tags from file =====' */
/* $subtitle:   'Elipse Ltd.  [jms]           Revised: 1999 Mar 18' */


#include <stdio.h>

#define CHAR_START  '<'
#define CHAR_STOP   '>'

int main( int argc, const char *argv[] );
int html2txt( void );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\nHTML2TXT: Remove HTML tags\n" );
    fprintf( stderr, "\n(C) 1999, Elipse Limited\n" );
    return html2txt( );
    }                                   /* main */


/* HTML2TXT ===== Body of program ===== $pageif:6 */

int html2txt( void )
    {                                   /* html2txt */
    int level, i, ch;
    level = i = ch = 0;

    while (level >= 0)
        {
        ch = getchar( );
        if (ch == EOF)
            return 0;
        if (ch == CHAR_START)
            ++level;
        if (level == 0)
            putchar( ch );
        if (ch == CHAR_STOP)
            --level;
            }

    fprintf( stderr, "\nNesting error just prior to...\n" );
    while ((++i < 60) && (ch != EOF))
        {
        putchar( ch );
        ch = getchar( );
        }
    return 1;
    }                                   /* html2txt */
