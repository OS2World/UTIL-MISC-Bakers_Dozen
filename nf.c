/* $title:       'NF.C ===== List files from newest to oldest ======' */
/* $subtitle:    'Elipse Ltd.  [jms]            Revised: 2011 Oct 15' */

/* NOTE: DO NOT link with wildarg.obj     */
/*       argument expansion is not wanted */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <io.h>

#define FILE_ATTRIBUTE 32               /* normal files */
#define HIDE_ATTRIBUTE  2               /* hidden files */
#define DEFAULT_SHOW   12               /* number of files */

unsigned int num_files = DEFAULT_SHOW;
char spec[_MAX_PATH + 4];
struct _finddata_t *info_array = NULL;

int main( int argc, char *argv[] );
void init( void );
void new_files( void );
void array_create( void );
void array_destroy( void );
unsigned int get_files( char *path );
void add_file( const struct _finddata_t *dptr );
unsigned int array_old( void );
int cronological( const struct _finddata_t *t1, const struct _finddata_t *t2 );
void put_files( unsigned int n );
void put_time( time_t tim );
void put_size( unsigned long sz );
struct _finddata_t *take_file( struct _finddata_t *curr );
unsigned int array_new( void );
void parse_args( const char *str );


/* MAIN ===== Calling routine ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int i;
    fprintf( stderr, "\n\tNF (New Files) ======= (C) Elipse Limited, %s\n", __DATE__ );
    init( );
    for ( i = argc - 1;  i > 0;  --i )
        parse_args( argv[i] );
    new_files( );
    fprintf( stderr, "\n" );
    return 0;
    }                                   /* main */


/* INIT ===== Initialize statics ===== $pageif:6 */

void init( void )
    {                                   /* init */
    info_array = NULL;
    num_files = DEFAULT_SHOW;
    memset( spec, '\0', sizeof (spec) );
    spec[0] = '*';
    }                                   /* init */


/* NEW_FILES ===== Actual file lister ===== $pageif:6 */

void new_files( void )
    {                                   /* new_files */
    unsigned int n;
    char str[_MAX_PATH + 4];
    memset( str, '\0', sizeof (str) );
    strcpy( str, spec );
    printf( "\t%s\t", str );
    array_create( );
    n = get_files( str );
    if ( n == 0 )
        printf( "\n No matching files.\n" );
    else
        {
        printf( "\t%u files found in directory.\n\n", n );
        put_files( n );
        }
    array_destroy( );
    }                                   /* new_files */


/* ARRAY_CREATE ===== Malloc memory ===== $pageif:6 */

void array_create( void )
    {                                   /* array_create */
    info_array = calloc( num_files, sizeof(struct _finddata_t) );
    if (info_array == NULL )
        {
        printf( "\nCannot allocate memory!\n" );
        exit( 5 );
        }
    }                                   /* array_create */


/* ARRAY_DESTROY ===== Free memory ===== $pageif:6 */

void array_destroy( void )
    {                                   /* array_destroy */
    free( info_array );
    info_array = NULL;
    }                                   /* array_destroy */


/* GET_FILES ===== Load file names to memory ===== $pageif:6 */

unsigned get_files( char *mask )
    {                                   /* get_files */
    unsigned int n = 0;
    unsigned long tot_bytes = 0L;
    long FindHandle = 0L;
    struct _finddata_t FindBuffer;
    memset( &FindBuffer, '\0', sizeof (FindBuffer) );

    FindHandle = _findfirst( mask, &FindBuffer );
    if (FindHandle != -1L)
        {
        do
            {
            if (FindBuffer.attrib & FILE_ATTRIBUTE)
                {                       /* file, not dir */
                if (!(FindBuffer.attrib & HIDE_ATTRIBUTE))
                    {                   /* not hidden */
                    ++n;
                    tot_bytes += FindBuffer.size;
                    add_file( &FindBuffer );
                    }
                }
            }
        while (_findnext( FindHandle, &FindBuffer ) != -1 );
        }
    _findclose( FindHandle );
    return n;
    }                                   /* get_files */


/* ADD_FILE ===== To current list ===== $pageif:6 */

void add_file( const struct _finddata_t *dptr )
    {                                   /* add_file */
    unsigned int oldest = array_old( );
    if ( cronological( &info_array[oldest], dptr ) )
        memcpy( &info_array[oldest], dptr, sizeof( struct _finddata_t ) );
    }                                   /* add_file */


