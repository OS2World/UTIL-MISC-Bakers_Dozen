/* $title:      'BOOTDRV.C ===== Display boot drive letter =====' */
/* $subtitle:   'Elipse Ltd.  [jms]         Revised: 1998 Aug 28' */


#define INCL_DOSMISC
#define INCL_DOSERRORS
#include <os2.h>
#include <stdio.h>

static char bootdrv( void );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {
    char drv = '\0';
    fprintf( stderr, "\n\tBootDrv ======= (C) Elipse Limited, %s\n", __DATE__ );
    drv = bootdrv( );

    if (drv > ' ')
        {
        printf( "\nBoot drive is %c:\n", drv );
        return 0;
        }
    return 1;
    }


/* BOOTDRV ===== Body of program ===== $pageif:6 */

static char bootdrv( void )
    {                                   /* bootdrv */
    const static char all[] = "?ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    ULONG info = 0;
    APIRET rc = DosQuerySysInfo( QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, (PVOID) &info, sizeof( info ) );

    if (rc == NO_ERROR)
        {
        if ((info > 0) && (info < sizeof( all )))
            return all[info];
        else
            {
            printf( "\nImpossible drive value of %d\n", info );
            return '\0';
            }
        }

    printf( "\nDosQuerySysInfo error: return code = %u\n", rc );
    return '\0';
    }                                   /* bootdrv */

