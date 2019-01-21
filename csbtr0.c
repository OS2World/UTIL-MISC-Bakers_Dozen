/* FROM BTRIEVE DEVELOPERS' JOURNAL, Vol IV No. 4 */


/* Listing 1   Structure definitions to support the Btrieve client/server communication. */


#ifndef CHAT_H
#define CHAT_H

#include  btrapi.h 
#define CS_REQ_CHAT 1000
 
struct BTRIEVE_PARM_INFO
    {
    BTI_WORD opCode;
    BTI_BYTE posBlk[128];
    BTI_WORD dataLen;
    BTI_BYTE keyBuf[255];
    BTI_SINT keyNum;
    BTI_BYTE clientID[16];
    BTI_CHAR dataBuf[32000];
    };

struct BTI_POS_BLK
    {
    BTI_BYTE *pb;
    };

#endif






/* Listing 2   doChat() function that passes requests to the Server's MKDE. */


void DoChat( DATA_PACKET *p,WORD commHandle )
    {
    CHAR buffer[1152];
    CHAR who[100];
    CHAR chRequest[MAX_DATA_SIZE];
    BTI_WORD status;
    DWORD bytesRead,bytesReplied;
    BOOL fSuccess;
    BTRIEVE_PARM_INFO *btrvParms;
    int thisClient = 0;
    int thisHandle = 0;
    /*critical section! */
    EnterCriticalSection( &CriticalSection );
    thisClient = curClient;
    curClient++;
    LeaveCriticalSection( &CriticalSection );
    /* Leave critical section. */

    while ( 1 )
        {
        memset( chRequest, \0, MAX_DATA_SIZE );
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
                for ( thisHandle=0; thisHandle<MAX_CLIENT_FILES && posBlocks[thisClient][thisHandle].pb!=NULL; )
                    {
                    thisHandle++;
                    }
                posBlocks[thisClient][thisHandle].pb = malloc( 128 );
                status = BTRVID( btrvParms->opCode, posBlocks[thisClient][thisHandle].pb, btrvParms->dataBuf, &btrvParms->dataLen, btrvParms->keyBuf, btrvParms->keyNum, btrvParms->clientID );
                if ( status != 0 ) /*Error opening? */
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
                for ( thisHandle = 0, status = 0; thisHandle < MAX_CLIENT_FILES && status == 0; thisHandle++ )
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
        memcpy( chRequest,(char *)p, MAX_DATA_SIZE );
        /* although the structure has a 32000 byte buffer, */
        /* send only what is needed. */
        bytesReplied = (sizeof(struct BTRIEVE_PARM_INFO) - (32000 - btrvParms - dataLen)) + 12;
        fSuccess = csSend( commHandle, chRequest, bytesReplied );
        }
    }
 }





/* Listing 3 */


BTI_API BTRVID( BTI_WORD operation, BTI_VOID_PTR posBlock, BTI_VOID_PTR dataBuffer, BTI_WORD_PTR dataLength, BTI_VOID_PTR keyBuffer, BTI_SINT keyNumber, BTI_BUFFER_PTR clientID )
    {
    static SOCKET cliSocket = INVALID_SOCKET;
    struct sockaddr_in cliSocketAddr;
    struct sockaddr_in srvSocketAddr;
    struct hostent *host;
    LPVOID lpvBuffer = NULL;
    CHAR chBuffer[MAX_DATA_SIZE];
    CHAR gotSock = 0;
    BOOL fSuccess = FALSE;
    DWORD bytesRead;
    DWORD bytesWritten;
    DWORD dwMode;
    BTI_WORD status;
    int error;
    CHAR serverName[255];
    CHAR who[100];
    DATA_PACKET p;
    BTRIEVE_PARM_INFO *btrvParms;
    status = 0; /* default to success... */
    /* this is the server I am attempting to get to... */
    strcpy( serverName, Pentium );
    /* do this the first time through or if RESET was last called... */
    if ( cliSocket == INVALID_SOCKET )
        {
        cliSocket = socket( PF_INET, SOCK_STREAM, 0 );
        gotSock = 1;
        }
    if ( cliSocket == INVALID_SOCKET )
        {
        /* Report that Windows Sockets did not respond to the WSAStartup() call */
        printf( "\nCLIENT error - Invalid socket returned from socket( )\n" );
        WSACleanup( );
        exit( 0 );
        }
    if ( gotSock )
        {
        cliSocketAddr.sin_family = AF_INET;
        cliSocketAddr.sin_addr.s_addr = INADDR_ANY;
        cliSocketAddr.sin_port = 0;
        if ( (bind( cliSocket, (LPSOCKADDR) &cliSocketAddr, sizeof(cliSocketAddr) )) == SOCKET_ERROR )
            {
            printf( "\nCLIENT error %d - socket can t be bound\n", WSAGetLastError( ) );
            WSACleanup( );
            exit( 0 );
            }
            host = gethostbyname( serverName );
            if ( host == NULL )
                {
                status = 20; /* record manager not found... */
                }
            else
                {
                srvSocketAddr.sin_family = AF_INET;
                srvSocketAddr.sin_addr.s_addr = INADDR_ANY;
                memcpy( &srvSocketAddr.sin_addr.s_addr, *host->h_addr_list, 4 );
                srvSocketAddr.sin_port = 5001;
                if ( connect( cliSocket, (LPSOCKADDR) &srvSocketAddr, sizeof(srvSocketAddr) ) == SOCKET_ERROR )
                    {
                    printf( "\nCLIENT error %d - socket can t connect\n", WAGetLastError( ) );
                    WSACleanup( );
                    exit( 0 );
                    }
                /* initial send to establish communications... */
                p.serviceCode = CS_REQ_CHAT;
                p.returnCode = 0;
                bytesWritten = strlen( p.data ) + 12;
                fSuccess = csSend( 0, (LPVOID) &p, bytesWritten );
                if ( !fSuccess )
                    {
                    showError( WriteFile );
                    }
                else
                    {
                    printf( "\nCONNECTED to server, awaiting a response!\n" );
                    }
                }
            }
        /* Use handle 0, just for a consistant approach. Change the connect code above, and the */
        /* csSend and csRecv code, and this can be used for a different protocol. */
        commHandles[0].flag = 1;
        commHandles[0].cliSocket = cliSocket;
        btrvParms = (struct BTRIEVE_PARM_INFO *) &p.data[0];
        btrvParms->opCode = operation;
        btrvParms->keyNum = keyNumber;
        btrvParms->dataLen = *dataLength;
        memcpy( btrvParms->posBlk, posBlock, 128 );
        memcpy( btrvParms->dataBuf, dataBuffer, *dataLength );
        memcpy( btrvParms->keyBuf, keyBuffer, 255 );
        if ( status == 0 )
            {
            fSuccess = csSend( 0, (LPVOID) &p, bytesWritten );
            /* default to failure... */
            p.returnCode = 9999;
            fSuccess=csRecv( 0, (LPVOID) &p, bytesRead );
            if ( fSuccess )
                {
                btrvParms = (struct BTRIEVE_PARM_INFO *) &p.data[0];
                dataLength = btrvParms->dataLen;
                memcpy( posBlock, btrvParms->posBlk, 128 );
                memcpy( dataBuffer, btrvParms->dataBuf, btrvParms->dataLen );
                memcpy( keyBuffer, btrvParms->keyBuf, 255 );
                }
            if ( operation == B_STOP || operation == B_RESET )
                {
                WSACleanup( );
                cliSocket = INVALID_SOCKET;
                }
            }
        else
            {
            p.returnCode = status;
            }
        return p.returnCode;
    }
