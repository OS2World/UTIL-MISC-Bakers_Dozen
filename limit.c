/* $title:      'LIMIT.C ===== To show only the first n lines =====' */
/* $subtitle:   'Elipse Ltd.  [jms]                     2005 Mar 29' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE       4090
#define DEF_LINES       16

int main( int argc, char *argv[] );
static void limit( int numlin );


/* MAIN ===== Start of program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int numlin = DEF_LINES;
    fprintf( stderr, "\nLIMIT, (C) Elipse Ltd., Mar 2005\n\n" );

    if (argc > 1)
        {
        int n = atoi( argv[1] );
        if (n > 0)
            numlin = n;
        }

    limit( numlin );
    return 0;
    }                                   /* main */


/* LIMIT ===== Program body ===== $pageif:6 */

static void limit( int numlin )
    {                                   /* limit */
    char buf[LINE_SIZE + 2];
    int n = numlin;
    memset( buf, '\0', sizeof( buf ) );

    while ((gets( buf ) != NULL) && (--n >= 0))
        puts( buf );
    }                                   /* limit */
