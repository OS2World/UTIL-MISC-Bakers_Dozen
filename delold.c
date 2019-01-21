/* $title:       'DELOLD.C ===== Delete files older than given date ======' */
/* $subtitle:    'Elipse Ltd.  [jms]                  Revised: 2012 Jan 17' */

/* NOTE: DO NOT link with wildarg.obj     **
**       argument expansion is not wanted */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <io.h>

#define FILE_ATTRIBUTE  32          // normal files
#define HIDE_ATTRIBUTE   2          // hidden files
#define DEFAULT_SHOW    12          // number of files
#define SEC_PER_DAY     (60*60*24)  // seconds in one day

typedef struct
    {
    int year;
    int month;
    int day;
    } simple_date;


int main( int argc, char *argv[] );
void delold( char *mask, time_t limit );
void parse_args( const char *str, char *spec, simple_date *lim );
int is_spec( const char *str, char *spec );
int is_date( const char *str, simple_date *lim );
int is_number( const char *str, simple_date *lim );
simple_date *make_date( int n );
time_t make_time( simple_date *sd );
void show_use( void );


/* MAIN ===== Calling routine ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   // main
    fprintf( stderr, "\n\tDelOld (Delete Old Files) ======= (C) Elipse Limited, %s\n", __DATE__ );
    if (argc < 2)
        show_use( );
    else
        {
        int i;
        char spec[_MAX_PATH + 4];
        simple_date lim_date;
        memset( &lim_date, '\0', sizeof (lim_date) );
        memset( spec, '\0', sizeof (spec) );
        spec[0] = '*';

        for ( i = 1; i < argc;  ++i )
            parse_args( argv[i], spec, &lim_date );

        if (lim_date.year > 1900)
            {
            delold( spec, make_time( &lim_date ) );
            return 0;
            }
        }
    return 1;
    }                                   // main


/* DELOLD ===== Main program ===== $pageif:6 */

void delold( char *mask, time_t limit )
    {                                   // delold
    long FindHandle = 0L;
    struct _finddata_t FindBuffer;
    memset( &FindBuffer, '\0', sizeof (FindBuffer) );

    FindHandle = _findfirst( mask, &FindBuffer );
    if (FindHandle != -1L)
        {
        do
            {
            if (FindBuffer.attrib & FILE_ATTRIBUTE)
                {                       // file, not dir
                if (!(FindBuffer.attrib & HIDE_ATTRIBUTE))
                    {                   // not hidden
                    if (difftime( FindBuffer.time_write, limit ) < 0)
                        if (remove( FindBuffer.name ) == 0)
                            printf( "%s\n", FindBuffer.name );
                        else
                            printf( "%s - Unable to delete file\n", FindBuffer.name );
                    }
                }
            }
        while (_findnext( FindHandle, &FindBuffer ) != -1 );
        }
    _findclose( FindHandle );
    }                                   // delold


/* PARSE_ARGS ===== Decipher user input ===== $pageif:6 */

void parse_args( const char *str, char *spec, simple_date *lim )
    {                                   // parse_args
    if (is_spec( str, spec ))
        return;
    if ((*str == '/') || (*str == '-'))
        ++str;
    if (is_number( str, lim ))
        return;
    if (is_date( str, lim ))
        return;
    fprintf( stderr, "\nInvalid parameter: %s\n", str );
    show_use( );
    }                                   // parse_args


/* IS_SPEC ===== Determine if string is a file spec and return it ===== $pageif:6 */

int is_spec( const char *str, char *spec )
    {                                   // is_spec
    if ((strchr( str, '*' ) != NULL) || (strchr( str, '?' ) != NULL))
        {
        strcpy( spec, str );            // valid file spec
        return 1;
        }
    return 0;
    }                                   // is_spec


/* IS_DATE ===== Determine if string is a date and return it ===== $pageif:6 */

int is_date( const char *str, simple_date *lim )
    {                                   // is_date
    int y, m, d;
    int n = sscanf( str, "%d-%d-%d", &y, &m, &d );
    if (n != 3)
        return 0;
    if ((y >= 0) && (y <= 89))
        y += 2000;
    if ((y >= 90) && (y <= 99))
        y += 1900;
    if ((y < 1990) || (y > 2100))
        return 0;
    if ((m < 1) || (m > 12))
        return 0;
    if ((d < 1) || (d > 31))
        return 0;
    lim->year = y;
    lim->month = m;
    lim->day = d;
    return 1;
    }                                   // is_date


/* IS_NUMBER ===== Determine if string is a number (of days) and return as date ===== $pageif:6 */

int is_number( const char *str, simple_date *lim )
    {                                   // is_number
    int n;
    char *p = (char *) str;
    while (*p != '\0')
        {
        if (!isdigit( *p ))
            return 0;
        ++p;
        }
    n = atoi( str );
    if ((n < 1) || (n > 2000))
        return 0;
    memcpy( lim, make_date( n ), sizeof (simple_date) );
    return 1;
    }                                   // is_number


/* MAKE_DATE ===== Convert number of days into a date (y-m-d) ===== $pageif:6 */

simple_date *make_date( int n )
    {                                   // make_date
    static simple_date sd;
    struct tm *ctm;
    time_t cur = time( NULL );
    cur -= ((long) n * (long) SEC_PER_DAY);
    ctm = localtime( &cur );
    sd.year = ctm->tm_year + 1900;
    sd.month = ctm->tm_mon + 1;
    sd.day =ctm->tm_mday;
    return &sd;
    }                                   // make_date


/* MAKE_TIME ===== Convert date (y-m-d) into a time_t counter ===== $pageif:6 */

time_t make_time( simple_date *sd )
    {                                   // make_time
    struct tm ctm;
    memset( &ctm, 0, sizeof( ctm ) );
    ctm.tm_year = sd->year - 1900;
    ctm.tm_mon = sd->month - 1;
    ctm.tm_mday = sd->day;
    return mktime( &ctm );
    }                                   // make_time


/* SHOW_USE ===== Display instructions ===== $pageif:6 */

void show_use( void )
    {                                   // show_use
    fprintf( stderr, "\nNF (New files):  List the newest files in a directory\n" );
    fprintf( stderr, "\nUsage:  DELOLD [[/][n | yyyy-mm-dd]] [spec]\n" );
    fprintf( stderr, "\nWhere spec = files to look for (e.g. file0*.chk)\n" );
    fprintf( stderr, "And n = maximum number of days old to keep\n" );
    fprintf( stderr, "Or yyyy-mm-dd = delete files older than this date\n" );
    }                                   // show_use
