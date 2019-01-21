/* $title:      'INPUTCHR.C ===== Get singel digit user input =====' */
/* $subtitle:   'Elipse Ltd.  (jms)            Revised: 2003 Jan 25' */

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>

static char *displayprompt( int argc, char *argv[] );
static int readcharacter( const char *choices );


int main( int argc, char *argv[] )
    {                                   /* main */
    return readcharacter( displayprompt( argc, argv ) );
    }                                   /* main */


static char *displayprompt( int argc, char *argv[] )
    {                                   /* displayprompt */
    static char *choices = NULL;
    int i;
    for (i = 1;  i < argc;  ++i)
        {
        if (*argv[i] == '/')
            choices = strupr( ++argv[i] );
        else
            {
            fputc( ' ', stdout );
            fputs( argv[i], stdout );
            }
        }
    printf( "  [" );
    if (choices != NULL)
        printf( "select from %s, ", choices );
    printf( "<ESC> to exit]\n" );
    return choices;
    }                                   /* displayprompt */


static int readcharacter( const char *choices )
    {                                   /* readcharacter */
    int n = -1;
    char *p = NULL;
    fputc( '-', stdout );
    fputc( '\b', stdout );
    fflush( stdout );
    if (choices == NULL)
        choices = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    while (1)
        {
        int ch = toupper( getch( ) );
        switch (ch)
            {
        case 0:                         /* function */
            ch = getch( );
            break;
        case 27:                        /* escape */
            fputc( ' ', stdout );
            fflush( stdout );
            return 127;
        case 13:                        /* return */
            if (n >= 0)
                return n;
        default:                        /* letter or digit */
            p = strchr( choices, ch );
            if (p != NULL)
                {
                n = p - choices;
                fputc( ch, stdout );
                fputc( '\b', stdout );
                fflush( stdout );
                }
            }
        }
    }                                   /* readcharacter */

