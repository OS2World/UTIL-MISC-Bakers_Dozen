/* $title:      'LP.C ===== Line Print given file(s) =====' */
/* $subtitle:   'Elipse Ltd.  [jms]   Revised: 2005 Dec 11' */


/* NOTE: For wildcard expansion to work from the command line, */
/*       Using CSet++ ...                                      */
/*              Link with SETARGV.OBJ                          */
/*       Using Zortech C++ ...                                 */
/*              Link with _MAINS.OBJ                           */
/*       Using Watcom C++ ...                                  */
/*              Link with c:\watcom\src\startup\WILDARGV.C     */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <lprint.h>

#define NORM_VERT   6
#define NORM_HORIZ  0

static FILE *dout = NULL;               /* printer device or file */
static char input_file[256];            /* name of input */
static struct
    {
    int noprint;
    int landscape;
    int tray;
    int lpi;
    int cpi;
    int duplex;
    int lines;
    int pages;
    int maxwidth;
    char output[256];
    } flag;

int  main( int argc, const char *argv[] );
void init( void );
void lp( const char *infil );
int  count( FILE *input );
void print( FILE *input, int lwidth );
void heading( int pageno );
int  prt_setup( int lwidth );
int  open_output( void );
int  set_flag( const char *flarg );
void show_flag( void );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    int i;
    fprintf( stderr, "\n\n\t LP\t---\t (C) Elipse Ltd.  Dec. 2005\n\n" );
    init( );

    if ( argc < 2 )
        {
        show_flag( );
        return 1;
        }

    for (i = 1;  i < argc;  ++i)        /* check all flags */
        if ((*argv[i] == '/') || (*argv[i] == '-'))
            if (set_flag( argv[i] ))
                return 2;
    if (!open_output( ))                /* open output */
        return 3;

    for (i = 1;  i < argc;  ++i)        /* output all files */
        if ((*argv[i] != '/') && (*argv[i] != '-'))
            lp( argv[i] );
    fprintf( stderr, "\n" );
    fclose( dout );
    return 0;
    }                                   /* main */


/* INIT ===== Initialize data ===== $pageif:6 */

void init( void )
    {                                   /* init */
    char *pev = getenv( "PRN" );
    memset( input_file, '\0', sizeof( input_file ) );
    memset( &flag, '\0', sizeof( flag ) );
    flag.lpi = NORM_VERT;
    flag.cpi = NORM_HORIZ;
    flag.duplex = DUPLEX_LONG;
    if ( pev != NULL )
        strcpy( flag.output, pev );
    else
        strcpy( flag.output, "PRN" );
    }                                   /* init */


/* LP ===== Main printing routine ===== $pageif:6 */

void lp( const char *infil )
    {                                   /* lp */
    FILE *fp = NULL;
    if ( infil == NULL )
        return;                         /* no name */
    if ( *infil == '\0' )
        return;                         /* no name */

    if ( (fp = fopen( infil, "rb" )) == NULL )
        fprintf( stderr, "\n\t ERROR: Cannot open file %s\n", infil );
    else
        {
        int lw;
        strupr( strcpy( input_file, infil ) );
        fprintf( stderr, "\nPrinting file %s\n", input_file );
        lw = count( fp );
        if (prt_setup( lw ))
            print( fp, lw );
        fclose( fp );
        }
    }                                   /* lp */


/* COUNT ===== Main counting routine ===== $pageif:6 */

int count( FILE *input )
    {                                   /* count */
    int maxwidth = 0;                   /* size of widest line */
    int curwidth = 0;                   /* size of current line */
    int numlines = 0;                   /* total number of lines */
    int numpages = 0;                   /* total number of form-feeds */
    int ch = 0;                         /* current character */

    while ( (ch = getc( input )) != EOF )
        {
        switch ( ch)
            {
        case '\t':                      /* tab */
            curwidth += 4;              /* increase to 4 spaces */
            break;
        case '\n':                      /* new line */
            if ( curwidth > maxwidth )
                maxwidth = curwidth;
            curwidth = 0;               /* reset for ext line */
            numlines++;                 /* increment count */
            break;
        case '\f':                      /* new page */
            numpages++;                 /* increment count */
            break;
        default:                        /* normal character */
            ++curwidth;
            }
        }

    rewind( input );
    flag.maxwidth = maxwidth;
    flag.lines = numlines;
    flag.pages = numpages;
    return maxwidth;
    }                                   /* count */


