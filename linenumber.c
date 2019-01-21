/* $title:      'LINENUMBER.C ===== To add or remove line numbers =====' */
/* $subtitle:   'Elipse Ltd.  [jms]                         2012 Nov 21' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXBUF          8190

int   lines_out = 0;
char  delim = '.';

int   main( int argc, char *argv[] );
void linenumber( const char* fname );
int   compare( const void *arg1, const void *arg2 );
char* numout( char* str );
int   check_switch( int argc, char *argv[] );
int   is_switch( char *arg );


/* MAIN ===== Start of program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int i = 0;
    fprintf( stderr, "\nLINENUMBER, (C) Elipse Ltd., Nov 2012\n" );

    if (argc < 2)
        {
        fprintf( stderr, "\nUSAGE: linenumber [switch] infile1 [infile2] ...\n" );
        fprintf( stderr, "\nSWITCHES AVAILABLE: + to add line numbers (default)\n" );
        fprintf( stderr, "                    - to remove line numbers\n" );
        fprintf( stderr, " any other non alphanumeric as delimiter\n" );
        return 1;
        }

    lines_out = check_switch( argc, argv );
    for (i = 1;  i < argc;  ++i)
        {
        if (!is_switch( argv[i] ))
            linenumber( argv[i] );
        }
    return 0;
    }                                   /* main */


/* LINENUMBER ===== Program body ===== $pageif:6 */

void linenumber( const char* fname )
    {                                   /* linenumber */
    char buf[MAXBUF + 2];
    unsigned line = 0;
    FILE *fp = NULL;
    memset( buf, '\0', sizeof( buf ) );

    fprintf( stderr, "Reading %s\n", fname );
    fp = fopen( fname, "rb" );
    if (fp != NULL)
        {
        while (fgets( buf, MAXBUF, fp ) != NULL)
            {
            ++line;
            if (lines_out)
                printf( "%s", numout( buf ) );
            else
                printf( "%4d%c %s", line, delim, buf );
            }
        fclose( fp );
        }

    }                                   /* linenumber */


/*  NUMOUT ===== Remove line number from fron of string ===== */

char* numout( char* str )
    {                                   /* numout */
    int state = 0;
    char *p = str;
    while (*p != '\0')
        {
        if (isdigit( *p ))
            state = 1;
        if ((state == 1) && isspace( *p ))
            state = 2;
        if ((state == 2) && !isspace( *p ))
            break;
        ++p;
        }
    return p;
    }                                   /* numout */


/*  CHECK_SWITCH ===== Decipher switch settings ===== $pageif:6 */

int check_switch( int argc, char *argv[] )
    {                                   /* check_switch */
    int i = 0;
    int ret = 0;
    for (i = 1;  i < argc;  ++i)
        {
        if (is_switch( argv[i] ))
            {
            if (*argv[i] == '-')
                ret = 1;                /* ask remove */
            if (*argv[i] == '+')
                ret = 0;                /* ask insert */
            delim = *argv[i];
            }
        }
    return ret;                         /* default insert */
    }                                   /* check_switch */


/*  IS_SWITCH ===== Return true if switch found ===== $pageif:6 */

int is_switch( char *arg )
    {                                   /* is_switch */
    if ((strlen( arg ) == 1) && (!isalnum( *arg )))
        return 1;                       /* switch found */
    return 0;                           /* not switch */
    }                                   /* is_switch */
