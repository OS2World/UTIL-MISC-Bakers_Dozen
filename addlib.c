/* $title:      'ADDLIB.C ===== Add directory to LIBPATH =====' */
/* $subtitle:   'Elipse Ltd.  [jms]       Revised: 1998 Aug 31' */


#define INCL_DOSMISC
#define INCL_DOSERRORS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXRECLEN       3180
#define MAXFILNAME      260
#define ROOTFILE        "Config"

static char dirname[MAXFILNAME];

int main( int argc, char *argv[] );
static void set_dir( const char *arg );
static void addlib( void );
static void set_globals( char *cfgfile, char *tmpfile, char *newfile, char *bakfile );
static int check_file( char *fname );
static int inc_string( char *str );
static int body_loop( FILE *from, FILE *mod, FILE *copy );
static int check_record( char *buf );
static char bootdrv( void );


/* MAIN ===== Start up and call menu ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\n\t\t Add directory to libpath\n" );
    fprintf( stderr, "\n\t\t Last revision: %s \n", __DATE__ );
    if ( argc > 1 )
        {
        set_dir( argv[1] );
        addlib( );
        return 0;
        }
    else
        {
        fprintf( stderr, "\n Must give directory name to add\n" );
        return 1;
        }
    }                                   /* main */


/* SET_DIR ===== Directory name to add ===== $pageif:6 */

static void set_dir( const char *arg )
    {                                   /* set_dir */
    memset( dirname, '\0', sizeof (dirname) );
    strcpy( dirname, arg );
    strupr( dirname );
    strcat( dirname, ";" );
    }                                   /* set_dir */


/* ADDLIB ===== Body of program ===== $pageif:6 */

static void addlib( void )
    {                                   /* addlib */
    FILE *from, *mod, *copy;
    char cfgfile[MAXFILNAME], tmpfile[MAXFILNAME], newfile[MAXFILNAME], bakfile[MAXFILNAME];
    int chg_flg = 0;
    set_globals( cfgfile, tmpfile, newfile, bakfile );

    from = fopen( cfgfile, "r" );
    if ( from == NULL )
        return;

    mod = fopen( tmpfile, "w" );
    if ( mod == NULL )
        return;

    copy = fopen( bakfile, "w" );
    if ( copy == NULL )
        return;

    chg_flg = body_loop( from, mod, copy );

    fclose( from );
    fclose( mod );
    fclose( copy );

    if ( chg_flg )
        {
        if ( rename( cfgfile, newfile ) == 0 )
            if ( rename( tmpfile, cfgfile ) == 0 )
                remove( newfile );
        }
    else
        remove( tmpfile );
    }                                   /* addlib */


/* SET_GLOBALS ===== From screen input ===== $pageif:6 */

static void set_globals( char *cfgfile, char *tmpfile, char *newfile, char *bakfile )
    {                                   /* set_globals */
    char str[MAXFILNAME];
    FILE *fp = NULL;
    memset( str, '\0', MAXFILNAME );
    str[0] = bootdrv( );
    if ( str[0] <= ' ' )
        {
        fprintf( stderr, "\n Cannot determine boot drive\n" );
        exit( 9 );
        }

    str[1] = ':';
    str[2] = '\\';
    strcat( str, ROOTFILE );

    memset( cfgfile, '\0', MAXFILNAME );
    strcpy( cfgfile, str );
    strcat( cfgfile, ".sys" );
    fp = fopen( cfgfile, "r" );
    if ( fp == NULL )
        {
        fprintf( stderr, "\n Cannot read CONFIG.SYS file\n" );
        exit( 10 );
        }
    fclose( fp );

    memset( tmpfile, '\0', MAXFILNAME );
    strcpy( tmpfile, str );
    strcat( tmpfile, ".t$0" );
    if ( !check_file( tmpfile ) )
        {
        fprintf( stderr, "\n Cannot create temporary file (1)\n" );
        exit( 11 );
        }

    memset( newfile, '\0', MAXFILNAME );
    strcpy( newfile, tmpfile );
    if ( !inc_string( newfile ) || !check_file( newfile ) )
        {
        fprintf( stderr, "\n Cannot create temporary file (2)\n" );
        exit( 12 );
        }

    memset( bakfile, '\0', MAXFILNAME );
    strcpy( bakfile, str );
    strcat( bakfile, ".000" );
    if ( !check_file( bakfile ) )
        {
        fprintf( stderr, "\n Cannot create back up file\n" );
        exit( 13 );
        }
    }                                   /* set_globals */


/* CHECK_FILE ===== Get usable name ===== $pageif:6 */

static int check_file( char *fname )
    {                                   /* check_file */
    do  
        {
        FILE *fp = fopen( fname, "r" );
        if ( fp == NULL )
            return 1;                   /* name available */
        fclose( fp );
        }
    while ( inc_string( fname ) );
    return 0;                           /* unable to find */
    }                                   /* check_file */


/* INC_STRING ===== Increment string counter ===== $pageif:6 */

static int inc_string( char *str )
    {                                   /* inc_string */
    int pos;
    for ( pos = strlen( str ) - 1;  pos >= 0;  --pos )
        {
        if ( !isdigit( str[pos] ) )
            if ( str[pos] == '.' )
                break;
            else
                str[pos] = '0';
        ++str[pos];
        if ( isdigit( str[pos] ) )
            return 1;
        str[pos] = '0';
        }
    return 0;                           /* failed */
    }                                   /* inc_string */


/* BODY_LOOP ===== Main loop of program ===== $pageif:6 */

static int body_loop( FILE *from, FILE *mod, FILE *copy )
    {                                   /* body_loop */
    int chg_flag = 0;
    char buf[MAXRECLEN + MAXFILNAME];
    memset( buf, '\0', sizeof (buf) );

    while ( fgets( buf, sizeof (buf), from ) != NULL )
        {
        fputs( buf, copy );
        if ( check_record( buf ) )
            chg_flag = 1;
        fputs( buf, mod );
        }

    return chg_flag;
    }                                   /* body_loop */


/* CHECK_RECORD ===== Print single record ===== $pageif:6 */

static int check_record( char *buf )
    {                                   /* check_record */
    static const char *test_str = "LIBPATH";
    if ( strnicmp( buf, test_str, strlen( test_str ) ) == 0 )
        {
        if ( strstr( strupr( buf ), dirname ) == NULL )
            {
            int n = strlen( buf ) - 1;
            if ( buf[n - 1] == ';' )
                buf[n] = '\0';
            else
                buf[n] = ';';
            strcat( buf, dirname );
            strcat( buf, "\n" );
            return 1;
            }
        }
    return 0;
    }                                   /* check_record */


/* BOOTDRV ===== Body of program ===== $pageif:6 */

static char bootdrv( void )
    {                                   /* bootdrv */
    const static char all[] = "?ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    ULONG info = 0;
    APIRET rc = DosQuerySysInfo( QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, (PVOID) &info, sizeof( info ) );

    if (rc == NO_ERROR)
        {
        if ((info > 0) && (info < sizeof( all )))
            return all[info];
        else
            {
            printf( "\nImpossible drive value of %d\n", info );
            return '\0';
            }
        }

    printf( "\nDosQuerySysInfo error: return code = %u\n", rc );
    return '\0';
    }                                   /* bootdrv */

