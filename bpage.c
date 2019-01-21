/* title:       'BPAGE.C ===== Calc Page Size Table =====' */
/* subtitle:    'Elipse Ltd.  [jms]  Revised: 1998 Jan 21' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OVERHEAD    6

int  main( int argc, char *argv[] );
static void bpage( int recsz );
static void heading( void );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int i;
    fprintf( stderr, "\n\n\tBpage ===== Btrieve Page Table =====\n\n" );

    if ( argc < 2 )
        {
        fprintf( stderr, "\n\tMust give one or more record sizes\n" );
        return 1;
        }

    heading( );
    for ( i = 1;  i < argc;  ++i )
        {
        int sz = atoi( argv[i] );
        if (sz > 0)
            bpage( sz );
        }
    return 0;
    }                                   /* main */


/* BPAGE ===== Body of program ===== $pageif:6 */

static void bpage( int recsz )
    {                                   /* bpage */
    int pagesz;
    printf( "\n" );
    for( pagesz = 512;  pagesz <= 4096;  pagesz += 512 )
        {
        int availsz = (pagesz - OVERHEAD);
        int numrec = availsz / recsz;
        int usedsz = numrec * recsz;
        int wastesz = availsz - usedsz;
        printf( "%9d%9d%9d%9d%9d\n", pagesz, recsz, numrec, usedsz, wastesz );
        }
    printf( "\n" );
    }                                   /* bpage */


/* HEADING ===== Column headings ===== $pageif:6 */

static void heading( void )
    {                                   /* heading */
    printf( "\n   PageSz    RecSz   NumRec   UsedSz  WasteSz \n" );
    }                                   /* heading */

