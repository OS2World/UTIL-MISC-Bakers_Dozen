/* $title:      'DUPOUT.C ===== Remove duplicate lines =====' */
/* $subtitle:   'Elipse Ltd.  [jms]              2000 Jan 28' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINSIZ      4096

static int test_len = 0;

int main( int argc, char *argv[] );
static void dupout( void );
static int duplicate( const char *str );


/* MAIN ===== Start of program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\nDUPOUT, (C) Elipse Ltd. %s\n",  __DATE__ );
    if (argc > 1)
        test_len = atoi( argv[1] );
    if (test_len < 0)
        {
        fprintf( stderr, "\nERROR: number of characters must be 0 or positive\n" );
        return 1;
        }
    if (test_len > LINSIZ)
        {
        fprintf( stderr, "\nERROR: number of characters must be less than %d\n", LINSIZ );
        return 2;
        }
    if (test_len > 0)
        fprintf( stderr, "\nBased on first %d characters\n", test_len );
    else
        fprintf( stderr, "\nExact match on record\n" );
    dupout( );
    fprintf( stderr, "\nEnd of program\n" );
    return 0;
    }                                   /* main */


/* DUPOUT ===== Program body ===== $pageif:6 */

static void dupout( void )
    {                                   /* dupout */
    char buf[LINSIZ + 4];
    unsigned lines_in = 0;
    unsigned lines_out = 0;
    memset( buf, '\0', sizeof( buf ) );
    duplicate( "" );                    /* initialize */

    while (gets( buf ) != NULL)
        {
        ++lines_in;
        if (!duplicate( buf ))
            {
            puts( buf );
            ++lines_out;
            }
        }

    fprintf( stderr, "\nComplete...\n" );
    fprintf( stderr, "%5u lines read\n", lines_in );
    fprintf( stderr, "%5u lines written\n", lines_out );
    }                                   /* dupout */


/* DUPLICATE ===== Check if match previous ===== $pageif:6 */

static int duplicate( const char *str )
    {                                   /* dupout */
    static char prv[LINSIZ + 4] = { 0 };
    if ((test_len == 0) && (strcmp( str, prv ) == 0))
        return 1;                       /* full match */
    if ((test_len > 0) && (strncmp( str, prv, test_len ) == 0))
        return 1;                       /* partial match */
    strncpy( prv, str, sizeof( prv ) );
    return 0;                           /* no match */
    }                                   /* dupout */
