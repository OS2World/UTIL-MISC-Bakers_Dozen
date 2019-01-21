/* $title:      'OPENDEF.C ===== Reformat index file =====' */
/* $subtitle:   'Elipse Ltd.  [jms]     Revised: 2012 Jan 01' */

/* NOTE: For wildcard expansion to work from the command line, */
/*       Link with WildArgv.obj                                */
/*       compile command:  wcl386 opendef.c WildArgv.obj           */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#define MAXBUF      8190
#define MAXCMD      1020
#define MAXFIND     50
#define MAXTRY      5

static char *search;
static char *results[MAXFIND + 2];
static int numfind = 0;

int  main( int argc, char *argv[] );
void usage( void );
void opendef( char *infil );
FILE *find_file( char *infil );
char *parent_dir( char *path );
int check_file( FILE *input );
void scan( FILE *input );
void show_results( void );
char *line_num( char *buf );
int check_name( const char *name );
int is_numeric( const char *p,  const char *q );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int i;
    fprintf( stderr, "\n\tOpenDef ===== (C) Elipse Limited, %s\n\n", __DATE__ );
    if ( argc < 3 )
        usage( );
    search = argv[1];
    opendef( argv[2] );
    return 0;
    }                                   /* main */


/* USAGE ===== Input file name and open ===== $pageif:6 */

void usage( void )
    {                                   /* usage */
    fprintf( stderr, "\n\t USAGE:  opendef methodName indexFileName\n\n" );
    abort( );
    }                                   /* usage */


/* OPENDEF ===== Main search routine ===== $pageif:6 */

void opendef( char *infil )
    {                                   /* opendef */
    if (infil != NULL)
        {
        FILE *fp = find_file( infil );
        if (check_file( fp ))
            {
            scan( fp );
            if (numfind > 0)
                show_results( );
            else
                printf( "No definition found for %s\n", search );
            }
        fclose( fp );
        }
    }                                   /* opendef */


/* FIND_FILE ===== Verify input file is valid ===== $pageif:6 */

FILE *find_file( char *infil )
    {                                   /* find_file */
    FILE *fp = NULL;
    int trys = MAXTRY;
    while (infil != NULL)
        {
        fp = fopen( infil, "rb" );
        if (fp != NULL)
            break;
        infil = parent_dir( infil );
        if (--trys < 0)
            break;
        }
    return fp;
    }                                   /* find_file */


/* PARENT_DIR ===== Bump path to higher level ===== $pageif:6 */

char *parent_dir( char *path )
    {                                   /* parent_dir */
    char *p, *q;
    if (path == NULL)
        {
        fprintf( stderr, "parent_dir( ): BAD FILE NAME\n" );
        return NULL;
        }
    q = strrchr( path, '\\' );
    if ((q != NULL) && (q != path))
        {
        *q = '\0';
        p = strrchr( path, '\\' );
        if (p != NULL)
            {
            strcpy( p + 1, q + 1 );
            return path;
            }
        }
    return NULL;
    }                                   /* parent_dir */


/* CHECK_FILE ===== Verify input file is valid ===== $pageif:6 */

int check_file( FILE *input )
    {                                   /* check_file */
    char buf[MAXBUF + 2];
    memset( buf, '\0', sizeof( buf ) );
    if (input == NULL)
        {
        fprintf( stderr, "UNABLE TO OPEN INPUT FILE\n" );
        return 0;
        }
    if (fgets( buf, MAXBUF, input ) == NULL)
        {
        fprintf( stderr, "EMPTY INPUT FILE\n" );
        return 0;
        }
    if (strstr( buf, "#!-2-" ) == NULL)
        {
        fprintf( stderr, "WRONG FILE TYPE\n" );
        return 0;
        }
    return 1;                           /* success */
    }                                   /* check_file */


/* SCAN ===== Main scan routine (per file) ===== $pageif:6 */

void scan( FILE *input )
    {                                   /* scan */
    char buf[MAXBUF + 2];
    int lgth = strlen( search );
    memset( buf, '\0', sizeof( buf ) );

    while (fgets( buf, MAXBUF, input ) != NULL)
        {
        int cmp = strncmp( search, buf, lgth );
        if (cmp < 0)
            break;                      /* past it */
        if (cmp == 0)
            {
            results[numfind] = strdup( buf );
            if (++numfind > MAXFIND)
                break;                  /* too many matches */
            }
        }
    }                                   /* scan */

/*  SHOW_RESULTS ===== List results or load editor ===== $pageif:6 */

void show_results( void )
    {                                   /* show_results */
    char *ed = getenv( "EDIT" );
    if ((numfind > 1) || (ed == NULL))
        {
        int i;
        for (i = 0; i < numfind; ++i)
            {
            if (results[i] != NULL)
                {
                char *posn = strchr( results[i], '*' );
                if (posn != NULL)
                    printf( ++posn );
                free( results[i] );
                results[i] = NULL;
                }
            }
        }
    else
        {
        if (results[0] != NULL)
            {
            char cmd[MAXCMD + 2];
            char *locn, *posn;
            memset( cmd, '\0', sizeof( cmd ) );
            posn = strchr( results[0], '*' );
            locn = line_num( ++posn );
            sprintf( cmd, "\"%s\" %s -goto:%s\n", ed, posn, locn );
            free( results[0] );
            results[0] = NULL;
            system( cmd );
            }
        }
    }                                   /* show_results */


/*  LINE_NUM ===== Parse out line number ===== $pageif:6 */

char *line_num( char *buf )
    {                                   /* line_num */
    char *q, *p = buf;
    if (buf == NULL)
        {
        fprintf( stderr, "line_num( ): BAD INPUT\n" );
        return NULL;
        }
    do
        {
        q = strchr( p, ':' );
        if (q != NULL)
            {
            if (is_numeric( p, q - 1 ))
                {
                *q = '\0';
                *(p - 1) = '\0';
                return p;
                }
            p = q + 1;
            }
        }
    while ((p != NULL) && (q != NULL));
    return NULL;
    }                                   /* line_num */


/*  IS_NUMERIC ===== Make sure method is a valid name ===== $pageif:6 */

int is_numeric( const char *p,  const char *q )
    {                                   /* is_numeric */
    int ret = 1;
    if ((p == NULL) || (q == NULL))
        {
        fprintf( stderr, "is_numeric( ): BAD INPUT\n" );
        return 0;
        }
    if (p == q)
        return 0;
    while (p <= q)
        {
        if (!isdigit( *p))
            ret = 0;
        ++p;
        }
    return ret;
    }                                   /* is_numeric */
