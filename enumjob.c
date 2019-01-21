/* EnumJob.C */

/* 
 This sample code accepts a queue name from the  command line, and then 
 prints out all the information associated with each job in that queue. Level 0 
 and 2 are valid; we have chosen to print out level 2 information. 
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
    ULONG  cTotal = 0L;
    ULONG  cReturned = 0L;
    ULONG  cbNeeded = 0L;
    ULONG  i = 0L;
    PSZ    pszQueueName = NULL;
    PVOID  pBuf = NULL;
    PPRJINFO2 pprj2;
 
    if (argc < 2)
        {
        printf( "\nSyntax:  EnumJob  QueueName\n" );
        return 1;
        }

    pszQueueName = argv[1];             /* Queue name from input */
    splerr = SplEnumJob( NULL, pszQueueName, 2L, pBuf, 0L, &cReturned, &cTotal, &cbNeeded, NULL );

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
    if ((splerr != NERR_BufTooSmall) && (splerr != ERROR_MORE_DATA))
        {
        printf( "SplEnumJob Error=%ld, Total Jobs=%ld, Returned Jobs=%ld, Bytes Needed=%ld\n", splerr, cTotal, cReturned, cbNeeded );
        return splerr;
        }

    if ((cbNeeded > 0) && ((pBuf = malloc( cbNeeded )) != NULL))
        {
        cbBuf = cbNeeded ;
        SplEnumJob( NULL, pszQueueName, 2L, pBuf, cbBuf, &cReturned, &cTotal, &cbNeeded, NULL );
        pprj2 = (PPRJINFO2) pBuf;
        for ( i = 0;  i < cReturned;  ++i )
            {
            printf( "Job ID       = %d\n",   pprj2->uJobId );
            printf( "Job Priority = %d\n",   pprj2->uPriority );
            printf( "User Name    = %s\n",   pprj2->pszUserName );
            printf( "Position     = %d\n",   pprj2->uPosition );
            printf( "Status       = %d\n",   pprj2->fsStatus );
            printf( "Submitted    = %ld\n",  pprj2->ulSubmitted );
            printf( "Size         = %ld\n",  pprj2->ulSize );
            printf( "Comment      = %s\n",   pprj2->pszComment );
            printf( "Document     = %s\n\n", pprj2->pszDocument );
            pprj2++;                    /* Increment the pointer to point to the next structure */
            }
        free( pBuf );
        }
    return splerr;
    }
