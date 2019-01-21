/* $title:       'FF.C ===== Find files recursively ======' */
/* $subtitle:    'Elipse Ltd.  [jms]  Revised: 2011 Dec 30' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <direct.h>

int main( int argc, const char *argv[] );
static unsigned list_files( const char *spec, const char *dir );
static char* make_mask( const char *spec, const char *dir );
static int real_dir( const char *name );
static void show_help( void );


/* MAIN ===== Calling routine ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    int i;
    fprintf( stderr, "\n\tFF (Find Files) ======= (C) Elipse Limited, %s\n", __DATE__ );
    if (argc < 2)
        {
        printf( "\nInput required\n" );
        show_help( );
        exit( 2 );
        }
    if (argc == 2)
        list_files( argv[1], getcwd( NULL, 0 ) );
    else
        {
        for (i = 2;  i < argc;  ++i)
            list_files( argv[1], argv[i] );
        }

    return 0;
    }                                   /* main */


/* LIST_FILES ===== Scan directory, list files, and scan subdirectories ===== $pageif:6 */

static unsigned list_files( const char *spec, const char *dir )
    {                                   /* list_files */
    unsigned int n = 0;
    long FindHandle = 0L;
    struct _finddata_t FindBuffer;
    char *mask = make_mask( spec, dir );
    memset( &mask, '\0', sizeof (mask) );
    memset( &FindBuffer, '\0', sizeof (FindBuffer) );

printf( "\nlist_files(): %s, %s ==> %s\n", spec, dir, mask );

    FindHandle = _findfirst( mask, &FindBuffer );
    if (FindHandle != -1L)
        {
        do
            {

printf( "\t %4x %s\n", FindBuffer.attrib, FindBuffer.name );

            if (!(FindBuffer.attrib & _A_HIDDEN))
                {
                if ((FindBuffer.attrib & _A_NORMAL) || (FindBuffer.attrib & _A_RDONLY))
                    {
                    printf( "%s\n", FindBuffer.name );
                    ++n;
                    }
                if ((FindBuffer.attrib & _A_SUBDIR) && real_dir( FindBuffer.name ))
                    list_files( mask, FindBuffer.name );
                }
            }
        while (_findnext( FindHandle, &FindBuffer ) != -1);
        }
    _findclose( FindHandle );
    free( mask );

    return n;
    }                                   /* list_files */


/*  MAKE_MASK ===== Create search mask for finding files ===== $pageif:6 */

static char* make_mask( const char *spec, const char *dir )
    {
    int ns = strlen( spec );
    int nd = strlen( dir );
    char *mask = malloc( ns + nd + 4 );
    memset( mask, '\0', ns + nd + 3 );
    strcpy( mask, dir );
    if (dir[nd-1] != '\\')
        strcat( mask, "\\" );
    strcat( mask, spec );
    return mask;
    }


/* REAL_DIR ===== False if . or ..; otherwise true ===== $pageif:6 */

static int real_dir( const char *name )
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


/*  SHOW_HELP ===== User instructions =====  $pageif:6 */

static void show_help( void )
    {
    printf( "\nFF (Find Files):  Find files matching file spec in given directorys and sub-directories\n" );
    printf( "\nUsage:  FF [spec] [dir1] [dir2] ...\n" );
    printf( "\nWhere spec = file name to look for (e.g. *.c)\n" );
    printf( "\nWhere dirX = directories to look in (e.g. C:\windows)\n" );
    printf( "\nIf no directories are given, the current directory is used\n" );
    }                                   /* parse_args */
