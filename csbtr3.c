/* FROM BTRIEVE DEVELOPERS' JOURNAL, Vol IV No. 4 */

/* Listing 3 */


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
#define LPVOID          void *
#define LPSOCKADDR      void *
#define INVALID_SOCKET  -1
#define SOCKET_ERROR    -1



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
    strcpy( serverName, "Pentium" );
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
            fSuccess = csRecv( 0, (LPVOID) &p, bytesRead );
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
