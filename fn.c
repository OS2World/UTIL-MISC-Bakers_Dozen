/* $title:      'FN.C ===== Find text in files (with line number) =====' */
/* $subtitle:   'Elipse Ltd.  [jms]                Revised: 2011 Oct 13' */

/* NOTE: For wildcard expansion to work from the command line, */
/*       Link with WildArgv.obj                                */
/*       compile command:  wcl386 fn.c WildArgv.obj            */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <process.h>

#define MAXBUF          8190
#define MAX_EDIT_BUF    24
#define ARG_SIZE        MAX_EDIT_BUF + MAX_EDIT_BUF
#define EDIT_VAR        "EDIT"
#define VIEW_VAR        "VIEW"

static char *search_text = NULL;            /* what we are looking for */
static char *viewer = "notepad";            /* program to view files */
static char *editor = "notepad";            /* program to edit files */
static const char *extarg[ARG_SIZE + 4];     /* parameters for editor */
static unsigned nout = 0;                   /* index of extarg array */
static int ignore_case = 0;                 /* non-zero == case insensitive */
static int view_out = 0;                    /* non-zero == open files in viewer */
static int edit_out = 0;                    /* non-zero == open files in editor */
static int test = 0;                        /* non-zero == test mode */


int  main( int argc, char *argv[] );
void setup( void );
void usage( void );
void show_error( void );
int set_switch( const char *str );
unsigned fn( const char *infil );
unsigned scan( FILE *input, const char *infil );
int match( char *str );
void output( const char *infil, const char *buf, unsigned line, unsigned upos );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    unsigned num_file = 0;
    unsigned num_match = 0;
    unsigned i = 0;
    fprintf( stderr, "\n\n\t FN\t---\t (C) Elipse Ltd.  Dec 2008-2011\n\n" );
    setup( );
    if (argc < 3)
        {
        usage( );
        return 1;
        }

    for (i = 1;  i < argc;  ++i)
        {
        char fchr = *argv[i];
        if ((fchr == '-') || (fchr =='/'))
            if (set_switch( argv[i] ))
                continue;
        if (search_text == NULL)
            search_text = argv[i];
        else
            {
            unsigned n = fn( argv[i] );
            if (n != 0)
                {
                num_file++;
                num_match += n;
                }
            }
        }
    printf( "\n" );
    fprintf( stderr, "\nFound %u occurrances in %u files\n", num_match, num_file );
    if (!test)
        {
        int ret;
        if (view_out)
            {
            extarg[0] = strdup( viewer );
            ret = execv( viewer, extarg );
            if (ret < 0)
                {
                show_error( );
                return 2;
                }
            }
        if (edit_out)
            {
            extarg[0] = strdup( editor );
            ret = execv( editor, extarg );
            if (ret < 0)
                {
                show_error( );
                return 3;
                }
            }
        }
    return 0;
    }                                   /* main */


/* SETUP ===== Initialize variables ===== $pageif:6 */

void setup( void )
    {                                   /* setup */
    unsigned n;
    char *p;
    for (n = 0;  n < (ARG_SIZE + 3);  ++n)
        extarg[n] = NULL;
    p = getenv( VIEW_VAR );
    if (p != NULL)
        viewer = p;
    p = getenv( EDIT_VAR );
    if (p != NULL)
        editor = p;
    }                                   /* setup */


/* USAGE ===== Input file name and open ===== $pageif:6 */

void usage( void )
    {                                   /* usage */
    fprintf( stderr, "\n" );
    fprintf( stderr, "\t   USAGE:  fn [-switches] text filename [filename] ... \n" );
    fprintf( stderr, "\n" );
    fprintf( stderr, "\tswitches:  \n" );
    fprintf( stderr, "\t           i = ignore case\n" );
    fprintf( stderr, "\t           v = open in viewer \n" );
    fprintf( stderr, "\t           e = open in editor \n" );
    fprintf( stderr, "\t           t = test editor (show arguments but do not run editor) \n" );
    fprintf( stderr, "\n" );
    fprintf( stderr, "\t    NOTE:  Viewer is located by environment variable VIEW \n" );
    fprintf( stderr, "\t           Current value is %s \n", viewer );
    fprintf( stderr, "\n" );
    fprintf( stderr, "\t           Editor is located by environment variable EDIT \n" );
    fprintf( stderr, "\t           Current value is %s \n", editor );
    fprintf( stderr, "\n" );
    }                                   /* usage */

