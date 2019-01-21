/* $title:      'SORT.C ===== To sort from stdin to stdout =====' */
/* $subtitle:   'Elipse Ltd.  [jms]                  1996 Nov 20' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LINSIZ      2048
#define ARRSIZ      20000

int reverse = 0;
int indent = 0;

int main( int argc, char *argv[] );
int sort( void );
int compare( const void *arg1, const void *arg2 );
int switches( int argc, char *argv[], char *asw[] );


/* MAIN ===== Start of program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    char *asw[7];
    int i = 0;
    reverse = 0;
    indent = 0;
    fprintf( stderr, "\nSORT, (C) Elipse Ltd., Nov 1999\n" );

    if ((argc > 3) || (switches( argc, argv, asw ) != 1))
        {
        fprintf( stderr, "\nSORT called improperly \n" );
        fprintf( stderr, "PROPER CALL: sort <infile >outfile\n" );
        fprintf( stderr, "SWITCHES AVAILABLE: /R (reverse) /+n (start position)\n" );
        return 1;
        }

    while ((asw[++i] != NULL) && (i < 6))
        {
        if (toupper( *asw[i] ) == 'R')
            reverse = 1;
        if (*asw[i] == '+')
            indent = atoi( 1 + asw[i] ) - 1;
        }
    if ((indent < 1) || (indent > LINSIZ))
        indent = 0;

    return sort();
    }                                   /* main */


/* SORT ===== Program body ===== $pageif:6 */

int sort( void )
    {                                   /* sort */
    char **array, buf[LINSIZ];
    unsigned n = 0;
    int ret = 0;
    int i;

    if ((array = (char **) calloc( ARRSIZ, sizeof(char **) )) == NULL)
        {
        fprintf( stderr, "Out of memory for array!" );
        return 99;
        }

    while ((n < ARRSIZ) && (gets( buf ) != NULL))
        {
        if ((array[n++] = strdup( buf )) == NULL)
            {
            fprintf( stderr, "Out of memory for strings!" );
            ret = 2;
            break;
            }
        }

    qsort( array, n, sizeof(char*), compare );

    if (reverse)
        for (i = (int) (n - 1);  i >= 0;  --i)
            puts( array[i] );
    else 
        for (i = 0;  i < (int) n;  ++i)
            puts( array[i] );

    return ret;
    }                                   /* sort */


/* COMPARE ===== Sort string pointers ====== $pageif:6 */

int compare( const void *arg1, const void *arg2 )
    {                                   /* compare */
    return strcmp( *(char **)arg1 + indent, *(char **)arg2 + indent );
    }                                   /* compare */


/*  SWITCHES ===== Decipher switch settings ===== $pageif:6 */

int switches( int argc, char *argv[], char *asw[] )
    {                                   /* switches */
    int i = 0, j = 0, k = 0;
    char *p;
    asw[0] = NULL;

    while (++i < argc)                  /* remove switches from argv list */
        {
        if (*argv[i] == '/')
            asw[++j] = argv[i] + 1;
        else 
            argv[++k] = argv[i];
        }

    for (i = 1;  i <= j;  ++i)          /* seperate combined switches */
        {
        while ((p = strchr( asw[i], '/' )) != NULL)
            {
            *p = '\0';
            asw[++j] = p + 1;
            }
        }

    asw[++j] = NULL;
    argv[++k] = NULL;
    return k;                           /* the reduced argv[] count */
    }                                   /* switches */
