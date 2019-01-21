/* NPSERV.C */


#define INCL_DOSQUEUES
#define INCL_DOS
#include <os2.h>

#include <process.h>
#include <string.h>
#include <stdio.h>

#define PIPESIZE 256
#define SERVER_PIPE_NAME "\\PIPE\\CMDPIPE"
#define HF_STDIN 0
#define HF_STDOUT 1
#define HF_STDERR 2

HPIPE hpOutR, hpOutW;
HPIPE hpInR, hpInW;
HPIPE hpErrR, hpErrW;
HPIPE NmPipeHandle;


int  main( int argc, char *argv[] );
void read( void *dummy );
void write( void *dummy );


/* MAIN ===== Starting point ===== pageif:6 */

int main( int argc, char *argv[] )
    {
    CHAR ServerPipe[CCHMAXPATH];
    APIRET rc;
    RESULTCODES resc;
    CHAR szFailName[CCHMAXPATH];
    HFILE hfSaveOut = -1, hfNewOut = HF_STDOUT;
    HFILE hfSaveIn  = 10, hfNewIn  = HF_STDIN;
    fprintf( stderr, "\n\nNPSERV\n\n" );

    memset( ServerPipe, '\0', sizeof( ServerPipe ) );
    if (argc > 1)
        {
        strcpy( ServerPipe, "\\\\" );
        strcat( ServerPipe, argv[1] );
        }
    strcat( ServerPipe, SERVER_PIPE_NAME );
    fprintf( stderr, "%s\n", ServerPipe );

    DosDupHandle( HF_STDOUT , &hfSaveOut );
    DosDupHandle( HF_STDIN  , &hfSaveIn  );

    DosCreatePipe( &hpOutR, &hpOutW, PIPESIZE);
    DosCreatePipe( &hpInR , &hpInW , PIPESIZE);

    DosDupHandle( hpOutW, &hfNewOut );
    DosDupHandle( hpInR,  &hfNewIn  );

    DosExecPgm( szFailName, sizeof( szFailName ),
                EXEC_ASYNC, (PSZ) NULL, (PSZ) NULL, &resc,
                "C:\\OS2\\CMD.EXE");

    DosClose( hpOutW );
    DosClose( hpInR );

    DosDupHandle( hfSaveOut, &hfNewOut );
    DosDupHandle( hfSaveIn,  &hfNewIn  );

    rc = DosCreateNPipe( ServerPipe, &NmPipeHandle, NP_ACCESS_DUPLEX, 
                NP_WMESG | NP_RMESG | 0x01, 256 , 256 , -1 );
    if (rc)
        {
        fprintf( stderr, "\nNamed pipe failed to create (%d)\n", rc );
        return 1;
        }

    rc = DosConnectNPipe( NmPipeHandle );
    if (rc)
        {
        fprintf( stderr, "\nNamed pipe failed to connect (%d)\n", rc );
        return 2;
        }

    _beginthread( &write , NULL, 64000, NULL );
    read( NULL );

    fprintf( stderr, "\nEnd NPSERV\n" );
    return 0;
    }


/* READ ===== From named pipe ===== pageif:6 */

void read( void *dummy )
    {
    ULONG cbRead, cbWritten;
    CHAR achBuf[PIPESIZE];

    do
        {
        DosRead( hpOutR, achBuf, sizeof( achBuf ), &cbRead );
        DosWrite( NmPipeHandle, achBuf, cbRead, &cbWritten );
        } while (cbRead);
    }


/* WRITE ===== To named pipe ===== pageif:6 */

void write( void *dummy )
    {
    ULONG cbRead, cbWritten;
    CHAR achBuf[PIPESIZE];

    do 
        {
        DosRead( NmPipeHandle, achBuf, sizeof( achBuf ), &cbRead );
        DosWrite( hpInW, achBuf, cbRead, &cbWritten );
        } while (cbRead);
    }

