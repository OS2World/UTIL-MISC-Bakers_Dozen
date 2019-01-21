/*  gpib_c.c
 *  Rev: 2009 Mar 13
 *
 *  These functions provide low level communication
 *  with GPIB devices.
 *
 *  This is not intended to be a DLL.
 *  Compile this file to OBJ and link with the application.
 *
 *  This code uses the National Instruments header file, "ni488.h"
 *  It also calls the National Instruments DLL, "GPIB-32.DLL"
 *  This DLL will be installed (in C:\Windows\System32) by the
 *  National Instruments install CD.
 */

#include <string.h>
#include <windows.h>
#include "ni488.h"
#include "gpib_c.h"

#define BDINDEX               0     // Board Index
#define NO_SECONDARY_ADDR     0     // Secondary address of device
#define TIMEOUT               T3s   // Timeout value = 3 seconds
#define EOTMODE               1     // Enable the END message
#define EOSMODE               0     // Disable the EOS mode

static char* err_mnemonic[32] =
    {
    "EDVR", "ECIC", "ENOL", "EADR", "EARG", "ESAC", "EABO", "ENEB",
    "EDMA", "",     "EOIP", "ECAP", "EFSO", "",     "EBUS", "ESTB",
    "ESRQ", "",     "",      "",    "ETAB", "ELCK", "EARM", "EHDL",
    "",     "",     "EWIP", "ERST", "EPWR", "",     "ELIB", "EDEV"
    };


/****************************************
 ********  CODE TO ACCESS GPIB-32.DLL
 ***********************************************/

static int (__stdcall *Pibclr)( int ud );
static int (__stdcall *Pibdev)( int ud, int pad, int sad, int tmo, int eot, int eos );
static int (__stdcall *Pibonl)( int ud, int v );
static int (__stdcall *Pibrd )( int ud, PVOID buf, LONG cnt );
static int (__stdcall *Pibwrt)( int ud, PVOID buf, LONG cnt );

static HINSTANCE Gpib32Lib = NULL;
static int  *Pibsta;
static int  *Piberr;
static long *Pibcntl;

/*******************************************************************/

/*
 * Check internal variables for errors.
 * This function is not public and only used internally.
 */
static int check_status( int dev )
    {
    if (Gpib32Lib == NULL)
        return ERR & 30;                // Library not loaded
    if (dev == -1)
        return ERR & 31;                // No valid device handle
    return 0;                           // All OK
    }

/*
 * Return the current status word
 */
int gpib_status( )
    {
    if (Gpib32Lib == NULL)
        return ERR & 30;                // Library not loaded
    return (*Pibsta);
    }

/*
 * Return the current error as 4 byte string
 */
char* gpib_error( )
    {
    return err_mnemonic[(*Piberr)];
    }

/*
 *  Assign a unique identifier to the device and store in the variable
 *  dev.  If the ERR bit is set in ibsta, call gpib_cleanup with an
 *  error message. Otherwise, the device handle, dev, is returned and
 *  is used in all subsequent calls to the device.
 */
int gpib_open( int addr )
    {
    if (Gpib32Lib == NULL)              // If DLL not loaded,
        gpib_init( );                   // try to load.
    if (Gpib32Lib == NULL)              // If still not loaded,
        return -1;                      // return failure handle
    return (*Pibdev)( BDINDEX, addr, NO_SECONDARY_ADDR,
                     TIMEOUT, EOTMODE, EOSMODE );
    }


/*
 *  Write string to a device.
 *  param:  dev = device handle (from call to gpib_open)
 *  param:  cmd = data to write
 *  return: status word
 */
int gpib_write( int dev, char* cmd )
    {
    int sts = check_status( dev );
    if (sts != 0)
        return sts;
    (*Pibwrt)( dev, cmd, strlen( cmd ) );
    return (*Pibsta);
    }


/*
 *  Read string from a device.
 *  param:  dev = device handle (from call to gpib_open)
 *  param:  buf = buffer to hold result
 *  param:  siz = maximum number of bytes to accept
 *  return: status word
 */
int gpib_read( int dev, char* buf, int siz )
    {
    char *p = NULL;
    int sts = check_status( dev );
    if (sts != 0)
        return sts;
    (*Pibrd)( dev, buf, siz );
    buf[*Pibcntl] = '\0';
    p = strchr( buf, '\n' );
    if (p != NULL)
        *p = '\0';
    return (*Pibsta);
    }

/*
 *  Take the device offline.
 */
void gpib_close( int dev )
    {
    int sts = check_status( dev );
    if (sts == 0)
        (*Pibonl)( dev, 0 );
    }

/*
 *  Call LoadLibrary to load the 32-bit GPIB DLL.  Save the handle
 *  into the global 'Gpib32Lib'.
 */
int gpib_init( void )
    {
    Gpib32Lib = LoadLibrary( "GPIB-32.DLL" );
    if (Gpib32Lib == NULL)
        return 0;                       // LoadLibrary call failed

   /*
    *    The GPIB library is loaded.
    *    Get a pointer to the functions.
    *    If the GetProcAddress call fails, return with an error.
    */
   Pibsta  = (int *)  GetProcAddress( Gpib32Lib, (LPCSTR)"user_ibsta" );
   Piberr  = (int *)  GetProcAddress( Gpib32Lib, (LPCSTR)"user_iberr" );
   Pibcntl = (long *) GetProcAddress( Gpib32Lib, (LPCSTR)"user_ibcnt" );

   Pibclr  = (int (__stdcall *)(int)) GetProcAddress( Gpib32Lib, (LPCSTR) "ibclr" );
   Pibdev  = (int (__stdcall *)(int, int, int, int, int, int)) GetProcAddress( Gpib32Lib, (LPCSTR) "ibdev" );
   Pibonl  = (int (__stdcall *)(int, int)) GetProcAddress(Gpib32Lib, (LPCSTR) "ibonl" );
   Pibrd   = (int (__stdcall *)(int, PVOID, LONG)) GetProcAddress( Gpib32Lib, (LPCSTR) "ibrd" );
   Pibwrt  = (int (__stdcall *)(int, PVOID, LONG)) GetProcAddress( Gpib32Lib, (LPCSTR) "ibwrt" );

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
        return 0;                       // Failure
        }
    return 1;                           // Success
    }

/*
 * Free the library
 */
void gpib_quit( void )
    {
    FreeLibrary( Gpib32Lib );
    Gpib32Lib = NULL;
    }
