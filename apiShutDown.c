/* apiShutDown.C */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include <sys\types.h>

#define PORT    13579

static int skt = 0;

static int shut( char *host );
static void sendrecv( void );


/*
 * Client Main.
 */

int main( int argc, char *argv[] )
    {
    printf( "\napiShutDown starting\n" );
    if (argc < 2)
        {
        fprintf( stderr, "\nUsage: apiShutDown <host address>\n" );
        return 1;
        }
    return shut( argv[1] );
    }


static int shut( char *host )
    {
    struct sockaddr_in server;          /* server address */

    if ((skt = socket( PF_INET, SOCK_STREAM, 0 )) < 0)
        {
        fprintf( stderr, "\nSocket( ) failed\n" );
        return 2;
        }

    server.sin_family      = AF_INET;               /* Internet Domain */
    server.sin_port        = htons( PORT );         /* Server Port */
    server.sin_addr.s_addr = inet_addr( host );     /* Server's Address */
    if (connect( skt, (struct sockaddr *) &server, sizeof( server ) ) < 0)
        {
        fprintf( stderr, "\nConnect( ) failed\n" );
        return 3;
        }
    printf( "\nConnected on socket %d\n", skt );

    sendrecv( );

    printf( "\nClient Ended Successfully\n" );
    return 0;
    }

static void sendrecv( void )
    {
    char* outbuf = "{\"Function\":\"ShutDown\"}";
    char inbuf[1024];
    memset( inbuf, '\0', sizeof( inbuf ) );
    printf( outbuf );
    fflush( stdout );
    printf( "\n" );
    if (send( skt, outbuf, strlen( outbuf ), 0 ) < 0)
        fprintf( stderr, "\nSend( ) failed\n" );
    else
        {
        recv( skt, inbuf, sizeof( inbuf ), 0 );
        printf( "%s", inbuf );
        }
    }
