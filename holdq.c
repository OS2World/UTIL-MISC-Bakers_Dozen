/* HoldQ.C */

/*
 This sample code will hold the local queue name that is entered at the  prompt. 
*/

#define INCL_SPL
#define INCL_SPLERRORS
#define INCL_DOSERRORS

#include <os2.h>
#include <stdio.h>
 
int main( int argc, char *argv[] )
    {
    SPLERR splerr = 0L;
    PSZ    pszQueueName = NULL;

    if (argc < 2)
        {
        printf( "Syntax:  HoldQ QueueName\n" );
        return 1;
        }
 
    pszQueueName = argv[1];             /* Queue name from input */ 
    splerr = SplHoldQueue( NULL, pszQueueName );
    if (splerr != NO_ERROR)
        {
        switch (splerr)
            {
            case NERR_QNotFound:
                printf( "Queue does not exist.\n" );
                break;
            case NERR_SpoolerNotLoaded:
                printf( "The Spooler is not running.\n" );
                break;
            default:
                printf( "Errorcode = %ld\n", splerr );
            }
        }
    else
        printf( "Queue %s was held.\n", pszQueueName );
    return splerr;
    }
