/* $title:      'KeepUp.C ===== Dispatch & track work objects =====' */
/* $subtitle:   'Elipse Ltd.  [jms]            Revised: 2000 Jun 20' */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define LOG_NAME     "KeepUp.log"
#define TIC_FACT    (1000L / CLOCKS_PER_SEC)

static int quit = 0;

int main( int argc, char *argv[] );
static void keep_up( int argc, char *argv[] );
static void launch( const char *cmd );
static void log( const char *msg );
static void waitfor( unsigned ms );
static char *time_string( void );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\nKeepUP ==== Restart program as needed\n" );
    if (argc < 2)
        {
        fprintf( stderr, "\nArguments:\n" );
        fprintf( stderr, "\tProgram to run [and its arguments]\n" );
        return 1;
        }
    keep_up( argc, argv );
    return 0;
    }                                   /* main */


/* KEEP_UP ===== Body of program ===== $pageif:6 */

static void keep_up( int argc, char *argv[] )
    {                                   /* keep_up */
    int i;
    char param[256];
    memset( param, '\0', sizeof( param ) );
    for (i = 1;  i < argc;  ++i)
        {
        strcat( param, argv[i] );
        strcat( param, " " );
        }
    launch( param );
    }                                   /* keep_up */


/* LAUNCH ===== Start up single work unit ===== $pageif:6 */

static void launch( const char *cmd )
    {                                   /* launch */
    char msg[256];
    int rc = 0;
    memset( msg, '\0', sizeof( msg ) );
    while (!quit)
        {
        sprintf( msg, "%s Begin", cmd );
        log( msg );
        rc = system( cmd );
        if (rc == 0)
            sprintf( msg, "%s Ended normally", cmd );
        else
            sprintf( msg, "%s Ended with code %d", cmd, rc );
        log( msg );
        }
    }                                   /* launch */


/* LOG ===== Output message ===== $pageif:6 */

static void log( const char *msg )
    {                                   /* log */
    FILE *logfile = NULL;
    int count = 10;
    while ((logfile == NULL) && (--count > 0))
        {
        waitfor( 200 );
        logfile = fopen( LOG_NAME, "a" );
        }
    if (logfile != NULL)
        {
        fprintf( logfile, "%s  %s\n", time_string( ), msg );
        fclose( logfile );
        }
    }                                   /* log */



/* WAITFOR ===== Wait for time or keypress ===== $pageif:6 */

static void waitfor( unsigned ms )
    {                                   /* waitfor */
    long stime = (long) clock( ) * TIC_FACT;
    long etime = stime + (long) ms;
    while ( ((long) clock( ) * TIC_FACT) < etime )
        ;
    }                                   /* waitfor */


/* TIME_STRING ===== Return date & time string ===== $pageif:6 */

static char *time_string( void )
    {                                   /* time_string */
    static char tstr[32];
    struct tm *st = NULL;
    time_t cd = time( &cd );
    memset( tstr, '\0', sizeof( tstr ) );
    st = localtime( &cd );
    if (st == NULL)
        strcpy( tstr, "0000-00-00 00:00:00 \0" );
    else
        sprintf( tstr, "%04d-%02d-%02d %02d:%02d:%02d \0", 
            st->tm_year + 1900, st->tm_mon + 1, st->tm_mday, st->tm_hour, st->tm_min, st->tm_sec );
    return tstr;
    }                                   /* time_string */
