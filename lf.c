/* $title:       'LF.C ===== List files across in sorted order ======' */
/* $subtitle:    'Elipse Ltd.  [jms]             Revised: 2009 Dec 02' */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#define FILE_ATTRIBUTE 22               /* normal files */
#define MAXEXT   4096
#define MAXNAM   16384

int main( int argc, const char *argv[] );
void init( void );
void list_files( const char *spec );
unsigned get_files( char *mask );
void add_file( const char *full );
char *ext_part( const char *str );
char *nam_part( const char *str );
int compare( const void *, const void * );
void put_files( void );
void appendif( char *str, char chk, char add );

unsigned long tot_bytes = 0L;
unsigned int num_ext = 0;
unsigned int siz_ext = 0;
unsigned int siz_nam = 0;

struct file_ext
    {
    char *name;
    unsigned count;
    char **ptr;
    } ext[MAXEXT];


/* MAIN ===== Calling routine ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\n\tLF (List Files) ======= (C) Elipse Limited, %s\n", __DATE__ );
    if ( argc < 2 )
        {
        init( );
        list_files( "*" );
        }
    else
        {
        int i;
        for ( i = 1;  i < argc;  ++i )
            {
            init( );
            list_files( argv[i] );
            }
        }
    fprintf( stderr, "\n" );
    return 0;
    }                                   /* main */


/* INIT ===== Initialize statics ===== $pageif:6 */

void init( void )
    {                                   /* init */
    int i;
    for ( i = 0;  i < MAXEXT;  ++i )
        {
        ext[i].name = NULL;
        ext[i].count = 0;
        ext[i].ptr = NULL;
        }
    num_ext = 0;
    tot_bytes = 0L;
    }                                   /* init */


/* LIST_FILES ===== Actual file lister ===== $pageif:6 */

void list_files( const char *spec )
    {                                   /* list_files */
    unsigned n = 0;
    char str[_MAX_PATH + 4];
    memset( str, '\0', sizeof (str) );
    strcpy( str, spec );
    appendif( str, ':', '\\' );
    appendif( str, '\\', '*' );
    if ( strchr( str, '.' ) == NULL )
        strcat( str, ".*" );
    fprintf( stderr, "\n\t%s\t", str );
    fflush( stderr );
    n = get_files( str );
    if ( n == 0 )
        fprintf( stderr, "\t No matching files.\n" );
    else
        {
        fprintf( stderr, "\t%d files found totaling %ld bytes.\n", n, tot_bytes );
        put_files( );
        }
    }                                   /* list_files */


/* GET_FILES ===== Load file names to memory ===== $pageif:6 */

unsigned get_files( char *mask )
    {                                   /* get_files */
    unsigned n = 0;
    long FindHandle = 0L;
    struct _finddata_t FindBuffer;
    memset( &FindBuffer, '\0', sizeof (FindBuffer) );

    FindHandle = _findfirst( mask, &FindBuffer );
    if (FindHandle != -1L)
        {
        do
            {
            if ( !(FindBuffer.attrib & FILE_ATTRIBUTE) )
                {
                ++n;
                tot_bytes += FindBuffer.size;
                add_file( FindBuffer.name );
                }
            }
        while (_findnext( FindHandle, &FindBuffer ) != -1 );
        }
    _findclose( FindHandle );
    return n;
    }                                   /* get_files */


/* ADD_FILE ===== Add file name to table list ===== $pageif:6 */

void add_file( const char *full )
    {                                   /* add_file */
    int i;
    char *front = nam_part( full );
    char *back  = ext_part( full );

    for ( i = 0;  i < num_ext;  ++i )
        if ( strcmp( back, ext[i].name ) == 0 )
            {
            ext[i].ptr[ext[i].count] = strdup( front );
            if ( ++ext[i].count > MAXNAM )
                {
                fprintf( stderr, "\nToo many file names for extension %s.\n", back );
                exit( 5 );
                }
            return;
            }
    ext[num_ext].ptr = calloc( MAXNAM, sizeof (char *) );
    if ( ext[num_ext].ptr == NULL )
        {
        fprintf( stderr, "\nNot enough memory available.\n" );
        exit( 6 );
        }
    ext[num_ext].name = strdup( back );
    ext[num_ext].ptr[0] = strdup( front );
    ++ext[num_ext].count;
    if ( ++num_ext > MAXEXT )
        {
        fprintf( stderr, "\nToo many file name extensions.\n" );
        exit( 7 );
        }
    }                                   /* add_file */


/* EXT_PART ===== Extract file extension from name ===== $pageif:6 */

char *ext_part( const char *str )
    {                                   /* ext_part */
    static char buf[_MAX_PATH];
    char *pt;
    int lgth;
    buf[0] = '\0';
    if ( (pt = strrchr( str, '.' )) != NULL )
        strcpy( buf, pt + 1 );
    lgth = strlen( buf );
    if (lgth > siz_ext)
        siz_ext = lgth;
    return buf;
    }                                   /* ext_part */


/* NAM_PART ===== Extract file name from full name ===== $pageif:6 */

char *nam_part( const char *str )
    {                                   /* nam_part */
    static char buf[_MAX_PATH];
    char *pt;
    int lgth;
    strcpy( buf, str );
    if ( (pt = strrchr( buf, '.' )) != NULL )
        *pt = '\0';
    lgth = strlen( buf );
    if (lgth > siz_nam)
        siz_nam = lgth;
    return buf;
    }                                   /* nam_part */


/* COMPARE ===== Sort string pointers ====== $pageif:6 */

int compare( const void *arg1, const void *arg2 )
   {
   return ( strcmp( *(char **)arg1, *(char **)arg2 ) );
   }


/* PUT_FILES ===== Output file names across page ===== $pageif:6 */

void put_files( void )
    {                                   /* put_files */
    unsigned i, j, k;
    int we = siz_ext + 1;
    int wn = siz_nam + 1;
    int nc = (77 - we) / wn;
    qsort( ext, num_ext, sizeof (struct file_ext), compare );

    for ( i = 0;  i < num_ext;  ++i )
        {
        printf( "\n.%-*s ", we, ext[i].name );
        qsort( ext[i].ptr, ext[i].count, sizeof (char **), compare );
        k = 0;
        for ( j = 0;  j < ext[i].count;  ++j )
            {
            if ( ++k > nc )
                {
                printf( "\n %-*s ", we, " " );
                k = 1;
                }
            printf( "%-*s", wn, ext[i].ptr[j] );
            free( ext[i].ptr[j] );
            ext[i].ptr[j] = NULL;
            }
        free( ext[i].name );
        ext[i].name = NULL;
        free( ext[i].ptr );
        ext[i].ptr = NULL;
        }
    printf( "\n" );
    }                                   /* put_files */


/* APPENDIF ===== Append char to string conditionally ===== $pageif:6 */

void appendif( char *str, char chk, char add )
    {                                   /* appendif */
    unsigned last = strlen( str ) - 1;
    if ( str[last] == chk )
        {
        str[last + 1] = add;
        str[last + 2] = '\0';
        }
    }                                   /* appendif */
