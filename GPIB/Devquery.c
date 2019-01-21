/* Devquery.c
 *
 *  This sample application is comprised of three basic parts:
 *
 *  1. Initialization
 *  2. Main Body
 *  3. Cleanup
 *
 *  The Initialization portion consists of getting a handle to a
 *  device and then clearing the device.
 *
 *  In the Main Body, this application queries a device for its
 *  identification code by issuing the '*IDN?' command. Many
 *  instruments respond to this command with an identification string.
 *  Note, 488.2 compliant devices are required to respond to this
 *  command.
 *
 *  The last step, Cleanup, takes the device offline.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

/*
 *  Include the WINDOWS.H and NI488.H files. The standard Windows
 *  header file, WINDOWS.H, contains definitions used by NI488.H and
 *  NI488.H contains prototypes for the GPIB routines and constants.
 */
#include <windows.h>
#include "ni488.h"

#define ARRAYSIZE 100            // Size of read buffer

int  Dev;                        // Device handle
char ReadBuffer[ARRAYSIZE + 1];  // Read data buffer
char ErrorMnemonic[29][5] = { "EDVR", "ECIC", "ENOL", "EADR", "EARG",
                              "ESAC", "EABO", "ENEB", "EDMA", "",
                              "EOIP", "ECAP", "EFSO", "",     "EBUS",
                              "ESTB", "ESRQ", "",     "",      "",
                              "ETAB", "ELCK", "EARM", "EHDL",  "",
                              "",     "EWIP", "ERST", "EPWR" };


void GPIBCleanup( int Dev, char* ErrorMsg );
static BOOLEAN LoadDll( void );
static void FreeDll( void );

/****************************************
 ********  CODE TO ACCESS GPIB-32.DLL
 ***********************************************/

static int (__stdcall *Pibclr)( int ud );
static int (__stdcall *Pibdev)( int ud, int pad, int sad, int tmo, int eot, int eos );
static int (__stdcall *Pibonl)( int ud, int v );
static int (__stdcall *Pibrd )( int ud, PVOID buf, LONG cnt );
static int (__stdcall *Pibwrt)( int ud, PVOID buf, LONG cnt );

/*
 *    This is private data for the language interface only so it is
 *    defined as 'static'.
 */
static HINSTANCE Gpib32Lib = NULL;
static int  *Pibsta;
static int  *Piberr;
static long *Pibcntl;


int __cdecl main( int argc, char *argv[] )
    {
    int devAddr = 0;
    printf( "\n\nProgram Devquery: Check device at given address.\n\n" );
    if (argc < 2)
        {
        printf( "Required argument: address of device to check.\n" );
        return 1;
        }
    devAddr = atoi( argv[1] );
    if ((devAddr < 1) || (devAddr > 31))
        {
        printf( "Address is out of range (Allowed: 1-31).\n" );
        return 2;
        }
    if (!LoadDll( ))
        {
        printf( "Unable to correctly access the 32-bit GPIB DLL.\n" );
        return 3;
        }

/*
 * ========================================================================
 *
 * INITIALIZATION SECTION
 *
 * ========================================================================
 */

/*
 *  Assign a unique identifier to the device and store in the variable
 *  Dev.  If the ERR bit is set in ibsta, call GPIBCleanup with an
 *  error message. Otherwise, the device handle, Dev, is returned and
 *  is used in all subsequent calls to the device.
 */
#define BDINDEX               0     // Board Index
#define PRIMARY_ADDR_OF_DMM   0     // Primary address of device (not used!)
#define NO_SECONDARY_ADDR     0     // Secondary address of device
#define TIMEOUT               T10s  // Timeout value = 10 seconds
#define EOTMODE               1     // Enable the END message
#define EOSMODE               0     // Disable the EOS mode

    Dev = (*Pibdev) ( BDINDEX, devAddr, NO_SECONDARY_ADDR,
                     TIMEOUT, EOTMODE, EOSMODE );
    if ((*Pibsta) & ERR)
        {
        GPIBCleanup( Dev, "Unable to open device" );
        return 1;
        }

/*
 *  Clear the internal or device functions of the device.  If the error
 *  bit ERR is set in ibsta, call GPIBCleanup with an error message.
 */

    (*Pibclr) (Dev);
    if ((*Pibsta) & ERR)
        {
        GPIBCleanup( Dev, "Unable to clear device" );
        return 1;
        }

/*
 * ========================================================================
 *
 *  MAIN BODY SECTION
 *
 *  In this application, the Main Body communicates with the instrument
 *  by writing a command to it and reading its response. This would be
 *  the right place to put other instrument communication.
 *
 * ========================================================================
 */

/*
 *  Request the identification code by sending the instruction '*IDN?'.
 *  If the error bit ERR is set in ibsta, call GPIBCleanup with an error
 *  message.
 */

    (*Pibwrt) (Dev, "*IDN?", 5L);
    if ((*Pibsta) & ERR)
        {
        GPIBCleanup( Dev, "Unable to write to device" );
        return 1;
        }

/*
 *  Read the identification code by calling ibrd. If the ERR bit is
 *  set in ibsta, call GPIBCleanup with an error message.
 */

    (*Pibrd) (Dev, ReadBuffer, ARRAYSIZE);
    if ((*Pibsta) & ERR)
        {
        GPIBCleanup( Dev, "Unable to read data from device" );
        return 1;
        }

/*
 *  Assume that the returned string contains ASCII data. NULL terminate
 *  the string using the value in ibcntl which is the number of bytes
 *  read in. Use printf to display the string.
 */

    ReadBuffer[*Pibcntl] = '\0';
    printf( "Returned string:  %s\n", ReadBuffer );


/*
 * ========================================================================
 *
 * CLEANUP SECTION
 *
 * ========================================================================
 */

/*  Take the device offline.                                                */

    (*Pibonl)(Dev, 0);
    FreeDll( );
    return 0;
    }

