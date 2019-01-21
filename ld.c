/* $title:       'LD.C ===== List directories in name order ======' */
/* $subtitle:    'Elipse Ltd.  [jms]          Revised: 2013 May 10' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <io.h>

#define DIR_ATTRIBUTE  _A_SUBDIR        /* directory */

static char spec[_MAX_PATH + 4] = "*\0\0";

int main( int argc, char *argv[] );
static void list_dirs( void );
static unsigned int get_dirs( char *path );
static void add_dir( const struct _finddata_t *dptr );
static int real_dir( char *name );
static void parse_args( const char *str );


/* MAIN ===== Calling routine ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int i;
    fprintf( stderr, "\n\tLD (List Directories) ======= (C) Elipse Limited, %s\n", __DATE__ );
    for (i = argc - 1;  i > 0;  --i)
        parse_args( argv[i] );
    list_dirs( );
    return 0;
    }                                   /* main */


/* LIST_DIRS ===== Actual directory lister ===== $pageif:6 */

static void list_dirs( void )
    {                                   /* list_dirs */
    unsigned int n;
    char str[_MAX_PATH + 4];
    memset( str, '\0', sizeof (str) );
    strcpy( str, spec );
    fprintf( stderr, "\t\t%s\n", str );
    fflush( stderr );
    n = get_dirs( str );
    if (n == 0)
        fprintf( stderr, "\t\tNo matching directories.\n" );
    else
        fprintf( stderr, "\t\t%u directories found.\n", n );
    fflush( stderr );
    }                                   /* list_dirs */


/* GET_DIRS ===== Load file names to memory ===== $pageif:6 */

static unsigned get_dirs( char *mask )
    {                                   /* get_dirs */
    unsigned int n = 0;
    long FindHandle = 0L;
    struct _finddata_t FindBuffer;
    memset( &FindBuffer, '\0', sizeof (FindBuffer) );

    FindHandle = _findfirst( mask, &FindBuffer );
    if (FindHandle != -1L)
        {
        do
            {
            if ((FindBuffer.attrib & _A_SUBDIR)
                    && !(FindBuffer.attrib & _A_HIDDEN))
                {
                if (real_dir( FindBuffer.name ))
                    {
                    printf( "%s\n", FindBuffer.name );
                    ++n;
                    }
                }
            }
        while (_findnext( FindHandle, &FindBuffer ) != -1);
        }
    _findclose( FindHandle );

    fflush( stdout );
    return n;
    }                                   /* get_dirs */


/* REAL_DIR ===== False if . or ..; otherwise true ===== $pageif:6 */

static int real_dir( char *name )
    {                                   /* real_dir */
    if (name[0] != '.')
        return 1;
    if (name[1] == '\0')
        return 0;
    if (name[1] != '.')
        return 1;
    if (name[2] == '\0')
        return 0;
    return 1;
    }                                   /* real_dir */


/* PARSE_ARGS ===== Decipher user input ===== $pageif:6 */

static void parse_args( const char *str )
    {                                   /* parse_args */
    int n;
    if ((strchr( str, '*' ) != NULL) || (strchr( str, '?' ) != NULL))
        {
        strcpy( spec, str );            /* valid file spec */
        return;
        }
    if ((*str == '-') || (*str == '/'))
        ++str;
    if ((*str != 'H') && (*str != 'h'))
        printf( "\nInvalid input: %s\n", str );

    printf( "\nLD (List Directories):  List directories in name order\n" );
    printf( "\nUsage:  LD [spec]\n" );
    printf( "\nWhere spec = directories to look for (e.g. abc*)\n" );
    exit( 1 );
    }                                   /* parse_args */
