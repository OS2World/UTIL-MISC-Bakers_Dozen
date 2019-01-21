/* $title:      'JAVAERR.C ===== Find text in files (with line number) =====' */
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
#define EDITOR          "C:\\Program Files\\Scintilla\\SciTE.exe"
#define VIEWER          "C:\\Program Files\\JPsoft\\tcmd13\\v.exe"
#define EDIT_VAR        "EDIT"
#define VIEW_VAR        "VIEW"

static char *search_text = NULL;            /* what we are looking for */
static char *editor = EDITOR;               /* what we are looking for */
static const char *edarg[ARG_SIZE + 4];     /* parameters for editor */
unsigned nout = 0;                          /* index of edarg array */
static int good_compile = 0;                /* set true if error free */

int main( int argc, char *argv[] );
void setup( void );
int javaerr( void );
int scan( const char *buf );
int output( const char *buf, char *p1, char *p2 );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    unsigned num;
    setup( );
    num = javaerr( );
    if (good_compile)
        printf( "\n--- No compile errors ---\n\n");
    else
        if (num != 0)
            return execv( editor, edarg );
    return 0;
    }                                   /* main */


/* SETUP ===== Initialize variables ===== $pageif:6 */

void setup( void )
    {                                   /* setup */
    int n = 0;
    char *p = getenv( EDIT_VAR );
    if (p != NULL)
        editor = p;
    edarg[0] = strdup( editor );
    for (n = 1;  n < (ARG_SIZE + 3);  ++n)
        edarg[n] = NULL;
    }                                   /* setup */


/* JAVAERR ===== Main search routine ===== $pageif:6 */

int javaerr( )
    {                                   /* javaerr */
    int err = 0;
    int msg = 0;
    char buf[MAXBUF + 2];
    memset( buf, '\0', sizeof( buf ) );

    while (gets( buf ) != NULL)
        {
        ++msg;
        printf( "%s\n", buf );          /* show messages as they occur */
        if (scan( buf ))
            ++err;
        }
    if (msg == 0)
        good_compile = 1;
    return err;
    }                                   /* javaerr */


/* SCAN ===== Main scan routine (per file) ===== $pageif:6 */

int scan( const char *buf )
    {                                   /* scan */
    char *posn1 = strchr( buf, ':' );
    if (posn1 != NULL)
        {
        char *posn2 = strchr( posn1 + 1, ':' );
        if (posn2 != NULL)
            return output( buf, posn1, posn2 );
        }
    return 0;
    }                                   /* scan */


/* OUTPUT ===== Show match on console ===== $pageif:6 */

int output( const char *buf, char *p1, char *p2 )
    {                                   /* output */
    if (nout < ARG_SIZE)
        {
        int n;
        char str[512];
        char *line = p1 + 1;
        *p1 = '\0';
        *p2 = '\0';
        n = atoi( line );
        if (n > 0)
            {
            edarg[++nout] = strdup( buf );
            sprintf( str, "-goto:%d\0", n );
            edarg[++nout] = strdup( str );
            return 1;
            }
        }
    return 0;
    }                                   /* output */
