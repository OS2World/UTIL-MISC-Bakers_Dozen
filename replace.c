/* title:       'REPLACE.C ===== Replace string wiht another =====' */
/* subtitle:    'Elipse Ltd.  [jms]           Revised: 2008 Mar 21' */

/**
    Filter stdin to stdout where
    the first string is replaced by the second
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE        8190

int main( int argc, char *argv[] );
static void replace( void );
static void convert( char *data );

static char *from = NULL;
static char *to = NULL;
static int lgth = 0;


/* MAIN ===== Entry point for program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\nREPLACE ===== (C) 2008, Elipse Ltd.\n" );
    if (argc < 3)
        {
        fprintf( stderr, "\nNeed arguments FROM and TO\n" );
        return 1;
        }
    lgth = strlen( argv[1] );
    if (lgth == 0)
        {
        fprintf( stderr, "\nFrom string cannot be empty\n" );
        return 2;
        }
    if (strcmp( argv[1], argv[2] ) == 0 )
        {
        fprintf( stderr, "\nMust substitute different string\n" );
        return 3;
        }
    from = argv[1];
    to = argv[2];
    replace( );
    return 0;
    }                                   /* main */


/* REPLACE ===== Body of program ===== $pageif:6 */

static void replace( void )
    {                                   /* replace */
    char data[BUF_SIZE + 2];
    memset( data, '\0', BUF_SIZE + 1 );
    while (fgets( data, BUF_SIZE, stdin ) != NULL)
        convert( data );
    }                                   /* replace */


/* CONVERT ===== Actual replacement and output ===== $pageif:6 */

static void convert( char *data )
    {                                   /* convert */
    while (*data != '\0')
        {
        char *p = strstr( data, from );
        if (p == NULL)                  /* not found */
            break;
        *p = '\0';
        fputs( data, stdout );
        fputs( to, stdout );
        data = p + lgth;
        }
    fputs( data, stdout );
    }                                   /* convert */
