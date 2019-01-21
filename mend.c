/* $title:      'MEND.C ===== Put parts together into one file =====' */
/* $subtitle:   'Elipse Ltd.  [jms]             Revised: 1997 Apr 03' */


#include <stdio.h>

int  main( int argc, char *argv[] );
void usage( void );
int  mend( FILE *tofile, char *namelist[], int lo, int hi );
void copypart( FILE *tofile, FILE *infile );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    FILE *tofile = NULL;
    int ret;
    fprintf( stderr, "\nMEND ===== (C) 1997, Elipse Ltd.\n" );
    if (argc < 4)
        {
        fprintf( stderr, "\nToo few parameters\n" );
        usage( );
        return 1;
        }
    fprintf( stderr, "\nWriting to %s\n", argv[1] );
    tofile = fopen( argv[1], "wb" );
    if (tofile == NULL)
        {
        fprintf( stderr, "\nCannot open output file %s\n", argv[1] );
        usage( );
        return 2;
        }
    ret = mend( tofile, argv, 2, argc - 1 );
    fclose( tofile );
    if (!ret)
        {
        fprintf( stderr, "\nError copying file\n", argv[1] );
        return 3;
        }
    fprintf( stderr, "\nEnd program\n" );
    return 0;
    }                                   /* main */


/* USAGE ===== Show correct syntax ===== $pageif:6 */

void usage( void )
    {                                   /* usage */
    fprintf( stderr, "\nCorrect usage:" );
    fprintf( stderr, "\nmend destfile src1 scr2 [scr3] [...]\n" );
    fprintf( stderr, "\nUse with program slice\n" );
    }                                   /* usage */


/* MEND ===== Body of program ===== $pageif:6 */

int mend( FILE *tofile, char *namelist[], int lo, int hi )
    {                                   /* mend */
    int tf;
    for (tf = lo;  tf <= hi;  ++tf)
        {
        FILE *infile;
        fprintf( stderr, "Reading from %s\n", namelist[tf] );
        infile = fopen( namelist[tf], "rb" );
        if (infile == NULL)
            {
            fprintf( stderr, "\nCannot open input file %s\n", namelist[tf] );
            return 0;                   /* failure */
            }
        copypart( tofile, infile );
        fclose( infile );
        }
    return 1;                           /* all output files full */
    }                                   /* mend */


/* COPYPART ===== Copy some of infile to outfile ===== $pageif:6 */

void copypart( FILE *tofile, FILE *infile )
    {                                   /* copypart */
    int ch;
    while ((ch = fgetc( infile )) != EOF)
        fputc( ch, tofile );
    }                                   /* copypart */