static BOOLEAN LoadDll( void )
    {
    /*
     *  Call LoadLibrary to load the 32-bit GPIB DLL.  Save the handle
     *  into the global 'Gpib32Lib'.
     */
    Gpib32Lib = LoadLibrary( "GPIB-32.DLL" );

    if (!Gpib32Lib)
        {
        /*
        *    The LoadLibrary call failed, return with an error.
        */
        return FALSE;
        }

   /*
    *    OK, the GPIB library is loaded.  Let's get a pointer to the
    *    requested function.  If the GetProcAddress call fails, then
    *    return with an error.
    */
   Pibsta  = (int *) GetProcAddress(Gpib32Lib, (LPCSTR)"user_ibsta");
   Piberr  = (int *) GetProcAddress(Gpib32Lib, (LPCSTR)"user_iberr");
   Pibcntl = (long *)GetProcAddress(Gpib32Lib, (LPCSTR)"user_ibcnt");

   Pibclr  = (int (__stdcall *)(int))GetProcAddress(Gpib32Lib, (LPCSTR)"ibclr");
   Pibdev  = (int (__stdcall *)(int, int, int, int, int, int))GetProcAddress(Gpib32Lib, (LPCSTR)"ibdev");
   Pibonl  = (int (__stdcall *)(int, int))GetProcAddress(Gpib32Lib, (LPCSTR)"ibonl");
   Pibrd   = (int (__stdcall *)(int, PVOID, LONG))GetProcAddress(Gpib32Lib, (LPCSTR)"ibrd");
   Pibwrt  = (int (__stdcall *)(int, PVOID, LONG))GetProcAddress(Gpib32Lib, (LPCSTR)"ibwrt");


   if ((Pibsta  == NULL) ||
       (Piberr  == NULL) ||
       (Pibcntl == NULL) ||
       (Pibclr  == NULL) ||
       (Pibdev  == NULL) ||
       (Pibonl  == NULL) ||
       (Pibrd   == NULL) ||
       (Pibwrt  == NULL))
        {
        FreeLibrary( Gpib32Lib );
        Gpib32Lib = NULL;
        return FALSE;
        }
   else
        {
        return TRUE;
        }
    }  /* end of LoadDll */


static void FreeDll( void )
    {
    FreeLibrary( Gpib32Lib );
    Gpib32Lib = NULL;
    return;
    }

/*
 *  After each GPIB call, the application checks whether the call
 *  succeeded. If an NI-488.2 call fails, the GPIB driver sets the
 *  corresponding bit in the global status variable. If the call
 *  failed, this procedure prints an error message, takes
 *  the device offline and exits.
 */
void GPIBCleanup( int ud, char* ErrorMsg )
    {
    printf( "Error : %s\nibsta = 0x%x iberr = %d (%s)\n",
            ErrorMsg, *Pibsta, *Piberr, ErrorMnemonic[(*Piberr)] );
    if (ud != -1)
        {
        printf( "Cleanup: Taking device offline\n" );
        (*Pibonl)(ud, 0);
        }
    FreeDll( );
    }
