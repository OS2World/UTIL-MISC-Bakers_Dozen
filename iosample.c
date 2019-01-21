
#: 6516 S1/Base OS API's
    21-Nov-92  03:44:49
Sb: #6514-#DosDevIOCtl() Problem
Fm: Matthew S. Osborn 75230,1346
To: Dave F. Baskin 73340,3533 (X)

>>Please, somebody tell me what I'm doing wrong! Thanks!

#define INCL_DOSFILEMGR
#define INCL_DOSDEVIOCTL
#define INCL_DOSDEVICES
#include <os2.h>
#include <stdio.h>

int main (void);

int main( ) {
    HFILE hf;
    USHORT usBPS = 9600;
    ULONG ulParmLen = 2;
    ULONG ulAction;
    APIRET rc;
    ULONG plen;
    char buf[] = "ATDT\r";

    rc = DosOpen( "COM2", &hf, &ulAction, 0, FILE_NORMAL, FILE_OPEN,
                  OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE,
                  (PEAOP2)NULL );
    printf( "DosOpen() status %lu\n", rc );

    rc = DosDevIOCtl( hf, IOCTL_ASYNC, ASYNC_SETBAUDRATE,
                      (PULONG)&usBPS, sizeof(usBPS), &ulParmLen,
                      NULL, 0, NULL );
    printf( "DosDevIOCtl() Set Baud, status %lu\n", rc );

    rc = DosWrite( hf, buf, 5, &plen );
    printf( "DosWrite() status %lu, bytes %lu\n", rc, plen );

    rc = DosClose( hf );
    printf( "DosClose() status %lu\n", rc ); }

 Just add INCL_DOSDEVICES, include <stdio.h>, and add a prototype for main and all is well.

 Matt


-------------------------------------------------------------------------------------------



#: 8023 S1/Base OS API's
    02-Dec-92  17:35:15
Sb: #DosCevIOCtl for Async
Fm: Kronenwett 100064,1247
To: ALL

I have a problem with the DosDevIOCtl function 0x0041 set bit rate. The return Code is 87 invalid parameter, what is wrong with the code ? It is an example from IBM.
    HFILE hf; /* File handle for the device */
    USHORT usBPS = 9600; /* Bit rate to set the COM port to */
    ULONG ulParmLen = 2; /* Maximum size of the parameter packet */
    ULONG ulAction; /* Action taken by DosOpen */
    APIRET rc; /* Return code */
    rc = DosOpen("COM1", &hf, &ulAction, 0, FILE_NORMAL, FILE_OPEN,
           OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE,  (PEAOP2) NULL);
    printf("rc=%d\n",rc);

    rc = DosDevIOCtl(hf, /* Device handle */
                     IOCTL_ASYNC, /* Serial-device control */
                     ASYNC_SETBAUDRATE, /* Sets bit rate */
                     (PULONG) &usBPS, /* Points at bit rate */
                     sizeof(usBPS), /* Maximum size of parameter list */
                     &ulParmLen, /* Size of parameter packet */
                     NULL, /* No data packet */
                     0, /* Maximum size of data packet */
                     NULL); /* Size of data packet */
    printf("rc=%d\n",rc);

-----------------------------------------------------------------

#: 8061 S1/Base OS API's
    02-Dec-92  23:31:35
Sb: #8023-DosCevIOCtl for Async
Fm: Wayne Kovsky (KSW) 76164,3504
To: Kronenwett 100064,1247 (X)

Here is some working code to set baud rate, extracted into a small function:

static ULONG ComSetBaudRate (HFILE const hfPort,
                             ULONG const ulBaud) {
    ULONG ulRC ;
    ULONG ulParmLen = sizeof (USHORT) ;
    ULONG ulDataLen = 0 ;
    USHORT usBaud = (USHORT) ulBaud ;

    /* Set baud rate. */
    ulRC = DosDevIOCtl (hfPort, IOCTL_ASYNC,
      ASYNC_SETBAUDRATE,
      &usBaud, ulParmLen, &ulParmLen,
      (PVOID) NULL, ulDataLen, &ulDataLen) ;
    if (ulRC) {
        /* Report error. */
    } }

Note that you should read the baud rate immediately after this call, because 
here are cases where it might not be set to the value you expect, even though
no return code will indicate that.

My COM port open is also slightly different from yours (this is part of a
function where the port name is passed as a parameter):

    ulRC = DosOpen (pszPortName,    /* "COM1" or whatever. */
      &hfPort, &ulAction, 0,
      FILE_NORMAL,
      (OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS),
      (OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYREADWRITE |
      OPEN_FLAGS_FAIL_ON_ERROR | OPEN_FLAGS_SEQUENTIAL),
      (PEAOP2) NULL) ;

Hope this helps.

