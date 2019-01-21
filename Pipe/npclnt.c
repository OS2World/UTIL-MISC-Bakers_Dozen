/* NPCLNT.C */


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

HPIPE hp = 0;

int  main( int argc, char *argv[] );
void read( void *dummy );
void write( void *dummy );


/* MAIN ===== Starting Point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {
    CHAR ServerPipe[CCHMAXPATH];
    ULONG ulAction;
    APIRET rc;
    fprintf( stderr, "\n\nNPCLNT\n\n" );

    memset( ServerPipe, '\0', sizeof( ServerPipe ) );
    if (argc > 1)
        {
        strcpy( ServerPipe, "\\\\" );
        strcat( ServerPipe, argv[1] );
        }
    strcat( ServerPipe, SERVER_PIPE_NAME );
    fprintf( stderr, "%s\n", ServerPipe );

    rc = DosOpen( ServerPipe, &hp, &ulAction, 0,
                FILE_NORMAL, FILE_OPEN,
                OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE,
                NULL );
    if (rc)
        {
        fprintf( stderr, "\nNamed pipe failed to open (%d)\n", rc );
        return 2;
        }

    _beginthread( &write , NULL, 64000, NULL );
    read( NULL );

    DosClose( hp );
    fprintf( stderr, "\nEnd NPCLNT\n" );
    return 0;
    }


/* READ ===== From Named Pipe ===== $pageif:6 */

void read( void *dummy )
    {
    ULONG cbRead, cbWritten;
    CHAR achBuf[PIPESIZE];

    do
        {
        DosRead( hp, achBuf, sizeof( achBuf ), &cbRead );
        DosWrite( HF_STDOUT, achBuf, cbRead, &cbWritten );
        } while (cbRead);
    }


/* WRITE ===== To Named Pipe ===== $pageif:6 */

void write( void *dummy )
    {
    ULONG cbRead, cbWritten;
    CHAR achBuf[PIPESIZE];

    do 
        {
        DosRead( HF_STDIN, achBuf, sizeof( achBuf ), &cbRead );
        DosWrite( hp, achBuf, cbRead, &cbWritten );
        } while (cbRead);
    }
