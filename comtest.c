/* title:       'COMTEST.C ===== Serial port duplex communication =====' */
/* subtitle:    'Elipse Ltd.  [jms]                Revised: 1997 Sep 23' */


#ifndef __OS2__
#include <os2compat.h>
#else
#define INCL_DOS
#include <os2.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>

#define COMPORT     "COM1"              /* Default */

int  main( int argc, char *argv[] );
int  comtest( void );
void fromterm( void *dummy );
void fromkeyb( void *dummy );
int startcomm( const char *ioport );
void stopcomm( void );

int sh = -1;                            /* handle for COM port */


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int ret = 0;
    char ioport[40];
    fprintf( stderr, "\n\nBegin terminal I/O test\n\n" );

    if ( argc < 2 )
        strcpy( ioport, COMPORT );
    else
        strcpy( ioport, argv[1] );
    fprintf( stderr, "\nUsing com port: %s\n", ioport );
    fprintf( stderr, "\nUse MODE to set line speed and parity!\n" );
    fprintf( stderr, "\nTerminate with *\n\n" );

    if (!startcomm( ioport ))
        return 1;

    ret = comtest( );
    if ( ret > 0 )
        fprintf( stderr, "\n\nTerminal I/O ended with error\n\n" );
    else
        fprintf( stderr, "\n\nTerminal I/O ended normally\n\n" );
    return ret;
    }                                   /* main */


/* COMTEST ===== Body of program ===== $pageif:6 */

int comtest( void )
    {                                   /* comtest */
    TID tid;
    if (_beginthread( fromterm, NULL, 12288, NULL ) < 0L)
        {
        fprintf( stderr, "\nUnable to start fromterm.\n" );
        return 1;
        }
    if (_beginthread( fromkeyb, NULL, 12288, NULL ) < 0L)
        {
        fprintf( stderr, "\nUnable to start fromkeyb.\n" );
        return 1;
        }

    tid = 0L;
    DosWaitThread( &tid, DCWW_WAIT );   /* wait for one thread */
    tid = 0L;
    DosWaitThread( &tid, DCWW_WAIT );   /* wait for other thread */
    return 0;
    }                                   /* comtest */


/* FROMTERM ===== Read terminal input (and display) ===== $pageif:6 */

void fromterm( void *dummy )
    {                                   /* fromterm */
    while (sh > 0)
        {
        char ch = '\0';
        read( sh, &ch, 1 );             /* read from terminal */
        putchar( ch );                  /* write on console */
        fflush( stdout );
        if (ch == '*')
            stopcomm( );
        }
    }                                   /* fromterm */


/* FROMKEYB ===== Read keyboard input (and transmit) ===== $pageif:6 */

void fromkeyb( void *dummy )
    {                                   /* fromkeyb */
    while (sh > 0)
        {
        char ch = (char) getch( );      /* read from console */
        write( sh, &ch, 1 );            /* write to terminal */
        if (ch == '*')
            stopcomm( );
        }
    }                                   /* fromkeyb */


/* STARTCOMM ===== Open device ===== $pageif:6 */

int startcomm( const char *ioport )
    {                                   /* startcomm */
    sh = open( ioport, O_RDWR );
    if (sh <= 0)
        {
        fprintf( stderr, "\nUnable to open COM port %s.\n", ioport );
        return 0;                       /* failure */
        }
    return 1;                           /* success */
    }                                   /* startcomm */


/* STOPCOMM ===== Close device ===== $pageif:6 */

void stopcomm( void )
    {                                   /* stopcomm */
    DosSleep( 1 );
    close( sh );
    sh = -1;
    DosSleep( 1 );
    }                                   /* stopcomm */

