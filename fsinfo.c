/* FSINFO.C
 *
 * Sends some Info about attached Filesystems to stdout.
 *
 * If IBM VisualAge C/C++ Compiler is used, compile with:
 * icc -Sse -W3 -O+ fsinfo.c
 *
 * Guenter Kukkukk
 * Damaschkestr. 24
 * 10711 Berlin (Germany)
 * Tel.   +49 327 024 50
 * Fax:   +49 327 024 52
 * EMail: kukuk@berlin.snafu.de
 */
 
#define  INCL_DOS
#define  INCL_DOSERRORS
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>

int main ( int argc, char *argv[] )
    {
    PSZ         pszLine = "--------------------------------------------------";
    APIRET      rc;
    ULONG       ulDisk, ulTmp;
    FSALLOCATE  xAlloc;
    FSINFO      xInfo;
    PSZ         pszTmp;
    CHAR        chDev[3];
    ULONG       ulSize;
    CHAR        xFSQBuf2[sizeof(FSQBUFFER2) + (3 * CCHMAXPATH)];
    PFSQBUFFER2 pxFSQ;
  
    /* Disable all harderror-popups for this application. */
    /* So we get the errors without user intervention. */
    rc = DosError( FERR_DISABLEHARDERR );
    if (rc != NO_ERROR)
        {
        printf( "DosError (FERR_DISABLEHARDERR) failed! rc = %lu\n", rc );
        exit (1);
        }

    /* Now loop through all possible drives */
    for ( ulDisk = 1;  ulDisk <= 26;  ulDisk++ )
        {
        printf( "%s\nDrive: %c\n", pszLine, (CHAR) (ulDisk + 'A' - 1) );
        rc = DosQueryFSInfo( ulDisk, FSIL_VOLSER, &xInfo, sizeof (xInfo) );
        if (rc != NO_ERROR)
            {
            printf( "DosQueryFSInfo() rc = %5lu  ", rc );
            switch (rc)
                {
            case ERROR_NOT_READY:
                pszTmp = "ERROR_NOT_READY";
                break;
            case ERROR_DISK_CHANGE:
                pszTmp = "ERROR_DISK_CHANGE <ERROR_NOT_READY>";
                break;
            case ERROR_INVALID_DRIVE:
                pszTmp = "ERROR_INVALID_DRIVE";
                break;
            default:
                pszTmp = "<UNKNOWN>";
                break;
                }
            printf ("%s\n", pszTmp);
            }
        else
            printf( "Volume Label: %s\n" , xInfo.vol.szVolLabel );
        if (rc != NO_ERROR)
            continue;
    
        rc = DosQueryFSInfo( ulDisk, FSIL_ALLOC, &xAlloc, sizeof (xAlloc) );
        if (rc != NO_ERROR)
            printf( "Drive: %c: DosQueryFSInfo() rc = %lu\n", (CHAR) (ulDisk + 'A' - 1), rc );
        else
            {
            printf( "\nDrive-ID:\t\t%12lu\n", xAlloc.idFileSystem );
            printf( "SecPerCluster:\t\t%12lu\n", xAlloc.cSectorUnit );
            printf( "NumClusters:\t\t%12lu\n", xAlloc.cUnit );
            printf( "AvailClusters:\t\t%12lu\n", xAlloc.cUnitAvail );
            printf( "BytesPerSec:\t\t%12lu\n", xAlloc.cbSector );
            ulTmp = xAlloc.cbSector * xAlloc.cSectorUnit * xAlloc.cUnit;
            printf( "Total DiskSpace:\t%12lu  %9.1f MB\n", ulTmp, (double) ulTmp / (1024.0 * 1024.0) );
            ulTmp = xAlloc.cbSector * xAlloc.cSectorUnit * xAlloc.cUnitAvail;
            printf( "Avail.DiskSpace:\t%12lu  %9.1f MB\n", ulTmp, (double) ulTmp / (1024.0 * 1024.0) );
            }

        /* Note: */
        /* DosQueryFSAttach() is somewhat crazy to use..... */
        sprintf( chDev, "%c:", (CHAR) (ulDisk + 'A' - 1) );
        ulSize = sizeof (xFSQBuf2) + 3 * CCHMAXPATH;
        rc = DosQueryFSAttach( chDev, 0, FSAIL_QUERYNAME, (PFSQBUFFER2)&xFSQBuf2, &ulSize );
        if (rc != NO_ERROR)
            {
            printf( "DosQueryFSAttach() returned: %lu\n", rc );
            continue;
            }
        else
            {
            pxFSQ = (PFSQBUFFER2) xFSQBuf2;
            printf( "\niType:     %hu   ", pxFSQ->iType );
            switch (pxFSQ->iType)
                {
            case FSAT_CHARDEV:
                pszTmp = "Resident character device";
                break;
            case FSAT_PSEUDODEV:
                pszTmp = "Pseudo-character device";
                break;
            case FSAT_LOCALDRV:
                pszTmp = "Local drive";
                break;
            case FSAT_REMOTEDRV:
                pszTmp = "Remote drive attached to FSD";
                break;
            default:
                pszTmp = "<Unknown device>";
                break;
                }
            printf( "%s\n", pszTmp );
            printf( "szName:    %s\n", pxFSQ->szName );
            printf( "szFSDName: %s\n", (PBYTE)pxFSQ->szFSDName + pxFSQ->cbName );
            printf( "rgFSAData: %s\n", (PBYTE)pxFSQ->rgFSAData + pxFSQ->cbName + pxFSQ->cbFSDName );
            }
        }
    printf( "%s\n", pszLine );
    return 0;
    }


