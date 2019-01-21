/* $title:      'QLIST.C ===== Show all printer queues =====' */
/* $subtitle:   'Elipse Ltd.  [jms]     Revised: 1998 Jul 30' */


#define INCL_SPL
#define INCL_SPLDOSPRINT

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int qlist( void );


/* MAIN ===== Starting point ===== $pageif:10 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int numq = 0;
    fprintf( stderr, "\nQLIST ===== List all available print queues\n" );
    numq = qlist( );
    fprintf( stderr, "\n   %d queues found\n", numq );
    return 0;
    }                                   /* main */


/* QLIST ===== Body of program ===== $pageif:10 */

static int qlist( void )
    {                                   /* qlist */
    ULONG qcount;                       /* number print queues returned */
    ULONG total;                        /* number print queues available */
    ULONG bufsize;                      /* buffer size in bytes for all queues */
    int count = 0;

    SplEnumQueue( NULL, 3, NULL, 0, &qcount, &total, &bufsize, NULL );
    if (total > 0L)                     /* check at least one printer */
        {
        int i;
        static char *fmt = " %c %-15s %-20s %-35s\n";
        PPRQINFO3 qinfo = _alloca( bufsize );   /* create new buffer */
        PPRQINFO3 pqi = (PPRQINFO3) NULL;       /* Pointer to print queue used */
        SplEnumQueue( NULL, 3, qinfo, bufsize, &qcount, &total, &bufsize, NULL );
        printf( "\n" );
        printf( fmt, ' ', "Queue", "Label", "Driver" );
        printf( fmt, ' ', "=====", "=====", "======" );
        printf( "\n" );

        count = (int) qcount;
        for ( i = 0, pqi = qinfo;  i < count;  ++i, ++pqi )
            {
            char def = ' ';
            if (pqi->fsType & PRQ3_TYPE_APPDEFAULT)
                def = '*';
            printf( fmt, def, pqi->pszName, pqi->pszComment, pqi->pszDriverName );
            }
        }
    return count;                       /* Return number of print queues read */
    }                                   /* qlist */
