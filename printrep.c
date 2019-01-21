/* $title:      'PRINTREP.C ===== Generic report printer =====' */
/* $subtitle:   'Elipse Ltd.  [jms]       Revised: 1999 Jan 13' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <screen.h>
#include <lprint.h>

#define MAXRECLEN     1536
#define MAX_HEAD      10

static FILE *infile = NULL;
static int orientation = -1;
static int lpi = 0;
static int cpi = 0;
static int line_space = 0;
static int page_length = 0;
static int page_width = 0;
static int match_pos = -1;
static int match_len = 0;
static char *match_val = NULL;
static char head[MAX_HEAD][MAXRECLEN];

int main( int argc, char *argv[] );
static int printrep( const char *filename );
static void show_globals( void );
static void set_print( void );
static void set_globals( void );
static int set_orientation( char orient );
static int set_cpi( void );
static int set_lpi( void );
static int info_record( void );
static void headrep( int pg );
static void bodyloop( void );
static void one_record( char *p );


/* MAIN ===== Start up and call menu ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\n\t\t FORMAT AND PRINT REPORT\n" );
    fprintf( stderr, "\n\t\t Last revision: %s \n", __DATE__ );
    memset( head, '\0', sizeof (head) );
    if ( argc > 1 )
        {
        if (argc > 3)
            {
            match_pos = atoi( argv[2] );
            match_val = argv[3];
            match_len = strlen( match_val );
            if (argc > 4)
                {
                head[MAX_HEAD - 2][0] = '>';
                strcpy( &head[MAX_HEAD - 2][1], argv[4] );
                strcpy( head[MAX_HEAD - 1], ">  " );
                }
            }
        return printrep( argv[1] );
        }
    fprintf( stderr, "\n Must give name file to print\n" );
    fprintf( stderr, "\n Optional arguments:\n" );
    fprintf( stderr, "\t Match position\n" );
    fprintf( stderr, "\t Match value\n" );
    fprintf( stderr, "\t Addional heading\n" );
    return 1;
    }                                   /* main */


/* PRINTREP ===== Body of program ===== $pageif:6 */

static int printrep( const char *filename )
    {                                   /* printrep */
    FILE *pr = NULL;
    infile = fopen( filename, "r" );
    if ( infile == NULL )
        {
        fprintf( stderr, "\n Unable to open file %s\n", filename );
        return 2;
        }
    set_globals( );
    show_globals( );

    if (pr == NULL)
        {
        char *p = getenv( "TRACE" );
        if (p != NULL)
            pr = fopen( p, "w" );
        }
    if (pr == NULL)
        {
        char *p = getenv( "PRN" );
        if (p != NULL)
            pr = fopen( p, "w" );
        }
    if (pr == NULL)
        pr = fopen( "PRN", "w" );
    if (pr == NULL)
        pr = stdout;

    lprint_open( pr, page_length, page_width, 1, headrep );
    set_print( );
    bodyloop( );
    lprint_close( NULL );
    fclose( infile );
    return 0;
    }                                   /* printrep */


/* SHOW_GLOBALS ===== From screen input ===== $pageif:6 */

static void show_globals( void )
    {                                   /* show_globals */
    static const char *fmtn = "%24s =%4d\n";
    static const char *fmta = "%24s = %-s\n";
    static const char *pl[] = { "Portrait", "Landscape", "" };
    int i;
    printf( "\n Globals:\n" );
    printf( fmta, "orientation", pl[orientation] );
    printf( fmtn, "lpi", lpi );
    printf( fmtn, "cpi", cpi );
    printf( fmtn, "line_space", line_space );
    printf( fmtn, "page_length", page_length );
    printf( fmtn, "page_width", page_width );
    printf( fmtn, "match_pos", match_pos );
    printf( fmtn, "match_len", match_len );
    if (match_val != NULL)
        printf( fmta, "match_val", match_val );

    printf( " Headings:\n" );
    for ( i = 0;  i < MAX_HEAD;  ++i )
        {
        if (*head[i] >= ' ')
            printf( "%2d) %s\n", i, head[i] );
        }
    printf( "\n" );
    }                                   /* show_globals */


/* SET_PRINT ===== Control codes to set up printing ===== $pageif:6 */

static void set_print( void )
    {                                   /* set_print */
    lprint_reset( );
    lprint_orient( orientation );
    lprint_proportional( OFF );
    lprint_lpi( lpi );
    lprint_cpi( cpi );
    }                                   /* set_print */


