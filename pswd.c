/* PSWD.C */

#include <stdio.h>
#include <string.h>

#define BKSP    8
#define ENTER   10
#define ESC     27


void get_password( char *databuf, unsigned maxlen )
    {
    int i, ch, pos = 0;
    memset( databuf, '\0', maxlen );
    while ( 1 )
        {
        ch = getchar( );
        switch ( ch)
            {
        case ENTER:
            return;
        case ESC:
            while ( pos > 0 )
                {
                databuf[--pos] = '\0';
                printf( "\x08 " );
                }
            break;
        case BKSP:
            databuf[--pos] = '\0';
            printf( "\x08 " );
            break;
        default:
            if ( (ch > 0x20) && (ch < 0x7f) )
                {
                databuf[pos++] = ch;
                printf( "\x08*" );
                }
            }
        }
    }


int main( void )
    {
    char pw[32];
    printf( "\n\nTEST PASSWORD\n\n" );
    printf( "Enter password: " );
    fflush( stdout );
    get_password( pw, 32 );
    printf( "\n\nThe secret password is %s\n\n", pw );
    return 0;
    }
