/* $title:      'RUNFIX.C ===== Fix Java source code threading =====' */
/* $subtitle:   'Elipse Ltd.  [jms]                      2007 Jul 30' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main( int argc, char *argv[] );
static void runfix( const char *source );
static int scan( const char *infile, const char *outfile );
static int replace( const char *infile, const char *outfile );
static void check_tokens( char *str );
static char *fix_class( char *str );
static char *fix_run( char *str );

static int public_class = 0;
static int public_void = 0;
static int now_runnable = 0;
static int public_run = 0;
static int calls_run = 0;
static int implements = 0;


/* MAIN ===== Start of program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\nRUNFIX, (C) Elipse Ltd. %s\n", __DATE__ );
    if ( argc < 2 )
        {
        fprintf( stderr, "\nUSAGE: runfix JavaSourceName\n" );
        return 1;
        }
    runfix( argv[1] );
    fprintf( stderr, "\nEnd of program\n" );
    return 0;
    }                                   /* main */


/* RUNFIX ===== Program body ===== $pageif:6 */

static void runfix( const char *source )
    {                                   /* runfix */
    char *p, source_base[260], srcfile[260], bakfile[260];
    memset( source_base, '\0', sizeof (source_base) );
    memset( srcfile, '\0', sizeof (srcfile) );
    memset( bakfile, '\0', sizeof (bakfile) );
    strcpy( source_base, source );
    p = strchr( source_base, '.' );
    if ( p != NULL )
        *p = '\0';
    strcpy( srcfile, source_base );
    strcat( srcfile, ".java" );
    strcpy( bakfile, source_base );
    strcat( bakfile, ".bak" );
    if ( scan( srcfile, bakfile ) )
        replace( bakfile, srcfile );
    }                                   /* runfix */


/* SCAN ===== Look for tokens and make backup ===== $pageif:6 */

static int scan( const char *infile, const char *outfile )
    {                                   /* scan */
    char buf[2048];
    FILE *fin = fopen( infile, "r" );
    FILE *fout = fopen( outfile, "w" );
    if ( (fin == NULL) || (fout == NULL) )
        {
        fprintf( stderr, "\nError opening files on scan\n" );
        return 0;                       /* failure */
        }
    public_class = public_void = now_runnable = public_run = calls_run = implements = 0;
    memset( buf, '\0', sizeof (buf) );
    while ( fgets( buf, sizeof (buf), fin ) != NULL )
        {
        fprintf( fout, buf );
        check_tokens( buf );
        }
    fclose( fin );
    fclose( fout );
    if ( !now_runnable && public_run && calls_run )
        return 1;                       /* all conditions met */
    return 0;
    }                                   /* scan */


/* REPLACE ===== Fix up source code ===== $pageif:6 */

static int replace( const char *infile, const char *outfile )
    {                                   /* replace */
    char buf[2048], tmp[2048];
    int was_pub = 0;
    FILE *fin = fopen( infile, "r" );
    FILE *fout = fopen( outfile, "w" );
    if ( (fin == NULL) || (fout == NULL) )
        {
        fprintf( stderr, "\nError opening files on replace\n" );
        return 0;                       /* failure */
        }
    public_class = public_void = now_runnable = public_run = calls_run = implements = 0;
    memset( buf, '\0', sizeof (buf) );
    memset( tmp, '\0', sizeof (tmp) );
    while ( fgets( buf, sizeof (buf), fin ) != NULL )
        {
        strcpy( tmp, buf );
        check_tokens( buf );
        if ( public_class || was_pub )
            {
            fprintf( fout, fix_class( tmp ) );
            if ( was_pub )
                was_pub = 0;
            if ( public_class )
                was_pub = 1;
            }
        else if ( calls_run )
            fprintf( fout, fix_run( tmp ) );
        else
            fprintf( fout, tmp );
        }
    fclose( fin );
    fclose( fout );
    return 1;
    }                                   /* replace */


/* CHECK_TOKENS ===== See if source has need for changeing ===== $pageif:6 */

static void check_tokens( char *str )
    {                                   /* check_tokens */
    char *token = strtok( str, " \t\n\r" );
    char *last = NULL;
    while ( token != NULL )
        {
        if ( strcmp( token, "{" ) == 0 )
            {
            public_class = public_void = 0;
            }
        if ( strcmp( token, "class" ) == 0 )
            {
            if ( last != NULL )
                if ( strcmp( last, "public" ) == 0 )
                    public_class = 1;
            }
        if ( strcmp( token, "implements" ) == 0 )
            {
            if ( public_class )
                implements = 1;
            }
        if ( strcmp( token, "void" ) == 0 )
            {
            if ( last != NULL )
                if ( strcmp( last, "public" ) == 0 )
                    public_void = 1;
            }
        if ( strncmp( token, "run(", 4 ) == 0 )
            {
            if ( public_void )
                public_run = 1;
            else
                calls_run = 1;
            }
        if ( strncmp( token, "(", 1 ) == 0 )
            {
            if ( last != NULL )
                if ( strcmp( last, "run" ) == 0 )
                    if ( public_void )
                        public_run = 1;
                    else
                        calls_run = 1;
            }
        if ( strcmp( token, "Runnable" ) == 0 )
            {
            if ( public_class )
                if ( implements )
                    now_runnable = 1;
            }
        last = token;
        token = strtok( NULL, " \t\n\r" );
        }
    }


/* FIX_CLASS ===== Add Runnable to declaration ====== $pageif:6 */

static char *fix_class( char *str )
    {                                   /* fix_class */
    static char buf[2048];
    char *p = NULL;
    memset( buf, '\0', sizeof (buf) );
    p = strchr( str, '{' );
    if ( p != NULL )
        {
        *p = '\0';
        strcpy( buf, str );
        if ( implements )
            strcat( buf, " , Runnable {" );
        else
            strcat( buf, " implements Runnable {" );
        strcat( buf, ++p );
        public_class = implements = 0;
        return buf;
        }
    return str;
    }                                   /* fix_class */


/* FIX_RUN ===== Replace run call with threading ===== $pageif:6 */

static char *fix_run( char *str )
    {                                   /* fix_run */
    static char buf[2048];
    char *p = NULL;
    memset( buf, '\0', sizeof (buf) );
    p = strstr( str, " run(" );
    if ( p == NULL )
        p = strstr( str, " run " );
    if ( p == NULL )
        p = strstr( str, "\trun(" );
    if ( p == NULL )
        p = strstr( str, "\trun " );
    if ( p != NULL )
        {
        *(++p) = '\0';
        strcpy( buf, str );
        strcat( buf, " { Thread th = new Thread( this ); th.start( ); } " );
        p = strchr( ++p, ';' );
        if ( p != NULL )
            strcat( buf, ++p );
        calls_run = 0;
        return buf;
        }
    return str;
    }                                   /* fix_run */
