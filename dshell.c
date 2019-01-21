/* $title:      'DSHELL.C ===== Run program with parameters =====' */
/* $subtitle:   'Elipse Ltd.  [jms]          Revised: 2003 Jan 28' */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main( int argc, const char *argv[] );
static int dshell( int argc, const char *argv[] );
static char *elicit( const char *prompt );
static int firstspecial( const char *str );
static int lastspecial( const char *str );

static char *special = " /-=>.:&|%()\\";


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    if (argc > 1)
        {
        dshell( argc, argv );
        return 0;
        }
    fprintf( stderr, "\n DSHELL ===== Run program with parameters =====\n " );
    fprintf( stderr, "\n              (C) Elipse Ltd. %s\n\n ", __DATE__ );
    fprintf( stderr, "\n Required: name of program to run" );
    fprintf( stderr, "\n Optional: prompts for parameters\n" );
    return 127;
    }                                   /* main */


/* DSHELL ===== Body of program ===== $pageif:6 */

static int dshell( int argc, const char *argv[] )
    {                                   /* dshell */
    char cmd[1024];
    if (argc > 1)
        {
        int i;
        for (i = 0;  i < 20;  ++i)
            fputc( '\n', stderr );
        strncpy( cmd, argv[1], sizeof( cmd ) );
        strcat( cmd, " " );
        for (i = 2;  i < argc;  ++i)
            {
            char *pval = NULL;
            if (firstspecial( argv[i] ) || lastspecial( argv[i] ))
                pval = (char *) argv[i];
            else
                pval = elicit( argv[i] );
            if (!firstspecial( pval ) && !lastspecial( cmd ))
                strcat( cmd, " " );
            strcat( cmd, pval );
            }
        if (firstspecial( cmd ))
            fprintf( stderr, "\n\n ** Execute: %s\n\n", &cmd[1] );
        else
            system( cmd );
        }
    return 0;
    }                                   /* dshell */


/* ELICIT ===== Get user input ===== $pageif:6 */

static char *elicit( const char *prompt )
    {                                   /* elicit */
    static char pval[128];
    memset( pval, '\0', sizeof( pval ) );
    if (prompt != NULL)
        {
        fprintf( stderr, "\n\n%s\n", prompt );
        while (*pval == '\0')
            if (gets( pval ) == NULL)
                exit( 99 );
        }
    return pval;
    }                                   /* elicit */


/* FIRSTSPECIAL ===== True if begin with special char ===== $pageif:6 */

static int firstspecial( const char *str )
    {                                   /* firstspecial */
    if (str != NULL)
        {
        char p = *str;
        if (strchr( special, p ) != NULL)
            return 1;
        }
    return 0;
    }                                   /* firstspecial */


/* LASTSPECIAL ===== True if end with special char ===== $pageif:6 */

static int lastspecial( const char *str )
    {                                   /* lastspecial */
    if (str != NULL)
        {
        int lgth = strlen( str );
        if (lgth > 0)
            {
            char p = str[lgth - 1];
            if (strchr( special, p ) != NULL)
                return 1;
            }
        }
    return 0;
    }                                   /* lastspecial */

