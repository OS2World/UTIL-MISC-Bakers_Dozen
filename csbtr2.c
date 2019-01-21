/* FROM BTRIEVE DEVELOPERS' JOURNAL, Vol IV No. 4 */

/* Listing 2   doChat() function that passes requests to the Server's MKDE. */


#define INCL_DOSPROCESS
#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "csBTR1.h"

#include <types.h>
#include <netinet\in.h>
#include <sys\socket.h>

/* TEMP SECTION */
#define DATA_PACKET     char
#define MAX_DATA_SIZE   32000
#define WORD            unsigned short
#define DWORD           unsigned long

static int curClient = -1;


void DoChat( DATA_PACKET *p, WORD commHandle )
    {
    CHAR buffer[1152];
    CHAR who[100];
    CHAR chRequest[MAX_DATA_SIZE];
    BTI_WORD status;
    DWORD bytesRead, bytesReplied;
    BOOL fSuccess;
    BTRIEVE_PARM_INFO *btrvParms;
    int thisClient = 0;
    int thisHandle = 0;
    /* critical section! */
    DosEnterCritSec( );
    thisClient = curClient;
    curClient++;
    DosExitCritSec( );
    /* Leave critical section. */

    while ( 1 )
        {
        memset( chRequest, '\0', MAX_DATA_SIZE );
        fSuccess = csRecv( commHandle, chRequest, bytesRead );
        memcpy( (char *)p, &chRequest[0], MAX_DATA_SIZE );
        btrvParms = &p->data[0];
        if ( p->serviceCode == CS_ABANDON_REQUEST )
            {
            csDisconnect( commHandle );
            break;
            }
        if ( fSuccess == FALSE || bytesRead == 0 )
            {
            showError( csRecv );
            break;
            }
        else
            {
            switch ( btrvParms->opCode )
                {
            case B_OPEN:
                for ( thisHandle = 0;  thisHandle < MAX_CLIENT_FILES && posBlocks[thisClient][thisHandle].pb != NULL; )
                    {
                    thisHandle++;
                    }
                posBlocks[thisClient][thisHandle].pb = malloc( 128 );
                status = BTRVID( btrvParms->opCode, posBlocks[thisClient][thisHandle].pb, btrvParms->dataBuf, &btrvParms->dataLen, btrvParms->keyBuf, btrvParms->keyNum, btrvParms->clientID );
                if ( status != 0 )      /* Error opening? */
                    {
                    free( posBlocks[thisClient][thisHandle].pb );
                    posBlocks[thisClient][thisHandle].pb = NULL;
                    }
                else
                    {
                    /* this is to allow us to send back fictitious */
                    /* position blocks and get back to the real block. */
                    /* This is needed so that we can reset all files */
                    /* when STOP or RESET is called. */
                    wsprintf( btrvParms->posBlk, %d ,thisHandle );
                    }
                break;
            case B_CLOSE:
                status = BTRVID( B_CLOSE, posBlocks[thisClient][atoi(btrvParms->posBlock)].pb, btrvParms->dataBuf, &btrvParms->dataLen, btrvParms->keyBuf, btrvParms->keyNum, btrvParms->clientID );
                free( posBlocks[thisClient][atoi(btrvParms->posBlock)].pb );
                posBlocks[thisClient][atoi(btrvParms->posBlock)].pb = NULL;
                break;
            case B_RESET:
            case B_STOP:
                for ( thisHandle = 0, status = 0;  thisHandle < MAX_CLIENT_FILES && status == 0;  thisHandle++ )
                    {
                    if ( posBlocks[thisClient][thisHandle].pb != NULL )
                        {
                        status = BTRVID( B_CLOSE, posBlocks[thisClient][thisHandle].pb, btrvParms->dataBuf, &btrvParms->dataLen, btrvParms->keyBuf, btrvParms->keyNum, btrvParms->clientID );
                        free( posBlocks[thisClient][thisHandle].pb );
                        posBlocks[thisClient][thisHandle].pb = NULL;
                        }
                    }
                csDisconnect( commHandle );
                break;
            default:
                status = BTRVID( btrvParms->opCode, posBlocks[thisClient][atoi(btrvParms->posBlock)].pb, btrvParms->dataBuf, &btrvParms->dataLen, btrvParms->keyBuf, btrvParms->keyNum, btrvParms->clientID );
                break;
            }
        p->returnCode = status;
        memcpy( chRequest, (char *) p, MAX_DATA_SIZE );
        /* although the structure has a 32000 byte buffer, */
        /* send only what is needed. */
        bytesReplied = (sizeof(struct BTRIEVE_PARM_INFO) - (32000 - btrvParms - dataLen)) + 12;
        fSuccess = csSend( commHandle, chRequest, bytesReplied );
        }
    }
 }

