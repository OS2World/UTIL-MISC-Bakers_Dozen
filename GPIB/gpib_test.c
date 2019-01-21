/*  gpib_test.c
 *  Rev: 2009 Mar 13
 *
 *  This is a test program to communicate with any GPIB device.
 *
 *  After compiling, link with the object code from gpib_c
 *  which, in turn, talks to the NI DLL (GPIB-32.DLL)
 *
 *  Parameters: DEVICE ADDRESS (1-31)
 *
 *  Program initializes the device and then sends INQ
 *  The response to INQ is displayed.
 *  Then the program enters a loop asking for commands
 *  the commands are sent and responses are displayed.
 *
 *  When all commands are done the device is taken offline.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gpib_c.h"

#define WRITESIZE  250              // Size of write buffer
#define READSIZE  1020              // Size of read buffer
#define TIC_FACT  (1000L/CLOCKS_PER_SEC)

int main( int argc, char *argv[] );
static void gpib_test( void );
static void wait( char *cmd );
static void process( char *cmd );

static int dev = -1;                    // Device handle


/*
 * Entry point
 * param:  GPIB address (1-31)
 */
int main( int argc, char *argv[] )
    {
    int addr = 0;
    printf( "\n\nProgram GPIB_TEST: Try out gpib_io functions.\n" );
    if (argc < 2)
        {
        printf( "Required argument: address of device to check.\n" );
        return 1;
        }
    addr = atoi( argv[1] );
    if ((addr < 1) || (addr > 31))
        {
        printf( "Address is out of range (Allowed: 1-31).\n" );
        return 2;
        }
    if (!gpib_init( ))
        {
        printf( "Unable to correctly access the 32-bit GPIB DLL.\n" );
        return 3;
        }
    dev = gpib_open( addr );
    if (dev < 0)
        {
        printf( "Unable to open device at %s.\n", addr );
        gpib_quit( );
        return 4;
        }
    gpib_test( );
    gpib_close( dev );
    gpib_quit( );
    printf( "\nEnd of program.\n" );
    return 0;
    }


/*
 * Body of program
 */
static void gpib_test( void )
    {
    char cmd[WRITESIZE + 2];
    memset( cmd, '\0', sizeof( cmd ) );
    printf( "\n" );
    strcpy( cmd, "*IDN?" );
    while (*cmd != '\0')
        {
        char *p = strchr( cmd, '!' );   // ! = comment
        if (p != NULL)
            *p = '\0';
        if (*cmd != '\0')
            {
            if (*cmd == '@')            // @ = wait
                wait( cmd );
            else                        // All other
                process( cmd );
            }
        *cmd = '\0';
        printf( "\nCommand: " );
        gets( cmd );
        }
    printf( "\n" );
    }


/*
 * Pause the program for the indicated number of milliseconds
 * param:  cmd = @xxxx where xxx is number of ms
 */
static void wait( char *cmd )
    {
    long stime, etime;
    *cmd = ' ';
    stime = (long) clock( ) * TIC_FACT; // read current time in ms
    etime = stime + atol( cmd );        // calc final time
    while ( ((long) clock( ) * TIC_FACT) < etime )
        ;                               // idle until time
    }

/*
 * Send command to device
 * If the command ends with ?, wait for response
 * On both read and write, check for error
 * param cmd = Any command understood by the device
 */
static void process( char *cmd )
    {
    int stat = 0;
    char buf[READSIZE + 2];
    memset( buf, '\0', sizeof( buf ) );
    stat = gpib_write( dev, cmd );
    printf( "   Sent: {%s}  Stat: 0x%04x\n", cmd, stat );
    if (stat & ERR)
        printf( "* Error: %s\n", gpib_error( ) );
    else
        {
        if (strchr( cmd, '?' ) != NULL)
            {
            stat = gpib_read( dev, buf, READSIZE );
            printf( "   Rcvd: {%s}  Stat: 0x%04x\n", buf, stat );
            if (stat & ERR)
                printf( "* Error: %s\n", gpib_error( ) );
            }
        }
    }
