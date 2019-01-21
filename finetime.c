/* $title:      'FINETIME.C ===== Test Hi-Res timer driver =====' */
/* $subtitle:   'Elipse Ltd.  [jms]         Revised: 1999 Aug 06' */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main( int argc, char *argv[] );
static void finetime( int num_rep, int wait_ms );

static int suspend_for( int num_rep, int wait_ms );
static int suspend_open( void );
static int suspend_close( void );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\n FINETIME ===== Test hi-res timer =====\n " );
    fprintf( stderr, "\n         (C) Elipse Ltd. %s\n\n ", __DATE__ );
    if (argc > 2)
        {
        finetime( atoi( argv[1] ), atoi( argv[2] ) );
        return 0;
        }
    fprintf( stderr, "\nMust give number of repeats and wait time in ms\n" );
    return 1;
    }                                   /* main */


/* FINETIME ===== Body of program ===== $pageif:6 */

static void finetime( int num_rep, int wait_ms )
    {                                   /* finetime */
    long tm = 0;
    suspend_open( );
    printf( "\n Wait for a total of %5.3f seconds\n", 
                        ((float) (num_rep) * (float) (wait_ms) / 1000.0) );

    tm = clock( );
    suspend_for( num_rep, wait_ms );
    tm = clock( );
    printf( "\n Actual elapsed time: %5.3f seconds\n", tm / (float) CLOCKS_PER_SEC );

    suspend_close( );
    }                                   /* finetime */


/********************************************************************************************/


#define INCL_NOPMAPI
#define INCL_DOSFILEMGR
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL

#include <os2.h>

#define HRT_IOCTL_CATEGORY      0x80
#define HRT_BLOCKUNTIL          0x05

static LHANDLE hfile = (LHANDLE) NULL;


/* SUSPEND_FOR ===== Actual delay using timer ===== $pageif:6 */

static int suspend_for( int num_rep, int wait_ms )
    {                                   /* suspend_for */
    int i;
    ULONG ulDelay = (ULONG) wait_ms;
    ULONG ulSize2 = (ULONG) sizeof( ulDelay );

    for ( i = 0;  i < num_rep;  ++i )
        {
        APIRET rc = DosDevIOCtl( hfile, HRT_IOCTL_CATEGORY, HRT_BLOCKUNTIL, 
                    &ulDelay, ulSize2, &ulSize2, NULL, 0, NULL );
        if ( rc != 0L )
            {
            fprintf( stderr, "\nDosIOCtl failed. Code=%ld\n", rc );
            return rc;
            }
        }
    return 0;
    }                                   /* suspend_for */


/* SUSPEND_OPEN ===== Prepare for use ===== $pageif:6 */

static int suspend_open( void )
    {                                   /* suspend_open */
    ULONG ulAction = (ULONG) 0;
    ULONG ulAttribute = (ULONG) ( FILE_NORMAL );
    ULONG ulOpenFlag = (ULONG) ( OPEN_ACTION_OPEN_IF_EXISTS );
    ULONG ulOpenMode = (ULONG) ( OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYREADWRITE );
    APIRET rc = DosOpen( "TIMER0$ ", &hfile, &ulAction, 0, 
                    ulAttribute, ulOpenFlag, ulOpenMode, NULL );
    if ( rc != 0L )
        fprintf( stderr, "\nDosOpen failed. Code=%ld\n", rc );
    return rc;
    }                                   /* suspend_open */


/* SUSPEND_CLOSE ===== Finish using ===== $pageif:6 */

static int suspend_close( void )
    {                                   /* suspend_close */
    APIRET rc = DosClose( hfile );
    if ( rc != 0L )
        fprintf( stderr, "\nDosClose failed. Code=%ld\n", rc );
    hfile = (LHANDLE) NULL;
    return rc;
    }                                   /* suspend_close */