/* ARRAY_OLD ===== To locate oldest entry ===== $pageif:6 */

unsigned int array_old( void )
    {                                   /* array_old */
    unsigned int i, p = 0;
    for ( i = 1;  i < num_files;  ++i )
        {
        if ( cronological( &info_array[i], &info_array[p] ) )
            p = i;
        }
    return p;
    }                                   /* array_old */


/* CRONOLOGICAL ===== Return true or false ===== $pageif:6 */

int cronological( const struct _finddata_t *t1, const struct _finddata_t *t2 )
    {                                   /* cronological */
    return ( (t1->time_write) <= (t2->time_write) );
    }                                   /* cronological */


/* PUT_FILES ===== Output file information ===== $pageif:6 */

void put_files( unsigned int n )
    {                                   /* put_files */
    unsigned int i;
    struct _finddata_t *curr = NULL;
    if ( n > num_files )
        n = num_files;
    for ( i = 0;  i < n;  ++i )
        {
        curr = take_file( curr );
        if ( curr == NULL )
            break;
        put_time( curr->time_write );
        put_size( curr->size );
        printf( "  %s\n", curr->name );
        }
    }                                   /* put_files */


/* PUT_TIME ===== Output file time ===== $pageif:6 */

void put_time( time_t tim )
    {                                   /* put_time */
    static const char *mth[] = { "Jan", "Feb", "Mar", "Apr",
                 "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "?" };
    char dt_str[16], tm_str[16];
    struct tm filetm;
    _localtime( &tim, &filetm );
    sprintf( dt_str, "%04d-%3s-%02d\0\0", filetm.tm_year + 1900, mth[filetm.tm_mon], filetm.tm_mday  );
    sprintf( tm_str, "%2d:%02d:%02d\0\0", filetm.tm_hour, filetm.tm_min, filetm.tm_sec );
    printf( "%s %s ", dt_str, tm_str );
    }                                   /* put_time */


/* PUT_SIZE ===== Output file size ===== $pageif:6 */

void put_size( unsigned long sz )
    {                                   /* put_size */
    char str[24];
    unsigned int a, b, c, i;
    a = (unsigned int) (sz / 1000000L);
    sz = sz % 1000000L;
    b = (unsigned int) (sz / 1000L);
    c = (unsigned int) sz % 1000L;
    sprintf( str, "%5u,%03u,%03u ", a, b, c );
    for ( i = 0;  i < 12;  ++i )
        {
        if ( str[i] > '0' )
            break;
        str[i] = ' ';
        }
    printf( str );
    }                                   /* put_size */


/* TAKE_FILE ===== The newest in array ===== $pageif:6 */

struct _finddata_t *take_file( struct _finddata_t *curr )
    {                                   /* take_file */
    unsigned int newest;
    if ( curr != NULL )
        memset( curr, '\0', sizeof( struct _finddata_t ) );
    newest = array_new( );
    if ( info_array[newest].time_write > 0L )
        return &info_array[newest];     /* success */
    return NULL;                        /* none left */
    }                                   /* take_file */


/* ARRAY_NEW ===== To locate newest entry ===== $pageif:6 */

unsigned int array_new( void )
    {                                   /* array_new */
    unsigned int i, p = 0;
    for ( i = 1;  i < num_files;  ++i )
        {
        if ( cronological( &info_array[p], &info_array[i] ) )
            p = i;
        }
    return p;
    }                                   /* array_new */


/* PARSE_ARGS ===== Decipher user input ===== $pageif:6 */

void parse_args( const char *str )
    {                                   /* parse_args */
    int n;
    if ( (strchr( str, '*' ) != NULL) || (strchr( str, '?' ) != NULL) )
        {
        strcpy( spec, str );            /* valid file spec */
        return;
        }
    if ( (*str == '-') || (*str == '/') )
        ++str;
    n = atoi( str );
    if ( n > 0 )
        {
        num_files = (unsigned) n;
        return;
        }
    if ( (*str != 'H') && (*str != 'h') )
        printf( "\nInvalid input: %s\n", str );

    printf( "\nNF (New files):  List the newest files in a directory\n" );
    printf( "\nUsage:  NF [[/][n]] [spec]\n" );
    printf( "\nWhere spec = files to look for (e.g. file0*.chk)\n" );
    printf( "And n = number of files to show (default = %d)\n", DEFAULT_SHOW );
    exit( 1 );
    }                                   /* parse_args */
