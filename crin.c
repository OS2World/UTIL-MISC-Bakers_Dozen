/* $title:      'CRIN.C ===== Insert all carriage returns to file =====' */
/* $subtitle:   'Elipse Ltd.  [jms]                Revised: 1996 Oct 30' */


#include <stdio.h>

int main( int argc, const char *argv[] );
int crout( const char *namein, const char *nameout );
void fcopy( FILE *fin, FILE *fout );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\nCRIN: Insert carriage returns\n" );
    if (argc < 3)
        {
        fprintf( stderr, "\nMust name input and output files\n" );
        return 1;
        }
    return crout( argv[1], argv[2] );
    }                                   /* main */


/* CROUT ===== Body of program ===== $pageif:6 */

int crout( const char *namein, const char *nameout )
    {                                   /* crout */
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
    }                                   /* crout */


/* FCOPY ===== Actual byte copy loop ===== $pageif:6 */

void fcopy( FILE *fin, FILE *fout )
    {                                   /* fcopy */
    int ch;
    while ((ch = getc( fin )) != EOF)
        {
        if (ch == '\n')
            putc( '\r', fout );
        putc( ch, fout );
        }
    }                                   /* fcopy */
