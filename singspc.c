/* $title:      'SINGSPC.C ===== Remove all extra carriage returns =====' */
/* $subtitle:   'Elipse Ltd.  [jms]                 Revised: 1996 Nov 16' */


#include <stdio.h>

int main( int argc, const char *argv[] );
int singspc( const char *namein, const char *nameout );
void fcopy( FILE *fin, FILE *fout );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\nSINGSPC: Remove extra carriage returns\n" );
    if (argc < 3)
        {
        fprintf( stderr, "\nMust name input and output files\n" );
        return 1;
        }
    return singspc( argv[1], argv[2] );
    }                                   /* main */


/* SINGSPC ===== Body of program ===== $pageif:6 */

int singspc( const char *namein, const char *nameout )
    {                                   /* singspc */
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
    }                                   /* singspc */


/* FCOPY ===== Actual byte copy loop ===== $pageif:6 */

void fcopy( FILE *fin, FILE *fout )
    {                                   /* fcopy */
    int ch, lastcr = 0;
    while ((ch = getc( fin )) != EOF)
        {
        if (ch == '\r')
            {
            if (lastcr)
                lastcr = 0;             /* skip second cr */
            else
                {
                lastcr = 1;             /* note cr */
                putc( ch, fout );       /* and save */
                }
            }
        else
            putc( ch, fout );           /* save normal char */
        }
    }                                   /* fcopy */
