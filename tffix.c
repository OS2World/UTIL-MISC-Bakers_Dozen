/* $title:      'TFFIX.C ===== Fix Java source code text fields =====' */
/* $subtitle:   'Elipse Ltd.  [jms]                       2001 Jan 14' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEM      80
#define TFF_TOKEN     "//TFF//"

int main( int argc, char *argv[] );
static void tffix( const char *source );
static int scan( const char *infile, const char *outfile );
static int replace( const char *infile, const char *outfile );
static int check_token( char *str );
static void clear_table( void );
static void add_item( char *str );
static char *check_field( char *str );

static char *field[MAX_ITEM + 2];
static char *param[MAX_ITEM + 2];
static int count = 0;


/* MAIN ===== Start of program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    char str_val[512];
    memset( str_val, '\0', sizeof (str_val) );
    fprintf( stderr, "\nTFFIX, (C) Elipse Ltd. %s\n", __DATE__ );
    if ( argc < 2 )
        {
        fprintf( stderr, "\nUSAGE: tffix JavaSourceName\n" );
        return 1;
        }
    tffix( argv[1] );
    fprintf( stderr, "\nEnd of program\n" );
    return 0;
    }                                   /* main */


/* TFFIX ===== Program body ===== $pageif:6 */

static void tffix( const char *source )
    {                                   /* tffix */
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
    clear_table( );
    if ( (scan( srcfile, bakfile )) && (count > 0) )
        replace( bakfile, srcfile );
    clear_table( );
    }                                   /* tffix */


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
    memset( buf, '\0', sizeof (buf) );
    while ( fgets( buf, sizeof (buf), fin ) != NULL )
        {
        fprintf( fout, buf );
        if ( check_token( buf ) )
            add_item( buf );
        }
    fclose( fin );
    fclose( fout );
    return 1;
    }                                   /* scan */


/* REPLACE ===== Fix up source code ===== $pageif:6 */

static int replace( const char *infile, const char *outfile )
    {                                   /* replace */
    char buf[2048];
    FILE *fin = fopen( infile, "r" );
    FILE *fout = fopen( outfile, "w" );
    if ( (fin == NULL) || (fout == NULL) )
        {
        fprintf( stderr, "\nError opening files on replace\n" );
        return 0;                       /* failure */
        }
    memset( buf, '\0', sizeof (buf) );
    while ( fgets( buf, sizeof (buf), fin ) != NULL )
        {
        if ( !check_token( buf ) )
            fprintf( fout, check_field( buf ) );
        }
    fclose( fin );
    fclose( fout );
    return 1;
    }                                   /* replace */


/* CHECK_TOKEN ===== See if source is special comment ===== $pageif:6 */

static int check_token( char *str )
    {                                   /* check_token */
    char token[2048];
    memset( token, '\0', sizeof (token) );
    sscanf( str, "%s", token );
    strupr( token );
    if ( strcmp( token, TFF_TOKEN ) == 0 )
        return 1;                       /* found */
    return 0;
    }                                   /* check_token */


/* CLEAR_TABLE ===== De-allocate all memory ===== $pageif:6 */

static void clear_table( void )
    {                                   /* clear_table */
    int i;
    for ( i = 0;  i <= MAX_ITEM;  ++i )
        {
        if ( field[i] != NULL )
            free( field[i] );
        if ( param[i] != NULL )
            free( param[i] );
        field[i] = param[i] = NULL;
        }
    count = 0;
    }                                   /* clear_table */


/* ADD_ITEM ===== To list ===== $pageif:6 */

static void add_item( char *str )
    {                                   /* add_item */
    char new_field[1024], temp[1536], *p, *q;
    if ( (str == NULL) || (count > MAX_ITEM) )
        return;
    memset( new_field, '\0', sizeof (new_field) );
    sscanf( str, "%s %s", temp, new_field );
    q = strchr( new_field, ':' );
    if ( q != NULL )
        *q = '\0';
    p = strchr( str, ':' );
    if ( p != NULL )
        {
        while ( *(++p) == ' ' )
            ;
        q = strchr( p, '\n' );
        if ( q != NULL )
            *q = '\0';
        field[count] = strdup( new_field );
        param[count] = strdup( p );
        ++count;
        }
    }                                   /* add_item */


/* CHECK_FIELD ===== Scan table for match ===== $pageif:6 */

static char *check_field( char *str )
    {                                   /* check_field */
    static char buf[2048];
    int i;
    memset( buf, '\0', sizeof (buf) );
    for ( i = 0;  i < count;  ++i )
        {
        char *p = strstr( str, field[i] );
        if ( p != NULL )
            {
            char *q = strchr( p, '=' );
            if ( q != NULL )
                {
                char *r = strstr( q, "new " );
                if ( r != NULL )
                    {
                    char *s = strchr( r, '(' );
                    if ( s != NULL )
                        {
                        strncpy( buf, str, ++s - str );
                        strcat( buf, " new EnforceDocument( EnforceDocument." );
                        strcat( buf, param[i] );
                        strcat( buf, " ), " );
                        strcat( buf, s );
                        s = strchr( buf, '\n' );
                        if ( s != NULL )
                            *s = '\0';
                        strcat( buf, " /* Mod by TFfix */\n" );
                        return buf;
                        }
                    }
                }
            }
        }
    return str;
    }                                   /* check_field */