/* PRINT ===== Output to printer ===== $pageif:6 */

void print( FILE *input, int lwidth )
    {                                   /* print */
    char *buf = NULL;
    int siz = lwidth + 4;
    if ( lwidth == 0 )
        {
        fprintf( stderr, "\nInternal error! (line width)\n" );
        return;
        }
    buf = malloc( siz + 4 );
    if ( buf == NULL )
        {
        fprintf( stderr, "\nInternal error! (out of mem)\n" );
        return;
        }

    memset( buf, '\0', (siz + 2) );
    while ( fgets( buf, siz, input ) )
        lprint( "%s", buf );

    lprint_close( NULL );
    rewind( input );
    free( buf );
    }                                   /* print */


/* HEADING ===== Heading on each page ===== $pageif:6 */

void heading( int pageno )
    {                                   /* heading */
    lprint_weight( +3 );
    lprint_centre( input_file );
    lprint_weight( -1 );
    lprint_line( 2 );
    }                                   /* heading */


/* PRT_SETUP ===== Set printer to desired configuration ===== $pageif:6 */

int prt_setup( int lwidth )
    {                                   /* prt_setup */
    int lmarg = 0;                      /* left side margin (char) */
    int tmarg = 3;                      /* top margin (lines) */
    int txlen = 58;                     /* text length (lines) */
    int dinch = 81;                     /* page width (10 x inch) */
    int pitch = 12;                     /* char/inch horizontal */
    int width = lwidth;                 /* print width (char) */

    if (flag.tray == TRAY_LOWER)
        {
        if (flag.landscape)             /* lower tray, landscape */
            {
            lmarg = 0;
            tmarg = flag.lpi;
            txlen = 8 * flag.lpi - tmarg - 1;
            dinch = 106;
            }
        else                            /* lower tray, portrait */
            {
            lmarg = 1;
            tmarg = (flag.lpi + 1) / 2;
            txlen = 10 * flag.lpi - 2;
            dinch = 76;
            }
        }
    else
        {
        if (flag.landscape)             /* upper tray, landscape */
            {
            lmarg = 0;
            tmarg = (flag.lpi + 1) / 2;
            txlen = 8 * flag.lpi - tmarg - 1;
            dinch = 106;
            }
        else                            /* upper tray, portrait */
            {
            lmarg = 0;
            tmarg = (flag.lpi + 1) / 2;
            txlen = 10 * flag.lpi - 2;
            dinch = 81;
            }
        }

    if (flag.cpi > 0)
        {
        pitch = flag.cpi;
        lwidth = ((dinch * pitch) / 10) - 10;
        }
    else
        {
        pitch = 10 * (lwidth + 12) / dinch;
        if (pitch < 12)
            {
            if (pitch < 6)
                pitch = 10;
            else
                pitch = 12;
            }
        }
    width = (dinch * pitch) / 10;
    if (lwidth < width / 2)
        lwidth = width / 2;

    if (lmarg)
        lmarg = (width + pitch - lwidth + 1) / 2;
    else
        lmarg = (width - lwidth + 1) / 2;

    if (flag.noprint)
        {
        int npages = flag.pages + (flag.lines / txlen) + 1;
        fprintf( stderr, "%12d  line width\n", flag.maxwidth );
        fprintf( stderr, "%12d  top margin\n", tmarg );
        fprintf( stderr, "%12d  text length\n", txlen );
        fprintf( stderr, "%12d  pitch\n", pitch );
        fprintf( stderr, "%12d  page width\n", width );
        fprintf( stderr, "%12d  text width\n", lwidth );
        fprintf( stderr, "%12d  left margin\n", lmarg );
        fprintf( stderr, "%12d  page(s) approx\n", npages );
        fprintf( stderr, "%12s  output device\n", flag.output );
        fprintf( stderr, "\n" );
        return 0;                       /* do not continue */
        }

    lprint_open( dout, txlen, lwidth+1, 1, heading );
    lprint_reset( );
    if (flag.landscape)
        lprint_orient( LANDSCAPE );
    else
        lprint_orient( PORTRAIT );
    switch (flag.tray)
        {
    case 1:
        lprint_bin( TRAY_UPPER );
        break;
    case 2:
        lprint_bin( TRAY_LOWER );
        break;
    case 3:
        lprint_bin( TRAY_MULTI );
        break;
        }
    lprint_lpi( flag.lpi );
    lprint_duplex( flag.duplex );
    lprint_weight( -1 );
    lprint_proportional( OFF );
    lprint_cpi( pitch );
    lprint_margin_top( tmarg );
    lprint_margin_left( lmarg );
    return 1;                           /* continue */
    }                                   /* prt_setup */


