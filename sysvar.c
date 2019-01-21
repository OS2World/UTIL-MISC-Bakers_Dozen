/* title:        'SYSVAR.C ===== Show system query information ======' */
/* subtitle:    'Elipse Ltd.  [jms]             Revised: 1999 Aug 05' */


#define INCL_DOSMISC
#define INCL_DOSERRORS
#include <os2.h>
#include <stdio.h>


int main( VOID )
    { 
    ULONG   aulSysInfo[QSV_MAX] = {0};       /* System Information Data Buffer */
    APIRET  rc                  = NO_ERROR;  /* Return code                    */

    fprintf( stderr, "\nSYSVAR ===== Elipse Ltd. %s\n\n", __DATE__ );

    rc = DosQuerySysInfo( 1L,                /* Request all available system   */
                          QSV_MAX,           /* information                    */
                          (PVOID) aulSysInfo,
                          sizeof(ULONG) * QSV_MAX );

    if (rc != NO_ERROR)
        {
        printf( "DosQuerySysInfo error: return code = %u\n", rc );
        }
    else
        {
        printf( "Maximum length for a path name is %u characters.\n",
              aulSysInfo[QSV_MAX_PATH_LENGTH-1] );
 
        printf( "Total number of text sessions is %u.\n",
              aulSysInfo[QSV_MAX_TEXT_SESSIONS-1] );

        printf( "Total number of PM sessions is %u.\n",
              aulSysInfo[QSV_MAX_PM_SESSIONS-1] );

        printf( "Total number of DOS sessions is %u.\n",
              aulSysInfo[QSV_MAX_VDM_SESSIONS-1] );

        printf( "Boot drive was %c:.\n",
              aulSysInfo[QSV_BOOT_DRIVE-1] + '@' );

        if (aulSysInfo[QSV_DYN_PRI_VARIATION-1])
            printf( "Priority is dynamic.\n" );
        else
            printf( "Priority is absolute.\n" );

        printf( "Maximum wait is %u seconds.\n",
              aulSysInfo[QSV_MAX_WAIT-1] );

        printf( "Minimum time slice is %u milliseconds.\n",
              aulSysInfo[QSV_MIN_SLICE-1] );

        printf( "Maximum time slice is %u milliseconds.\n",
              aulSysInfo[QSV_MAX_SLICE-1] );

        printf( "Memory page size is %u bytes.\n",
              aulSysInfo[QSV_PAGE_SIZE-1] );

        printf( "System major version is %u.\n",
              aulSysInfo[QSV_VERSION_MAJOR-1] );

        printf( "System minor version is %u.\n",
              aulSysInfo[QSV_VERSION_MINOR-1] );

        printf( "System version revision is %u.\n",
              aulSysInfo[QSV_VERSION_REVISION-1] );

        printf( "Total physical memory is %6.3f megabytes.\n",
              (float) aulSysInfo[QSV_TOTPHYSMEM-1] / 1e6 );

        printf( "Maximum private memory is %6.3f megabytes.\n",
              (float) aulSysInfo[QSV_MAXPRMEM-1] /1e6 );

        printf( "Maximum shared memory is %6.3f megabytes.\n",
              (float) aulSysInfo[QSV_MAXSHMEM-1] /1e6);

        printf( "Timer interval is %3.1f milliseconds.\n",
              (float) aulSysInfo[QSV_TIMER_INTERVAL-1] / 10.0 );

        }
    printf( "\n" );

    return rc;
    }