/* SET_GLOBALS ===== From screen input ===== $pageif:6 */

static void set_globals( void )
    {                                   /* set_globals */
    char buf[MAXRECLEN];
    orientation = PORTRAIT;
    page_width = 80;
    page_length = 58;
    line_space = 0;
    memset( buf, '\0', sizeof (buf) );

    while ( info_record( ) )
        {
        if ( fgets( buf, sizeof (buf), infile ) != NULL )
            {
            int i = buf[0] - '1';
            if ( i < 0 )                /* spacing */
                {
                char str[128];
                memset( str, '\0', sizeof( str ) );
                sscanf( &buf[1], "%d%d%d%s", &page_length, &page_width, &line_space, str );
                orientation = set_orientation( *str );
                }
            if ( (i >= 0) && (i < MAX_HEAD) )   /* heading */
                strcpy( head[i], &buf[1] );
            }
        }
    cpi = set_cpi( );
    lpi = set_lpi( );
    }                                   /* set_globals */


/* SET_ORIENTATION ===== Based on input ===== $pageif:6 */

static int set_orientation( char orient )
    {                                   /* set_orientation */
    char chr = toupper( orient );
    if (chr == 'P')
        return PORTRAIT;
    if (chr == 'L')
        return LANDSCAPE;
    if (page_length > 70)
        return PORTRAIT;
    if (page_length < 52)
        return LANDSCAPE;
    if (page_width < 114)
        return PORTRAIT;
    return LANDSCAPE;
    }                                   /* set_orientation */


/* SET_CPI ===== Based on orientation & page_width===== $pageif:6 */

static int set_cpi( void )
    {                                   /* set_cpi */
    if (orientation == PORTRAIT)
        {
        if (page_width > 100)
            return 16;
        if (page_width > 83)
            return 12;
        }
    if (orientation == LANDSCAPE)
        {
        if (page_width > 130)
            return 16;
        if (page_width > 108)
            return 12;
        }
    return 10;
    }                                   /* set_cpi */


/* SET_LPI ===== Based on orientation & page_length===== $pageif:6 */

static int set_lpi( void )
    {                                   /* set_lpi */
    if (orientation == PORTRAIT)
        if (page_length > 66)
            return 8;
    if (orientation == LANDSCAPE)
        if (page_length > 51)
            return 8;
    return 6;
    }                                   /* set_lpi */


/* INFO_RECORD ===== Is next record informational ===== $pageif:6 */

static int info_record( void )
    {                                   /* info_record */
    int ch = fgetc( infile );
    if ( ch < ' ' )
        return 1;
    ungetc( ch, infile );
    return 0;
    }                                   /* info_record */


/* HEADREP ===== Report page heading ===== $pageif:6 */

static void headrep( int pg )
    {                                   /* headrep */
    int i;
    for ( i = 0;  i < MAX_HEAD;  ++i )
        {
        switch ( head[i][0])
            {
        case '\0':
            break;
        case '>':
            lprint_centre( &head[i][1] );
            break;
        case '_':
        case '-':
        case '=':
            lprint_repeat( '_', page_width - 1 );
            lprint( "\n" );
            break;
        default:
            lprint( "%s", head[i] );
            }
        }
    lprint( "\n" );
    }                                   /* headrep */


/* BODYLOOP ===== Main loop of program ===== $pageif:6 */

static void bodyloop( void )
    {                                   /* bodyloop */
    char buf[MAXRECLEN];
    memset( buf, '\0', sizeof (buf) );

    while ( fgets( buf, sizeof (buf), infile ) != NULL )
        {
        if ( (match_pos < 0) || (match_len <= 0) || (match_val == NULL) )
            one_record( buf );
        else
            if (strncmp( &buf[match_pos], match_val, match_len ) == 0)
                one_record( buf );
        }
    }                                   /* bodyloop */


/* ONE_RECORD ===== Print single record ===== $pageif:6 */

static void one_record( char *p )
    {                                   /* one_record */
    char *q;
    if ( *p > 'z' )
        p += 2;
    while ( (q = strchr( p, '|' )) != NULL )
        {
        *q = '\0';
        lprint( "%s\n", p );
        p = q + 1;
        }
    lprint( p );                    /* trailing LF is assumed */
    if ( (line_space > 0) && (!lprint_pageif( line_space )) )
        lprint_line( line_space );
    }                                   /* one_record */
 