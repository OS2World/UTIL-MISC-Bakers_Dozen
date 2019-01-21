/* QueryQ.C */

/* 
 This sample code queries the local workstation for a queue name that is 
 entered at the command prompt. The query is done at level 4 which returns 
 returns in the buffer information in a PRQINFO3 structure and follows this 
 with PRJINFO2 structures - one for each job in the queue. 
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
    ULONG  i = 0L;
    USHORT uJobCount = 0;
    PSZ    pszQueueName = NULL;
    PVOID  pBuf = NULL;
    PPRJINFO2 prj2;
    PPRQINFO3 prq3;
 
    if (argc < 2)
        {
        printf( "\nSyntax:  QueryQ QueueName\n" );
        return 1;
        }
 
    pszQueueName = argv[1];             /* Queue name from input */
    splerr = SplQueryQueue( NULL, pszQueueName, 4L, (PVOID) NULL, 0L, &cbNeeded );

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
        splerr = SplQueryQueue( NULL, pszQueueName, 4L, pBuf, cbBuf, &cbNeeded );
        prq3 = (PPRQINFO3) pBuf;
        printf( "Queue info: name- %s\n", prq3->pszName );
        if ( prq3->fsType & PRQ3_TYPE_APPDEFAULT )
            printf( "  This is the application default print queue\n" );
        printf( "  priority - %d  starttime - %d  endtime - %d fsType - %X\n",
                    prq3->uPriority ,  prq3->uStartTime , prq3->uUntilTime , prq3->fsType );
        printf( "  pszSepFile    - %s\n", prq3->pszSepFile );
        printf( "  pszPrProc     - %s\n", prq3->pszPrProc );
        printf( "  pszParms      - %s\n", prq3->pszParms );
        printf( "  pszComment    - %s\n", prq3->pszComment );
        printf( "  pszPrinters   - %s\n", prq3->pszPrinters );
        printf( "  pszDriverName - %s\n", prq3->pszDriverName );
        if (prq3->pDriverData)
            {
            printf( "  pDriverData->cb           - %ld\n", (ULONG) prq3->pDriverData->cb );
            printf( "  pDriverData->lVersion     - %ld\n", (ULONG) prq3->pDriverData->lVersion );
            printf( "  pDriverData->szDeviceName - %s\n", prq3->pDriverData->szDeviceName );
            }
        /* Store the job count for use later in the for loop. */
        uJobCount = prq3->cJobs;
        printf( "Job count in this queue is %d\n\n",uJobCount );
 
        /* Increment the pointer to the PRQINFO3 structure so that it points to the first structure after itself. */
        prq3++;
 
        /* Cast the prq3 pointer to point to a PRJINFO2 structure, and set a pointer to point to that place. */
        prj2 = (PPRJINFO2) prq3;
        for ( i = 0;  i < uJobCount;  i++ )
            {
            printf( "Job ID    = %d\n",  prj2->uJobId );
            printf( "Priority  = %d\n",  prj2->uPriority );
            printf( "User Name = %s\n",  prj2->pszUserName );
            printf( "Position  = %d\n",  prj2->uPosition );
            printf( "Status    = %d\n",  prj2->fsStatus );
            printf( "Submitted = %ld\n", prj2->ulSubmitted );
            printf( "Size      = %ld\n", prj2->ulSize );
            printf( "Comment   = %s\n",  prj2->pszComment );
            printf( "Document  = %s\n\n",prj2->pszDocument );
            prj2++;                     /* Increment the pointer to point to the next structure. */
            }
        free( pBuf );
        }
    return splerr;
    }
