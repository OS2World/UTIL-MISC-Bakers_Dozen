/* DelQJob.C */

/* 
 This sample code will delete the job id that is entered at the prompt. 
*/

#define INCL_SPL
#define INCL_SPLERRORS
#define INCL_DOSERRORS

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
 
int main( int argc, char *argv[] )
    {
    SPLERR splerr = 0L;
    ULONG  ulJob = 0L;
    PSZ    pszQueueName = NULL;
 
    if (argc < 2)
        {
        printf( "\nSyntax:  DelQJob Job_ID\n" );
        return 1;
        }

    ulJob = atoi( argv[1] );            /* job id from input argument. */
    splerr = SplDeleteJob( NULL, pszQueueName, ulJob );

    if (splerr != NO_ERROR)
        {
        switch (splerr)
            {
            case NERR_JobNotFound:
                printf( "Job does not exist.\n" );
                break;
            case NERR_JobInvalidState:
                printf( "This operation can't be performed on the print job.\n" );
                break;
            default:
                printf( "Errorcode = %ld\n", splerr );
            }
        }
    else
        printf( "Job %d was deleted.\n", ulJob );
    return splerr;
    }
 