/* $title:      'FULLPATH.C ===== Complete file path for given =====' */
/* $subtitle:   'Elipse Ltd.  [jms]             Revised: 2002 Aug 29' */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


int main( int argc, char *argv[] );
char *fullpath( char *path_var, char *file_name );


/* MAIN ===== Starting point for program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {
    if (argc < 3)
        {
        fprintf( stderr, "Require two inputs:\n" );
        fprintf( stderr, " - environment variable to search (e.g. PATH, or CLASSPATH)\n" );
        fprintf( stderr, " - file name to search for (e.g. epm.exe, or space.bmp)\n" );
        return 1;
        }
    printf( fullpath( argv[1], argv[2] ) );
    return 0;
    }


/* FULLPATH ===== Body of program ===== $pageif:6 */

char *fullpath( char *path_var, char *file_name )
    {                                   /* fullpath */
    static char buf[260];
    memset( buf, '\0', sizeof( buf ) );
    _searchenv( file_name, path_var, buf );
    return buf;
    }                                   /* fullpath */
