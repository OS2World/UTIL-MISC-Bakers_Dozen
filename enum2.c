/* Enum2.C */

/* 
 This sample code accepts a queue name from the  command line, and then 
 prints out the first found job id.
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
    ULONG  cTotal = 0L;
    ULONG  cReturned = 0L;
    ULONG  cbNeeded = 0L;
    PSZ    pszQueueName = NULL;
    USHORT JobID = 0;
 
    if (argc < 2)
        {
        printf( "\nSyntax:  Enum2  QueueName\n" );
        return 1;
        }

    pszQueueName = argv[1];             /* Queue name from input */
    splerr = SplEnumJob( NULL, pszQueueName, 0L, &JobID, sizeof( USHORT ), &cReturned, &cTotal, &cbNeeded, NULL );

    if ((splerr == NO_ERROR) && (cTotal == 0L))
        {
        printf( "\nNo jobs pending!\n" );
        return 0;
        }
    if (splerr == NERR_QNotFound)
        {
        printf( "\nInvalid Print Queue! (%s)\n", pszQueueName );
        return 2;
        }

    if ((splerr == NO_ERROR) || (splerr == NERR_BufTooSmall) || (splerr == ERROR_MORE_DATA))
        printf( "First Job on queue %s is number %d\n", pszQueueName, JobID );
    else
        printf( "SplEnumJob Error=%ld, Total Jobs=%ld, Returned Jobs=%ld, Bytes Needed=%ld\n", splerr, cTotal, cReturned, cbNeeded );
    return splerr;
    }
