/* timetest.c ----- 1992 Aug 24 */

/* Program to try out periodic timers in OS/2 */


#ifdef __OS2__
#define INCL_DOSSEMAPHORES              /* Semaphore values */
#define INCL_DOSDATETIME                /* Date and time values */
#include <os2.h>
#else
#include <os2compat.h>
#endif

#include <stdio.h>
#include <stdlib.h>


#define MAXTIME           30000         /* Maximum wait 1/2 min */
#ifndef ERROR_TIMEOUT
#define ERROR_TIMEOUT     640
#define ERROR_INTERRUPT   95
#endif

HEV      event_sem_handle;              /* Event semaphore handle */
HTIMER   timer_handle;                  /* Timer handle */

int main(int argc, char *argv[]);
void timetest(void);
void set_timer(int interval);
void stop_timer(void);
void wait_timer(void);


/* MAIN ===== */

int main(int argc, char *argv[])
    {                                   /* main */
    int interval = 4;
    printf( "\ntimetest ---- set periodic timer -----\n" );
    if ( argc > 1 )
        interval = atoi( argv[1] );
    printf( "\nWill set timer interval for %d seconds\n", interval );
    set_timer( interval );
    timetest( );
    printf( "\nEnd of program\n" );
    return 0;
    }                                   /* main */


/* TIMETEST ===== */

void timetest(void)
    {                                   /* timetest */
    int i;
    for ( i = 0;  i < 4;  ++i )
        {
        wait_timer( );
        printf( "\n I'm alive!  i=%d\n", i );
        }                               /* endfor */
    }                                   /* timetest */


/* SET_TIMER ===== */

void set_timer(int interval)
    {                                   /* set_timer */
    APIRET  rc;                         /* Return code */
    ULONG    TimeInterval;              /* Interval (milliseconds) */
    rc = DosCreateEventSem( NULL, &event_sem_handle, DC_SEM_SHARED, 1UL );
    if ( rc != 0 )
        printf( "\nDosCreateEventSem error: return code = %ld\n", rc );

    TimeInterval = 1000L * (ULONG) interval;
    rc = DosStartTimer( TimeInterval, (HSEM) event_sem_handle, &timer_handle );
    if ( rc != 0 )
        printf( "\nDosStartTimer error: return code = %ld\n", rc );
    }                                   /* set_timer */


/* STOP_TIMER ===== */

void stop_timer(void)
    {                                   /* stop_timer */
    APIRET   rc;                        /* Return code */
    rc = DosStopTimer( timer_handle );
    if ( rc != 0 )
        printf( "\nDosStopTimer error: return code = %ld\n", rc );

    rc = DosCloseEventSem( event_sem_handle );
    if ( rc != 0 )
        printf( "\nDosCloseEventSem error: return code = %ld\n", rc );

    }                                   /* stop_timer */


/* WAIT_TIMER ===== */

void wait_timer(void)
    {                                   /* wait_timer */
    ULONG   ulTimeout;                  /* Number of milliseconds to wait */
    APIRET  rc;                         /* Return code */
    ULONG   ulPostCt;                   /* Post count for event semaphore */

    rc = DosResetEventSem( event_sem_handle, &ulPostCt );
    if ( rc != 0 )
        printf( "\nDosResetEventSem error: return code = %ld\n", rc );

    ulTimeout = MAXTIME;                /* Set time out value */
    rc = DosWaitEventSem( event_sem_handle, ulTimeout );

    if ( rc == ERROR_TIMEOUT )
        {
        printf( "\nDosWaitEventSem call timed out\n" );
        return;
        }
    if ( rc == ERROR_INTERRUPT )
        {
        printf( "\nDosWaitEventSem call was interrupted\n" );
        return;
        }
    if ( rc != 0 )
        printf( "\nDosWaitEventSem error: return code = %ld\n", rc );
    }                                   /* wait_timer */

