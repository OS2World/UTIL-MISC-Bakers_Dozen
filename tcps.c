/* TCPS.C */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include <sys\types.h>


/*
 * Server Main.
 */

int main( int argc, char *argv[] )
    {
    unsigned short port;                /* port server binds to */
    FILE *logfile = NULL;
    struct sockaddr_in server;          /* server address information */
    int s;                     /* socket for accepting connections */

    printf( "\nTCPS starting\n" );
    if (argc < 2)
        {
        fprintf( stderr, "Usage: %s <port> [optional log file]\n", argv[0] );
        return 1;
        }
    if (argc > 2)
        {
        fprintf( stderr, "Logging to file %s\n", argv[2] );
        logfile = fopen( argv[2], "w" );
        }

    port = (unsigned short) atoi( argv[1] );
    if (( s = socket(PF_INET, SOCK_STREAM, 0) ) < 0)
        return 2;
    server.sin_family = AF_INET;
    server.sin_port = htons( port );
    server.sin_addr.s_addr = INADDR_ANY;
    if (bind( s, (struct sockaddr *) &server, sizeof( server ) ) < 0)
        return 3;
    if (listen( s, 1 ) != 0)
        return 4;
    printf( "\nListening on socket %d\n", s );

    while (1)
        {
        char buf[20];                   /* buffer for sending and receiving data */
        struct sockaddr_in client;      /* client address information */
        int ns;                /* socket connected to client */
        int namelen;           /* length of client name */
        namelen = sizeof( client );
        if ((ns = accept( s, (struct sockaddr *) &client, &namelen) ) == -1)
            return 5;

        printf( "\nAccepted connection on socket %d\n", ns );
        do
            {
            int ds;
            memset( buf, '\0', sizeof( buf ) );
            ds = recv( ns, buf, sizeof( buf ), 0 );
            if (ds < 0)
                return 6;
            buf[ds] = '\0';
            printf( "\n Echoing: (%d) %s\n", ds, buf );
            if (logfile != NULL)
                {
                fprintf( logfile, buf );
                fflush( logfile );
                }
            if (send( ns, buf, ds, 0 ) < 0)
                return 7;
            }
        while (*buf != '*');
        if (logfile != NULL)
            fclose( logfile );
        printf( "\nClosing socket %d\n", ns );
        }

    }
