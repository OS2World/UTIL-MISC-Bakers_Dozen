/* $title:      'SLICE.C ===== Cut file into multiple parts =====' */
/* $subtitle:   'Elipse Ltd.  [jms]          Revised: 1997 Apr 03' */

#include <GL/gl.h>
#include <stdio.h>

int  main( int argc, char *argv[] );
void usage( void );
int  slice( FILE *infile, char *namelist[], int lo, int hi );
int  copypart( FILE *tofile, FILE *infile, long outsize );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    FILE *infile = NULL;
    int ret;
    fprintf( stderr, "\nSLICE ===== (C) 1997, Elipse Ltd.\n" );
    if (argc < 4)
        {
        fprintf( stderr, "\nToo few parameters\n" );
        usage( );
        return 1;
        }
    fprintf( stderr, "\nReading from %s\n", argv[1] );
    infile = fopen( argv[1], "rb" );
    if (infile == NULL)
        {
        fprintf( stderr, "\nCannot open input file %s\n", argv[1] );
        usage( );
        return 2;
        }
    ret = slice( infile, argv, 2, argc - 1 );
    fclose( infile );
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
    fprintf( stderr, "\nslice sourcefile dest1 dest2 [dest3] [...]\n" );
    fprintf( stderr, "\nUse with program mend\n" );
    }                                   /* usage */


/* SLICE ===== Body of program ===== $pageif:6 */

int slice( FILE *infile, char *namelist[], int lo, int hi )
    {                                   /* slice */
    long insize, outsize;
    int numf, tf;

    numf = hi - lo + 1;
    fseek( infile, 0L, SEEK_END );
    insize = ftell( infile );
    fseek( infile, 0L, SEEK_SET );
    outsize = insize / (long) numf;

    for (tf = lo;  tf <= hi;  ++tf)
        {
        FILE *tofile;
        int ret;
        fprintf( stderr, "Writing to %s\n", namelist[tf] );
        tofile = fopen( namelist[tf], "wb" );
        if (tofile == NULL)
            {
            fprintf( stderr, "\nCannot open output file %s\n", namelist[tf] );
            return 0;                   /* failure */
            }
        ret = copypart( tofile, infile, outsize );
        fclose( tofile );
        if (ret)
            return 1;                   /* end of input file */
        }
    return 1;                           /* all output files full */
    }                                   /* slice */


/* COPYPART ===== Copy some of infile to outfile ===== $pageif:6 */

int copypart( FILE *tofile, FILE *infile, long outsize )
    {                                   /* copypart */
    long count = 0L;
    int ch;

    while ((ch = fgetc( infile )) != EOF)
        {
        fputc( ch, tofile );
        if (++count > outsize)
            return 0;                   /* split point reached */
        }
    return 1;                           /* end of input file */
    }                                   /* copypart */
