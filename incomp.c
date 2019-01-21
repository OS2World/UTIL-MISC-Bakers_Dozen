/* $title:      'INCOMP.C ===== Show volume label of given disk =====' */
/* $subtitle:   'Elipse Ltd.  [jms]              Revised: 1999 Jan 11' */


#include <stdio.h>
#include <string.h>

int main( int argc, const char *argv[] );
int incomp( int argc, const char *argv[] );



/* MAIN ===== Starting point for program ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {
    if (incomp( argc, argv ))
        return 0;                       /* match */
    else
        return 127;                     /* no match */
    }


/* INCOMP ===== Body of program ===== $pageif:6 */

int incomp( int argc, const char *argv[] )
    {                                   /* incomp */
    int i;
    char buf[260];
    memset( buf, '\0', sizeof( buf ) );
    for ( i = 1;  i < argc;  ++i )
        {
        if (argv[i] == NULL)
            break;                      /* end of list */
        if (scanf( "%s", buf ) < 1)
            return 0;                   /* failed; no input */
        if (stricmp( buf, argv[i] ) != 0)
            return 0;                   /* failed; not equal */
        }
    return 1;                           /* all matched */
    }                                   /* incomp */
