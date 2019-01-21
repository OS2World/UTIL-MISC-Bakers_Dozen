/* $title:      'FMTFIND.C ===== Format output from FIND =====' */
/* $subtitle:   'Elipse Ltd.  [jms]       Revised: 2006 Jan 14' */

#include <stdio.h>
#include <string.h>

#define FALSE       0
#define TRUE        1


int main( int argc, const char *argv[] );
static int fmtfind( void );
static void clean_data( char *buf );
static char *parse_file( const char *buf );
static int parse_line( const char *buf, const char *fname );
static int verify_ref( const char *buf );
static void show_totals( int nlines, int nfiles );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\n\n\t FMTFIND\t---\t (C) Elipse Ltd.  Jan. 2006\n\n" );
    printf( "Cross Reference Results" );
    if (argc > 1)
        printf( " for \"%s\"", argv[1] );
    printf( "\n\n" );
    return (fmtfind( ) == 0);
    }                                   /* main */


/* FMTFIND ===== Main formatting routine ===== $pageif:6 */

static int fmtfind( void )
    {                                   /* fmtfind */
    int nfiles = 0;
    int nlines = 0;
    char buf[4096], *currfile = NULL;
    memset( buf, '\0', sizeof( buf ) );

    while (gets( buf ) != NULL)
        {
        clean_data( buf );
        switch (*buf)
            {
        case '-':
            currfile = parse_file( buf );
            ++nfiles;
            break;
        case '\0':
        case '\n':
        case '\r':
            break;
        default:
            if (parse_line( buf, currfile ))
                ++nlines;
            break;
            }
        }
    show_totals( nlines, nfiles );
    return nfiles;
    }                                   /* fmtfind */


/* CLEAN_DATA ===== Remove control characters ===== $pageif:6 */

static void clean_data( char *buf )
    {                                   /* clean_data */
    while (*buf != '\0')
        {
        if (*buf == '\r')
            {
            *buf = '\0';
            return;
            }
        if (*buf == '\t')
            *buf = ' ';
        ++buf;
        }
    }                                   /* clean_data */


/* PARSE_FILE ===== Get file name from string ===== $pageif:6 */

static char *parse_file( const char *buf )
    {                                   /* parse_file */
    static char fname[512];
    memset( fname, '\0', sizeof( fname ) );

    if (buf != NULL)
        {
        char *p = strchr( buf, ' ' );
        if (p != NULL)
            strncpy( fname, ++p, sizeof( fname ) );
        else
            strncpy( fname, buf, sizeof( fname ) );
        }
    return fname;
    }                                   /* parse_file */


/* PARSE_LINE ===== Output file name and text ===== $pageif:6 */

static int parse_line( const char *buf, const char *fname )
    {                                   /* parse_line */
    if (verify_ref( buf ))
        {
        if (fname != NULL)
            printf( "File \"%s\", ", fname );
        if (buf != NULL)
            printf( "Line %s", buf );
        printf( "\n" );
        return TRUE;
        }
    return FALSE;
    }                                   /* parse_line */


/* VERIFY_REF ===== Must be function ===== $pageif:6 */

static int verify_ref( const char *buf )
    {                                   /* verify_ref */
    char *p = strchr( buf, '(' );
    if (p != NULL)
        {
        char *q = strchr( p, ')' );
        if (q != NULL)
            return TRUE;
        }
    return FALSE;
    }                                   /* verify_ref */


/* SHOW_TOTALS ===== Output final count ===== $pageif:6 */

static void show_totals( int nlines, int nfiles )
    {                                   /* show_totals */
    char *ln[] = { "lines", "line" };
    char *fl[] = { "files", "file" };
    printf( "\n %d %s found in %d %s\n", nlines, ln[nlines == 1], nfiles, fl[nfiles == 1] );
    }                                   /* show_totals */
