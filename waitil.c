/* $title:      'WAITIL.C ===== Delay until given time =====' */
/* $subtitle:   'Elipse Ltd.  [jms]     Revised: 2002 May 28' */

#ifdef __OS2__
#define INCL_DOSPROCESS
#include <os2.h>
#else
#define DosSleep(x)
#endif

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>

int main(int argc, char *argv[]);
int waitil(int hour, int minute);


/* MAIN ===== Starting point ===== $pageif:6 */

int main(int argc, char *argv[])
    {                                   /* main */
    int hour, minute = 0;
    printf( "\nWAITIL ===== Wait until given time =====\n" );
    printf( "(C) 1991, Elipse Limited\n\n" );

    if (argc < 2)
        goto call_err;

    hour = atoi( argv[1] );
    if ((hour < 0) || (hour > 23))
        goto call_err;

    if (argc > 2)
        {
        minute = atoi( argv[2] );
        if ((minute < 0) || (minute > 59))
            goto call_err;
        }

    return waitil( hour, minute );

call_err:
    printf( "\nCalled incorrectly: Give hour (0-23) and minute (0-59)\n" );
    return 9;
    }                                   /* main */


/* WAITIL ===== Body of program ===== $pageif:6 */

int waitil(int hour, int minute)
    {                                   /* waitil */
    struct tm *st = NULL;
    time_t cd = time( &cd );
    if ((st = localtime( &cd )) == NULL)
        {
        printf( "\nInternal program error occurred\n" );
        return 5;                       /* internal error */
        }
    if ((st->tm_hour == hour) && (st->tm_min >= minute))
        {
        printf( "\nTime has already passed\n" );
        return 3;                       /* time passed */
        }
    printf( "Waiting for %02d:%02d\n", hour, minute );
    printf( "Press ESC to abort\n" );

    while (1)
        {
        cd = time( &cd );
        st = localtime( &cd );
        if ((st->tm_hour == hour) && (st->tm_min >= minute))
            {
            printf( "\nTime reached\n" );
            return 0;                   /* time reached */
            }
        if (kbhit( ))
            {
            if (getch( ) == 27)
                {
                printf( "\nEscape key pressed at %02d:%02d\n", st->tm_hour, st->tm_min );
                return 1;               /* user abort */
                }
            }
        DosSleep( 15000 );
        }

    }                                   /* waitil */
