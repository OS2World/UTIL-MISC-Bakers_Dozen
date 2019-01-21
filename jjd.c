/* $title:      'JJD.C ===== Execute java jar file with macros =====' */
/* $subtitle:   'Elipse Ltd.  (jms)             Revised: 2008 Dec 27' */


#include <stdlib.h>
#include <string.h>
#include <process.h>


/* MAIN ===== Program start up ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int i, found = 0;
    char def[512], cmd[512];
    memset( def, '\0', sizeof( def ) );
    memset( cmd, '\0', sizeof( cmd ) );

    for (i = 1;  i < argc;  ++i)
        {
        if (strchr( argv[i], '=' ) != NULL)
            {
            strcat( def, " -D" );
            strcat( def, argv[i] );
            }
        }

    for (i = 1;  i < argc;  ++i)
        {
        if (strchr( argv[i], '=' ) == NULL)
            {
            strcat( cmd, " " );
            strcat( cmd, argv[i] );
            if (!found)
                {
                found = 1;
                if (strchr( argv[i], '.' ) == NULL)
                    strcat( cmd, ".jar" );
                }
            }
        }

    execlp( "java.exe", "java.exe", "-jar", def, cmd, NULL );
    return 0;
    }
