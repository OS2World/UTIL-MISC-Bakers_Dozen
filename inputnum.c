/* $title:      'INPUTNUM.C ===== Get singel digit user input =====' */
/* $subtitle:   'Elipse Ltd.  (jms)            Revised: 2003 Jan 25' */

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>

static int displayprompt( int argc, char *argv[] );
static int readnumber( int maxchoice );


int main( int argc, char *argv[] )
    {                                   /* main */
    return readnumber( displayprompt( argc, argv ) );
    }                                   /* main */


static int displayprompt( int argc, char *argv[] )
    {                                   /* displayprompt */
    int maxchoice = 10;
    int i;
    for (i = 1;  i < argc;  ++i)
        {
        if (*argv[i] == '/')
            {
            char ch = argv[i][1];
            if (isdigit( ch ))
                maxchoice = ch - '0';
            }
        else
            {
            fputc( ' ', stdout );
            fputs( argv[i], stdout );
            }
        }
    printf( "  [" );
    if (maxchoice < 10)
        printf( "range 0-%d, ", maxchoice );
    printf( "<ESC> to exit]\n" );
    return maxchoice;
    }                                   /* displayprompt */


static int readnumber( int maxchoice )
    {                                   /* readnumber */
    int n = -1;
    fputc( '-', stdout );
    fputc( '\b', stdout );
    fflush( stdout );
    while (1)
        {
        int ch = getch( );
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
            if (isdigit( ch ))
                {
                int k = ch - '0';
                if (k <= maxchoice)
                    {
                    n = k;
                    fputc( ch, stdout );
                    fputc( '\b', stdout );
                    fflush( stdout );
                    }
                }
            }
        }
    }                                   /* readnumber */

