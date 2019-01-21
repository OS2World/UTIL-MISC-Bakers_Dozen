/* title:       'SEMTEST.C === Try out semaphores & shared memory ===' */
/* subtitle:    'Elipse Ltd.  [jms]              Revised: 1992 Nov 25' */


#define INCL_DOSSEMAPHORES              /* Semaphore values */
#define INCL_DOSDATETIME                /* Date and time values */
#define INCL_DOSPROCESS                 /* Process and thread values */
#define INCL_DOSERRORS                  /* Error defines */

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define COM_MEM "\\sharemem\\common"    /* identifier for shared memory */
#define ACC_SEM "\\sem32\\access"       /* memory access semaphore */
#define CLOCK_TICS  1000                /* tics/second */

struct shared_data                      /* Available to all processes */
    {
    int some_stuff[10];
    float more_stuff[5];
    } ;


HMTX mutex_semhandle;                   /* mutex semaphore handle */
struct shared_data *common = NULL;      /* will point to shared structure */
volatile int endprog = 0;               /* user signal to end program */

int main(int argc, char *argv[]);
int initialize(void);
void semtest(void);
void signal_handler(int sval);
int cleanup(void);
int request_mem_access(void);
int release_mem_access(void);
int suspend_for(unsigned seconds);


/* MAIN ===== Starting point ===== $pageif:6 */

int main(int argc, char *argv[])
    {                                   /* main */
    int ret;
    fprintf( stderr, "\nProgram SEMTEST now running\n\n" );
    fprintf( stderr, "\nPress Control-Break to stop\n\n\n" );
    ret = initialize( );
    if ( ret == 0 )
        {
        semtest( );
        ret = cleanup( );
        }
    fprintf( stderr, "\nSEMTEST Ending\n" );
    return ret;
    }                                   /* main */


/* INITIALIZE ===== Start up fresh ===== $pageif:6 */

int initialize(void)
    {                                   /* initialize */
    ULONG mflag = PAG_COMMIT | PAG_READ | PAG_WRITE;
    APIRET rc = DosCreateMutexSem( ACC_SEM, &mutex_semhandle, 0L, 1L );
    if ( rc != 0L )
        {
        fprintf( stderr, "\nCreateMutexSem failed. Code=%lu\n", rc );
        return (int) rc;
        }
    rc = DosAllocSharedMem( (void *) &common, COM_MEM, sizeof(struct shared_data), mflag );
    if ( rc != 0L )
        {
        fprintf( stderr, "\nAllocSharedMem failed. Code=%lu\n", rc );
        return (int) rc;
        }
    memset( common, '\0', sizeof(struct shared_data) );
    endprog = 0;
    if ( signal( SIGINT, signal_handler ) == SIG_ERR )
        {
        fprintf( stderr, "\nSignal set failed.\n" );
        return -1;
        }
    return 0;
    }                                   /* initialize */


/* SEMTEST ===== Body of program ===== $pageif:6 */

void semtest(void)
    {                                   /* semtest */
    fprintf( stderr, "If you run PSTAT /M you see the name COMMON in use by this program.\n" );
    fprintf( stderr, "If you run PSTAT /S you DO NOT see the name \\SEM32\\ACCESS.\n" );
    fprintf( stderr, "Similarly, PSTAT /P:xxxx for this process,\n" );
    fprintf( stderr, "           shows COMMON under shared memory names,\n" );
    fprintf( stderr, "           but not \\SEM32\\ACCESS under owned semaphore names.\n\n" );
    release_mem_access( );              /* allow other users */
    while ( !endprog )
        {
        suspend_for( 1 );               /* give up some time */
        request_mem_access( );          /* get semaphore */
        common->some_stuff[1] += 1;     /* modify shared ... */
        common->some_stuff[2] += 2;     /* ... memory */
        release_mem_access( );          /* release semaphore */
        }
    }                                   /* semtest */


/* SIGNAL_HANDLER ===== Called by keyboard (Ctrl-Brk) ===== $pagif:6 */

void signal_handler(int sval)
    {                                   /* signal_handler */
    fprintf( stderr, "\nProgram termination requested (Ctrl-Break)\n" );
    endprog = 1;
    }                                   /* signal_handler */


/* CLEANUP ===== Return memory and exit ===== $pageif:6 */

int cleanup(void)
    {                                   /* cleanup */
    APIRET rc;
    if ( common != NULL )
        {
        rc = DosFreeMem( common );
        if ( rc != 0L )
            fprintf( stderr, "\nFreeMem failed. Code=%lu\n", rc );
        }
    rc = DosCloseMutexSem( mutex_semhandle );
    if ( rc != 0L )
        fprintf( stderr, "\nCloseMutexSem failed. Code=%lu\n", rc );
    return (int) rc;
    }                                   /* cleanup */


/* REQUEST_MEM_ACCESS ===== Get exclusive control ===== $pageif:6 */

int request_mem_access(void)
    {                                   /* request_mem_access */
    APIRET rc;
    while ( 1 )
        {
        rc = DosRequestMutexSem( mutex_semhandle, 200 );
        if ( rc == 0L )
            return 0;                   /* success */
        if ( rc == ERROR_TIMEOUT )
            {
            fprintf( stderr, "Waiting for memory access" );
            suspend_for( 2 );           /* Wait 2 sec & try again */
            }
        else
            {
            fprintf( stderr, "RequestMutexSem failed. Code=%lu", rc );
            return (int) rc;            /* failure */
            }
        }
    }                                   /* request_mem_access */


/* RELEASE_MEM_ACCESS ===== Release control ===== $pageif:6 */

int release_mem_access(void)
    {                                   /* release_mem_access */
    APIRET rc = DosReleaseMutexSem( mutex_semhandle );
    if ( rc != 0L )
        fprintf( stderr, "ReleaseMutexSem failed. Code=%lu", rc );
    return (int) rc;
    }                                   /* release_mem_access */


/* SUSPEND_FOR ===== Wait for given period ===== $pageif:6 */

int suspend_for(unsigned seconds)
    {                                   /* suspend_for */
    APIRET rc = DosSleep( (ULONG) seconds * (ULONG) CLOCK_TICS );
    if ( rc != 0L )
        fprintf( stderr, "Sleep failed. Code=%lu", rc );
    return (int) rc;
    }                                   /* suspend_for */
