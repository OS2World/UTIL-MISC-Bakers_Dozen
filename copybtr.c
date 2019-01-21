/* $title:      'COPYBTR.C ===== Copy Btrieve file even if open =====' */
/* $subtitle:   'Elipse Ltd.  [jms]              Revised: 2002 Dec 11' */


#include "cpyright.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pstruct.h"
#include <bfile.h>

static int hand1 = -1;
static int hand2 = -1;

int main( int argc, char *argv[] );
static void copybtr( const char *ffrom, const char *fto, int reclen );
static int  open_files( const char *ffrom, const char *fto, int reclen );
static void copyloop( int reclen );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\nCopyBtr ===== Back up Btrieve file =====\n" );
    if (argc < 4)
        {
        fprintf( stderr, "\nMust provide source and destination file names and record size\n" );
        return 1;
        }
    copybtr( argv[1], argv[2], atoi( argv[3] ) );
    fprintf( stderr, "\nEnd program\n" );
    return 0;
    }                                   /* main */


/* COPYBTR ===== Body of program ====== $pageif:6 */

static void copybtr( const char *ffrom, const char *fto, int reclen )
    {                                   /* copybtr */
    if ((ffrom == NULL) || (strlen( ffrom ) == 0))
        {
        printf( "\nSource file not given\n" );
        return;
        }
    if ((fto == NULL) || (strlen( fto ) == 0))
        {
        printf( "\nDestination file not given\n" );
        return;
        }
    if (reclen < 1)
        {
        printf( "\nRecord length not given\n" );
        return;
        }
    if (open_files( ffrom, fto, reclen ))
        copyloop( reclen );
    b_closeall( );
    }                                   /* copybtr */


/*  OPEN_FILES ===== Both source and destination ===== $pageif:6 */

static int open_files( const char *ffrom, const char *fto, int reclen )
    {                                   /* open_files */
    init_bfiles( 0 );
    hand1 = b_open( ffrom, reclen, B_OPEN_RO );
    if (hand1 < 0)
        {
        printf( "\nError opening %s: %d\n", ffrom, hand1 );
        return 0;                       /* failure */
        }
    hand2 = b_open( fto, reclen, B_OPEN_ACCL );
    if (hand2 < 0)
        {
        printf( "\nError opening %s: %d\n", fto, hand2 );
        return 0;                       /* failure */
        }
    return 1;                           /* success */
    }                                   /* open_files */


/* COPYLOOP ===== Through all records ===== $pageif:6 */

static void copyloop( int reclen )
    {                                   /* copyloop */
    unsigned count = 0;
    int sts = 0;
    char *rec = malloc( reclen );
    if (rec == NULL)
        {
        printf( "\nUnable to allocate memory\n" );
        return;
        }
    memset( rec, '\0', reclen );

    sts = search_first( hand1, rec );
    while (sts == 0)
        {
        sts = add_rec( hand2, rec );
        if (sts != 0)
            printf( "Error %d inserting record\n", sts );
        else
            ++count;
        sts = search_next( hand1, rec );
        }
    if (sts != 9)
        printf( "Error %d reading record\n", sts );

    printf( "\n%8u records copied\n", count );
    free( rec );
    }                                   /* copyloop */
