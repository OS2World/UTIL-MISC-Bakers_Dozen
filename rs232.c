/* title:       'RS232 ===== Manipulate serial port =====' */
/* subtitle:    'Elipse Ltd.  [jms]  Revised: 1992 Dec 15' */


#define INCL_DOSFILEMGR
#define INCL_DOSDEVICES
#define INCL_DOSPROCESS
#define INCL_DOSDEVIOCTL
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define default_port    1

HFILE DevHandle = 0L;


int main( int argc, char *argv[] );
int open_port( int port );
int query_port(void);
int close_port(void);
APIRET AsyncSetLine( HFILE fh, short data, short parity, short stop );

 
 
/*
 #define INCL_DOSPROCESS
 #include <os2.h>
 
 HFILE   DevHandle;
 ULONG   ulCategory;
 ULONG   ulFunction;
 PVOID   pParmList;
 ULONG   ulParmLengthMax;
 PULONG  pParmLengthInOut;
 PVOID   pDataArea;
 ULONG   ulDataLengthMax;
 PULONG  pDataLengthInOut;
 APIRET  rc;
*/
 
APIRET _System DosDevIOCtl( HFILE DevHandle,
         ULONG ulCategory,
         ULONG ulFunction, PVOID pParmList,
         ULONG ulParmLengthMax,
         PULONG pParmLengthInOut, PVOID pDataArea,
         ULONG ulDataLengthMax, PULONG pDataLengthInOut);


int main( int argc, char *argv[] )
    {                                   /* main */
    int port = 0;
    APIRET rc = 0xffff;
    printf( "\nRS232 ---- Manipulate serial port -----\n" );
    DevHandle = 0xffff;

    if (argc > 1)
        port = atoi( argv[1] );
    if (port < 1)
        port = default_port;
    open_port( port );
    rc = AsyncSetLine( DevHandle, 7, 'E', 1 );
    if (rc != 0L)
        printf( "\nAsyncSetLine returned %ld\n", rc );

    query_port( );
    close_port( );
    return 0;
    }                                   /* main */


int open_port( int port )
    {                                   /* open_port */
    /* DosOpen opens a new file, an existing file, or a replacement for an
    existing file.  An open file can have extended attributes. */
 
    ULONG   ulAction = 0L;
    ULONG   ulFileAttribute;
    ULONG   ulOpenFlag;
    ULONG   ulOpenMode;
    PEAOP2  pEABuf;
    APIRET  rc;
    UCHAR   devname[64];
    sprintf( devname, "COM%d", port );

    rc = DosOpen( devname, &DevHandle, &ulAction, 0L, FILE_NORMAL, FILE_OPEN,
              (unsigned) OPEN_ACCESS_READWRITE | (unsigned) OPEN_SHARE_DENYNONE, (PEAOP2) NULL );
 
    if (rc != 0L)
        printf( "\nDosOpen returned %ld\n", rc );

    printf( "\nOpen returns handle %lu, action %lu\n", DevHandle, ulAction );

    return (int) rc;
    }                                   /* open_port */


int query_port(void)
    {                                   /* query_port */
    /* DosDevIOCtl performs control functions on a device specified
    by an opened device handle. */
 
/*  UCHAR   ucStatus; */
    ULONG   ucStatus = 0L;
    ULONG   ulStatusLen = 4L;
    APIRET  rc;
    UCHAR   dummy[32];
    ULONG   dumlen = 32L;
    memset( dummy, '\0', sizeof( dummy ) );

printf( "\n Query input:\n" );
printf( "\t handle   %lu\n", DevHandle );
printf( "\t category %lx\n", IOCTL_ASYNC );
printf( "\t function %lx\n", ASYNC_GETCOMMSTATUS );

    rc = DosDevIOCtl( DevHandle, IOCTL_ASYNC, ASYNC_GETCOMMSTATUS, dummy,
                sizeof(dummy), &dumlen, &ucStatus, sizeof( ucStatus ),
                &ulStatusLen );

    if (rc != 0L)
        printf( "\nDosDevIOCtl returned %ld\n", rc );

    printf( "\nQuery returns status %lx, length %lu\n", ucStatus, ulStatusLen );

    return (int) rc;
    }                                   /* query_port */

 

int close_port(void)
    {                                   /* close_port */
    /* DosClose closes a handle to a file, pipe, or device. */
  
    APIRET  rc;
 
    rc = DosClose( DevHandle );
 
    if (rc != 0L)
        printf( "\nDosClose returned %ld\n", rc );

    return (int) rc;
    }                                   /* close_port */



/*************** FROM BBS *****************/



APIRET AsyncSetLine( HFILE fh, short data, short parity, short stop )
    {
        ULONG           ParmLengthInOut=0;
        ULONG           DataLengthInOut=0;
        UCHAR           cpar[4];
        APIRET          retval;

        cpar[0] = (UCHAR) data;
        cpar[1] = (UCHAR) parity;
        cpar[2] = (UCHAR) stop;

        retval=DosDevIOCtl(     fh,     /* file handle */
                                0x01,   /*category */
                                0x42,   /* function */
                                (PVOID) cpar,
                                0x3l,   /* 3 bytes */
                                &ParmLengthInOut,
                                (PVOID)NULL, /* packet */
                                0x0l,   /* length */
                                &DataLengthInOut);
        return(retval);
    }


