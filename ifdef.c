/* $title:      'IFDEF.C ===== Extract source where defined =====' */
/* $subtitle:   'Elipse Ltd.  [jms]          Revised: 2000 Nov 19' */

/* NOTE: For wildcard expansion to work from the command line, */
/*       Link with SETARGV.OBJ                                 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NORMAL      0
#define DEFINED     1
#define ELSE        2

static int state = NORMAL;
static char macro[80];
static FILE *fpin = NULL;
static FILE *fpout = NULL;

int main( int argc, char *argv[] );
void ifdef( const char *filename );
void process_file( void );
void process_line( const char *buf );
void putif( const char *buf );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int i;
    fprintf( stderr, "\n\n\t IFDEF\t---\t (C) Elipse Ltd.  Nov, 2000\n\n" );
    if ( argc < 3 )
        {
        fprintf( stderr, "\n\t USAGE:  IfDef MacroName InputFile1 [InputFile2] ...\n" );
        return 1;
        }
    strncpy( macro, argv[1], sizeof (macro) );
    fprintf( stderr, "\nProcessing macro %s\n", macro );
    for ( i = 2;  i < argc;  ++i )
        ifdef( argv[i] );
    return 0;
    }                                   /* main */


/* IFDEF ===== Main counting routine ===== $pageif:6 */

void ifdef( const char *filename )
    {                                   /* ifdef */
    char buf[512], *p;
    strncpy( buf, filename, sizeof (buf) );
    fpin = fopen( buf, "r" );
    if ( fpin == NULL )
        {
        strcat( buf, ".c" );
        fpin = fopen( buf, "r" );
        if ( fpin == NULL )
            {
            fprintf( stderr, "\nError. File %s not available\n", filename );
            return;
            }
        }
    p = strrchr( buf, '.' );
    if ( p != NULL )
        *p = '\0';
    strcat( buf, ".new" );
    fpout = fopen( buf, "w" );
    if ( fpout == NULL )
        {
        fprintf( stderr, "\nError. Output file %s not available\n", buf );
        return;
        }
    fprintf( stderr, "\nProcessing file %s\n", filename );
    process_file( );
    fclose( fpin );
    fclose( fpout );
    fpin = fpout = NULL;
    }                                   /* ifdef */


/* PROCESS_FILE ===== Main processing routine ===== $pageif:6 */

void process_file( void )
    {                                   /* process_file */
    char buf[2048];
    memset( buf, '\0', sizeof (buf) );
    state = NORMAL;
    while ( (fgets( buf, sizeof (buf), fpin )) != NULL )
        process_line( buf );
    }                                   /* process_file */


/* PROCESS_LINE ===== Processing one source line ===== $pageif:6 */

void process_line( const char *buf )
    {                                   /* process_line */
    char cmd[1024], val[1024];
    memset( cmd, '\0', sizeof (cmd) );
    memset( val, '\0', sizeof (val) );
    if ( sscanf( buf, "%s%s", cmd, val ) < 1 )
        {
        putif( buf );
        return;
        }
    if ( *cmd != '#' )
        {
        putif( buf );
        return;
        }
    if ( strcmp( cmd, "#else" ) == 0 )
        {
        if ( state == DEFINED )
            {
            state = ELSE;
            return;
            }
        if ( state == ELSE )
            {
            state = DEFINED;
            return;
            }
        putif( buf );
        return;
        }
    if ( strcmp( cmd, "#endif" ) == 0 )
        {
        if ( state == NORMAL )
            putif( buf );
        state = NORMAL;
        return;
        }
    if ( strcmp( val, macro ) != 0 )
        {
        putif( buf );
        return;
        }
    if ( strcmp( cmd, "#if" ) == 0 )
        {
        state = DEFINED;
        return;
        }
    if ( strcmp( cmd, "#ifdef" ) == 0 )
        {
        state = DEFINED;
        return;
        }
    if ( strcmp( cmd, "#ifndef" ) == 0 )
        {
        state = ELSE;
        return;
        }
    }                                   /* process_line */


/* PUTIF ===== Output string if state allows ===== $pageif:6 */

void putif( const char *buf )
    {                                   /* putif */
    if ( state != ELSE )
        fputs( buf, fpout );
    }                                   /* putif */
