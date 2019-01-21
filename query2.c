/* Query2.C */

/* 
 This sample code queries the local workstation for a queue name that is 
 entered at the command prompt.
*/

#define INCL_SPL
#define INCL_SPLDOSPRINT
#define INCL_SPLERRORS
#define INCL_DOSERRORS

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] )
    {
    ULONG  splerr = 0L;
    ULONG  cbBuf = 0L;
    ULONG  cbNeeded = 0L;
    PSZ    pszQueueName = NULL;
    PVOID  pBuf = NULL;
    PPRQINFO3 prq3;
 
    if (argc < 2)
        {
        printf( "\nSyntax:  Query2 QueueName\n" );
        return 1;
        }
 
    pszQueueName = argv[1];             /* Queue name from input */
    splerr = SplQueryQueue( NULL, pszQueueName, 3L, (PVOID) NULL, 0L, &cbNeeded );

    if (splerr == NERR_QNotFound)
        {
        printf( "\nInvalid Print Queue! (%s)\n", pszQueueName );
        return 2;
        }
    if ((splerr != NERR_BufTooSmall) && (splerr != ERROR_MORE_DATA))
        {
        printf( "SplQueryQueue Error=%ld, cbNeeded=%ld\n", splerr, cbNeeded );
        return splerr;
        }

    if ((cbNeeded > 0) && ((pBuf = malloc( cbNeeded )) != NULL))
        {
        cbBuf = cbNeeded;
        splerr = SplQueryQueue( NULL, pszQueueName, 3L, pBuf, cbBuf, &cbNeeded );
        prq3 = (PPRQINFO3) pBuf;
        printf( "Queue name - %s\n", prq3->pszName );
        printf( "Icon Label - %s\n", prq3->pszComment );
        printf( "Driver     - %s\n", prq3->pszDriverName );
        if (prq3->pDriverData)
            printf( "Printer    - %s\n", prq3->pDriverData->szDeviceName );
        free( pBuf );
        }
    return splerr;
    }
