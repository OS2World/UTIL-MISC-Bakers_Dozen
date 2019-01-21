/* $title:      'MAKERESP.C ===== Output for linker response file =====' */
/* $subtitle:   'Elipse Ltd.  (jms)                Revised: 2001 Apr 27' */


#include <string.h>
#include <stdio.h>

int main( int argc, char *argv[] );
void makeresp( const char *fname );


/* MAIN ===== Program start up ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    char fname[256];
    fprintf( stderr, "\nMakeResp ===== Make linker response file\n" );
    fprintf( stderr, "(C) Elipse Limited. September, 1995\n\n" );
    memset( fname, '\0', sizeof( fname ) );

    while (scanf( "%s", fname ) > 0)
        makeresp( fname );

    return 0;
    }


/* MAKERESP ===== Body of program ===== $pageif:6 */

void makeresp( const char *fname )
    {                                   /* makeresp */
    const char *q;
    char *p;
    q = strchr( fname, '\\' );
    if (q == NULL)
        q = strchr( fname, '/' );
    if (q == NULL)
        q = fname;
    p = strchr( q, '.' );
    if (p == NULL)
        {
        printf( "%s.Obj +\n", fname );
        }
    else
        {
        if (strnicmp( ++p, "obj", 3 ) == 0)
            printf( "%s +\n", fname );
        }
    }                                   /* makeresp */
