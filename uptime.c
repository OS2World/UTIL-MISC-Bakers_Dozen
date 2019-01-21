/* $title:      'UPTIME.C ===== Display time since boot =====' */
/* $subtitle:   'Elipse Ltd.  [jms]      Revised: 1998 Aug 28' */


#define INCL_DOSMISC
#define INCL_DOSERRORS
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static int uptime( void );
static char bootdrv( void );
static char *swappath( char drv, char *pth );
static time_t creatime( char *fname );
static void printdur( long elapsed );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {
    fprintf( stderr, "\n\tUpTime ======= (C) Elipse Limited, %s\n\n", __DATE__ );
    return uptime( );
    }


/* UPTIME ===== Body of program ===== $pageif:6 */

static int uptime( void )
    {                                   /* uptime */
    char drv = '\0';
    char pth[260];
    time_t start, now;
    memset( pth, '\0', sizeof( pth ) );

    drv = bootdrv( );
    if (drv <= ' ')
        return 1;
    if (swappath( drv, pth ) == NULL)
        return 2;
    strcat( pth, "\\SWAPPER.DAT" );
    start = creatime( pth );
    if ((long) start <= 0L)
        return 3;
    printf( "    Boot time: %s", ctime( &start ) );
    now = time( NULL );
    if ((long) now <= 0L)
        return 4;
    printf( " Current time: %s", ctime( &now ) );
    printdur( now - start );
    return 0;
    }                                   /* uptime */


/* BOOTDRV ===== Body of program ===== $pageif:6 */

static char bootdrv( void )
    {                                   /* bootdrv */
    const static char all[] = "?ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    ULONG info = 0;
    APIRET rc = DosQuerySysInfo( QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, (PVOID) &info, sizeof( info ) );

    if (rc == NO_ERROR)
        {
        if ((info > 0) && (info < sizeof( all )))
            return all[info];
        else
            {
            printf( "\nImpossible drive value of %d\n", info );
            return '\0';
            }
        }

    fprintf( stderr, "\nDosQuerySysInfo error: code = %ld\n", rc );
    return '\0';
    }                                   /* bootdrv */


/* SWAPPATH ===== Read config.sys to get swapper path ===== $pagif:6 */

static char *swappath( char drv, char *pth )
    {                                   /* swappath */
    FILE *cfg = NULL;
    char buf[2048];
    memset( buf, '\0', sizeof( buf ) );
    *pth = '\0';
    *buf = drv;
    strcat( buf, ":\\config.sys" );
    cfg = fopen( buf, "r" );
    if (cfg == NULL)
        {
        fprintf( stderr, "\nUnable to find CONFIG.SYS\n" );
        return NULL;
        }
    while (fgets( buf, sizeof(buf), cfg ) != NULL)
        {
        if (strnicmp( buf, "SWAPPATH=", 9 ) == 0)
            {
            sscanf( &buf[9], "%s", pth );
            break;
            }
        }
    fclose( cfg );
    if (*pth < ' ')
        {
        fprintf( stderr, "\nUnable to find 'SWAPPATH=' in CONFIG.SYS\n" );
        return NULL;
        }
    return pth;
    }                                   /* swappath */


/* CREATIME ===== Get creation time of given file ===== $pageif:6 */

static time_t creatime( char *fname )
    {                                   /* creatime */
    FILESTATUS3 full_stat;
    APIRET rc;
    memset( &full_stat, '\0', sizeof( FILESTATUS3 ) );
    rc = DosQueryPathInfo( fname, FIL_STANDARD, &full_stat, (ULONG) sizeof( FILESTATUS3 ) );
    if (rc == NO_ERROR)
        {
        struct tm tmst;
        memset( &tmst, '\0', sizeof( struct tm ) );
        tmst.tm_mday = (int) full_stat.fdateCreation.day;
        tmst.tm_mon = (int) (full_stat.fdateCreation.month) - 1;
        tmst.tm_year = (int) (full_stat.fdateCreation.year) + 80;
        tmst.tm_isdst = -1;
        tmst.tm_sec = (int) (full_stat.ftimeCreation.twosecs) << 1;
        tmst.tm_min = (int) full_stat.ftimeCreation.minutes;
        tmst.tm_hour = (int) full_stat.ftimeCreation.hours;
        return mktime( &tmst );
        }
    fprintf( stderr, "\nDosQueryPathInfo error: code = %ld\n", rc );
    return (time_t) 0;
    }                                   /* creatime */


/* PRTDUR ==== Print elapsed time ===== $pageif:6 */

static void printdur( long elapsed )
    {                                   /* printdur */
    if (elapsed < 0L)
        fprintf( stderr, "\nNegative elapsed time of %ld\n", elapsed );
    else
        {
        long w, d, h, m, s;
        s = elapsed;
        m = s / 60L;
        s = s % 60L;
        h = m / 60L;
        m = m % 60L;
        d = h / 24L;
        h = h % 24L;
        w = d / 7L;
        d = d % 7L;
        printf( "\n Time since last boot is " );
        if (w > 0L)
            printf( "%ld weeks, %ld days, %ld hours, %ld minutes, %ld seconds", w, d, h, m, s );
        else if (d > 0L)
            printf( "%ld days, %ld hours, %ld minutes, %ld seconds", d, h, m, s );
        else if (h > 0L)
            printf( "%ld hours, %ld minutes, %ld seconds", h, m, s );
        else if (m > 0L)
            printf( "%ld minutes, %ld seconds", m, s );
        else
            printf( "%ld seconds", s );
        printf( "\n\n" );
        }
    }                                   /* printdur */