/* SHOW_ERROR ===== Show external run info ===== $pageif:6 */

void show_error( void )
    {                                   /* show_error */
    unsigned n = 0;
    fprintf( stderr, "\nError %d trying to run editor \n\n", errno );
    fprintf( stderr, "  Edit command: %s \n\n", editor );
    fprintf( stderr, "     Arguments: \n" );
    for (n = 0;  n <= nout; ++n)
        fprintf( stderr, "[%02u] %s\n", n, extarg[n] );
    fprintf( stderr, "\n" );
    }                                   /* show_error */


/* SET_SWITCH ===== Set based on input ===== $pageif:6 */

int set_switch( const char *str )
    {                                   /* set_switch */
    while (*(++str) != '\0')
        {
        switch (*str)
            {
        case 'i':
        case 'I':
            ignore_case = 1;
            return 1;
        case 'v':
        case 'V':
            view_out = 1;
            return 1;
        case 'e':
        case 'E':
            edit_out = 1;
            return 1;
        case 't':
        case 'T':
            edit_out = 1;
            test = 1;
            return 1;
            }
        }
    return 0;
    }                                   /* set_switch */


/* FN ===== Main search routine ===== $pageif:6 */

unsigned fn( const char *infil )
    {                                   /* fn */
    unsigned count = 0;
    if (search_text != NULL)
        {
        if (ignore_case)
            search_text = strlwr( search_text );
        if (infil != NULL)
            {
            FILE *fp = fopen( infil, "rb" );
            if (fp != NULL)
                {
                count = scan( fp, infil );
                fclose( fp );
                }
            }
        }
    return count;
    }                                   /* fn */


/* SCAN ===== Main scan routine (per file) ===== $pageif:6 */

unsigned scan( FILE *input, const char *infil )
    {                                   /* scan */
    char buf[MAXBUF + 2], *p;
    unsigned line = 0;
    unsigned count = 0;
    int posn = 0;
    memset( buf, '\0', sizeof( buf ) );

    while (fgets( buf, MAXBUF, input ) != NULL)
        {
        ++line;
        posn = match( buf );
        if (posn >= 0)
            {
            if (count++ == 0)
                output( infil, buf, line, posn );   /* first occurrance */
            else
                output( NULL, buf, line, posn );    /* subsequent */
            }
        }
    return count;
    }                                   /* scan */


/* MATCH ===== Return true if text found ===== $pageif:6 */

int match( char *buf )
    {                                   /* match */
    char *p;
    if (ignore_case)
        p = strstr( strlwr( buf ), search_text );
    else
        p = strstr( buf, search_text );
    if (p == NULL)
        return (-1);
    return (p - buf);
    }                                   /* match */


/* OUTPUT ===== Show match on console ===== $pageif:6 */

void output( const char *infil, const char *buf, unsigned line, unsigned upos )
    {                                   /* output */
    char *p = NULL;
    if (infil != NULL)
        {
        printf( "\n----- %s\n", infil );
        if ((view_out) || (edit_out))
            {
            if ((infil != NULL) && (nout < ARG_SIZE))
                {
                char str[512];
                sprintf( str, "%s\0", infil );
                extarg[++nout] = strdup( str );
                if (view_out)
                    sprintf( str, "/L:%u\0", line, upos );
                if (edit_out)
                    sprintf( str, "-goto:%u,%u\0", line, upos );
                extarg[++nout] = strdup( str );
                }
            }
        }
    p = strchr( buf, '\r' );
    if (p != NULL)
        *p = ' ';
    printf( "[%03u] %s", line, buf );
    }                                   /* output */
