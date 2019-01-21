/* $title:      'TRIM.C ===== Remove trailing blanks =====' */
/* $subtitle:   'Elipse Ltd.  [jms]   Revised: 2008 May 05' */


#include <stdio.h>
#include <string.h>

#define LINESIZE    2048

int main( int argc, const char *argv[] );
int trim( const char *namein, const char *nameout );
void fcopy( FILE *fin, FILE *fout );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\nTRIM: Remove trailing blanks - (C) Elipse Ltd. May 2008\n" );
    if ( argc < 3 )
        {
        fprintf( stderr, "\nMust name input and output files\n" );
        return 1;
        }
    return trim( argv[1], argv[2] );
    }                                   /* main */


/* TRIM ===== Body of program ===== $pageif:6 */

int trim( const char *namein, const char *nameout )
    {                                   /* trim */
    FILE *from, *to;
    if ( (from = fopen( namein, "r" )) == NULL )
        {
        fprintf( stderr, "\nCannot open input file: %s\n", namein );
        return 2;
        }
    if ( (to = fopen( nameout, "w" )) == NULL )
        {
        fprintf( stderr, "\nCannot open output file: %s\n", nameout );
        return 3;
        }

    fcopy( from, to );
    fclose( to );
    fclose( from );

    return 0;                           /* success */
    }                                   /* trim */


/* FCOPY ===== Actual copy loop ===== $pageif:6 */

void fcopy( FILE *fin, FILE *fout )
    {                                   /* fcopy */
    int n;
    char buf[LINESIZE];
    memset( buf, '\0', sizeof (buf) );

    while ( fgets( buf, sizeof (buf), fin ) != NULL )
        {
        n = strlen( buf );
        while ( (--n > 0) && ((buf[n] == ' ') || (buf[n] == '\n')) )
            buf[n] = '\0';
        fprintf( fout, "%s\n", buf );
        }
    }                                   /* fcopy */
