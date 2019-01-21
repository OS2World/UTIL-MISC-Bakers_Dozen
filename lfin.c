/* $title:      'LFIN.C ===== Insert all line feeds to file =====' */
/* $subtitle:   'Elipse Ltd.  [jms]          Revised: 1996 Oct 30' */


#include <stdio.h>

int main( int argc, const char *argv[] );
int lfout( const char *namein, const char *nameout );
void fcopy( FILE *fin, FILE *fout );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\nLFIN: Insert line feeds\n" );
    if (argc < 3)
        {
        fprintf( stderr, "\nMust name input and output files\n" );
        return 1;
        }
    return lfout( argv[1], argv[2] );
    }                                   /* main */


/* LFOUT ===== Body of program ===== $pageif:6 */

int lfout( const char *namein, const char *nameout )
    {                                   /* lfout */
    FILE *from, *to;
    if ((from = fopen( namein, "rb" )) == NULL)
        {
        fprintf( stderr, "\nCannot open input file: %s\n", namein );
        return 2;
        }
    if ((to = fopen( nameout, "wb" )) == NULL)
        {
        fprintf( stderr, "\nCannot open output file: %s\n", nameout );
        return 3;
        }

    fcopy( from, to );
    fclose( to );
    fclose( from );

    return 0;                           /* success */
    }                                   /* lfout */


/* FCOPY ===== Actual byte copy loop ===== $pageif:6 */

void fcopy( FILE *fin, FILE *fout )
    {                                   /* fcopy */
    int ch;
    while ((ch = getc( fin )) != EOF)
        {
        putc( ch, fout );
        if (ch == '\r')
            putc( '\n', fout );
        }
    }                                   /* fcopy */
