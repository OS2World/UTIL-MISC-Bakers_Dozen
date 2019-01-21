/* $title:       'UPDATE.C ===== Copy newer files to target ======' */
/* $subtitle:    'Elipse Ltd.  [jms]          Revised: 2008 May 23' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <io.h>

#define BUF_SIZE    4096

       int  main( int argc, const char *argv[] );
static int  update( const char *filename );
static int  copyfile( const char *filename, const char *fileto );
static long dateof( const char *filename );
static int  barename( char *bare, const char *full );


/* MAIN ===== Calling routine ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    int i, num_files = 0;
    fprintf( stderr, "\n\tUPDATE ======= (C) Elipse Limited, %s\n\n", __DATE__ );
    if (argc < 2)
        {
        fprintf( stderr, "\n\tusage: UPDATE filespec ...\n" );
        return 1;
        }

    for (i = 1;  i < argc;  ++i)
        if (update( argv[i] ))
            ++num_files;

    if (num_files == 0)
        fprintf( stderr, "\nNo files copied.\n" );
    else
        fprintf( stderr, "\n%u file(s) copied.\n", num_files );
    return 0;
    }                                   /* main */


/* UPDATE ===== Compare and decide ===== $pageif:6 */

static int update( const char *filename )
    {                                   /* update */
    int ret = 0;
    char fileto[512];
    memset( fileto, '\0', sizeof( fileto ) );
    if (barename( fileto, filename ))
        {
        long datefr = dateof( filename );
        if (datefr > 0L)
            {
            long dateto = dateof( fileto );
            if (datefr > dateto)
                if (copyfile( filename, fileto ))
                    ret = 1;
            }
        }
    return ret;
    }                                   /* update */


/* COPYFILE ===== Actual file copy ===== $pageif:6 */

static int copyfile( const char *filename, const char *fileto )
    {                                   /* copyfile */
    char buf[BUF_SIZE + 4];
    int ret = 1;
    int bytes_in = 0;
    int bytes_out = 0;
    int rd_mode = O_RDONLY | O_BINARY | O_EXCL;
    int wt_mode = O_WRONLY | O_BINARY | O_CREAT | O_TRUNC;
    int permits = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    int src = open( filename,  rd_mode );
    int dst = open( fileto,    wt_mode | permits );
    memset( buf, '\0', sizeof( buf ) );

    while ((bytes_in = read( src, buf, BUF_SIZE )) > 0)
        {
        bytes_out = write( dst, buf, bytes_in );
        if (bytes_out != bytes_in)
            {
            printf( "Bytes in  = %d\n", bytes_in );
            printf( "Bytes out = %d\n", bytes_out );
            break;
            }
        }

    close( src );
    close( dst );
    return ret;
    }                                   /* copyfile */


/* DATEOF ===== Return file date if exists ===== $pageif:6 */

static long dateof( const char *filename )
    {                                   /* dateof */
    struct stat buf;
    memset( &buf, '\0', sizeof( buf ) );
    if (stat( (char *) filename, &buf ) == 0)
        if (buf.st_mode & (unsigned) S_IFREG)
            return buf.st_mtime;
    return 0L;
    }                                   /* dateof */


/* BARENAME ===== Strip path and get file only ===== $pageif:6 */

static int barename( char *bare, const char *full )
    {                                   /* barename */
    char *p = strrchr( full, '\\' );    /* check subdirectories */
    if (p == NULL)
        {
        p = strrchr( full, ':' );       /* check disk */
        if (p == NULL)
            return 0;                   /* failure */
        }
    strcpy( bare, (p + 1) );
    return 1;                           /* success */
    }                                   /* barename */
