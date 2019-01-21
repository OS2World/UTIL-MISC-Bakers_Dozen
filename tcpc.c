/* TCPC.C */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include <sys\types.h>


static int skt = 0;

static int tcpc( unsigned short port, char *host );
static void io_loop( void );


/*
 * Client Main.
 */

int main( int argc, char *argv[] )
    {
    printf( "\nTCPC starting\n" );
    if (argc != 3)
        {
        fprintf( stderr, "\nUsage: tcpc <port> <host address>\n" );
        return 1;
        }
    return tcpc( (unsigned short) atoi( argv[1] ), argv[2] );
    }


static int tcpc( unsigned short port, char *host )
    {
    struct sockaddr_in server;          /* server address */
    unsigned long tid;                  /* thread id for receiver */

    if ((skt = socket( PF_INET, SOCK_STREAM, 0 )) < 0)
        {
        fprintf( stderr, "\nSocket( ) failed\n" );
        return 2;
        }

    server.sin_family      = AF_INET;               /* Internet Domain */
    server.sin_port        = htons( port );         /* Server Port */
/// server.sin_addr.s_addr = inet_addr( host );     /* Server's Address */
    if (connect( skt, (struct sockaddr *) &server, sizeof( server ) ) < 0)
        {
        fprintf( stderr, "\nConnect( ) failed\n" );
        return 3;
        }
    printf( "\nConnected on socket %d\n", skt );

    io_loop( );

    printf( "\nClient Ended Successfully\n" );
    return 0;
    }


static void io_loop( void )
    {
    char outbuf[512], inbuf[4096];
    while (1)
        {
        memset( outbuf, '\0', sizeof( outbuf ) );
        printf( "\nType message (or '*' to end):" );
        fflush( stdout );
        if ((gets( outbuf ) == NULL) || (*outbuf == '*'))
            break;
        printf( "\n" );
        if (send( skt, outbuf, strlen( outbuf ), 0 ) < 0)
            {
            fprintf( stderr, "\nSend( ) failed\n" );
            break;
            }
        memset( inbuf, '\0', sizeof( inbuf ) );
        if (recv( skt, inbuf, sizeof( inbuf ), 0 ) < 0)
            break;
        printf( "%s", inbuf );
        fflush( stdout );
        }
    }
