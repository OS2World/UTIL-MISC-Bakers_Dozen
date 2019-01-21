/* $title:       'Qprint.c ===== Copy stdin to printer queue =====' */
/* $subtitle:   'Elipse Ltd.  [jms]          Revised: 1998 Jul 30' */


#define INCL_SPL
#define INCL_SPLDOSPRINT
#define INCL_BASE
#define INCL_SPLERRORS
#define INCL_DOSERRORS

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static HSPL hspl = (HSPL) 0;

static void qprint( char *qname );
static void qbody( void );
static char *qdriver( char *qname );
static HSPL qopen( char *qname, char *driver, char *title );
static void qwrite( const char *buf );
static unsigned long qclose( void );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\nQPRINT ===== Send stdin to given print queue\n" );
    if (argc < 2)
        {
        fprintf( stderr, "\nMust specify queue name\n" );
        return 127;
        }
    qprint( argv[1] );
    fprintf( stderr, "\nQPRINT ===== End program\n" );
    return 0;
    }                                   /* main */


/* QPRINT ===== Body of program ===== $pageif:6 */

static void qprint( char *qname )
    {                                   /* qprint */
    char *dr = qdriver( qname );
    if (dr != NULL)
        {
        char drivname[256];
        strncpy( drivname, dr, sizeof( drivname ) );
        hspl = qopen( qname, drivname, "Qprint Test!" );
        if (hspl != (HSPL) 0)
            {
            unsigned long jobid;
            char reset[8];
            sprintf( reset, "%cE\0", 27 );
            qwrite( reset );
            qbody( );
            qwrite( reset );
            jobid = qclose( );
            fprintf( stderr, "\nJob ID     - %lu\n", jobid );
            }
        }
    }                                   /* qprint */


/* QBODY ===== Output the data ===== $pageif:6 */

static void qbody( void )
    {                                   /* qbody */
    char buf[1024];
    memset( buf, '\0', sizeof( buf ) );
    fprintf( stderr, "\n============\n" );
    while (gets( buf ) != NULL)
        {
        qwrite( buf );
        qwrite( "\r\n" );
        }
    fprintf( stderr, "\n============\n" );
    }                                   /* qbody */


/* QDRIVER ===== Return driver name for queue ===== $pageif:6 */

static char *qdriver( char *qname )
    {                                   /* qdriver */
    static char driver_name[256];
    ULONG splerr = 0L;
    ULONG szbuf = 0L;
    ULONG szneeded = 0L;
    PVOID bufr = NULL;
    PPRQINFO3 prq3;
    memset( driver_name, '\0', sizeof( driver_name ) );

    splerr = SplQueryQueue( NULL, qname, 3L, (PVOID) NULL, 0L, &szneeded );

    if (splerr == NERR_QNotFound)
        {
        fprintf( stderr, "\nInvalid Print Queue! (%s)\n", qname );
        return NULL;
        }
    if ((splerr != NERR_BufTooSmall) && (splerr != ERROR_MORE_DATA))
        {
        fprintf( stderr, "SplQueryQueue Error = %ld, szneeded = %ld\n", splerr, szneeded );
        return NULL;
        }

    if ((szneeded > 0) && ((bufr = _alloca( szneeded )) != NULL))
        {
        szbuf = szneeded;
        splerr = SplQueryQueue( NULL, qname, 3L, bufr, szbuf, &szneeded );
        prq3 = (PPRQINFO3) bufr;
        fprintf( stderr, "\nQueue name - %s\n", prq3->pszName );
        fprintf( stderr, "\nIcon Label - %s\n", prq3->pszComment );
        printf( "\n" );
        fprintf( stderr, "\nDriver     - %s\n", prq3->pszDriverName );
        if (prq3->pDriverData)
            fprintf( stderr, "\nPrinter    - %s\n", prq3->pDriverData->szDeviceName );
        strncpy( driver_name, prq3->pszDriverName, sizeof( driver_name ) );
        }
    return driver_name;
    }                                   /* qdriver */


/* QOPEN ===== Body of program ===== $pageif:6 */

static HSPL qopen( char *qname, char *driver, char *title )
    {                                   /* qopen */
    HSPL tspl;                          /* Temporary spool handle */
    PDEVOPENSTRUC pdata;                /* Pointer to a DEVOPENSTRUC structure */
    pdata = _alloca( sizeof( DEVOPENSTRUC ) );  /* Allocate memory for pdata */
    if (pdata != NULL)
        {                                       /* Initialize elements of pdata */
        pdata->pszLogAddress      = qname;
        pdata->pszDriverName      = driver;
        pdata->pdriv              = NULL;
        pdata->pszDataType        = "PM_Q_RAW";
        pdata->pszComment         = "Qprint";
        pdata->pszQueueProcName   = NULL;
        pdata->pszQueueProcParams = NULL;
        pdata->pszSpoolerParams   = NULL;
        pdata->pszNetworkParams   = NULL;

        tspl = SplQmOpen( "*", 4L, (PQMOPENDATA) pdata );
        if (tspl != (HSPL) 0)
            if (SplQmStartDoc( tspl, title ))
                return tspl; 
        }
    fprintf( stderr, "\nFailed to open (or start doc) printer queue %s\n", qname );
    return (HSPL) 0;
    }                                   /* qopen */

 
/* QWRITE ===== Output single line to queue ===== $pageif:6 */

static void qwrite( const char *buf ) 
    {                                   /* qwrite */
    if (!SplQmWrite( hspl, strlen( buf ), (PVOID) buf ))
        fprintf( stderr, "\nFailed to write %s\n", buf );
    }                                   /* qwrite */

 
/* QCLOSE ===== Close the queue spool ===== $pageif:6 */

static unsigned long qclose( void )
    {                                   /* qclose */
    ULONG jobid = SplQmEndDoc( hspl );
    if (jobid == 0L)
        fprintf( stderr, "\nFailed to end doc\n" );
    if (!SplQmClose( hspl ))
        fprintf( stderr, "\nFailed to close\n" );
    return jobid;
    }                                   /* qclose */