/* OPEN_OUTPUT ===== As given or PRN ===== $pageif:6 */

int open_output( void )
    {                                   /* open_output */
    dout = NULL;
    if (*flag.output > ' ')
        dout = fopen( flag.output, "w" );
    else
        dout = fopen( "PRN", "w" );
    if (dout == NULL)
        {
        fprintf( stderr, "\nInvalid output file or device %s\n", flag.output );
        return 0;                       /* do not continue */
        }
    return 1;                           /* continue */
    }                                   /* open_output */


/* SET_FLAG ===== Parse parameter and set ===== $pageif:6 */

int set_flag( const char *flarg )
    {                                   /* set_flag */
    const char *p = flarg;
    int quit = 0;
    int temp = 0;

    switch (toupper( *(++p) ))
        {
        case '?':                       /* Help */
            show_flag( );
            quit = 1;
            break;
        case 'N':                       /* No print */
            flag.noprint = 1;
            break;
        case 'P':                       /* Portrait */
            flag.landscape = 0;
            break;
        case 'L':                       /* Landscape */
            flag.landscape = 1;
            break;
        case 'T':                       /* Tray (1, 2, 3) */
            temp = atoi( ++p );
            if ((temp < 1) || (temp > 3))
                temp = TRAY_UPPER;
            flag.tray = temp;
            break;
        case 'D':                       /* Duplex (0, 1, 2) */
            temp = atoi( ++p );
            if ((temp < 0) && (temp > 2))
                temp = DUPLEX_LONG;
            flag.duplex = temp;         /* use given */
            break;
        case 'V':                       /* Vertical spacing (1-16) */
            temp = atoi( ++p );
            if ((temp < 1) || (temp > 16))
                temp = NORM_VERT;
            flag.lpi = temp;
            break;
        case 'H':                       /* Horizontal spacing (4-24) */
            temp = atoi( ++p );
            if ((temp < 4) || (temp > 24))
                temp = NORM_HORIZ;
            flag.cpi = temp;
            break;
        case 'O':                       /* Output device or file name */
            strcpy( flag.output, ++p );
            break;
        default:
            fprintf( stderr, "\nInvalid switch %s\n", flarg );
            quit = 1;
        }
    return quit;
    }                                   /* set_flag */


/* SHOW_FLAG ===== List all choices ===== $pageif:6 */

void show_flag( void )
    {                                   /* show_flag */
    fprintf( stderr, "          USAGE:  lp [switches] filename [filename] ...\n\n" );
    fprintf( stderr, "/?        Help (show this screen)\n" );
    fprintf( stderr, "/N        Report results, but do not print\n" );
    fprintf( stderr, "/P        Portrait orientation (default)\n" );
    fprintf( stderr, "/L        Landscape orientation\n" );
    fprintf( stderr, "/T1       Primary paper tray (default)\n" );
    fprintf( stderr, "/T2       Secondary paper tray\n" );
    fprintf( stderr, "/T3       Auxillary paper tray\n" );
    fprintf( stderr, "/D0       Duplex off\n" );
    fprintf( stderr, "/D1       Duplex Long edge bind (default)\n" );
    fprintf( stderr, "/D2       Duplex Short edge bind\n" );
    fprintf( stderr, "/V<n>     Vertical spacing at n lines/inch (default=6)\n" );
    fprintf( stderr, "/H<n>     Horizontal spacing at n characters/inch (default=variable)\n" );
    fprintf( stderr, "/O<name>  Device for output is <name> (default=PRN)\n" );
    }                                   /* show_flag */
